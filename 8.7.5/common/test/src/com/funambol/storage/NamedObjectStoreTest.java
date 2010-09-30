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
package com.funambol.storage;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import junit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;
import javax.microedition.rms.RecordStoreNotFoundException;

/**
 * Test case for the NamedObjectStore class
 *
 *
 */// TODO: enable some test that have been written but
// are not currently called
public class NamedObjectStoreTest extends TestCase {
    private static final String STORENAME = "Store";
    private static final String STORENAME_1 = "Store1";
    private static final String TEST_RECORD = "Test";
    

    private NamedObjectStore os = null;
    private TestContainer container;

    public NamedObjectStoreTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.TRACE);
        container = null;
    }

    /**
     * Set up the tests
     */
    public void setUp() {
        container = new TestContainer(new TestClass("TestClass element"));
        os = new NamedObjectStore();
        try {
            os.create(STORENAME);
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Clean up resources
     */
    public void tearDown() {
        container = null;
        try {
            os.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        os = null;
        try {

            RecordStore.deleteRecordStore(STORENAME);

        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Create a record store.
     */
    public void testCreate() throws Exception {
        Log.info("=== Test create ===========================================");
        NamedObjectStore nos = new NamedObjectStore();
        nos.create(STORENAME_1);
        boolean isStore1Created = isRecordStoreAvailable(STORENAME_1);

        try {
            nos.close();
            RecordStore.deleteRecordStore(STORENAME_1);
        } catch (RecordStoreNotFoundException ex) {
            Log.error(STORENAME_1 + " Not Found: " + ex);
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        } 
        
        boolean isStore1Available = isRecordStoreAvailable(STORENAME_1);
        
        if (isStore1Available) {
            Log.info(STORENAME_1 + " has NOT been deleted");
        } else {
            Log.info(STORENAME_1 + " has been deleted");
        }
        
        assertTrue(isStore1Created&&!isStore1Available);
        Log.info("=====================================================[ OK ]");
    }

    /**
     * Test open method
     */
    public void testOpen() throws Exception {
        Log.info("=== Test Open =============================================");
        os.close();
        assertTrue(os.open(STORENAME));
        os.close();
        Log.info("=====================================================[ OK ]");
    }

    /**
     * Test recursive calls to the open method
     */
    public void testReOpen() throws Exception {
        Log.info("=== Test reopen ===========================================");
        os.open(STORENAME);
        assertTrue(!os.open(STORENAME));
        Log.info("======================================================[ OK ]");
    }

    /**
     * Open a non existent store
     */
    public void testOpenNonExistentStore() throws Exception {
        Log.info("=== Test open not existent ================================");

        boolean ret = false;

        try {
            os.open("NonExiststentStore");
        } catch (RecordStoreException e) {
            ret = true;
        } finally {
            assertTrue(ret);
        }
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test the storing of a Serializable TestClass into the default store
     */
    public void testStore() throws Exception {
        Log.info("=== Test store =============================================");
        os.open(STORENAME);
        TestClass tc = new TestClass("TestClass");
        assertTrue(os.store("Test", tc));
        os.close();
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #5: Test store
     */
    public void testRemove() throws Exception {
        Log.info("=== Test store =============================================");
        os.open(STORENAME);
        TestClass tc = new TestClass("TestClass");
        os.store("Test", tc);
        assertTrue(os.remove("Test"));
        os.close();
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #6: Test retrieve
     */
    public void testRetrieveExistent() throws Exception {
        Log.info("=== Test retrieve Existent=================================");
        os.open(STORENAME);
        TestClass tc = new TestClass("TestClass");
        os.store("Test", tc);
        TestClass tcCompare = (TestClass) os.retrieve("Test", tc);
        assertEquals(tc.toString(), tcCompare.toString());
        os.close();
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #7: Try to get a not existend object
     */
    public void testRetrieveNotExistent() throws Exception {
        Log.info("=== Test retrieve not Existent=============================");
        os.open(STORENAME);
        TestClass tc = new TestClass();
        boolean ret = false;
        try {
            os.retrieve("NonExistentTest", tc);
        } catch (RecordStoreException e) {
            ret = true;
        } finally {
            assertTrue(ret);
            os.close();
        }
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #8: Test to store a Vector
     */
    public void testStoreVector() throws Exception {
        Log.info("=== Test store vector =====================================");

        os.open(STORENAME);
        assertTrue(os.store("TestContainer", container));
        os.close();
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #9: Test to retieve a Vector
     */
    public void testRetrieveVector() throws Exception {
        Log.info("=== Test retrieve Vector ==================================");
        os.open(STORENAME);
        TestContainer c = new TestContainer();
        os.store("TestContainer", container);
        os.retrieve("TestContainer", c);
        assertTrue(container.equals(c));
        Log.info("======================================================[ OK ]");
    }

    /**
     * Test #10: Test to delete a Vector
     */
    public void testDeleteVector() throws Exception {
        Log.info("=== Test delete Vector ====================================");
        os.open(STORENAME);
        TestContainer c = new TestContainer();
        os.store("TestContainer", container);
        assertTrue(os.remove("TestContainer"));
        os.close();
        Log.info("======================================================[ OK ]");
    }
    
    private boolean isRecordStoreAvailable(String storeName) throws Exception {
        String[] stores = RecordStore.listRecordStores();
        for (int i=0; i<stores.length; i++) {
            if (stores[i].equals(storeName)) {
                //Return true if the given name is related to an existent 
                //storage
                Log.info(storeName + " Found into the StoreList");
                return true;
            }
        }
        //return false if the given storename is not found
        Log.info(storeName + " not found into the StoreList");
        return false;
    }
}

