/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "base/Log.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


bool saveFile(const char *filename, const char *buffer, size_t len, bool binary)
{
    FILE *f = fopen(filename, "w");

    if(!f)
        return false;

    if (fwrite(buffer, sizeof(char), len, f) != len) {
        fclose(f);
        return false;
    }
    fclose(f);

    return true;
}

bool readFile(const char* path, char **message, size_t *len, bool binary)
{
    FILE *f = NULL;
    size_t msglen=0;
    char *msg=0;
    const char *mode = binary ? "rb" : "r" ;
    struct stat buf;
    bool res = false;

    f = fopen(path, mode);
    if ( !f ) {
        goto done;
    }
    if ( fstat(fileno(f), &buf) ) {
        goto done;
    }

    msglen = buf.st_size;
    msg = new char[msglen+1];
    msg[msglen] = 0;

    *len=fread(msg, sizeof(char), msglen, f);
    if ( ferror(f) || *len < msglen ){
        goto done;
    }

    // Set return parameters
    *message=msg;
    msg = 0;
    res = true;

  done:
    if (f) {
        fclose(f);
    }
    if (msg) {
        delete [] msg;
    }

    return res;
}


char** readDir(char* name, int *count, bool onlyCount) {
    char **entries = NULL;
    *count = 0;

    // count entries
    int total = 0;
    DIR *dir = opendir(name);
    if (dir) {
        struct dirent *entry = readdir(dir);
        while (entry) {
            if (strcmp(entry->d_name, ".") &&
                strcmp(entry->d_name, "..")) {
                total++;
            }
            entry = readdir(dir);
        }

        if (!onlyCount && total) {
            entries = new char *[total];

            rewinddir(dir);
            entry = readdir(dir);
            while (entry && *count < total) {
                if (strcmp(entry->d_name, ".") &&
                    strcmp(entry->d_name, "..")) {
                    entries[*count] = stringdup(entry->d_name);
                    ++*count;
                }
                entry = readdir(dir);
            }
        }
        closedir(dir);
    }

    return entries;
}

unsigned long getFileModTime(const char* name)
{
    struct stat buf;

    return stat(name, &buf) ? 0 :
        buf.st_ctime > buf.st_mtime ? buf.st_ctime :
        buf.st_mtime;
}

bool removeFileInDir(const char* d, const char* fname) {
       
    char toFind    [512];       
    bool ret = false;
    char** totalFiles = NULL;
    int numFiles = 0;

    if (fname) {
        sprintf(toFind, "%s/%s", d, fname);    
        if (remove(toFind) != 0) {
            LOG.error("Error deleting the %s file", toFind); ret = true;
        } else {
            LOG.debug("File %s deleted succesfully", toFind);
        }
    }
    else {        
        totalFiles = readDir((char*)d, &numFiles, false);
        if (totalFiles && numFiles > 0) {
            for (int i = 0; i < numFiles; i++) {
                sprintf(toFind, "%s/%s", d, totalFiles[i]);
                remove(toFind);            
            }
        }
        ret = true;
    }
    if (totalFiles) {
        for (int i = 0; i < numFiles; i++) {
            delete [] totalFiles[i]; 
        }
        delete [] totalFiles; totalFiles = NULL;
    }

    return ret;
}

/**
 * Creates a folder. TODO: move it to SDK utils
 * @return 0 on success, -1 otherwise.
 */
int createFolder(const char *path) {
    DIR* d = opendir(path);
    int res = 0;
    if (!d) {
        res = mkdir(path, 0777);
    } else {
        closedir(d);        
    }
    return res;
}

StringBuffer getHomeFolder() {
    StringBuffer home(getenv("HOME"));
    if( home.null() ) {
        LOG.error("Home directory not defined.");
    }
    return home;
}

StringBuffer getCacheDirectory() {
    
    StringBuffer ret(getenv("HOME"));
    ret.append("/");
    ret.append(CACHE_REP);
    
    DIR* d = opendir(ret);
    if (!d) {
        mkdir(ret, 0777);
    } else {
        closedir(d);
    }  
    return ret;
}
    
// TODO: convert to the specified encoding, assuming wc is UTF-8
char* toMultibyte(const WCHAR *wc, const char *encoding)
{
#ifdef USE_WCHAR
    size_t length = wcstombs(NULL, wc, 0) + 1;
    if(length == -1) {
        LOG.error("toMultibyte: invalid string.");
        return strdup("");
    }
    char* ret = new char[length];
    wcstombs(ret, wc, length);

    return ret;
#else
    return stringdup(wc);
#endif
}

// TODO: convert to UTF-8 from the specified encoding
WCHAR* toWideChar(const char *mb, const char *encoding)
{
#ifdef USE_WCHAR
    size_t length = mbstowcs(NULL, mb, 0) + 1;
    if(length == -1) {
        LOG.error("toWideChar: invalid string.");
        return wstrdup(TEXT(""));
    }
    WCHAR* ret = new WCHAR[length];
    mbstowcs(ret, mb, length);

    return ret;
#else
    return stringdup(mb);
#endif
}

#ifdef USE_WCHAR
// FIXME-----------------------------------------------------------------------
//Overloading thread-safe version of wcstok (defined in Linux) to make it
//work like the non-thread safe version available on Windows (same as strtok).
// FIXME: adjust VOCL code to NOT use this but String::split and remove it!!
//-----------------------------------------------------------------------------
WCHAR *wcstok(WCHAR *s, const WCHAR *delim)
{
    static WCHAR *state = 0;
    return ::wcstok(s, delim, &state);
}
#endif

// Implemented using mkstemp() with a template hard-coded
// to /tmp. Because the API of mkTempFileName() cannot return
// an open file, the file has to be closed. This exposes
// the possibility for an attacker to replace the file
// before it is opened again if /tmp has incorrect permissions.
char *mkTempFileName(const char *name)
{
    char *filename = new char[strlen("/tmp/") + strlen(name) + strlen(".XXXXXX") + 1];
    int fd;

    sprintf(filename, "/tmp/%s.XXXXXX", name );
    fd = mkstemp(filename);
    if (fd == -1) {
        delete [] filename;
        return NULL;
    } else {
        close(fd);
        return filename;
    }
}

END_NAMESPACE

