/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

package com.funambol.client.test;

import com.funambol.syncml.client.TrackableSyncSource;
import com.funambol.syncml.client.CacheTracker;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.syncml.spds.SyncException;

import com.funambol.storage.StringKeyValueMemoryStore;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

public class CheckSyncSource extends TrackableSyncSource {

    private static CacheTracker tracker = new CacheTracker(new StringKeyValueMemoryStore());

    private Hashtable allItemsWrapper = new Hashtable();

    private Vector newItemsFromServer = new Vector();
    private Vector updItemsFromServer = new Vector();
    private Vector delItemsFromServer = new Vector();

    public CheckSyncSource(String name, String type, String remoteUri) {
        super(new SourceConfig(name, type, remoteUri), tracker);
        config.setEncoding(SyncSource.ENCODING_NONE);
    }

    public void beginSync(int syncMode) throws SyncException {
        super.beginSync(syncMode);

        newItemsFromServer.removeAllElements();
        updItemsFromServer.removeAllElements();
        delItemsFromServer.removeAllElements();
    }

    public int addItem(SyncItem item) throws SyncException {
        super.addItem(item);
        allItemsWrapper.put(item.getKey(), item);
        newItemsFromServer.addElement(item);
        return SyncMLStatus.SUCCESS;
    }

    public int updateItem(SyncItem item) throws SyncException {
        super.updateItem(item);
        allItemsWrapper.put(item.getKey(), item);
        updItemsFromServer.addElement(item);
        return SyncMLStatus.SUCCESS;
    }

    public int deleteItem(String key) throws SyncException {
        super.deleteItem(key);
        allItemsWrapper.remove(key);
        delItemsFromServer.addElement(key);
        return SyncMLStatus.SUCCESS;
    }

    public Enumeration getAllItemsKeys() throws SyncException {
        return allItemsWrapper.keys();
    }

    public SyncItem getItemContent(SyncItem item) throws SyncException {
        return (SyncItem)allItemsWrapper.get(item.getKey());
    }

    //------------------- Methods used by automatic tests --------------------//

    public void addItemFromOutside(SyncItem item) throws SyncException {
        allItemsWrapper.put(item.getKey(), item);
    }

    public void updateItemFromOutside(SyncItem item) throws SyncException {
        allItemsWrapper.put(item.getKey(), item);
    }

    public void deleteItemFromOutside(String key) throws SyncException {
        allItemsWrapper.remove(key);
    }

    public void deleteAllFromOutside() throws SyncException {
        allItemsWrapper.clear();
    }

    public Enumeration getAddedItems() {
        return newItemsFromServer.elements();
    }

    public Enumeration getUpdatedItems() {
        return updItemsFromServer.elements();
    }

    public Enumeration getDeletedItems() {
        return delItemsFromServer.elements();
    }

    public Hashtable getAllItems() {
        return allItemsWrapper;
    }

    public int getAllItemsCount() {
        return allItemsWrapper.size();
    }

    protected void deleteAllItems() {
        allItemsWrapper.clear();
    }
}