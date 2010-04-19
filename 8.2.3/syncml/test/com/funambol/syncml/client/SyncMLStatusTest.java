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


import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.syncml.spds.SyncMLFormatter;
import com.funambol.util.ChunkedString;
import com.funambol.util.Log;

import junit.framework.*;


/**
 * Test the method to parse the SyncML status tag
 */
public class SyncMLStatusTest extends TestCase {

    
    public SyncMLStatusTest(String name) {
        super(name);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {
    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {
    }

    /**
     * Test the toString Method
     * @throws java.lang.Exception
     */
    public void testToString() throws Exception {
        boolean result = false;
        String msg = "";
        String expectedString = "<Status><CmdID>1</CmdID>\n" +
                "<MsgRef>3</MsgRef>\n" +
                "<CmdRef>1</CmdRef>\n" +
                "<Cmd>Sync</Cmd>\n" +
                "<SourceRef>card</SourceRef>\n" +
                "<TargetRef>contact</TargetRef>\n" +
                "<Data>200</Data>\n</Status>\n";
        SyncMLStatus status = new SyncMLStatus();
        status.setCmdId("1");
        status.setMsgRef("3");
        status.setCmdRef("1");
        status.setCmd(SyncML.TAG_SYNC);
        status.setTgtRef("contact");
        status.setSrcRef("card");

        SyncMLFormatter formatter = new SyncMLFormatter();
        String statusText = formatter.formatItemStatus(status);
        
        if(expectedString.equals(statusText)) {
            result = true;
            
        }else{
            msg = "SyncMLStatusTest - testToString - \n" +
                    "found : "+status.toString()+"\n" +
                    "istead of : "+ expectedString;
        }
      
        assertTrue(msg,result);
    }
    
    /**
     * Test the parse Method
     * @throws java.lang.Exception
     */
    public void testParse() throws Exception{
        boolean result = false;
        String msg = "";
        String expectedStatus = "<Status><CmdID>1</CmdID>\n"+
                                "<MsgRef>3</MsgRef>\n"+
                                "<CmdRef>0</CmdRef>\n"+
                                "<Cmd>SyncHdr</Cmd>\n"+
                                "<SourceRef>fjm-alemulator</SourceRef>\n"+
                                "<TargetRef>http://pavia.funambol.com:80/funambol/ds;jsessionid=10A93948FC384D4C5066531502D31EF3</TargetRef>\n"+
                                "<Data>200</Data>\n</Status>\n";
        ChunkedString tag = new ChunkedString("<CmdID>1</CmdID>\n" +
                                                "<MsgRef>3</MsgRef>\n" +
                                                "<CmdRef>0</CmdRef>\n" +
                                                "<Cmd>SyncHdr</Cmd>\n" +
                                                "<TargetRef>http://pavia.funambol.com:80/funambol/ds;jsessionid=10A93948FC384D4C5066531502D31EF3</TargetRef>\n" +
                                                "<SourceRef>fjm-alemulator</SourceRef>\n" +
                                                "<Data>200</Data>\n");
        SyncMLStatus status = SyncMLStatus.parse(tag);
        SyncMLFormatter formatter = new SyncMLFormatter();
        String statusText = formatter.formatItemStatus(status);

        if(expectedStatus.equals(statusText)){
            result = true;
        }else{
            msg = "SyncMLStatusTest - testToString - \n" +
                    "found : "+status+"\n" +
                    "istead of : "+ expectedStatus;
        }
        assertTrue(msg,result);
    }

    public void testParseChallenge() throws Exception{

        final String CHAL_TYPE = SyncML.AUTH_TYPE_MD5;
        final String CHAL_FORMAT = "b64";
        final String CHAL_NEXT_NONCE = "Tm9uY2U=";
        
        ChunkedString statusTag = new ChunkedString("<Status>\n" +
            "<CmdID>1</CmdID>\n" +
            "<MsgRef>1</MsgRef><CmdRef>0</CmdRef><Cmd>SyncHdr</Cmd>\n" +
            "<TargetRef>http://www.syncml.org/sync-server</TargetRef>\n" +
            "<SourceRef>IMEI:493005100592800</SourceRef>\n" +
            "<Chal>\n" +
            "<Meta>\n" +
            "<Type xmlns=’syncml:metinf’>" + CHAL_TYPE + "</Type>\n" +
            "<Format xmlns=’syncml:metinf’>" + CHAL_FORMAT + "</Format>\n" +
            "<NextNonce xmlns=’syncml:metinf’>" + CHAL_NEXT_NONCE + "</NextNonce>\n" +
            "</Meta>\n" +
            "</Chal>\n" +
            "<Data>407</Data> \n" +
            "</Status>");

        SyncMLStatus status = SyncMLStatus.parse(statusTag);

        assertEquals(status.getChalType(), CHAL_TYPE);
        assertEquals(status.getChalFormat(), CHAL_FORMAT);
        assertEquals(status.getChalNextNonce(), CHAL_NEXT_NONCE);
    }

}

