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

import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;

import com.funambol.sync.SyncException;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;

import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.Sync;
import com.funambol.syncml.protocol.Cred;
import com.funambol.syncml.protocol.Meta;
import com.funambol.syncml.protocol.Target;
import com.funambol.syncml.client.TestSyncSource;
import com.funambol.util.CodedException;
import com.funambol.util.Log;
import com.funambol.util.TransportAgent;

import junit.framework.*;

/**
 * Test the SyncManager authentication methods
 */
public class SyncManagerAuthTest extends TestCase {

    private static final String TAG_LOG = "SyncManagerAuthTest";

    private final String TEST_SERVER_URL  = "http://test.server.url";
    private final String TEST_USERNAME    = "test";
    private final String TEST_PASSWORD    = "test";

    private final String TEST_END_MESSAGE = "Test Ended";

    private final String TEST_NONCE_1     = "OzkyP1Q5VjY4aXBtLCpROQ==";
    private final String TEST_NONCE_2     = "WSd/QFQkLFVhekA8IF4xPw==";
    private final String JSESSION_ID      = ";jsessionid=F2EA56F802D65950FAC3E37336BE1EEA.NODE01";
    
    private SyncManager  sm = null;
    private SyncConfig   sc = null;
    private DeviceConfig dc = null;
    private SourceConfig ssc = null;
    private TestSyncSource tss = null;

    private int sentMessagesCount = 0;
    
    public SyncManagerAuthTest(String name) {
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
        
        sm = new SyncManager(sc, dc);

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
    public void testAuthentication_BASIC_OK() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_OK started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;
        
        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {
            
            public byte[] handleMessage(byte message[]) throws Exception {

                byte response[] = null;
                sentMessagesCount++;
                
                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_OK().getBytes("UTF-8");
                        break;
                    case 2:
                        assertTrue(hasSyncCommand(message));
                        throw new SyncException(SyncException.CLIENT_ERROR, 
                                TEST_END_MESSAGE);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 2);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_OK successfull");
        }
    }

    /**
     * Test failed basic authentication. Reply 401
     */
    public void testAuthentication_BASIC_KO() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;
        
        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte message[]) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO().getBytes("UTF-8");
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO successfull");
        }
    }

    /**
     * Test failed basic authentication. Require MD5 authentication.
     */
    public void testAuthentication_BASIC_KO_TO_MD5() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5 started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte message[]) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        assertTrue(sc.clientNonce == null);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5().getBytes("UTF-8");
                        break;
                    case 2:
                        // Handle second message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertRespUri(message, sc.syncUrl + JSESSION_ID);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_OK().getBytes("UTF-8");
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(hasSyncCommand(message));
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5 successfull");
        }
    }

    /**
     * Test failed basic authentication. Require MD5 authentication.
     */
    public void testAuthentication_BASIC_KO_TO_MD5_KO() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5_KO started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte message[]) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        assertTrue(sc.clientNonce == null);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5().getBytes("UTF-8");
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO().getBytes("UTF-8");
                        break;
                    case 3:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 2);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5_KO successfull");
        }
    }

    /**
     * Test failed basic authentication. Require MD5 authentication, but the
     * client doesn't allow it.
     */
    public void testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;
        sc.supportedAuthTypes = new int[] {SyncConfig.AUTH_TYPE_BASIC};

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte message[]) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO_TO_MD5().getBytes("UTF-8");
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO_TO_MD5_NOT_ALLOWED successfull");
        }
    }

    /**
     * Test successfull MD5 authentication
     */
    public void testAuthentication_MD5_OK() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_OK started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte message[]) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        response = getServerResponse_AUTH_MD5_OK().getBytes("UTF-8");
                        break;
                    case 2:
                        // The client should send now the sync command
                        assertTrue(hasSyncCommand(message));
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 2);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_OK successfull");
        }
    }

    /**
     * Test failed MD5 authentication. Reply 401 without nonce
     */
    public void testAuthentication_MD5_KO() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte[] message) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        response = getServerResponse_AUTH_MD5_KO().getBytes("UTF-8");
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_BASIC_KO successfull");
        }
    }

    /**
     * Test failed MD5 authentication. Send new nonce.
     */
    public void testAuthentication_MD5_KO_NEW_NONCE() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_NEW_NONCE started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte[] message) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message

                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_NEW_NONCE().getBytes("UTF-8");
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_2);
                        response = getServerResponse_AUTH_MD5_OK_NONCE2().getBytes("UTF-8");
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(hasSyncCommand(message));
                        assertEquals(sc.clientNonce, TEST_NONCE_2);
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_NEW_NONCE successfull");
        }
    }

    /**
     * Test failed MD5 authentication. Require basic authentication.
     */
    public void testAuthentication_MD5_KO_TO_BASIC() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte[] message) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC().getBytes("UTF-8");
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_OK().getBytes("UTF-8");
                        break;
                    case 3:
                        // The client should send now the sync command
                        assertTrue(hasSyncCommand(message));
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                TEST_END_MESSAGE);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.CLIENT_ERROR);
            assertTrue(ex.getMessage().endsWith(TEST_END_MESSAGE));
            assertTrue(sentMessagesCount == 3);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC successfull");
        }
    }

    /**
     * Test failed MD5 authentication. Require basic authentication.
     */
    public void testAuthentication_MD5_KO_TO_BASIC_KO() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC_KO started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte[] message) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC().getBytes("UTF-8");
                        break;
                    case 2:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_BASIC);
                        response = getServerResponse_AUTH_BASIC_KO().getBytes("UTF-8");
                        break;
                    case 3:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 2);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC_KO successfull");
        }
    }

    /**
     * Test failed MD5 authentication. Require basic authentication, but the
     * client doesn't allow it.
     */
    public void testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED() throws Exception {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED started");
        }

        sc.preferredAuthType = SyncConfig.AUTH_TYPE_MD5;
        sc.supportedAuthTypes = new int[] {SyncConfig.AUTH_TYPE_MD5};
        sc.clientNonce = TEST_NONCE_1;

        sm.setTransportAgent(new TestTransportAgent(new TestMessageHandler() {

            public byte[] handleMessage(byte[] message) throws Exception {

                byte[] response = null;
                sentMessagesCount++;

                switch(sentMessagesCount) {
                    case 1:
                        // Handle first message
                        assertAuthType(message, SyncML.AUTH_TYPE_MD5);
                        assertEquals(sc.clientNonce, TEST_NONCE_1);
                        response = getServerResponse_AUTH_MD5_KO_TO_BASIC().getBytes("UTF-8");
                        break;
                    case 2:
                        // The client shouldn't send further messages
                        assertTrue(false);
                }
                return response;
            }

            public String handleMessage(String message) throws Exception {
                return null;
            }
        }));

        try {
            sm.sync(tss);
        } catch(SyncException ex) {
            assertTrue(ex.getCode() == SyncException.AUTH_ERROR);
            assertTrue(sentMessagesCount == 1);
        }

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "testAuthentication_MD5_KO_TO_BASIC_NOT_ALLOWED successfull");
        }
    }

    private void assertAuthType(byte message[], String authType) throws Exception {
        // The client should send now the sync command
        SyncMLParser parser = new SyncMLParser(false);
        SyncML msg = parser.parse(message);
        assertTrue(msg.getSyncHdr() != null);
        Cred cred = msg.getSyncHdr().getCred();
        assertTrue(cred != null);
        Meta meta = cred.getMeta();
        assertTrue(meta != null);
        String type = meta.getType();
        assertEquals(type, authType);
    }

    private void assertRespUri(byte message[], String respURI) throws Exception {
        // The client should send now the sync command
        SyncMLParser parser = new SyncMLParser(false);
        SyncML msg = parser.parse(message);
        assertTrue(msg.getSyncHdr() != null);
        Target target = msg.getSyncHdr().getTarget();
        assertTrue(target != null);
        String uri = target.getLocURI();
        assertEquals(respURI, uri);
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
               "<Data>401</Data>\n" +
               "</Status>";
    }

    private String getStatus_AUTH_MD5_KO_NONCE() {
        return "<Status>\n" +
               "<CmdID>1</CmdID>\n" +
               "<MsgRef>1</MsgRef>\n" +
               "<CmdRef>0</CmdRef>\n" +
               "<Cmd>SyncHdr</Cmd>\n" +
               "<TargetRef>" + sc.syncUrl + JSESSION_ID  + "</TargetRef>\n" +
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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
               "<SourceRef>" + dc.getDevID() + "</SourceRef>\n" +
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

    private boolean hasSyncCommand(byte message[]) throws Exception {
        // The client should send now the sync command
        SyncMLParser parser = new SyncMLParser(false);
        SyncML msg = parser.parse(message);
        assertTrue(msg.getSyncBody() != null);
        Vector commands = msg.getSyncBody().getCommands();
        assertTrue(commands != null);
        assertTrue(commands.size() > 0);
        boolean found = false;
        for(int i=0;i<commands.size();++i) {
            Object command = commands.elementAt(i);
            if (command instanceof Sync) {
                found = true;
                break;
            }
        }
        return found;
    }
    
    private class TestTransportAgent implements TransportAgent {

        private TestMessageHandler handler;

        public TestTransportAgent(TestMessageHandler h) {
            handler = h;
        }

        public String sendMessage(String request, String charset) throws CodedException {
            try {
                return handler.handleMessage(request);
            } catch (Exception e) {
                throw new CodedException(-1, e.toString());
            }
        }

        public String sendMessage(String request) throws CodedException {
            return sendMessage(request, null);
        }

        public byte[] sendMessage(byte[] request) throws CodedException {
            try {
                return handler.handleMessage(request);
            } catch (Exception e) {
                throw new CodedException(-1, e.toString());
            }
        }

        public void setRetryOnWrite(int retries) { }
        public void setRequestURL(String requestUrl) { }
        public String getResponseDate() { return new Date().toString(); }

        public void setRequestContentType(String contentType) {
        }

        public void setCustomHeaders(Hashtable headers) {
        }
    }

    private interface TestMessageHandler {
        public String handleMessage(String message) throws Exception;
        public byte[] handleMessage(byte[] message) throws Exception;
    }
}

