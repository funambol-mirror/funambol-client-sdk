/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2008 Funambol, Inc.
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


package com.funambol.push;

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;
import java.io.IOException;
import com.funambol.util.FunBasicTest;

import j2meunit.framework.*;

/**
 * This tests use a inner class that extends CTPService in order to be used as a 
 * mock object for the tests.
 */
public class CTPServiceTest extends FunBasicTest implements CTPListener {

    CTPServiceTester cst = null;
    
    /** Creates a new instance of CTPServiceTest */
    public CTPServiceTest() {
        super(13, "CTPService Test");
        Log.initLog(new ConsoleAppender(), Log.DEBUG);
    }

    public void setUp() {
        cst = new CTPServiceTester();
        cst.setCTPListener(this);
    }

    public void tearDown() {
        cst = null;
    }

    
    public void test(int i) throws Throwable {
        switch(i) {
            case 0:
                testStartService();
                break;
            case 1:
                testSetOfflineMode();
                break;
            case 2:
                testIsOfflineMode();
                break;
            case 3:
                testIsNotOfflineMode();
                break;
            case 4:
                testStopService();
                break;
            case 5:
                testForceDisconnect();
                testGetCTPStringState();
                break;
            case 6:
                testGetCTPStringState();
                break;
            case 7:
                testIsPushActive();
                break;
            case 8:
                testRestartService();
                break;
            case 9:
                testIsRunning();
                break;
            case 10:
                testGetInstance();
                break;
            case 11:
                testGetConfig();
                break;
            case 12:
                testSetConfig();
                break;
            default:
                break;
        }
    }

    
    /**
     * Test the setConfig of the CTP service
     */
    public void testSetConfig() throws Exception {
        PushConfig pc1 = new PushConfig();
        PushConfig pc2 = new PushConfig();
        PushConfig pc3 = new PushConfig();
        cst.setConfig(pc1);
        cst.setConfig(pc2);
        cst.setConfig(pc3);
        assertTrue(cst.getConfig().equals(pc3));
    }
    /**
     * Test the getConfig of the CTP service
     */
    public void testGetConfig() throws Exception {
        PushConfig pc = new PushConfig();
        cst.setConfig(pc);
        assertTrue(cst.getConfig().equals(pc));
    }
    /**
     * Test the getInstance of the CTP service
     */
    public void testGetInstance() throws Exception {
        assertTrue(cst instanceof CTPServiceTester);
    }
    
    /**
     * Test the isRunning of the CTP service
     */
    public void testIsRunning() throws Exception {
        boolean init = cst.isRunning()==false;
        
        cst.startService(new PushConfig());
        
        Thread.sleep(2000);
        
        assertTrue(cst.isRunning()&&init);
        cst.stopService();
    }
        
    /**
     * Test the restartService of the CTP service
     */
    public void testRestartService() throws Exception {
        
        cst.restartService(new PushConfig());
        
        Thread.sleep(2000);
        assertTrue(cst.state==cst.LISTENING);
        cst.stopService();
    }
        

    /**
     * Test the startService of the CTP service
     */
    public void testStartService() throws Exception {
        cst.startService(new PushConfig());
        try {
            // assertion not really useful due to threads usage
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
        }
        assertTrue(cst.state==cst.LISTENING);
        cst.stopService();
    }
        
    /**
     * Test the setOfflineMode of the CTP service
     */
    public void testSetOfflineMode() throws Exception {
        cst.startService();
        cst.setOfflineMode(true);
        assertTrue(cst.isOfflineMode());
        cst.stopService();
    }
        
    /**
     * Test the isOfflineMode of the CTP service
     */
    public void testIsOfflineMode() throws Exception {
        cst.setOfflineMode(false);
        assertTrue(!cst.isOfflineMode());
    }
        
    /**
     * Test the isOfflineMode of the CTP service
     */
    public void testIsNotOfflineMode() throws Exception {
        cst.setOfflineMode(true);
        assertTrue(cst.isOfflineMode());
    }
        
    /**
     * Test the stopService of the CTP service
     */
    public void testStopService() throws Exception {
        cst.startService(new PushConfig());
        try {
            // assertion not really useful due to threads usage
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
        }
        cst.stopService();
        assertTrue(cst.state==cst.DISCONNECTED);
    }
       
    /**
     * Test the forceDisconnect of the CTP service
     */
    public void testForceDisconnect() throws Exception {
        cst.startService(new PushConfig());
        try {
            // assertion not really useful due to threads usage
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
        }
        cst.stopService();
        cst.forceDisconnect();
        assertTrue(cst.state==cst.DISCONNECTED);
    }
    
    /**
     * Test the getCTPStringState method
     */
    public void testGetCTPStringState() {
        cst.state = cst.DISCONNECTED;
        boolean result = "Disconnected".equals(cst.getCTPStringState());
        cst.state = cst.CONNECTING;
        result = "Connecting...".equals(cst.getCTPStringState());
        cst.state = cst.CONNECTED;
        result = "Connected".equals(cst.getCTPStringState());
        cst.state = cst.AUTHENTICATING;
        result = "Authenticating...".equals(cst.getCTPStringState());
        cst.state = cst.AUTHENTICATED;
        result = "Authenticated".equals(cst.getCTPStringState());
        cst.state = cst.LISTENING;
        result = "SAN Listening...".equals(cst.getCTPStringState());
        assertTrue(result);
    }
    
    /**
     * Test the isPushActive method
     */
    public void testIsPushActive() {
        boolean init = cst.isPushActive()==false;
        cst.startService(new PushConfig());
        try {
            // assertion not really useful due to threads usage
            Thread.sleep(2000);
        } catch (InterruptedException ex) {
        }
        boolean result = init && cst.isPushActive(); 
        assertTrue(result);
        cst.stopService();
    }
    
    public class CTPServiceTester extends CTPService {
        
        /**
         * Overrides the connect method in order to giv e mock object on the 
         * IO stream
         */
        protected void connect(int retry){
            state=CONNECTED;
            Log.info("[CTPServiceTest]connect");
        }
        
        /**
         * Overrides the connect method in order to giv e mock object on the 
         * IO stream
         */
        protected void disconnect(){
            state=DISCONNECTED;
            Log.info("[CTPServiceTest]disconnect");
        }

        /**
         * Overrides the connect method in order to giv e mock object on the 
         * IO stream
         */
        protected void closeConnection() {
            Log.info("[CTPServiceTest]closeConnection");
        }

        /**
         * Overrides the connect method in order to giv e mock object on the 
         * IO stream
         */
        protected CTPMessage receiveMessage() throws IOException {
            Log.info("[CTPServiceTest]receivemessage");
            CTPMessage ctpmsg = new CTPMessage();
            ctpmsg.setCommand(ST_OK);
            return ctpmsg;
        }

        protected void sendMessage(CTPMessage message) throws IOException {
            Log.info("[CTPServiceTest]sendmessage");
        }

        protected CTPMessage receiveMessageWithTimeout() throws IOException {
            Log.info("[CTPServiceTest]receivemessagewithTO");
            return new CTPMessage();
        }

        protected int authenticate() throws IOException {
            Log.info("[CTPServiceTest]authenticate");
            state = AUTHENTICATED;
            return ST_OK;
        }
    }

    public void CTPDisconnected() {
        Log.info("[CTPServiceTest-listener]Disconnected");
    }

    public void CTPConnecting() {
        Log.info("[CTPServiceTest-listener]Connecting");
    }

    public void CTPConnected() {
        Log.info("[CTPServiceTest-listener]Connected");
    }

    public void CTPAuthenticating() {
        Log.info("[CTPServiceTest-listener]Authenticating");
    }

    public void CTPAuthenticated() {
        Log.info("[CTPServiceTest-listener]Authenticated");
    }

    public void CTPListening() {
        Log.info("[CTPServiceTest-listener]Listening");
    }
}
