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

#include "base/Log.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "client/FileSyncItem.h"
#include "base/globalsdef.h"
#include "inputStream/FileInputStream.h"
#include "inputStream/FileDataInputStream.h"

USE_NAMESPACE


// Default constructor
FileSyncItem::FileSyncItem(const StringBuffer& path, const WCHAR* key, const bool isFileData) : SyncItem(key) {

    this->isFileData = isFileData;
    this->filePath   = path;

    // Create the right InputStream
    if (isFileData) {
        inputStream = new FileDataInputStream(path);
    } else {
        inputStream = new FileInputStream(path);
    }
}

FileSyncItem::FileSyncItem(const StringBuffer& path, const bool isFileData) : SyncItem() {

    this->isFileData = isFileData;
    this->filePath   = path;

    StringBuffer fileName = getFileNameFromPath(path);
    WCHAR* wfileName = toWideChar(fileName.c_str());
    setKey(wfileName);
    delete [] wfileName;

    // Create the right InputStream
    if (isFileData) {
        inputStream = new FileDataInputStream(path);
    } else {
        inputStream = new FileInputStream(path);
    }
}

FileSyncItem::~FileSyncItem() {}


long FileSyncItem::getDataSize() const {
    return inputStream->getTotalSize();
}


void* FileSyncItem::setData(const void* data, long size) {
    LOG.info("Warning: deprecated method FileSyncItem::setData() in mo more used");
    return NULL;
}

void* FileSyncItem::getData() const {
    LOG.info("Warning: deprecated method FileSyncItem::getData() in mo more used");
    return NULL;
}

int FileSyncItem::changeDataEncoding(const char* encoding, const char* encryption, const char* credentialInfo) {
    LOG.info("Warning: method FileSyncItem::changeDataEncoding is not implemented. The item is not trasferred");
    return ERR_UNSPECIFIED;
}


void FileSyncItem::setDataSize(long s) {
    LOG.info("Warning: deprecated method FileSyncItem::setDataSize() in mo more used");
}


ArrayElement* FileSyncItem::clone() {

    FileSyncItem* ret = new FileSyncItem(filePath, getKey(), isFileData);

    ret->setDataType        (getDataType());
    ret->setModificationTime(getModificationTime());
    ret->setState           (getState());
    ret->setSourceParent    (getSourceParent());
    ret->setTargetParent    (getTargetParent());

    return ret;
}
