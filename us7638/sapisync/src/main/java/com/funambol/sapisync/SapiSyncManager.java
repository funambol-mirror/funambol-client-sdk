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
import java.util.Date;
import java.io.OutputStream;
import java.io.IOException;

import org.json.me.JSONException;
import org.json.me.JSONObject;
import org.json.me.JSONArray;

import com.funambol.sync.BasicSyncListener;
import com.funambol.sync.ItemStatus;
import com.funambol.sync.SyncAnchor;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SyncManagerI;
import com.funambol.sync.TwinDetectionSource;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.sync.ItemDownloadInterruptionException;
import com.funambol.sync.Filter;
import com.funambol.sync.SyncFilter;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;


/**
 * <code>SapiSyncManager</code> represents the synchronization engine performed
 * via SAPI.
 */
public class SapiSyncManager implements SyncManagerI {

    private static final String TAG_LOG = "SapiSyncManager";

    private static final int MAX_ITEMS_PER_BLOCK = 100;
    private static final int FULL_SYNC_DOWNLOAD_LIMIT = 300;

    private SyncConfig syncConfig = null;
    private SapiSyncHandler sapiSyncHandler = null;
    private SapiSyncStatus syncStatus = null;

    /**
     * Unique instance of a BasicSyncListener which is used when the user does
     * not set up a listener in the SyncSource. In order to avoid the creation
     * of multiple instances of this class we use this static variable
     */
    private static SyncListener basicListener = null;

    /**
     * <code>SapiSyncManager</code> constructor
     * @param config
     */
    public SapiSyncManager(SyncConfig config) {
        this.syncConfig = config;
        this.sapiSyncHandler = new SapiSyncHandler(
                StringUtil.extractAddressFromUrl(syncConfig.getSyncUrl()),
                syncConfig.getUserName(),
                syncConfig.getPassword());
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

        if (basicListener == null) {
            basicListener = new BasicSyncListener();
        }

        // JSONSyncSource require an updated sync config, therefore we update it
        // at the beginning of each sync
        if (src instanceof JSONSyncSource) {
            JSONSyncSource jsonSyncSource = (JSONSyncSource)src;
            jsonSyncSource.updateSyncConfig(syncConfig);
        }
        
        // TODO FIXME: check if resume is needed
        boolean resume = false;

        syncStatus = new SapiSyncStatus(src.getName());
        try {
            syncStatus.load();
        } catch (Exception e) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Cannot load sync status, use an empty one");
            }
        }
 
        try {
            // Set the basic properties in the sync status
            syncStatus.setRemoteUri(src.getConfig().getRemoteUri());
            syncStatus.setInterrupted(true);
            syncStatus.save();

            getSyncListenerFromSource(src).startSession();

            performInitializationPhase(src, getActualSyncMode(src, syncMode), resume);

            getSyncListenerFromSource(src).syncStarted(getActualSyncMode(src, syncMode));

            if(isDownloadPhaseNeeded(syncMode)) {
                // Get ready to update the download anchor
                long newDownloadAnchor = (new Date()).getTime();
                performDownloadPhase(src, getActualDownloadSyncMode(src), resume);
                // If we had no error so far, then we update the anchor
                SapiSyncAnchor anchor = (SapiSyncAnchor)src.getSyncAnchor();
                anchor.setDownloadAnchor(newDownloadAnchor);
            }

            if(isUploadPhaseNeeded(syncMode)) {
                long newUploadAnchor = (new Date()).getTime();
                performUploadPhase(src, getActualUploadSyncMode(src), resume);
                // If we had no error so far, then we update the anchor
                SapiSyncAnchor anchor = (SapiSyncAnchor)src.getSyncAnchor();
                anchor.setUploadAnchor(newUploadAnchor);
            }

            performFinalizationPhase(src);

            syncStatus.setInterrupted(false);
            syncStatus.setStatusCode(SyncListener.SUCCESS);

        } catch (Throwable t) {
            Log.error(TAG_LOG, "Error while synchronizing", t);
            syncStatus.setSyncException(t);
            try {
                syncStatus.save();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot save sync status", ioe);
            }
            SyncException se;
            if (t instanceof SyncException) {
                se = (SyncException)t;
            } else {
                se = new SyncException(SyncException.CLIENT_ERROR, "Generic error");
            }
            throw se;
        } finally {
            // We must guarantee this method is invoked in all cases
            getSyncListenerFromSource(src).endSession(syncStatus);
        }
    }

    public void cancel() {
        // TODO FIXME
        performFinalizationPhase(null);
    }

    private void performInitializationPhase(SyncSource src, int syncMode,
            boolean resume) {
        src.beginSync(syncMode, resume);
        sapiSyncHandler.login();
    }

    private void performUploadPhase(SyncSource src, int syncMode, boolean resume) {

        Vector sourceStatus = new Vector();
        
        boolean incremental = isIncrementalSync(syncMode);

        int uploadedCount = 0;
        SyncItem item = getNextItemToUpload(src, incremental, uploadedCount);
        while(item != null) {
            try {
                // Upload the item to the server
                sapiSyncHandler.uploadItem(item, getSyncListenerFromSource(src));
                
                // Set the item status
                sourceStatus.addElement(new ItemStatus(item.getKey(),
                        SyncSource.SUCCESS_STATUS));
            } catch(Exception ex) {
                if(Log.isLoggable(Log.ERROR)) {
                    Log.error(TAG_LOG, "Failed to upload item with key: " +
                            item.getKey(), ex);
                }
                sourceStatus.addElement(new ItemStatus(item.getKey(),
                        SyncSource.STATUS_SEND_ERROR));
            }
            uploadedCount++;
            item = getNextItemToUpload(src, incremental, uploadedCount);
        }
        
        src.applyItemsStatus(sourceStatus);
    }

    private SyncItem getNextItemToUpload(SyncSource src, boolean incremental,
            int uploadedCount) {
        SyncItem nextItem = getNextItem(src, incremental);
        SyncFilter syncFilter = src.getFilter();
        if(syncFilter != null) {
            Filter filter = incremental ? syncFilter.getIncrementalUploadFilter() :
                syncFilter.getFullUploadFilter();
            if(filter != null) {
                if(filter.getType() == Filter.ITEMS_COUNT_TYPE) {
                    if(uploadedCount >= filter.getCount()) {
                        // No further items shall be uploaded
                        nextItem = null;
                    }
                } else {
                    throw new UnsupportedOperationException("Not supported yet.");
                }
            }
        }
        return nextItem;
    }

    private SyncItem getNextItem(SyncSource src, boolean incremental) {
        if(incremental) {
            SyncItem next = src.getNextNewItem();
            if(next == null) {
                next = src.getNextUpdatedItem();
            }
            return next;
        } else {
            return src.getNextItem();
        }
    }

    private void performDownloadPhase(SyncSource src, int syncMode,
            boolean resume) throws SyncException {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Starting download phase " + syncMode);
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
        String remoteUri = src.getConfig().getRemoteUri();

        SyncFilter syncFilter = src.getFilter();
        
        if (syncMode == SyncSource.FULL_DOWNLOAD) {

            int totalCount = -1;

            int filterMaxCount = -1;
            Date filterFrom = null;
            
            Filter fullDownloadFilter = null;
            if(syncFilter != null) {
                fullDownloadFilter = syncFilter.getFullDownloadFilter();
            }
            if(fullDownloadFilter != null) {
                if(fullDownloadFilter != null && fullDownloadFilter.getType()
                        == Filter.ITEMS_COUNT_TYPE) {
                    filterMaxCount = fullDownloadFilter.getCount();
                } else if(fullDownloadFilter != null && fullDownloadFilter.getType()
                        == Filter.DATE_RECENT_TYPE) {
                    filterFrom = new Date(fullDownloadFilter.getDate());
                } else {
                    throw new UnsupportedOperationException("Not implemented yet");
                }
            }
            
            // Get the number of items and notify the listener
            try {
                totalCount = sapiSyncHandler.getItemsCount(remoteUri, filterFrom);
                if (totalCount != -1) {
                    if(filterMaxCount > 0) {
                        getSyncListenerFromSource(src).startReceiving(filterMaxCount);
                    } else {
                        getSyncListenerFromSource(src).startReceiving(totalCount);
                    }
                }
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot get the number of items on server", e);
                // We ignore this exception and try to get the content
            }
            int downloadLimit = FULL_SYNC_DOWNLOAD_LIMIT;
            String dataTag = getDataTag(src);
            int offset = 0;
            boolean done = false;
            try {
                do {
                    // Update the download limit given the total amount of items
                    // to download
                    if((offset + downloadLimit) > totalCount) {
                        downloadLimit = totalCount - offset;
                    }
                    JSONArray items = sapiSyncHandler.getItems(remoteUri, dataTag, null,
                            Integer.toString(downloadLimit),
                            Integer.toString(offset), filterFrom);
                    if (items != null && items.length() > 0) {
                        done = applyNewUpdToSyncSource(src, items, SyncItem.STATE_NEW,
                                filterMaxCount, offset, mapping, true);
                        offset += items.length();
                        if ((items.length() < FULL_SYNC_DOWNLOAD_LIMIT)) {
                            done = true;
                        }
                    } else {
                        done = true;
                    }
                } while(!done);
            } catch (JSONException je) {
                Log.error(TAG_LOG, "Cannot parse server data", je);
            } catch (ItemDownloadInterruptionException ide) {
                // An item could not be downloaded
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "");
                }
                // TODO FIXME: handle the suspend/resume
            }
        } else if (syncMode == SyncSource.INCREMENTAL_DOWNLOAD) {
            if(syncFilter != null && syncFilter.getIncrementalDownloadFilter() != null) {
                throw new UnsupportedOperationException("Not implemented yet");
            }
            SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)src.getConfig().getSyncAnchor();
            Date anchor = new Date(sapiAnchor.getDownloadAnchor());
            try {
                SapiSyncHandler.ChangesSet changesSet =
                        sapiSyncHandler.getIncrementalChanges(anchor, remoteUri);

                if (changesSet != null) {
                    // Count the number of items to be received and notify the
                    // listener
                    int total = 0;
                    if (changesSet.added != null) {
                        total += changesSet.added.length();
                    }
                    if (changesSet.updated != null) {
                        total += changesSet.updated.length();
                    }
                    if (changesSet.deleted != null) {
                        total += changesSet.deleted.length();
                    }

                    getSyncListenerFromSource(src).startReceiving(total);

                    // We must pass all of the items to the sync source, but for each
                    // item we need to retrieve the complete information
                    if (changesSet.added != null) {
                        applyNewUpdItems(src, changesSet.added,
                                SyncItem.STATE_NEW, mapping);
                    }
                    if (changesSet.updated != null) {
                        applyNewUpdItems(src, changesSet.updated,
                                SyncItem.STATE_UPDATED, mapping);
                    }
                    if (changesSet.deleted != null) {
                        applyDelItems(src, changesSet.deleted, mapping);
                    }
                }
            } catch (JSONException je) {
                Log.error(TAG_LOG, "Cannot apply changes", je);
            }
        }
        try {
            mapping.save();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot save mapping store", e);
        }
    }

    private void applyNewUpdItems(SyncSource src, JSONArray added, char state, 
            StringKeyValueStore mapping) throws SyncException, JSONException {
        // The JSONArray contains the "id" of the new items, we still need to
        // download their complete meta information. We get the new items in
        // pages to make sure we don't use too much memory. Each page of items
        // is then passed to the sync source
        int i = 0;
        String dataTag = getDataTag(src);
        while(i < added.length()) {
            // Fetch a single page of items
            JSONArray itemsId = new JSONArray();
            for(int j=0;j<MAX_ITEMS_PER_BLOCK && i < added.length();++j) {
                int id = Integer.parseInt(added.getString(i++));
                itemsId.put(id);
            }
            if (itemsId.length() > 0) {
                // Ask for these items
                JSONArray items = sapiSyncHandler.getItems(
                        src.getConfig().getRemoteUri(), dataTag,
                        itemsId, null, null, null);

                if (items != null) {
                    applyNewUpdToSyncSource(src, items, state, -1, -1, mapping, false);
                }
            }
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
            char state, int maxItemsCount, int appliedItemsCount,
            StringKeyValueStore mapping, boolean deepTwinSearch)
            throws SyncException, JSONException {

        boolean done = false;
        
        // Apply these changes into the sync source
        Vector sourceItems = new Vector();
        for(int k=0; k<items.length() && !done; ++k) {
            JSONObject item = items.getJSONObject(k);
            String     guid = item.getString("id");
            long       size = Long.parseLong(item.getString("size"));

            if (deepTwinSearch) {
                // In this case we cannot rely on mappings to detect twins, we
                // rather perform a content analysis to determine twins
                if (src instanceof TwinDetectionSource) {
                    TwinDetectionSource twinSource = (TwinDetectionSource)src;
                    SyncItem sourceItem = new SyncItem(guid, src.getType(), state, null);
                    sourceItem.setContent(item.toString().getBytes());
                    SyncItem twinItem = twinSource.findTwin(sourceItem);
                    if (twinItem != null) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Found twin for item: " + guid);
                        }
                        // Skip the processing of this item
                        continue;
                    }
                } else {
                    Log.error(TAG_LOG, "Source does not implement TwinDetection, "
                            + "possible creation of duplicates");
                }
            } else {
                // In this case we only check if an item with the same guid
                // already exists. In such a case we turn the command into a
                // replace
                if (state == SyncItem.STATE_NEW && mapping.get(guid) != null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Turning add into replace as item "
                                + "already exists " + guid);
                    }
                    state = SyncItem.STATE_UPDATED;
                }
            }

            String luid;
            if (state == SyncItem.STATE_UPDATED) {
                luid = mapping.get(guid);
            } else {
                luid = guid;
            }
            // If the client doesn't have the luid we change the state to new
            if(StringUtil.isNullOrEmpty(luid)) {
                state = SyncItem.STATE_NEW;
            }

            if (state == SyncItem.STATE_NEW) {
                getSyncListenerFromSource(src).itemAddReceivingStarted(luid, null, size);
            } else if(state == SyncItem.STATE_UPDATED) {
                getSyncListenerFromSource(src).itemReplaceReceivingStarted(luid, null, size);
            }

            SyncItem syncItem = null;
            if(src instanceof JSONSyncSource) {
                syncItem = ((JSONSyncSource)src).createSyncItem(
                        luid, src.getType(), state, null, item);
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
            syncItem.setGuid(guid);
            
            // Filter downloaded items for JSONSyncSources only
            if(src instanceof JSONSyncSource) {
                if(((JSONSyncSource)src).filterSyncItem(syncItem)) {
                    sourceItems.addElement(syncItem);
                }
            } else {
                sourceItems.addElement(syncItem);
            }
            // Apply items count filter
            if(maxItemsCount > 0) {
                if((appliedItemsCount + sourceItems.size()) >= maxItemsCount) {
                    done = true;
                }
            }
            if (state == SyncItem.STATE_NEW) {
                getSyncListenerFromSource(src).itemAddReceivingEnded(luid, null);
            } else if(state == SyncItem.STATE_UPDATED) {
                getSyncListenerFromSource(src).itemReplaceReceivingEnded(luid, null);
            }
        }
        // Apply the items in the sync source
        sourceItems = src.applyChanges(sourceItems);
        // The sourceItems returned by the call contains the LUID,
        // so we can create the luid/guid map here
        try {
            for(int l=0;l<sourceItems.size();++l) {
                SyncItem newItem = (SyncItem)sourceItems.elementAt(l);
                // Update the sync status
                syncStatus.addReceivedItem(newItem.getGuid(), newItem.getKey(),
                        newItem.getState(), newItem.getSyncStatus());
                // and the mapping table
                if (state == SyncItem.STATE_NEW) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Updating mapping info for: " +
                                newItem.getGuid() + "," + newItem.getKey());
                    }
                    mapping.add(newItem.getGuid(), newItem.getKey());
                }
            }
            syncStatus.save();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot save mappings", e);
        }
        return done;
    }


    private void applyDelItems(SyncSource src, JSONArray removed, 
            StringKeyValueStore mapping) throws SyncException, JSONException {
        Vector delItems = new Vector();
        for(int i=0;i < removed.length();++i) {
            String guid = removed.getString(i);
            String luid = mapping.get(guid);
            if (luid == null) {
                luid = guid;
            }
            SyncItem delItem = new SyncItem(luid, src.getType(),
                    SyncItem.STATE_DELETED, null);
            delItem.setGuid(guid);
            delItems.addElement(delItem);
            getSyncListenerFromSource(src).itemDeleted(delItem);
        }

        if (delItems.size() > 0) {
            src.applyChanges(delItems);
        }
    }


    private void performFinalizationPhase(SyncSource src) {
        sapiSyncHandler.logout();
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
}
