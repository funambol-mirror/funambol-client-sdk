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

import java.util.Enumeration;
import java.io.IOException;

import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValuePair;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;


public class MappingTable {

    private static final String TAG_LOG = "MappingTable";

    private StringKeyValueStore store;

    public MappingTable(String sourceName) {
        StringKeyValueStoreFactory mappingFactory = StringKeyValueStoreFactory.getInstance();
        store = mappingFactory.getStringKeyValueStore("mapping_" + sourceName);
    }

    public void reset() throws IOException {
        store.reset();
    }

    public void load() throws IOException {
        store.load();
    }

    public void save() throws IOException {
        store.save();
    }

    public void remove(String guid) {
        store.remove(guid);
    }

    public void add(String guid, String luid, String crc, String name) {
        String value = createValue(luid, crc, name);
        store.add(guid, value);
    }

    public String getLuid(String guid) {
        String value = store.get(guid);
        return getFieldFromValue(value, 0);
    }

    public String getCRC(String guid) {
        String value = store.get(guid);
        return getFieldFromValue(value, 1);
    }

    public String getName(String guid) {
        String value = store.get(guid);
        return getFieldFromValue(value, 2);
    }

    public Enumeration keyValuePairs() {
        return store.keyValuePairs();
    }

    public Enumeration keys() {
        return store.keys();
    }

    public String getGuid(String luid) {
        Enumeration keyValuePairs = store.keyValuePairs();
        while (keyValuePairs.hasMoreElements()) {
            StringKeyValuePair pair = (StringKeyValuePair)keyValuePairs.nextElement();
            String value = pair.getValue();
            if (luid.equals(getFieldFromValue(value, 0))) {
                return pair.getKey();
            }
        }
        return null;
    }

    public void add(String guid, String luid) {
        throw new IllegalArgumentException("Missing CRC");
    }

    public void put(String guid, String luid) {
        throw new IllegalArgumentException("Missing CRC");
    }

    public void update(String guid, String luid) {
        throw new IllegalArgumentException("Missing CRC");
    }

    // Both luid and crc cannot contain commas
    private String createValue(String luid, String crc, String name) {
        StringBuffer buf = new StringBuffer();
        buf.append(luid).append(",").append(crc).append(",").append(name);
        return buf.toString();
    }

    private String getFieldFromValue(String value, int idx) {
        if (value == null) {
            return null;
        }
        String v[] = StringUtil.split(value, ",");
        return v[idx];
    }
}

 
