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

import java.util.Vector;
import java.util.Hashtable;
import java.util.Date;
import java.util.Enumeration;
import java.io.OutputStream;
import java.io.IOException;

import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;
import com.funambol.sync.BasicSyncListener;
import com.funambol.sync.ItemStatus;
import com.funambol.sync.SyncAnchor;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SyncSource;
import com.funambol.sync.ResumableSource;
import com.funambol.sync.SyncManagerI;
import com.funambol.sync.TwinDetectionSource;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.sync.Filter;
import com.funambol.sync.SyncFilter;
import com.funambol.sync.DeviceConfigI;
import com.funambol.sync.NonBlockingSyncException;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

public class SapiSyncStrategy {

    private static final String TAG_LOG = "SapiSyncStrategy";

    private static final int FULL_SYNC_DOWNLOAD_LIMIT = 300;

    private JSONArray addedArray   = null;
    private JSONArray updatedArray = null;
    private JSONArray deletedArray = null;

    private Hashtable localUpdated;
    private Hashtable localDeleted;
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
                            boolean resume, StringKeyValueStore mapping, boolean incrementalDownload,
                            boolean incrementalUpload, Vector twins)
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
        }

        // Resolve conflicts
        finalizePreparePhase(src, mapping, twins);
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

    private void prepareSyncIncrementalDownload(SyncSource src, StringKeyValueStore mapping, Vector twins)
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

    private void prepareSyncIncrementalUpload(SyncSource src, StringKeyValueStore mapping, Vector twins)
    throws SyncException, JSONException
    {
        localUpdated = new Hashtable();
        localDeleted = new Hashtable();

        SyncItem localUpdatedItem = src.getNextUpdatedItem();
        while(localUpdatedItem != null) {
            localUpdated.put(localUpdatedItem.getKey(), localUpdatedItem);
            localUpdatedItem = src.getNextUpdatedItem();
        }

        SyncItem localDeletedItem = src.getNextDeletedItem();
        while(localDeletedItem != null) {
            localDeleted.put(localDeletedItem.getKey(), localDeletedItem);
            localDeletedItem = src.getNextDeletedItem();
        }
    }

    private void prepareSyncFullUpload(SyncSource src, StringKeyValueStore mapping,
                                       int downloadSyncMode, boolean incrementalDownload, Vector twins)
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
                            fullSet.serverUrl, mapping, twins);
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

    private void finalizePreparePhase(SyncSource src, StringKeyValueStore mapping, Vector twins)
    throws JSONException {

        // Now we have all the required information to decide what we need
        // to download/upload. It is here that we resolve conflicts and
        // twins
        if (addedArray != null) {
            // The server has items to send.

            // First of all check if this command is a real add or an update
            for(int i=0;i<addedArray.length();++i) {
                JSONObject item = addedArray.getJSONObject(i);
                String     guid = item.getString("id");

                if (mapping.get(guid) != null) {
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
                }
            }
        }

        // Now check if there is any update which is not an update, but
        // just an add instead (i.e. it does not exist in our mapping)
        if (updatedArray != null) {
            for(int i=0;i<updatedArray.length();++i) {
                JSONObject item = updatedArray.getJSONObject(i);
                String     guid = item.getString("id");

                if (mapping.get(guid) == null) {
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
            }
        }

        // Now check the added/updated lists searching for twins and
        // conflicts
        if (addedArray != null) {
            discardTwinAndConflictFromList(src, addedArray, localUpdated,
                    localDeleted, addedServerUrl, mapping, twins);
        }
        if (updatedArray != null) {
            discardTwinAndConflictFromList(src, updatedArray, localUpdated,
                    localDeleted, updatedServerUrl, mapping, twins);
        }
        /*
        if (deletedArray != null) {
            discardTwinAndConflictFromList(src, deletedArray, localUpdated,
                     localDeleted, updatedServerUrl, mapping, twins);
        }
        */
    }

    private void prepareSyncFullDownload(SyncSource src, StringKeyValueStore mapping, Vector twins)
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

        // Get the number of items and notify the listener
        try {
            totalCount = sapiSyncHandler.getItemsCount(remoteUri, null);
        } catch (SapiException e) {
            String errorMessage = "Cannot perform a full sync";
            utils.processCommonSapiExceptions(e, errorMessage, false);
            utils.processCustomSapiExceptions(e, errorMessage, true);
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
            if((offset + downloadLimit) > totalCount) {
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
                        fullSet.serverUrl, mapping, twins);

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
                        if (item.has("date")) {

                            long creationDate = item.getLong("date");
                            if (creationDate < filterFrom) {
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

    private void discardTwinAndConflictFromList(SyncSource src, JSONArray items,
                                                Hashtable localMods, Hashtable localDel,
                                                String serverUrl, StringKeyValueStore mapping,
                                                Vector twins)
    throws JSONException
    {
        if (src instanceof TwinDetectionSource) {
            for(int i=0;i<items.length();++i) {
                JSONObject item = items.getJSONObject(i);
                if (item != removedItemMarker) {
                    // First of all we check if we already have the very same item
                    String guid = item.getString("id");
                    long   size = Long.parseLong(item.getString("size"));
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
                        twins.addElement(twin.getKey());
                    }
                    // Now we check if the client has a pending delete for this
                    // item. If an item is scheduled for deletion, then its id
                    // must be in the mapping, so we can get its luid
                    if (mapping != null) {
                        String luid = (String)mapping.get(guid);
                        if (luid != null && localDel != null && localDel.get(luid) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Conflict detected, item sent by the server has been deleted on client. Ignoring " + luid);
                            }
                            items.put(i, removedItemMarker);
                        } else if (luid != null && localMods != null && localMods.get(luid) != null) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Conflict detected, item modified both on client and server side " + luid);
                                Log.info(TAG_LOG, "The most recent change shall win");
                            }
                            // TODO: get the last changed information on both sides
                            JSONSyncItem localItem = (JSONSyncItem)localMods.get(luid);
                            long localLastMod = localItem.getLastModified();
                            // TODO: adjust times by the // clientServerTimeDifference
                            if (localLastMod == -1) {
                                if (Log.isLoggable(Log.INFO)) {
                                    Log.info(TAG_LOG, "No local modification timestamp available. Client wins");
                                }
                                items.put(i, removedItemMarker);
                            }

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



}
