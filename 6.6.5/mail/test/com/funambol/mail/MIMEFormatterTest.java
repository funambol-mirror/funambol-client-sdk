/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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

package com.funambol.mail;

import com.funambol.util.StringUtil;
import java.util.Date;

import jmunit.framework.cldc10.AssertionFailedException;
import jmunit.framework.cldc10.TestCase;

import com.funambol.util.Log;
import com.funambol.util.MailDateFormatter;

/**
 * A JMUnit test class to check the functionality of the MIME formatter.
 * Implementing the SampleMessage interface this class has direct access to all
 * fields defined in that interface
 */
public class MIMEFormatterTest extends TestCase {

    /**
     * The message object to catch the data from that are to be formatted into a
     * RFC 2822 conform text
     */
    Message message = null;

    /**
     * A store (located on the device's RMS)
     */
    RMSStore store = null;

    /**
     * A folder to hold the message
     */
    Folder folder = null;

    /**
     * Today's date
     */
    Date date = null;

    /**
     * The message's content
     */
    String content = null;

    /**
     * Carriage Return + Line Feed
     */
    private static final String CRLF = "\r\n";


    

    /**
     * The constructor. Please refer to the JMUnit documentation
     */
    public MIMEFormatterTest() {
        super(1, "MIMEFormatterTest");
        Log.setLogLevel(Log.DEBUG);
    }


    /**
     * Prepares the conditions to make the tests. A test message is parsed by
     * the {@link MIMEParser} and a {@link} Message} object is built. A new
     * {@link Folder} in the RMS store is created and the message is appended to
     * it
     * 
     * @see jmunit.framework.cldc11.TestCase#setUp()
     */
    public void setUp() throws Exception {
        Address address = new Address(Address.TO, "monty@funambol.com");
        Address[] list = { address };
        content = "Hello, Fun!";
        date = new Date();

        message = new Message();
        message.addRecipients(list);
        message.addHeader(Message.FROM, "giuseppe.monticelli@gmail.com");
        message.addHeader(Message.SUBJECT, "An experiment with the SyncML");
        message.setSentDate(date);
        //message.setMessageId("11952120870002092558033@Funambol.mail");
        message.setContent(content);

        store = new RMSStore();
        folder = store.createFolder("inbox");
        folder.appendMessage(message);
    }


    /**
     * @see jmunit.framework.cldc11.TestCase#tearDown()
     */
    public void tearDown() {
        // not used
    }


    /**
     * @see jmunit.framework.cldc11.TestCase#test(int)
     */
    public void test(int testNumber) throws Throwable {
        switch (testNumber) {
            case 0:
                testFormat();
                break;
            default:
                break;
        }
    }
    
    /**
     * Tests the content of the "From:" header attribute in the formatted
     * message: it has to match the original one contained in the test email
     * message stored in a string constant in the {@link SampleMessage} interface
     * 
     * 
     * @throws AssertionFailedException
     */
    public void testFormat() throws AssertionFailedException {

        Log.debug("toString");

        Message m = null;
        
        String result = null;
        
        // here only the first (and unique) item is retrieved
        MIMEFormatter formatter = null;
        try {
            m = folder.getMessage("1");
            formatter = new MIMEFormatter();
            result = formatter.format(m);    
        } catch (MailException e) {
            Log.error("Error by retrieving a Message from the Folder:");
            e.printStackTrace();
        }

        String dateRfc2822 = MailDateFormatter.dateToRfc2822(m.getSentDate());
        String expectedResult =
            "Date: " + dateRfc2822 + CRLF +
            "MIME-Version: 1.0" + CRLF +
            "To: monty@funambol.com" + CRLF +
            "Subject: " + CRLF +
            "Message-ID: " + m.getMessageId() + CRLF +
            "Content-Type: text/plain; charset=UTF-8" + CRLF +
            "Content-Transfer-Encoding: 8bit" + CRLF + // empty string
            CRLF +
            content + CRLF
            ;

        Log.debug("Expected:\n\n" + expectedResult.toString());
        Log.debug("Result:\n\n" + result.toString());

        //The string coming from the Message object must be contained in the
        //formatted message
        assertTrue(StringUtil.equalsIgnoreCase(result.toString(), expectedResult.toString()));
    }


}
