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
import com.funambol.util.Base64;
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
     * Carriage Return + Line Feed
     */
    private static final String CRLF = "\r\n";

    /**
     * The constructor. Please refer to the JMUnit documentation
     */
    public MIMEFormatterTest() {
        super(3, "MIMEFormatterTest");
        Log.setLogLevel(Log.DEBUG);
    }


    /**
     * Prepares the conditions to make the tests. A test message is parsed by
     * the {@link MIMEParser} and a {@link} Message} object is built.
     * 
     * @see jmunit.framework.cldc11.TestCase#setUp()
     */
    public void setUp() throws Exception {
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
            case 1:
                testFormatB64WithCTE();
                break;
            case 2:
                testFormatB64WithoutCTE();
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

        try {
            String to = "monty@funambol.com";
            String from = "giuseppe.monticelli@gmail.com";
            String subject = "An experiment with the SyncML";
            String content = "Hello, Fun!";
            String messageId = "1@Funambol.com";

            Address address = new Address(Address.TO, to);
            Address[] list = { address };
            Date date = new Date();

            Message message = new Message();
            message.addRecipients(list);
            message.addHeader(Message.FROM, from);
            message.addHeader(Message.SUBJECT, subject);
            message.setSentDate(date);
            message.setContent(content);
            message.setMessageId(messageId);

            MIMEFormatter formatter = new MIMEFormatter();
            String result = formatter.format(message);    

            String dateRfc2822 = MailDateFormatter.dateToRfc2822(message.getSentDate());
            String expectedResult =
                "Date: " + dateRfc2822 + CRLF +
                "From: " + from + CRLF +
                "MIME-Version: 1.0" + CRLF +
                "To: monty@funambol.com" + CRLF +
                "Subject: " + subject + CRLF +
                "Message-ID: " + messageId + CRLF +
                "Content-Transfer-Encoding: 8bit" + CRLF + // empty string
                "Content-Type: text/plain; charset=UTF-8" + CRLF +
                CRLF +
                content + CRLF
                ;

            Log.debug("Expected:\n\n" + expectedResult.toString());
            Log.debug("Result:\n\n" + result.toString());

            //The string coming from the Message object must be contained in the
            //formatted message
            assertTrue(StringUtil.equalsIgnoreCase(result.toString(), expectedResult.toString()));

        } catch (MailException e) {
            Log.error("Error by retrieving a Message from the Folder:");
            e.printStackTrace();
            assertTrue(false);
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
    public void testFormatB64WithCTE() throws AssertionFailedException {

        try {
            String to = "monty@funambol.com";
            String from = "giuseppe.monticelli@gmail.com";
            String subject = "An experiment with the SyncML";
            String content = "Hello, Fun!";
            String messageId = "1@Funambol.com";

            Address address = new Address(Address.TO, to);
            Address[] list = { address };
            String contentB64;
            try {
                contentB64 = new String(Base64.encode(content.getBytes("UTF-8")));
            } catch (Exception e) {
                contentB64 = new String(Base64.encode(content.getBytes()));
            }
            Date date = new Date();

            Message message = new Message();
            message.addRecipients(list);
            message.addHeader(Message.FROM, from);
            message.addHeader(Message.SUBJECT, subject);
            message.addHeader(Message.CONTENT_TRANSFER_ENCODING, "base64");
            message.setSentDate(date);
            message.setContent(content);
            message.setMessageId(messageId);

            MIMEFormatter formatter = new MIMEFormatter();
            String result = formatter.format(message);    

            String dateRfc2822 = MailDateFormatter.dateToRfc2822(message.getSentDate());
            String expectedResult =
                "Date: " + dateRfc2822 + CRLF +
                "From: " + from + CRLF +
                "MIME-Version: 1.0" + CRLF +
                "To: " + to + CRLF +
                "Subject: " + subject + CRLF +
                "Message-ID: " + messageId + CRLF +
                "Content-Transfer-Encoding: base64" + CRLF + // empty string
                "Content-Type: text/plain; charset=UTF-8" + CRLF +
                CRLF +
                contentB64 + CRLF
                ;

            Log.debug("Expected:\n\n" + expectedResult.toString());
            Log.debug("Result:\n\n" + result.toString());

            //The string coming from the Message object must be contained in the
            //formatted message
            assertTrue(StringUtil.equalsIgnoreCase(result.toString(), expectedResult.toString()));

        } catch (MailException e) {
            Log.error("Error by retrieving a Message from the Folder:");
            e.printStackTrace();
            assertTrue(false);
        }
    }

    public void testFormatB64WithoutCTE() throws AssertionFailedException {

        try {
            String to = "monty@funambol.com";
            String from = "giuseppe.monticelli@gmail.com";
            String subject = "An experiment with the SyncML";
            byte contentBytes[] = new byte[80];
            String messageId = "1@Funambol.com";

            for(int i=0;i<contentBytes.length;++i) {
                contentBytes[i] = (byte)i;
            }
            String content = new String(contentBytes);

            Address address = new Address(Address.TO, to);
            Address[] list = { address };
            String contentB64;
            try {
                contentB64 = new String(Base64.encode(content.getBytes("UTF-8")));
            } catch (Exception e) {
                contentB64 = new String(Base64.encode(content.getBytes()));
            }
            // Since the content is longer then 76 chars, we expect it to be
            // broken on two lines
            contentB64 = contentB64.substring(0, 76) + CRLF + contentB64.substring(76) + CRLF;
            Date date = new Date();

            Message message = new Message();
            message.addRecipients(list);
            message.addHeader(Message.FROM, from);
            message.addHeader(Message.SUBJECT, subject);
            message.setSentDate(date);
            message.setContent(content);
            message.setMessageId(messageId);

            MIMEFormatter formatter = new MIMEFormatter();
            String result = formatter.format(message);    

            String dateRfc2822 = MailDateFormatter.dateToRfc2822(message.getSentDate());
            String expectedResult =
                "Date: " + dateRfc2822 + CRLF +
                "From: " + from + CRLF +
                "MIME-Version: 1.0" + CRLF +
                "To: " + to + CRLF +
                "Subject: " + subject + CRLF +
                "Message-ID: " + messageId + CRLF +
                "Content-Transfer-Encoding: base64" + CRLF + // empty string
                "Content-Type: text/plain; charset=UTF-8" + CRLF +
                CRLF +
                contentB64 + CRLF
                ;

            Log.debug("Expected:\n\n" + expectedResult.toString());
            Log.debug("Result:\n\n" + result.toString());

            //The string coming from the Message object must be contained in the
            //formatted message
            assertTrue(StringUtil.equalsIgnoreCase(result.toString(), expectedResult.toString()));

        } catch (MailException e) {
            Log.error("Error by retrieving a Message from the Folder:");
            e.printStackTrace();
            assertTrue(false);
        }
    }
}

