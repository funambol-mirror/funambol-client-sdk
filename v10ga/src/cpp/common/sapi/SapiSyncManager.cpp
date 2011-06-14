/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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

#include "sapi/SapiSyncManager.h"
#include "event/FireEvent.h"
#include "spds/spdsutils.h"
#include "spdm/constants.h"
#include "base/util/WString.h"
#include "http/URL.h"

USE_NAMESPACE

/// Returns true if passed syncMode is download enabled
static bool isDownloadEnabled(const SyncMode syncMode) {
    
    if (syncMode == SYNC_TWO_WAY || 
        syncMode == SYNC_ONE_WAY_FROM_SERVER ||
        syncMode == SYNC_REFRESH_FROM_SERVER) {
        return true;
    }
    return false;
}

/// Returns true if passed syncMode is upload enabled
static bool isUploadEnabled(const SyncMode syncMode) {
    
    if (syncMode == SYNC_TWO_WAY || 
        syncMode == SYNC_ONE_WAY_FROM_CLIENT ||
        syncMode == SYNC_REFRESH_FROM_CLIENT) {
        return true;
    }
    return false;
}


SapiSyncManager::SapiSyncManager(SapiSyncSource& s, AbstractSyncConfig& c) : 
                                 source(s), config(c), report(s.getReport()),
                                 syncMode(SYNC_TWO_WAY), clientFilterNumber(-1), 
                                 serverFilterNumber(-1), isSyncingItemChanges(false),
                                 downloadTimestamp(0) {

    // just for easy use in all methods.
    SyncSourceConfig& ssconfig = source.getConfig();
    sourceName = ssconfig.getName();
    sourceURI  = ssconfig.getURI();
    
    URL url(config.getSyncURL());
    StringBuffer host = url.getHostURL();
    offsetClientServer = 0;
    sapiMediaRequestManager = new SapiMediaRequestManager(host, 
                                                          getMediaSourceType(), 
                                                          config.getUserAgent(), 
                                                          config.getUsername(),
                                                          config.getPassword());
    // set http params, read from config
    sapiMediaRequestManager->setRequestTimeout   (config.getSapiRequestTimeout());
    sapiMediaRequestManager->setResponseTimeout  (config.getSapiResponseTimeout());
    sapiMediaRequestManager->setUploadChunkSize  (config.getSapiUploadChunkSize());
    sapiMediaRequestManager->setDownloadChunkSize(config.getSapiDownloadChunkSize());
}


SapiSyncManager::~SapiSyncManager() {

    delete sapiMediaRequestManager;
}


int SapiSyncManager::beginSync() {
    int ret;
    ESapiMediaRequestStatus err = ESMRSuccess; 
    StringBuffer lastSyncUrl;
    unsigned long serverLast = 0;
    time_t serverTime; 
    time_t clientTime;

    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally; 
    }
    
    syncMode = syncModeCode(source.getConfig().getSync());
    LOG.info("Begin synchronization of %s source (%s)", sourceName.c_str(), source.getConfig().getSync());
    
    fireSyncEvent(NULL, SYNC_BEGIN);
    report.setState(SOURCE_ACTIVE);

    // Save the begin sync time: now (optionally used by clients)
    source.getConfig().setBeginSyncTime((long)time(NULL));

    //
    // LOGIN
    //
    err = sapiMediaRequestManager->login(config.getDevID(), &serverTime);
    if (err != ESMRSuccess) {
        setSyncError(ESSMSapiError, err);
        goto finally;
    }
    clientTime = time(NULL);
    offsetClientServer = clientTime - serverTime;

    // Save these filters since they are used later in processing.
    clientFilterNumber = readClientFilterNumber();
    serverFilterNumber = readServerFilterNumber();

    //
    // Defines if we are syncing ALL items or CHANGES since a defined date
    //
    isSyncingItemChanges = defineSyncBehavior();

    LOG.info("Server url = %s", config.getSyncURL());

    // Check if destination Server / username have changed!
    // if yes: sync ALL (as it was a 1st sync) and clear maps/cache
    checkLastUserAndServer();


    //
    // GET CLIENT ITEMS (metadata)
    // ---------------------------
    // The sync behavior is passed to the source, to define the local lists to populate
    ret = source.beginSync(isSyncingItemChanges, config);
    if (ret) {
        setSyncError(ESSMBeginSyncError, ret);
        goto finally;
    }
    
    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally; 
    }

    fireSyncEvent(NULL, SEND_INITIALIZATION);

    //
    // GET SERVER ITEMS (metadata)
    // ---------------------------
    if (isSyncingItemChanges) {
        
        // Server changes are not necessary if download disabled (optimization)
        if (isDownloadEnabled(syncMode)) {
            // if here, serverLast is > 0
            serverLast = readServerLastTimestamp();
            // Filter by number is stronger: if it's set, don't set a filter by date!
            StringBuffer date("");
            if (serverFilterNumber < 0) {
                date = unixTimeToString(serverLast, true);
            }
            
            // GET CHANGES (no need to get server changes if one-way-from-client)
            if (getServerChanges(date)) {
                goto finally;
            }
        }
    }
    else {
        // GET ALL (always need to get all the server items, i.e. for twins)
        if (getAllServerItems("")) {
            goto finally;
        }
    }


    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally; 
    }
    
    //
    // PREPARE CLIENT/SERVER ITEMS FOR SYNC
    // ------------------------------------
    //
    prepareItems();

finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::getAllServerItems(const StringBuffer& fromDate) {

    int getAllStatus = -1;

    // Get all items info: paging requests
    int limit  = SAPI_PAGING_LIMIT;
    int offset = 0;
    LOG.debug("getting all items info from Server (paging = %d)", limit);
    while (1) {
        ArrayList items;
        LOG.debug("get items info from Server in range: [%d - %d]", offset, offset + limit);
        
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        
        ESapiMediaRequestStatus err = sapiMediaRequestManager->getAllItems(items, &downloadTimestamp, limit, offset);
        if (err == ESMRNetworkError) {
            err = retryGetAllItems(items, limit, offset);
        } else {
            // SAPI session maybe expired during download operation and
            // we get a HTTP 401 code ("access denied")
            if (err == ESMRAccessDenied) {
                // try a new login to get a new session id: if we
                // get an error interrupt the operation setting 
                // the item status
                LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
                err = sapiMediaRequestManager->login(config.getDevID());
                if (err == ESMRSuccess) {
                    LOG.debug("%s: retrying get all items info", __FUNCTION__);
                    err = sapiMediaRequestManager->getAllItems(items, &downloadTimestamp, limit, offset);
                    if (err == ESMRNetworkError) {
                        err = retryGetAllItems(items, limit, offset);
                    }
                }
            }
        }

        if (err != ESMRSuccess) {
            setSyncError(ESSMSapiError, err);
            goto finally;
        }
        
        allServerItems.add(&items);
        
        if (items.size() < limit) {
            break;      // means we reached the end on the server, no need to continue
        }
        offset += items.size();
    }
    getAllStatus = 0;   // success

finally:
    LOG.debug("%d items info received from server (ALL)", allServerItems.size());
    if (getAllStatus != 0) {
        getAllStatus = report.getLastErrorCode();
    }
    return getAllStatus;
}


int SapiSyncManager::getServerChanges(const StringBuffer& fromDate) 
{
    int getChangesStatus = -1;
    LOG.debug("get changes from Server (from date %s)", fromDate.c_str());

    // Get the lists of item GUIDs (only the keys)
    ArrayList newIDs, modIDs, delIDs;
    ESapiMediaRequestStatus err = sapiMediaRequestManager->getItemsChanges(newIDs, modIDs, delIDs, fromDate, &downloadTimestamp);
    
    if (err == ESMRNetworkError) {
        err = retryGetItemsChanges(newIDs, modIDs, delIDs, fromDate);
    } else {
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying resume get items info", __FUNCTION__);
                err = sapiMediaRequestManager->getItemsChanges(newIDs, modIDs, delIDs, fromDate, &downloadTimestamp);
                if (err == ESMRNetworkError) {
                    err = retryGetItemsChanges(newIDs, modIDs, delIDs, fromDate);
                }
            }
        }
    }

    if (err != ESMRSuccess) {
        setSyncError(ESSMSapiError, err);
        goto finally;
    }

    // Fill the NEW/MOD/DEL server lists of itemInfo
    // Paging requests: Server changes may be a lot, the getItemsFromId request can't be too long.
    if (newIDs.size() > 0) {
        LOG.info("Reading %d metadata of NEW items on the Server", newIDs.size());
        if (getServerItemsFromIds(newServerItems, newIDs)) {
            goto finally;
        }
    }

    if (modIDs.size() > 0) {
        LOG.info("Reading %d metadata of MOD items on the Server", modIDs.size());
        if (getServerItemsFromIds(modServerItems, modIDs)) {
            goto finally;
        }
    }

    LOG.debug("%d DEL items IDs received from Server", delIDs.size());
    for (int i=0; i<delIDs.size(); i++) {
        StringBuffer* guid = (StringBuffer*)delIDs.get(i);
        if (guid && !guid->empty()) {
            SapiSyncItemInfo info(guid->c_str(), NULL);   // empty SyncItemInfo, with only the GUID
            delServerItems.add(info);
        }
    }
    getChangesStatus = 0;   // success

finally:
    LOG.debug("%d new, %d updated, %d deleted items info received from server", 
              newServerItems.size(), modServerItems.size(), delServerItems.size());
    if (getChangesStatus != 0) {
        getChangesStatus = report.getLastErrorCode();
    }
    return getChangesStatus;
}


int SapiSyncManager::getServerItemsFromIds(ArrayList& items, const ArrayList& itemsIDs) {

    int offset = 0;
    int limit = SAPI_PAGING_LIMIT_IDS;

    while (1)
    {
        // copy N IDs into the pagedIds array (from offset to limit)
        ArrayList pagedIds;
        for (int i = offset; i < offset+limit; i++) {
            ArrayElement* e = itemsIDs.get(i);
            if (!e) { break; }
            pagedIds.add(*e);
        }

        if (pagedIds.size() == 0) {
            break;  // means we reached the end on the server, no need to continue
        }
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            goto finally; 
        }

        LOG.debug("get %d items from Server (paging offset=%d, limit=%d)", pagedIds.size(), offset, limit);
        ArrayList pagedServerItems;
        ESapiMediaRequestStatus err = sapiMediaRequestManager->getItemsFromId(pagedServerItems, pagedIds);
        if (err == ESMRNetworkError) {
            err = retryGetItemsFromId(pagedServerItems, pagedIds);
        } else {
            // SAPI session maybe expired during download operation and
            // we get a HTTP 401 code ("access denied")
            if (err == ESMRAccessDenied) {
                // try a new login to get a new session id: if we
                // get an error interrupt the operation setting the item status
                LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
                err = sapiMediaRequestManager->login(config.getDevID());
                if (err == ESMRSuccess) {
                    LOG.debug("%s: retrying resume get items info", __FUNCTION__);
                    err = sapiMediaRequestManager->getItemsFromId(pagedServerItems, pagedIds);
                
                    if (err == ESMRNetworkError) {
                        err = retryGetItemsFromId(pagedServerItems, pagedIds);
                    }
                }
            }
        }
        if (err != ESMRSuccess) {
            setSyncError(ESSMSapiError, err);
            goto finally;
        }

        // Append the N items to the returned items list
        items.add(&pagedServerItems);
        offset += pagedServerItems.size();
    }

finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::resolveConflicts() {
    return source.resolveConflicts(&modServerItems, &delServerItems, config, offsetClientServer);
}


int SapiSyncManager::prepareItems() {

    LOG.info("Preparing lists of items for synchronization...");
    int totalServerItems = 0;
    int totalClientItems = 0;

    if (isSyncingItemChanges) {
        //
        // Analyze NEW/MOD/DEL items lists, to avoid syncing items twice
        // Includes twin detections, conflicts resolutions and filterings.
        //
        if (prepareChangedItemsLists()) { 
            goto finally; 
        }
        
        // the client/server items lists have been validated by the source 
        // (see validateLocalLists, validateRemoteLists)
        totalServerItems = newServerItems.size() + 
                           modServerItems.size() + 
                           delServerItems.size();

        totalClientItems = getClientItemsNumber("NEW") + 
                           getClientItemsNumber("MOD") + 
                           getClientItemsNumber("DEL");

        LOG.info("%d Client items ready to upload:   [NEW,MOD,DEL] = [%d,%d,%d]", totalClientItems,
                 source.getItemsNumber("NEW"), source.getItemsNumber("MOD"), source.getItemsNumber("DEL"));        
        LOG.info("%d Server items ready to download: [NEW,MOD,DEL] = [%d,%d,%d]", totalServerItems, 
                 newServerItems.size(), modServerItems.size(), delServerItems.size());
    }
    else {
        //
        // Analyze ALL items lists, to avoid syncing items twice
        //
        if (prepareAllItemsLists()) { 
            goto finally; 
        }

        totalServerItems = allServerItems.size();
        totalClientItems = getClientItemsNumber("ALL");

        // FILTERING BY NUMBER
        // If the filter is set, keep only N most recent items from ALL
        if ((serverFilterNumber >= 0) && (serverFilterNumber < totalServerItems)) {
            totalServerItems = serverFilterNumber;
        }
        if ((clientFilterNumber >= 0) && (clientFilterNumber < totalClientItems)) {
            totalClientItems = clientFilterNumber;
        }

        LOG.info("%d Client items ready to upload",   totalClientItems);
        LOG.info("%d Server items ready to download", totalServerItems);
    }


    // Fire total items number to Client's UI.
    fireSyncSourceEvent(sourceURI.c_str(), 
                        sourceName.c_str(), 
                        syncMode,
                        totalClientItems, 
                        SYNC_SOURCE_TOTAL_CLIENT_ITEMS);

    fireSyncSourceEvent(sourceURI.c_str(), 
                        sourceName.c_str(), 
                        syncMode,
                        totalServerItems, 
                        SYNC_SOURCE_TOTAL_SERVER_ITEMS);

finally:
    return report.getLastErrorCode();
}



int SapiSyncManager::prepareAllItemsLists() {

    //
    // Check the local items list (ALL)
    //
    ArrayListEnumeration* allClientItems = source.getItemsList("ALL");
    if (allClientItems) {
        for (int i=0; i<allClientItems->size(); i++) {
            if (config.isToAbort()) {
                setSyncError(ESSMCanceled);
                goto finally; 
            }
            
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)allClientItems->get(i);
            if (!itemInfo) continue;

            // Filter by the extension allowed. If not the item isn't sent out
            // Note: this filter is the only one done at the beginning, to avoid processing
            //       many items that are not supported at all by this source (may take time)
            if (source.filterByExtension(*itemInfo)) {
                allClientItems->removeElementAt(i);     // discard it
                i--;
                continue;
            }

            // Exclude items already synced: check LUID in maps (btw it's an empty map at 1st sync)
            if (findLuidInMappings(itemInfo->getLuid().c_str())) {
                LOG.debug("skip local item %s (already synchronized)", itemInfo->getName().c_str());
                allClientItems->removeElementAt(i);
                i--;
                if (clientFilterNumber > 0) {
                    clientFilterNumber --;  // One item less to upload!
                }
                continue;
            }
        }
    }

    //
    // Check the remote items list (ALL)
    //
    for (int i=0; i<allServerItems.size(); i++) {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            goto finally; 
        }

        // Optimization to speed up:
        // if filtering by number, just stop after N items are accepted
        if ((serverFilterNumber >= 0) && (i > serverFilterNumber)) {
            break;
        }

        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)allServerItems.get(i);
        if (!itemInfo) continue;

        // 1. Exclude items already synced: check GUID in maps (btw it's an empty map at 1st sync)
        if (findGuidInMappings(itemInfo->getGuid().c_str())) {
            LOG.debug("skip server item %s (already synchronized)", itemInfo->getName().c_str());
            allServerItems.removeElementAt(i);
            i--;
            if (serverFilterNumber > 0) {
                serverFilterNumber --;  // One item less to download!
            }
            continue;
        }

        // 2. Exclude items existing locally *** TWIN DETECTION ***
        //    NOTE: if found, the local twin will be removed here
        SapiSyncItemInfo* localTwin = source.twinDetection(*itemInfo);
        if (localTwin) {
            StringBuffer luid = localTwin->getLuid();
            LOG.debug("skip item %s (twin detected)", itemInfo->getName().c_str());

            // add entry in the mappings/cache, for future syncs!
            source.getMappings().setPropertyValue          (itemInfo->getGuid().c_str(), luid.c_str());
            source.getServerDateMappings().setPropertyValue(itemInfo->getGuid().c_str(), StringBuffer().sprintf("%li", (long)itemInfo->getModificationDate()));
            source.getCache().setPropertyValue             (luid.c_str(),                StringBuffer().sprintf("%li", (long)localTwin->getModificationDate()));

            delete localTwin;
            allServerItems.removeElementAt(i);
            i--;
            if (clientFilterNumber > 0) {
                clientFilterNumber --;  // One item less to upload!
            }
            if (serverFilterNumber > 0) {
                serverFilterNumber --;  // One item less to download!
            }            
            continue;
        }
    }
    
    //
    // Filter outgoing & incoming items (ALL)
    //
    if (filterOutgoingItems("ALL")) { goto finally; }
    if (filterIncomingItems("ALL")) { goto finally; }

    
finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::prepareChangedItemsLists() {
    
    ArrayListEnumeration* modClientItems = NULL;
    
    // Filter by the extension allowed. If not the item isn't sent out
    // Note: this filter is the only one done at the beginning, to avoid processing
    //       many items that are not supported at all by this source (may take time)
    ArrayListEnumeration* newClientItems = source.getItemsList("NEW");
    if (newClientItems) {
        for (int i=0; i<newClientItems->size(); i++) {
            if (config.isToAbort()) {
                setSyncError(ESSMCanceled);
                goto finally; 
            }
            
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)newClientItems->get(i);
            if (!itemInfo) continue;

            if (source.filterByExtension(*itemInfo)) {
                newClientItems->removeElementAt(i);     // discard it
                i--;
                continue;
            }
        }
    }
    
    modClientItems = source.getItemsList("MOD");
    if (modClientItems) {
        for (int i=0; i<modClientItems->size(); i++) {
            if (config.isToAbort()) {
                setSyncError(ESSMCanceled);
                goto finally; 
            }
            
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)modClientItems->get(i);
            if (!itemInfo) continue;

            if (source.filterByExtension(*itemInfo)) {
                modClientItems->removeElementAt(i);     // discard it
                i--;
                continue;
            }
        }
    }


    //
    // Checks the GUID/LUID maps for NEW/MOD incoming items
    //
    if (checkMappings()) {
        goto finally;
    }

    //
    // remove fake updates from server list (lastModTime not changed)
    // 
    source.pruneModifiedItemsList(&modServerItems);
    
    // [optional] TWIN DETECTION: check if same item is NEW on both sides
    // NOTE: if found, the local twin will be removed too!
    // This may happen if the same item is added via another client (short circuit syncs)
    for (int i=0; i<newServerItems.size(); i++) {
        if (config.isToAbort()) { 
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)newServerItems.get(i);
        if (!itemInfo) continue;

        SapiSyncItemInfo* localTwin = source.twinDetection(*itemInfo, "NEW");
        if (localTwin) {
            StringBuffer luid = localTwin->getLuid();
            LOG.debug("skip item %s (twin detected) on new array", itemInfo->getName().c_str());

            // add entry in the mappings/cache, for future syncs!
            source.getMappings().setPropertyValue          (itemInfo->getGuid().c_str(), luid.c_str());
            source.getServerDateMappings().setPropertyValue(itemInfo->getGuid().c_str(), StringBuffer().sprintf("%li", (long)itemInfo->getModificationDate()));
            source.getCache().setPropertyValue             (luid.c_str(),                StringBuffer().sprintf("%li", (long)localTwin->getModificationDate()));

            delete localTwin;
            newServerItems.removeElementAt(i);
            i--;
            continue;
        }
    }

    // 
    // Conflicts resolution on MOD/DEL items
    //
    if (resolveConflicts()) {
        if (config.isToAbort()) { 
            setSyncError(ESSMCanceled);
        }
        
        goto finally; 
    }

    //
    // Check if local/remote items modified are just "renames".
    // If so, the item is flagged so the data content won't be uploaded/downloaded.
    // Done by the Syncsource, since it's specific checks for the item type.
    //
    source.localRenameChecks();
    source.remoteRenameChecks(modServerItems);

    if (config.isToAbort()) { 
        setSyncError(ESSMCanceled);
        goto finally; 
    }


    //
    // Validate local/remote lists (NEW/MOD/DEL):
    // a source may not support the updates or deletes on some direction,
    // in that case the corresponding list is cleared here.
    //
    source.validateLocalLists();
    source.validateRemoteLists(&newServerItems, &modServerItems, &delServerItems);


    //
    // Filter outgoing items (NEW & MOD)
    //
    if (filterOutgoingItems("NEW")) { goto finally; }
    if (filterOutgoingItems("MOD")) { goto finally; }


    //
    // Filter incoming items (NEW & MOD)
    //
    if (filterIncomingItems("NEW")) { goto finally; }
    if (filterIncomingItems("MOD")) { goto finally; }

    
finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::filterIncomingItems(const char* list) {

    ArrayList* serverItems = NULL;
    
    if (!list) { 
        setSyncError(ESSMGenericSyncError);
        goto finally; 
    }

    if      (!strcmp(list, "ALL"))  { serverItems = &allServerItems; }
    else if (!strcmp(list, "NEW"))  { serverItems = &newServerItems; } 
    else if (!strcmp(list, "MOD"))  { serverItems = &modServerItems; } 
    else if (!strcmp(list, "DEL"))  { serverItems = &delServerItems; }
    else { 
        setSyncError(ESSMGenericSyncError);
        goto finally; 
    }

    for (int i=0; i<serverItems->size(); i++) {
        if (config.isToAbort()) { 
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)serverItems->get(i);
        if (!itemInfo) continue;

        if (source.filterIncomingItem(*itemInfo, offsetClientServer)) {
            LOG.debug("skip server item %s (doesn't verify current incoming filters)", itemInfo->getName().c_str());
            serverItems->removeElementAt(i);
            i--;
            continue;
        }
    }

finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::filterOutgoingItems(const char* list) {
    
    ArrayListEnumeration* clientItems = NULL;
    
    if (!list) { 
        setSyncError(ESSMGenericSyncError);
        goto finally; 
    }

    clientItems = source.getItemsList(list);
    if (!clientItems) {
        // no error: the list may be NULL in the ssource
        goto finally; 
    }

    for (int i=0; i<clientItems->size(); i++) {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)clientItems->get(i);
        if (!itemInfo) continue;

        if (source.filterOutgoingItem(*itemInfo)) {
            LOG.debug("skip client item %s (doesn't verify current outgoing filters)", itemInfo->getName().c_str());
            clientItems->removeElementAt(i);
            i--;
            continue;
        }
    }

finally:
    return report.getLastErrorCode();
}


int SapiSyncManager::checkMappings() {

    //
    // Check the remote items list (NEW)
    // Exclude items already synced: check if GUID exists in maps
    // If exists -> treat it as an update
    // Note: no checks on the items size
    for (int i=0; i<newServerItems.size(); i++) {
        if (config.isToAbort()) { 
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)newServerItems.get(i);
        if (!itemInfo) continue;

        StringBuffer& guid = itemInfo->getGuid();
        StringBuffer luid = source.getMappings().readPropertyValue(guid.c_str());
        if (!luid.null()) {
            // updated item from server
            SapiSyncItemInfo* itemInfoTmp = (SapiSyncItemInfo*)newServerItems.get(i);
            modServerItems.add(*itemInfoTmp);
            newServerItems.removeElementAt(i);
            i--;
            continue;
        }
    }

    //
    // Check the remote items list (MOD)
    // Items not found in GUID/LUID map are treated as NEW
    //
    for (int i=0; i<modServerItems.size(); i++) {
        if (config.isToAbort()) { 
            setSyncError(ESSMCanceled);
            goto finally; 
        }
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)modServerItems.get(i);
        if (!itemInfo) continue;

        if (!findGuidInMappings(itemInfo->getGuid().c_str())) {
            LOG.debug("mod server item %s not found in mapping. Moving in new server items array", itemInfo->getName().c_str());
            SapiSyncItemInfo* itemInfoTmp = (SapiSyncItemInfo*)modServerItems.get(i);
            newServerItems.add(*itemInfoTmp);
            modServerItems.removeElementAt(i);
            i--;
            continue;
        }
    }

finally:
    return report.getLastErrorCode();
}





//
// ---------------------- UPLOAD ----------------------------
//
int SapiSyncManager::upload() 
{
    UploadSapiSyncItem* clientItem = NULL;

    LOG.info("Starting upload of client items");

    //
    // RESUME UPLOAD
    //
    int err = resumeUploads();
    if (err != ESSMSuccess && err != ESSMSapiError) {
        // stop all the uploads
        goto finally;
    }

    if (isSyncingItemChanges) {
        //
        // UPLOAD: NEW ITEMS
        //
        int err = 0;
        while ( (clientItem = source.getNextNewItem(&err)) != 0 ) 
        {
            if (err) {
                setSyncError(ESSMGetItemError, err);        // manage errors that need the upload to continue/exit
                continue;
            }
            int err = uploadItem(*clientItem, COMMAND_ADD);
            if (err == ESSMNetworkError) {
                err = retryUpload(clientItem);
            }
            delete clientItem;

            if (err == ESSMSuccess || err == ESSMSapiError) {
                // continue with next item
                continue;
            } else {
                // stop all the uploads
                goto finally;
            }
        }

        //
        // UPLOAD: MODIFICATIONS
        //
        while ( (clientItem = source.getNextModItem(&err)) != 0 ) 
        {
            int err = uploadItem(*clientItem, COMMAND_REPLACE);
            if (err == ESSMNetworkError) {
                err = retryUpload(clientItem);
            }
            delete clientItem;
            if (err == ESSMSuccess || err == ESSMSapiError) {
                // continue with next item
                continue;
            } else {
                // stop all the uploads
                goto finally;
            }
        }

        //
        // SEND DELETES
        //
        /*SapiSyncItemInfo* clientItemInfo = NULL;
        while ( (clientItemInfo = source.getNextDelItem(&err)) != 0 ) 
        {
            int err = deleteItem(*clientItemInfo, COMMAND_DELETE);
            delete clientItem;
        }*/
    }
    else {
        //
        // UPLOAD: ALL ITEMS
        //
        int count = 0;
        int err = 0;
        while ( (clientItem = source.getNextItem(&err)) != 0 ) {
            if (err) {
                setSyncError(ESSMGetItemError, err);        // manage errors that need the upload to continue/exit
                continue;
            }
            
            // if filtering by number, here we upload only N items
            if (clientFilterNumber < 0 || count < clientFilterNumber) {
                err = uploadItem(*clientItem, COMMAND_ADD);
                if (err == ESSMNetworkError) {
                    err = retryUpload(clientItem);
                }
                delete clientItem;
                count ++;

                if ((err != ESSMSuccess) && (err != ESSMSapiError)) {
                    // stop all the uploads
                    goto finally;
                }
            }
            
            // check if filtering count has been reached and then exit
            if (count == clientFilterNumber) { 
                LOG.debug("%s: filtering number count reached: stopping item get loop", __FUNCTION__); 
                break;
            }
        }
    }

finally:
    // commit final actions for the upload phase
    source.endUpload();

    // save timestamp for next upload
    if (report.getLastErrorCode() == 0) {
        source.getConfig().setLast((unsigned long)time(NULL));
    }

    // persist resume/mappings tables to disk
    source.getResume().close();
    source.getMappings().close();
    source.getServerDateMappings().close();

    return report.getLastErrorCode();
}


int SapiSyncManager::resumeUploads() {

    int err = 0;

    Enumeration& resumeIDs = source.getResume().getProperties();
    while (resumeIDs.hasMoreElement()) 
    {
        KeyValuePair* kvp = (KeyValuePair*)resumeIDs.getNextElement();
        if (!kvp) continue;

        StringBuffer type;
        SapiSyncItemInfo* oldItemInfo = createResumeItemInfo(*kvp, type);
        if (!oldItemInfo) continue;

        if (type == RESUME_UPLOAD) {
            // Get the item from syncsource
            StringBuffer luid = oldItemInfo->getLuid();
            StringBuffer guid = oldItemInfo->getGuid();
            StringBuffer command;
            
            UploadSapiSyncItem* resumeItem = source.getItem(luid);
            if (!resumeItem) {
                LOG.debug("The item to resume '%s' not found anymore locally -> clean it", luid.c_str());
                removeFromResumeMap(*oldItemInfo);
                continue;   // no error
            }

            if (findGuidInMappings(guid)) {
                command = COMMAND_REPLACE;
            } else {
                command = COMMAND_ADD;
            }
            
            // *** resume upload ***
            err = resumeUpload(*resumeItem, *oldItemInfo, command);
            if (err == ESSMNetworkError) {
                err = retryUpload(resumeItem);
            }
            delete resumeItem;
        }
        delete oldItemInfo;

        if (err != ESSMSuccess && err != ESSMSapiError) {
            // stop all the uploads
            return err;
        }
    }

    return err;
}


int SapiSyncManager::resumeUpload(UploadSapiSyncItem& resumeItem, SapiSyncItemInfo& oldItemInfo, const StringBuffer& command) 
{
    size_t offset = 0, firstByteToSend = 0;
    ESapiMediaRequestStatus err;
    InputStream* stream = NULL;
    int uploadStatus = -1;
    SapiSyncItemInfo* itemInfo = NULL;
    
    StringBuffer luid = oldItemInfo.getLuid();
    StringBuffer guid = oldItemInfo.getGuid();
    
    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally;
    }

    itemInfo = resumeItem.getSapiSyncItemInfo();
    
    if (!itemInfo) {
        LOG.error("ItemInfo for item to resume is NULL");
        setSyncError(ESSMGenericSyncError);
        removeFromResumeMap(oldItemInfo);
        goto finally;
    }
 
    LOG.info("Found a local item to resume: %s", itemInfo->getName().c_str());

    // Set the GUID (was stored last time in the resume map)
    itemInfo->setGuid(guid.c_str());

    // Check the name is the same as last time
    if (itemInfo->getName() != oldItemInfo.getName()) {
        LOG.info("Name changed: item can't be resumed (old name was %s)", oldItemInfo.getName().c_str());
        setSyncError(ESSMGetItemError);
        removeFromResumeMap(oldItemInfo);
        goto finally;
    }

    // Check the size is the same as last time
    if (itemInfo->getSize() != oldItemInfo.getSize()) {
        LOG.info("Size mismatch: item can't be resumed, will be uploaded from scratch");
        uploadStatus = uploadItem(resumeItem, COMMAND_ADD, false);
        goto finally;
    }
    else {
        // Get info about how much data the Server already have
        offset = 0;
        err = sapiMediaRequestManager->getItemResumeInfo(&resumeItem, &offset);
        
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying resume item info", __FUNCTION__);
                err = sapiMediaRequestManager->getItemResumeInfo(&resumeItem, &offset);
            }
        }
        
        if (err != ESMRSuccess) {
            setSyncError(ESSMSapiError, err);
            goto finally;
        }
    }

    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally;
    }

    // 'offset' is the last byte the server already has
    firstByteToSend = offset + 1;

    if (firstByteToSend >= itemInfo->getSize()) {
        LOG.info("Server already has all the item's data: skip it");
        source.setItemStatus(*itemInfo, 0, COMMAND_ADD);
        source.getMappings().setPropertyValue(guid.c_str(), luid.c_str());
        source.getServerDateMappings().setPropertyValue(guid.c_str(), StringBuffer().sprintf("%li", (long)itemInfo->getModificationDate()));
        removeFromResumeMap(*itemInfo);
        WString wluid;
        wluid = luid;
        report.addItem(SERVER, COMMAND_ADD, wluid.c_str(), ALREADY_EXISTS, NULL);
        
        // no upload actually happens but send anyway notifications to keep client updated 
        fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wluid.c_str(), ITEM_UPLOADING);
        fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wluid.c_str(), ITEM_UPLOADED);

        goto finally;
    }
    
    // Set the offset into the item
    LOG.debug("Resume from byte %d", firstByteToSend);
    stream = resumeItem.getStream();
    if (stream) {
        stream->setPosition(firstByteToSend);
    }

    // upload the remaining part of the item
    uploadStatus = uploadItem(resumeItem, COMMAND_ADD, true);
  
finally:

    if (uploadStatus != 0) {
        uploadStatus = report.getLastErrorCode();
    }
    return uploadStatus;
}



int SapiSyncManager::uploadItem(UploadSapiSyncItem& clientItem, const StringBuffer& command, bool isResume) {

    SapiSyncItemInfo* itemInfo;
    StringBuffer guid, luid;
    ESapiMediaRequestStatus err = ESMRSuccess;
    int uploadStatus = -1;
    time_t lastUpdate = 0;
    
    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        return report.getLastErrorCode();
    }

    itemInfo = clientItem.getSapiSyncItemInfo();
    if (!itemInfo) {
        LOG.error("Internal error: NULL upload item info");
        setSyncError(ESSMGenericSyncError);
        return report.getLastErrorCode();
    }
    
    luid = itemInfo->getLuid();
    if (luid.empty()) {
        LOG.error("Internal error: empty upload item LUID");
        setSyncError(ESSMGenericSyncError);
        return report.getLastErrorCode();
    }
    
    WString wluid;
    wluid = luid;
    
    // fire syncItem event
    fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wluid.c_str(), ITEM_UPLOADING);
    
    if (isResume == false) {
        //
        // Save item metadata on Server (get the Server GUID to upload data)
        //
        LOG.debug("Save metadata for item: %s", itemInfo->getName().c_str());
        ESapiMediaRequestStatus err = sapiMediaRequestManager->uploadItemMetaData(&clientItem);
        
        // SAPI session maybe expired during upload operation 
        // and we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we get an error 
            // interrupt the operation setting the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying add metadata for item: %s", __FUNCTION__, itemInfo->getName().c_str()); 
                // retry with new session
                err = sapiMediaRequestManager->uploadItemMetaData(&clientItem);
            }
        }
        
        if (err == ESMRQuotaExceeded) {
            LOG.info("Can't upload item %s: server quota exceeded", itemInfo->getName().c_str());
        }
        
        if (err != ESMRSuccess) {
            setSyncError(ESSMSapiError, err);
            goto finally;
        }
    }

    // Manage renames from client to Server 
    // (TODO: not yet implemented: the rename flag is not really set!)
    if (command == COMMAND_REPLACE && itemInfo->isRename() && !isResume) {
        LOG.debug("It's a rename: no need to upload item data");
        fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wluid.c_str(), ITEM_UPLOADED);
        uploadStatus = 0;
        goto finally;
    }


    // GUID is mandatory to upload data.
    //  - if it's an ADD, the GUID has been set by the sapiMediaRequestManager::uploadItemMetaData()
    //  - if it's a MOD, the GUID was set by the prepareItems() with a lookup in the mappings
    //  - if it's a resume, the GUID was stored in the resume map
    guid = itemInfo->getGuid();
    if (guid.empty()) {
        LOG.error("%s: empty upload item GUID, cannot proceed with update", __FUNCTION__);
        setSyncError(ESSMGenericSyncError);
        goto finally;
    }
    LOG.debug("Item GUID is %s", guid.c_str());

    
    //
    // ----- Upload item data content -----
    //

    // Add entry in resume map, in case of sync iterruption
    if (isResume == false) {
        addToResumeMap(*itemInfo, RESUME_UPLOAD);
    }

    LOG.info("Uploading item '%s'...", itemInfo->getName().c_str());
    err = sapiMediaRequestManager->uploadItemData(&clientItem, &lastUpdate);

    // SAPI session maybe expired during upload operation and
    // we get a HTTP 401 code ("access denied")
    if (err == ESMRAccessDenied) {
        // try a new login to get a new session id: if we
        // get an error interrupt the operation setting 
        // the item status
        LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
        err = sapiMediaRequestManager->login(config.getDevID());
        if (err == ESMRSuccess) {
            LOG.debug("%s: retrying item data upload", __FUNCTION__);
            // retry upload with new session
            err = sapiMediaRequestManager->uploadItemData(&clientItem, &lastUpdate); 
        }
    }
    
    // update source's cache
    source.setItemStatus(*itemInfo, err, command.c_str());

    if (err != ESMRSuccess) {
        if (err == ESMRQuotaExceeded) {
            LOG.info("Can't upload item %s: server quota exceeded", itemInfo->getName().c_str());
        }
        
        if (err == ESMRQuotaExceeded          || err == ESMRInvalidParam ||
            err == ESMRSapiMessageFormatError || err == ESMRHTTPFunctionalityNotSupported ||
            err == ESMRSapiNotSupported || err == ESMRInvalidContentRange  || 
            err == ESMRUnknownMediaException) {
            // For these error codes, we DON'T resume http upload
            removeFromResumeMap(*itemInfo);
        }
        setSyncError(ESSMSapiError, err);
        goto finally;
    }

    
    // upload was ok: add entry in GUID/LUID map + remove LUID from resume map
    // (for updates, it replaces the existing entry: the luid may be changed if it was a rename)
    source.getMappings().setPropertyValue(guid.c_str(), luid.c_str());

    
    // add/update entry in the ServerDate mappings
    source.getServerDateMappings().setPropertyValue(guid.c_str(), StringBuffer().sprintf("%li", (long)lastUpdate));
    
    removeFromResumeMap(*itemInfo);
    
    // fire syncItem event
    fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wluid.c_str(), ITEM_UPLOADED);
    LOG.debug("Upload complete (guid = %s)", guid.c_str());
    uploadStatus = 0;   // success

finally:
    if (uploadStatus != 0) {
        uploadStatus = report.getLastErrorCode();
    }

    // always update report, even if error!
    report.addItem(SERVER, command.c_str(), wluid.c_str(), uploadStatus, NULL);

    return uploadStatus;
}



//
// ---------------------- DOWNLOAD ----------------------------
//

int SapiSyncManager::download() {

    LOG.info("Starting download of server items");

    //
    // RESUME DOWNLOAD
    //
    int err = resumeDownloads();
    if ((err != ESSMSuccess) && (err != ESSMSapiError) &&
        (err != ESSMSetItemError)) {
        // stop all the downloads
        goto finally;
    }


    if (isSyncingItemChanges) {
        //
        // DOWNLOAD: NEW ITEMS
        //
        for (int i=0; i < newServerItems.size(); i++) {
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)newServerItems.get(i);
            if (!itemInfo) continue;

            int err = downloadItem(*itemInfo, COMMAND_ADD);

            if ((err == ESSMSuccess) || (err == ESSMSapiError) ||
                (err == ESSMSetItemError) || (err == ESSMItemNotSupportedBySource)) {
                // continue with next item
                continue;
            } else {
                // stop all the downloads
                goto finally;
            }
        }

        //
        // DOWNLOAD: MODIFICATIONS
        //
        for (int i=0; i < modServerItems.size(); i++) {
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)modServerItems.get(i);
            if (!itemInfo) continue;
            
            // TODO: if item's size is the same as locally, no need to download item's data (optimization)

            int err = downloadItem(*itemInfo, COMMAND_REPLACE);

            if (err == ESSMSuccess || err == ESSMSapiError) {
                // continue with next item
                continue;
            } else {
                // stop all the downloads
                goto finally;
            }
        }
        
        //
        // RECEIVE SERVER DELETES
        //
        for (int i=0; i < delServerItems.size(); i++) {
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)delServerItems.get(i);
            if (!itemInfo) continue;
            
            //
            // retrieve the luid needed to delete the item on the client
            //
            StringBuffer luid = source.getMappings().readPropertyValue(itemInfo->getGuid());
            itemInfo->setLuid(luid);
            
            //
            // fire the "fake" item downloading to update the feedback to the user
            //
            WString wguid;
            wguid = itemInfo->getGuid();
            fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wguid.c_str(), ITEM_DOWNLOADING);
    
            int err = source.deleteItem(*itemInfo);
            if (err >= 0) {
                source.getMappings().removeProperty(itemInfo->getGuid());
                source.getServerDateMappings().removeProperty(itemInfo->getGuid());
            } else {
                // should we care errors on deletes?
            }
            fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wguid.c_str(), ITEM_DOWNLOADED);
            
            // update report
            report.addItem(CLIENT, COMMAND_DELETE, wguid.c_str(), err, NULL);
        }
        
    }
    else {
        //
        // DOWNLOAD: ALL ITEMS
        //
        int count = 0;
        for (int i=0; i < allServerItems.size(); i++) {

            // if filtering by number, here we download only N items
            if (serverFilterNumber < 0 || count < serverFilterNumber) {
                SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)allServerItems.get(i);
                if (!itemInfo) continue;
                
                int err = downloadItem(*itemInfo, COMMAND_ADD);
                count ++;

                if ((err == ESSMSuccess) || (err == ESSMSapiError) ||
                    (err == ESSMSetItemError) || (err == ESSMItemNotSupportedBySource)) {
                    // continue with next item
                    continue;
                } else {
                    // stop all the downloads
                    goto finally;
                }
            }
        }
    }

finally:
    int errCode = 0;
    
    // commit final actions for the download phase
    source.endDownload();
    
    errCode = report.getLastErrorCode();
     
    // save timestamp for next download
    if ((errCode == 0) || (errCode == ESSMItemNotSupportedBySource)) {
        source.getConfig().setLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, (long)downloadTimestamp);
    }

    // persist resume/mappings tables to disk
    source.getResume().close();
    source.getMappings().close();
    source.getServerDateMappings().close();
    source.getBlacklist().close();
    
    return report.getLastErrorCode();
}

int SapiSyncManager::resumeDownloads() {

    int err = 0;
    Enumeration& resumeIDs = source.getResume().getProperties();

    while (resumeIDs.hasMoreElement()) {
        KeyValuePair* kvp = (KeyValuePair*)resumeIDs.getNextElement();
        if (!kvp) continue;

        StringBuffer type;
        SapiSyncItemInfo* itemInfo = createResumeItemInfo(*kvp, type);
        if (!itemInfo) continue;

        if (type == RESUME_DOWNLOAD) {
            err = resumeDownload(*itemInfo);
        }
        delete itemInfo;

        if (err != ESSMSuccess && err != ESSMSapiError) {
            // stop all the uploads
            return err;
        }
    }
    
    return err;
}

int SapiSyncManager::resumeDownload(SapiSyncItemInfo& oldItemInfo) {

    SapiSyncItemInfo* itemInfo = NULL;
    bool removeTmpItem = false;
    bool resume = true;
    int downloadStatus = -1;
    StringBuffer luid;
    StringBuffer command(COMMAND_ADD);
    StringBuffer guid = oldItemInfo.getGuid();
    
    if (config.isToAbort()) {
        setSyncError(ESSMCanceled);
        goto finally;
    }

    // Get the metadata of item to resume, from the server lists
    itemInfo = copyAndRemoveSapiSyncItemInfo(guid);
    if (itemInfo == NULL) {
        LOG.debug("The item to resume '%s' is not found anymore in server lists -> clean it", guid.c_str());
        removeFromResumeMap(oldItemInfo);
        removeTmpItem = true;
        goto finally;   // no error
    }
    LOG.info("Found a server item to resume: %s", itemInfo->getName().c_str());

    // Check the size is the same as last time
    if (itemInfo->getSize() != oldItemInfo.getSize()) {
        LOG.info("Size mismatch: item can't be resumed, will be downloaded from scratch");
        source.cleanTemporaryDownloadedItem(guid);
        resume = false;
    }
   
    luid = source.getMappings().readPropertyValue(guid);
    
    if (!luid.empty()) {
        command = COMMAND_REPLACE;
    }

    // download the remaining part of the item
    downloadStatus = downloadItem(*itemInfo, command, resume);

finally:

    if (removeTmpItem) {
        // clear tmp item in case of error
        source.cleanTemporaryDownloadedItem(guid);
    }

    delete itemInfo;

    if (downloadStatus != 0) {
        downloadStatus = report.getLastErrorCode();
    }
    return downloadStatus;
}

int SapiSyncManager::downloadItem(SapiSyncItemInfo& itemInfo, const StringBuffer& command, bool isResume) {

    WString wguid;
    StringBuffer luid;
    DownloadSapiSyncItem* serverItem = NULL;
    ESapiMediaRequestStatus err;
    ESapiSyncSourceError sourceErr = ESSSNoErr;
    bool removeTmpItem = false;
    int downloadStatus = -1;

    if (config.isToAbort()) {
        setSyncError(ESSMCanceled); 
        return report.getLastErrorCode();
    }

    StringBuffer& guid = itemInfo.getGuid();
    if (guid.empty()) {
        LOG.error("Internal error: empty download item GUID");
        setSyncError(ESSMGenericSyncError);
        goto finally;
    }
    wguid = guid;

    // Check local quota (not in case of resume!)
    if (isResume == false) {
        int requiredSpace = itemInfo.getSize();
        int errorCode = 0;        
        if (command == COMMAND_REPLACE) {
            // calculate required disk space for an update
            SapiSyncItemInfo* localItem = source.getItemInfo(luid);
            if (localItem) {
                if (itemInfo.getSize() > localItem->getSize()) {
                    requiredSpace = itemInfo.getSize() - localItem->getSize();
                }
            }
        }        
        if (source.isLocalStorageAvailable(requiredSpace, &errorCode) == false) {
            if (errorCode == ERR_FILE_SYSTEM) {
                LOG.info("Can't download item %s: cannot find the MediaHub path", itemInfo.getName().c_str());
                setSyncError(ESSMMediaHubPathNotFound);            
            } else {
                LOG.info("Can't download item %s: local storage is full", itemInfo.getName().c_str());
                setSyncError(ESSMClientQuotaExceeded);
            }
            goto finally;
        }
    }

    // Allocate space for the incoming item (creates the output stream)
    // In case of resume, opens the already existing item and links the output stream
    // at the end of the data already received
    serverItem = source.createItem(itemInfo);
    if (serverItem == NULL) {
        setSyncError(ESSMSetItemError);
        goto finally;
    }

    // write GUID in resume map: add entry <guid,"download">
    if (isResume == false) {
        addToResumeMap(itemInfo, RESUME_DOWNLOAD);
    }

    // fire syncItem event
    fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wguid.c_str(), ITEM_DOWNLOADING);


    // Manage renames from Server to client
    if (command == COMMAND_REPLACE && itemInfo.isRename() && !isResume) {
        LOG.debug("It's a rename: no need to download item data");
        luid = source.updateItem(serverItem, &sourceErr);
        fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wguid.c_str(), ITEM_DOWNLOADED);
        downloadStatus = 0;
        goto finally;
    }

    //
    // *** Download data ***
    //
    LOG.info("Downloading item '%s'...", itemInfo.getName().c_str());
    err = sapiMediaRequestManager->downloadItem(serverItem);
    
    if (err == ESMRNetworkError) {
        err = retryDownload(serverItem);
    } else {
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying item download", __FUNCTION__);
                err = sapiMediaRequestManager->downloadItem(serverItem);
            
                if (err == ESMRNetworkError) {
                    err = retryDownload(serverItem);
                }
            }
        }
    }

    // SAPI session maybe expired during download operation and
    // we get a HTTP 401 code ("access denied")
    if (err == ESMRAccessDenied) {
        // try a new login to get a new session id: if we
        // get an error interrupt the operation setting 
        // the item status
        LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
        err = sapiMediaRequestManager->login(config.getDevID());
        if (err == ESMRSuccess) {
            LOG.debug("%s: retrying item download", __FUNCTION__);
            err = sapiMediaRequestManager->downloadItem(serverItem);
        
            if (err == ESMRNetworkError) {
                err = retryDownload(serverItem);
            }
        }
    }

    if (err != ESMRSuccess) {
        if (err == ESMRNetworkError) {
            setSyncError(ESSMNetworkError, config.getSapiMaxRetriesOnError());
            goto finally;
        }
        if (err == ESMRInvalidParam        || err == ESMRConnectionSetupError ||
            err == ESMRAccessDenied        || err == ESMRHTTPFunctionalityNotSupported ||
            err == ESMRSapiNotSupported    || err == ESMRInvalidContentRange || 
            err == ESMRErrorRetrievingMediaItem) {
            // For these error codes, we DON'T resume http download
            removeFromResumeMap(itemInfo);
            removeTmpItem = true;
        }
        setSyncError(ESSMSapiError, err);
        goto finally;
    }

    // download completed: remove GUID from resume map
    removeFromResumeMap(itemInfo);

    //
    // Insert the item into the local storage
    //
    luid = "";
    if (command == COMMAND_ADD) {
        luid = source.addItem(serverItem, &sourceErr);
    } 
    else if (command == COMMAND_REPLACE) {
        luid = source.updateItem(serverItem, &sourceErr);
    }
    
    if (luid.empty()) {
        switch (sourceErr) {
            case ESSSItemNotSupported:
                setSyncError(ESSMItemNotSupportedBySource);
                break;
                
            default:
                setSyncError(ESSMSetItemError);
                break;
        }
        
        goto finally;
    }

    // download ok and item correctly inserted: add entry in GUID/LUID map (and serverDate map)
    source.getMappings().setPropertyValue          (guid.c_str(), luid.c_str());
    source.getServerDateMappings().setPropertyValue(guid.c_str(), StringBuffer().sprintf("%li", (long)itemInfo.getModificationDate()));
    
    // fire syncItem event
    fireSyncItemEvent(sourceURI.c_str(), sourceName.c_str(), wguid.c_str(), ITEM_DOWNLOADED);
    LOG.debug("Download complete (luid = %s)", luid.c_str());
    downloadStatus = 0;   // success


finally:

    if (downloadStatus != 0) {
        downloadStatus = report.getLastErrorCode();
    }

    // always update report, even if error!
    report.addItem(CLIENT, command.c_str(), wguid.c_str(), downloadStatus, NULL);

    delete serverItem;

    if (removeTmpItem) {
        // clear tmp item in case of error
        source.cleanTemporaryDownloadedItem(guid);
    }
    return downloadStatus;
}



int SapiSyncManager::endSync() {

    fireSyncEvent(NULL, SEND_FINALIZATION);

    // LOGOUT
    ESapiMediaRequestStatus err = sapiMediaRequestManager->logout();
    if (err != ESMRSuccess) {
        LOG.error("%s: session logout failed with code: %d", __FUNCTION__, err);  
    }

    // Commit final actions on the syncsource
    int ret = source.endSync();
    if (ret) {
        setSyncError(ESSMEndSyncError, ret);
    }

    // Set the end sync time (now)
    source.getConfig().setEndSyncTime((unsigned long) time(NULL));
    
    // Fire endSync event
    fireSyncEvent(NULL, SYNC_END);

    LOG.info("Synchronization of %s source completed (ret = %d)", sourceName.c_str(), report.getLastErrorCode());
    return report.getLastErrorCode();
}


//
// ------------------------ retry mechanism ---------------------------
//

ESMRStatus SapiSyncManager::retryGetItemsChanges(ArrayList& newIDs, ArrayList& modIDs, ArrayList& delIDs, 
                                          const StringBuffer& fromDate) {
    ESMRStatus err = ESMRNetworkError;
    int maxRetries = config.getSapiMaxRetriesOnError();
    if (maxRetries == 0) {
        return err;
    }

    int attempt = 0;
    while (attempt < maxRetries) 
    {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            return err;
        }

        LOG.info("Retry getItemsChanges (%d of %d)...", attempt+1, maxRetries);
        fireSyncSourceEvent(sourceURI.c_str(), sourceName.c_str(), syncMode, attempt+1, SYNC_SOURCE_RETRY);

        long sleepMsec = config.getSapiSleepTimeOnRetry();
        if (sleepMsec) {
            LOG.debug("sleep %li msec", sleepMsec);
            sleepMilliSeconds(sleepMsec);
        }

        newIDs.clear();
        modIDs.clear();
        delIDs.clear();
        
        err = sapiMediaRequestManager->getItemsChanges(newIDs, modIDs, delIDs, fromDate, &downloadTimestamp);
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying get items changes", __FUNCTION__);
                err = sapiMediaRequestManager->getItemsChanges(newIDs, modIDs, delIDs, fromDate, &downloadTimestamp); 
            } else {
                break;
            }
        }
        
        if (err == ESMRNetworkError) {
            attempt ++;     // Network error
            continue;
        } 
        else {
            break;          // all other errors
        }
    }
    return err;
}

ESMRStatus SapiSyncManager::retryGetItemsFromId(ArrayList& items, const ArrayList& itemsIDs) {

    ESMRStatus err = ESMRNetworkError;
    int maxRetries = config.getSapiMaxRetriesOnError();

    if (maxRetries == 0) {
        return err;
    }

    int attempt = 0;
    while (attempt < maxRetries) 
    {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            return err;
        }

        LOG.info("Retry getItemsFromIDs (%d of %d)...", attempt+1, maxRetries);
        fireSyncSourceEvent(sourceURI.c_str(), sourceName.c_str(), syncMode, attempt+1, SYNC_SOURCE_RETRY);

        long sleepMsec = config.getSapiSleepTimeOnRetry();
        if (sleepMsec) {
            LOG.debug("sleep %li msec", sleepMsec);
            sleepMilliSeconds(sleepMsec);
        }

        items.clear();
        err = sapiMediaRequestManager->getItemsFromId(items, itemsIDs);
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying get items from id", __FUNCTION__);
                err = sapiMediaRequestManager->getItemsFromId(items, itemsIDs); 
            } else {
                break;
            }
        }
        
        if (err == ESMRNetworkError) {
            attempt ++;     // Network error
            continue;
        } 
        else {
            break;          // all other errors
        }
    }
    return err;
}

ESMRStatus SapiSyncManager::retryGetAllItems(ArrayList& items, int limit, int offset) {
    
    ESMRStatus err = ESMRNetworkError;
    int maxRetries = config.getSapiMaxRetriesOnError();
    if (maxRetries == 0) {
        return err;
    }

    int attempt = 0;
    while (attempt < maxRetries) 
    {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            return err;
        }

        LOG.info("Retry getAllItems (%d of %d)...", attempt+1, maxRetries);
        fireSyncSourceEvent(sourceURI.c_str(), sourceName.c_str(), syncMode, attempt+1, SYNC_SOURCE_RETRY);

        long sleepMsec = config.getSapiSleepTimeOnRetry();
        if (sleepMsec) {
            LOG.debug("sleep %li msec", sleepMsec);
            sleepMilliSeconds(sleepMsec);
        }

        items.clear();
        err = sapiMediaRequestManager->getAllItems(items, &downloadTimestamp, limit, offset); 
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying get all items", __FUNCTION__);
                err = sapiMediaRequestManager->getAllItems(items, &downloadTimestamp, limit, offset); 
            } else {
                break;
            }
        }
        
        if (err == ESMRNetworkError) {
            attempt ++;     // Network error
            continue;
        } 
        else {
            
            break;          // all other errors
        }
    }
    return err;
}

int SapiSyncManager::retryUpload(UploadSapiSyncItem* clientItem) {

    int err = ESSMNetworkError;
    int maxRetries = config.getSapiMaxRetriesOnError();
    SapiSyncItemInfo* itemInfo;
    StringBuffer command;
    StringBuffer guid;
    
    if (maxRetries == 0) {
        err = ESSMNetworkError;
        goto finally;
    }

    itemInfo = clientItem->getSapiSyncItemInfo();
    int attempt; attempt = 0;
    
    guid = itemInfo->getGuid();
    
    if (findGuidInMappings(guid)) {
        command = COMMAND_REPLACE;
    } else {
        command = COMMAND_ADD;
    }
        
    while (attempt < maxRetries) 
    {
        if (config.isToAbort()) {
            err = ESSMCanceled;
            goto finally;
        }

        LOG.info("Retry upload of item '%s' (%d of %d)...", itemInfo->getName().c_str(), attempt+1, maxRetries);
        fireSyncSourceEvent(sourceURI.c_str(), sourceName.c_str(), syncMode, attempt+1, SYNC_SOURCE_RETRY);

        long sleepMsec = config.getSapiSleepTimeOnRetry();
        if (sleepMsec) {
            LOG.debug("sleep %li msec", sleepMsec);
            sleepMilliSeconds(sleepMsec);
        }

        int oldPos = clientItem->getStream()->getPosition();

        // reset stream if needed 
        if (config.getSapiResetStreamOnRetry()) {
            clientItem->resetStream();
        }
        
        err = resumeUpload(*clientItem, *itemInfo, command);
        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying item resume upload", __FUNCTION__);
                err = resumeUpload(*clientItem, *itemInfo, command);    
            } else {
                break;
            }
        }
        
        if (err == ESSMNetworkError) {
            // Network error: check the amount of data transferred
            int newPos = clientItem->getStream()->getPosition();
            LOG.debug("Retry upload failed: data transferred = %d bytes", newPos - oldPos);
            
            long minDataSize = config.getSapiMinDataSizeOnRetry();
            if ((minDataSize == 0) || (newPos - oldPos) <= minDataSize) {
                // transferred data is small (or none)
                attempt ++;
            } else {
                // transferred data is big: reset the retry mechanism
                attempt = 0;
            }
            continue;
        } else {
            // all other errors: out
            break;
        }
    }

finally:
    setSyncError((ESSMError)err, maxRetries);
    return err;
}

ESMRStatus SapiSyncManager::retryDownload(DownloadSapiSyncItem* serverItem) {

    ESMRStatus err = ESMRNetworkError;
    int maxRetries = config.getSapiMaxRetriesOnError();
    if (maxRetries == 0) {
        return err;
    }

    SapiSyncItemInfo* itemInfo = serverItem->getSapiSyncItemInfo();
    
    int attempt = 0;
    while (attempt < maxRetries) 
    {
        if (config.isToAbort()) {
            setSyncError(ESSMCanceled);
            return err;
        }

        LOG.info("Retry download of item '%s' (%d of %d)...", itemInfo->getName().c_str(), attempt+1, maxRetries);
        fireSyncSourceEvent(sourceURI.c_str(), sourceName.c_str(), syncMode, attempt+1, SYNC_SOURCE_RETRY);

        long sleepMsec = config.getSapiSleepTimeOnRetry();
        if (sleepMsec) {
            LOG.debug("sleep %li msec", sleepMsec);
            sleepMilliSeconds(sleepMsec);
        }

        int oldPos = serverItem->getStream()->size();

        err = sapiMediaRequestManager->downloadItem(serverItem);

        // SAPI session maybe expired during download operation and
        // we get a HTTP 401 code ("access denied")
        if (err == ESMRAccessDenied) {
            // try a new login to get a new session id: if we
            // get an error interrupt the operation setting 
            // the item status
            LOG.debug("%s: session id expired: starting new login request", __FUNCTION__);
            err = sapiMediaRequestManager->login(config.getDevID());
            if (err == ESMRSuccess) {
                LOG.debug("%s: retrying item download", __FUNCTION__);
                err = sapiMediaRequestManager->downloadItem(serverItem);
            } else {
                break;
            }
        }

        if (err == ESMRNetworkError) {
            // Network error: check the amount of data transferred
            int newPos = serverItem->getStream()->size();
            LOG.debug("Retry download failed: data transferred = %d bytes", newPos - oldPos);
            
            long minDataSize = config.getSapiMinDataSizeOnRetry();
            if ((minDataSize == 0) || (newPos - oldPos) <= minDataSize) {
                // transferred data is small (or none)
                attempt ++;
            } else {
                // transferred data is big: reset the retry mechanism
                attempt = 0;
            }
         
            continue;
        } else {
            // all other errors: out
            break;
        }
    }

    return err;
}


// ------------ private utility methods -------------

SapiMediaSourceType SapiSyncManager::getMediaSourceType() {

    if      (sourceName == "picture")  return ESapiMediaSourcePictures;
    else if (sourceName == "video")    return ESapiMediaSourceVideos;
    else if (sourceName == "files")    return ESapiMediaSourceFiles;
    else                               return ESapiMediaSourceUndefined;
}

bool SapiSyncManager::findGuidInMappings(const char* guid) {
    StringBuffer luid = source.getMappings().readPropertyValue(guid);
    return !(luid.null());
}

bool SapiSyncManager::findLuidInMappings(const char* luid) {
    // Use the cache (optimization: LUID is the key)
    StringBuffer signature = source.getCache().readPropertyValue(luid);
    return !(signature.null());
}

int SapiSyncManager::getClientItemsNumber(const char* listType) {

    int count = source.getItemsNumber(listType);
    if (count < 0) {
        // list not initialized or unknown
        count = 0;
    }
    return count;
}

int SapiSyncManager::readClientFilterNumber() {

    bool err = false;
    int num = source.getConfig().getLongProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
    if (err) {
        num = -1;   // default = disabled
    }
    return num;
}

int SapiSyncManager::readServerFilterNumber() {

    bool err = false;
    int num = source.getConfig().getLongProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
    if (err) {
        num = -1;   // default = disabled
    }
    return num;
}

unsigned long SapiSyncManager::readServerLastTimestamp() {

    bool err = false;
    unsigned long serverLast = source.getConfig().getLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, &err);
    if (err) { serverLast = 0; }
    return serverLast;
}


bool SapiSyncManager::defineSyncBehavior() {

    // Read all source filters and last sync times.
    unsigned long clientLast = source.getConfig().getLast();
    unsigned long serverLast = readServerLastTimestamp();

    if (clientLast == 0 && serverLast == 0) {
        // Very 1st sync: sync ALL (must check the twins)
        return false;
    }

    if (isDownloadEnabled(syncMode)) {
        if (serverFilterNumber >= 0) {
            // Filter by number is set on download: sync ALL
            return false;
        }
        if (serverLast == 0) {
            // First download ever: sync ALL
            return false;
        }
    }

    if (isUploadEnabled(syncMode)) {
        if (clientFilterNumber >= 0) {
            // Filter by number is set on upload: sync ALL
            return false;
        }
        if (clientLast == 0) {
            // First upload ever: sync ALL
            return false;
        }
    }

    // A filter by date is set on an active sync direction
    // (and it's not the first sync, nor a filter by number is set)
    return true;
}


SapiSyncItemInfo* SapiSyncManager::copyAndRemoveSapiSyncItemInfo(const StringBuffer& guid) {

    SapiSyncItemInfo* itemInfo = NULL;

    if (isSyncingItemChanges) {
        itemInfo = copyAndRemoveSapiSyncItemInfo(newServerItems, guid);
        if (itemInfo) {
            return itemInfo;
        }
        itemInfo = copyAndRemoveSapiSyncItemInfo(modServerItems, guid);
        // DEL items list is excluded
    }
    else {
        itemInfo = copyAndRemoveSapiSyncItemInfo(allServerItems, guid);
    }
    return itemInfo;
}

SapiSyncItemInfo* SapiSyncManager::copyAndRemoveSapiSyncItemInfo(ArrayList& list, const StringBuffer& guid) {

    for (int i=0; i<list.size(); i++) {
        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)list.get(i);
        if (!itemInfo) continue;

        if (itemInfo->getGuid() == guid) {
            SapiSyncItemInfo* ret = (SapiSyncItemInfo*)itemInfo->clone();
            list.removeElementAt(i);
            return ret;
        }
    }
    return NULL;
}


SapiSyncItemInfo* SapiSyncManager::createResumeItemInfo(const KeyValuePair& kvp, StringBuffer& type) {

    type = "";
    const char* guid = kvp.getKey().c_str();
    if (guid == NULL) return NULL;  // guid is required
    
    ArrayList values;
    kvp.getValue().split(values, RESUME_MAP_FIELD_SEPARATOR);
    
    StringBuffer* resumeType = (StringBuffer*)values.get(0);
    if (resumeType == NULL) return NULL; // type is required
    type = *resumeType;

    StringBuffer* luid = (StringBuffer*)values.get(1);
    StringBuffer* name = (StringBuffer*)values.get(2);
    StringBuffer* size = (StringBuffer*)values.get(3);
    // add other params if needed

    SapiSyncItemInfo* info = new SapiSyncItemInfo();
    info->setGuid(guid);
    if (luid) info->setLuid(luid->c_str());
    if (name) info->setName(name->c_str());
    if (size && !size->empty()) {
        int itemSize = atoi(size->c_str());
        info->setSize(itemSize);
    }
    return info;
}


void SapiSyncManager::addToResumeMap(SapiSyncItemInfo& itemInfo, const char* type) {

    // (to disable resumable uploads)
    //if (!strcmp(type, RESUME_UPLOAD)) {
    //    return;
    //}

    StringBuffer resumeType(type);
    StringBuffer size;
    size.sprintf("%lu", itemInfo.getSize());

    ArrayList params;
    params.add(resumeType);
    params.add(itemInfo.getLuid());
    params.add(itemInfo.getName());
    params.add(size);

    const char* key = itemInfo.getGuid().c_str();
    StringBuffer value;
    value.join(params, RESUME_MAP_FIELD_SEPARATOR);

    source.getResume().setPropertyValue(key, value.c_str());
}

void SapiSyncManager::removeFromResumeMap(SapiSyncItemInfo& itemInfo) {

    const char* key = itemInfo.getGuid().c_str();
    source.getResume().removeProperty(key);
}


bool SapiSyncManager::checkLastUserAndServer() {

    StringBuffer lastSyncUrl  = source.getConfig().getProperty(PROPERTY_LAST_SYNC_URL);
    StringBuffer lastSyncUser = source.getConfig().getProperty(PROPERTY_LAST_SYNC_USERNAME);

    if (lastSyncUrl  != config.getSyncURL() ||
        lastSyncUser != config.getUsername()) 
    {
        LOG.debug("last server url or username has changed: cleaning up local cache and maps");
        isSyncingItemChanges = false;
        source.getCache().removeAllProperties();
        source.getMappings().removeAllProperties();
        source.getServerDateMappings().removeAllProperties();
        source.getResume().removeAllProperties();
        source.getBlacklist().removeAllProperties();
        
        source.getConfig().setLast(0);
        source.getConfig().setLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, 0);
        
        // Set immediately the new values
        source.getConfig().setProperty(PROPERTY_LAST_SYNC_URL, config.getSyncURL());
        source.getConfig().setProperty(PROPERTY_LAST_SYNC_USERNAME, config.getUsername());
        return true;
    }
    return false;
}


void SapiSyncManager::setSyncError(const ESSMError errorCode, const int data) {

    ESSMError code = errorCode;
    StringBuffer msg("");

    if (config.isToAbort()) {
        // Cancel sync error wins over other errors
        code = ESSMCanceled;
    }

    switch (code)
    {
        case ESSMSuccess:
        {
            // reset error
            report.setState(SOURCE_ACTIVE);
            report.setLastErrorType("");
            break;
        }
        case ESSMCanceled:
        {
            msg = "Sync canceled by the user";
            break;
        }
        case ESSMConfigError:
        {
            msg.sprintf("Configuration error for source %s", sourceName.c_str());
            break;
        }

        // SAPI errors
        case ESSMSapiError:
        {
            //
            // filter SAPI error codes (data) -> set high level errors (http/invalid sapi/...)
            //
            ESMRStatus sapiStatus = (ESMRStatus)data;
            switch (sapiStatus)
            {
                case ESMRAccessDenied:
                {
                    code = ESSMAuthenticationError;
                    msg.sprintf("SAPI authentication error: code %s%d", ERROR_TYPE_SAPI_REQUEST_MANAGER, data);
                    break;
                }
                
                case ESMRHTTPFunctionalityNotSupported:
                case ESMRSapiNotSupported:
                {
                    code = ESSMSapiNotSupported;
                    msg.sprintf("SAPI request not implemented by server: code %s%d", ERROR_TYPE_SAPI_REQUEST_MANAGER, data);
                    
                    break;
                }
                
                case ESMRConnectionSetupError:
                case ESMRGenericHttpError:
                case ESMRNetworkError:
                case ESMRRequestTimeout:
                {
                    code = ESSMNetworkError;
                    msg.sprintf("Network error: code %s%d", ERROR_TYPE_SAPI_REQUEST_MANAGER, data);
                    break;
                }
                case ESMRQuotaExceeded:
                {
                    code = ESSMServerQuotaExceeded;
                    msg.sprintf("Server quota exceeded for source %s", sourceName.c_str()); // add info about quota??
                    break;
                }
                default:
                {
                    msg.sprintf("SAPI request error: code %s%d", ERROR_TYPE_SAPI_REQUEST_MANAGER, data);
                    break;
                }
            }
            break;
        }

        // SyncSource related errors
        case ESSMBeginSyncError:
        {
            msg.sprintf("Error in beginSync of source %s (code %d)", sourceName.c_str(), data);
            break;
        }
        case ESSMEndSyncError:
        {
            msg.sprintf("Error in endSync of source %s (code %d)", sourceName.c_str(), data);
            break;
        }
        case ESSMGetItemError:
        {
            msg.sprintf("Error getting item of source %s (code %d)", sourceName.c_str(), data);
            break;
        }
        case ESSMSetItemError:
        {
            msg.sprintf("Error setting item of source %s (code %d)", sourceName.c_str(), data);
            break;
        }
        case ESSMItemNotSupportedBySource:
        {
            msg.sprintf("Item not supported by source %s (code %d)", sourceName.c_str(), data);
            break;
        }
        case ESSMNetworkError:
        {
            if (data > 0) {
                msg.sprintf("Network error (%d attempts failed)", data);
            } else {
                msg.sprintf("Network error");
            }
            break;
        }
        case ESSMClientQuotaExceeded:
        {
            msg.sprintf("Local storage space is full for source %s", sourceName.c_str()); // add info about quota??
            break;
        }
        
        default:
        {
            msg = "SapiSyncManager error";
            break;
        }
    }

    report.setLastErrorCode((int)code);
    report.setLastErrorMsg(msg.c_str());

    if (code != ESSMSuccess) {
        report.setState(SOURCE_ERROR);
        report.setLastErrorType(ERROR_TYPE_SAPI_SYNC_MANAGER);
        fireSyncEvent(report.getLastErrorMsg(), SYNC_ERROR);
    }
}

