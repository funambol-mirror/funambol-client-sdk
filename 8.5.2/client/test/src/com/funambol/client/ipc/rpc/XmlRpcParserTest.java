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

import java.io.ByteArrayInputStream;
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
public class XmlRpcParserTest extends TestCase {
    
    public XmlRpcParserTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }
    
    public void tearDown() {
    }
    
    public void testMethodResponse() throws Exception {

        String state = "South Dakota";
        String xmlRpc = "<?xml version=\"1.0\"?>\n" +
                        "<methodResponse>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value><string>" + state + "</string></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodResponse>\n";

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(xmlRpc.getBytes());
        XmlRpcMessage message = parser.parse(is);
        // Check all the message properties
        assertTrue(!message.isMethodCall());
        Vector params = message.getParams();
        assertTrue(params.size() == 1);
        RPCParam param = (RPCParam)params.elementAt(0);
        assertTrue(param.getType() == RPCParam.TYPE_STRING);
        assertTrue(state.equals(param.getStringValue()));
    }

    public void testMethodCall() throws Exception {

        String methodName = "examples.getStateName";
        String xmlRpc = "<?xml version=\"1.0\"?>\n" +
                        "<methodCall>\n" +
                        "<methodName>" + methodName + "</methodName>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value><i4>41</i4></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodCall>\n";

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(xmlRpc.getBytes());
        XmlRpcMessage message = parser.parse(is);
        // Check all the message properties
        assertTrue(message.isMethodCall());
        assertTrue(methodName.equals(message.getMethodName()));
        Vector params = message.getParams();
        assertTrue(params.size() == 1);
        RPCParam param = (RPCParam)params.elementAt(0);
        assertTrue(param.getType() == RPCParam.TYPE_INT);
        assertTrue(param.getIntValue() == 41);
    }

    public void testMethodCallEscaping() throws Exception {

        String methodName = "examples.getStateName";
        String value = "Is 5 < 4?";
        String escapedValue = XmlUtil.escapeXml(value);

        String xmlRpc = "<?xml version=\"1.0\"?>\n" +
                        "<methodCall>\n" +
                        "<methodName>" + methodName + "</methodName>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value>" + escapedValue + "</value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodCall>\n";

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(xmlRpc.getBytes());
        XmlRpcMessage message = parser.parse(is);
        // Check all the message properties
        assertTrue(message.isMethodCall());
        assertTrue(methodName.equals(message.getMethodName()));
        Vector params = message.getParams();
        assertTrue(params.size() == 1);
        RPCParam param = (RPCParam)params.elementAt(0);
        assertTrue(param.getType() == RPCParam.TYPE_STRING);
        assertTrue(value.equals(param.getStringValue()));
    }

    public void testMethodCallWithArray() throws Exception {

        String methodName = "examples.getStateName";
        String value = "Is 5 < 4?";
        String escapedValue = XmlUtil.escapeXml(value);

        String xmlRpc = "<?xml version=\"1.0\"?>\n" +
                        "<methodCall>\n" +
                        "<methodName>" + methodName + "</methodName>\n" +
                        "<params>\n" +
                        "<param>\n" +
                        "<value>" + escapedValue + "</value>\n" +
                        "</param>\n" +
                        "<param>\n" +
                        "<value><array><data>\n" +
                        "<value><i4>12</i4></value>\n" +
                        "<value><string>Egypt</string></value>\n" +
                        "<value><boolean>0</boolean></value>\n" +
                        "<value><i4>-31</i4></value>\n" +
                        "</data></array></value>\n" +
                        "</param>\n" +
                        "</params>\n" +
                        "</methodCall>\n";

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(xmlRpc.getBytes());
        XmlRpcMessage message = parser.parse(is);
        // Check all the message properties
        assertTrue(message.isMethodCall());
        assertTrue(methodName.equals(message.getMethodName()));
        Vector params = message.getParams();
        assertTrue(params.size() == 2);
        RPCParam param = (RPCParam)params.elementAt(0);
        assertTrue(param.getType() == RPCParam.TYPE_STRING);
        assertTrue(value.equals(param.getStringValue()));
        param = (RPCParam)params.elementAt(1);
        RPCParam arrayValues[] = param.getArrayValue();
        assertTrue(arrayValues.length == 4);
        assertTrue(arrayValues[0].getIntValue() == 12);
        assertTrue(arrayValues[1].getStringValue().equals("Egypt"));
        assertTrue(arrayValues[2].getBooleanValue() == false);
        assertTrue(arrayValues[3].getIntValue() == -31);
    }

    public void testFaultResponse() throws Exception {

        String xmlRpc = "<?xml version=\"1.0\"?>\n" +
                        "<methodResponse>\n" +
                        "<fault>\n" +
                        "<value>\n" +
                        "<struct>\n" +
                        "<member>\n" +
                        "<name>faultCode</name>\n" +
                        "<value><int>4</int></value>\n" +
                        "</member>\n" +
                        "<member>\n" +
                        "<name>faultString</name>\n" +
                        "<value><string>Too many parameters.</string></value>\n" +
                        "</member>\n" +
                        "</struct>\n" +
                        "</value>\n" +
                        "</fault>\n" +
                        "</methodResponse>\n";

        XmlRpcParser parser = new XmlRpcParser();
        ByteArrayInputStream is = new ByteArrayInputStream(xmlRpc.getBytes());
        XmlRpcMessage message = parser.parse(is);
        // Check all the message properties
        assertTrue(message.isFault());
        Vector params = message.getParams();
        assertTrue(params.size() == 1);
        RPCParam param = (RPCParam)params.elementAt(0);
        assertTrue(param.getType() == RPCParam.TYPE_STRUCT);
        RPCParam members[] = param.getStructValue();
        assertTrue(members.length == 2);
        RPCParam m1 = members[0];
        RPCParam m2 = members[1];

        assertTrue("faultCode".equals(m1.getName()));
        assertTrue("faultString".equals(m2.getName()));
        assertTrue(m1.getType() == RPCParam.TYPE_INT);
        assertTrue(m2.getType() == RPCParam.TYPE_STRING);
        assertTrue(m1.getIntValue() == 4);
        assertTrue("Too many parameters.".equals(m2.getStringValue()));
    }

}

