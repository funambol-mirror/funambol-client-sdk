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

import com.funambol.util.Log;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import jmunit.framework.cldc10.TestCase;

import javax.microedition.rms.RecordStoreException;

public class ObjectStoreTest extends TestCase {
    
    private ObjectStore os = null;
    private String storename1 = "Test1";
    private String storename2 = "Test2";
    int recordIndex = -1;
    
    public class TestHtClass implements Serializable {
        private Hashtable ht;
        
        public TestHtClass() {
            ht = new Hashtable();
            ht.put(new String("Funambol"), new String("Developers"));
            ht.put(new String("Patrick"), new String("Ohly"));
            ht.put(new String("Andrea"), new String("Toccalini"));
        }
        
        public void serialize(DataOutputStream out) throws IOException {
            ComplexSerializer.serializeHashTable(out, ht);
        }
        
        public void deserialize(DataInputStream in) throws IOException {
            ComplexSerializer.deserializeHashTable(in);
        }
        
        public String toString(){
            StringBuffer ret = new StringBuffer(this.getClass().getName());
            for(Enumeration e = ht.keys(); e.hasMoreElements(); ){
                String k = (String) e.nextElement();
                ret.append(k);
                ret.append(": ");
                ret.append((String)ht.get(k));
                ret.append("\n");
            }
            return ret.toString();
        }
    }
    
    public ObjectStoreTest() {
        super(6, "ObjectStoreTest");
        Log.setLogLevel(Log.INFO);
    }
    
    
    
    public void test(int testNumber) throws Throwable {
        
        os = new ObjectStore();
        
        switch(testNumber) {
            case 0:
                testCreate();
                break;
            case 1:
                testOpen();
                break;
            case 2:
                testReOpen();
                break;
            case 3:
                testOpenNotExistent();
                break;
            case 4:
                testStore();
                break;
            case 5:
                testRetrieve();
                break;
            default:
                break;
        }
        
        os = null;
    }
    
    public void testCreate() throws Exception {
        Log.info("=== Test Create============================================");
        assertTrue(os.create(storename1));
        os.close();
        Log.info("=====================================================[ OK ]");
        
    }
    
    public void testOpen() throws Exception {
        Log.info("=== Test Open =============================================");
        os.create(storename1);
        os.close();
        assertTrue(os.open(storename1));
        Log.info("=====================================================[ OK ]");
        
        
    }
    
    public void testOpenNotExistent() throws Exception  {
        Log.info("=== Test Open Not Existent ================================");
        try {
            os.open("NonExiststentStore");
            //if we reach this point, we've failed!
            fail();
        }catch (RecordStoreException e){
            System.out.println(
                    new StringBuffer("Exception catched: ")
                    .append(e.getMessage()).toString() );
            assertTrue(true);
        }
        Log.info("=====================================================[ OK ]");
        
    }
    
    public void testReOpen() throws Exception {
        Log.info("=== Test Reopen ===========================================");
        os.create(storename1);
        // the store has been opened by the create method
        assertFalse(os.open(storename1));
        Log.info("=====================================================[ OK ]");
    }
    
    
    private void testStore() throws Exception {
        Log.info("=== Test Store ============================================");
        os.create(storename1);
        TestHtClass htc = new TestHtClass();
        os.store(htc);
        os.close();
        Log.info("=====================================================[ OK ]");
        
    }
    
    private void testRetrieve() throws Exception {
        Log.info("=== Test retrieve =========================================");
        os.create(storename1);
        os.open(storename1);
        TestHtClass htc = new TestHtClass();
        recordIndex = os.store(htc);
        TestHtClass retrieved = new TestHtClass();
        os.retrieve(recordIndex, retrieved);
        assertEquals(htc.toString(), retrieved.toString());
        os.close();
        Log.info("=====================================================[ OK ]");
        
    }
    
    public void setUp() throws Throwable {
        
    }
    
    public void tearDown() {
        
    }
}

