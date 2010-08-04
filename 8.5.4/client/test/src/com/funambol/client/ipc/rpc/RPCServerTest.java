/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.client.ipc.rpc;

import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.Vector;

import junit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

public class RPCServerTest extends TestCase {

    private TestRpcServer rpcServer;
    private boolean serviceConnected;
    private boolean messageSent;
    private int testStatus;

    private String rpcMsg;

    private String CONTENT_LENGTH = "Content-Length: ";
    private String EOL = "\r\n";
    private String content = "TestRpcServer";

    private String testString =CONTENT_LENGTH+ content.length()+EOL+EOL+content;

    private ByteArrayInputStream testIs;
    

    public RPCServerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
        
    }

    public void setUp(){
        serviceConnected = false;
        messageSent = false;

        // The string must be plain ASCII
        byte arr[] = new byte[testString.length()];
        for(int i=0;i<testString.length();++i) {
            char ch = testString.charAt(i);
            arr[i] = (byte)ch;
        }
        testIs = new ByteArrayInputStream(arr);
       

        rpcServer = new TestRpcServer();
        rpcServer.setListener(new TestRpcListener());
    }
    
    public void tearDown() {
        rpcServer = null;
    }
    
    public void testServiceConnected() throws Exception {
        rpcServer.connect();
        assertTrue(serviceConnected);
    }

    

    public void testSendMessage()throws Exception{
        rpcServer.sendMessage(content);
        assertTrue(messageSent);
        assertEquals(testString, rpcMsg);
    }


    public void testListen()throws Exception{

        rpcServer.startService();
        try {
            Thread.sleep(2 * 1000);
        } catch (Exception e) {}
        assertEquals(content, rpcMsg);
    }


    private class TestRpcServer extends RPCServer {

        public TestRpcServer (){ 
            is = testIs;
            os = new ByteArrayOutputStream();
        }

        public void sendMessage(String msg) throws IOException{
            super.sendMessage(msg);
            rpcMsg = os.toString();
        }

        protected void connect (){
            done=false;
            status = CONNECTED;
            testStatus = CONNECTED;
            listener.serviceConnected();
        }

        protected void disconnect() {
            done=true;
            status = DISCONNECTED;
            testStatus = DISCONNECTED;
            listener.serviceStopped();
        }

    }

    private class TestRpcListener implements RPCServerListener{

        public TestRpcListener(){
        }

        public void messageReceived(String msg){
            rpcMsg = msg;
            rpcServer.disconnect();
        }

        public void messageSent(){
            messageSent = true;
            
        }

        public void serviceConnected() {
            serviceConnected = true;
        }

        public void serviceStopped() {
        }

        public void serviceListening() {
        }

        public void serviceDisconnected() {
        }
    }
    
} 
