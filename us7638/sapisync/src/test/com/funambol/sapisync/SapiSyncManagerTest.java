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

package com.funambol.sapisync;

import java.util.Hashtable;
import java.util.Vector;

import org.json.me.JSONException;
import org.json.me.JSONObject;
import org.json.me.JSONArray;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncFilter;
import com.funambol.sync.Filter;
import com.funambol.sync.BasicSyncListener;
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.*;

public class SapiSyncManagerTest extends TestCase {

    private SapiSyncManager syncManager = null;
    private MockSapiSyncHandler sapiSyncHandler = null;
    private MockSyncSource syncSource = null;
    private TestSyncListener syncSourceListener = null;

    private class TestSyncListener extends BasicSyncListener {

        private int numReceiving;
        private int numAdd;
        private int numUpd;
        private int numDel;

        public void startReceiving(int number) {
            numReceiving = number;
        }

        public void endReceiving() {
        }

        public void itemAddReceivingStarted(String key, String parent, long size) {
            numAdd++;
        }

        public void itemDeleted(SyncItem item) {
            numDel++;
        }

        public void itemReplaceReceivingStarted(String key, String parent, long size) {
            numUpd++;
        }

        public int getNumAdd() {
            return numAdd;
        }

        public int getNumUpd() {
            return numUpd;
        }

        public int getNumDel() {
            return numDel;
        }

        public int getNumReceiving() {
            return numReceiving;
        }
    }

    public SapiSyncManagerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {
        
        syncManager = new SapiSyncManager(new SyncConfig());

        sapiSyncHandler = new MockSapiSyncHandler();
        syncManager.setSapiSyncHandler(sapiSyncHandler);
        
        syncSource = new MockSyncSource(new SourceConfig(
                "test", "application/*", "test"));
        syncSourceListener = new TestSyncListener();
        syncSource.setListener(syncSourceListener);

        try {
            StringKeyValueStoreFactory mappingFactory = StringKeyValueStoreFactory.getInstance();
            StringKeyValueStore mapping = mappingFactory.getStringKeyValueStore("mapping_" + syncSource.getName());
            mapping.reset();
        } catch (Exception e) {
        }

    }

    public void tearDown() {
        syncManager = null;
    }

    public void testFullUpload() throws Exception {

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialItemsCount(100);

        syncManager.sync(syncSource);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 100);
    }

    public void testIncrementalUpload() throws Exception {

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(100);

        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialNewItemsCount(5);
        syncSource.setInitialUpdatedItemsCount(3);

        syncManager.sync(syncSource);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 8);

        Hashtable status = syncSource.getStatusTable();
        assertEquals(status.size(), 8);
    }

    public void testFullDownload1() throws Exception {
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);

        // In a full download we expect the count items sapi to be invoked and
        // then the sapi to retrive the list of all items
        sapiSyncHandler.setItemsCount(10);
        JSONArray items = new JSONArray();
        
        for(int i=0;i<10;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items.put(item);
        }
        JSONArray allItems[] = new JSONArray[1];
        allItems[0] = items;
        sapiSyncHandler.setItems(allItems);

        syncManager.sync(syncSource);
        // We expect 10 adds into the sync source
        assertEquals(syncSource.getAddedItemsCount(), 10);
        assertEquals(syncSourceListener.getNumAdd(), 10);
        assertEquals(syncSourceListener.getNumUpd(), 0);
        assertEquals(syncSourceListener.getNumDel(), 0);
        assertEquals(syncSourceListener.getNumReceiving(), 10);
    }

    public void testFullDownload2() throws Exception {

        // Simulate a first sync with enough items to require two different
        // SAPI requests. Check that the engine requires the right items.
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);

        // In a full download we expect the count items sapi to be invoked and
        // then the sapi to retrive the list of all items
        sapiSyncHandler.setItemsCount(310);
        JSONArray items0 = new JSONArray();
        // The SapiSyncManager asks 300 items per request
        for(int i=0;i<300;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items0.put(item);
        }

        JSONArray items1 = new JSONArray();
        // The SapiSyncManager asks 300 items per request
        for(int i=300;i<310;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items1.put(item);
        }

        JSONArray allItems[] = new JSONArray[2];
        allItems[0] = items0;
        allItems[1] = items1;
        sapiSyncHandler.setItems(allItems);

        syncManager.sync(syncSource);
        // We expect 310 adds into the sync source
        assertEquals(syncSource.getAddedItemsCount(), 310);
        assertEquals(syncSourceListener.getNumAdd(), 310);
        assertEquals(syncSourceListener.getNumUpd(), 0);
        assertEquals(syncSourceListener.getNumDel(), 0);
        assertEquals(syncSourceListener.getNumReceiving(), 310);
        assertEquals(sapiSyncHandler.getLimitRequests().size(), 2);
        assertEquals(sapiSyncHandler.getOffsetRequests().size(), 2);
        Vector limitRequests = sapiSyncHandler.getLimitRequests();
        Vector offsetRequests = sapiSyncHandler.getOffsetRequests();
        String limit0 = (String)limitRequests.elementAt(0);
        String limit1 = (String)limitRequests.elementAt(1);
        String offset0 = (String)offsetRequests.elementAt(0);
        String offset1 = (String)offsetRequests.elementAt(1);

        assertEquals(limit0, "300");
        assertEquals(limit1, "10");
        assertEquals(offset0, "0");
        assertEquals(offset1, "300");
    }

    public void testIncrementalDownload1() throws Exception {

        // Setup the mapping with the items that we pretend are already in the
        // local store
        StringKeyValueStoreFactory mappingFactory = StringKeyValueStoreFactory.getInstance();
        StringKeyValueStore mapping = mappingFactory.getStringKeyValueStore("mapping_" + syncSource.getName());
        for(int i=10;i<25;++i) {
            mapping.put(""+i, ""+i);
        }
        mapping.save();

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(100);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);
        // In an incremental download we expect the get changes API to be invoked and then the API to retrieve the
        // changed items
        JSONArray addItemKeys = new JSONArray();
        JSONArray addItems = new JSONArray();
        for(int i=0;i<10;++i) {
            addItemKeys.put(""+i);
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            addItems.put(item);
        }
        JSONArray updItemKeys = new JSONArray();
        JSONArray updItems = new JSONArray();
        for(int i=0;i<8;++i) {
            updItemKeys.put("" + (10 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (10 + i));
            item.put("size", "" + i);
            updItems.put(item);
        }
        JSONArray delItemKeys = new JSONArray();
        JSONArray delItems = new JSONArray();
        for(int i=0;i<7;++i) {
            delItemKeys.put("" + (18 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (18 + i));
            item.put("size", "" + i);
            delItems.put(item);
        }
        sapiSyncHandler.setIncrementalChanges(addItemKeys, updItemKeys, delItemKeys);
        JSONArray allArray[] = new JSONArray[3];
        allArray[0] = addItems;
        allArray[1] = updItems;
        allArray[2] = delItems;
        sapiSyncHandler.setItems(allArray);

        syncManager.sync(syncSource);
        // We expect 10 adds into the sync source
        assertEquals(syncSource.getAddedItemsCount(), 10);
        assertEquals(syncSource.getUpdatedItemsCount(), 8);
        assertEquals(syncSource.getDeletedItemsCount(), 7);
        assertEquals(syncSourceListener.getNumAdd(), 10);
        assertEquals(syncSourceListener.getNumUpd(), 8);
        assertEquals(syncSourceListener.getNumDel(), 7);
        assertEquals(syncSourceListener.getNumReceiving(), 25);
    }

    public void testIncrementalDownload2() throws Exception {

        // Setup the mapping with the items that we pretend are already in the
        // local store
        StringKeyValueStoreFactory mappingFactory = StringKeyValueStoreFactory.getInstance();
        StringKeyValueStore mapping = mappingFactory.getStringKeyValueStore("mapping_" + syncSource.getName());
        for(int i=110;i<140;++i) {
            mapping.put(""+i, ""+i);
        }
        mapping.save();

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(100);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);
        // In an incremental download we expect the get changes API to be invoked and then the API to retrieve the
        // changed items
        JSONArray addItemKeys = new JSONArray();
        JSONArray addItems0 = new JSONArray();
        for(int i=0;i<100;++i) {
            addItemKeys.put(""+i);
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            addItems0.put(item);
        }
        JSONArray addItems1 = new JSONArray();
        for(int i=0;i<10;++i) {
            addItemKeys.put(""+ (100 +i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (100 + i));
            item.put("size", "" + i);
            addItems1.put(item);
        }
        JSONArray updItemKeys = new JSONArray();
        JSONArray updItems = new JSONArray();
        for(int i=0;i<20;++i) {
            updItemKeys.put("" + (110 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (110 + i));
            item.put("size", "" + i);
            updItems.put(item);
        }
        JSONArray delItemKeys = new JSONArray();
        JSONArray delItems = new JSONArray();
        for(int i=0;i<10;++i) {
            delItemKeys.put("" + (130 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (130 + i));
            item.put("size", "" + i);
            delItems.put(item);
        }
        sapiSyncHandler.setIncrementalChanges(addItemKeys, updItemKeys, delItemKeys);
        JSONArray allArray[] = new JSONArray[4];
        allArray[0] = addItems0;
        allArray[1] = addItems1;
        allArray[2] = updItems;
        allArray[3] = delItems;
        sapiSyncHandler.setItems(allArray);

        syncManager.sync(syncSource);
        // We expect 10 adds into the sync source
        assertEquals(syncSource.getAddedItemsCount(), 110);
        assertEquals(syncSource.getUpdatedItemsCount(), 20);
        assertEquals(syncSource.getDeletedItemsCount(), 10);
        assertEquals(syncSourceListener.getNumAdd(), 110);
        assertEquals(syncSourceListener.getNumUpd(), 20);
        assertEquals(syncSourceListener.getNumDel(), 10);
        assertEquals(syncSourceListener.getNumReceiving(), 140);
        // Since the number of add is greater than 100, we expect that two SAPI
        // are invoked to the the list of changes. Therefore we have two
        // invokations for the "adds" and one for the "updates" (no one for the
        // deletes)
        assertEquals(sapiSyncHandler.getIdsRequests().size(), 3);
    }
    
    public void testFullUploadFilter_ItemsCount() {
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        SyncFilter syncFilter = new SyncFilter();
        syncFilter.setFullUploadFilter(
            new Filter(Filter.ITEMS_COUNT_TYPE, -1, 8));
        
        syncSource.setFilter(syncFilter);
        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialItemsCount(100);

        syncManager.sync(syncSource);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 8);
    }
    
    public void testIncrementalUploadFilter_ItemsCount() {
    
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(100);

        SyncFilter syncFilter = new SyncFilter();
        syncFilter.setIncrementalUploadFilter(
            new Filter(Filter.ITEMS_COUNT_TYPE, -1, 12));
        
        syncSource.setFilter(syncFilter);
        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialNewItemsCount(40);

        syncManager.sync(syncSource);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 12);

        Hashtable status = syncSource.getStatusTable();
        assertEquals(status.size(), 12);
    }
    
    public void testFullDownloadFilter_ItemsCount() throws Exception {
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);
        
        SyncFilter syncFilter = new SyncFilter();
        syncFilter.setFullDownloadFilter(
            new Filter(Filter.ITEMS_COUNT_TYPE, -1, 3));
        
        syncSource.setFilter(syncFilter);

        // In a full download we expect the count items sapi to be invoked and
        // then the sapi to retrive the list of all items
        sapiSyncHandler.setItemsCount(10);
        JSONArray items = new JSONArray();
        
        for(int i=0;i<10;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items.put(item);
        }
        JSONArray allItems[] = new JSONArray[1];
        allItems[0] = items;
        sapiSyncHandler.setItems(allItems);

        syncManager.sync(syncSource);
        
        assertEquals(sapiSyncHandler.getLimitRequests().size(), 1);
        assertEquals(sapiSyncHandler.getOffsetRequests().size(), 1);
        
        Vector limitRequests = sapiSyncHandler.getLimitRequests();
        Vector offsetRequests = sapiSyncHandler.getOffsetRequests();
        String limit0 = (String)limitRequests.elementAt(0);
        String offset0 = (String)offsetRequests.elementAt(0);

        assertEquals(limit0, "3");
        assertEquals(offset0, "0");
    }
    
    public void testFullDownloadFilter_ItemsCount2() throws Exception {
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);
        
        SyncFilter syncFilter = new SyncFilter();
        syncFilter.setFullDownloadFilter(
            new Filter(Filter.ITEMS_COUNT_TYPE, -1, 309));
        
        syncSource.setFilter(syncFilter);

        // In a full download we expect the count items sapi to be invoked and
        // then the sapi to retrive the list of all items
        sapiSyncHandler.setItemsCount(340);
        JSONArray items = new JSONArray();
        
        for(int i=0;i<300;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items.put(item);
        }
        
        JSONArray items1 = new JSONArray();
        // The SapiSyncManager asks 300 items per request
        for(int i=300;i<340;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            items1.put(item);
        }
        
        JSONArray allItems[] = new JSONArray[2];
        allItems[0] = items;
        allItems[1] = items1;
        sapiSyncHandler.setItems(allItems);

        syncManager.sync(syncSource);
        
        assertEquals(sapiSyncHandler.getLimitRequests().size(), 2);
        assertEquals(sapiSyncHandler.getOffsetRequests().size(), 2);
        
        Vector limitRequests = sapiSyncHandler.getLimitRequests();
        Vector offsetRequests = sapiSyncHandler.getOffsetRequests();
        String limit0 = (String)limitRequests.elementAt(0);
        String limit1 = (String)limitRequests.elementAt(1);
        String offset0 = (String)offsetRequests.elementAt(0);
        String offset1 = (String)offsetRequests.elementAt(1);

        assertEquals(limit0, "300");
        assertEquals(offset0, "0");
        assertEquals(limit1, "9");
        assertEquals(offset1, "300");
    }
}
