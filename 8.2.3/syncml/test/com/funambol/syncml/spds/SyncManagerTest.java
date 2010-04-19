/*
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

import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.client.TestSyncSource;
import com.funambol.util.ChunkedString;
import com.funambol.util.CodedException;
import com.funambol.util.Log;
import com.funambol.util.TransportAgent;
import com.funambol.util.XmlUtil;

import java.util.Date;

import junit.framework.*;

/**
 * Test the SyncManager methods
 */
public class SyncManagerTest extends TestCase {

    private final String TEST_SERVER_URL  = "http://test.server.url";
    private final String TEST_USERNAME    = "test";
    private final String TEST_PASSWORD    = "test";

    private final String TEST_END_MESSAGE = "Test Ended";

    private final String TEST_NONCE_1     = "OzkyP1Q5VjY4aXBtLCpROQ==";
    private final String TEST_NONCE_2     = "WSd/QFQkLFVhekA8IF4xPw==";
    
    private SyncManager  sm = null;
    private SyncConfig   sc = null;
    private SourceConfig ssc = null;
    private TestSyncSource tss = null;

    private int sentMessagesCount = 0;
    
    public SyncManagerTest(String name) {
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
        sc.deviceConfig = new DeviceConfig();

        ssc = new SourceConfig();
        
        sm = new SyncManager(sc);

        tss = new TestSyncSource(ssc);
        tss.ITEMS_NUMBER = 0;

        sentMessagesCount = 0;
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
    public void testAuthentication_BASIC_OK() {

        Log.info("testAuthentication_BASIC_OK started");

        sc.authType = SyncML.AUTH_TYPE_BASIC;
        
        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {
            
            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;
                
                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_OK();
                        break;
                    case 2:
                        // The client should send now the sync command
                        assertTrue(XmlUtil.getTag(new ChunkedString(message),
                                SyncML.TAG_SYNC) != -1);
                        throw new SyncException(SyncException.CLIENT_ERROR, 
                                TEST_END_MESSAGE);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 2);
        }

        Log.info("testAuthentication_BASIC_OK successfull");
    }

    /**
     * Test failed basic authentication. Reply 401
     */
    public void testAuthentication_BASIC_KO() {

        Log.info("testAuthentication_BASIC_KO started");

        sc.authType = SyncML.AUTH_TYPE_BASIC;
        
        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO();
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        Log.info("testAuthentication_BASIC_KO successfull");
    }

    /**
     * Test failed basic authentication. Require MD5 authentication.
     */
    public void testAuthentication_BASIC_KO_TO_MD5() {

        Log.info("testAuthentication_BASIC_KO_TO_MD5 started");

        sc.authType = SyncML.AUTH_TYPE_BASIC;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        assertTrue(sc.clientNonce == null);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5();
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_OK();
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(XmlUtil.getTag(new ChunkedString(message),
                                SyncML.TAG_SYNC) != -1);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        Log.info("testAuthentication_BASIC_KO_TO_MD5 successfull");
    }

    /**
     * Test failed basic authentication. Require MD5 authentication.
     */
    public void testAuthentication_BASIC_KO_TO_MD5_KO() {

        Log.info("testAuthentication_BASIC_KO_TO_MD5_KO started");

        sc.authType = SyncML.AUTH_TYPE_BASIC;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        assertTrue(sc.clientNonce == null);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5();
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO();
                        break;
                    case 3:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 2);
        }

        Log.info("testAuthentication_BASIC_KO_TO_MD5_KO successfull");
    }

    /**
     * Test failed basic authentication. Require MD5 authentication, but the
     * client doesn't allow it.
     */
    public void testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED() {

        Log.info("testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED started");

        sc.authType = SyncML.AUTH_TYPE_BASIC;
        sc.supportedAuthTypes = new String[] {SyncML.AUTH_TYPE_BASIC};

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5();
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        Log.info("testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED successfull");
    }

    /**
     * Test successfull MD5 authentication
     */
    public void testAuthentication_MD5_OK() {

        Log.info("testAuthentication_MD5_OK started");

        sc.authType = SyncML.AUTH_TYPE_MD5;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        response = getServerResponse_AUTH_MD5_OK();
                        break;
                    case 2:
                        // The client should send now the sync command
                        assertTrue(XmlUtil.getTag(new ChunkedString(message),
                                SyncML.TAG_SYNC) != -1);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 2);
        }

        Log.info("testAuthentication_MD5_OK successfull");
    }

    /**
     * Test failed MD5 authentication. Reply 401 without nonce
     */
    public void testAuthentication_MD5_KO() {

        Log.info("testAuthentication_BASIC_KO started");

        sc.authType = SyncML.AUTH_TYPE_MD5;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        response = getServerResponse_AUTH_MD5_KO();
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        Log.info("testAuthentication_BASIC_KO successfull");
    }

    /**
     * Test failed MD5 authentication. Send new nonce.
     */
    public void testAuthentication_MD5_KO_NEW_NONCE() {

        Log.info("testAuthentication_MD5_KO_NEW_NONCE started");

        sc.authType = SyncML.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message

                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_NEW_NONCE();
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_2);
                        response = getServerResponse_AUTH_MD5_OK_NONCE2();
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(XmlUtil.getTag(new ChunkedString(message),
                                SyncML.TAG_SYNC) != -1);
                        assertEquals(sc.clientNonce, TEST_NONCE_2);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        Log.info("testAuthentication_MD5_KO_NEW_NONCE successfull");
    }

    /**
     * Test failed MD5 authentication. Require basic authentication.
     */
    public void testAuthentication_MD5_KO_TO_BASIC() {

        Log.info("testAuthentication_MD5_KO_TO_BASIC started");

        sc.authType = SyncML.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC();
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_OK();
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(XmlUtil.getTag(new ChunkedString(message),
                                SyncML.TAG_SYNC) != -1);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        Log.info("testAuthentication_MD5_KO_TO_BASIC successfull");
    }

    /**
     * Test failed MD5 authentication. Require basic authentication.
     */
    public void testAuthentication_MD5_KO_TO_BASIC_KO() {

        Log.info("testAuthentication_MD5_KO_TO_BASIC_KO started");

        sc.authType = SyncML.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC();
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO();
                        break;
                    case 3:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 2);
        }

        Log.info("testAuthentication_MD5_KO_TO_BASIC_KO successfull");
    }

    /**
     * Test failed MD5 authentication. Require basic authentication, but the
     * client doesn't allow it.
     */
    public void testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED() {

        Log.info("testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED started");

        sc.authType = SyncML.AUTH_TYPE_MD5;
        sc.supportedAuthTypes = new String[] {SyncML.AUTH_TYPE_MD5};
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public String handleMessage(String message) {

                String response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC();
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        Log.info("testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED successfull");
    }

    public void assertAuthType(String message, String authType) {
        ChunkedString type = null;
        try {
            ChunkedString cred = XmlUtil.getTagValue(new ChunkedString(message), "Cred");
            type = XmlUtil.getTagValue(cred, "Type");
        } catch(Exception ex) {
            assertTrue(false);
        }
        assertEquals(type.toString(), authType);
    }

    private String getServerResponse_AUTH_BASIC_OK() {
        return getServerResponseFromStatus(getStatus_AUTH_BASIC_OK(), true);
    }

    private String getServerResponse_AUTH_BASIC_KO() {
        return getServerResponseFromStatus(getStatus_AUTH_BASIC_KO(), true);
    }

    private String getServerResponse_AUTH_BASIC_KO_TO_MD5() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_KO_NONCE(), true);
    }

    private String getServerResponse_AUTH_MD5_OK() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_OK(), true);
    }

    private String getServerResponse_AUTH_MD5_OK_NONCE2() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_OK_NONCE2(), true);
    }

    private String getServerResponse_AUTH_MD5_KO() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_KO(), true);
    }

    private String getServerResponse_AUTH_MD5_KO_TO_BASIC() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_KO_TO_BASIC(), true);
    }

    private String getServerResponse_AUTH_MD5_KO_NONCE() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_KO_NONCE(), true);
    }

    private String getServerResponse_AUTH_MD5_KO_NEW_NONCE() {
        return getServerResponseFromStatus(getStatus_AUTH_MD5_KO_NEW_NONCE(), true);
    }

    private String getStatus_AUTH_BASIC_OK() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               "<Data>212</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_BASIC_KO() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               "<Data>401</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_OK() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               getChallenge(TEST_NONCE_1, SyncML.AUTH_TYPE_MD5) +
               "<Data>212</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_OK_NONCE2() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               getChallenge(TEST_NONCE_2, SyncML.AUTH_TYPE_MD5) +
               "<Data>212</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_KO() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               "<Data>401</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_KO_NONCE() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               getChallenge(TEST_NONCE_1, SyncML.AUTH_TYPE_MD5) +
               "<Data>401</Data>\n" +
               "</Status>";
    }
    
    private String getStatus_AUTH_MD5_KO_NEW_NONCE() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               getChallenge(TEST_NONCE_2, SyncML.AUTH_TYPE_MD5) +
               "<Data>401</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_KO_TO_BASIC() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + "</TargetRef>\n" +
               "<SourceRef>" + sc.deviceConfig.devID + "</SourceRef>\n" +
               getChallenge(null, SyncML.AUTH_TYPE_BASIC) +
               "<Data>401</Data>\n" +
               "</Status>";
    }

    private String getChallenge(String nextNonce, String type) {
        return "<Chal>\n" +
               "<Meta>\n" +
               (type.equals(SyncML.AUTH_TYPE_MD5) ?
                    "<Format xmlns='syncml:metinf'>b64</Format>\n" : "") +
               "<Type xmlns='syncml:metinf'>" + type + "</Type>\n" +
               (nextNonce != null ?
                    "<NextNonce xmlns='syncml:metinf'>" + nextNonce + "</NextNonce>\n" : "") +
               "</Meta>\n" +
               "</Chal>\n";
    }

    private String getServerResponseFromStatus(String status, boolean addAlertFromServer) {
        return "<SyncML>\n" +
               "<SyncHdr>\n" +
               "<VerDTD>1.2</VerDTD>\n" +
               "<VerProto>SyncML/1.2</VerProto>\n" +
               "<SessionID>1266917419910</SessionID>\n" +
               "<MsgID>1</MsgID>\n" +
               "<Target>\n" +
               "<LocURI>" + sc.deviceConfig.devID + "</LocURI>\n" +
               "</Target>\n" +
               "<Source>\n" +
               "<LocURI>" + sc.syncUrl + "</LocURI>\n" +
               "</Source>\n" +
               "<RespURI>" + sc.syncUrl + ";jsessionid=F2" +
               "EA56F802D65950FAC3E37336BE1EEA.NODE01</RespURI>\n" +
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

    
    private class TestTransportAgent implements TransportAgent {

        private TestMessageHandler handler;

        public TestTransportAgent(TestMessageHandler h) {
            handler = h;
        }

        public String sendMessage(String request, String charset) throws CodedException {
            return handler.handleMessage(request);
        }

        public String sendMessage(String request) throws CodedException {
            return sendMessage(request, null);
        }

        public void setRetryOnWrite(int retries) { }
        public void setRequestURL(String requestUrl) { }
        public String getResponseDate() { return new Date().toString(); }
        
    }

    private interface TestMessageHandler {
        public String handleMessage(String message);
    }
}

