/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
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

#include "sapi/MediaSapiSyncSource.h"

BEGIN_FUNAMBOL_NAMESPACE

MediaSapiSyncSource::MediaSapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& rep, size_t incomingFilterDate, size_t outgoingFilterDate, const char* storageLocation, bool recursive_) 
        : FileSapiSyncSource(sc, rep, incomingFilterDate, outgoingFilterDate, storageLocation, recursive_) {

}


MediaSapiSyncSource::~MediaSapiSyncSource() {
}
   
StringBuffer MediaSapiSyncSource::changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) {    

    return SapiSyncSource::changeItem(syncItem, errCode, modificationDate);
}

SapiSyncItemInfo* MediaSapiSyncSource::twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array) {
    return SapiSyncSource::twinDetection(serverItemInfo, array);
}

UploadSapiSyncItem* MediaSapiSyncSource::getNextModItem(int* err) {
    return SapiSyncSource::getNextModItem(err);  
}

void MediaSapiSyncSource::validateLocalLists() {
    //
    // Only NEW items are supported, client to server.
    //
    if (updatedItemInfo && updatedItemInfo->size() > 0) {
        LOG.info("Updates from client to server are NOT supported (%d items skipped)", updatedItemInfo->size());
        updatedItemInfo->clear();
    }
    if (deletedItemInfo && deletedItemInfo->size() > 0) {
        LOG.info("Deletes from client to server are NOT supported (%d items skipped)", deletedItemInfo->size());
        deletedItemInfo->clear();
    }
}

void MediaSapiSyncSource::validateRemoteLists(ArrayList* newServerItems, 
                                              ArrayList* modServerItems, 
                                              ArrayList* delServerItems) {
    //
    // Only NEW and DEL items are supported, server to client.
    //
    if (modServerItems && modServerItems->size() > 0) {
        LOG.info("Updates from server to client are NOT supported (%d items skipped)", modServerItems->size());
        modServerItems->clear();
    }
}


END_FUNAMBOL_NAMESPACE


