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

package com.funambol.sapisync.source;

import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.client.TrackableSyncSource;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;
import java.util.Enumeration;
import java.util.Vector;

import junit.framework.*;

public class CacheTrackerWithRenamesTest extends TestCase {

    MockSyncSource ss = null;
    TestCacheTrackerWithRenames tracker = null;

    StringKeyValueStore statusStore = null;
    StringKeyValueStore renamesStore = null;
    
    public CacheTrackerWithRenamesTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {

        StringKeyValueStoreFactory storeFactory = StringKeyValueStoreFactory.getInstance();
        
        statusStore = storeFactory.getStringKeyValueStore("test_source_status");
        renamesStore = storeFactory.getStringKeyValueStore("test_source_renames");

        try {
            statusStore.reset();
            renamesStore.reset();
        } catch(Exception ex) {}

        tracker = new TestCacheTrackerWithRenames(renamesStore, statusStore);

        ss = new MockSyncSource(new SourceConfig("name", "type", "uri"), tracker);

        tracker.setSyncSource(ss);
    }

    public void tearDown() {
    }

    private class TestCacheTrackerWithRenames extends CacheTrackerWithRenames {

        public TestCacheTrackerWithRenames(StringKeyValueStore renamesStore,
                StringKeyValueStore statusStore) {
            super(renamesStore, statusStore);
        }

        protected String computeFingerprint(SyncItem item) {
            return "fingerprint";
        }

    }

    private class MockSyncSource extends TrackableSyncSource {

        Vector items = new Vector();
        
        public MockSyncSource(SourceConfig config, ChangesTracker tracker) {
            super(config, tracker);
        }

        public void addItemKey(String key) {
            items.add(key);
        }

        public void resetItems() {
            items.clear();
        }
        
        protected Enumeration getAllItemsKeys() {
            return items.elements();
        }

        protected SyncItem getItemContent(SyncItem item) throws SyncException {
            return item;
        }
    }

    public void testRename() throws Throwable {

        ss.addItemKey("old.txt");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        tracker.fileRenamed("old.txt", "new.txt");
        assertEquals(renamesStore.get("new.txt"), "old.txt");

        ss.resetItems();
        ss.addItemKey("new.txt");
        
        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 1);

        assertTrue(tracker.isRenamedItem("new.txt"));
        assertEquals(tracker.getRenamedFileName("new.txt"), "old.txt");

        Enumeration updated = tracker.getUpdatedItems();
        assertEquals(updated.nextElement(), "new.txt");
 
        tracker.setItemStatus("new.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        assertEquals(statusStore.get("new.txt"), "fingerprint");
        assertTrue(!renamesStore.keys().hasMoreElements());
    }

    public void testRename_twice() throws Throwable {

        ss.addItemKey("old.txt");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        tracker.fileRenamed("old.txt", "temp.txt");
        assertEquals(renamesStore.get("temp.txt"), "old.txt");

        tracker.fileRenamed("temp.txt", "new.txt");
        assertEquals(renamesStore.get("new.txt"), "old.txt");

        ss.resetItems();
        ss.addItemKey("new.txt");

        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 1);

        assertTrue(tracker.isRenamedItem("new.txt"));
        assertEquals(tracker.getRenamedFileName("new.txt"), "old.txt");

        Enumeration updated = tracker.getUpdatedItems();
        assertEquals(updated.nextElement(), "new.txt");

        tracker.setItemStatus("new.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        assertEquals(statusStore.get("new.txt"), "fingerprint");
        assertTrue(!renamesStore.keys().hasMoreElements());
    }

    public void testRename_twice2() throws Throwable {

        ss.addItemKey("old.txt");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        tracker.fileRenamed("old.txt", "temp.txt");
        assertEquals(renamesStore.get("temp.txt"), "old.txt");

        tracker.fileRenamed("temp.txt", "old.txt");
        assertTrue(!renamesStore.keys().hasMoreElements());

        ss.resetItems();
        ss.addItemKey("old.txt");

        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        assertTrue(!tracker.isRenamedItem("old.txt"));

        tracker.end();
    }

    public void testEmpty() throws Throwable {

        ss.addItemKey("old.txt");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        tracker.fileRenamed("old.txt", "new.txt");

        assertEquals(statusStore.get("old.txt"), "fingerprint");
        assertEquals(renamesStore.get("new.txt"), "old.txt");
        
        tracker.empty();

        assertTrue(!statusStore.keys().hasMoreElements());
        assertTrue(!renamesStore.keys().hasMoreElements());

        ss.resetItems();
        
        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        tracker.end();
    }

    public void testReset() throws Throwable {

        ss.addItemKey("old.txt");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        tracker.fileRenamed("old.txt", "new.txt");

        assertEquals(statusStore.get("old.txt"), "fingerprint");
        assertEquals(renamesStore.get("new.txt"), "old.txt");

        tracker.reset();

        assertEquals(statusStore.get("old.txt"), "fingerprint");
        assertTrue(!renamesStore.keys().hasMoreElements());

        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        tracker.end();
    }

}
