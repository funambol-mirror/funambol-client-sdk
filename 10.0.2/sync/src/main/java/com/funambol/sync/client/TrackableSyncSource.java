/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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
package com.funambol.sync.client;

import java.util.Enumeration;
import java.util.Vector;

import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncFilter;
import com.funambol.sync.SyncAnchor;
import com.funambol.sync.ItemStatus;

import com.funambol.util.Log;

/**
 * An abstract implementation of SyncSource, providing
 * the ability to use a ChangesTracker to automatically trace
 * changes.
 * This class needs a ChangesTracker to be able to track changes
 */
public abstract class TrackableSyncSource implements SyncSource {

    private static final String TAG_LOG = "TrackableSyncSource";
    
    //--------------------------------------------------------------- Attributes
    protected ChangesTracker tracker;

    /** All items for slow syncs */
    protected Enumeration allItems = null;

    /** New items for fast syncs */
    protected Enumeration newItems = null;

    /** Updated items for fast syncs */
    protected Enumeration updItems = null;

    /** Deleted items for fast syncs */
    protected Enumeration delItems = null;

    /** SyncSource configuration */
    protected SourceConfig config;

    /** Synchronization filter */
    protected SyncFilter filter;

    /** SyncMode, set by beginSync */
    protected int syncMode;

    /** The number of items to be sent to the server in the session */
    protected int clientItemsNumber;
    
    /** The number of items that the server announced to send in the session */
    protected int serverItemsNumber;

    /** The number of new items to be sent to the server in the session */
    protected int clientAddItemsNumber;

    /** The number of replaced items to be sent to the server in the session */
    protected int clientReplaceItemsNumber;

    /** The number of deleted items to be sent to the server in the session */
    protected int clientDeleteItemsNumber;

    /** Status of the sync source summarized in an integer value. See constants
     * defined in SyncSource */
    protected int globalStatus;

    /** Listener of the sync process */
    private SyncListener listener;

    /**
     * This is the flag used to indicate that any current operation shall be 
     * cancelled. Users can call the cancel (@see cancel) method to cancel any
     * current operation.
     */
    private boolean cancel = false;

    //------------------------------------------------------------- Constructors

    /**
     * TrackableSyncSource constructor: initialize source config
     */
    public TrackableSyncSource(SourceConfig config, ChangesTracker tracker) {

        this.config = config;
        // Set up the tracker
        this.tracker = tracker;
        if (tracker != null) {
            tracker.setSyncSource(this);
        }
    }

    /**
     * Re-sets the tracker. Be careful when using this method. Any change
     * tracked in the previous tracker is going to be lost.
     *
     * @param tracker the new tracker
     */
    public void setTracker(ChangesTracker tracker) {
        this.tracker = tracker;
        tracker.setSyncSource(this);
    }

    /**
     * Return the current tracker for this source
     */
    public final ChangesTracker getTracker() {
        return tracker;
    }

    /**
     * This implementation performs a linear scan of the items and calls add/update/delete items
     * accordingly.
     * @param syncItems
     * @throws SyncException
     */
    public void applyChanges(Vector syncItems) throws SyncException {
        
        int status = -1; // outside of the loop because it's used at each step 
                         // after the first one to keep track of the previous 
                         // item's sync status
        for(int i = 0; i < syncItems.size(); ++i) {

            cancelIfNeeded();
            
            SyncItem item = (SyncItem)syncItems.elementAt(i);
            try {
                if (item.getState() == SyncItem.STATE_NEW) {                    
                    status = addItem(item);
                } else if (item.getState() == SyncItem.STATE_UPDATED) {
                    status = updateItem(item);
                } else { // STATE_DELETED
                    status = deleteItem(item.getKey());
                }
            } catch (Exception e) {
                status = ERROR_STATUS;
            }
            item.setSyncStatus(status);
        }
    }

    public void applyItemsStatus(Vector itemsStatus) {
        tracker.setItemsStatus(itemsStatus);
    }

    public void beginSync(int syncMode, boolean resume) throws SyncException {

        this.syncMode = syncMode;
        cancel = false;

        if (tracker == null) {
            throw new SyncException(SyncException.CLIENT_ERROR, "Trackable source without tracker");
        }

        // The tracker must be initialized before the source
        // as it may invoke the initXXXItems which depend on the tracker
        try {
            tracker.begin(syncMode, resume);
        } catch (TrackerException te) {
            Log.error(TAG_LOG, "Cannot track changes: " + te);
            throw new SyncException(SyncException.CLIENT_ERROR, te.toString());
        }

        allItems = null;
        newItems = null;
        updItems = null;
        delItems = null;

        // Init lists
        switch(syncMode) {
            case FULL_SYNC:
            case FULL_UPLOAD:
                // A refresh from client is like a slow here
                allItems = getAllItemsKeys();
                allItems = applyFiltersForOutgoingItems(allItems);
                // We guarantee that the getAllItemsCount is invoked after the
                // getAllItemsKeys
                clientItemsNumber = getAllItemsCount();
                clientAddItemsNumber = 0;
                clientReplaceItemsNumber = 0;
                clientDeleteItemsNumber = 0;
                break;
            case INCREMENTAL_SYNC:
            case INCREMENTAL_UPLOAD:
                // A one way from client is like a fast here
                newItems = tracker.getNewItems();
                updItems = tracker.getUpdatedItems();
                delItems = tracker.getDeletedItems();

                // Init number of changes counter
                clientAddItemsNumber = tracker.getNewItemsCount();
                clientReplaceItemsNumber = tracker.getUpdatedItemsCount();
                clientDeleteItemsNumber = tracker.getDeletedItemsCount();

                clientItemsNumber = clientAddItemsNumber +
                                    clientReplaceItemsNumber +
                                    clientDeleteItemsNumber;
                break;
            case INCREMENTAL_DOWNLOAD:
                // No modifications to send (it's not
                // strictly necessary to reset the lists,
                // because the engine will not ask items to
                // the SyncSource, but it's good to do it)
                newItems = null;
                updItems = null;
                delItems = null;
                // Init number of changes counter
                clientItemsNumber = 0;
                clientAddItemsNumber = 0;
                clientReplaceItemsNumber = 0;
                clientDeleteItemsNumber = 0;
                break;
            case FULL_DOWNLOAD:
                // In this case, the SyncSource should
                // delete all the items in the database
                // (possibly asking the user before that)

                // delete all items only if the syncsource allow this
                // (generally, for PIM syncsources is yes, for media syncsources is no)
                if (isDeleteAllItemsAllowed()) {
                    deleteAllItems();
                } else {
                    Log.debug(TAG_LOG, "Skipping deletion of local items");
                }
                
                // No modifications to send.
                newItems = null;
                updItems = null;
                delItems = null;
                // Init number of changes counter
                clientItemsNumber = 0;
                clientAddItemsNumber = 0;
                clientReplaceItemsNumber = 0;
                clientDeleteItemsNumber = 0;
                break;
            default:
                throw new SyncException(SyncException.SERVER_ERROR,
                                        "SyncSource "+getName()+
                                        ": invalid sync mode "+getSyncMode());
        }
    }

    private Enumeration applyFiltersForOutgoingItems(Enumeration allItems) {
        if (null == allItems || !allItems.hasMoreElements()) {
            return allItems;
        }

        Vector goodItems = new Vector();
        
        //removed items
        while (allItems.hasMoreElements()) {
            String key = (String)allItems.nextElement();
            boolean filteredOut = filterOutgoingItem(key, false);
            if (!filteredOut) {
                goodItems.addElement(key);
            } else {
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Item with key " + key + " was filtered out");
                }
            }
        }
        
        return goodItems.elements();
    }

    public void endSync() throws SyncException {
        tracker.end();
    }

    public SyncItem getNextItem() throws SyncException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getNextItem");
        }
        if (allItems == null) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Internal error: allItems not initialized");
        }
        SyncItem item = null;
        if (allItems.hasMoreElements()) {
            String key = (String)allItems.nextElement();
            item = new SyncItem(key);
            item = getItemContent(item);
        }
        return item;
    }

    public SyncItem getNextNewItem() throws SyncException {
        if (newItems == null) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Internal error: newItems not initialized");
        }
        SyncItem item = null;
        if (newItems.hasMoreElements()) {
            String key = (String)newItems.nextElement();
            item = new SyncItem(key);
            item.setState(SyncItem.STATE_NEW);
            item = getItemContent(item);
        }
        return item;
    }

    public SyncItem getNextUpdatedItem() throws SyncException {
        if (updItems == null) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Internal error: updItems not initialized");
        }
        SyncItem item = null;
        if (updItems.hasMoreElements()) {
            String key = (String)updItems.nextElement();
            item = new SyncItem(key);
            item.setState(SyncItem.STATE_UPDATED);
            item = getItemContent(item);
        }
        return item;
    }

    public SyncItem getNextDeletedItem() throws SyncException {
        if (delItems == null) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Internal error: delItems not initialized");
        }
        SyncItem item = null;
        if (delItems.hasMoreElements()) {
            String key = (String)delItems.nextElement();
            item = new SyncItem(key);
            item.setState(SyncItem.STATE_DELETED);
        }
        return item;
    }

    /**
     * Creates a new SyncItem for the engine to store incoming items
     */
    public SyncItem createSyncItem(String key, String type, char state,
                                   String parent, long size) {

        SyncItem item = new SyncItem(key, type, state, parent);
        return item;
    }

    /**
     * The purpose of this implementation is simply to notify the tracker.
     * Classes that extends the TrackableSyncSource should invoke this method at
     * the end of their processing in the addItem so that the tracker is
     * properly updated. Otherwise they are in charge of notifying the tracker.
     *
     * @param item is the item being added
     *
     * @return SUCCESS if the tracker was successfully updated, ERROR
     * otherwise
     */
    protected int addItem(SyncItem item) throws SyncException {
        // a new item was added during the sync. we must notify our tracker
        boolean done = tracker.removeItem(item);
        return done ? SUCCESS_STATUS : ERROR_STATUS;
    }
    
    /**
     * The purpose of this implementation is simply to notify the tracker.
     * Classes that extends the TrackableSyncSource should invoke this method at
     * the end of their processing in the updateItem so that the tracker is
     * properly updated. Otherwise they are in charge of notifying the tracker.
     *
     * @param item is the item being updated
     *
     * @return SUCCESS if the tracker was successfully updated, ERROR
     * otherwise
     */
    protected int updateItem(SyncItem item) throws SyncException {
        // a new item was replaced during the sync. we must notify our tracker
        boolean done = tracker.removeItem(item);
        return done ? SUCCESS_STATUS : ERROR_STATUS;
    }
    
    /**
     * The purpose of this implementation is simply to notify the tracker.
     * Classes that extends the TrackableSyncSource should invoke this method at
     * the end of their processing in the updateItem so that the tracker is
     * properly updated. Otherwise they are in charge of notifying the tracker.
     *
     * @param key is the key of the item being deleted
     *
     * @return SUCCESS if the tracker was successfully updated, ERROR
     * otherwise
     */
    protected int deleteItem(String key) throws SyncException {
        // a new item was replaced during the sync. we must notify our tracker
        SyncItem item = new SyncItem(key,getType(),SyncItem.STATE_DELETED,null);
        boolean done = tracker.removeItem(item);
        return done ? SUCCESS_STATUS : ERROR_STATUS;
    }

    /**
     * This method is for backward compatibility only
     * @deprecated
     * @throws SyncException
     */
    protected void setItemStatus(String key, int status) throws SyncException {
        Vector items = new Vector();
        items.addElement(new ItemStatus(key, status));
        tracker.setItemsStatus(items);
    }

    /**
     * Physically delete all items. Here we simply reset the tracker status.
     * This shall be defined by subclasses in order to phisically delete all
     * the items.
     */
    protected void deleteAllItems() {
        tracker.empty();
    }
    
    /**
     * Returns the config of the source. The client can use this method
     * to obtain the config object and change some parameter. A setConfig()
     * must be called to actually change the source configuration.
     *
     * @return the config of the source
     */
    public SourceConfig getConfig() {
        return config;
    }

    /**
     * Sets the config of the source. The client can use this method
     * to change the config of the source configuration.
     * This operation should not be done while the sync is in progress.
     *
     */
    public void setConfig(SourceConfig config) {
        this.config = config;
    }

    /**
     * Returns the status of the sync source. The status is encoded as a bit
     * mask of the STATUS_* values
     */
    public int getStatus() {
        return globalStatus;
    }

    /**
     * Set a sync listener.
     *
     * @param listener the listener or null to remove it
     */
    public void setListener(SyncListener listener) {
        this.listener = listener;
    }

    /**
     * Returns the current listener (or null if not set)
     */
    public SyncListener getListener() {
        return listener;
    }

        /**
     * Returns the name of the source
     *
     * @return the name of the source
     */
    public String getName() {
        return config.getName();
    }

    /**
     * Returns the source URI
     *
     * @return the absolute URI of the source
     */
    public String getSourceUri() {
        return config.getRemoteUri();
    }

    /**
     * Returns the type of the source.
     * The types are defined as mime-types, for instance * text/x-vcard).
     * @return the type of the source
     */
    public String getType() {
        return config.getType();
    }

    /**
     * Returns the encoding of the source.
     * The encoding can be 'b64' or 'none' only. The standard defines
     * also 'des' and '3des' but they are not implemented in this version
     * of the APIs.
     *
     * @return the encoding of the source
     */
    public String getEncoding() {
        return config.getEncoding();
    }

    /**
     * Returns the preferred sync mode of the source.
     * The preferred sync mode is the one that the SyncManager sends
     * to the server in the initialization phase. The server can respond
     * with a different alert code, to force, for instance, a slow.
     *
     * @return the preferred sync mode for this source
     */
    public int getSyncMode() {
        return config.getSyncMode();
    }

    /**
     * Returns the current filter for this SyncSource.
     */
    public SyncFilter getFilter() {
        return filter;
    }

    /**
     * Set a new filter for this SyncSource
     */
    public void setFilter(SyncFilter filter) {
        this.filter = filter;
    }

    /**
     * Return the number of changes that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of items to be sent to the server.
     *
     * The number of changes is computed by initXXXItems() during beginSync().
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientItemsNumber() {
        return clientItemsNumber;
    }

    /**
     * Return the number of new items (add) that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of new items to be sent to the server.
     *
     * The number of changes is computed by initXXXItems() during beginSync().
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientAddNumber() {
        return clientAddItemsNumber;
    }

    /**
     * Return the number of replaced items that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of replaced items to be sent to the server.
     *
     * The number of changes is computed by initXXXItems() during beginSync().
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientReplaceNumber() {
        return clientReplaceItemsNumber;
    }

    /**
     * Return the number of deleted items that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of delted items to be sent to the server.
     *
     * The number of changes is computed by initXXXItems() during beginSync().
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientDeleteNumber() {
        return clientDeleteItemsNumber;
    }


    /**
     * Return the number of changes that the server will send during the
     * session. This method, after the beginSync() call, should return
     * the number of items to be sent to the server.
     *
     * @return number of changes from the server, or -1 if not announced.
     */
    public int getServerItemsNumber() {
        return serverItemsNumber;
    }

    /**
     * Set the number of changes that the server will send during the
     * session. This method is called by the engine to notify the Source
     * of the number of changes announced by the server. If the server
     * does not announce the number of changes, the engine will call
     * this method with parameter -1.
     *
     * @param number of changes from the server, or -1 if not announced.
     */
    public void setServerItemsNumber(int number) {
        serverItemsNumber = number;
    }

    /** 
     * Return the sync Anchor for this source
     */
    public SyncAnchor getSyncAnchor() {
        return config.getSyncAnchor();
    }
    
    /** 
     * Set the value of the Last Anchor for this source
     */
    public void setSyncAnchor(SyncAnchor syncAnchor) {
        config.setSyncAnchor(syncAnchor);
    }

    public void cancel() {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Cancelling any current operation");
        }
        cancel = true;
    }
    
    /**
     * Checks if the item received from server must be filtered out
     * or not
     * (for example, content type not supported, size too big etc)
     * 
     * TODO: move this method to new manipulator object
     * 
     * @param key
     * @return true is the item must be discarded, otherwise false
     */
    public boolean filterIncomingItem(String key) {
        //default implementation, item is always accepted
        return false;
    }
    
    /**
     * Checks if the item that should be sent to server must be filtered out
     * or not.
     * (for example, size too big, item too old etc)
     * 
     * TODO: move this method to new manipulator object
     * 
     * @param key
     * @param removed specifies if the item has been removed from the local
     * storage
     *
     * @return true if the item must be discarded, otherwise false
     */
    public boolean filterOutgoingItem(String key, boolean removed) {
        //default implementation, item is always sent
        return false;
    }
    
    
    
    /**
     * This method return the number of items returned by the getAllItemsKeys.
     * It is guaranteed that the method is invoked after getAllItemsKeys.
     * It can return -1 if the number of items is unknown.
     */
    protected int getAllItemsCount() throws SyncException {
        return -1;
    }

    protected abstract Enumeration getAllItemsKeys();

    protected abstract SyncItem getItemContent(SyncItem item) throws SyncException;

    protected void cancelIfNeeded() throws SyncException {
        if(cancel) {
            throw new SyncException(SyncException.CANCELLED, "Cancelled");
        }
    }

    protected boolean isCancelled() throws SyncException {
        return cancel;
    }

    /**
     * Returns true if, during a {@link SyncSource#FULL_DOWNLOAD},
     * items on client can be deleted. Otherwise false.
     * For example, PIM items can be deleted, media items no.
     *   
     * @return true if local items can be deleted, otherwise false 
     */
    protected boolean isDeleteAllItemsAllowed() {
        return true;
    }
}

