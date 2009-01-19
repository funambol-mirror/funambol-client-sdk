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

import com.funambol.util.ChunkedString;
import com.funambol.util.Log;
import com.funambol.util.MailDateFormatter;
import com.funambol.util.QuotedPrintable;

import jmunit.framework.cldc10.AssertionFailedException;
import jmunit.framework.cldc10.TestCase;

/**
 * Checks the functionality of the MIME parser
 */
public class MIMEProcessorTest extends TestCase {

    /** Field for the MIMEProcessor instance */
    MIMEProcessor mp;
    
    /** The message given by mp */
    Message message;
    
    /**
     * The default constructor for the test class
     */
    public MIMEProcessorTest() {
        super(4, "MIMEProcessorTest");
        Log.setLogLevel(Log.DEBUG);
    }
    
    /**
     * Sets up the test environment
     */
    public void setUp() {
        //Creates a MIMEProcessor instance
        mp = new MIMEProcessor();
        //Parse the message located in the class com.funambol.mail.SampleMessage
    }

    /**
     * Tears down the test environment
     */
    public void tearDown() {
        mp = null;
    }

    /**
     * @see jmunit.framework.cldc11.TestCase#test(int)
     */
    public void test(int testNumber) throws Throwable {
        switch (testNumber) {
            case 0:
                testparseRFC2822();
            case 1:
                testparseRFC2822ByChunkedString();
            case 2:
                testparseTHUNDERBIRD();
            case 3:
                testparseTHUNDERBIRDByChunkedString();
            default:
                break;
        }
    }

    /**
     * Test the parse method for a standard RFC2822 mail message
     * 
     */
    public void testparseRFC2822()
                                throws AssertionFailedException, MailException {
        Log.debug("parseRFC2822");
        Message m = mp.parseMailMessage(SampleMessage.RFC_2822);
        
        assertEquals("MsgID", SampleMessage.RFC2822_MSGID, m.getMessageId());
        assertEquals("Received Date", SampleMessage.RFC2822_RECEIVED, MailDateFormatter.dateToRfc2822(m.getReceivedDate()));
        assertEquals("From", SampleMessage.RFC2822_FROM, m.getFrom().toString());
        assertEquals("To", SampleMessage.RFC2822_TO, m.getTo()[0].toString());
        assertEquals("Subject", SampleMessage.RFC2822_SUBJECT, m.getSubject());
        assertEquals("Content Type", SampleMessage.RFC2822_CONTENT_TYPE, m.getContentType());
        assertEquals("Content", SampleMessage.RFC2822_CONTENT, m.getContent());
    
    }
    
    /**
     * Test the parse method for a standard RFC2822 mail message
     * wrapping with a chunkedstring object
     */
    public void testparseRFC2822ByChunkedString()
                                throws AssertionFailedException, MailException {

        Log.debug("parseRFC2822ByChunkedString");
        Message m = mp.parseMailMessage(new ChunkedString(SampleMessage.RFC_2822));
        
        assertEquals("MsgID", SampleMessage.RFC2822_MSGID, m.getMessageId());
        assertEquals("Received Date", SampleMessage.RFC2822_RECEIVED, MailDateFormatter.dateToRfc2822(m.getReceivedDate()));
        assertEquals("From", SampleMessage.RFC2822_FROM, m.getFrom().toString());
        assertEquals("To", SampleMessage.RFC2822_TO, m.getTo()[0].toString());
        assertEquals("Subject", SampleMessage.RFC2822_SUBJECT, m.getSubject());
        assertEquals("Content Type", SampleMessage.RFC2822_CONTENT_TYPE, m.getContentType());
        assertEquals("Content", SampleMessage.RFC2822_CONTENT, m.getContent());
    }
    
    /**
     * Test the parse method for a mail sent by mozilla thunderbird email client
     * 
     */
    public void testparseTHUNDERBIRD()
                                throws AssertionFailedException, MailException {
        Log.debug("parseTHUNDERBIRD");
        Message m = mp.parseMailMessage(SampleMessage.THUNDERBIRD);
        
        assertEquals("MsgID", SampleMessage.THUNDERBIRD_MSGID, m.getMessageId());
        assertEquals("Received Date", SampleMessage.THUNDERBIRD_RECEIVED, MailDateFormatter.dateToRfc2822(m.getReceivedDate()));
        assertEquals("From", SampleMessage.THUNDERBIRD_FROM, m.getFrom().toString());
        assertEquals("To 0", SampleMessage.THUNDERBIRD_TO_0, m.getTo()[0].toString());
        assertEquals("To 1", SampleMessage.THUNDERBIRD_TO_1, m.getTo()[1].toString());
        assertEquals("Cc 0", SampleMessage.THUNDERBIRD_CC_0, m.getCc()[0].toString());
        assertEquals("Subject", SampleMessage.THUNDERBIRD_SUBJECT, m.getSubject());
        assertEquals("Content Type", SampleMessage.THUNDERBIRD_CONTENT_TYPE, m.getContentType());
        assertEquals("Content", SampleMessage.THUNDERBIRD_CONTENT, m.getContent());
    
    }
    
    /**
     * Test the parse method for a mail sent by mozilla thunderbird email client
     * wrapping with a chunkedstring object
     */
    public void testparseTHUNDERBIRDByChunkedString()
                                throws AssertionFailedException, MailException {

        Log.debug("parseTHUNDERBIRDByChunkedString");
        Message m = mp.parseMailMessage(new ChunkedString(SampleMessage.THUNDERBIRD));
        
        assertEquals("MsgID", SampleMessage.THUNDERBIRD_MSGID, m.getMessageId());
        assertEquals("Received Date", SampleMessage.THUNDERBIRD_RECEIVED, MailDateFormatter.dateToRfc2822(m.getReceivedDate()));
        assertEquals("From", SampleMessage.THUNDERBIRD_FROM, m.getFrom().toString());
        assertEquals("To 0", SampleMessage.THUNDERBIRD_TO_0, m.getTo()[0].toString());
        assertEquals("To 1", SampleMessage.THUNDERBIRD_TO_1, m.getTo()[1].toString());
        assertEquals("Cc 0", SampleMessage.THUNDERBIRD_CC_0, m.getCc()[0].toString());
        assertEquals("Subject", SampleMessage.THUNDERBIRD_SUBJECT, m.getSubject());
        assertEquals("Content Type", SampleMessage.THUNDERBIRD_CONTENT_TYPE, m.getContentType());
        assertEquals("Content", SampleMessage.THUNDERBIRD_CONTENT, m.getContent());
    }
}
