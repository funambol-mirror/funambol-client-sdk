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
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

import java.util.Vector;

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
        
        // TODO FIXME: check if resume is needed
        boolean resume = false;

        try {
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
        } finally {
            // TODO: create a report
            getSyncListenerFromSource(src).endSession(null);
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

        SyncItem item = getNextItemToUpload(src, incremental);
        while(item != null) {
            try {
                // Upload the item to the server
                sapiSyncHandler.uploadItem(item, getSyncListenerFromSource(src));
                
                // Set the item status
                sourceStatus.addElement(new ItemStatus(item.getKey(),
                        SyncSource.STATUS_SUCCESS));
            } catch(Exception ex) {
                if(Log.isLoggable(Log.ERROR)) {
                    Log.error(TAG_LOG, "Failed to upload item with key: " +
                            item.getKey(), ex);
                }
                sourceStatus.addElement(new ItemStatus(item.getKey(),
                        SyncSource.STATUS_SEND_ERROR));
            }
            item = getNextItemToUpload(src, incremental);
        }
        
        src.applyItemsStatus(sourceStatus);
    }

    private SyncItem getNextItemToUpload(SyncSource src, boolean incremental) {
        if(incremental) {
            return src.getNextNewItem();
        } else {
            return src.getNextItem();
        }
    }

    private void performDownloadPhase(SyncSource src, int syncMode, boolean resume) throws SyncException {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Starting download phase " + syncMode);
        }

        StringKeyValueStoreFactory mappingFactory = StringKeyValueStoreFactory.getInstance();
        StringKeyValueStore mapping = mappingFactory.getStringKeyValueStore(src.getName() + "_mapping");
        try {
            mapping.load();
        } catch (Exception e) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "The mapping store does not exist, use an empty one");
            }
        }

        if (syncMode == SyncSource.FULL_DOWNLOAD) {
            // We need to grabe the entire list of server items
            boolean done = false;
            int offset = 0;
            try {
                do {
                    // TODO FIXME: use the remote name
                    JSONArray items = sapiSyncHandler.getItems("picture", null, "" + FULL_SYNC_DOWNLOAD_LIMIT,
                                                               "" + offset);
                    if (items != null && items.length() > 0) {
                        applyNewUpdToSyncSource(src, items, SyncItem.STATE_NEW, mapping, true);
                        offset += items.length();
                        if (items.length() < FULL_SYNC_DOWNLOAD_LIMIT) {
                            done = true;
                        }
                    } else {
                        done = true;
                    }
                } while(!done);
            } catch (JSONException je) {
                Log.error(TAG_LOG, "Cannot parse server data", je);
            }
        } else if (syncMode == SyncSource.INCREMENTAL_DOWNLOAD) {
            SapiSyncAnchor sapiAnchor = (SapiSyncAnchor)src.getConfig().getSyncAnchor();
            Date anchor = new Date(sapiAnchor.getDownloadAnchor());
            Date now    = (new Date());

            try {
                // TODO FIXME: use the remote uri
                SapiSyncHandler.ChangesSet changesSet = sapiSyncHandler.getIncrementalChanges(anchor, "picture");

                if (changesSet != null) {
                    // We must pass all of the items to the sync source, but for each
                    // item we need to retrieve the complete information
                    if (changesSet.added != null) {
                        applyNewUpdItems(src, changesSet.added, SyncItem.STATE_NEW, mapping);
                    }

                    if (changesSet.updated != null) {
                        applyNewUpdItems(src, changesSet.updated, SyncItem.STATE_UPDATED, mapping);
                    }

                    if (changesSet.deleted != null) {
                        applyDelItems(src, changesSet.deleted, mapping);
                    }
                }
            } catch (JSONException je) {
                Log.error(TAG_LOG, "Cannot apply changes", je);
            }
        }
    }

    private void applyNewUpdItems(SyncSource src, JSONArray added, char state, StringKeyValueStore mapping)
    throws SyncException, JSONException {
        // The JSONArray contains the "id" of the new items, we still need to
        // download their complete meta information. We get the new items in
        // pages to make sure we don't use too much memory. Each page of items
        // is then passed to the sync source
        int i = 0;
        while(i < added.length()) {
            // Fetch a single page of items
            JSONArray itemsId = new JSONArray();
            for(int j=0;j<MAX_ITEMS_PER_BLOCK && i < added.length();++j) {
                int id = Integer.parseInt(added.getString(i++));
                itemsId.put(id);
            }
            if (itemsId.length() > 0) {
                // Ask for these items
                JSONArray items = sapiSyncHandler.getItems("picture", itemsId, null, null);
                if (items != null) {
                    applyNewUpdToSyncSource(src, items, state, mapping, false);
                }
            }
        }
    }

    private void applyNewUpdToSyncSource(SyncSource src, JSONArray items, char state, StringKeyValueStore mapping,
                                         boolean deepTwinSearch) throws SyncException, JSONException {
        // Apply these changes into the sync source
        Vector sourceItems = new Vector();
        for(int k=0;k<items.length();++k) {
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
                    Log.error(TAG_LOG, "Source does not implement TwinDetection, possible creation of duplicates");
                }
            } else {
                // In this case we only check if an item with the same guid
                // already exists. In such a case we turn the command into a
                // replace
                if (state == SyncItem.STATE_NEW && mapping.get(guid) != null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Turning add into replace as item already exists " + guid);
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

            SyncItem syncItem = src.createSyncItem(luid, src.getType(), state,  null, size);
            syncItem.setGuid(guid);
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

            if (state == SyncItem.STATE_UPDATED) {
                getSyncListenerFromSource(src).itemUpdated(syncItem);
            } else {
                getSyncListenerFromSource(src).itemReceived(syncItem);
            }

            sourceItems.addElement(syncItem);
        }
        // Apply the items in the sync source
        sourceItems = src.applyChanges(sourceItems);
        // The sourceItems returned by the call contains the LUID,
        // so we can create the luid/guid map here
        if (state == SyncItem.STATE_NEW) {
            try {
                for(int l=0;l<sourceItems.size();++l) {
                    SyncItem newItem = (SyncItem)sourceItems.elementAt(l);
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Updating mapping info for: " + newItem.getGuid() + "," + newItem.getKey());
                    }
                    mapping.put(newItem.getGuid(), newItem.getKey());
                    mapping.save();
                }
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot save mappings", e);
            }
        }
    }


    private void applyDelItems(SyncSource src, JSONArray removed, StringKeyValueStore mapping)
    throws SyncException, JSONException {

        Vector delItems = new Vector();
        for(int i=0;i < removed.length();++i) {
            String guid = removed.getString(i++);
            String luid = mapping.get(guid);
            if (luid == null) {
                luid = guid;
            }
            SyncItem delItem = new SyncItem(luid, src.getType(), SyncItem.STATE_DELETED, null);
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
}
