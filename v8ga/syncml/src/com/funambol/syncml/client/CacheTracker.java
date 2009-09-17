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

package com.funambol.syncml.client;

import java.util.Hashtable;
import java.util.Enumeration;
import java.io.IOException;

import com.funambol.util.MD5;
import com.funambol.util.Base64;
import com.funambol.util.Log;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.protocol.SyncMLStatus;

/**
 * This class implements a ChangesTracker and it is based on comparison
 * of fingerprints. This means that the class can take
 * a snapshot of the SyncSource and store it in a StringKeyValueStore (a parameter
 * the client must provide). For each item in the SyncSource its
 * fingerprint is stored in the store.
 * When getNewItems, getUpdatedItems and getDeletedItems are invoked, they
 * compare the SyncSource current state and the last snapshot and detect
 * changes.
 * By default MD5 is used to compute fingerprints, but the method can be
 * redefined if a client wants to use a different method.
 */
public class CacheTracker implements ChangesTracker {

    private Hashtable newItems;
    private Hashtable deletedItems;
    private Hashtable updatedItems;
    private TrackableSyncSource ss;
    private StringKeyValueStore status;

    /**
     * Creates a CacheTracker. The constructor detects changes so that
     * the method to get the changes can be used right away
     *
     * @param status is the key value store with stored data
     */
    public CacheTracker(StringKeyValueStore status) {
        this.ss = ss;
        this.status = status;
    }

    /**
     * Associates this tracker to the given sync source
     *
     * @param ss the sync source
     */
    public void setSyncSource(TrackableSyncSource ss) {
        this.ss = ss;
    }

    /**
     * This method cleans any pending change. In the cache sync source
     * this means that the fingerprint of each item is updated to its current
     * value. The fingerprint tables will contain exactly the same items that
     * are currently in the Sync source.
     */
    public void reset() throws TrackerException {
        // TODO
    }

    public void begin() throws TrackerException {
        Log.trace("[CacheTracker.begin]");

        Enumeration allItemsKeys = ss.getAllItemsKeys();
        Hashtable snapshot = new Hashtable();

        newItems      = new Hashtable();
        updatedItems  = new Hashtable();
        deletedItems  = new Hashtable();

        while (allItemsKeys.hasMoreElements()) {
            String key = (String)allItemsKeys.nextElement();
            SyncItem item = new SyncItem(key);
            item = ss.getItemContent(item);
            // Compute the fingerprint for this item
            Log.trace("Computing fingerprint for " + item.getKey());
            String fp = computeFingerprint(item);
            Log.trace("Fingerpint is: " + fp);
            // Store the fingerprint for this item
            snapshot.put(item.getKey(), fp);
        }
        // Initialize the status by loading its content
        try {
            this.status.load();
        } catch (Exception e) {
            Log.debug("Cannot load tracker status, create an empty one");
            try {
                this.status.save();
            } catch (Exception e1) {
                Log.error("Cannot load tracker status");
                throw new TrackerException(e.toString());
            }
        }
        // Now compute the three lists
        Enumeration snapshotKeys = snapshot.keys();
        // Detect new items and updated items
        while (snapshotKeys.hasMoreElements()) {
            String newKey = (String)snapshotKeys.nextElement();
            if (status.get(newKey) == null) {
                Log.trace("Found a new item with key: " + newKey);
                newItems.put(newKey, snapshot.get(newKey));
            } else {
                // Check if their fingerprints are the same
                String oldFP = (String)this.status.get(newKey);
                String newFP = (String)snapshot.get(newKey);
                if (!oldFP.equals(newFP)) {
                    Log.trace("Found an updated item with key: " + newKey);
                    updatedItems.put(newKey, newFP);
                }
            }
        }
        // Detect deleted items
        Enumeration statusKeys = this.status.keys();
        while (statusKeys.hasMoreElements()) {
            String oldKey = (String)statusKeys.nextElement();
            if (snapshot.get(oldKey) == null) {
                Log.trace("Found a deleted item with key: " + oldKey);
                deletedItems.put(oldKey, (String)status.get(oldKey));
            }
        }
    }

    public void end() throws TrackerException {
        Log.trace("[CacheTracker.end]");
        // We must update the data store
        try {
            status.save();
        } catch (IOException ioe) {
            Log.error("Cannot save the cache data store");
        }

        // Allow the GC to pick this memory
        newItems      = null;
        updatedItems  = null;
        deletedItems  = null;
    }

    /**
     * Returns the list of new items.
     * @return the list of new items as an Enumeration
     *         of SyncItem
     */
    public Enumeration getNewItems() throws TrackerException {
        Log.trace("[CacheTracker.getNewItems]");
        // Any item in the sync source which is not part of the
        // old state is a new item
        if (newItems != null) {
            return newItems.keys();
        } else {
            return null;
        }
    }

    /**
     * Returns the number of new items that will be returned by the getNewItems
     * method
     *
     * @return the number of items
     */
    public int getNewItemsCount() throws TrackerException {
        if (newItems != null) {
            return newItems.size();
        } else {
            return 0;
        }
    }



    /**
     * Returns the list of updated items.
     * @return the list of updated items as an Enumeration
     *         of SyncItem
     */
    public Enumeration getUpdatedItems() throws TrackerException {
        Log.trace("[CacheTracker.getUpdatedItems]");
        // Any item whose fingerprint has changed is a new item
        if (updatedItems != null) {
            return updatedItems.keys();
        } else {
            return null;
        }
    }

    /**
     * Returns the number of deleted items that will be returned by the getDeletedItems
     * method
     *
     * @return the number of items
     */
    public int getUpdatedItemsCount() throws TrackerException {
        if (updatedItems != null) {
            return updatedItems.size();
        } else {
            return 0;
        }
    }


    /**
     * Returns the list of deleted items.
     * @return the list of updated items as an Enumeration
     *         of strings (SyncItem's keys)
     */
    public Enumeration getDeletedItems() throws TrackerException {
        Log.trace("[CacheTracker.getDeletedItems]");
        // Any item in the sync source which is not part of the
        // old state is a new item
        if (deletedItems != null) {
            return deletedItems.keys();
        } else {
            return null;
        }
    }

    /**
     * Returns the number of deleted items that will be returned by the getDeletedItems
     * method
     *
     * @return the number of items
     */
    public int getDeletedItemsCount() throws TrackerException {
        if (deletedItems != null) {
            return deletedItems.size();
        } else {
            return 0;
        }
    }

    public void setItemStatus(String key, int itemStatus) throws TrackerException {
        Log.trace("[CacheTracker.setItemStatus] " + key + "," + itemStatus);
        if (isSuccess(itemStatus)) {
            // We must update the fingerprint store with the value of the
            // fingerprint at the last sync
            if (newItems.get(key) != null) {
                // This is a new item
                String itemFP = (String)newItems.get(key);
                // Update the fingerprint
                status.put(key, itemFP);
            } else if (updatedItems.get(key) != null) {
                // This is a new item
                String itemFP = (String)updatedItems.get(key);
                // Update the fingerprint
                status.put(key, itemFP);
            } else if (deletedItems.get(key) != null) {
                // Update the fingerprint
                status.remove(key);
            }
        } else {
            // On error we do not change the fp so the change will
            // be reconsidered at the next sync
        }
    }

    protected String computeFingerprint(SyncItem item) {
        Log.trace("[CacheTracker.computeFingerprint]");
        byte data[] = item.getContent();
        MD5 md5 = new MD5();
        byte[] fp = md5.calculateMD5(data);
        byte[] fpB64 = Base64.encode(fp);
        return new String(fpB64);
    }

    protected boolean isSuccess(int status) {
        Log.trace("[CacheTracker.isSuccess] " + status);
        return SyncMLStatus.isSuccess(status);
    }

    public boolean removeItem(SyncItem item) throws TrackerException {
        // In a cache sync source an item is removed from the cache
        // if it actually part of the cache. In such a case it will not
        // be reported as a new item
        String fp;
        boolean res = true;
        switch (item.getState()) {
            case SyncItem.STATE_NEW:
                fp = computeFingerprint(item);
                status.put(item.getKey(), fp);
                break;
            case SyncItem.STATE_UPDATED:
                fp = computeFingerprint(item);
                status.put(item.getKey(), fp);
                break;
            case SyncItem.STATE_DELETED:
                status.remove(item.getKey());
                break;
            default:
                Log.error("Cache Tracker cannot remove item");
                res = false;
        }
        return res;
    }
}

