/**
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

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

import junit.framework.*;

/**
 * Test the SyncManager performance
 */
public class SyncManagerPerfTest extends TestCase {

    private static final String TAG_LOG = "SyncManagerPerfTest";

    private final String TEST_SERVER_URL  = "http://test.server.url";
    private final String TEST_USERNAME    = "test";
    private final String TEST_PASSWORD    = "test";

    private SyncManager  sm = null;
    private SyncConfig   sc = null;
    private DeviceConfig dc = null;
    private SourceConfig ssc = null;
    private TestSyncSource tss = null;

    private final String JSESSION_ID = ";jsessionid=F2EA56F802D65950FAC3E37336BE1EEA.NODE01";
    private int sentMessagesCount = 0;

    private static final int NUM_ITERATIONS = 80;
    
    public SyncManagerPerfTest(String name) {
        super(name);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {

        sc = new SyncConfig();
        sc.syncUrl = TEST_SERVER_URL;
        sc.userName = TEST_USERNAME;
        sc.password = TEST_PASSWORD;
        dc = new DeviceConfig();

        ssc = new SourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase");
        SyncMLAnchor anchor = new SyncMLAnchor();
        ssc.setSyncAnchor(anchor);
        
        sm = new SyncManager(sc, dc);

        tss = new TestSyncSource(ssc);

        sentMessagesCount = 0;

        Log.initLog(new ConsoleAppender(), Log.ERROR);
    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {

    }

    //----------------------------------------------- Authentication phase tests

    /**
     * Test successfull basic authentication
     */
    public void testSyncPerformance() throws Exception {

        // Check if the user is running performance test.
        String profiling = System.getProperty("profiling");
        if (profiling == null) {
            return;
        }

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {
            
            public byte[] handleMessage(byte message[]) throws Exception {

                byte response[] = null;
                sentMessagesCount++;

                if (sentMessagesCount == 1) {
                    // Handle first message (Force a slow sync)
                    response = getInitResponse().getBytes("UTF-8");
                } else if (sentMessagesCount <= NUM_ITERATIONS) {
                    // The client is sending its items
                    int firstLuid = tss.getFirstLuid();
                    int lastLuid  = tss.getLastLuid();
                    response = getRecevingResponse(firstLuid, lastLuid);
                    if (sentMessagesCount == NUM_ITERATIONS) {
                        // Block the source from providing items
                        tss.endSending();
                    }
                } else if (sentMessagesCount < (2 * NUM_ITERATIONS)) {
                    if (sentMessagesCount == (NUM_ITERATIONS + 1)) {
                        int firstLuid = tss.getFirstLuid();
                        int lastLuid  = tss.getLastLuid();
                        response = getRecevingResponse(firstLuid, lastLuid);
                    } else {
                        // Start sending items
                        response = getSendingResponse(false);
                    }
                    // The client is receiving items
                } else if (sentMessagesCount == (2 * NUM_ITERATIONS)) {
                    // Stop sending items
                    response = getSendingResponse(true);
                }

                // Note that we don't send the status to the map command, but
                // currently the SyncManager does not really care ;)

                tss.resetLuidCounters();
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        sm.sync(tss);
    }


    private String getInitResponse() {
        return getServerResponseFromStatus(getStatus_AUTH_BASIC_OK(), true);
    }

    private String getStatus_AUTH_BASIC_OK() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
               "<Data>212</Data>\n" +
               "</Status>";
    }

    private String getServerResponseFromStatus(String status, boolean addAlertFromServer) {
        return "<SyncML>\n" +
               "<SyncHdr>\n" +
               "<VerDTD>1.2</VerDTD>\n" +
               "<VerProto>SyncML/1.2</VerProto>\n" +
               "<SessionID>1266917419910</SessionID>\n" +
               "<MsgID>1</MsgID>\n" +
               "<Target>\n" +
               "<LocURI>" + dc.getDevID() + "</LocURI>\n" +
               "</Target>\n" +
               "<Source>\n" +
               "<LocURI>" + sc.syncUrl + "</LocURI>\n" +
               "</Source>\n" +
               "<RespURI>" + sc.syncUrl + JSESSION_ID + "</RespURI>\n" +
               "</SyncHdr>\n" +
               "<SyncBody>\n" +
               status +
               "<Status>\n" +
               "<CmdID>2</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>1</CmdRef>\n" +
               "<Cmd>Alert</Cmd>\n" +
               "<TargetRef>" + ssc.getName() + "</TargetRef>\n" +
               "<SourceRef>" + ssc.getRemoteUri() + "</SourceRef>\n" +
               "<Data>508</Data>\n" +
               "</Status>\n" +
               "<Status>\n" +
               "<CmdID>3</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>2</CmdRef>\n" +
               "<Cmd>Put</Cmd>\n" +
               "<SourceRef>./devinf12</SourceRef>\n" +
               "<Data>200</Data>\n" +
               "</Status>\n" +
               (addAlertFromServer ? getAlertFromServer() : "") +
               "<Final/>\n" +
               "</SyncBody>\n" +
               "</SyncML>";
    }

    private String getAlertFromServer() {
        return "<Alert>\n" +
               "<CmdID>4</CmdID>\n" +
               "<Data>201</Data>\n" +
               "<Item>\n" +
               "<Target>\n" +
               "<LocURI>" + ssc.getName() + "</LocURI>\n" +
               "</Target>\n" +
               "<Source>\n" +
               "<LocURI>" + ssc.getRemoteUri() + "</LocURI>\n" +
               "</Source>\n" +
               "<Meta>\n" +
               "<Anchor xmlns='syncml:metinf'>\n" +
               "<Last>0</Last>\n" +
               "<Next>0</Next>\n" +
               "</Anchor>\n" +
               "</Meta>\n" +
               "</Item>\n" + 
               "</Alert>\n";
    }

    private byte[] getRecevingResponse(int firstLuid, int lastLuid) {

        StringBuffer res = new StringBuffer();

        res.append("<SyncML>\n")
           .append("<SyncHdr>\n")
           .append("<VerDTD>1.2</VerDTD>\n")
           .append("<VerProto>SyncML/1.2</VerProto>\n")
           .append("<SessionID>1266917419910</SessionID>\n")
           .append("<MsgID>1</MsgID>\n")
           .append("<Target>\n")
           .append("<LocURI>").append(dc.getDevID()).append("</LocURI>\n")
           .append("</Target>\n")
           .append("<Source>\n")
           .append("<LocURI>" + sc.syncUrl + "</LocURI>\n")
           .append("</Source>\n")
           .append("<RespURI>").append(sc.syncUrl).append(JSESSION_ID).append("</RespURI>\n")
           .append("</SyncHdr>\n")
           .append("<SyncBody>\n");

        int cmdId = 1;

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>1</MsgRef>\n")
           .append("<CmdRef>0</CmdRef>\n")
           .append("<Cmd>SyncHdr</Cmd>\n")
           .append("<TargetRef>").append(sc.syncUrl).append("</TargetRef>\n")
           .append("<SourceRef>").append(dc.getDevID()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");


        for(int i=firstLuid;i<lastLuid+1;++i) {
            res.append("<Status>\n")
               .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
               .append("<MsgRef>1</MsgRef>\n")
               .append("<CmdRef>0</CmdRef>\n")
               .append("<Cmd>Replace</Cmd>\n")
               .append("<SourceRef>").append(i).append("</SourceRef>\n")
               .append("<Data>200</Data>\n")
               .append("</Status>");
        }

        res.append("</SyncBody>\n")
           .append("</SyncML>");

        return res.toString().getBytes();
    }

    private byte[] getSendingResponse(boolean last) {

        StringBuffer res = new StringBuffer();

        res.append("<SyncML>\n")
           .append("<SyncHdr>\n")
           .append("<VerDTD>1.2</VerDTD>\n")
           .append("<VerProto>SyncML/1.2</VerProto>\n")
           .append("<SessionID>1266917419910</SessionID>\n")
           .append("<MsgID>1</MsgID>\n")
           .append("<Target>\n")
           .append("<LocURI>").append(dc.getDevID()).append("</LocURI>\n")
           .append("</Target>\n")
           .append("<Source>\n")
           .append("<LocURI>" + sc.syncUrl + "</LocURI>\n")
           .append("</Source>\n")
           .append("<RespURI>").append(sc.syncUrl).append(JSESSION_ID).append("</RespURI>\n")
           .append("</SyncHdr>\n")
           .append("<SyncBody>\n");

        int cmdId = 1;

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           //FIXME this number should refers to right message number, but
           //      actually client doesn't take care of it
           .append("<MsgRef>1</MsgRef>\n")
           .append("<CmdRef>0</CmdRef>\n")
           .append("<Cmd>SyncHdr</Cmd>\n")
           .append("<TargetRef>").append(sc.syncUrl).append("</TargetRef>\n")
           .append("<SourceRef>").append(dc.getDevID()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");

        res.append("<Sync>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<Target>\n")
           .append("<LocURI>briefcase</LocURI>\n")
           .append("</Target>\n")
           .append("<Source>\n")
           .append("<LocURI>briefcase</LocURI>\n")
           .append("</Source>\n");

        for(int i=0;i<20;++i) {
            res.append("<Replace>\n")
               .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
               .append("<Item>\n")
               .append("<Source>\n")
               .append("<LocURI>1205</LocURI>\n")
               .append("</Source>\n")
               .append("<Meta>\n")
               .append("<Type xmlns=\"syncml:metinf\">application/*</Type>\n")
               .append("</Meta>\n")
               .append("<Data>This is just a test for performance evaluation</Data>\n")
               .append("</Item>\n")
               .append("</Replace>\n");
        }
 
        res.append("</Sync>\n");

        if (last) {
            res.append("<Final/>\n");
        }

        res.append("</SyncBody>\n")
           .append("</SyncML>");

        return res.toString().getBytes();
    }

}

