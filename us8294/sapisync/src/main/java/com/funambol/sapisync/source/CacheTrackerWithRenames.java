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

package com.funambol.sapisync.source;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

import com.funambol.storage.StringKeyValueStore;
import com.funambol.sync.client.CacheTracker;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.TrackerException;
import com.funambol.util.Log;



/**
 * Represents a CacheTracker which is able to detect rename operation within the
 * retrieved changes.
 */
public class CacheTrackerWithRenames extends CacheTracker {

    private static final String TAG_LOG = "CacheTrackerWithRenames";

    private Hashtable renamesMap;
    private String sourceName;

    public CacheTrackerWithRenames(String sourceName, StringKeyValueStore statusStore) {
        super(statusStore);
        this.sourceName = sourceName;
        this.renamesMap = new Hashtable();
    }

    /*
    public CacheTrackerWithRenames(StringKeyValueStore renamesStore, StringKeyValueStore statusStore) {
        super(statusStore);
        this.renamesStore = renamesStore;
    }
    */

    public void begin(int syncMode, boolean reset) throws TrackerException {
        super.begin(syncMode, reset);
        if (syncMode == SyncSource.INCREMENTAL_SYNC || syncMode == SyncSource.INCREMENTAL_UPLOAD) {
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Checking renames");
            }

            if (newItems.size() == 0 || deletedItems.size() == 0) {
                return;
            }

            // If we have a new item with the very same fingerprint as a deleted
            // one, then we turn this pair of add/delete into an update
            Enumeration nEnum = newItems.keys();
            while(nEnum.hasMoreElements()) {
                String nKey = (String)nEnum.nextElement();
                SyncItem item = createItemForFingerprint(nKey);
                String nfp = computeFingerprint(item);
                // Now search among the deleted items
                Enumeration dEnum = deletedItems.keys();
                while(dEnum.hasMoreElements()) {
                    String dKey = (String)dEnum.nextElement();
                    String dfp = status.get(dKey);
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Comparing added/deleted fingerprint for " + nKey + " and " + dKey
                                           + " whose fingerprint are " + nfp + " and " + dfp);
                    }
                    if (dfp != null && dfp.equals(nfp)) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Found a renamed item. Was " + dKey + " and now is " + nKey);
                        }
                        newItems.remove(nKey);
                        deletedItems.remove(dKey);
                        updatedItems.put(nKey, nfp);

                        renamesMap.put(nKey, dKey);
                    }
                }
            }
        }
    }

    // TODO: redefine this to return just the key
    protected SyncItem createItemForFingerprint(String key) {
        SyncItem item = new SyncItem(key);
        return getItemContent(item);
    }

    public String getRenamedFileName(String newFileName) {
        return (String)renamesMap.get(newFileName);
    }

    public boolean isRenamedItemUpdated(String oldKey, String newKey) {
        String oldFP = status.get(oldKey);
        String newFP = (String)updatedItems.get(newKey);
        boolean updated = true;
        if(oldFP != null && newFP != null) {
            updated = !oldFP.equals(newFP);
        }
        return updated;
    }

    public boolean isRenamedItem(String key) {
        return renamesMap.get(key) != null;
    }

    protected void setItemStatus(String key, int itemStatus) throws TrackerException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "setItemStatus " + key + "," + itemStatus);
        }
        boolean renameStatusHandled = false;
        if (isSuccess(itemStatus) && itemStatus != SyncSource.CHUNK_SUCCESS_STATUS) {
            String deletedKey = (String)renamesMap.get(key);
            if (deletedKey != null) {
                // Update the base tracker status
                removeItem(new SyncItem(key, null, SyncItem.STATE_NEW, null));
                removeItem(new SyncItem(deletedKey, null, SyncItem.STATE_DELETED, null));
                // Update the renames status
                renamesMap.remove(key);
                renameStatusHandled = true;
            }
        }
        if(!renameStatusHandled) {
            super.setItemStatus(key, itemStatus);
        }
    }

    public void reset() throws TrackerException {
        super.reset();
        renamesMap.clear();
    }

    public void empty() throws TrackerException {
        super.empty();
        renamesMap.clear();
    }
}


