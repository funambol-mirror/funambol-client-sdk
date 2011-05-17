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

import java.util.Hashtable;

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
    CacheTrackerWithRenames tracker = null;

    StringKeyValueStore statusStore = null;
    
    public CacheTrackerWithRenamesTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {

        StringKeyValueStoreFactory storeFactory = StringKeyValueStoreFactory.getInstance();
        
        statusStore = storeFactory.getStringKeyValueStore("test_source_status");

        try {
            statusStore.reset();
        } catch(Exception ex) {}

        tracker = new CacheTrackerWithRenames("testsource", statusStore);

        ss = new MockSyncSource(new SourceConfig("name", "type", "uri"), tracker);

        tracker.setSyncSource(ss);
    }

    public void tearDown() {
    }

    private class MockSyncSource extends TrackableSyncSource {

        private Hashtable items = new Hashtable();
        
        public MockSyncSource(SourceConfig config, ChangesTracker tracker) {
            super(config, tracker);
        }

        public void addItem(String key, String content) {
            items.put(key, content);
        }

        public void resetItems() {
            items.clear();
        }
        
        protected Enumeration getAllItemsKeys() {
            return items.keys();
        }

        protected SyncItem getItemContent(SyncItem item) throws SyncException {
            String content = (String)items.get(item.getKey());
            SyncItem res = new SyncItem(item.getKey());
            res.setContent(content.getBytes());
            return res;
        }
    }

    public void testRename() throws Throwable {

        ss.addItem("old.txt", "Content0");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        ss.resetItems();
        ss.addItem("new.txt", "Content0");
        
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
    }

    public void testRenameAndUpdate() throws Throwable {

        ss.addItem("old.txt", "Content0");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        ss.resetItems();
        ss.addItem("new.txt", "Content1");
        
        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 1);
        assertEquals(tracker.getDeletedItemsCount(), 1);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        assertTrue(!tracker.isRenamedItem("new.txt"));
        tracker.setItemStatus("new.txt", SyncSource.SUCCESS_STATUS);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();
    }


    public void testEmpty() throws Throwable {

        ss.addItem("old.txt", "Content0");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        ss.addItem("new.txt", "Content0");

        tracker.empty();
        ss.resetItems();
        
        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        tracker.end();
    }

    public void testReset() throws Throwable {
        ss.addItem("old.txt", "Content0");
        tracker.begin(SyncSource.FULL_UPLOAD, true);
        tracker.setItemStatus("old.txt", SyncSource.SUCCESS_STATUS);
        tracker.end();

        ss.addItem("new.txt", "Content0");

        tracker.reset();

        tracker.begin(SyncSource.INCREMENTAL_UPLOAD, true);

        assertEquals(tracker.getNewItemsCount(), 0);
        assertEquals(tracker.getDeletedItemsCount(), 0);
        assertEquals(tracker.getUpdatedItemsCount(), 0);

        tracker.end();
    }

}
