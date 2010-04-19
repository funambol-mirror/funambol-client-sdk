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
import java.util.Vector;

import junit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.XmlUtil;
import com.funambol.util.ConsoleAppender;

public class RPCManagerTest extends TestCase {

    private class TestRPCServer extends RPCServer {

        private int replyDelay = 0;
        private String lastMsg = null;

        private class AnswerThread extends Thread {
            public AnswerThread() {
            }

            public void run() {
                try {
                    Thread.sleep(replyDelay * 1000);
                } catch (Exception e) {}
                if (resp != null && listener != null) {
                    listener.messageReceived(resp);
                }
            }
        }

        public TestRPCServer() {
        }

        private String resp = null;

        public void sendMessage(String msg) throws IOException {
            lastMsg = msg;
            // Simulate an immediate response
            if (replyDelay == 0) {
                if (resp != null && listener != null) {
                    listener.messageReceived(resp);
                    resp = null;
                }
            } else {
                // Start a thread
                AnswerThread at = new AnswerThread();
                at.start();
            }
        }

        public void setNextResponse(String resp) {
            this.resp = resp;
        }

        public boolean isRunning() {
            return true;
        }

        public void startService() {
        }

        public void setReplyDelay(int value) {
            replyDelay = value;
        }

        public String getLastMsg() {
            return lastMsg;
        }
    }

    public RPCManagerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }
    
    public void tearDown() {
    }
    
    public void testInvoke1() throws Exception {

        TestRPCServer server = new TestRPCServer();
        String state = "Massachussets";
        String resp =   "<?xml version=\"1.0\"?>\n" +
                        "<methodResponse>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value><string>" + state + "</string></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodResponse>\n";
        server.setNextResponse(resp);
        server.setReplyDelay(0);

        RPCManager manager = RPCManager.getInstance();
        manager.setRPCServer(server);
        server.setListener(manager);
        RPCParam res = manager.invoke("TestMethod", null);
        // We expect a string in return
        String retState = res.getStringValue();
        assertTrue(state.equals(retState));
    }

    public void testInvoke2() throws Exception {

        TestRPCServer server = new TestRPCServer();
        String state = "Massachussets";
        String resp =   "<?xml version=\"1.0\"?>\n" +
                        "<methodResponse>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value><string>" + state + "</string></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodResponse>\n";
        server.setNextResponse(resp);
        server.setReplyDelay(3);

        RPCManager manager = RPCManager.getInstance();
        manager.setRPCServer(server);
        server.setListener(manager);
        RPCParam res = manager.invoke("TestMethod", null);
        // We expect a string in return
        String retState = res.getStringValue();
        assertTrue(state.equals(retState));
    }

    private class GetStateInfoMethod extends RPCMethod {
        RPCManagerTest test;

        public GetStateInfoMethod(RPCManagerTest test) {
            super("getStateInfo");
            this.test = test;
        }

        public RPCParam execute(RPCParam[] params) throws Exception {
            // We expect one parameter (the state name)
            if (params.length != 1) {
                throw new IllegalArgumentException("Expected one parameter");
            }
            RPCParam param = params[0];
            if (param.getType() != RPCParam.TYPE_STRING) {
                throw new IllegalArgumentException("Expected a string parameter");
            }

            // Perform the callback
            String res = test.getStateInfo(param.getStringValue());
            RPCParam ret = new RPCParam();
            ret.setStringValue(res);
            return ret;
        }
    }

    public String getStateInfo(String state) {
        return "No info available for " + state;
    }

    public void testInvoked1() throws Exception {

        TestRPCServer server = new TestRPCServer();
        String state = "Massachussets";
        String call  =   "<?xml version=\"1.0\"?>\n" +
                        "<methodCall>\n" +
                        "<methodName>getStateInfo</methodName>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value><string>" + state + "</string></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodCall>\n";

        RPCManager manager = RPCManager.getInstance();
        GetStateInfoMethod method1 = new GetStateInfoMethod(this);
        manager.register(method1);

        manager.setRPCServer(server);
        server.setListener(manager);

        manager.messageReceived(call);
        try {
            Thread.sleep(1000);
        } catch (Exception e) {}
        String retValue = server.getLastMsg();

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(retValue.getBytes());
        XmlRpcMessage r = parser.parse(is);
        assertTrue(!r.isMethodCall());
        Vector params = r.getParams();
        assertTrue(params.size() == 1);
        RPCParam infoParam = (RPCParam)params.elementAt(0);
        assertTrue(infoParam.getType() == RPCParam.TYPE_STRING);
        String info = infoParam.getStringValue();
        assertTrue("No info available for Massachussets".equals(info));
    }

    public void testReceivedInvalidMessage() throws Exception {

        TestRPCServer server = new TestRPCServer();
        String state = "Massachussets";
        String resp = "<?xml version=\"1.0\"?>\n" +
                          "<methodResponse>\n" +
                          "<struct>\n" +
                          "<member>\n" +
                          "<name>field1</name>\n" +
                          "<value>test</value>\n" +
                          "</member>\n" +
                          "</struct>\n" +
                          "</methodResponse>\n";


        server.setNextResponse(resp);
        server.setReplyDelay(0);

        RPCManager manager = RPCManager.getInstance();
        manager.setRPCServer(server);
        server.setListener(manager);
        boolean exc = false;
        try {
            RPCParam res = manager.invoke("TestMethod", null);
        } catch (RPCException e) {
            exc = true;
        }
        assertTrue(exc);
    }
} 
