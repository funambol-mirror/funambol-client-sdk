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

import com.funambol.storage.ComplexSerializer;
import com.funambol.syncml.spds.ItemMap;
import com.funambol.util.ConsoleAppender;
import java.io.IOException;
import j2meunit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.FunBasicTest;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.Hashtable;


/**
 * test the ItemMap class, the serializable object that contains mappings
 * @author ivomania
 */
public class ItemMapTest extends FunBasicTest {

    private static final String SOURCE_NAME = "key1";
    
    private static final String KEY_1 = "key1";
    private static final String KEY_2 = "key_2";
    private static final String KEY_3 = "key_3";
    
    private static final String VALUE_1 = "Value1";
    private static final String VALUE_2 = "Value_2";
    private static final String VALUE_3 = "Value_3";
    
    Hashtable mappings = new Hashtable();
        
    public ItemMapTest() {
        super(7, "ItemMap Test");
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }

    /**
     * Set up the test
     */
    public void setUp() {
        Log.info("#########################");
        mappings.put(KEY_1, VALUE_1);
        mappings.put(KEY_2, VALUE_2);
        mappings.put(KEY_3, VALUE_3);
    }

    /**
     * Tear Down the test
     */
    public void tearDown() {
        mappings.clear();
    }

    /**
     * Launches all of the tests
     * @param testNumber
     * @throws java.lang.Throwable
     */
    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0: 
                testEmptyConstructor();
                break;
            case 1: 
                testMappingsConstructor();
                break;
            case 2: 
                testSourceNameConstructor();
                break;
            case 3: 
                testSerialize();
                break;
            case 4: 
                testDeserialize();
                break;
            case 5: 
                testGetMappings();
                break;
            case 6: 
                testGetSourceName();
                break;
            default:                    
                break;
        }
    }

    /**
     * Test the empty constructor
     * @throws Exception
     */
    public void testEmptyConstructor() throws Exception {
        Log.info("testEmptyConstructor");
        ItemMap im = new ItemMap();
        assertTrue(im.getSourceName().equals("")&&im.getMappings().isEmpty());
        Log.debug("succesfull");
    }
    
    /**
     * Test the constructor with mappings and sourcename
     * @throws Exception
     */
    public void testMappingsConstructor() throws Exception{
        Log.info("testMapppingsConstructor");

        ItemMap im = new ItemMap(SOURCE_NAME, mappings);
        String sourceName = im.getSourceName();
        boolean areHashtableEquals = areHashtablesEquals(im.getMappings(), mappings);
        
        assertTrue(sourceName.equals(SOURCE_NAME)&&areHashtableEquals);
        Log.debug("succesfull");
    }
    
    /**
     * Test the constructor with sourcename
     * @throws Exception
     */
    public void testSourceNameConstructor() throws Exception{
        Log.info("testSourceNameConstructor");
        
        ItemMap im = new ItemMap(SOURCE_NAME);

        boolean isNameGiven = im.getSourceName().equals(SOURCE_NAME);
        assertTrue(isNameGiven&&im.getMappings().isEmpty());
        Log.debug("succesfull");
    }

    /**
     * Test the serialize method
     * @throws Exception
     * @throws java.io.IOException
     */
    public void testSerialize() throws Exception {
        Log.info("testSerialize");
        
        //create Expected
        ByteArrayOutputStream expected = new ByteArrayOutputStream();
        DataOutputStream exp = new DataOutputStream(expected);
        
        exp.writeUTF(SOURCE_NAME);
        ComplexSerializer.serializeHashTable(exp, mappings);
        
        //Create result
        ByteArrayOutputStream result = new ByteArrayOutputStream();
        DataOutputStream res = new DataOutputStream(result);
        
        ItemMap im = new ItemMap(SOURCE_NAME, mappings);
        im.serialize(res);

        assertTrue(areByteStreamsEquals(expected.toByteArray(), result.toByteArray()));
        
        Log.debug("succesfull");
    }

    /**
     * Test the deserialize method
     * @throws Exception
     * @throws java.io.IOException
     */
    public void testDeserialize() throws Exception {
        Log.info("testDeserialize");
        
        ItemMap im = new ItemMap(SOURCE_NAME, mappings);

        //Create result
        ByteArrayOutputStream expected = new ByteArrayOutputStream();
        DataOutputStream exp = new DataOutputStream(expected);
        
        im.serialize(exp);
        
        ByteArrayInputStream expInStream = new ByteArrayInputStream(expected.toByteArray());
        DataInputStream expectedInData = new DataInputStream(expInStream);
        
        im.deserialize(expectedInData);
        
        boolean isNameEquals = im.getSourceName().equals(SOURCE_NAME);
        boolean isMapEquals = areHashtablesEquals(mappings, im.getMappings());

        assertTrue(isNameEquals&&isMapEquals);
        Log.debug("succesfull");
    }
    
    /**
     * Test the accessor method GET for mappings field
     * @throws Exception
     */
    public void testGetMappings() throws Exception {
        Log.info("testGetMappings");

        ItemMap im = new ItemMap(SOURCE_NAME, mappings);
        
        assertTrue(areHashtablesEquals(mappings, im.getMappings()));
        Log.debug("succesfull");
    }
    
    /**
     * Test the accessor method GET for sourceName field
     * @throws Exception
     */
    public void testGetSourceName() throws Exception {
        Log.info("testGetSourceName");
        ItemMap im = new ItemMap(SOURCE_NAME, mappings);
        
        assertTrue(im.getSourceName().equals(SOURCE_NAME));
        
        Log.debug("succesfull");
    }

    private boolean areHashtablesEquals(Hashtable expected, Hashtable result) {
        String e1 = (String) expected.get(KEY_1);
        String e2 = (String) expected.get(KEY_2);
        String e3 = (String) expected.get(KEY_3);
        
        String r1 = (String) result.get(KEY_1);
        String r2 = (String) result.get(KEY_2);
        String r3 = (String) result.get(KEY_3);
        
        return (
            e1.equals(r1)&&
            e2.equals(r2)&&
            e3.equals(r3)&&
            expected.size()==3&&
            result.size()==3
          );
    }

    private boolean areByteStreamsEquals(byte[] expected, byte[] result) {
        boolean ret = false;
        for (int i=0; i<expected.length; i++) {
            ret = expected[i]==result[i]; 
        }     
        return ret;
    }
}

