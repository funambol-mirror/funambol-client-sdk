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

import com.funambol.syncml.protocol.SyncMLCommand;
import com.funambol.util.FunBasicTest;

import j2meunit.framework.*;


/**
 * Test the methods related to the SyncML Commands
 */
public class SyncMLCommandTest extends FunBasicTest {

    public SyncMLCommandTest() {
        super(2, "SyncMLCommandTest");
    }
    
    /**
     * Set up all of the tests
     */
    public void setUp() {
    }

    /**
     * Tear Down all of the tests
     */
    public void tearDown() {
    }
    
    /**
     * Launches the Test Case
     */
    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0: 
                testToString();
                break;
            case 1:
                testToStringWithType();
            default:                    
                break;
        }
    }

    /**
     * Launches the toString method
     */
    public void testToString() throws Exception {
        boolean result = true;
        String msg = "";
        String expectetString = "<Command>Add</Command>\n<CmdId>1</CmdId>\n<Type>null</Type>\n\n";
        SyncMLCommand smc = new SyncMLCommand("Add", "1");        
        
        if(!expectetString.equals(smc.toString())){
            result = false;
            msg = "SyncMLCommandTest - testToStringTypeNull - \n" +
                    "found : "+smc.toString()+"\n" +
                    "istead of : "+ expectetString;
        }
        
        assertTrue(msg,result);
    }

    /**
     * Launches the toString method versus the constructor invoked with type
     */
    public void testToStringWithType() throws Exception{
       boolean result = true;
        String msg = "";
        String expectetString = "<Command>Add</Command>\n<CmdId>1</CmdId>\n<Type>Type</Type>\n\n";
        SyncMLCommand smc = new SyncMLCommand("Add", "1", "Type");        
        
        if(!expectetString.equals(smc.toString())){
            result = false;
            msg = "SyncMLCommandTest - testToStringTypeNull - \n" +
                    "found : "+smc.toString()+"\n" +
                    "istead of : "+ expectetString;
        }
        
        assertTrue(msg,result);
    }

   

}

