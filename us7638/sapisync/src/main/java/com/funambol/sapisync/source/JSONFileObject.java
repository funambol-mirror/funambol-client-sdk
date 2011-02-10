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
import org.json.me.JSONArray;

import org.json.me.JSONException;
import org.json.me.JSONObject;

/**
 * Represents a JSON object containing a file meta data. It can also contain a
 * Vector of JSONFileThumbnail objects.
 */
public class JSONFileObject extends JSONObject {

    private String id;
    private String name;
    private String url;
    
    private long date;
    private long size;

    private Vector thumbnails;

    public JSONFileObject(String json) throws JSONException {
        super(json);

        this.id   = getString("id");
        this.name = getString("name");
        this.url  = getString("url");

        this.date = getLong("date");
        this.size = getLong("size");

        if (this.has("thumbnails")) {
            JSONArray thumbs = getJSONArray("thumbnails");
            if(thumbs != null) {
                this.thumbnails = new Vector();
                for(int i=0; i<thumbs.length(); i++) {
                    JSONObject thumb = thumbs.getJSONObject(i);
                    this.thumbnails.addElement(new JSONFileThumbnail(thumb));
                }
            }
        }
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getUrl() {
        return url;
    }

    public long getDate() {
        return date;
    }

    public long getSize() {
        return size;
    }

    public Vector getThumbnails() {
        return thumbnails;
    }

    public class JSONFileThumbnail {

        private String size;
        private String url;

        public JSONFileThumbnail(JSONObject json) throws JSONException {
            this.size = json.getString("size");
            this.url  = getString("url");
        }

        public String getSize() {
            return size;
        }

        public String getUrl() {
            return url;
        }
    }
}
