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

import java.util.Vector;

import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONArray;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.DeviceConfigI;
import com.funambol.sync.TwinDetectionSource;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.*;

public class SapiSyncManagerTwinTest extends TestCase {

    private static final String TAG_LOG = "SapiSyncManagerTwinTest";
    
    private SapiSyncManager syncManager = null;
    private MockSapiSyncHandler sapiSyncHandler = null;
    private MockTwinSyncSource syncSource = null;
    private MockSyncListener syncSourceListener = null;

    private class MockTwinSyncSource extends MockSyncSource
            implements TwinDetectionSource {

        Vector twins = new Vector();
        
        public MockTwinSyncSource(SourceConfig config) {
            super(config);
        }
        
        public SyncItem findTwin(SyncItem item) {
            if(twins.contains(item.getKey())) {
                return item;
            }
            return null;
        }

        public void setTwins(Vector items) {
            twins = items;
        }
        
        protected void initAllItems() {
            allItems = new SyncItem[initialItemsCount];
            for(int i=0; i<initialItemsCount; i++) {
                allItems[i] = new SyncItem("Twin"+i);
            }
        }
    }

    public SapiSyncManagerTwinTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {
        
        syncManager = new SapiSyncManager(new SyncConfig(), new DeviceConfig());

        sapiSyncHandler = new MockSapiSyncHandler();
        syncManager.setSapiSyncHandler(sapiSyncHandler);
        
        syncSource = new MockTwinSyncSource(new SourceConfig(
                "test", "application/*", "test"));
        syncSourceListener = new MockSyncListener();
        syncSource.setListener(syncSourceListener);
    }

    public void tearDown() {
        syncManager = null;
    }

    public void testTwinDetection() throws Exception {
        
        Log.info(TAG_LOG, "testTwinDetection start");

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        // Assume 50 items on the client
        syncSource.setSyncAnchor(anchor);
        syncSource.setInitialItemsCount(50);

        // Assume 20 items on the server
        sapiSyncHandler.setItemsCount(20);
        JSONArray items = new JSONArray();

        // Assume all the server items are twin
        Vector twins = new Vector();
        for(int i=0;i<20;++i) {
            JSONObject item = new JSONObject();
            item.put("id", "Twin"+i);
            item.put("size", "1230");
            items.put(item);
            twins.add("Twin"+i);
        }
        JSONArray allItems[] = new JSONArray[1];
        allItems[0] = items;
        sapiSyncHandler.setItems(allItems);

        syncSource.setTwins(twins);

        syncManager.sync(syncSource);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        // Excluding twins we actually upload 30 items
        Vector uploaded = sapiSyncHandler.getUploadedItems();
        assertEquals(uploaded.size(), 30);

        assertEquals(syncSourceListener.getNumReceiving(), 0);
        assertEquals(syncSourceListener.getNumSending(), 30);
        
        Log.info(TAG_LOG, "testTwinDetection end");
    }

    private class DeviceConfig implements DeviceConfigI {
        public String getDevID() {
            return "test-device-id";
        }
    }
}
