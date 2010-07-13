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

import java.io.IOException;
import java.util.Random;
import javax.microedition.rms.RecordStoreException;
import junit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

/**
 * Test case for the NamedObjectStore class
 */
public class NamedObjectStorePerfTest extends TestCase {
    
    private NamedObjectStore os = null;
    private static final String STORENAME = "PerfTest";
    
    //number of stored objects
    private static final int OBJNUMBER = 20;
    
    // the item to retrieve. must be <= OBJNUMBER
    private static final int TOGET = 10;
    private final TestClass[] storedObjects;
    
    
    public NamedObjectStorePerfTest(String name) {
        super("NamedObjectStorePerfTest");
        storedObjects = new TestClass[OBJNUMBER];
        
        
        //creating test objects
        for (int i =0; i< OBJNUMBER; i++) {
            storedObjects[i] = new TestClass("Object"+i);
        }

        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
        
    }
    
    /**
     * Set up the tests
     */
    public void setUp() {
        
        //storing stuff
        os = new NamedObjectStore();
        try {
            os.create(STORENAME);
            Log.info("storing objects...");
            for (int i=0; i<storedObjects.length; i++) {
                os.store("Object"+i, storedObjects[i]);
            }
            os.close();
        } catch(Exception e){
            e.printStackTrace();
            return;
        }
        Log.info("setup finished");
    }
    
    /**
     * Clean up resources
     */
    public void tearDown() {
       /* try {
            Log.info("removing store " + STORENAME);
            os.remove(STORENAME);
        } catch (IOException ex) {
            ex.printStackTrace();
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        }*/
    }
    
    /**
     * Test #1: Test to retrieve the a large number of objects.
     */
    public void testRetrieveOne() throws Exception {
        Log.info("=== Retrieve One Element ==================================");
        os.open(STORENAME);
        TestClass tc = new TestClass();
        
        Log.info("trying to get element n°" + TOGET);
        os.retrieve("Object"+TOGET, tc);
        assertEquals(tc.toString(), storedObjects[TOGET].toString());
        os.close();
        Log.info("=====================================================[ OK ]");
    }
    
    /**
     * Test #1: Test to retrieve the a large number of objects.
     */
    public void testRetrieveAll() throws Exception {
        Log.info("=== Retrieve All Elements =================================");
        os.open(STORENAME);
        TestClass tc = new TestClass();
        for (int i=0; i<OBJNUMBER; i++) {
            os.retrieve("Object"+i, tc);
            if (!tc.toString().equals(storedObjects[i].toString()))  {
                fail();
            };
            
        }
      //  os.close();
        Log.info("=====================================================[ OK ]");
    }
    
}

