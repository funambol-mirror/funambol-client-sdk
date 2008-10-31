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
#include "spds/FileData.h"

#include "client/FileSyncSource.h"
#include "base/globalsdef.h"

USE_NAMESPACE


FileSyncSource::FileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc) : SyncSource(name, sc) {
    dir  = NULL;
    fileNode = NULL;

    setDir(".");
}

FileSyncSource::~FileSyncSource() {
    if(dir) {
        delete [] dir;
        dir = NULL;
    }
}



void FileSyncSource::setDir(const char* p) {
    if (dir)
        delete [] dir;

    dir = (p) ? stringdup(p) : stringdup("\\");
}

const char* FileSyncSource::getDir() {
    return dir;
}



/////////////////////////////////////////////////////////////////////////////////////////


int FileSyncSource::beginSync() {
    allItems.items.clear();
    deletedItems.items.clear();
    newItems.items.clear();
    updatedItems.items.clear();


    //
    // Get file list.
    //
    int count;
    char** fileNames = readDir(dir, &count);
    LOG.info("The client number of files to sync are %i", count);

    //
    // Create array list with empty data from file names.
    //
    for (int i=0; i<count; i++) {
        if (fileNames[i]) {
            WCHAR* wname = toWideChar(fileNames[i]);
            SyncItem* s = new SyncItem(wname);
            allItems.items.add(*s);

            if (fileNode) {
                char completeName[512];
                sprintf(completeName, "%s/%s", dir, fileNames[i]);
                unsigned long fileModTime = getFileModTime(completeName);
                unsigned long serverModTime = getServerModTime(fileNames[i]);

                if (!serverModTime) {
                    // added file
                    newItems.items.add(*s);
                } else if (serverModTime < fileModTime) {
                    // updated file
                    updatedItems.items.add(*s);
                }
            }

            delete s;
            delete [] wname;
            delete [] fileNames[i];
        }
    }

    if (fileNode) {
        // iterate over all files stored on server (i.e. those with non-zero time stamp property)
        // and check which of these have been deleted locally
        //
        // TODO: currently impossible with the ManagementNode interface, have to guess file names (works
        // for RawFileSyncSource)

        for (int key = 0; key < 1000; key++) {
            char keystr[80];
            sprintf(keystr, "%d", key);

            if (getServerModTime(keystr)) {
                char completeName[512];
                sprintf(completeName, "%s/%s", dir, keystr);
                if (!getFileModTime(completeName)) {
                    // file no longer exists locally
                    WCHAR* wname = toWideChar(keystr);
                    SyncItem* s = new SyncItem(wname);
                    deletedItems.items.add(*s);
                    delete s;
                    delete [] wname;
                }
            }
        }
    }

    if (fileNames) {
        delete [] fileNames;
        fileNames = NULL;
    }
    return 0;
}

SyncItem* FileSyncSource::getFirst(ItemIteratorContainer& container, bool getData) {
    container.index = 0;
    if (container.index >= container.items.size()) {
        return NULL;
    }
    SyncItem* syncItem = (SyncItem*)container.items.get(container.index)->clone();

    //
    // Set data from file content, return syncItem (freed by API)
    //
    if (!getData || setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}

SyncItem* FileSyncSource::getNext(ItemIteratorContainer& container, bool getData) {
    container.index++;
    if (container.index >= container.items.size()) {
        return NULL;
    }
    SyncItem* syncItem = (SyncItem*)container.items.get(container.index)->clone();

    // Set data from file content, return syncItem (freed by API)
    if (!getData || setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}

unsigned long FileSyncSource::getServerModTime(const char* keystr) {
    unsigned long modtime = 0;
    if (fileNode) {
        char* timestr = fileNode->readPropertyValue(keystr);
        modtime = anchorToTimestamp(timestr);
        delete [] timestr;
    }
    return modtime;
}

void FileSyncSource::setItemStatus(const WCHAR* key, int status) {
    LOG.debug("item key: %" WCHAR_PRINTF ", status: %i", key, status);
}

int FileSyncSource::removeAllItems() {return 0; }

//////////////////////////////////////////////////////////////////////////////////////////


int FileSyncSource::addItem(SyncItem& item) {

    // format is the custom XML format understood by FileData
    int ret = STC_COMMAND_FAILED;
    FileData file;
    char* data = (char*)item.getData();
    size_t len = item.getDataSize();

    if (file.parse(data, len)) {
        setError(ERR_BAD_FILE_CONTENT, "Error parsing item from server");
        LOG.error("%s", getLastErrorMsg());
        report->setLastErrorCode(ERR_BAD_FILE_CONTENT);
        report->setLastErrorMsg(getLastErrorMsg());
        report->setState(SOURCE_ERROR);
        return STC_COMMAND_FAILED;
    }


    if (file.getSize() >= 0) {
        //
        // Save item on FS
        //
        char completeName[512];
        sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, file.getName());
        if (!saveFile(completeName, file.getBody(), file.getSize(), true)) {
            setErrorF(ERR_FILE_SYSTEM, "Error saving file %" WCHAR_PRINTF, file.getName());
            LOG.error("%s", getLastErrorMsg());
            report->setLastErrorCode(ERR_FILE_SYSTEM);
            report->setLastErrorMsg(getLastErrorMsg());
            report->setState(SOURCE_ERROR);
            return STC_COMMAND_FAILED;
        }
        ret = addedItem(item, file.getName());
        LOG.debug("Added item: %" WCHAR_PRINTF, file.getName());
    }
    return ret;
}

int FileSyncSource::addedItem(SyncItem& item, const WCHAR* key) {
    item.setKey(key);

    // remember this item so that endSync() can store its time stamp
    SyncItem smallitem;
    smallitem.setKey(key);
    allItems.items.add(smallitem);

    return STC_ITEM_ADDED;
}

int FileSyncSource::updateItem(SyncItem& item) {
    ////// TBD ////////
    return STC_COMMAND_FAILED;
    ///////////////////

    int ret = STC_COMMAND_FAILED;

    FileData file;
    char* data      = NULL;
    long h          = 0;
    WCHAR* encod  = NULL;
    int size = 0;
    int res = 0;
    size = item.getDataSize();
    data = new char[size + 1];
    memset(data, 0, size + 1);
    memcpy(data, item.getData(), size);
    res = file.parse((data));
    encod = (WCHAR*)file.getEnc();
    delete [] data;

    if (wcslen(encod) > 0) {
        item.setData(file.getBody(), file.getSize());
        //
        // Replace item on FS (res=h)
        //
    }

    if (h == 0) {
        ret = STC_OK;
        LOG.debug("updated item: %S", item.getKey());
    }
    return ret;
}

int FileSyncSource::deleteItem(SyncItem& item) {
    int ret = STC_COMMAND_FAILED;

    char completeName[512];
    sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item.getKey());
    if (
#ifdef WIN32
        !_unlink(completeName)
#else
        !unlink(completeName)
#endif
        ) {
        ret = STC_OK;
    }

    return ret;
}


int FileSyncSource::endSync() {
    if (fileNode) {
        SyncItem* item;

        // reset information about deleted items
        for (item = getFirst(deletedItems, false); item; item = getNext(deletedItems, false)) {
            char *tmp = toMultibyte(item->getKey());
            fileNode->setPropertyValue(tmp, "");
            delete [] tmp;
            delete item;
        }

        // update information about each file that currently exists on the server
        for (item = getFirst(allItems, false); item; item = getNext(allItems, false)) {
            char completeName[512];
            sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item->getKey());
            unsigned long modTime = getFileModTime(completeName);
            char anchor[30];
            timestampToAnchor(modTime, anchor);
            char *tmp = toMultibyte(item->getKey());
            fileNode->setPropertyValue(tmp, anchor);
            delete [] tmp;
            delete item;
        }
    }

    return 0;
}

void FileSyncSource::assign(FileSyncSource& s) {
    SyncSource::assign(s);
    setDir(getDir());
}

bool FileSyncSource::setItemData(SyncItem* syncItem) {

    size_t len;
    char* content;
    char fileName[512];

    //
    // Get file content.
    //
    sprintf(fileName, "%s/%" WCHAR_PRINTF, dir, syncItem->getKey());
    if (!readFile(fileName, &content, &len, true)) {
        setErrorF(ERR_FILE_SYSTEM, "Error opening the file '%s'", fileName);
        LOG.error("%s", getLastErrorMsg());
        report->setLastErrorCode(ERR_FILE_SYSTEM);
        report->setLastErrorMsg(getLastErrorMsg());
        report->setState(SOURCE_ERROR);
        return false;
    }

    //
    // Set data
    //
    if (content) {
        FileData file;
        file.setName(syncItem->getKey());
        file.setSize((int)len);
        file.setEnc(TEXT("base64"));
        file.setBody(content, (int)len);
        char* encContent = file.format();
        syncItem->setData(encContent, (int)strlen(encContent));
        delete [] encContent;
        encContent = NULL;
        //syncItem->setData(content, (long)len);
        delete [] content;
        content = NULL;
        return true;
    }
    else {
        setErrorF(ERR_BAD_FILE_CONTENT, "Error bad file content: '%s'", fileName);
        LOG.error("%s", getLastErrorMsg());
        report->setLastErrorCode(ERR_BAD_FILE_CONTENT);
        report->setLastErrorMsg(getLastErrorMsg());
        report->setState(SOURCE_ERROR);
        return false;
    }
}
