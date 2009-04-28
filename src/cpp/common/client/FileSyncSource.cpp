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
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "syncml/core/TagNames.h"
#include "base/util/ArrayListEnumeration.h"
#include "client/FileSyncSource.h"

BEGIN_NAMESPACE

#define OMA_MIME_TYPE "application/vnd.omads-file+xml"

//------------------------------------------------------------------------------ Static functions

static StringBuffer getCompleteName(const char *dir, const WCHAR *name) {
    
    StringBuffer fileName;
    fileName.convert(name);
    
    if (fileName.find(dir) == 0) {
        // Filename contains the path from the first char -> it's already the complete name
        return fileName;
    }
    else {
        StringBuffer pathName(dir);
        pathName += "/"; 
        pathName += fileName;
        return pathName;
    }
}


//Returns the relative path+name of a file under the 'dir' folder, given its full name.
StringBuffer getRelativeName(const StringBuffer& dir, const StringBuffer& fullName) {
    
    if (dir.empty()) {
        return fullName;
    }
    
    StringBuffer relativeName("");
    int start = dir.length() + 1;
    
    // Get the relative path (cuts the trailing 'dir')
    if (fullName.length() > start) {
        relativeName = fullName.substr(start, fullName.length() - start);
    }
    return relativeName;
}



static int saveFileContent(const char *name, const char *content, size_t size, bool isUpdate) {
    
    if (!isUpdate) { // it is an add
        if(fileExists(name)) {
            return STC_ALREADY_EXISTS;
        }
    }   
    if (!saveFile(name, content, size, true)) {
        return STC_COMMAND_FAILED;
    } 
    
    return STC_OK;
}

static int saveFileData(const char *dir, FileData& file, bool isUpdate) {
    return saveFileContent(
            getCompleteName(dir, file.getName()), file.getBody(), file.getSize(), isUpdate);

}

static int saveFileItem(const char *dir, SyncItem& item, bool isUpdate) {
    return saveFileContent(
            getCompleteName(dir, item.getKey()), 
            (const char *)item.getData(),
            item.getDataSize(),
            isUpdate);
}


/**
 * Removes all files from an arraylist of file names (absolute paths expected).
 * @return true if no error
 */
static bool removeAllFiles(ArrayList& files) {
    
    bool ret = true;
    
    for (int i=0; i<files.size(); i++) {
        StringBuffer* fullName = (StringBuffer*)files[i];
        if (fullName) {
            int pos = fullName->rfind("/");
            StringBuffer filePath = fullName->substr(0, pos);
            StringBuffer fileName = fullName->substr(pos+1, fullName->length() - pos);
            //LOG.debug("fullname = %s, path = %s, name = %s", fullName->c_str(), filePath.c_str(), fileName.c_str());
            
            if ( removeFileInDir(filePath.c_str(), fileName.c_str()) == false ) {
                LOG.error("Error removing file: '%s'", fullName->c_str());
                ret = false;
            }
        }
    }
    return ret;
}



//---------------------------------------------------------------------------------- Constructors



FileSyncSource::FileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc, 
                               const StringBuffer& aDir, KeyValueStore* cache)
                              : CacheSyncSource(name, sc, cache), 
                              dir(aDir), 
                              recursive(false) {
    
    // Cut the last "\" or "/"
    if (aDir.endsWith("\\") || aDir.endsWith("/")) {
        dir = aDir.substr(0, aDir.length()-1);
    }            
}

FileSyncSource::~FileSyncSource() { }


Enumeration* FileSyncSource::getAllItemList() {
    
    ArrayList filesFound;
    Enumeration* allKeys = NULL;        
    
    if (scanFolder(dir, filesFound) == false) {
        LOG.error("error reading folder: %s", dir.c_str());
    }    
    
    LOG.info("The Client number of files read is %i", filesFound.size());
    allKeys = new ArrayListEnumeration(filesFound);
    return allKeys;
}

/**
 * Adds an item on the file system in the set directory.
 */
int FileSyncSource::insertItem(SyncItem& item) {
    
    int ret = STC_COMMAND_FAILED;
    FileData file;

    // Try the OMA file data first
    if (file.parse(item.getData(),item.getDataSize()) == 0) {
        if (file.getSize() >= 0) {   
            ret = saveFileData(dir, file, false);
            if (ret == STC_OK) {            // Set the LUID with the local name 
                item.setKey(file.getName());
            }
        }        
    } else {
        // treat it as a raw file
        ret = saveFileItem(dir, item, false);
    }

    if (ret == STC_OK) {
        LOG.debug("Added item: %" WCHAR_PRINTF, item.getKey());
    }
    else if (ret == STC_ALREADY_EXISTS) {
        LOG.debug("Item not added (already exists): %" WCHAR_PRINTF, item.getKey());
    }
    else {
        report->setLastErrorCode(ERR_ITEM_ERROR);
        report->setLastErrorMsg(ERRMSG_ITEM_ERROR);
        report->setState(SOURCE_ERROR);
        LOG.debug("Error adding item: %" WCHAR_PRINTF, item.getKey());
    }
    return ret;
}

int FileSyncSource::modifyItem(SyncItem& item) {
    
    int ret = STC_COMMAND_FAILED;
    FileData file;
    
    if (file.parse(item.getData(),item.getDataSize()) == 0) {
        if (file.getSize() >= 0) {   
            ret = saveFileData(dir, file, true);
        }        
    } else {
         ret = saveFileItem(dir, item, true);    
    }

    if (ret == STC_OK) {
        LOG.debug("Updated item: %" WCHAR_PRINTF, item.getKey());
    }
    else {
        report->setLastErrorCode(ERR_ITEM_ERROR);
        report->setLastErrorMsg(ERRMSG_ITEM_ERROR);
        report->setState(SOURCE_ERROR);
        LOG.debug("Error updating item: %" WCHAR_PRINTF, item.getKey());
    }
    return ret;
}

int FileSyncSource::removeItem(SyncItem& item) {
    
    const char* filename = toMultibyte(item.getKey());
    removeFileInDir(dir, filename);
    if (filename) { delete [] filename; filename = NULL; }
    LOG.debug("Item deleted: %" WCHAR_PRINTF, item.getKey());
    
    return STC_OK;
}

int FileSyncSource::removeAllItems() {
    
    if (!recursive) {
        if (removeFileInDir(dir) == false) {
            return 1;
        }
    }
    else {
        ArrayList filesFound;
        if (scanFolder(dir, filesFound, false) == false) {
            return 1;
        }
        if (removeAllFiles(filesFound) == false) {
            return 1;
        }
    }
    
    return 0;
}

/**
* Get the content of an item given the key. It is used to populate
* the SyncItem before the engine uses it in the usual flow of the sync.
* It is used also by the itemHandler if needed 
* (i.e. in the cache implementation)
*
* @param key      the local key of the item
* @param size     OUT: the size of the content
*
* @return         the local item content. It's new allocated, it must be 
*                 deleted by the caller using delete []
*                 Returns NULL in case of error
*/
void* FileSyncSource::getItemContent(StringBuffer& key, size_t* size) {
    
    char* fileContent = NULL; 
    char* itemContent = NULL;
    *size = 0;
    WCHAR* fileName = toWideChar(key);
    StringBuffer completeName(getCompleteName(dir, fileName));
    
    StringBuffer relativeName(getRelativeName(dir, completeName));
    WCHAR* wRelativeName = toWideChar(relativeName.c_str());
    
    if (!readFile(completeName, &fileContent, size, true)) {        
        LOG.error("Content of the file not read: %s", completeName.c_str());
        return NULL;
    }

    // get the SyncSource mime type
    const char* mimeType = config->getType();

    if(!strcmp(mimeType, OMA_MIME_TYPE))
    {
        // the item content must be set as OMA file obj format
        FileData file;

        file.setName(wRelativeName);
        file.setSize(*size);
        file.setBody(fileContent, *size);
        
        // Sets the file creation time, if info available
        struct stat st;
        memset(&st, 0, sizeof(struct stat));
        if (stat(completeName, &st) >= 0) {
            StringBuffer tmp;
            tmp.sprintf("%i", st.st_mtime);
            WCHAR* time = toWideChar(tmp.c_str());
            
            file.setModified(time);
            delete [] time;
        }

        itemContent = file.format();
        *size = strlen(itemContent);

        delete [] fileContent; fileContent = NULL;
    }
    else
    {
        // fill the item content with raw file content
        itemContent = fileContent;
    }

    delete [] fileName; fileName = NULL;
    delete [] wRelativeName; wRelativeName = NULL;

    return itemContent;
}



// read recursively directory contents
bool FileSyncSource::scanFolder(const StringBuffer& fullPath, ArrayList& filesFound, bool applyFiltering)
{
    int count = 0;
    struct stat st;
    StringBuffer fullName;
    
    // Remove the trailing "/" or "\" if exists
    StringBuffer dirPath(fullPath);
    if (fullPath.endsWith("\\") || fullPath.endsWith("/")) {
        dirPath = fullPath.substr(0, fullPath.length()-1);
    }
    else if (fullPath.empty()) {
        dirPath = dir;
    }
    
    char** fileNames = readDir(dirPath.c_str(), &count, false);
    if (fileNames == NULL) {
        //LOG.debug("Directory '%s' has no items", dirPath.c_str());
        return true;
    }

    for (int i=0; i<count; i++) {
        if (fileNames[i]) {
            memset(&st, 0, sizeof(struct stat));
            
            fullName.sprintf("%s/%s", dirPath.c_str(), fileNames[i]);
            //LOG.debug("fullName = %s", fullName.c_str());
            
            if (stat(fullName, &st) < 0) {
                LOG.error("can't stat file '%s' [%d]", fullName.c_str(), errno);
                continue;
            }
            
            // Filtering on outgoing items
            if (applyFiltering) {
                if (filterOutgoingItem(fullName, st)) {
                    //LOG.debug("Skipping item '%s'", fullName.c_str());
                    continue;
                }
            }

            // Recurse into subfolders.
            if (recursive && S_ISDIR(st.st_mode)) {
                if (scanFolder(fullName, filesFound) == false) {
                    LOG.error("Error reading '%s' folder", fullName.c_str());
                }
            }
            else {
                //
                // It's a file -> add it (key is its full path + name)
                //
                filesFound.add(fullName);
            }
        }
    }
    
    // delete fileNames
    for (int i = 0; i < count; i++) {
        delete [] fileNames[i]; fileNames[i] = NULL; 
    }
    if (fileNames != NULL) {
        delete [] fileNames; fileNames = NULL;
    }
    
    return true;
}


bool FileSyncSource::filterOutgoingItem(const StringBuffer& fullName, struct stat& st) 
{
    // No filtering.
    return false; 
}


bool FileSyncSource::checkFileExtension(const StringBuffer& fileName, const StringBuffer& extension)
{
    unsigned long pos = fileName.rfind(".");
    
    if (pos == StringBuffer::npos) {
        return false;
    }
    if (pos < fileName.length()) {
        pos += 1;
        StringBuffer ext = fileName.substr(pos, fileName.length() - pos);
        if (ext == extension) {
            //LOG.debug("extension is '%s'", ext.c_str());
            return true;
        }
    }
    return false;
}



END_NAMESPACE
