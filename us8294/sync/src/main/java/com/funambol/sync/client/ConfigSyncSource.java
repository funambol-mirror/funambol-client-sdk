/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncSource;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.util.Log;


/**
 * An implementation of TrackableSyncSource, providing
 * the ability to sync Funambol's client configuration
 */
public class ConfigSyncSource extends TrackableSyncSource {

    private static final String TAG_LOG = "ConfigSyncSource";

    private StringKeyValueStore store = null;

    //------------------------------------------------------------- Constructors

    /**
     * ConfigSyncSource constructor
     */
    public ConfigSyncSource(SourceConfig config, ChangesTracker tracker, StringKeyValueStore store) {

        super(config, tracker);
        this.store = store;
        // Set up the tracker
        this.tracker = tracker;
        tracker.setSyncSource(this);
    }

    public void beginSync(int syncMode, boolean resume) throws SyncException {
        super.beginSync(syncMode, resume);
        try {
            store.load();
        } catch (Exception e) {
            // The store may not exist, try to save it so that it gets created
            try {
                store.save();
            } catch (Exception e1) {
                throw new SyncException(SyncException.CLIENT_ERROR, "Cannot load config store " + e.toString());
            }
        }
        // Shall be catched in order to end the sync successfully
        throw new SyncException(SyncException.CONTROLLED_INTERRUPTION, "Controlled interruption");
    }

    public void endSync() throws SyncException {

        super.endSync();
        
        try {
            store.save();
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, "Cannot save config store " + e.toString());
        }
    }

    protected Enumeration getAllItemsKeys() throws SyncException {
        Enumeration keys = store.keys();
        return keys;
    }

    public int addItem(SyncItem item) throws SyncException {
        String key = item.getKey();
        String value = new String(item.getContent());
        store.add(key, value);
        return SyncSource.SUCCESS_STATUS;
    }

    public int updateItem(SyncItem item) throws SyncException {
        return addItem(item);
    }
    
    public int deleteItem(String key) throws SyncException {
        store.remove(key);
        return SyncSource.SUCCESS_STATUS;
    }

    protected SyncItem getItemContent(final SyncItem item) throws SyncException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getItemContent");
        }
        String key = item.getKey();
        String value = store.get(key);
        SyncItem newItem = new SyncItem(item);
        newItem.setContent(value.getBytes());
        return newItem;
    }

    protected void deleteAllItems()
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "deleteAllItems");
        }
    }
    

}

