/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.jsync;

import java.util.Properties;
import java.util.Set;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.IOException;

import com.funambol.syncml.protocol.DataStore;
import com.funambol.syncml.spds.SourceConfig;

public class JSyncSourceConfig extends SourceConfig {

    private static final String KEY_NAME = "name";
    private static final String KEY_TYPE = "type";
    private static final String KEY_ENCODING = "encoding";
    private static final String KEY_SYNC_MODE = "sync-mode";
    private static final String KEY_REMOTE_URI = "remote-uri";
    private static final String KEY_LAST_ANCHOR = "last-anchor";
    private static final String KEY_NEXT_ANCHOR = "next-anchor";
    private static final String KEY_MAX_ITEMS_PER_MESSAGE = "max-items-per-message-in-slow-sync";
    private static final String KEY_BREAK_ON_LAST_CHUNK = "break-on-last-chunk";

    private static final String TRUE  = "true";
    private static final String FALSE = "false";

    public JSyncSourceConfig() {
        super();
    }

    public JSyncSourceConfig(String name, String type, String remoteUri) {
        super(name, type, remoteUri);
    }

    public JSyncSourceConfig(String name, String type, String remoteUri, DataStore ds) {
        super(name, type, remoteUri, ds);
    }

    public void save(String fileName) throws IOException {

        Properties props = new Properties();
        String name = getName();
        if (name != null) {
            props.put(KEY_NAME, name);
        }
        String type = getName();
        if (type != null) {
            props.put(KEY_TYPE, type);
        }
        String encoding = getEncoding();
        if (encoding != null) {
            props.put(KEY_ENCODING, encoding);
        }
        int syncMode = getSyncMode();
        props.put(KEY_SYNC_MODE, "" + syncMode);
        String remoteUri = getRemoteUri();
        if (remoteUri != null) {
            props.put(KEY_REMOTE_URI, remoteUri);
        }
        long lastAnchor = getLastAnchor();
        props.put(KEY_LAST_ANCHOR, "" + lastAnchor);

        long nextAnchor = getNextAnchor();
        props.put(KEY_NEXT_ANCHOR, "" + nextAnchor);

        int maxItemsPerMessageInSlowSync = getMaxItemsPerMessageInSlowSync();
        props.put(KEY_MAX_ITEMS_PER_MESSAGE, "" + maxItemsPerMessageInSlowSync);

        boolean breakOnLastChunk = getBreakMsgOnLastChunk();
        props.put(KEY_BREAK_ON_LAST_CHUNK, breakOnLastChunk ? TRUE : FALSE);

        FileOutputStream os = new FileOutputStream(fileName);
        props.store(os, "Source configuration");
        os.close();
    }

    public void load(String fileName) throws IOException {
        FileInputStream is = new FileInputStream(fileName);
        Properties props = new Properties();
        props.load(is);

        Set keys = props.keySet();
        for(Object key : keys) {
            String k = (String)key;
            String v = (String)props.get(k);

            if (KEY_NAME.equals(k)) {
                setName(v);
            } else if (KEY_TYPE.equals(k)) {
                setType(v);
            } else if (KEY_ENCODING.equals(k)) {
                setEncoding(v);
            } else if (KEY_SYNC_MODE.equals(k)) {
                int syncMode = Integer.parseInt(v);
                setSyncMode(syncMode);
            } else if (KEY_REMOTE_URI.equals(k)) {
                setRemoteUri(v);
            } else if (KEY_LAST_ANCHOR.equals(k)) {
                long lastAnchor = Long.parseLong(v);
                setLastAnchor(lastAnchor);
            } else if (KEY_NEXT_ANCHOR.equals(k)) {
                long nextAnchor = Long.parseLong(v);
                setNextAnchor(nextAnchor);
            } else if (KEY_MAX_ITEMS_PER_MESSAGE.equals(k)) {
                int maxItemsPerMessageInSlowSync = Integer.parseInt(v);
                setMaxItemsPerMessageInSlowSync(maxItemsPerMessageInSlowSync);
            } else if (KEY_BREAK_ON_LAST_CHUNK.equals(k)) {
                if (v.toLowerCase().equals(TRUE)) {
                    setBreakMsgOnLastChunk(true);
                }
            } else {
                throw new IOException("Unknown property in source configuration");
            }
        }
        is.close();
    }
}
