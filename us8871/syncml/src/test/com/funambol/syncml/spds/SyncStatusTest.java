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

package com.funambol.syncml.spds;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

import java.util.Hashtable;
import java.util.Vector;
import junit.framework.*;

import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.spds.SyncStatus;
import com.funambol.storage.FileTable;
import com.funambol.storage.StringKeyValueMemoryStore;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueStoreFactory;
import com.funambol.storage.Table;
import com.funambol.storage.TableFactory;

public class SyncStatusTest extends TestCase {

    private SyncStatus syncStatus;

    public SyncStatusTest(String name) {
        super(name);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {

        syncStatus = new SyncStatus("test");

        try {
            syncStatus.reset();
        } catch (Exception e) {
            fail("Unable to reset syncStatus at the beginning of a test");
        }

    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {
        if (syncStatus != null) {
            try {
                syncStatus.reset();
            } catch (Exception e) {
                fail("Unable to reset syncStatus at the end of a test");
            }
        }
    }

    public void testSentItems1() throws Throwable {

        syncStatus.addSentItem("1", SyncML.TAG_ADD);
        syncStatus.addSentItem("2", SyncML.TAG_ADD);
        syncStatus.addSentItem("3", SyncML.TAG_ADD);
        assertTrue(syncStatus.getSentItemsCount() == 3);
        assertTrue(syncStatus.getSentItemStatus("1") == -1);
        assertTrue(syncStatus.getSentItemStatus("2") == -1);
        assertTrue(syncStatus.getSentItemStatus("3") == -1);
        assertTrue(syncStatus.getSentItemStatus("4") == -1);
        syncStatus.receivedItemStatus("1", 200);
        assertTrue(syncStatus.getSentItemsCount() == 3);
        assertTrue(syncStatus.getSentItemStatus("1") == 200);

        // Now save everything
        syncStatus.save();
        
        // Nothing should have changed...
        assertTrue(syncStatus.getSentItemsCount() == 3);
        assertTrue(syncStatus.getSentItemStatus("2") == -1);
        assertTrue(syncStatus.getSentItemStatus("3") == -1);
        assertTrue(syncStatus.getSentItemStatus("4") == -1);
        assertTrue(syncStatus.getSentItemStatus("1") == 200);
    }

    public void testReceivedItems1() throws Throwable {

        syncStatus.addReceivedItem("guid1", "luid1", SyncML.TAG_ADD, 200);
        syncStatus.addReceivedItem("guid2", "luid2", SyncML.TAG_ADD, 200);
        assertTrue(syncStatus.getReceivedItemsCount() == 2);
        Hashtable mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 2);
        // Now simulate one mapping was sent
        syncStatus.addMappingSent("luid1");
        mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 1);
        // Save the status
        syncStatus.save();
        // Nothing should have changed...
        mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 1);
        assertTrue(syncStatus.getReceivedItemsCount() == 2);
    }

    public void testDeleteAddSameKey() throws Throwable {

        // Simulate a case where keys are reused
        syncStatus.addReceivedItem("guid1", "luid1", SyncML.TAG_DELETE, 200);
        syncStatus.addReceivedItem("guid2", "luid1", SyncML.TAG_ADD, 200);
        assertTrue(syncStatus.getReceivedItemsCount() == 2);
        Hashtable mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 1);
        assertTrue(mappings.get("luid1") != null);
        // Now simulate one mapping was sent
        syncStatus.addMappingSent("luid1");
        mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 0);
    }

    public void testSaveLoad1() throws Throwable {
        // Initialize a status and save it
        TestStringKeyValueStoreFactory storeBuilder = new TestStringKeyValueStoreFactory();
        TestTableFactory tableBuilder = new TestTableFactory();
        SyncStatus.setStoreFactory(storeBuilder);
        SyncStatus.setTableFactory(tableBuilder);

        syncStatus = new SyncStatus("test");
        syncStatus.addReceivedItem("guid1", "luid1", SyncML.TAG_ADD, 200);
        syncStatus.addReceivedItem("guid2", "luid2", SyncML.TAG_ADD, 200);
        syncStatus.addSentItem("luid3", SyncML.TAG_ADD);
        syncStatus.addSentItem("luid4", SyncML.TAG_ADD);
        syncStatus.addMappingSent("luid1");
        syncStatus.receivedItemStatus("luid3", 200);
        syncStatus.save();

        assertTrue(syncStatus.getReceivedItemsCount() == 2);
        assertTrue(syncStatus.getSentItemsCount() == 2);
        assertTrue(syncStatus.getSentItemStatus("luid3") == 200);
        Hashtable mappings = syncStatus.getPendingMappings();
        assertTrue(mappings.size() == 1);

        // Now reload everything (using a shared store)
        SyncStatus syncStatus2 = new SyncStatus("test");
        syncStatus2.load();
        // Reassert on the same properties
        assertTrue("#received = " + syncStatus2.getReceivedItemsCount(), syncStatus2.getReceivedItemsCount() == 2);
        assertTrue("#sent = " + syncStatus2.getSentItemsCount(), syncStatus2.getSentItemsCount() == 2);
        assertTrue("luid3 " + syncStatus2.getSentItemStatus("luid3"), syncStatus2.getSentItemStatus("luid3") == 200);
        mappings = syncStatus2.getPendingMappings();
        assertTrue("#mappings = " + mappings.size(), mappings.size() == 1);

        // Reset the store builder
        SyncStatus.setStoreFactory(null);
    }

    private class TestStringKeyValueStoreFactory extends StringKeyValueStoreFactory {
        
        private StringKeyValueStore store = new StringKeyValueMemoryStore();

        public TestStringKeyValueStoreFactory() {
        }

        public StringKeyValueStore getStringKeyValueStore(String name) {
            return store;
        }
    }
    
    private class TestTableFactory extends TableFactory {

        private Table table;

        public TestTableFactory() {
        }

        public Table getStringTable(String name, String[] colsName, int[] colsType, int index) {
            if (table == null) {
                table = new FileTable(".", name, colsName, colsType, index);
            }
            return table;
        }
    }
}
