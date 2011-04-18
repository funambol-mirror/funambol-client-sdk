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

import com.funambol.sync.SyncItem;

import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONException;

import com.funambol.util.StringUtil;

/**
 * Represents a SyncItem which holds a JSONFileObject
 */
public class JSONSyncItem extends SyncItem {

    private JSONFileObject fileObject = null;

    // This is used to handle rename operations
    private String oldKey = null;

    public JSONSyncItem(String key) {
        super(key);
    }

    public JSONSyncItem(String key, String type, char state, String parent,
            JSONObject jsonObject, String serverUrl) throws JSONException {
        this(key, type, state, parent, new JSONFileObject(jsonObject, serverUrl));
    }

    public JSONSyncItem(String key, String type, char state, String parent,
            JSONFileObject jsonFileObject) {
        super(key, type, state, parent);
        fileObject = jsonFileObject;
    }

    public JSONSyncItem(JSONSyncItem that) {
        super(that);
        fileObject = that.getJSONFileObject();
    }

    public void setOldKey(String key) {
        oldKey = key;
    }

    public String getOldKey() {
        return oldKey;
    }

    public JSONFileObject getJSONFileObject() {
        return fileObject;
    }

    /*
     * This method returns the url content for this item. This url can be
     * anything and just needs to point to the actual content. If there is
     * no remote content, but the content is withing the item itself, then this
     * method shall return null.
     */
    public String getContentUrl(String syncUrl) {
        if (fileObject != null) {
            return composeUrl(syncUrl, fileObject.getServerUrl(), fileObject.getUrl());
        } else {
            return null;
        }
    }

    public long getContentSize() {
        if (fileObject != null) {
            return fileObject.getSize();
        } else {
            return 0;
        }
    }

    public String getContentName() {
        if (fileObject != null) {
            return fileObject.getName();
        } else {
            return null;
        }
    }

    // Return the timestamp of the last modification for this item. If the item
    // is flowing client -> server then this is the timestamp of the last local
    // modification, otherwise it is the timestamp of the last remote
    // modification
    public long getLastModified() {
        return -1;
    }

    /**
     * Composes the url to use for the download operation.
     *
     * @param serverUrl
     * @param baseUrl
     * @param filename
     * @return
     */
    private String composeUrl(String syncUrl, String serverUrl, String baseUrl) {

        if(StringUtil.isNullOrEmpty(syncUrl)) {
            serverUrl = StringUtil.extractAddressFromUrl(syncUrl);
        }
        StringBuffer res = new StringBuffer();
        res.append(serverUrl);
        res.append(baseUrl);
        return res.toString();
    }

}
