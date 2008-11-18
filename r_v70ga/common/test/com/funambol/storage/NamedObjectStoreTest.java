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
import jmunit.framework.cldc10.AssertionFailedException;
import jmunit.framework.cldc10.TestCase;
import jmunit.framework.cldc10.TestSuite;

import com.funambol.util.Log;

/**
 * Test case for the NamedObjectStore class
 *
 *
 */

// TODO: enable some test that have been written but
// are not currently called

public class NamedObjectStoreTest extends TestCase {
    
    private NamedObjectStore os = null;
    private static final String STORENAME = "Store";
    private TestContainer container;
    
    public NamedObjectStoreTest() {
        super(8, "NamedObjectStoreTest");
        
        container = null;
    }
    
    /**
     * Set up the tests
     */
    public void setUp() {
        container = new TestContainer(new TestClass("TestClass element"));
        os = new NamedObjectStore();
    }
    
    /**
     * Clean up resources
     */
    public void tearDown() {
        container = null;
        try {
            os.close();
        } catch (Exception e){
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
     * Run the tests
     */
    public void test(int testNumber) throws Throwable {
        
        switch(testNumber) {
            case 0: testCreate();               break;
            case 1: testRemove();               break;
            case 2: testOpen();                 break;
            case 3: testReOpen();               break;
            case 4: testOpenNonExistentStore();  break;
             case 5: testStore();                break;
           
            case 6: testRetrieveExistent();     break;
            case 7: testRetrieveNotExistent();  break;
            case 8: testStoreVector();          break;
            case 9: testRetrieveVector();       break;
            case 10: testDeleteVector();        break;
            
            default:                            break;
        }
    }
    
    /**
     * Test #0: Create the two record store.
     */
    public void testCreate() throws Exception {
        Log.info("=== Test create ===========================================");
        // recordstore is created in setup method
        // we need to remove it to test the add
        os.remove(STORENAME);
        assertTrue( os.create(STORENAME) );
        Log.info("=====================================================[ OK ]");
    }
    
    /**
     * test #1 remove recordstore
     */
    public void testRemove() throws Exception {
        Log.info("=== Test remove===========================================");
        assertTrue(os.remove(STORENAME));
        Log.info("=====================================================[ OK ]");
    }
    
    
    /**
     * Test #2: Tests on open.
     */
    
    public void testOpen() throws Exception {
        Log.info("=== Test Open =============================================");
        
        os.create(STORENAME);
        os.close();
        assertTrue(os.open(STORENAME));
        os.close();
        Log.info("=====================================================[ OK ]");
    }
    
    
    /**
     * test #3 test a double open
     */
    
    
    public void testReOpen() throws Exception {
        Log.info("=== Test reopen ===========================================");
        
        os.create(STORENAME);
        os.open(STORENAME);
        assertFalse(os.open(STORENAME));
        Log.info("======================================================[ OK ]");
    }
    
    
    /**
     * test 4 opening a non existent store
     */
    public void testOpenNonExistentStore() throws Exception {
        Log.info("=== Test open not existent ================================");
        
        boolean ret = false;
        
        try {
            os.open("NonExiststentStore");
        } catch (RecordStoreException e){
            ret = true;
        } finally {
            assertTrue(ret);
        }
        Log.info("======================================================[ OK ]");
    }
    
    /**
     * Test #5: Test store
     */
    private void testStore() throws Exception {
        Log.info("=== Test store =============================================");
        os.create(STORENAME);
        os.open(STORENAME);
        TestClass tc = new TestClass("TestClass");
        assertTrue( os.store("Test", tc) );
        os.close();
        Log.info("======================================================[ OK ]");
    }
    
    
    
    
    /**
     * Test #6: Test retrieve
     */
    private void testRetrieveExistent() throws Exception {
        Log.info("=== Test retrieve Existent=================================");
        os.create(STORENAME);
        os.open(STORENAME);
        TestClass tc = new TestClass("TestClass");
        os.store("Test", tc) ;
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
        os.create(STORENAME);
        os.open(STORENAME);
        TestClass tc = new TestClass();
        boolean ret = false;
        try {
            os.retrieve("NonExistentTest", tc);
        } catch (RecordStoreException e) {
            ret =true;
        } finally{
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
        
        os.create(STORENAME);
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
        os.create(STORENAME);
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
        os.create(STORENAME);
        os.open(STORENAME);
        TestContainer c = new TestContainer();
        os.store("TestContainer", container);
        assertTrue(os.remove("TestContainer"));
        os.close();
        Log.info("======================================================[ OK ]");
    }
}

