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

import java.util.*;

import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONArray;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncFilter;
import com.funambol.sync.Filter;
import com.funambol.sync.DeviceConfigI;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.*;

public class SapiSyncManagerTest extends TestCase {
    private static final String TAG_LOG = "SapiSyncManagerTest";
    
    private SapiSyncManager syncManager = null;
    private MockSapiSyncHandler sapiSyncHandler = null;
    private MockSyncSource syncSource = null;
    private MockSyncListener syncSourceListener = null;

    public SapiSyncManagerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {
        
        syncManager = new SapiSyncManager(new SyncConfig(), new DeviceConfig());

        sapiSyncHandler = new MockSapiSyncHandler();
        syncManager.setSapiSyncHandler(sapiSyncHandler);
        
        syncSource = new MockSyncSource(new SourceConfig(
                "test", "application/*", "test"));
        syncSourceListener = new MockSyncListener();
        syncSource.setListener(syncSourceListener);

        try {
            MappingTable mapping = new MappingTable(syncSource.getName());
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

        assertEquals(syncSourceListener.getNumSending(), 100);
        
        // Check mapping table
        MappingTable mapping = new MappingTable(syncSource.getName());
        try {
            mapping.load();
        } catch (Exception e) { }
        
        assertTrue(mapping != null);
        
        int mCount = 0;
        Enumeration keys = mapping.keys();
        while(keys.hasMoreElements()) {
            mCount++;
            String guid = (String)keys.nextElement();
            String luid = mapping.getLuid(guid);
            assertEquals(guid, "guid_"+luid);
        }
        assertEquals(mCount, 100);
    }

    public void testIncrementalUpload() throws Exception {

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(100);

        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialNewItemsCount(8);

        syncManager.sync(syncSource);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 8);

        Hashtable status = syncSource.getStatusTable();
        assertEquals(status.size(), 8);

        assertEquals(syncSourceListener.getNumSending(), 8);
        
        // Check mapping table
        MappingTable mapping = new MappingTable(syncSource.getName());
        try {
            mapping.load();
        } catch (Exception e) { }
        
        assertTrue(mapping != null);
        
        int mCount = 0;
        Enumeration keys = mapping.keys();
        while(keys.hasMoreElements()) {
            mCount++;
            String guid = (String)keys.nextElement();
            String luid = mapping.getLuid(guid);
            assertEquals(guid, "guid_"+luid);
        }
        assertEquals(mCount, 8);
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
            item.put("name", "" + i);
            item.put("date", (long)i);
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
            item.put("name", "" + i);
            item.put("date", (long)i);
            items0.put(item);
        }

        JSONArray items1 = new JSONArray();
        // The SapiSyncManager asks 300 items per request
        for(int i=300;i<310;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
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
        Log.info(TAG_LOG, "testIncrementalDownload1 starts");

        // Setup the mapping with the items that we pretend are already in the
        // local store
        MappingTable mapping = new MappingTable(syncSource.getName());
        for(int i=10;i<25;++i) {
            mapping.add(""+i, ""+i, ""+i, ""+i);
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
            item.put("name", "" + i);
            item.put("date", (long)i);
            addItems.put(item);
        }
        JSONArray updItemKeys = new JSONArray();
        JSONArray updItems = new JSONArray();
        for(int i=0;i<8;++i) {
            updItemKeys.put("" + (10 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (10 + i));
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
            updItems.put(item);
        }
        JSONArray delItemKeys = new JSONArray();
        JSONArray delItems = new JSONArray();
        for(int i=0;i<7;++i) {
            delItemKeys.put("" + (18 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (18 + i));
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
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
        assertEquals(10, syncSource.getAddedItemsCount());
        assertEquals(8, syncSource.getUpdatedItemsCount());
        assertEquals(7, syncSource.getDeletedItemsCount());
        assertEquals(10, syncSourceListener.getNumAdd());
        assertEquals(8, syncSourceListener.getNumUpd());
        assertEquals(7, syncSourceListener.getNumDel());
        assertEquals(25, syncSourceListener.getNumReceiving());
    }

    public void testIncrementalDownload2() throws Exception {
        // Setup the mapping with the items that we pretend are already in the
        // local store
        MappingTable mapping = new MappingTable(syncSource.getName());
        for(int i=110;i<140;++i) {
            mapping.add(""+i, ""+i, ""+i, ""+i);
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
        for(int i=0;i<110;++i) {
            addItemKeys.put(""+i);
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
            addItems.put(item);
        }
        JSONArray updItemKeys = new JSONArray();
        JSONArray updItems = new JSONArray();
        for(int i=0;i<20;++i) {
            updItemKeys.put("" + (110 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (110 + i));
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
            updItems.put(item);
        }
        JSONArray delItemKeys = new JSONArray();
        JSONArray delItems = new JSONArray();
        for(int i=0;i<10;++i) {
            delItemKeys.put("" + (130 + i));
            JSONObject item = new JSONObject();
            item.put("id", "" + (130 + i));
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
            delItems.put(item);
        }
        sapiSyncHandler.setIncrementalChanges(addItemKeys, updItemKeys, delItemKeys);
        JSONArray allArray[] = new JSONArray[4];
        allArray[0] = addItems;
        allArray[1] = updItems;
        allArray[2] = delItems;
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
        assertEquals(sapiSyncHandler.getIdsRequests().size(), 2);
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
            item.put("date", (long)i);
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

        assertEquals(limit0, "10");
        assertEquals(offset0, "0");

        assertEquals(syncSourceListener.getNumReceiving(), 3);
        assertEquals(syncSourceListener.getNumAdd(), 3);
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
            item.put("name", "" + i);
            item.put("date", (long)i);
            items.put(item);
        }
        
        JSONArray items1 = new JSONArray();
        // The SapiSyncManager asks 300 items per request
        for(int i=300;i<340;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
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
        assertEquals(limit1, "40");
        assertEquals(offset1, "300");

        assertEquals(syncSourceListener.getNumReceiving(), 309);
        assertEquals(syncSourceListener.getNumAdd(), 309);
    }

    public void testFullDownloadFilter_DateRecent() throws Exception {
        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        syncSource.setSyncAnchor(anchor);

        SyncFilter syncFilter = new SyncFilter();
        syncFilter.setFullDownloadFilter(
            new Filter(Filter.DATE_RECENT_TYPE, 1234567890, 0));

        syncSource.setFilter(syncFilter);

        // In a full download we expect the count items sapi to be invoked and
        // then the sapi to retrive the list of all items
        sapiSyncHandler.setItemsCount(10);
        JSONArray items = new JSONArray();

        for(int i=0;i<10;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "" + i);
            item.put("size", "" + i);
            item.put("name", "" + i);
            item.put("date", (long)i);
            item.put("datecreated", (long)100);
            items.put(item);
        }
        JSONArray allItems[] = new JSONArray[1];
        allItems[0] = items;
        sapiSyncHandler.setItems(allItems);

        syncManager.sync(syncSource);

        Vector dateLimitAllRequests = sapiSyncHandler.getDateLimitAllRequests();
        Vector dateLimitRequests = sapiSyncHandler.getDateLimitRequests();
       
        // We never specify the fromdate in our requests because we need to
        // perform full twin detection, so we need to know everything available
        // on the server
        assertEquals(dateLimitAllRequests.size(), 0);
        assertEquals(dateLimitRequests.size(), 0);
    }

    private class DeviceConfig implements DeviceConfigI {
        public String getDevID() {
            return "test-device-id";
        }
    }
}
