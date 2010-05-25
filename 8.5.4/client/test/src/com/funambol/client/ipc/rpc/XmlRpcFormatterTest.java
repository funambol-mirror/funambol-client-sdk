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
import java.util.Vector;

import junit.framework.*;

import com.funambol.util.Log;
import com.funambol.util.XmlUtil;
import com.funambol.util.ConsoleAppender;

/**
 * Test case for the MessageManager class.
 * The tests initialize the message store, perform the initial sync to
 * get what is on the server, send a test message to self, get the response
 * and remove the sent items.
 */
public class XmlRpcFormatterTest extends TestCase {

    public XmlRpcFormatterTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }

    public void tearDown() {
    }

    public void testMethodCall1() throws Exception {
        XmlRpcMessage msg = new XmlRpcMessage();
        msg.setMethodCall(true);
        msg.setMethodName("testMethodCall");
        RPCParam param = new RPCParam();
        param.setIntValue(4);
        msg.addParam(param);

        XmlRpcFormatter formatter = new XmlRpcFormatter();
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        formatter.format(msg, os);

        String expectedOutput = "<?xml version=\"1.0\"?>\n" +
                                "<methodCall>\n" +
                                "<methodName>testMethodCall</methodName>\n" +
                                "<params>\n" +
                                "<param>\n" +
                                "<value><int>4</int></value></param>\n" +
                                "</params>\n" +
                                "</methodCall>\n\n";

        assertTrue(expectedOutput.equals(os.toString()));
    }

    public void testMethodCallWithArray() throws Exception {
        XmlRpcMessage msg = new XmlRpcMessage();
        msg.setMethodCall(true);
        msg.setMethodName("testMethodCall");
        RPCParam param = new RPCParam();
        param.setIntValue(4);
        msg.addParam(param);

        RPCParam values[] = new RPCParam[2];
        RPCParam v0 = new RPCParam();
        v0.setBooleanValue(false);
        values[0] = v0;

        RPCParam v1 = new RPCParam();
        v1.setIntValue(3);
        values[1] = v1;

        param = new RPCParam();
        param.setArrayValue(values);
        msg.addParam(param);

        XmlRpcFormatter formatter = new XmlRpcFormatter();
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        formatter.format(msg, os);

        String expectedOutput = "<?xml version=\"1.0\"?>\n" +
                                "<methodCall>\n" +
                                "<methodName>testMethodCall</methodName>\n" +
                                "<params>\n" +
                                "<param>\n" +
                                "<value><int>4</int></value></param>\n" +
                                "<param>\n" +
                                "<value><array><data><value><boolean>0</boolean></value>" +
                                "<value><int>3</int></value></data></array></value></param>\n" +
                                "</params>\n" +
                                "</methodCall>\n\n";

        assertTrue(expectedOutput.equals(os.toString()));
    }

    public void testFaultyMethodResponse() throws Exception {
        XmlRpcMessage msg = new XmlRpcMessage();
        msg.setMethodCall(false);
        msg.setFault(true);
        RPCParam param = new RPCParam();

        RPCParam members[] = new RPCParam[2];
        RPCParam faultCode = new RPCParam();
        faultCode.setName("faultCode");
        faultCode.setIntValue(4);
        RPCParam faultReason = new RPCParam();
        faultReason.setName("faultString");
        faultReason.setStringValue("Too many parameters.");
        members[0] = faultCode;
        members[1] = faultReason;

        param.setStructValue(members);
        msg.addParam(param);

        XmlRpcFormatter formatter = new XmlRpcFormatter();
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        formatter.format(msg, os);

        String expectedOutput = "<?xml version=\"1.0\"?>\n" +
                                "<methodResponse>\n" +
                                "<fault>\n" +
                                "<value><struct><member>\n" +
                                "<name>faultCode</name>\n" +
                                "<value><int>4</int></value>\n" +
                                "</member>\n" +
                                "<member>\n" +
                                "<name>faultString</name>\n" +
                                "<value>Too many parameters.</value>\n" +
                                "</member>\n" +
                                "</struct></value></fault>\n" +
                                "</methodResponse>\n\n";

        assertTrue(expectedOutput.equals(os.toString()));
    }

    public void testFaultyMethodResponse2() throws Exception {
        XmlRpcMessage msg = new XmlRpcMessage();
        msg.setMethodCall(false);
        msg.setFault(4, "Too many parameters.");

        XmlRpcFormatter formatter = new XmlRpcFormatter();
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        formatter.format(msg, os);

        String expectedOutput = "<?xml version=\"1.0\"?>\n" +
                                "<methodResponse>\n" +
                                "<fault>\n" +
                                "<value><struct><member>\n" +
                                "<name>faultCode</name>\n" +
                                "<value><int>4</int></value>\n" +
                                "</member>\n" +
                                "<member>\n" +
                                "<name>faultString</name>\n" +
                                "<value>Too many parameters.</value>\n" +
                                "</member>\n" +
                                "</struct></value></fault>\n" +
                                "</methodResponse>\n\n";

        assertTrue(expectedOutput.equals(os.toString()));
    }

}
