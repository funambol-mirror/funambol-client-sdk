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

#ifndef MEDIA_SAPI_SYNC_SOURCE
#define MEDIA_SAPI_SYNC_SOURCE

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncSourceConfig.h"
#include "sapi/SapiSyncItemInfo.h"
#include "sapi/SapiSyncSource.h"
#include "sapi/FileSapiSyncSource.h"
#include "sapi/SapiContentTypes.h"
#include "base/util/KeyValueStore.h"
#include "spds/SyncSourceReport.h"
#include "base/util/PropertyFile.h"
#include "spds/spdsutils.h"

BEGIN_FUNAMBOL_NAMESPACE

/**
 * The super class that represents the SyncSource used for the SapiSyncManager
 */
class MediaSapiSyncSource : public FileSapiSyncSource
{
  

public:

    /**
     * Constructs the SapiSyncSource.
     * 
     * @param sc    the syncSource's configuration
     * @param report the syncSource's report
     * @param incomingFilterDate  the filter by date for incoming items (downloads)
     *                            timestamp value (unix time), if 0 means the filter is disabled
     * @param outgoingFilterDate  the filter by date for outcoming items (uploads)
     *                            timestamp value (unix time), if 0 means the filter is disabled
     * @param cacheLocation  the path to store cache/resume/mappings tables. By default this is
     *                       null and uses the one from the PlatformAdapter. If not null, it tries
     *                       to create it and if no success, it uses the one from PlatformAdapter
     */
    MediaSapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& report, 
                                              size_t incomingFilterDate, size_t outgoingFilterDate, 
                                              const char* storageLocation = NULL);
                       
    virtual ~MediaSapiSyncSource();
     
    /**
     * Default implementation to update item. By default it is not implemented.
     * @param the DownloadSapiSyncItem containing the Luid of the item to change.
     * @return empty string ("") if it is not implemented of if there is an error.
     *         the Luid of the updated item.
     */
    virtual StringBuffer changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate);

    virtual SapiSyncItemInfo* twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array = NULL);
    
    virtual UploadSapiSyncItem* getNextModItem(int* err);
    
    /**
     * Validates the lists of NEW/MOD/DEL items from client to server.
     * Called by SapiSyncManager before upload starts, to meet the behavior of this source.
     * This implementation clears the MOD and DEL lists, since only NEW items are
     * supported (derived classes may redefine this behavior).
     */
    virtual void validateLocalLists();

    /**
     * Validates the lists of NEW/MOD/DEL items from server to client.
     * Called by SapiSyncManager before download starts, to meet the behavior of this source.
     * This implementation clears the MOD lists, since only NEW and DEL items are
     * supported (derived classes may redefine this behavior).
     * @param newServerItems the array of new server items
     * @param modServerItems the array of mod server items
     * @param delServerItems the array of del server items
     */
    virtual void validateRemoteLists(ArrayList* newServerItems, ArrayList* modServerItems, ArrayList* delServerItems);
 
    /**
     * Overrides FileSapiSyncSource::remoteRenameChecks().
     * Calls SapiSyncSource::remoteRenameChecks(), having the same size is enough
     * to decide an updated item is a rename, for media items.
     */
    virtual void remoteRenameChecks(ArrayList& modServerItems) {
        SapiSyncSource::remoteRenameChecks(modServerItems);
    }

};

END_FUNAMBOL_NAMESPACE

#endif 

