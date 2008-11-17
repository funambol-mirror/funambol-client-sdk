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

#include "client/RawFileSyncSource.h"
#include "base/globalsdef.h"

USE_NAMESPACE


RawFileSyncSource::RawFileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc) : FileSyncSource(name, sc) {
}

int RawFileSyncSource::addItem(SyncItem& item) {
    char completeName[512];
    int key = 0;

    while(1) {
        sprintf(completeName, "%s/%d", dir, key);

        FILE *fh = fopen(completeName, "r");
        if (!fh) {
            if (!saveFile(completeName, (const char *)item.getData(), item.getDataSize(), true)) {
                setErrorF(ERR_FILE_SYSTEM, "Error saving file %s", completeName);
                LOG.error("%s", getLastErrorMsg());
                report->setLastErrorCode(ERR_FILE_SYSTEM);
                report->setLastErrorMsg(getLastErrorMsg());
                report->setState(SOURCE_ERROR);
                return STC_COMMAND_FAILED;
            } else {
                WCHAR keystr[80];
                swprintf(keystr, 80, TEXT("%d"), key);
                item.setKey(keystr);
                return addedItem(item, keystr);
            }
        } else {
            fclose(fh);
            key++;
        }
    }
}

int RawFileSyncSource::updateItem(SyncItem& item) {
    char completeName[512];
    sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item.getKey());
    if (!saveFile(completeName, (const char *)item.getData(), item.getDataSize(), true)) {
        setErrorF(ERR_FILE_SYSTEM, "Error saving file %s", completeName);
        LOG.error("%s", getLastErrorMsg());
        report->setLastErrorCode(ERR_FILE_SYSTEM);
        report->setLastErrorMsg(getLastErrorMsg());
        report->setState(SOURCE_ERROR);
        return STC_COMMAND_FAILED;
    } else {
        return STC_OK;
    }
}

bool RawFileSyncSource::setItemData(SyncItem* syncItem) {

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
        syncItem->setData(content, (long)len);
        WCHAR *tmp = toWideChar(getConfig().getType());
        syncItem->setDataType(tmp);
        delete [] tmp;
        delete [] content;
        content = NULL;
    }
    return true;
}
