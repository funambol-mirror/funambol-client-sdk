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

package com.funambol.sync;

import java.util.Vector;

/**
 * A <i>SyncSource</i> is responsible for storing and retrieving <i>SyncItem</i>
 * objects from/to an external data source.
 * Note that the <i>SyncSource</i> interface makes no assumptions about the
 * underlying data source or about how data are formatted: each concrete
 * implementation will use its specific storage and format.
 * <p>
 * A <i>SyncSource</i> is not used directly by the host application, instead its
 * methods are called by the synchronization engine during modifications analysis.
 * <p>
 * The SyncSource methods are designed to perform an efficient synchronization
 * process, letting the source selecting the changed items instead of doing more
 * complex field by field comparison. It is responsibility of the source
 * developer to make sure that the <i>getNextNew/Updated/DeletedItem()</i>
 * methods return the correct values.
 * <p>
 * The configuration information required to set up a SyncSource is stored in
 * the class <i>SourceConfig</i> , used by the BaseSyncSource class.
 *
 */
public interface SyncSource {
    
    //------------------------------------------------------------ Attributes
    
    public static final String ENCODING_NONE = "none" ;
    public static final String ENCODING_B64  = "b64"  ;

    /**
     * No error for this session
     */
    public static final int STATUS_SUCCESS = 0;
    /**
     * At least one message has an error during send
     */
    public static final int STATUS_SEND_ERROR = 1;
    /**
     * At least one message has an error during receive
     */
    public static final int STATUS_RECV_ERROR = 2;
    /**
     * An error occurred with the remote server.
     * For instance an invalid messagev or item, but not
     * blocking for the sync.
     */
    public static final int STATUS_SERVER_ERROR = 4;
    /**
     * A problem with the connection to the remote server occurred
     */
    public static final int STATUS_CONNECTION_ERROR = 8;

    // A sync can be configured in different modes
    /**
     * A full two way sync (download and upload), only {@link SyncManager} can
     * use this value.
     *
     * The value is the same as SyncML.ALERT_CODE_SLOW
     */
    public static final int FULL_SYNC = 201;
    /**
     * A full upload from client to server sync, only {@link SyncManager} can
     * use this value.
     *
     * The value is the same as SyncML.ALERT_CODE_ONE_WAY_FROM_CLIENT_SLOW
     */
    public static final int FULL_UPLOAD = 251; 
    /**
     * A full download from server to client sync, only {@link SyncManager} can
     * use this value.
     *
     * The value is the same as SyncML.ALERT_CODE_ONE_WAY_FROM_SERVER_SLOW
     */
    public static final int FULL_DOWNLOAD = 252;
    /**
     * A "fast" two way sync, only newer item are exchanged between client and
     * server, user can choose this value in sync source settings.
     *
     * The value is the same as SyncML.ALERT_CODE_FAST
     */
    public static final int INCREMENTAL_SYNC = 200; 
    /**
     * Upload only newer item from client to server, user can choose this value
     * in sync source settings.
     *
     * The value is the same as SyncML.ALERT_CODE_ONE_WAY_FROM_CLIENT
     */
    public static final int INCREMENTAL_UPLOAD = 202; 
    /**
     * Download only newer item from server to client sync, user can choose this
     * value in sync source settings.
     *
     * The value is the same as SyncML.ALERT_CODE_ONE_WAY_FROM_SERVER
     */
    public static final int INCREMENTAL_DOWNLOAD = 204; 

    public static final int NO_SYNC = 0;

    // These are the status that sync source operations can return
    public static final int SUCCESS_STATUS           = 0;
    public static final int ERROR_STATUS             = 1;
    public static final int SERVER_FULL_ERROR_STATUS = 2;
    public static final int CHUNK_SUCCESS_STATUS     = 3;
    public static final int DEVICE_FULL_ERROR_STATUS = 4;
    public static final int INTERRUPTED_STATUS       = 5;

    //--------------------------------------------------------------- Methods

    /**
     * Returns the name of the source
     *
     * @return the name of the source
     */
    public String getName();

    /**
     * Returns the source URI
     *
     * @return the absolute URI of the source
     */
    public String getSourceUri();

    /**
     * Returns the type of the source.
     * The types are defined as mime-types, for instance * text/x-vcard).
     * @return the type of the source
     */
    public String getType();

    /**
     * Returns the encoding of the source.
     * The encoding can be 'b64' or 'none' only. The standard defines
     * also 'des' and '3des' but they are not implemented in this version
     * of the APIs.
     *
     * @return the encoding of the source
     */
    public String getEncoding();

    /**
     * Return the preferred sync mode of the source.
     *
     * @return the preferred sync mode for this source
     */
    public int getSyncMode() ;

    /**
     * Return the current filter for this SyncSource
     *
     */
    public SyncFilter getFilter();

    /**
     * Set a new filter for this SyncSource
     *
     */
    public void setFilter(SyncFilter filter);

    /**
     * Apply changes sent from server. There are three different kind of change requests:
     * 
     * <ul>
     *   <li> Add a new SyncItem to this source backend.
     *        The item key after a successful add must contain the local UID,
     *        that is used by the engine to send the mappings to the server.
     *        The source must then change the item key accordingly before return.
     *   </li>
     *   <li> Update a given SyncItem stored in the source backend.  </li>
     *   <li> Delete a SyncItem stored in the source backend. </li>
     * </ul>
     *
     * @param items the list of modified items.
     * @throws SyncException
     */
    public void applyChanges(Vector items) throws SyncException;

    /**
     *
     * Tell the SyncSource the status returned by the server
     * for a group of items previously sent.
     * @param itemsStatusList a collection of ItemStatus with the status info
     * @throws SyncException
     */
    public void applyItemsStatus(Vector itemsStatusList) throws SyncException;


    /**
     * Cancels any current or pending operations
     */
    public void cancel();

    /**
     * Returns the next item of the store (for slow sync).<p>
     * The method returns the set of items in the source. This set
     * can be frozen just after the beginSync is invoked or it can
     * change as new changes are applied.
     * The method only guarantees that it returns all the items that
     * were in the store when beginSync got invoked.
     * When an item is returned by this method it may be no longer in the source
     * if it got deleted. In other words there is no guarantee an item
     * returned is still in the source.
     * This method acts as an iterator and is guaranteed to be initialized after
     * beginSync.
     *
     * @return the current item or null if no more items
     */
    public SyncItem getNextItem() throws SyncException;
    
    /**
     * Returns the first/next new item of the store.<p>
     * The implementation of this method must iterate on the items of the
     * source not yet sent to the server, starting from the first one after a
     * beginSync() call and returning <code>null</code> when no more items are
     * available.
     *
     * @return the first new item, in a SyncItem object, or null if
     *         no new items are present.
     */
    public SyncItem getNextNewItem() throws SyncException;
    
    /**
     * Returns the first/next updated item of the store
     * (changed from the last sync)
     *
     * @return the first updated item, in a SyncItem object, or null if
     *         no updated items are present.
     */
    public SyncItem getNextUpdatedItem() throws SyncException;
    
    /** 
     * Returns a SyncItem containing the key of the first/next
     * deleted item of the store (locally removed after the last sync,
     * but not yet deleted on server)
     *
     * @return the first deleted item, in a SyncItem object, or null if
     *         no deleted items are present.
     */
    public SyncItem getNextDeletedItem() throws SyncException;
    
    /**
     * Return the number of changes that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of items to be sent to the server.
     *
     * This is a read-only value for the Syncmanager.
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientItemsNumber();

    /**
     * Return the number of new items (add) that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of new items to be sent to the server.
     *
     * This is a read-only value for the Syncmanager.
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientAddNumber();

    /**
     * Return the number of replaced items that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of replaced items to be sent to the server.
     *
     * This is a read-only value for the Syncmanager.
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientReplaceNumber();

    /**
     * Return the number of deleted items that the client will send during the
     * session. This method, after the beginSync() call, should return
     * the number of delted items to be sent to the server.
     *
     * This is a read-only value for the Syncmanager.
     *
     * @return number of items to sent, or -1 if unknown
     */
    public int getClientDeleteNumber();

    /**
     * Retiurn the number of changes that the server will send during the
     * session.
     * The value is set by the engine after the Sync tag is sent by the
     * server, with the value of the NumberOfchanges tag or -1 if not present.
     *
     * @return number of changes from the server, or -1 if not announced.
     */
    public int getServerItemsNumber();

    /**
     * Set the number of changes that the server will send during the
     * session. This method is called by the engine to notify the Source
     * of the number of changes announced by the server. If the server
     * does not announce the number of changes, the engine will call
     * this method with parameter -1.
     *
     * @param number of changes from the server, or -1 if not announced.
     */
    public void setServerItemsNumber(int number);

    /**
     * Returns the current anchor for this source
     */
    public SyncAnchor getSyncAnchor();

    /**
     * Sets the anchor for this source
     */
    public void setSyncAnchor(SyncAnchor syncAnchor);

    /**
     * Called after SyncManager preparation and initialization just before start
     * the synchronization of the SyncSource.
     * The implementation must reset the all/new/upd/del item lists when this
     * method is called by the sync engine.
     *
     * @param syncMode the synchronization mode (full vs incremental)
     *
     * @param resume specifies if this sync is a resume
     *
     * @throws SyncException in case of error. This will stop the sync process
     */
    public void beginSync(int syncMode, boolean resume) throws SyncException;
    

    /**
     * Called just before committing the synchronization process by the
     * SyncManager. The SyncSource can stop the commit phase raising an
     * exception here.
     *
     * @throws SyncException in case of error, to stop the commit.
     */
    public void endSync() throws SyncException;

    /**
     * Set a sync listener.
     *
     * @param listener the listener or null to remove it
     */
    public void setListener(SyncListener listener);

    /**
     * Returns the current listener (or null if not set)
     */
    public SyncListener getListener();

    /**
     * Returns the status of the sync source. The status is encoded as a bit
     * mask of the STATUS_* values
     */
    public int getStatus();

    /**
     * Returns the config of the source. The client can use this method
     * to obtain the config object and change some parameter. A setConfig()
     * must be called to actually change the source configuration.
     *
     * @return the config of the source
     */
    public SourceConfig getConfig();

    /**
     * Sets the config of the source. The client can use this method
     * to change the config of the source configuration.
     * This operation should not be done while the sync is in progress.
     *
     */
    public void setConfig(SourceConfig config);

    /**
     * Creates a SyncItem that the sync engine can use to store an incoming
     * item.
     *
     * @param key is the item key
     * @param type is the item type
     * @param state this item's state
     * @param parent is the item's parent
     * @param size is the item size
     */
    public SyncItem createSyncItem(String key, String type, char state,
                                   String parent, long size) throws SyncException;

    /**
     * This method specifies if this sync source supports suspend and resume. When the SyncManager
     * detects an interrupted sync it tries to resume it, but before doing it, it checks if the sync
     * source supports resuming. A SyncSource that supports resume must have a valid implementation for
     * the methods {@link exists} and {@link hasChangedSinceLastSync}
     *
     * @return true if the source supports suspend and resume
     */
    public boolean supportsResume();

    /**
     * This method checks if a given items exists locally.
     * @param key the item key
     * @return true if the item exists in the local data store
     * @throws SyncException if the local data store cannot be properly accessed
     */
    public boolean exists(String key) throws SyncException;

    /**
     * This method checks if a given item has changed since the last sync, whose start timestamp
     * is provided in case the source needs it.
     * @param key the item key
     * @param lastSyncStartTime the time at which the last sync started
     * @return true if the item has changed
     */
    public boolean hasChangedSinceLastSync(String key, long lastSyncStartTime);
}

