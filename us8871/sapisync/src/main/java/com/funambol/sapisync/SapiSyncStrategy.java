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

package com.funambol.sapisync;

import java.util.Hashtable;
import java.util.Date;

import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncSource;
import com.funambol.sync.TwinDetectionSource;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.sync.Filter;
import com.funambol.sync.SyncFilter;
import com.funambol.util.Log;


public class SapiSyncStrategy {

    private static final String TAG_LOG = "SapiSyncStrategy";

    private static final int FULL_SYNC_DOWNLOAD_LIMIT = 300;

    private JSONArray addedArray   = null;
    private JSONArray updatedArray = null;
    private JSONArray deletedArray = null;

    private Hashtable localUpdated;
    private Hashtable localDeleted;
    private Hashtable localRenamed;

    private SapiSyncHandler sapiSyncHandler;
    private JSONObject removedItemMarker;
    private long downloadNextAnchor;
    private String addedServerUrl = null;
    private String updatedServerUrl = null;
    private long clientServerTimeDifference = 0;
    private SapiSyncUtils utils = new SapiSyncUtils();

    public SapiSyncStrategy(SapiSyncHandler sapiSyncHandler, JSONObject removedItemMarker) {
        this.sapiSyncHandler = sapiSyncHandler;
        this.removedItemMarker = removedItemMarker;
    }

    /**
     * This method computes the set of items to be downloaded from the server in
     * an incremental sync.
     * The set of data to be downloaded depends on many things, including the
     * changes made locally. After this method the src update/delete items have
     * been consumed and the getNextNewItem and getNextUpdItem will return null.
     */
    public void prepareSync(SyncSource src, int downloadSyncMode, int uploadSyncMode,
                            boolean resume, MappingTable mapping, boolean incrementalDownload,
                            boolean incrementalUpload, Hashtable twins)
    throws SyncException, JSONException
    {
        // Check what is available on the server and what changed locally to
        // determine the list of items to be exchanged
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Computing changes set for fast sync");
        }

        addedArray   = null;
        updatedArray = null;
        deletedArray = null;

        // Get all the info on what is available on the server
        if (downloadSyncMode != SyncSource.NO_SYNC) {
            if (incrementalDownload) {
                prepareSyncIncrementalDownload(src, mapping, twins);
            } else {
                prepareSyncFullDownload(src, mapping, twins);
            }
        }

        // Get all the info on what changes are to be sent
        if (uploadSyncMode != SyncSource.NO_SYNC) {
            if (incrementalUpload) {
                prepareSyncIncrementalUpload(src, mapping, twins);
            } else {
                prepareSyncFullUpload(src, mapping, downloadSyncMode, incrementalDownload, twins);
            }
        } else {
            localUpdated = null;
            localDeleted = null;
            localRenamed = null;
        }

        // Resolve conflicts
        finalizePreparePhase(src, mapping, twins, incrementalDownload, incrementalUpload);
    }

    public JSONArray getServerAddedItems() {
        return addedArray;
    }

    public JSONArray getServerUpdatedItems() {
        return updatedArray;
    }

    public JSONArray getServerDeletedItems() {
        return deletedArray;
    }

    public long getDownloadNextAnchor() {
        return downloadNextAnchor;
    }

    public String getAddedServerUrl() {
        return addedServerUrl;
    }

    public String getUpdatedServerUrl() {
        return updatedServerUrl;
    }

    public void setClientServerTimeDifference(long clientServerTimeDifference) {
        this.clientServerTimeDifference = clientServerTimeDifference;
    }

    public void setSapiSyncHandler(SapiSyncHandler sapiSyncHandler) {
        this.sapiSyncHandler = sapiSyncHandler;
    }

    public Hashtable getLocalUpdates() {
        return localUpdated;
    }

    public Hashtable getLocalDeletes() {
        return localDeleted;
    }

    private void prepareSyncIncrementalDownload(SyncSource src, MappingTable mapping, Hashtable twins)
    throws SyncException, JSONException
    {
        String remoteUri = src.getConfig().getRemoteUri();
        SyncFilter syncFilter = src.getFilter();
        if(syncFilter != null && syncFilter.getIncrementalDownloadFilter() != null) {
            throw new UnsupportedOperationException("Not implemented yet");
        }
        SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)src.getConfig().getSyncAnchor();
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Last download anchor is: " + sapiAnchor.getDownloadAnchor());
        }
        Date anchor = new Date(sapiAnchor.getDownloadAnchor());

        SapiSyncHandler.ChangesSet changesSet = null;
        try {
            changesSet = sapiSyncHandler.getIncrementalChanges(anchor, remoteUri);
        } catch (SapiException e) {
            String errorMessage = "Client error while getting incremental changes";
            utils.processCommonSapiExceptions(e, errorMessage, false);
            utils.processCustomSapiExceptions(e, errorMessage, true);
        }
            
        if (changesSet != null) {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "There are changes pending on the server");
            }

            // Use the above value as timestamp for the next sync
            downloadNextAnchor = changesSet.timeStamp;

            SapiSyncHandler.FullSet addedInfo   = null;
            SapiSyncHandler.FullSet updatedInfo = null;
            SapiSyncHandler.FullSet deletedInfo = null;

            addedInfo   = fetchItemsInfo(src, changesSet.added);
            updatedInfo = fetchItemsInfo(src, changesSet.updated);
            deletedArray = changesSet.deleted;

            if (addedInfo != null) {
                addedArray = addedInfo.items;
                addedServerUrl = addedInfo.serverUrl;
            } 
            if (updatedInfo != null) {
                updatedArray = updatedInfo.items;
                updatedServerUrl = updatedInfo.serverUrl;
            }
        }
    }

    private void prepareSyncIncrementalUpload(SyncSource src, MappingTable mapping, Hashtable twins)
    throws SyncException, JSONException
    {
        localUpdated = new Hashtable();
        localDeleted = new Hashtable();
        localRenamed = new Hashtable();

        JSONSyncItem localUpdatedItem = (JSONSyncItem)src.getNextUpdatedItem();
        while(localUpdatedItem != null) {

            if (localUpdatedItem.isItemKeyUpdated() && localUpdatedItem.getOldKey() != null) {
                localRenamed.put(localUpdatedItem.getOldKey(), localUpdatedItem);
            }

            localUpdated.put(localUpdatedItem.getKey(), localUpdatedItem);
            localUpdatedItem = (JSONSyncItem)src.getNextUpdatedItem();

        }

        SyncItem localDeletedItem = src.getNextDeletedItem();
        while(localDeletedItem != null) {
            localDeleted.put(localDeletedItem.getKey(), localDeletedItem);
            localDeletedItem = src.getNextDeletedItem();
        }
    }

    private void prepareSyncFullUpload(SyncSource src, MappingTable mapping,
                                       int downloadSyncMode, boolean incrementalDownload, Hashtable twins)
    throws SyncException, JSONException {

        // In a full upload we need to know all the server items in order to
        // detect twins and avoid duplicates.
        // We do it only if we don't already perform a full download in this sync

        if (incrementalDownload || downloadSyncMode == SyncSource.NO_SYNC) {
            int offset = 0;
            boolean done = false;
            do {
                SapiSyncHandler.FullSet fullSet = sapiSyncHandler.getItems(
                        src.getConfig().getRemoteUri(), getDataTag(src), null,
                        Integer.toString(FULL_SYNC_DOWNLOAD_LIMIT),
                        Integer.toString(offset), null);
                if (fullSet != null && fullSet.items != null && fullSet.items.length() > 0) {
                    // This will find all the twins that will be skipped during the upload
                    discardTwinAndConflictFromList(src, fullSet.items, null, null,
                            fullSet.serverUrl, mapping, twins, true);
                    offset += fullSet.items.length();
                    if ((fullSet.items.length() < FULL_SYNC_DOWNLOAD_LIMIT)) {
                        done = true;
                    }
                } else {
                    done = true;
                }
            } while(!done);
        }
    }

    private void finalizePreparePhase(SyncSource src, MappingTable mapping, Hashtable twins,
                                      boolean incrementalDownload, boolean incrementalUpload)
    throws JSONException {

        // Now we have all the required information to decide what we need
        // to download/upload. It is here that we resolve conflicts and
        // twins
        if (addedArray != null) {
            // The server has items to send.

            // First of all check if this command is a real add or an update
            for(int i=0;i<addedArray.length();++i) {
                JSONObject item = addedArray.getJSONObject(i);
                String     guid = item.getString(SapiSyncManager.ID_FIELD);

                String localItemId = mapping.getLuid(guid);

                if (localItemId != null) {

                    ItemComparisonResult equal = compareItems(item, mapping);

                    if (equal.getIdentical()) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Server sent an add which already exists on client, ignore it");
                        }
                        addedArray.put(i, removedItemMarker);
                    } else {
                        // This is rather an update because the guid is already
                        // in the mapping table
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Turning an add into an update");
                        }

                        // Nullify this item
                        addedArray.put(i, removedItemMarker);
                        if (updatedArray == null) {
                            updatedArray = new JSONArray();
                            updatedServerUrl = addedServerUrl;
                        }
                        updatedArray.put(item);

                        // Update the item's properties according to what
                        // changed in this update
                        setUpdatedProperties(item, equal, mapping.getLuid(guid));
                    }
                } else {
                    // On the first sync after an upgrade, the mapping is empty
                    // because in old sync engine we did not retain mapping info
                    // Some items of the last sync can still be reported as part
                    // of the change set, we need to skip them by using twin
                    // detection
                    if (src instanceof TwinDetectionSource) {
                        TwinDetectionSource tds = (TwinDetectionSource)src;
                        JSONSyncItem jItem = new JSONSyncItem(guid, src.getType(), SyncItem.STATE_NEW, null, item, null);
                        if (tds.findTwin(jItem) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Found a twin item, ignoring its add " + guid);
                            }
                            addedArray.put(i, removedItemMarker);
                        }
                    }
                }
            }
        }

        // Now check if there is any update which is not an update, but
        // just an add instead (i.e. it does not exist in our mapping)
        if (updatedArray != null) {
            for(int i=0;i<updatedArray.length();++i) {
                JSONObject item = updatedArray.getJSONObject(i);
                String     guid = item.getString(SapiSyncManager.ID_FIELD);

                String localItemId = mapping.getLuid(guid);

                if (localItemId == null) {

                    // On the first sync after an upgrade, the mapping is empty
                    // because in old sync engine we did not retain mapping info
                    // Some items of the last sync can still be reported as part
                    // of the change set, we need to skip them by using twin
                    // detection
                    boolean twin = false;
                    if (src instanceof TwinDetectionSource) {
                        JSONSyncItem jItem = new JSONSyncItem(guid, src.getType(), SyncItem.STATE_NEW, null, item, null);
                        TwinDetectionSource tds = (TwinDetectionSource)src;
                        if (tds.findTwin(jItem) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Found a twin item, ignoring its update " + guid);
                            }
                            updatedArray.put(i, removedItemMarker);
                            twin = true;
                        }
                    }

                    if (!twin) {
                        // This is rather an add because the guid is not in the
                        // mapping table
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Turning an update into an add");
                        }
                        // Nullify this item
                        updatedArray.put(i, removedItemMarker);
                        if (addedArray == null) {
                            addedArray = new JSONArray();
                            addedServerUrl = updatedServerUrl;
                        }
                        addedArray.put(item);
                    }
                } else {
                    ItemComparisonResult equal = compareItems(item, mapping);

                    if (equal.getIdentical()) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Server sent an update for an item already on the client, ignore it");
                        }
                        updatedArray.put(i, removedItemMarker);
                    } else {
                        // Update the item's properties according to what
                        // changed in this update
                        setUpdatedProperties(item, equal, mapping.getLuid(guid));
                    }
                }
            }
        }

        // Now check the added/updated lists searching for twins and
        // conflicts
        // If either full upload or download is needed, then we perform a deep
        // twin search
        boolean deepTwinSearch = !incrementalDownload || !incrementalUpload;
        if (addedArray != null) {
            discardTwinAndConflictFromList(src, addedArray, localUpdated,
                    localDeleted, addedServerUrl, mapping, twins, deepTwinSearch);
        }
        if (updatedArray != null) {
            discardTwinAndConflictFromList(src, updatedArray, localUpdated,
                    localDeleted, updatedServerUrl, mapping, twins, deepTwinSearch);
        }
        if (deletedArray != null) {
            handleServerDeleteConflicts(src, deletedArray, localUpdated,
                    localDeleted, localRenamed, mapping);
        }
    }

    private void prepareSyncFullDownload(SyncSource src, MappingTable mapping, Hashtable twins)
    throws SyncException, JSONException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "prepareSyncFullDownload");
        }

        String remoteUri = src.getConfig().getRemoteUri();
        SyncFilter syncFilter = src.getFilter();

        int totalCount = -1;
        int filterMaxCount = -1;
        long filterFrom = -1;

        Filter fullDownloadFilter = null;
        if(syncFilter != null) {
            fullDownloadFilter = syncFilter.getFullDownloadFilter();

            if(fullDownloadFilter != null) {
                if(fullDownloadFilter != null && fullDownloadFilter.isEnabled() &&
                        fullDownloadFilter.getType() == Filter.ITEMS_COUNT_TYPE) {
                    filterMaxCount = fullDownloadFilter.getCount();
                } else if(fullDownloadFilter != null && fullDownloadFilter.getType()
                        == Filter.DATE_RECENT_TYPE) {

                    // This filter specifies a client based timestamp. We need
                    // to correct it according to the time difference between
                    // client and server
                    filterFrom = fullDownloadFilter.getDate();
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Adjusting from filter by " + clientServerTimeDifference);
                    }
                    filterFrom += clientServerTimeDifference;
                } else {
                    throw new UnsupportedOperationException("Not implemented yet");
                }
            }
        }

        if (filterMaxCount != -1) {
            // Get the number of items and notify the listener
            try {
                totalCount = sapiSyncHandler.getItemsCount(remoteUri, null);
            } catch (SapiException e) {
                String errorMessage = "Cannot perform a full sync";
                utils.processCommonSapiExceptions(e, errorMessage, false);
                utils.processCustomSapiExceptions(e, errorMessage, true);
            }
        }

        // Fill the addedArray
        addedArray = null;
        addedServerUrl = null;

        int downloadLimit = FULL_SYNC_DOWNLOAD_LIMIT;
        String dataTag = getDataTag(src);
        int offset = 0;
        boolean done = false;

        downloadNextAnchor = -1;
        do {
            // Update the download limit given the total amount of items
            // to download
            if(totalCount > 0 && (offset + downloadLimit) > totalCount) {
                downloadLimit = totalCount - offset;
            }

            // We need to get all items on the server to be able to do effective
            // twin detection.
            SapiSyncHandler.FullSet fullSet = sapiSyncHandler.getItems(remoteUri, dataTag, null,
                    Integer.toString(downloadLimit),
                    Integer.toString(offset), null);
            if (downloadNextAnchor == -1) {
                downloadNextAnchor = fullSet.timeStamp;
                addedServerUrl = fullSet.serverUrl;
            }
            if (fullSet != null && fullSet.items != null && fullSet.items.length() > 0) {
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "items = " + fullSet.items.toString());
                }

                if (addedArray == null) {
                    addedArray = new JSONArray();
                }

                // Search and discard twins, as there is no need to download
                // them again
                discardTwinAndConflictFromList(src, fullSet.items, null, null,
                        fullSet.serverUrl, mapping, twins, true);

                for(int i=0;i<fullSet.items.length();++i) {
                    JSONObject item = fullSet.items.getJSONObject(i);
                    if (item != removedItemMarker) {
                        // Apply items count filter
                        if(filterMaxCount > 0) {
                            if(addedArray.length() >= filterMaxCount) {
                                if (Log.isLoggable(Log.DEBUG)) {
                                    Log.debug(TAG_LOG, "The source doesn't accept more items");
                                }
                                done = true;
                                break;
                            }
                        }
                        // Apply filterfrom filter
                        boolean skip = false;
                        if (item.has(SapiSyncManager.UPLOAD_DATE_FIELD)) {

                            long uploadDate = item.getLong(SapiSyncManager.UPLOAD_DATE_FIELD);
                            if (uploadDate < filterFrom) {
                                skip = true;
                            }
                        }

                        if (skip) {
                            if (Log.isLoggable(Log.DEBUG)) {
                                Log.debug(TAG_LOG, "Ignoring item because out of date filter");
                            }
                        } else {
                            addedArray.put(item);
                        }
                    }
                }

                offset += fullSet.items.length();
                if ((fullSet.items.length() < FULL_SYNC_DOWNLOAD_LIMIT)) {
                    done = true;
                }
            } else {
                done = true;
            }
        } while(!done);
    }

    private void handleServerDeleteConflicts(SyncSource src, JSONArray serverDeletes,
                                             Hashtable localMods, Hashtable localDel,
                                             Hashtable localRenamed, MappingTable mapping)
    throws JSONException
    {
        for(int i=0;i<serverDeletes.length();++i) {
            String guid = serverDeletes.getString(i);
            String luid = mapping.getLuid(guid);
            if (luid != null) {
                // Check if we have a local update or delete for this item
                if (localMods != null && localMods.get(luid) != null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Found a server delete local update conflict, client wins");
                    }
                    serverDeletes.put(i, "");
                } else if (localDel != null && localDel.get(luid) != null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Found a server delete local delete conflict, ignore server delete");
                    }
                    serverDeletes.put(i, "");
                } else if (localRenamed != null && localRenamed.get(luid) != null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Found a server delete local rename conflict, ignore server delete");
                    }
                    serverDeletes.put(i, "");
                }
            }
        }
    }

    private void discardTwinAndConflictFromList(SyncSource src, JSONArray items,
                                                Hashtable localMods, Hashtable localDel,
                                                String serverUrl, MappingTable mapping,
                                                Hashtable twins, boolean deepTwinSearch)
    throws JSONException
    {
        if (src instanceof TwinDetectionSource) {
            for(int i=0;i<items.length();++i) {
                JSONObject item = items.getJSONObject(i);
                if (item != removedItemMarker) {
                    // If a twin search is needed, then we perform it here
                    String guid = item.getString(SapiSyncManager.ID_FIELD);
                    if (deepTwinSearch) {
                        long   size = Long.parseLong(item.getString(SapiSyncManager.SIZE_FIELD));
                        SyncItem syncItem = utils.createSyncItem(src, guid, SyncItem.STATE_NEW, size, item, serverUrl);
                        syncItem.setGuid(guid);
                        TwinDetectionSource twinSource = (TwinDetectionSource)src;
                        SyncItem twin = twinSource.findTwin(syncItem);
                        if (twin != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Found a twin for incoming command, ignoring it " + guid);
                            }
                            items.put(i, removedItemMarker);
                            // This item exists already on client and server. We
                            // don't need to upload it again. This shall change once
                            // we support updates
                            twins.put(twin.getKey(), twin);
                        }
                    } else {
                        // We simply check if we have this same item in the
                        // mapping already
                    }
                    // Now we check if the client has a pending delete for this
                    // item. If an item is scheduled for deletion, then its id
                    // must be in the mapping, so we can get its luid
                    if (mapping != null) {
                        String luid = mapping.getLuid(guid);

                        if (luid != null && localDel != null && localDel.get(luid) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Conflict detected, item sent by the server has been deleted "
                                                  + "on client. Receiving again " + luid);
                            }
                            if (item.has("nocontent")) {
                                // Since the item was locally removed, we shall
                                // remove the nocontent property and download
                                // the content once again (we must also ignore
                                // renaming as this is just like a new add)
                                item.remove("nocontent");
                                item.remove("oldkey");
                            }
                        } else if (luid != null && localMods != null && localMods.get(luid) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Conflict detected, item modified both on client and server side " + luid);
                                Log.info(TAG_LOG, "The most recent change shall win");
                            }
                            JSONSyncItem localItem = (JSONSyncItem)localMods.get(luid);
                            long localLastMod = localItem.getLastModified();
                            long remoteLastMod;
                            if (item.has(SapiSyncManager.UPLOAD_DATE_FIELD)) {
                               remoteLastMod = item.getLong(SapiSyncManager.UPLOAD_DATE_FIELD);
                            } else {
                                remoteLastMod = -1;
                            }

                            if (localLastMod == -1 || remoteLastMod ==  -1) {
                                if (Log.isLoggable(Log.INFO)) {
                                    Log.info(TAG_LOG, "No local or remote modification timestamp available. Client wins");
                                }
                                items.put(i, removedItemMarker);
                            } else {
                                // Pick the most recent one
                                localLastMod += clientServerTimeDifference;
                                if (Log.isLoggable(Log.INFO)) {
                                    Log.info(TAG_LOG, "Comparing local last mod " + localLastMod +
                                                      " with remote last mod " + remoteLastMod);
                                }
                                if (localLastMod > remoteLastMod) {
                                    // Client wins
                                    if (Log.isLoggable(Log.INFO)) {
                                        Log.info(TAG_LOG, "Client wins");
                                    }
                                    items.put(i, removedItemMarker);
                                } else {
                                    // Server wins
                                    if (Log.isLoggable(Log.INFO)) {
                                        Log.info(TAG_LOG, "Server wins");
                                    }
                                    localMods.remove(luid);
                                }
                            }
                        } else if (luid != null && localRenamed != null && localRenamed.get(luid) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Conflict detected, item renamed on client and modified on server " + luid);
                                Log.info(TAG_LOG, "Client wins");
                            }
                            items.put(i, removedItemMarker);
                        }
                    }
                }
            }
        }
    }

    private SapiSyncHandler.FullSet fetchItemsInfo(SyncSource src, JSONArray items)
    throws JSONException
    {
        SapiSyncHandler.FullSet fullSet = null;
        if (items != null) {
            String dataTag = getDataTag(src);
            JSONArray itemsId = new JSONArray();
            for(int i=0;i<items.length();++i) {
                int id = Integer.parseInt(items.getString(i));
                itemsId.put(id);
            }
            if (itemsId.length() > 0) {
                // Ask for these items
                fullSet = sapiSyncHandler.getItems(src.getConfig().getRemoteUri(), dataTag,
                        itemsId, null, null, null);
                if (fullSet != null && fullSet.items != null) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "items = " + fullSet.items.toString());
                    }
                }
            }
        }
        return fullSet;
    }

    private String getDataTag(SyncSource src) {
        String dataTag = null;
        if (src instanceof JSONSyncSource) {
            JSONSyncSource jsonSyncSource = (JSONSyncSource)src;
            dataTag = jsonSyncSource.getDataTag();
        }
        if (dataTag == null) {
            // This is the default value
            dataTag = src.getConfig().getRemoteUri() + "s";
        }
        return dataTag;
    }

    private ItemComparisonResult compareItems(JSONObject item, MappingTable mapping) throws JSONException {
        String guid = item.getString(SapiSyncManager.ID_FIELD);
        String remoteCRC  = item.getString(SapiSyncManager.CRC_FIELD);
        String localCRC   = mapping.getCRC(guid);
        String localName  = mapping.getName(guid);
        String remoteName = item.getString(SapiSyncManager.NAME_FIELD);

        // We need to know if the content or the metadata changed
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Comparing items corresponding to id " + guid);
            Log.debug(TAG_LOG, "Local name " + localName + " local CRC " + localCRC);
            Log.debug(TAG_LOG, "Remote name " + remoteName + " remote CRC " + remoteCRC);
        }

        boolean contentEqual = (localCRC != null && localCRC.equals(remoteCRC));
        boolean metaEqual    = (localName != null && localName.equals(remoteName));

        return new ItemComparisonResult(contentEqual, metaEqual);
    }

    private void setUpdatedProperties(JSONObject item, ItemComparisonResult equal, String oldKey) throws JSONException {
        // Set the content change properties
        if (equal.getContentEqual()) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "This update did not change the item content");
            }
            item.put("nocontent", true);
        }
        if (!equal.getMetaEqual()) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "This update changed the item metadata");
            }
            item.put("oldkey", oldKey);
        }
    }

    private class ItemComparisonResult {

        private boolean contentEqual;
        private boolean metaEqual;

        public ItemComparisonResult(boolean contentEqual, boolean metaEqual) {
            this.contentEqual = contentEqual;
            this.metaEqual    = metaEqual;
        }

        public boolean getContentEqual() {
            return contentEqual;
        }

        public boolean getMetaEqual() {
            return metaEqual;
        }

        public boolean getIdentical() {
            return getContentEqual() && getMetaEqual();
        }
    }
}
