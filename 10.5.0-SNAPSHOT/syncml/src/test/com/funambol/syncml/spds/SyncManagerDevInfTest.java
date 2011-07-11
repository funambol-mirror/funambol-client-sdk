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
import com.funambol.util.StringUtil;

import junit.framework.*;

/**
 * Test the SyncManager performance
 */
public class SyncManagerDevInfTest extends TestCase {

    private static final String TAG_LOG = "SyncManagerDevInfTest";

    private final String TEST_SERVER_URL  = "http://test.server.url";
    private final String TEST_USERNAME    = "test";
    private final String TEST_PASSWORD    = "test";

    private SyncManager  syncManager = null;
    private SyncConfig   syncConfig = null;
    private DeviceConfig deviceConfig = null;
    private SourceConfig syncSourceConfig = null;
    private TestSyncSource testSyncSource = null;

    private final String JSESSION_ID = ";jsessionid=F2EA56F802D65950FAC3E37336BE1EEA.NODE01";
    private int sentMessagesCount;

    public SyncManagerDevInfTest(String name) {
        super(name);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {

        syncConfig = new SyncConfig();
        syncConfig.syncUrl = TEST_SERVER_URL;
        syncConfig.userName = TEST_USERNAME;
        syncConfig.password = TEST_PASSWORD;
        deviceConfig = new DeviceConfig();

        syncSourceConfig = new SyncMLSourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase");
        SyncMLAnchor anchor = new SyncMLAnchor();
        syncSourceConfig.setSyncAnchor(anchor);
        
        syncManager = new SyncManager(syncConfig, deviceConfig);
        testSyncSource = new TestSyncSource(syncSourceConfig);

        sentMessagesCount = 0;

//        Log.initLog(new ConsoleAppender(), Log.ERROR);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {

    }

    //----------------------------------------------- DevInf tests

    /**
     * Test that DevInf are sent only one time if server receive them
     */
    public void testSendDevInfOnlyDuringFirstSync() throws Exception {
        Log.debug(TAG_LOG, "testSendDevInfOnlyDuringFirstSync");
        
        syncManager.setTransportAgent(testTransportAgentNormalSync);

        testSyncSource.endSending(); //no item to send to server
        String outgoingStream;
        
        Log.debug(TAG_LOG, "Starting first sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        Log.debug(TAG_LOG, "Starting second sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertFalse("DevInf sent to server", containsDevInf(outgoingStream));

        Log.debug(TAG_LOG, "Starting third sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertFalse("DevInf sent to server", containsDevInf(outgoingStream));
    }
    
    
    /**
     * Tests that DevInf are sent every time when using a {@link SourceConfig}
     * and not a {@link SyncMLSourceConfig}
     */
    public void testSendDevInfEveryTimeWithANormalSyncSource() {
        Log.debug(TAG_LOG, "testSendDevInfEveryTimeWithANormalSyncSource");

        //replaces the config
        syncSourceConfig = new SourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase");
        SyncMLAnchor anchor = new SyncMLAnchor();
        syncSourceConfig.setSyncAnchor(anchor);
        testSyncSource.setConfig(syncSourceConfig);

        syncManager.setTransportAgent(testTransportAgentNormalSync);

        testSyncSource.endSending(); //no item to send to server
        String outgoingStream;
        
        Log.debug(TAG_LOG, "Starting first sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        Log.debug(TAG_LOG, "Starting second sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertTrue("DevInf sent to server", containsDevInf(outgoingStream));
    }


    /**
     * Test that DevInf are sent when the flag to force them is set
     */
    public void testSendDevInfWhenForced() throws Exception {
        Log.debug(TAG_LOG, "testSendDevInfWhenForced");
        
        syncManager.setTransportAgent(testTransportAgentNormalSync);

        testSyncSource.endSending(); //no item to send to server
        String outgoingStream;
        
        Log.debug(TAG_LOG, "Starting first sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        Log.debug(TAG_LOG, "Starting second sync...");
        testTransportAgentNormalSync.flushOutgoingStream();
        syncManager.setFlagSendDevInf();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentNormalSync.getOutgoingStream();
        assertTrue("DevInf sent to server", containsDevInf(outgoingStream));
    }
    
    /**
     * Tests that DevInf are resent if the server doesn't accept them
     */
    public void testSendDevInfWhenServerDoesNotAcceptThem() {
        Log.debug(TAG_LOG, "testSendDevInfWhenServerDoesNotAcceptThem");
        
        TestTransportAgent testTransportAgentDenyDevInf =  new TestTransportAgent(new TestMessageHandler() {
            public byte[] handleMessage(byte message[]) throws Exception {
                String response = null;
                sentMessagesCount++;

                switch (sentMessagesCount) {
                case 1:
                    response = getServerResponse1(false);
                    break;
                case 2:
                    response = getServerResponse2();
                    break;
                case 3:
                    response = getServerResponse3();
                    break;
                case 4:
                    response = getServerResponse4();
                    sentMessagesCount = 0; //restart the count
                    break;
                }

                return response.getBytes("UTF-8");
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }, true);
        
        syncManager.setTransportAgent(testTransportAgentDenyDevInf);

        testSyncSource.endSending(); //no item to send to server
        String outgoingStream;
        
        //DevInf are not accepted
        Log.debug(TAG_LOG, "Starting first sync...");
        testTransportAgentDenyDevInf.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentDenyDevInf.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        //DevInf are resent, but still not accepted
        Log.debug(TAG_LOG, "Starting second sync...");
        testTransportAgentDenyDevInf.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentDenyDevInf.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        //now DevInf are accepted
        testTransportAgentDenyDevInf = testTransportAgentNormalSync;
        syncManager.setTransportAgent(testTransportAgentNormalSync);
        Log.debug(TAG_LOG, "Starting third sync...");
        testTransportAgentDenyDevInf.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentDenyDevInf.getOutgoingStream();
        assertTrue("DevInf not sent to server", containsDevInf(outgoingStream));

        //so there are not send in this sync
        Log.debug(TAG_LOG, "Starting fourth sync...");
        testTransportAgentDenyDevInf.flushOutgoingStream();
        syncManager.sync(testSyncSource);
        outgoingStream = testTransportAgentDenyDevInf.getOutgoingStream();
        assertFalse("DevInf sent to server", containsDevInf(outgoingStream));
    }


    private String getServerResponse1(boolean devInfAcceptedFromServer) {
        StringBuffer res = new StringBuffer();

        createServerHeader(res, 1);

        res.append(getStatus_AUTH_BASIC_OK());
        int cmdId = 2; //1 already used by previous block
        
        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>1</MsgRef>\n")
           .append("<CmdRef>1</CmdRef>\n")
           .append("<Cmd>Alert</Cmd>\n")
           .append("<TargetRef>" + syncSourceConfig.getName() + "</TargetRef>\n")
           .append("<SourceRef>" + syncSourceConfig.getRemoteUri() + "</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>\n")
           .append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>1</MsgRef>\n")
           .append("<CmdRef>2</CmdRef>\n")
           .append("<Cmd>Put</Cmd>\n")
           .append("<SourceRef>./devinf12</SourceRef>\n");
        
        if (devInfAcceptedFromServer) {
            //DevInf accepted
            res.append("<Data>200</Data>\n");
        } else {
            //DevInf not accepted
            res.append("<Data>500</Data>\n");
        }
        
        res.append("</Status>\n")
           .append(getAlertFromServer())
           .append("<Final/>\n")
           .append("</SyncBody>\n")
           .append("</SyncML>");
        
        return res.toString();
    }

    private String getServerResponse2() {
        StringBuffer res = new StringBuffer();
        int cmdId = 1;

        createServerHeader(res, 2);

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>2</MsgRef>\n")
           .append("<CmdRef>0</CmdRef>\n")
           .append("<Cmd>SyncHdr</Cmd>\n")
           .append("<TargetRef>").append(syncConfig.syncUrl).append("</TargetRef>\n")
           .append("<SourceRef>").append(deviceConfig.getDevID()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>2</MsgRef>\n")
           .append("<CmdRef>3</CmdRef>\n")
           .append("<Cmd>Sync</Cmd>\n")
           .append("<TargetRef>").append(syncSourceConfig.getName()).append("</TargetRef>\n")
           .append("<SourceRef>").append(syncSourceConfig.getRemoteUri()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");

        createServerFooter(res);

        return res.toString();
    }

    private String getServerResponse3() {
        StringBuffer res = new StringBuffer();
        int cmdId = 1;

        createServerHeader(res, 3);

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>3</MsgRef>\n")
           .append("<CmdRef>0</CmdRef>\n")
           .append("<Cmd>SyncHdr</Cmd>\n")
           .append("<TargetRef>").append(syncConfig.syncUrl).append("</TargetRef>\n")
           .append("<SourceRef>").append(deviceConfig.getDevID()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");


        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>3</MsgRef>\n")
           .append("<CmdRef>2</CmdRef>\n")
           .append("<Cmd>Alert</Cmd>\n")
           .append("<TargetRef>").append(syncSourceConfig.getName()).append("</TargetRef>\n")
           .append("<SourceRef>").append(syncSourceConfig.getRemoteUri()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("<Item>\n")
           .append("<Source>\n")
           .append("<LocURI>").append(syncSourceConfig.getRemoteUri()).append("</LocURI>\n")
           .append("</Source>\n")
           .append("<Target>\n")
           .append("<LocURI>").append(syncSourceConfig.getName()).append("</LocURI>\n")
           .append("</Target>\n")
           .append("</Item>\n")
           .append("</Status>\n");
        
        res.append("<Sync>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<Target>\n")
           .append("<LocURI>").append(syncSourceConfig.getRemoteUri()).append("</LocURI>\n")
           .append("</Target>\n")
           .append("<Source>\n")
           .append("<LocURI>").append(syncSourceConfig.getName()).append("</LocURI>\n")
           .append("</Source>\n")
           .append("<NumberOfChanges>0</NumberOfChanges>\n")
           .append("</Sync>\n")
           .append("<Final></Final>\n");
        
        createServerFooter(res);

        return res.toString();
    }

    private String getServerResponse4() {
        StringBuffer res = new StringBuffer();
        int cmdId = 1;

        createServerHeader(res, 4);

        res.append("<Status>\n")
           .append("<CmdID>").append(cmdId++).append("</CmdID>\n")
           .append("<MsgRef>4</MsgRef>\n")
           .append("<CmdRef>0</CmdRef>\n")
           .append("<Cmd>SyncHdr</Cmd>\n")
           .append("<TargetRef>").append(syncConfig.syncUrl).append("</TargetRef>\n")
           .append("<SourceRef>").append(deviceConfig.getDevID()).append("</SourceRef>\n")
           .append("<Data>200</Data>\n")
           .append("</Status>");

        res.append("<Final></Final>\n");

        createServerFooter(res);

        return res.toString();
    }

    private void createServerHeader(StringBuffer res, int messageCount) {
        res.append("<SyncML>\n")
           .append("<SyncHdr>\n")
           .append("<VerDTD>1.2</VerDTD>\n")
           .append("<VerProto>SyncML/1.2</VerProto>\n")
           .append("<SessionID>1266917419910</SessionID>\n")
           .append("<MsgID>").append(messageCount).append("</MsgID>\n")
           .append("<Target>\n")
           .append("<LocURI>").append(deviceConfig.getDevID()).append("</LocURI>\n")
           .append("</Target>\n")
           .append("<Source>\n")
           .append("<LocURI>" + syncConfig.syncUrl + "</LocURI>\n")
           .append("</Source>\n")
           .append("<RespURI>").append(syncConfig.syncUrl).append(JSESSION_ID).append("</RespURI>\n")
           .append("</SyncHdr>\n")
           .append("<SyncBody>\n");
    }
    
    private void createServerFooter(StringBuffer res) {
        res.append("</SyncBody>\n")
           .append("</SyncML>");
    }

    private String getStatus_AUTH_BASIC_OK() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + syncConfig.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + deviceConfig.getDevID() + "</SourceRef>\n" +
               "<Data>212</Data>\n" +
               "</Status>";
    }

    private String getAlertFromServer() {
        return "<Alert>\n" +
               "<CmdID>4</CmdID>\n" +
               "<Data>200</Data>\n" +
               "<Item>\n" +
               "<Target>\n" +
               "<LocURI>" + syncSourceConfig.getName() + "</LocURI>\n" +
               "</Target>\n" +
               "<Source>\n" +
               "<LocURI>" + syncSourceConfig.getRemoteUri() + "</LocURI>\n" +
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
    
    private boolean containsDevInf(String message) {
        if (StringUtil.isNullOrEmpty(message)) return false;
        
        return
            message.contains(
                "<Type xmlns=\"syncml:metinf\">application/vnd.syncml-devinf+wbxml</Type>")
            ||
            message.contains(
                    "<Type xmlns=\"syncml:metinf\">application/vnd.syncml-devinf+xml</Type>");
    }
    

    /**
     * Transport Agent for a normal sync, where DevInf are recognized and
     * accepted by the server
     */
    private final TestTransportAgent testTransportAgentNormalSync =  new TestTransportAgent(new TestMessageHandler() {
        public byte[] handleMessage(byte message[]) throws Exception {
            String response = null;
            sentMessagesCount++;

            switch (sentMessagesCount) {
            case 1:
                response = getServerResponse1(true);
                break;
            case 2:
                response = getServerResponse2();
                break;
            case 3:
                response = getServerResponse3();
                break;
            case 4:
                response = getServerResponse4();
                sentMessagesCount = 0; //restart the count
                break;
            }

            return response.getBytes("UTF-8");
        }

        public String handleMessage(String message) throws Exception {
            return null;
        }
    }, true);

}

