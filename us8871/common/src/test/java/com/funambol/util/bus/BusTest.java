/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.util.bus;

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;
import java.util.Vector;

import junit.framework.TestCase;

/**
 * Represents a simple bus communication framework which allows handlers to be
 * registered for upcoming messages of a specific type. Messages can be sent
 * without knowing the actual handlers or can be sent expecting a message result
 * from a specific handler. This implementation does not support message results
 * from different handlers.
 * This class implementation is thread safe.
 */
public class BusTest extends TestCase {

    private Bus bus;

    private class TestBusMessageHandler implements BusMessageHandler {

        Vector receivedMessages = new Vector();

        public TestBusMessageHandler() {
        }

        /**
         * Receives and handles a specific bus message.
         * @param message
         */
        public void receiveMessage(BusMessage message) {
            receivedMessages.addElement(message);
        }

        public Vector getReceivedMessages() {
            return receivedMessages;
        }
    }
    
    public BusTest(String name) {
        super(name);
    }
    
    public void setUp() {
        bus = Bus.getInstance();
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }
    
    public void tearDown() {
    }

    public void testSendMessage1() throws Exception {
        TestBusMessageHandler handler = new TestBusMessageHandler();
        bus.registerMessageHandler(BusMessageType1.class, handler);

        BusMessage msg0 = new BusMessageType1("FirstMessage");
        bus.sendMessage(msg0);

        BusMessage msg1 = new BusMessageType2("DiscardedMessage");
        bus.sendMessage(msg1);

        BusMessage msg2 = new BusMessageType1("SecondMessage");
        bus.sendMessage(msg2);

        // These calls are asynchronous, wait some time before checking the
        // receiver
        sleep(500);

        Vector receivedMessages = handler.getReceivedMessages();
        assertEquals(2, receivedMessages.size());

        BusMessage m0 = (BusMessage)receivedMessages.elementAt(0);
        BusMessage m1 = (BusMessage)receivedMessages.elementAt(1);

        assertTrue(m0 instanceof BusMessageType1);
        assertTrue(m1 instanceof BusMessageType1);

        String content0 = (String)m0.getMessage();
        String content1 = (String)m1.getMessage();

        assertEquals("FirstMessage", content0);
        assertEquals("SecondMessage", content1);

        bus.unregisterMessageHandler(BusMessageType1.class, handler);
    }

    public void testUnregisterMessage() throws Exception {
        TestBusMessageHandler handler = new TestBusMessageHandler();
        bus.registerMessageHandler(BusMessageType1.class, handler);

        BusMessage msg0 = new BusMessageType1("FirstMessage");
        bus.sendMessage(msg0);

        // These calls are asynchronous, wait some time before checking the
        // receiver
        sleep(500);

        // Now unregister
        bus.unregisterMessageHandler(BusMessageType1.class, handler);

        BusMessage msg1 = new BusMessageType1("DiscardedMessage");
        bus.sendMessage(msg1);

        BusMessage msg2 = new BusMessageType1("SecondMessage");
        bus.sendMessage(msg2);

        // These calls are asynchronous, wait some time before checking the
        // receiver
        sleep(500);

        Vector receivedMessages = handler.getReceivedMessages();
        assertEquals(receivedMessages.size(), 1);

        BusMessage m0 = (BusMessage)receivedMessages.elementAt(0);

        assertTrue(m0 instanceof BusMessageType1);
        
        String content0 = (String)m0.getMessage();

        assertEquals("FirstMessage", content0);
    }

    public void testSendMessage2() throws Exception {
        // By registering on the BusMessage type, we should receive all messages
        TestBusMessageHandler handler = new TestBusMessageHandler();
        bus.registerMessageHandler(BusMessage.class, handler);

        BusMessage msg0 = new BusMessageType1("FirstMessage");
        bus.sendMessage(msg0);

        BusMessage msg1 = new BusMessageType2("ThirdMessage");
        bus.sendMessage(msg1);

        BusMessage msg2 = new BusMessageType1("SecondMessage");
        bus.sendMessage(msg2);

        // These calls are asynchronous, wait some time before checking the
        // receiver
        sleep(500);

        Vector receivedMessages = handler.getReceivedMessages();
        assertEquals(receivedMessages.size(), 3);

        BusMessage m0 = (BusMessage)receivedMessages.elementAt(0);
        BusMessage m1 = (BusMessage)receivedMessages.elementAt(1);
        BusMessage m2 = (BusMessage)receivedMessages.elementAt(2);

        assertTrue(m0 instanceof BusMessageType1);
        assertTrue(m1 instanceof BusMessageType2);
        assertTrue(m2 instanceof BusMessageType1);

        String content0 = (String)m0.getMessage();
        String content1 = (String)m1.getMessage();
        String content2 = (String)m2.getMessage();

        assertEquals("FirstMessage", content0);
        assertEquals("ThirdMessage", content1);
        assertEquals("SecondMessage", content2);

        bus.unregisterMessageHandler(BusMessage.class, handler);
    }


    private class BusMessageType1 extends BusMessage {
        public BusMessageType1(Object msg) {
            super(msg);
        }
    }

    private class BusMessageType2 extends BusMessage {
        public BusMessageType2(Object msg) {
            super(msg);
        }
    }

    private void sleep(long time) {
        try {
            Thread.sleep(time);
        } catch (Exception e) {}
    }
}

