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
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.sync.Filter;
import com.funambol.sync.SyncFilter;
import com.funambol.sync.DeviceConfigI;
import com.funambol.sync.NonBlockingSyncException;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

/**
 * <code>SapiSyncManager</code> represents the synchronization engine performed
 * via SAPI.
 */
public class SapiSyncManager implements SyncManagerI {

    private static final String TAG_LOG = "SapiSyncManager";

    private static final int FULL_SYNC_DOWNLOAD_LIMIT = 300;

    private SyncConfig syncConfig = null;
    private SapiSyncHandler sapiSyncHandler = null;
    private SapiSyncStatus syncStatus = null;
    private String deviceId = null;

    // Holds the list of twins found during the download phase, those items must
    // not be uploaded to the server later in the upload phase
    private Vector twins = null;

    /**
     * Unique instance of a BasicSyncListener which is used when the user does
     * not set up a listener in the SyncSource. In order to avoid the creation
     * of multiple instances of this class we use this static variable
     */
    private static SyncListener basicListener = null;

    /**
     * This is the flag used to indicate that the sync shall be cancelled. Users
     * can call the cancel (@see cancel) method to cancel the current sync
     */
    private boolean cancel;

    private SyncSource currentSource = null;

    private JSONArray addedArray   = null;
    private JSONArray updatedArray = null;
    private JSONArray deletedArray = null;

    private String addedServerUrl = null;
    private String updatedServerUrl = null;

    private long downloadNextAnchor;

    private Hashtable localUpdated;
    private Hashtable localDeleted;

    private static final JSONObject REMOVED_ITEM = new JSONObject();

    private long clientServerTimeDifference = 0;


    /**
     * <code>SapiSyncManager</code> constructor
     * @param config
     */
    public SapiSyncManager(SyncConfig config, DeviceConfigI devConfig) {
        this.syncConfig = config;
        this.sapiSyncHandler = new SapiSyncHandler(
                StringUtil.extractAddressFromUrl(syncConfig.getSyncUrl()),
                syncConfig.getUserName(),
                syncConfig.getPassword());
        this.deviceId = devConfig.getDevID();
    }

    /**
     * Force a specific SapiSyncHandler to be used for testing purposes.
     * @param sapiSyncHandler
     */
    public void setSapiSyncHandler(SapiSyncHandler sapiSyncHandler) {
        this.sapiSyncHandler = sapiSyncHandler;
    }

    /**
     * Synchronizes the given source, using the preferred sync mode defined for
     * that SyncSource.
     *
     * @param source the SyncSource to synchronize
     *
     * @throws SyncException If an error occurs during synchronization
     *
     */
    public void sync(SyncSource source) throws SyncException {
        sync(source, source.getSyncMode(), false);
    }

    public void sync(SyncSource source, boolean askServerDevInf) throws SyncException {
        sync(source, source.getSyncMode(), askServerDevInf);
    }

    public void sync(SyncSource src, int syncMode) {
        sync(src, syncMode, false);
    }

    /**
     * Synchronizes the given source, using the given sync mode.
     *
     * @param source the SyncSource to synchronize
     * @param syncMode the sync mode
     * @param askServerDevInf true if the engine shall ask for server caps
     *
     * @throws SyncException If an error occurs during synchronization
     */
    public synchronized void sync(SyncSource src, int syncMode, boolean askServerDevInf)
    throws SyncException {

        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Starting sync");
        }

        cancel = false;

        if (basicListener == null) {
            basicListener = new BasicSyncListener();
        }

        currentSource = src;

        // JSONSyncSource require an updated sync config, therefore we update it
        // at the beginning of each sync
        if (src instanceof JSONSyncSource) {
            JSONSyncSource jsonSyncSource = (JSONSyncSource)src;
            jsonSyncSource.updateSyncConfig(syncConfig);
        }
        
        boolean resume = false;

        syncStatus = new SapiSyncStatus(src.getName());
        try {
            syncStatus.load();
            // If the sync was interrupted, then we shall resume
            resume = syncStatus.getInterrupted();
        } catch (Exception e) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Cannot load sync status, use an empty one");
            }
        }

        // If we are not resuming, then we can reset the status
        if (resume) {
            // We need to understand where the sync got stopped and if a single item
            // shall be resumed
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Resume is active");
            }
        } else {
            try {
                syncStatus.reset();
                syncStatus.setInterrupted(true);
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot reset status", ioe);
            }
        }

        StringKeyValueStoreFactory mappingFactory =
                StringKeyValueStoreFactory.getInstance();
        StringKeyValueStore mapping = mappingFactory.getStringKeyValueStore(
                "mapping_" + src.getName());
        try {
            mapping.load();
        } catch (Exception e) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "The mapping store does not exist, use an empty one");
            }
        }
        // Init twins vector
        twins = new Vector();
        try {
            
            Throwable downloadNonBlockingError = null;
            Throwable uploadNonBlockingError = null;
            
            // Set the basic properties in the sync status
            syncStatus.setRemoteUri(src.getConfig().getRemoteUri());
            syncStatus.setInterrupted(true);
            syncStatus.setLocUri(src.getName());
            syncStatus.setRequestedSyncMode(syncMode);
            syncStatus.setStartTime(System.currentTimeMillis());
            syncStatus.save();

            getSyncListenerFromSource(src).startSession();
            getSyncListenerFromSource(src).startConnecting();

            cancelSyncIfNeeded(src);

            performInitializationPhase(src, syncMode, resume, mapping);

            cancelSyncIfNeeded(src);

            getSyncListenerFromSource(src).syncStarted(getActualSyncMode(src, syncMode));

            if(isDownloadPhaseNeeded(syncMode)) {
                try {
                    // The download anchor is updated once it is received from the server
                    performDownloadPhase(src, getActualDownloadSyncMode(src), resume, mapping);
                } catch (NonBlockingSyncException nbse) {
                    // Carries on
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, 
                                "Caught a non blocking exception (code " + nbse.getCode() + 
                                ") during download, sync will continue");
                    }
                    downloadNonBlockingError = nbse;
                }
            }

            cancelSyncIfNeeded(src);
            
            if(isUploadPhaseNeeded(syncMode)) {
                try {
                    long newUploadAnchor = (new Date()).getTime();
                    performUploadPhase(src, getActualUploadSyncMode(src), resume, mapping);
                    // If we had no error so far, then we update the anchor
                    SapiSyncAnchor anchor = (SapiSyncAnchor)src.getSyncAnchor();
                    anchor.setUploadAnchor(newUploadAnchor);
                    
                // This catch block is not used so far, but it is here in case in the future we introduce
                // non-blocking exceptions in the upload phase too.
                } catch (NonBlockingSyncException nbse) {
                    // Carries on
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, 
                                "Caught a non blocking exception (code " + nbse.getCode() + 
                                ") during upload, sync will continue");
                    }
                    uploadNonBlockingError = nbse;
                }
            }

            cancelSyncIfNeeded(src);

            performFinalizationPhase(src);

            if (uploadNonBlockingError != null) { // upload errors prevail
                throw uploadNonBlockingError;
            } else if (downloadNonBlockingError != null) {
                throw downloadNonBlockingError;
            } else {
                syncStatus.setInterrupted(false);
                syncStatus.setStatusCode(SyncListener.SUCCESS);
            }

        // This catch block can be reached directly when sync-blocking exceptions are thrown, or
        // indirectly when non-blocking exceptions are re-thrown at the end of the sync (a few lines 
        // above).
        } catch (Throwable t) {
            Log.error(TAG_LOG, "Error while synchronizing", t);
            syncStatus.setSyncException(t);
            
            SyncException se;
            if (t instanceof SyncException) {
                se = (SyncException)t;
            } else {
                se = new SyncException(SyncException.CLIENT_ERROR, "Generic error");
            }
            int syncStatusCode = getListenerStatusFromSyncException(se);
            syncStatus.setStatusCode(syncStatusCode);
            throw se;
        } finally {
            try {
                mapping.save();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot save mapping store", e);
            }
            syncStatus.setEndTime(System.currentTimeMillis());
            try {
                syncStatus.save();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot save sync status", ioe);
            }
            // We must guarantee this method is invoked in all cases
            getSyncListenerFromSource(src).endSession(syncStatus);

            currentSource = null;
        }
    }

    public void cancel() {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Cancelling sync");
        }
        cancel = true;
        sapiSyncHandler.cancel();
        if(currentSource != null) {
            currentSource.cancel();
        }
    }

    private void cancelSyncIfNeeded(SyncSource src) throws SyncException {
        if(cancel) {
            performFinalizationPhase(null);
            throw new SyncException(SyncException.CANCELLED, "Sync got cancelled");
        }
    }

    private void performInitializationPhase(SyncSource src, int syncMode, boolean resume,
                                            StringKeyValueStore mapping)
    throws SyncException, JSONException
    {
        // Prepare the source for the sync
        src.beginSync(getActualSyncMode(src, syncMode), resume);

        try {
            // Perform a login to avoid multiple authentications
            long clientNow = System.currentTimeMillis();
            long serverNow = sapiSyncHandler.login(deviceId);
            if (serverNow > 0) {
                clientServerTimeDifference = serverNow - clientNow;
                if (Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Difference in time between server and client is " + clientServerTimeDifference);
                }
            } else {
                clientServerTimeDifference = 0;
            }
        } catch (SapiException e) {
            String errorMessage = "Cannot login";
            processCommonSapiExceptions(e, errorMessage, false);
            processCustomSapiExceptions(e, errorMessage, false);

            throw new SyncException(SyncException.AUTH_ERROR, errorMessage);
        }

        int downloadSyncMode = SyncSource.NO_SYNC;
        int uploadSyncMode   = SyncSource.NO_SYNC;

        if (isDownloadPhaseNeeded(syncMode)) {
            downloadSyncMode = getActualDownloadSyncMode(src);
        }

        if (isUploadPhaseNeeded(syncMode)) {
            uploadSyncMode   = getActualUploadSyncMode(src);
        }

        prepareSync(src, downloadSyncMode, uploadSyncMode, resume, mapping);
    }

    /**
     * This method computes the set of items to be downloaded from the server in
     * an incremental sync.
     * The set of data to be downloaded depends on many things, including the
     * changes made locally. After this method the src update/delete items have
     * been consumed and the getNextNewItem and getNextUpdItem will return null.
     */
    private void prepareSync(SyncSource src, int downloadSyncMode, int uploadSyncMode,
                             boolean resume, StringKeyValueStore mapping)
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
            boolean incremental = isIncrementalSync(downloadSyncMode);
            if (incremental) {
                prepareSyncIncrementalDownload(src, mapping);
            } else {
                prepareSyncFullDownload(src, mapping);
            }
        }

        // Get all the info on what changes are to be sent
        if (uploadSyncMode != SyncSource.NO_SYNC) {
            boolean incremental = isIncrementalSync(uploadSyncMode);
            if (incremental) {
                prepareSyncIncrementalUpload(src, mapping);
            } else {
                prepareSyncFullUpload(src, mapping, downloadSyncMode);
            }
        } else {
            localUpdated = null;
            localDeleted = null;
        }

        // Resolve conflicts
        finalizePreparePhase(src, mapping);
    }

    private void prepareSyncIncrementalDownload(SyncSource src, StringKeyValueStore mapping)
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
            processCommonSapiExceptions(e, errorMessage, false);
            processCustomSapiExceptions(e, errorMessage, true);
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

    private void prepareSyncIncrementalUpload(SyncSource src, StringKeyValueStore mapping)
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
            int downloadSyncMode) throws SyncException, JSONException {

        // In a full upload we need to know all the server items in order to
        // detect twins and avoid duplicates.
        // We do it only if we don't already perform a full download in this sync

        if(isIncrementalSync(downloadSyncMode) || downloadSyncMode == SyncSource.NO_SYNC) {
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
                            fullSet.serverUrl, mapping);
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

    private void finalizePreparePhase(SyncSource src, StringKeyValueStore mapping)
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
                    addedArray.put(i, REMOVED_ITEM);
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
                    updatedArray.put(i, REMOVED_ITEM);
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
                    localDeleted, addedServerUrl, mapping);
        }
        if (updatedArray != null) {
            discardTwinAndConflictFromList(src, updatedArray, localUpdated,
                    localDeleted, updatedServerUrl, mapping);
        }
    }

    private void prepareSyncFullDownload(SyncSource src, StringKeyValueStore mapping)
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
            processCommonSapiExceptions(e, errorMessage, false);
            processCustomSapiExceptions(e, errorMessage, true);
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
                        fullSet.serverUrl, mapping);

                for(int i=0;i<fullSet.items.length();++i) {
                    JSONObject item = fullSet.items.getJSONObject(i);
                    if (item != REMOVED_ITEM) {
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
                                                String serverUrl, StringKeyValueStore mapping)
    throws JSONException
    {
        if (src instanceof TwinDetectionSource) {
            for(int i=0;i<items.length();++i) {
                JSONObject item = items.getJSONObject(i);
                if (item != REMOVED_ITEM) {
                    // First of all we check if we already have the very same item
                    String guid = item.getString("id");
                    long   size = Long.parseLong(item.getString("size"));
                    SyncItem syncItem = createSyncItem(src, guid, SyncItem.STATE_NEW, size, item, serverUrl);
                    syncItem.setGuid(guid);
                    TwinDetectionSource twinSource = (TwinDetectionSource)src;
                    SyncItem twin = twinSource.findTwin(syncItem);
                    if (twin != null) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Found a twin for incoming command, ignoring it " + guid);
                        }
                        items.put(i, REMOVED_ITEM);
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
                            items.put(i, REMOVED_ITEM);
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

    private void performUploadPhase(SyncSource src, int syncMode, 
            boolean resume, StringKeyValueStore mapping) {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Starting upload phase with mode: " + syncMode);
        }
        Vector sourceStatus = new Vector();
        
        boolean incremental = isIncrementalSync(syncMode);

        String remoteUri = src.getConfig().getRemoteUri();

        int totalSending = -1;
        if (incremental) {
            totalSending = src.getClientAddNumber();
        } else {
            totalSending = src.getClientItemsNumber();
        }

        int maxSending = -1;
        // Apply upload filter to the total items count
        SyncFilter syncFilter = src.getFilter();
        if(syncFilter != null) {
            Filter uploadFilter = incremental ? 
                syncFilter.getIncrementalUploadFilter() :
                syncFilter.getFullUploadFilter();
            if(uploadFilter != null && uploadFilter.isEnabled() &&
                    uploadFilter.getType() == Filter.ITEMS_COUNT_TYPE) {
                maxSending = uploadFilter.getCount();
            }
        }

        // Exclude twins from total items count
        totalSending -= twins.size();

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Uploading items count: " + totalSending);
        }

        if(totalSending > 0) {
            if(maxSending > 0 && totalSending > maxSending) {
                totalSending = maxSending;
            }
            getSyncListenerFromSource(src).startSending(totalSending, 0, 0);
        }

        int uploadedCount = 0;
        SyncItem item = getNextItemToUpload(src, incremental);

        try {
            while(item != null && itemsCountFilter(maxSending, uploadedCount)) {
                try {
                    // Exclude twins
                    if(twins.contains(item.getKey())) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Exclude twin item to be uploaded: "
                                    + item.getKey());
                        }
                        sourceStatus.addElement(new ItemStatus(item.getKey(),
                            SyncSource.SUCCESS_STATUS));
                        continue;
                    }
                    // If the item was already sent in a previously interrupted
                    // sync, then we do not send it again
                    boolean uploadDone = false;
                    String remoteKey = null;
                    if (resume) {
                        int itemStatus = syncStatus.getSentItemStatus(item.getKey());
                        if (itemStatus == SyncSource.SUCCESS_STATUS) {
                            // TODO: check if it has changed since then
                            uploadDone = true;
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Skipping upload for " + item.getKey() + " which has been previously uploaded");
                            }
                            // Notify the listener that this upload has already
                            // been done
                            if (item.getState() == SyncItem.STATE_NEW) {
                                getSyncListenerFromSource(src).itemAddSendingStarted(item.getKey(), null, 0);
                                getSyncListenerFromSource(src).itemAddSendingEnded(item.getKey(), null);
                            } else if (item.getState() == SyncItem.STATE_UPDATED) {
                                getSyncListenerFromSource(src).itemReplaceSendingStarted(item.getKey(), null, 0);
                                getSyncListenerFromSource(src).itemReplaceSendingEnded(item.getKey(), null);
                            }
                        } else if (itemStatus == SyncSource.INTERRUPTED_STATUS) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Resuming upload for " + item.getKey());
                            }
                            remoteKey = syncStatus.getSentItemGuid(item.getKey());
                            String origGuid = remoteKey;
                            item.setGuid(remoteKey);
                            try {
                                remoteKey = sapiSyncHandler.resumeItemUpload(item,
                                        remoteUri, getSyncListenerFromSource(src));
                                mapping.add(remoteKey, item.getKey());
                            } catch (SapiException e) {
                                verifyErrorInUploadResponse(e, item, remoteKey, sourceStatus);
                            }
                            // If the returned key is the same as the item guid,
                            // the item has been resumed correctly.
                            if(remoteKey.equals(origGuid)) {
                                syncStatus.addSentResumedItem(item.getKey());
                            }
                            uploadDone = true;
                        }
                    }
                    
                    if (!uploadDone) {
                        try {
                            // Sets the status as interrupted so that if the
                            // client crashes badly we still remember this fact
                            remoteKey = sapiSyncHandler.prepareItemUpload(item, remoteUri);
                            item.setGuid(remoteKey);

                            if (item.getState() == SyncItem.STATE_NEW || item.getState() == SyncItem.STATE_UPDATED) {
                                syncStatus.addSentItem(item.getGuid(), item.getKey(), item.getState(),
                                        SyncSource.INTERRUPTED_STATUS);
                                try {
                                    syncStatus.save();
                                } catch (Exception e) {
                                    Log.error(TAG_LOG, "Cannot save sync status", e);
                                }
                            }

                            // Upload the item to the server
                            sapiSyncHandler.uploadItem(item, remoteUri, getSyncListenerFromSource(src));
                            mapping.add(remoteKey, item.getKey());
                        } catch (SapiException e) {
                            verifyErrorInUploadResponse(e, item, item.getGuid(), sourceStatus);
                        }
                    } 

                    syncStatus.setSentItemStatus(item.getGuid(), item.getKey(), item.getState(), SyncSource.SUCCESS_STATUS);
                    try {
                        syncStatus.save();
                    } catch (Exception e) {
                        Log.error(TAG_LOG, "Cannot save sync status", e);
                    }

                    // Set the item status
                    sourceStatus.addElement(new ItemStatus(item.getKey(),
                            SyncSource.SUCCESS_STATUS));

                } catch(SyncException ex) {
                    //relaunch managed sync exception
                    throw ex;
                } catch(Exception ex) {
                    //generic errors catch, just in case...
                    if(Log.isLoggable(Log.ERROR)) {
                        Log.error(TAG_LOG, "Failed to upload item with key: " +
                                item.getKey(), ex);
                    }
                    sourceStatus.addElement(new ItemStatus(item.getKey(),
                            SyncSource.ERROR_STATUS));
                } finally {
                    uploadedCount++;
                    item = getNextItemToUpload(src, incremental);
                    cancelSyncIfNeeded(src);
                }
            }
            if(incremental) {
                // Updates and deletes are not propagated, return a success status
                // for each item anyway
                if (localUpdated != null) {
                    Enumeration updKeys = localUpdated.keys();
                    while(updKeys.hasMoreElements()) {
                        String updKey = (String)updKeys.nextElement();
                        SyncItem update = (SyncItem)localUpdated.get(updKey);
                        sourceStatus.addElement(new ItemStatus(update.getKey(),
                                SyncSource.SUCCESS_STATUS));
                    }
                }
                if (localDeleted != null) {
                    Enumeration delKeys = localDeleted.keys();
                    while(delKeys.hasMoreElements()) {
                        String delKey = (String)delKeys.nextElement();
                        SyncItem delete = (SyncItem)localDeleted.get(delKey);
                        sourceStatus.addElement(new ItemStatus(delete.getKey(),
                                SyncSource.SUCCESS_STATUS));
                    }
                }
            }
        } finally {
            src.applyItemsStatus(sourceStatus);
        }
    }

    private boolean itemsCountFilter(int max, int current) {
        if(max >= 0) {
            return current < max;
        } else {
            return true;
        }
    }

    private SyncItem getNextItemToUpload(SyncSource src, boolean incremental) {
        if(incremental) {
            return src.getNextNewItem();
        } else {
            return src.getNextItem();
        }
    }

    private void performDownloadPhase(SyncSource src, int syncMode, boolean resume, StringKeyValueStore mapping)
    throws SyncException {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Starting download phase with mode: " + syncMode);
        }
       
        if (syncMode == SyncSource.FULL_DOWNLOAD) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Performing full download");
            }
            if (addedArray != null && addedArray.length() > 0) {
                getSyncListenerFromSource(src).startReceiving(addedArray.length());

                try {
                    applyNewUpdToSyncSource(src, addedArray, SyncItem.STATE_NEW, 
                                            addedServerUrl, mapping, resume);
                        
                    updateDownloadAnchor((SapiSyncAnchor) src.getConfig().getSyncAnchor(), downloadNextAnchor);
                } catch (JSONException je) {
                    Log.error(TAG_LOG, "Cannot parse server data", je);
                    throw new SyncException(SyncException.CLIENT_ERROR, je.toString());
                }
            } else {
                // If there are no items to download we can simply update the
                // download anchor
                updateDownloadAnchor((SapiSyncAnchor) src.getConfig().getSyncAnchor(), downloadNextAnchor);
            }
        } else if (syncMode == SyncSource.INCREMENTAL_DOWNLOAD) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Performing incremental download");
            }

            int total = 0;

            try {
                // Count the number of items to be received
                total += countActualItems(addedArray);
                total += countActualItems(updatedArray);
                total += countActualDeletedItems(deletedArray);

                getSyncListenerFromSource(src).startReceiving(total);

                if (addedArray != null) {
                    applyNewUpdToSyncSource(src, addedArray, SyncItem.STATE_NEW,
                            addedServerUrl, mapping, resume);
                }

                if (updatedArray != null) {
                    applyNewUpdToSyncSource(src, updatedArray, SyncItem.STATE_UPDATED,
                            updatedServerUrl, mapping, resume);
                }

                if (deletedArray != null) {
                    applyDelItems(src, deletedArray, mapping);
                }

                // Tries to update the anchor if everything went well
                if (downloadNextAnchor != -1) {
                    updateDownloadAnchor(
                            (SapiSyncAnchor)src.getConfig().getSyncAnchor(),
                            downloadNextAnchor);
                }
            } catch (JSONException jse) {
                Log.error(TAG_LOG, "Error applying server changes", jse);
                throw new SyncException(SyncException.CLIENT_ERROR, "Error applying server changes");
            }
        }
    }

    private int countActualItems(JSONArray items) throws JSONException{
        int count = 0;
        if (items != null) {
            for(int i=0;i<items.length();++i) {
                JSONObject item = items.getJSONObject(i);
                if (item != REMOVED_ITEM) {
                    count++;
                }
            }
        }
        return count;
    }

    private int countActualDeletedItems(JSONArray items) throws JSONException {
        // At the moment server deletes cannot be filtered out
        if (items != null) {
            return items.length();
        } else {
            return 0;
        }
    }

    /**
     * Apply the given items to the source, returning whether the source can
     * accept further items.
     * 
     * @param src
     * @param items
     * @param state
     * @param mapping
     * @param deepTwinSearch
     * @return
     * @throws SyncException
     * @throws JSONException
     */
    private boolean applyNewUpdToSyncSource(SyncSource src, JSONArray items,
                                            char state, String serverUrl,
                                            StringKeyValueStore mapping, boolean resume)
    throws SyncException, JSONException
    {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "apply new update items to source " + src.getName());
        }

        boolean done = false;
        
        // Apply these changes into the sync source
        Vector sourceItems = new Vector();
        for(int k=0; k<items.length() && !done; ++k) {

            cancelSyncIfNeeded(src);

            JSONObject item = items.getJSONObject(k);

            // We must skip items that were removed by the initialization phase
            if (item == REMOVED_ITEM) {
                continue;
            }

            String guid = item.getString("id");
            long size = Long.parseLong(item.getString("size"));

            String luid;
            if (state == SyncItem.STATE_UPDATED) {
                luid = mapping.get(guid);
            } else {
                // This is an add. If the item is already present in the mapping
                // then this is an add
                luid = guid;
            }

            // If the client doesn't have the luid we change the state to new
            if(StringUtil.isNullOrEmpty(luid)) {
                state = SyncItem.STATE_NEW;
            }

            // Create the item
            SyncItem syncItem = createSyncItem(src, luid, state, size, item, serverUrl);
            syncItem.setGuid(guid);

            // Shall we resume this item download?
            if (resume && syncStatus.getReceivedItemStatus(guid) == SyncSource.INTERRUPTED_STATUS) {
                if (src instanceof ResumableSource) {
                    long partialLength = 0;
                    ResumableSource rss = (ResumableSource)src;
                    partialLength = rss.getPartiallyReceivedItemSize(rss.getLuid(syncItem));
                    if (partialLength > 0) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Found an item whose download can be resumed at " + partialLength);
                        }
                        // Notify the sync status that we are trying to resume
                        syncStatus.addReceivedResumedItem(guid);
                        syncItem.setPartialLength(partialLength);
                    }
                }
            }


            // Notify the listener
            if (state == SyncItem.STATE_NEW) {
                getSyncListenerFromSource(src).itemAddReceivingStarted(luid, null, size);
            } else if(state == SyncItem.STATE_UPDATED) {
                getSyncListenerFromSource(src).itemReplaceReceivingStarted(luid, null, size);
            }

            // Filter downloaded items for JSONSyncSources only
            if(src instanceof JSONSyncSource) {
                if(((JSONSyncSource)src).filterSyncItem(syncItem)) {
                    sourceItems.addElement(syncItem);
                } else {
                    if (Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Item rejected by the source: " + luid);
                    }
                }
            } else {
                sourceItems.addElement(syncItem);
            }
            
            // Notify the listener
            if (state == SyncItem.STATE_NEW) {
                getSyncListenerFromSource(src).itemAddReceivingEnded(luid, null);
            } else if(state == SyncItem.STATE_UPDATED) {
                getSyncListenerFromSource(src).itemReplaceReceivingEnded(luid, null);
            }
        }

        try {

            // If the source supports resuming, then we mark all these items as
            // partially downloaded in the sync status. This way we are able to
            // resume their download in all cases (including a brutal death of
            // the application that cannot throw exceptions)
            if (src instanceof ResumableSource) {
                ResumableSource rss = (ResumableSource)src;
                for(int i=0;i<sourceItems.size();++i) {
                    SyncItem item = (SyncItem)sourceItems.elementAt(i);
                    if (item.getState() == SyncItem.STATE_NEW || item.getState() == SyncItem.STATE_UPDATED) {
                        String luid = rss.getLuid(item);
                        if (luid != null) {
                            syncStatus.addReceivedItem(item.getGuid(), luid, item.getState(), SyncSource.INTERRUPTED_STATUS);
                        }
                    }
                }
                try {
                    syncStatus.save();
                } catch (IOException ioe) {
                    Log.error(TAG_LOG, "Cannot save sync status", ioe);
                }
            }

            // Apply the items in the sync source
            src.applyChanges(sourceItems);
        } finally {
            // The sourceItems returned by the call contains the LUID,
            // so we can create the luid/guid map here
            for(int l=0;l<sourceItems.size();++l) {
                SyncItem newItem = (SyncItem)sourceItems.elementAt(l);
                // Update the sync status for all the items that were processed
                // by the source
                if (newItem.getSyncStatus() != -1 && newItem.getGuid() != null && newItem.getKey() != null) {
                    syncStatus.setReceivedItemStatus(newItem.getGuid(), newItem.getKey(), newItem.getState(), newItem.getSyncStatus());
                    // and the mapping table (if luid and guid are different)
                    if (state == SyncItem.STATE_NEW && !newItem.getGuid().equals(newItem.getKey())) {
                        if (Log.isLoggable(Log.TRACE)) {
                            Log.trace(TAG_LOG, "Updating mapping info for: " +
                                    newItem.getGuid() + "," + newItem.getKey());
                        }
                        mapping.add(newItem.getGuid(), newItem.getKey());
                    }
                }
            }
        }
        return done;
    }

    private SyncItem createSyncItem(SyncSource src, String luid, char state, 
            long size, JSONObject item, String serverUrl) throws JSONException {

        SyncItem syncItem = null;
        if(src instanceof JSONSyncSource) {
            syncItem = ((JSONSyncSource)src).createSyncItem(
                    luid, src.getType(), state, null, item, serverUrl);
        } else {
            // A generic sync item needs to be filled with the json item content
            syncItem = src.createSyncItem(luid, src.getType(), state, null, size);
            OutputStream os = null;
            try {
                os = syncItem.getOutputStream();
                os.write(item.toString().getBytes());
                os.close();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot write into sync item stream", ioe);
                // Ignore this item and continue
            } finally {
                try {
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException ioe) {
                }
            }
        }
        return syncItem;
    }

    private void applyDelItems(SyncSource src, JSONArray removed, 
                               StringKeyValueStore mapping) throws SyncException, JSONException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "applyDelItems");
        }
        Vector delItems = new Vector();
        for(int i=0;i < removed.length();++i) {
            String guid = removed.getString(i);
            String luid = mapping.get(guid);
            if (luid == null) {
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Cannot delete item with unknown luid " + guid);
                }
            } else {
                SyncItem delItem = new SyncItem(luid, src.getType(),
                        SyncItem.STATE_DELETED, null);
                delItem.setGuid(guid);
                delItems.addElement(delItem);
                getSyncListenerFromSource(src).itemDeleted(delItem);
                syncStatus.addReceivedItem(guid, luid, delItem.getState(), SyncSource.SUCCESS_STATUS);
            }
        }

        if (delItems.size() > 0) {
            src.applyChanges(delItems);
        }
    }

    private void performFinalizationPhase(SyncSource src)
    throws SyncException {
        try {
            sapiSyncHandler.logout();
        } catch (SapiException e) {
            String errorMessage = "Cannot logout";
            processCommonSapiExceptions(e, errorMessage, false);
            processCustomSapiExceptions(e, errorMessage, true);
        }
        if(src != null) {
            src.endSync();
        }
    }

    private SyncListener getSyncListenerFromSource(SyncSource source) {
        SyncListener slistener = source.getListener();
        if(slistener != null) {
            return slistener;
        } else {
            return basicListener;
        }
    }

    private int getActualSyncMode(SyncSource src, int syncMode) {
        SyncAnchor anchor = src.getSyncAnchor();
        if(anchor instanceof SapiSyncAnchor) {
            SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)anchor;
            if(syncMode == SyncSource.INCREMENTAL_SYNC) {
                if(sapiAnchor.getUploadAnchor() == 0) {
                    return SyncSource.FULL_SYNC;
                }
            } else if(syncMode == SyncSource.INCREMENTAL_UPLOAD) {
                if(sapiAnchor.getUploadAnchor() == 0) {
                    return SyncSource.FULL_UPLOAD;
                }
            } else if(syncMode == SyncSource.INCREMENTAL_DOWNLOAD) {
                if(sapiAnchor.getDownloadAnchor() == 0) {
                    return SyncSource.FULL_DOWNLOAD;
                }
            }
            return syncMode;
        } else {
            throw new SyncException(SyncException.ILLEGAL_ARGUMENT,
                    "Invalid source anchor format");
        }
    }

    private int getActualDownloadSyncMode(SyncSource src) {
        SyncAnchor anchor = src.getSyncAnchor();
        if(anchor instanceof SapiSyncAnchor) {
            SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)anchor;
            if(sapiAnchor.getDownloadAnchor() > 0) {
                return SyncSource.INCREMENTAL_DOWNLOAD;
            } else {
                return SyncSource.FULL_DOWNLOAD;
            }
        } else {
            throw new SyncException(SyncException.ILLEGAL_ARGUMENT,
                    "Invalid source anchor format");
        }
    }

    private int getActualUploadSyncMode(SyncSource src) {
        SyncAnchor anchor = src.getSyncAnchor();
        if(anchor instanceof SapiSyncAnchor) {
            SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)anchor;
            if(sapiAnchor.getUploadAnchor() > 0) {
                return SyncSource.INCREMENTAL_UPLOAD;
            } else {
                return SyncSource.FULL_UPLOAD;
            }
        } else {
            throw new SyncException(SyncException.ILLEGAL_ARGUMENT,
                    "Invalid source anchor format");
        }
    }

    private boolean isIncrementalSync(int syncMode) {
        return (syncMode == SyncSource.INCREMENTAL_SYNC) ||
               (syncMode == SyncSource.INCREMENTAL_DOWNLOAD) ||
               (syncMode == SyncSource.INCREMENTAL_UPLOAD);
    }

    private boolean isDownloadPhaseNeeded(int syncMode) {
        return ((syncMode == SyncSource.FULL_DOWNLOAD) ||
                (syncMode == SyncSource.FULL_SYNC) ||
                (syncMode == SyncSource.INCREMENTAL_SYNC) ||
                (syncMode == SyncSource.INCREMENTAL_DOWNLOAD));
    }

    private boolean isUploadPhaseNeeded(int syncMode) {
        return (syncMode == SyncSource.FULL_SYNC) ||
               (syncMode == SyncSource.FULL_UPLOAD) ||
               (syncMode == SyncSource.INCREMENTAL_SYNC) ||
               (syncMode == SyncSource.INCREMENTAL_UPLOAD);
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

    private int getListenerStatusFromSyncException(SyncException se) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getting listener status for " + se.getCode());
        }
        int statusCode;
        switch (se.getCode()) {
            case SyncException.AUTH_ERROR:
                statusCode = SyncListener.INVALID_CREDENTIALS;
                break;
            case SyncException.FORBIDDEN_ERROR:
                statusCode = SyncListener.FORBIDDEN_ERROR;
                break;
            case SyncException.CONN_NOT_FOUND:
                statusCode = SyncListener.CONN_NOT_FOUND;
                break;
            case SyncException.READ_SERVER_RESPONSE_ERROR:
                statusCode = SyncListener.READ_SERVER_RESPONSE_ERROR;
                break;
            case SyncException.WRITE_SERVER_REQUEST_ERROR:
                statusCode = SyncListener.WRITE_SERVER_REQUEST_ERROR;
                break;
            case SyncException.SERVER_CONNECTION_REQUEST_ERROR:
                statusCode = SyncListener.SERVER_CONNECTION_REQUEST_ERROR;
                break;
            case SyncException.BACKEND_AUTH_ERROR:
                statusCode = SyncListener.BACKEND_AUTH_ERROR;
                break;
            case SyncException.NOT_FOUND_URI_ERROR:
                statusCode = SyncListener.URI_NOT_FOUND_ERROR;
                break;
            case SyncException.CONNECTION_BLOCKED_BY_USER:
                statusCode = SyncListener.CONNECTION_BLOCKED_BY_USER;
                break;
            case SyncException.SMART_SLOW_SYNC_UNSUPPORTED:
                statusCode = SyncListener.SMART_SLOW_SYNC_UNSUPPORTED;
                break;
            case SyncException.CLIENT_ERROR:
                statusCode = SyncListener.CLIENT_ERROR;
                break;
            case SyncException.ACCESS_ERROR:
                statusCode = SyncListener.ACCESS_ERROR;
                break;
            case SyncException.DATA_NULL:
                statusCode = SyncListener.DATA_NULL;
                break;
            case SyncException.ILLEGAL_ARGUMENT:
                statusCode = SyncListener.ILLEGAL_ARGUMENT;
                break;
            case SyncException.SERVER_ERROR:
                statusCode = SyncListener.SERVER_ERROR;
                break;
            case SyncException.SERVER_BUSY:
                statusCode = SyncListener.SERVER_BUSY;
                break;
            case SyncException.BACKEND_ERROR:
                statusCode = SyncListener.BACKEND_ERROR;
                break;
            case SyncException.CANCELLED:
                statusCode = SyncListener.CANCELLED;
                break;
            case SyncException.NOT_SUPPORTED:
                statusCode = SyncListener.NOT_SUPPORTED;
                break;
            case SyncException.ERR_READING_COMPRESSED_DATA:
                statusCode = SyncListener.COMPRESSED_RESPONSE_ERROR;
                break;
            case SyncException.DEVICE_FULL:
                statusCode = SyncListener.SERVER_FULL_ERROR;
                break;
            case SyncException.LOCAL_DEVICE_FULL:
                statusCode = SyncListener.LOCAL_CLIENT_FULL_ERROR;
                break;
            default:
                statusCode = SyncListener.GENERIC_ERROR;
                break;
        }
        return statusCode;
    }
    
    /**
     * From a {@link SapiException} returns corresponding {@link SyncException}
     * @param sapiException exception to analyze
     * @param newErrorMessage error message for the exception
     * @param throwGenericException thrown a generic exception if a specific one
     *                              is not detected 
     * 
     * @throws SyncException
     */
    private void processCommonSapiExceptions(
            SapiException sapiException,
            String newErrorMessage,
            boolean throwGenericException) throws SyncException {
        
        if (null == sapiException) {
            return;
        }
        
        String genericErrorMessage = null;
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = newErrorMessage;
        }
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = "Generic server error";
        }
        
        //Referring to section 4.1.3 of "Funambol Server API Developers Guide" document
        if (SapiException.NO_CONNECTION.equals(sapiException.getCode()) || 
            SapiException.HTTP_400.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.CONN_NOT_FOUND,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.PAPI_0000.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        } else if (SapiException.SEC_1002.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1004.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1001.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1003.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.CUS_0003.equals(sapiException.getCode())) {
            throw new SyncException(SyncException.NOT_SUPPORTED,
                    sapiException.getMessage());
        }

        if (throwGenericException) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        }
        //SAPI specific errors must be handled by calling method
    }

    private void processCustomSapiExceptions(
            SapiException sapiException,
            String newErrorMessage,
            boolean throwGenericException) throws SyncException {

        String genericErrorMessage = null;
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = newErrorMessage;
        }
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = "Generic server error";
        }
        
        if (null == sapiException) {
            return;
        }
        if (SapiException.CUS_0003.equals(sapiException.getCode())) {
            throw new SyncException(SyncException.NOT_SUPPORTED,
                    sapiException.getMessage());
        }

        if (throwGenericException) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        }
    }

    private void updateDownloadAnchor(SapiSyncAnchor sapiAnchor, long newDownloadAnchor) {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Updating download anchor to " + newDownloadAnchor);
        }
        sapiAnchor.setDownloadAnchor(newDownloadAnchor);
    }

    
    /**
     * Common code used to verify specific error in upload sapi
     * (size mismatch, over quota etc)
     * 
     * @param sapiException
     * @param item
     * @param remoteKey
     * @param sourceStatus 
     * @throws SyncException
     */
    private void verifyErrorInUploadResponse(
            SapiException sapiException,
            SyncItem item,
            String remoteKey,
            Vector sourceStatus) 
    throws SyncException
    {
        //manage common errors
        processCommonSapiExceptions(sapiException, "Cannot upload item", false);

        //manage custom errors
        processCustomSapiExceptions(sapiException, "Cannot upload item", false);
        
        if (SapiException.MED_1002.equals(sapiException.getCode())
                || SapiException.CUS_0001.equals(sapiException.getCode())) {

            // An item could not be fully uploaded
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Error uploading item " + item.getKey());
            }

            // The size of the uploading media does not match the one declared
            item.setGuid(remoteKey);
            
            syncStatus.addSentItem(item.getGuid(), item.getKey(),
                    item.getState(), SyncSource.INTERRUPTED_STATUS);
            sourceStatus.addElement(new ItemStatus(item.getKey(),
                    SyncSource.INTERRUPTED_STATUS));
            // Interrupt the sync with a network error
            throw new SyncException(SyncException.CONN_NOT_FOUND, sapiException.getMessage());
            
        } else if(SapiException.MED_1007.equals(sapiException.getCode())) {
            // An item could not be uploaded because user quota on
            // server exceeded
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Server quota overflow error");
            }
            sourceStatus.addElement(new ItemStatus(item.getKey(),
                    SyncSource.SERVER_FULL_ERROR_STATUS));
            throw new SyncException(SyncException.DEVICE_FULL, "Server quota exceeded");

        } else {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Cannot upload item, error in SAPI response: " + sapiException.getMessage());
        }
    }    
}
