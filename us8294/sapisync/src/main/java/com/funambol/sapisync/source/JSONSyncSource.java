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

import java.util.Vector;
import java.io.IOException;
import java.io.OutputStream;

import com.funambol.sync.SyncItem;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.ItemDownloadInterruptionException;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.client.TrackableSyncSource;
import com.funambol.sync.Filter;

import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONException;
import com.funambol.sync.NonBlockingSyncException;
import com.funambol.util.Log;

/**
 * Represents a SyncSource which handles JSON file objects as input SyncItems.
 * You should define the getDownloadOutputStream in order to provide the
 * OutputSteam used to download the file.
 */
public abstract class JSONSyncSource extends TrackableSyncSource {

    private static final String TAG_LOG = "JSONSyncSource";

    private String dataTag = null;

    private class JSONSyncSourceItem extends JSONSyncItem {

        public JSONSyncSourceItem(String key, String type, char state, String parent,
                                  JSONObject jsonObject, String serverUrl)
        throws JSONException
        {
            super(key, type, state, parent, jsonObject, serverUrl);
        }

        public JSONSyncSourceItem(String key, String type, char state, String parent,
                                  JSONFileObject jsonFileObject)
        throws JSONException
        {
            super(key, type, state, parent, jsonFileObject);
        }

        /**
         * Returns an OutputStream to write data to. In this default implementation
         * a ByteArrayOutputStream is used to store the content. SyncSource that
         * need to manipulate bug items shall redefine this method to use a non
         * memory based sync item.
         */
        public OutputStream getOutputStream() throws IOException {
            return getDownloadOutputStream(getJSONFileObject(), 
                    getState() == SyncItem.STATE_UPDATED, false,
                    getPartialLength() > 0);
        }
    }

    //------------------------------------------------------------- Constructors

    /**
     * JSONSyncSource constructor: initialize source config
     */
    public JSONSyncSource(SourceConfig config, ChangesTracker tracker) {
        super(config, tracker);
    }

    public SyncItem createSyncItem(String key, String type, char state,
                                   String parent, JSONObject json,
                                   String serverUrl) throws JSONException {
        JSONSyncSourceItem item = new JSONSyncSourceItem(key, type, state, parent, json, serverUrl);
        return item;
    }

    public void applyChanges(Vector syncItems) throws SyncException {
        
        for(int i = 0; i < syncItems.size(); ++i) {
            int status = UNDEFINED_STATUS;
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
            } catch (ItemDownloadInterruptionException ide) {
                // The download got interrupted with a network error (this
                // interrupts the application of other items)
                throw ide;
            } catch (NonBlockingSyncException se) {
                // We interrupt the sync, but first we mark this item
                // according to the type of non-blocking exception raised
                if (se.getCode() == SyncException.LOCAL_DEVICE_FULL) {
                    // This item is invalid, nullify its key so that it will
                    // not be persisted in the mapping
                    item.setSyncStatus(DEVICE_FULL_ERROR_STATUS);
                    throw se;
                }
            } catch (Exception e) {
                status = ERROR_STATUS;
            }
            item.setSyncStatus(status);
        }
    }

    /**
     * This method returns the tag name in the JSONobject for the specific
     * type of data handled by this source. Refer to the SAPI documentation
     * for more info.
     */
    public String getDataTag() {
        return dataTag;
    }

    public void setDataTag(String dataTag) {
        this.dataTag = dataTag;
    }

    protected OutputStream getDownloadOutputStream(JSONFileObject jsonItem,
            boolean isUpdate, boolean isThumbnail, boolean append) throws IOException {
        return getDownloadOutputStream(
                jsonItem.getName(),
                jsonItem.getSize(),
                isUpdate,
                isThumbnail,
                append);
    }

    /**
     * Must be implemented in order to provide a proper OutputStream to download
     * the item.
     * 
     * @param name
     * @param size
     * @param isUpdate
     * @param isThumbnail
     * @return
     */
    protected abstract OutputStream getDownloadOutputStream(String name,
            long size, boolean isUpdate, boolean isThumbnail, boolean append) throws IOException;

}
