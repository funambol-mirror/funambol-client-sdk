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

#include <e32base.h>
#include <e32cmn.h>
#include <e32des8.h>
#include <e32def.h>
#include <e32std.h>
#include <f32file.h> 
#include <bautils.h>
#include <unistd.h>

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "base/util/symbianUtils.h"
#include "base/util/stringUtils.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

wchar_t * wcsncpy (wchar_t *dst, const wchar_t *src, size_t count) {
    memcpy(dst, src, count * sizeof(wchar_t));
    return dst;
}

int towlower(int c) {
    wchar_t tmpStr[2];
    tmpStr[0] = (wchar_t)c;
    tmpStr[1] = 0;
    TBuf16<1> buf = (const TUint16*)tmpStr;
    buf.LowerCase();
    const TUint16* resStr = buf.Ptr();
    return (int)*resStr;
}

int towupper(int c)  {
    wchar_t tmpStr[2];
    tmpStr[1] = 0;
    tmpStr[0] = (wchar_t)c;
    TBuf16<1> buf = (const TUint16*)tmpStr;
    buf.UpperCase();
    const TUint16* resStr = buf.Ptr();
    return (int)*resStr;
}

static size_t estimateMaxSize(const char* format, PLATFORM_VA_LIST ap) {

    const char* p = format;
    size_t maxSize = strlen(format);
    while (*p) {
        if (*p == '%') {
            ++p;
            // We may have a precision or width here and we must skip it
            char number[16];
            int idx = 0;
            while ((*p) >= '0' && (*p) <= '9') {
                number[idx++] = *p;
                ++p;
            }
            number[idx] = (char)0;
            char type = *p;
            switch (type) {
                case 'd':
                case 'i':
                case 'u':
                {
                    maxSize += 10;
                    PLATFORM_VA_ARG(ap, int32_t);
                    break;
                }
                case 's':
                {
                    char* s = PLATFORM_VA_ARG(ap, char*);
                    int len = 0;
                    if (s) {
                        len = strlen(s);
                    } else {
                        return (size_t)-2;
                    }
                    int align = atoi(number);
                    maxSize += (len>align) ? len : align;
                    break;
                }
                case 'b':
                {
                    maxSize += 32 * 8 + 2;
                    PLATFORM_VA_ARG(ap, int32_t);
                    break;
                }
                case 'x':
                {
                    maxSize += 32 + 2;
                    PLATFORM_VA_ARG(ap, int32_t);
                    break;
                }
                case 'o':
                {
                    maxSize += 32 * 2 + 2;
                    PLATFORM_VA_ARG(ap, int32_t);
                    break;
                }
                case 'c':
                {
                    maxSize += 1;
                    PLATFORM_VA_ARG(ap, int32_t);
                    break;
                }
                case 'L':
                case 'l':
                {
                    if (*(p+1) == 's') {
                        // Symbian does not support %ls because the underlying
                        // FormatList does not support it.
                        // To avoid problems we do not print this string, as we
                        // do not have an easy way of doing it
                        return (size_t)-2;
                    } else {
                        maxSize += 20;
                        PLATFORM_VA_ARG(ap, int64_t);
                    }
                    break;
                }
                case '*': {
                    // Symbian does not support %*[s|l|...]
                    return (size_t)-2;
                }

                default:
                {
                    // Floating point cases...
                    maxSize += 50;
                }
            }
        }
        ++p;
    }
    return maxSize;
}

static char* translateFormat(const char* format) {
    char* ret     = new char[strlen(format) + 1];
    char* p       = ret;
    const char* q = format;

    while (*q) {
        if (*q == '%') {
            q++;
            *(p++) = '%';
            // We may have a precision or width here and we must just copy it
            while ((*q) >= '0' && (*q) <= '9') {
                *(p++) = *(q++);
            }
            char fmt = *q;
            switch (fmt) {
                case 'l':
                {
                    q++;
                    if (*q == 'l') {
                        *p = 'L';
                        q++;
                        p++;
                    }
                    *p = *q;
                    break;
                }
                default:
                {
                    *p = fmt;
                }
            }
        } else {
            *p = *q;
        }
        ++q;
        ++p;
    }
    *p = NULL;
    return ret;
}

size_t vsnprintf(char* s, size_t size, const char* format, PLATFORM_VA_LIST aq) {

    char* newFormat = translateFormat(format);
    TPtrC8 formatBuf((unsigned char*)newFormat);
    TInt error;
    RBuf8 formattedBuf;
    PLATFORM_VA_LIST aqCopy;
    size_t finalSize = (size_t)-1;

#ifdef va_copy
        PLATFORM_VA_COPY(aqCopy, aq);
#else
        aqCopy = aq;
#endif

    size_t estimatedSize = estimateMaxSize(format, aqCopy);

    // Handle errors
    if (estimatedSize == (size_t)-2) {
        // The string contains formats which are not supported
        // and we cannot print the string
        char* replaceMsg = "Symbian unsupported printf";
        size_t replaceMsgLen = (size_t)strlen(replaceMsg);
        if (replaceMsgLen < size) {
            memcpy(s, replaceMsg, replaceMsgLen);
            s[replaceMsgLen-1] = (char)0;
            finalSize = replaceMsgLen;
        } else {
            memcpy(s, replaceMsg, size);
            if (size > 1) {
                s[size-1] = (char)0;
                finalSize = size-1;
            } else {
                finalSize = 0;
            }
        }
        goto finally;
    }

    if (estimatedSize > size) {
        finalSize = estimatedSize;
    } else {
        TRAP(error, formattedBuf.CreateL(size));
        if (error == KErrNone) {
            TRAP(error, formattedBuf.FormatList(formatBuf, aq));
            if (error == KErrNone) {
                char* ptr = (char *) formattedBuf.Ptr();
                finalSize = formattedBuf.Length();
                if (finalSize < size) {
                    memcpy(s, ptr, finalSize);
                    s[finalSize] = (char)0;           // Symbian descriptors don't have the trailing null char
                } else {
                    // In this case we truncate. We signal this by returning -1
                    memcpy(s, ptr, size);
                    if (size) {
                        s[size-1] = (char)0;
                    }
                    finalSize = (size_t)-1;
                }
            }
        }
    }

finally:
    delete newFormat;
    formattedBuf.Close();
    return finalSize;
}


// TODO: convert to the specified encoding, assuming wc is UTF-8
char* toMultibyte(const WCHAR *wc, const char *encoding)
{
#ifdef USE_WCHAR
    size_t length = wcstombs(NULL, wc, 0) + 1;
    if(length == (size_t)-1) {
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
    if(length == (size_t)-1) {
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


size_t snwprintf(WCHAR *v, size_t size, const WCHAR* format, unsigned long value) {

    TPtrC16 formatBuf((const TUint16*)format);
    TInt error;
    RBuf16 formattedBuf;
    size_t finalSize = (size_t)-1;

    TRAP(error, formattedBuf.CreateL(size));
    if (error == KErrNone) {
        TRAP(error, formattedBuf.Format(formatBuf, value));
        if (error == KErrNone) {
            WCHAR* ptr = (WCHAR *) formattedBuf.Ptr();
            finalSize = formattedBuf.Length() * sizeof(WCHAR);
            if (finalSize < size) {
                memcpy(v, ptr, finalSize);
                v[finalSize] = 0;   // Symbian descriptors don't have the trailing null char
            } else {
                // In this case we truncate. We signal this by returning -1
                memcpy(v, ptr, size);
                v[size] = 0;   // Symbian descriptors don't have the trailing null char
                finalSize = (size_t)-1;
            }
        }
    }

    formattedBuf.Close();
    // We cannot format the string. Return -1.
    return finalSize;
}

bool saveFile(const char *filename, const char *buffer, size_t len, bool binary)
{
    FILE *f = fopen(filename, "w+");

    if(!f)
        return false;

    fwrite(buffer, sizeof(char) * len, 1, f);
    
    if (ferror(f)) {
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
    bool res = false;
    size_t read = 0;

    *len = 0;
    f = fopen(path, mode);
    if ( !f ) {
        goto done;
    }

    // Get file length
    fseek(f, 0, SEEK_END);
    msglen = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    msg = new char[msglen+1];
    if (!msg) {
        goto done;
    }

    while (read < msglen) {
        read += fread(msg + read, sizeof(char), 256, f);
        if (ferror(f)) {
            goto done;
        }
    }
    if (read != msglen) {
        goto done;
    }

    *len = msglen;
    msg[msglen] = 0;

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


WCHAR *wcschr(const WCHAR *ws, WCHAR wc) {

    if (!ws) {
        return NULL;
    }
    WCHAR* res = const_cast<WCHAR *>(ws);
    while(*res) {
        if (*res == wc) {
            return res;
        }
        res++;
    }
    return NULL;
}

WCHAR *wcsstr(WCHAR *ws1, const WCHAR *ws2) {
    LOG.error("***** wcsstr not implemented *****");
    return NULL;
}

WCHAR *wcstok(WCHAR *ws1, const WCHAR *ws2) {
    LOG.error("***** wcstok not implemented *****");
    return NULL;
}

WCHAR *wcsncat(WCHAR *ws1, const WCHAR *ws2, size_t n) {
    LOG.error("***** wcsncat not implemented *****");
    return NULL;
}

double wcstod(const WCHAR *nptr, WCHAR **endptr) {
    LOG.error("***** wcstod not implemented *****");
    return 0.0;
}

int _wtoi(const WCHAR *str) {
    LOG.error("***** _wtoi not implemented *****");
    return 0;
}

int wcscasecmp(const WCHAR* s1, const WCHAR* s2)
{
    int c1, c2;

    if (s1 == s2)
        return 0;

    do
    {
        c1 = towlower(*s1++);
        c2 = towlower(*s2++);
        if (c1 == L'\0')
            break;
    }
    while (c1 == c2);

    return c1 - c2;
}

bool removeFileInDir(const char* d, const char* fname) {
    char toFind    [512];
    StringBuffer path;
    bool ret = false;
    char** totalFiles = NULL;
    int numFiles = 0;

    if (fname) {
        sprintf(toFind, "%s/%s", d, fname);
        path = contextToPath(toFind);
        
        if (remove(path.c_str()) != 0) {
            LOG.error("Error deleting the %s file", path.c_str()); 
        } else {
            LOG.debug("File %s deleted succesfully", path.c_str());
            ret = true;
        }
    }
    else {        
        totalFiles = readDir((char*)d, &numFiles, false);
        if (totalFiles && numFiles > 0) {
            for (int i = 0; i < numFiles; i++) {
                sprintf(toFind, "%s/%s", d, totalFiles[i]);
                path = contextToPath(toFind);
                remove(path.c_str());            
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

bool fileExists(const char *aPath)
{
    // Transform: "/" -> "\"
    StringBuffer path = contextToPath(aPath);

    // Connect to the file server
    RFs fileSession;
    TInt err = fileSession.Connect();
    if (err) {
        LOG.error("Cannot connect to the file server (code %d)", err);
        return false;
    }
    CleanupClosePushL(fileSession);

    RBuf aFileName;
    aFileName.Assign(stringBufferToNewBuf(path));
    TBool res = BaflUtils::FileExists(fileSession, aFileName);

    aFileName.Close();
    CleanupStack::PopAndDestroy(&fileSession);

    return (res  ?  true  :  false);
}

int createFolder(const char *aPath) {
    TInt err = KErrNone;
    // Transform: "/" -> "\"
    StringBuffer path = contextToPath(aPath);

    // Trailing char MUST be "\"
    const char* chars = path.c_str();
    if (chars[strlen(chars)-1] != '\\') {
        path += "\\";
    }

    // Connect to the file server
    RFs fileSession;
    err = fileSession.Connect();
    if (err) {
        LOG.error("Cannot connect to the file server (code %d)", err);
        return err;
    }
    CleanupClosePushL(fileSession);

    // Make the dirs
    RBuf pathDes;
    pathDes.Assign(stringBufferToNewBuf(path));
    err = fileSession.MkDirAll(pathDes);

    // if folder already existed don't return error
    if (err == KErrAlreadyExists) {    
        err = KErrNone;
    }
    pathDes.Close();
    CleanupStack::PopAndDestroy(&fileSession);
        
    return err;
}

char** readDir(const char* name, int *count, bool onlyCount) {
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
        } else {
            *count = total;    
        }
        
        closedir(dir);
    }

    return entries;
}

bool removeDir(const char *aDirPath)
{
    TInt err = KErrNone;
    // Transform: "/" -> "\"
    StringBuffer path = contextToPath(aDirPath);

    // Trailing char MUST be "\"
    const char* chars = path.c_str();
    if (chars[strlen(chars)-1] != '\\') {
        path += "\\";
    }
    
    // Connect to the file server
    RFs fileSession;
    err = fileSession.Connect();
    if (err) {
        LOG.error("Cannot connect to the file server (code %d)", err);
        return false;
    }
    CleanupClosePushL(fileSession);

    // delete the dir
    RBuf pathDes;    
    pathDes.Assign(stringBufferToNewBuf(path));
    err = fileSession.RmDir(pathDes);
    pathDes.Close();
    CleanupStack::PopAndDestroy(&fileSession);

    if (err == KErrNone) {
        return true;
    } else {
        return false;
    }
}

END_NAMESPACE

