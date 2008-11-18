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
    
    char* t = toMultibyte(name);
    StringBuffer pathName(dir);
    pathName += "/"; 
    pathName += t;
    delete [] t;
    return pathName;
    
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

//---------------------------------------------------------------------------------- Constructors

FileSyncSource::FileSyncSource(
                        const WCHAR* name,
                        AbstractSyncSourceConfig* sc)
    : CacheSyncSource(name, sc), dir(DEFAULT_SYNC_DIR) {
   
}

FileSyncSource::~FileSyncSource() { }

//-------------------------------------------------------------------------------- Public methods

Enumeration* FileSyncSource::getAllItemList() {
    
    //
    // Get the list of the files that are in the directory
    //
    ArrayList currentKeys;
    Enumeration* allKeys = NULL;        
    int count;
    char** fileNames = readDir((char*)dir.c_str(), &count);
    LOG.info("The client number of files to sync are %i", count);
    
    for (int i = 0; i < count; i++) {
        if (fileNames[i]) {
            StringBuffer s(fileNames[i]);            
            currentKeys.add(s);
        }
    }    

    // delete fileNames
    for (int i = 0; i < count; i++) {
        delete [] fileNames[i]; fileNames[i] = NULL; 
    }
    if (fileNames != NULL) {
        delete [] fileNames; fileNames = NULL;
    }    

    allKeys = new ArrayListEnumeration(currentKeys);
    
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
        ret = STC_ITEM_ADDED;
        LOG.debug("Added item: %" WCHAR_PRINTF, item.getKey());
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
    removeFileInDir(dir);
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
    
    if (!readFile(completeName, &fileContent, size, true)) {        
        LOG.error("Content of the file not read");
        return NULL;
    }

    // get the SyncSource mime type
    const char* mimeType = config->getType();

    if(!strcmp(mimeType, OMA_MIME_TYPE))
    {
        // the item content must be set as OMA file obj format
        FileData file;

        file.setName(fileName);
        file.setSize(*size);
        file.setBody(fileContent, *size);

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

    return itemContent;
}

END_NAMESPACE
