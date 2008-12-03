/*
 * Copyright (C) 2006-2007 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

package com.funambol.syncml.client;

import com.funambol.syncml.spds.MappingManager;
import com.funambol.util.ConsoleAppender;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import jmunit.framework.cldc10.AssertionFailedException;
import jmunit.framework.cldc10.TestCase;

import com.funambol.util.Log;
import java.util.Hashtable;
import javax.microedition.rms.RecordStore;

/**
 * Test the MappingManager class, that persists the mapping message into the 
 * store
 */
public class MappingManagerTest extends TestCase {
    private static final String MAPPING_STORE = "SyncMLMappingStore";
    
    private static final String SOURCE_1 = "Dummy_1";
    private static final String SOURCE_2 = "Dummy_2";
    private static final String SOURCE_3 = "Dummy_3";
    
    Hashtable mappingsSample = new Hashtable();
    
    RecordStore rs = null;
    
    MappingManager mm = null;
    
    public MappingManagerTest() {
        super(8, "Mapping storage Test");
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {
        mm = new MappingManager(); 
        mappingsSample.put("first", "1");
        mappingsSample.put("second", "2");
        mappingsSample.put("third", "3");
        Log.info("#########################");
    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {
        mm = null;
        mappingsSample.clear();
        deleteMappingStore();
        rs = null;
    }

    /**
     * Lauches the test case
     * @param testNumber
     * @throws java.lang.Throwable
     */
    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0: 
                testSaveEmptyMappingsNoStorage();
                break;
            case 1: 
                testGetEmptyMappingsNoStorage();
                break;
            case 2: 
                testSaveEmptyMappingsExistentStorage();
                break;
            case 3:                 testGetEmptyMappingsExistentStorage();
                break;
            case 4: 
                testSave3SourcesMappings();
                break;
            case 5: 
                testGet3SourcesMappings();
                break;
            case 6: 
                testModify3SourcesMappings();
                break;
            case 7: 
                testResetMappings();
                break;
            default:                    
                break;
        }
    }

    /**
     * Try to store an empty mapping without the store existent on the device
     * @throws jmunit.framework.cldc10.AssertionFailedException
     */
    public void testSaveEmptyMappingsNoStorage() throws AssertionFailedException {
        Log.info("testSaveEmptyMappingsNoStorage");
        mm.saveMappings(SOURCE_1, new Hashtable());
        Hashtable ht = mm.getMappings(SOURCE_1);
        assertTrue(ht.isEmpty());
        Log.info("succesfull");
    }
    
    /**
     * Try to get an empty mapping without the store existent on the device
     * @throws jmunit.framework.cldc10.AssertionFailedException
     */
    public void testGetEmptyMappingsNoStorage() throws AssertionFailedException {
        Log.info("testGetEmptyMappingsNoStorage");
        Hashtable ht = mm.getMappings("NotExistent");
        assertTrue(ht.isEmpty());
        Log.info("succesfull");
    }
    
    /**
     * Try to store an empty mapping with store existent on the device
     * @throws jmunit.framework.cldc10.AssertionFailedException
     */
    public void testSaveEmptyMappingsExistentStorage() throws AssertionFailedException {
        Log.info("testSaveEmptyMappingsExistentStorage");
        setExistentStorage();
        mm.saveMappings(SOURCE_1, new Hashtable());
        Hashtable ht = mm.getMappings(SOURCE_1);
        assertTrue(ht.isEmpty());
        Log.info("succesfull");
    }
    
    /**
     * Try to get an empty mapping with an already existent store on the device
     * @throws jmunit.framework.cldc10.AssertionFailedException
     */
    public void testGetEmptyMappingsExistentStorage() throws AssertionFailedException {
        Log.info("testGetEmptyMappingsExistentStorage");
        setExistentStorage();
        Hashtable ht = mm.getMappings("NotExistent");
        assertTrue(ht.isEmpty());
        Log.info("succesfull");
    }

    /**
     * Save 3 different mappings for 3 different sources
     * @throws java.lang.Throwable
     */
    public void testSave3SourcesMappings() throws Throwable {
        Log.info("testSave3SourcesMappings");
        mm.saveMappings(SOURCE_1, mappingsSample);
        mm.saveMappings(SOURCE_2, mappingsSample);
        mm.saveMappings(SOURCE_3, mappingsSample);
        rs = RecordStore.openRecordStore(MAPPING_STORE, false);
        int recordNum = rs.getNumRecords();
        Log.debug(recordNum + " record/s found");
        rs.closeRecordStore();
        assertTrue(recordNum==3);
        Log.info("succesfull");
    }
    
    /**
     * Save 3 different mappings for 3 different and then retrieves them
     * @throws java.lang.Throwable
     */
    public void testGet3SourcesMappings() throws Throwable {
        Log.info("testGet3SourcesMappings");
        String key1 = "key1";
        String value1 = "value1";
        String key2 = "key2";
        String value2 = "value2";
        String key3 = "key3";
        String value3 = "value3";
        mappingsSample.put(key1, value1);
        mm.saveMappings(SOURCE_1, mappingsSample);
        mappingsSample.put(key2, value2);
        mm.saveMappings(SOURCE_2, mappingsSample);
        mappingsSample.put(key3, value3);
        mm.saveMappings(SOURCE_3, mappingsSample);
        
        Hashtable m1 = mm.getMappings(SOURCE_1);
        Hashtable m2 = mm.getMappings(SOURCE_2);
        Hashtable m3 = mm.getMappings(SOURCE_3);
        
        boolean src1 = m1.containsKey(key1)&&m1.contains(value1);
        boolean src2 = m2.containsKey(key2)&&m2.contains(value2);
        boolean src3 = m3.containsKey(key3)&&m3.contains(value3);
    
        assertTrue(src1&&src2&&src3);
        Log.info("succesfull");
    }
    
    /**
     * Save 3 different mappings for 3 different sources
     * and then modify and retrieve them correctly from the store
     * @throws java.lang.Throwable
     */
    public void testModify3SourcesMappings() throws Throwable {
        Log.info("testGet3SourcesMappings");
        String key1 = "key1";
        String value1 = "value1";
        String key2 = "key2";
        String value2 = "value2";
        String key3 = "key3";
        String value3 = "value3";
        mappingsSample.put(key1, value1);
        mm.saveMappings(SOURCE_1, mappingsSample);
        mappingsSample.put(key2, value2);
        mm.saveMappings(SOURCE_2, mappingsSample);
        mappingsSample.put(key3, value3);
        mm.saveMappings(SOURCE_3, mappingsSample);
        
        Hashtable m1 = mm.getMappings(SOURCE_1);
        Hashtable m2 = mm.getMappings(SOURCE_2);
        Hashtable m3 = mm.getMappings(SOURCE_3);
        
        boolean src1 = m1.containsKey(key1)&&m1.contains(value1);
        boolean src2 = m2.containsKey(key2)&&m2.contains(value2);
        boolean src3 = m3.containsKey(key3)&&m3.contains(value3);
    
        //Changes the mapping values for the 3 sources
        mappingsSample.clear();
        mappingsSample.put(key2, value2);
        mm.saveMappings(SOURCE_1, mappingsSample);

        mappingsSample.clear();
        mappingsSample.put(key3, value3);
        mm.saveMappings(SOURCE_2, mappingsSample);

        mappingsSample.clear();
        mappingsSample.put(key1, value1);
        mm.saveMappings(SOURCE_3, mappingsSample);
        
        m1 = mm.getMappings(SOURCE_1);
        m2 = mm.getMappings(SOURCE_2);
        m3 = mm.getMappings(SOURCE_3);

        boolean m1Size = m1.size()==1; 
        boolean m2Size = m2.size()==1; 
        boolean m3Size = m3.size()==1; 
        
        boolean isRightMapsSize = m1Size&&m2Size&&m3Size;
        boolean chSrc1 = m1.containsKey(key2)&&m1.contains(value2);
        boolean chSrc2 = m2.containsKey(key3)&&m2.contains(value3);
        boolean chSrc3 = m3.containsKey(key1)&&m2.contains(value1);
        
        assertTrue(src1&&src2&&src3);
        Log.info("succesfull");
    }

    /**
     * Try to store an empty mapping with store existent on the device
     * @throws jmunit.framework.cldc10.AssertionFailedException
     */
    public void testResetMappings() throws AssertionFailedException {
        Log.info("testSaveEmptyMappingsExistentStorage");
        setExistentStorage();
        mm.saveMappings(SOURCE_1, mappingsSample);
        mm.resetMappings(SOURCE_1);
        Hashtable ht = mm.getMappings(SOURCE_1);
        assertTrue(ht.isEmpty());
        Log.info("succesfull");
    }
    

    private void deleteMappingStore() {
        try {
            RecordStore.deleteRecordStore(MAPPING_STORE);
        } catch (RecordStoreNotFoundException ex) {
            Log.error("Cannot find storage" + ex);
        } catch (RecordStoreException ex) {
            Log.error("Cannot delete storage" + ex);
        }
    }
    
    private void setExistentStorage() {
        try {
            rs = RecordStore.openRecordStore(MAPPING_STORE, true);
            rs.closeRecordStore();
        } catch (Exception e) {
            Log.error("RecordStore Exception: " + e);
        }
    }
}

