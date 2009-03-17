/*
 * Copyright (C) 2006-2007 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

package com.funambol.mail;

import com.funambol.mail.*;
import com.funambol.storage.ComplexSerializer;
import com.funambol.util.FunBasicTest;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;
import j2meunit.framework.*;

public class MessageTest extends FunBasicTest {

    private Message m = null;
    
    private byte[] messageStream;
 
    public void testGlobalLaziness() throws Exception {
        Log.debug("GlobalLaziness Accessor Methods");
        int globalLaziness = Message.LAZY_CONTENT;
        m.setGlobalLaziness(globalLaziness);
        
        //Calculate global laziness value as the method does
        int expectedResult = globalLaziness|=m.getGlobalLaziness();
        
        assertEquals(expectedResult, m.getGlobalLaziness());
    }

    /**
     * Test of setLaziness method, of class com.funambol.mail.Message.
     */
    public void testLaziness() throws Exception {
        Log.debug("Laziness Accessor Methods");
        int laziness = Message.LAZY_CONTENT;
        m.setLaziness(laziness);
        
        //Calculate global laziness value as the method does
        int expectedResult = laziness|=m.getLaziness();
        
        assertEquals(expectedResult, m.getLaziness());
    }

    /**
     * Test of addRecipients method, of class com.funambol.mail.Message.
     */
    public void testRecipients() throws Exception, Exception {
        Log.debug("addRecipients");
        Address[] list = new Address[3];
        Address[] expected = new Address[3];
        for (int i=0; i<list.length; i++) {
            list[i] = new Address(Address.TO, i+"th@funambol.com");
            expected[i] = new Address(Address.TO, i+"th@funambol.com");
        }
        
        m.addRecipients(list);
        
        Address[] recipients = m.getTo();
        
        boolean realTo = false;
        for (int i=0; i<3; i++) {
            realTo=recipients[i].getEmail().equals(expected[i].getEmail());
            if (!realTo) {
                break;
            }
        }
        //TODO review the generated test code and remove the default call to fail.
        assertTrue(realTo);
    }

    /**
     * Test of addRecipient method, of class com.funambol.mail.Message.
     */
    public void testRecipient() throws Exception, Exception {
        Log.debug("addRecipient");
        Address address = new Address(Address.CC, "address@funambol.com");
        m.addRecipient(address);
        
        boolean realCc = m.getCc()[0].getEmail().equals("address@funambol.com");
        //TODO review the generated test code and remove the default call to fail.
        assertTrue(realCc);
    }

    /**
     * Test of setKey method, of class com.funambol.mail.Message.
     */
    public void testKey() throws Exception {
        Log.debug("key Accessor Methods");
        String key = "Key1";
        m.setKey(key);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(key, m.getKey());
    }

    
    /**
     * Test of setRecordId method, of class com.funambol.mail.Message.
     */
    public void testRecordId() throws Exception {
        Log.debug("RecordId Accessor Methods");
        int record = 1;
        m.setRecordId(record);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(record, m.getRecordId());
    }

    
    /**
     * Test of getMessageId method, of class com.funambol.mail.Message.
     */
    public void testMessageId() throws Exception {
        Log.debug("MessageId Accessor Methods");
        String id = "1";
        m.setMessageId(id);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(id, m.getMessageId());
    }
    

    /**
     * Test of getFlags method, of class com.funambol.mail.Message.
     */
    public void testGetFlags() throws Exception {
        Log.debug("getFlags");
        MessageFlags expectedResult = m.flags;
        
        MessageFlags result = m.getFlags();
        assertEquals(expectedResult, result);
    }

    /**
     * Test of getFlags method, of class com.funambol.mail.Message.
     */
    public void testSetFlags() throws Exception {
        Log.debug("setFlags");
        MessageFlags mf = new MessageFlags();
        mf.setFlag(mf.OPENED, true);
        m.setFlags(mf);
        
        assertTrue(mf.isSet(mf.OPENED)==m.getFlags().isSet(mf.OPENED));
    }

    /**
     * Test of createUniqueMessageIDValue method, of class com.funambol.mail.Message.
     */
    public void testcreateUniqueMessageIDValue() throws Exception {
        Log.debug("UniqueMessageIdCreator");
        //Creates 100 different messageIds and tests if they are truly different
        String messagesId[] = new String[100];
        for (int i=0; i<messagesId.length; i++) {
            messagesId[i] = m.createUniqueMessageIDValue();
        }
        
        boolean result = false;
        for (int j=0; j<messagesId.length-1; j++) {
            if (result) {
                break;
            }
            
            result = messagesId[j].equals(messagesId[j+1]);
        }
        
        assertTrue(result);
    }
    
    
    
    
    /**
     * Test of getFrom method, of class com.funambol.mail.Message.
     */
    public void testFrom() throws Exception, Exception {
        Log.debug("From Accessor Methods");
        m.setFrom(new Address(Address.FROM, "address@funambol.com"));
        assertEquals("address@funambol.com", m.getFrom().getEmail());
    }

    
    
    /**
     * Test of getTo method, of class com.funambol.mail.Message.
     */
    public void testTo() throws Exception, Exception {
        Log.debug("To Accessor Methods");
        Address[] add = new Address[1];
        add[0] = new Address(Address.TO, "address@funambol.com");
        m.setTo(add);
        assertEquals("address@funambol.com", m.getTo()[0].getEmail());
    }

    /**
     * Test of getCc method, of class com.funambol.mail.Message.
     */
    public void testCc() throws Exception, Exception {
        Log.debug("Cc Accessor Methods");
        Address[] add = new Address[1];
        add[0] = new Address(Address.CC, "address@funambol.com");
        m.setCc(add);
        assertEquals("address@funambol.com", m.getCc()[0].getEmail());
    }

    /**
     * Test of getBcc method, of class com.funambol.mail.Message.
     */
    public void testBcc() throws Exception, Exception {
        Log.debug("Bcc Accessor Methods");
        Address[] add = new Address[1];
        add[0] = new Address(Address.BCC, "address@funambol.com");
        m.setBcc(add);
        assertEquals("address@funambol.com", m.getBcc()[0].getEmail());
    }

    /**
     * Test of getReplyTo method, of class com.funambol.mail.Message.
     */
    public void testReplyTo() throws Exception, Exception {
        Log.debug("ReplyTo Accessor Methods");
        Address[] add = new Address[1];
        add[0] = new Address(Address.REPLYTO, "address@funambol.com");
        m.setReplyTo(add);
        assertEquals("address@funambol.com", m.getReplyTo()[0].getEmail());
    }

    
    /**
     * Test of getSubject method, of class com.funambol.mail.Message.
     */
    public void testSubject() throws Exception {
        Log.debug("Subject Accessor Methods");
        m.setSubject("Message Subject");
        assertEquals("Message Subject", m.getSubject());
    }

    /**
     * Test of getSentDate method, of class com.funambol.mail.Message.
     */
    public void testSentDate() throws Exception {
        Log.debug("Sent Date Accessor Methods");
        Date d = new Date(System.currentTimeMillis());
        m.setSentDate(d);
        assertEquals(d, m.getSentDate());
    }

    /**
     * Test of getSentTime method, of class com.funambol.mail.Message.
     */
    public void testSentTime() throws Exception {
        Log.debug("Sent Time Accessor Methods");
        Date d = new Date(System.currentTimeMillis());
        m.setSentDate(d);
        assertEquals(d.getTime(), m.getSentTime());
    }

    /**
     * Test of getReceivedDate method, of class com.funambol.mail.Message.
     */
    public void testReceivedDate() throws Exception {
        Log.debug("Received Date Accessor Methods");
        Date d = new Date(System.currentTimeMillis());
        m.setReceivedDate(d);
        assertEquals(d, m.getReceivedDate());
    }

    /**
     * Test of getReceivedTime method, of class com.funambol.mail.Message.
     */
    public void testReceivedTime() throws Exception {
        Log.debug("Received Time Accessor Methods");
        Date d = new Date(System.currentTimeMillis());
        m.setReceivedDate(d);
        assertEquals(d.getTime(), m.getReceivedTime());
    }

    /**
     * Test of getParent method, of class com.funambol.mail.Message.
     */
    public void testParent() throws Exception {
        Log.debug("Parent Accessor Methods");
        Store store = createMsgStore();
        m.setParent(store.getFolder(store.INBOX));
        assertEquals(store.INBOX, m.getParent().getName());
        removeMsgStore(store);
    }

    /**
     * Test of getContent method, of class com.funambol.mail.Message.
     */
    public void testContent() throws Exception {
        Log.debug("Content Accessor Methods");
        m = createSampleMessage();
        m.setContent("Content of the message");
        assertEquals("Content of the message", m.getContent());
    }

    /**
     * Test of getTextContent method, of class com.funambol.mail.Message.
     */
    public void testgetTextContent() throws Exception {
        Log.debug("getTextContent");
        m = createSampleMessage();
        m.setContent("Content of the message");
        assertEquals("Content of the message", m.getTextContent());
    }

    
    /**
     * Test of getNumberOfRecipients method, of class com.funambol.mail.Message.
     */
    public void testgetNumberOfRecipients() throws Exception, Exception {
        Log.debug("getNumberOfRecipients");
        m = createSampleMessage();
        assertEquals(3, m.getNumberOfRecipients());
    }

    /**
     * Test of removeAllRecipients method, of class com.funambol.mail.Message.
     */
    public void testremoveAllRecipients() throws Exception {
        Log.debug("getNumberOfRecipients");
        m = createSampleMessage();
        m.removeAllRecipients(Address.TO);
        m.removeAllRecipients(Address.CC);
        m.removeAllRecipients(Address.BCC);
        assertEquals(0, m.getNumberOfRecipients());
    }

    /**
     * Test of reloadMessage method, of class com.funambol.mail.Message.
     */
    public void testReloadMessage() throws Exception, Exception {
        Log.debug("Reload Message");
        m = createSampleMessage();
        Message expected = m;
        Store store = createMsgStore();
        store.getFolder(Store.INBOX).appendMessage(m);
        m.reloadMessage();
        
        assertEquals(expected.getMessageId(), m.getMessageId());
        
        removeMsgStore(store);
    }
    
    
    
    private void testgetHeader() throws Exception {
        Log.debug("Get Header");
        m = createSampleMessage();
        
        boolean from = m.getHeader(m.FROM).equals("from@funambol.com"); 
        boolean to = m.getHeader(m.TO).equals("to@funambol.com"); 
        boolean cc = m.getHeader(m.CC).equals("cc@funambol.com");
        boolean bcc = m.getHeader(m.BCC).equals("bcc@funambol.com");
        boolean replyTo = m.getHeader(m.REPLYTO).equals("replyTo@funambol.com");
        
        assertTrue(from&&to&&cc&&bcc&&replyTo);
        
    }
    
    
    private void testgetAllHeaders() throws Exception {
        Log.debug("Get All Headers");
        m = createSampleMessage();
        String expectedHeaders = 
                "Cc: cc@funambol.com" +
                "To: to@funambol.com" +
                "From: from@funambol.com" +
                "Reply-To: replyTo@funambol.com" +
                "Bcc: bcc@funambol.com";

        boolean result = true;
        String[] headers = m.getAllHeaders();
        StringBuffer s = new StringBuffer();
        if (headers!=null) {
            for(int i=0; i<headers.length; i++) {
                if (expectedHeaders.indexOf(headers[i])<0) {
                    result = false;
                    break;
                }
            }    
        } else {
            Log.debug("Headers are null");
        }
        
        assertTrue(result);
    }
    
    private void testaddHeader() throws Exception {
        Log.debug("Add Header");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        
        //fix return null with return "" and EqualsIgnoreCase when retrieving
        //name
        assertEquals(m.getHeader("Dummy"), "dummy");
    }
    
    private void testsetHeader() throws Exception {
        Log.debug("Set Header");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        m.setHeader("Dummy", "changed dummy");
        assertEquals(m.getHeader("Dummy"), "changed dummy");
    }
    
    private void testremoveHeader() throws Exception {
        Log.debug("Remove Header");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        m.removeHeader("dummy");
        assertTrue(m.getHeader("dummy")==null);
    }
    
    private void testContentType() throws Exception {
        Log.debug("Content Type");
        m = createSampleMessage();
        m.setContentType("dummy/type");
        assertEquals(m.getContentType(), "dummy/type");
    }

    private void testisMultiPart() throws Exception {
        Log.debug("Is Multipart");
        m = createSampleMessage();
        //fix isMultipart method - with startsWith("multipart");
        m.setContentType(m.MULTIPART + "/");
        assertTrue(m.isMultipart());
    }

    private void testisText() throws Exception {
        Log.debug("Is Text");
        m = createSampleMessage();
        m.setContentType("text/dummy");
        assertTrue(m.isText());
    }

    private void testisTextPlain() throws Exception {
        Log.debug("Is Text Plain");
        m = createSampleMessage();
        m.setContentType(m.TEXT_PLAIN);
        assertTrue(m.isTextPlain());
    }

    private void testisTextHtml() throws Exception {
        Log.debug("Is Text Html");
        m = createSampleMessage();
        m.setContentType("text/html");
        assertTrue(m.isTextHtml());
    }
    
    private void testserialize() throws IOException, Exception {
        Log.debug("Test Serialize");
        
        m = createSampleMessage();
        
        //FIX: when serializing the message the hashtable is nullified
        //String[] elements = m.getAllHeaders();
        Hashtable headers = m.headers;
        MessageFlags mf = m.getFlags(); 
        
        String content = (String) m.getContent();
        int composedMsgLength = 10;
        m.setContentType(m.TEXT_PLAIN);

        m.setComposedMessageLength(composedMsgLength);
        //Serialize original message
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        
        m.setSentDate(new Date(0));
        m.setReceivedDate(new Date(0));
        
        m.serialize(dout);
        
        //Creates expected result
        ByteArrayOutputStream expectedBaos = new ByteArrayOutputStream();
        DataOutputStream expectedDout = new DataOutputStream(expectedBaos);
        
        expectedDout.writeUTF(m.getMessageId());
        
        mf.serialize(expectedDout);
        
        ComplexSerializer.serializeHashTable(expectedDout, headers);
        
        expectedDout.writeUTF(m.TEXT_PLAIN);
        
        expectedDout.writeLong(0);
        
        expectedDout.writeLong(0);

        ComplexSerializer.serializeObject(expectedDout, content);

        expectedDout.writeInt(composedMsgLength);
        
        assertTrue(isStreamEqual(expectedBaos, baos));
    }

    private void testdeserialize() throws Exception {
        Log.debug("Test Deserialize");
        
        m = createSampleMessage();
        m.setGlobalLaziness(Message.NO_LAZY);
        //FIX: when serializing the message the hashtable is nullified
        //String[] elements = m.getAllHeaders();
        String[] headers = m.getAllHeaders();
        
        MessageFlags mf = m.getFlags(); 
        
        String msgID = m.getMessageId();
        
        String content = (String) m.getContent();

        int composedMsgLength = 10;

        m.setContentType(m.TEXT_PLAIN);

        m.setComposedMessageLength(composedMsgLength);
        //Serialize original message
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        
        m.setSentDate(new Date(0));
        m.setReceivedDate(new Date(0));
        
        
        Log.debug("Serializing original");
        m.serialize(dout);
        
        Log.debug("OutputStream");
        ByteArrayInputStream bais = new ByteArrayInputStream(baos.toByteArray());
        Log.debug("Data stream");
        DataInputStream dis = new DataInputStream(bais);
        
        Log.debug("Expected");
        Message expected = new Message();
        Log.debug("Deserialization");
        expected.deserialize(dis);
        
        boolean result = true;
        
        Log.debug("MsgID");
        result = expected.getMessageId().equals(msgID);
        if (!result) {
            fail("MsgID Failed");
        }
        
        Log.debug("Flags");
        result = expected.getFlags().getFlags()==mf.getFlags();
        if (!result) {
            fail("Flags Failed");
        }
        
        Log.debug("Headers");
        
        Vector headerVector = new Vector();
        String[] newHeaders = expected.getAllHeaders();
        for (int i=0; i<newHeaders.length; i++) {
            headerVector.addElement(newHeaders[i]);
            Log.debug((String) headerVector.elementAt(i));
        }
        
        for (int i=0; i<headers.length; i++) {
            Log.debug(headers[i]);
            result = headerVector.contains(headers[i]);
            if (!result) {
                break;
            }
        }
        if (!result) {
            fail("Headers Failed");
        }
        
        Log.debug("Content Type");
        result = expected.getContentType().equals(m.TEXT_PLAIN);
        if (!result) {
            fail("Content Type Failed");
        }
        
        Log.debug("Sent");
        result = expected.getSentDate().getTime()==0;
        if (!result) {
            fail("Sent Date Failed");
        }
        
        Log.debug("Received");
        result = expected.getReceivedDate().getTime()==0;
        if (!result) {
            fail("Received Date Failed");
        }
        
        Log.debug("Content");
        result = expected.getContent().equals(content);
        if (!result) {
            fail("Content Failed");
        }
        assertTrue(result);

        Log.debug("composedMessageLength");
        result = expected.getComposedMessageLength()==composedMsgLength;
        if(!result){
            fail("composedMessageLength Failed");
        }
    }
    
    private void testSetGetComposedMessageLength() throws IOException, Exception {
        Log.debug("Test SetGetComposedMessageLength");
        
        m = createSampleMessage();
        
        //FIX: when serializing the message the hashtable is nullified
        //String[] elements = m.getAllHeaders();
        Hashtable headers = m.headers;
        MessageFlags mf = m.getFlags(); 
        
        String content = (String) m.getContent();
        int composedMsgLength = 10;
        m.setContentType(m.TEXT_PLAIN);

        m.setComposedMessageLength(composedMsgLength);
        //Serialize original message
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        
        m.setSentDate(new Date(0));
        m.setReceivedDate(new Date(0));
        
        m.serialize(dout);

        int result = m.getComposedMessageLength();

        assertEquals(result,composedMsgLength);
    }
    
    private void testGetComposedMessageLength() throws IOException, Exception {
        Log.debug("Test GetComposedMessageLength");
        
        m = createSampleMessage();
        
        //FIX: when serializing the message the hashtable is nullified
        //String[] elements = m.getAllHeaders();
        Hashtable headers = m.headers;
        MessageFlags mf = m.getFlags(); 
        
        String content = (String) m.getContent();
        
        m.setContentType(m.TEXT_PLAIN);

        //Serialize original message
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        
        m.setSentDate(new Date(0));
        m.setReceivedDate(new Date(0));
        
        m.serialize(dout);

        int result = m.getComposedMessageLength();

        assertEquals(result,-1);
    }
    
    /**
     * Compare actual and expected Byte array byte by byte.
     * @param baos is the ByteArrayOutputStream obtained from serialization
     * @param expectedBaos is the expected ByteArrayOutputStream
     * @return byte[] result for next test.
     */
    private boolean isStreamEqual(ByteArrayOutputStream baos, 
                               ByteArrayOutputStream expectedBaos) 
                               throws Exception {
        boolean ret = false;
        byte[] actual = expectedBaos.toByteArray();
        byte[] expected = baos.toByteArray();
        for (int j=0; j<actual.length; j++) {
            ret = actual[j]==expected[j];
            if (!ret) {
                break;
            }
        }
        //Return the byteArray to use for deserialization test 
        return ret;
    }

    
    private void removeMsgStore(Store store) throws MailException {
        Folder[] f = store.list();
        
        for (int i=0; i<f.length; i++) {
            store.removeFolder(f[i].getName());
        }
        
        store=null;
    }

    private Store createMsgStore() throws MailException {
        Store store = StoreFactory.getStore();
        store.init(false);
        return store;
    }

    private Message createSampleMessage() {
        Address from = new Address(Address.FROM, "from@funambol.com"); 
        Address[] to = new Address[1];
        to[0] = new Address(Address.TO, "to@funambol.com");
        Address[] cc = new Address[1];
        cc[0] = new Address(Address.CC, "cc@funambol.com");
        Address[] bcc = new Address[1];
        bcc[0] = new Address(Address.BCC, "bcc@funambol.com");
        Address[] replyTo = new Address[1];
        replyTo[0] = new Address(Address.REPLYTO, "replyTo@funambol.com");
        
        m.setFrom(from);
        m.setTo(to);
        m.setCc(cc);
        m.setBcc(bcc);
        m.setReplyTo(replyTo);
        
        String content = "generic content";
        m.setContent(content);
        
        return m;
    }

    public MessageTest() {
        super(40,"MessageTest");
        Log.setLogLevel(Log.DEBUG);
    }

    public void setUp() {
        m = new Message();
    }

    public void tearDown() {
        m=null;
    }

    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0:testGlobalLaziness();break;
            case 1:testLaziness();break;
            case 2:testRecipients();break;
            case 3:testRecipient();break;
            case 4:testKey();break;
            case 5:testRecordId();break;
            case 6:testMessageId();break;
            case 7:testGetFlags();break;
            case 8:testSetFlags();break;
            case 9:testcreateUniqueMessageIDValue();break;
            case 10:testFrom();break;
            case 11:testTo();break;
            case 12:testCc();break;
            case 13:testBcc();break;
            case 14:testReplyTo();break;
            case 15:testSubject();break;
            case 16:testSentDate();break;
            case 17:testSentTime();break;
            case 18:testReceivedDate();break;
            case 19:testReceivedTime();break;
            case 20:testParent();break;
            case 21:testContent();break;
            case 22:testgetTextContent();break;
            case 23:testgetNumberOfRecipients();break;
            case 24:testremoveAllRecipients();break;
            case 25:testReloadMessage();;break;
            
            //Test for Part class inherited method
            case 26:testgetHeader();break;
            case 27:testgetAllHeaders();break;
            case 28:testaddHeader();break;
            case 29:testsetHeader();break;
            case 30:testremoveHeader();break;
            case 31:testContentType();break;
            case 32:testisMultiPart();break;
            case 33:testisText();break;
            case 34:testisTextPlain();break;
            case 35:testisTextHtml();break;
            
            //Serialization tests
            case 36:testserialize();break;
            case 37:testdeserialize();break;

            
            case 38:testSetGetComposedMessageLength();break;
            case 39:testGetComposedMessageLength();break;
            
                    
            default: break;
        }
    }
}
