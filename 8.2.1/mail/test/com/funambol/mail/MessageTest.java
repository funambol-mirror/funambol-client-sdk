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

import com.funambol.storage.ComplexSerializer;
import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.StringUtil;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;

import junit.framework.*;

public class MessageTest extends TestCase {

    private Message m = null;
    
    public MessageTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
    }

    public void setUp() {
        m = new Message();
    }

    public void tearDown() {
        m=null;
    }

    public void testGlobalLaziness() throws Exception {
        Log.info("MessageTest: testGlobalLaziness");
        int globalLaziness = Message.LAZY_CONTENT;
        m.setGlobalLaziness(globalLaziness);
        
        //Calculate global laziness value as the method does
        int expectedResult = globalLaziness|=m.getGlobalLaziness();
        
        assertEquals(expectedResult, m.getGlobalLaziness());
        Log.info("MessageTest: testGlobalLaziness successful");
    }

    /**
     * Test of setLaziness method, of class com.funambol.mail.Message.
     */
    public void testLaziness() throws Exception {
        Log.info("MessageTest: testLaziness");
        int laziness = Message.LAZY_CONTENT;
        m.setLaziness(laziness);
        
        //Calculate global laziness value as the method does
        int expectedResult = laziness|=m.getLaziness();
        
        assertEquals(expectedResult, m.getLaziness());
        Log.info("MessageTest: testLaziness successful");
    }

    /**
     * Test of addRecipients method, of class com.funambol.mail.Message.
     */
    public void testRecipients() throws Exception, Exception {
        Log.info("MessageTest: testRecipients");
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
        Log.info("MessageTest: testRecipients successful");
    }

    /**
     * Test of addRecipient method, of class com.funambol.mail.Message.
     */
    public void testRecipient() throws Exception, Exception {
        Log.info("MessageTest: testRecipient");
        Address address = new Address(Address.CC, "address@funambol.com");
        m.addRecipient(address);
        
        boolean realCc = m.getCc()[0].getEmail().equals("address@funambol.com");
        //TODO review the generated test code and remove the default call to fail.
        assertTrue(realCc);
        Log.info("MessageTest: testRecipient successful");
    }

    /**
     * Test of setKey method, of class com.funambol.mail.Message.
     */
    public void testKey() throws Exception {
        Log.info("MessageTest: testKey");
        String key = "Key1";
        m.setKey(key);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(key, m.getKey());
        Log.info("MessageTest: testKey successful");
    }

    
    /**
     * Test of setRecordId method, of class com.funambol.mail.Message.
     */
    public void testRecordId() throws Exception {
        Log.info("MessageTest: testRecordId");
        int record = 1;
        m.setRecordId(record);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(record, m.getRecordId());
        Log.info("MessageTest: testRecordId successful");
    }

    
    /**
     * Test of getMessageId method, of class com.funambol.mail.Message.
     */
    public void testMessageId() throws Exception {
        Log.info("MessageTest: testMessageId");
        String id = "1";
        m.setMessageId(id);
        
        //TODO review the generated test code and remove the default call to fail.
        assertEquals(id, m.getMessageId());
        Log.info("MessageTest: testMessageId successful");
    }
    

    /**
     * Test of getFlags method, of class com.funambol.mail.Message.
     */
    public void testGetFlags() throws Exception {
        Log.info("MessageTest: testGetFlags");
        MessageFlags expectedResult = m.flags;
        
        MessageFlags result = m.getFlags();
        assertEquals(expectedResult, result);
        Log.info("MessageTest: testGetFlags successful");
    }

    /**
     * Test of getFlags method, of class com.funambol.mail.Message.
     */
    public void testSetFlags() throws Exception {
        Log.info("MessageTest: testSetFlags");
        MessageFlags mf = new MessageFlags();
        mf.setFlag(mf.OPENED, true);
        m.setFlags(mf);
        
        assertTrue(mf.isSet(mf.OPENED)==m.getFlags().isSet(mf.OPENED));
        Log.info("MessageTest: testSetFlags successful");
    }

    /**
     * Test of createUniqueMessageIDValue method, of class com.funambol.mail.Message.
     */
    public void testcreateUniqueMessageIDValue() throws Exception {
        Log.info("MessageTest: testcreateUniqueMessageIDValue");
        //Creates 100 different messageIds and tests if they are truly different
        String messagesId[] = new String[100];
        for (int i=0; i<messagesId.length; i++) {
            messagesId[i] = m.createUniqueMessageIDValue();
        }
        
        for(int i=0; i<messagesId.length - 1; ++i) {
            String id1 = messagesId[i];
            for(int j=i+1;j<messagesId.length; ++j) {
                String id2 = messagesId[j];
                assertTrue(!id1.equals(id2));
            }
        }
        Log.info("MessageTest: testcreateUniqueMessageIDValue successful");
    }

    /**
     * Test of getFrom method, of class com.funambol.mail.Message.
     */
    public void testFrom() throws Exception, Exception {
        Log.info("MessageTest: testFrom");
        m.setFrom(new Address(Address.FROM, "address@funambol.com"));
        assertEquals("address@funambol.com", m.getFrom().getEmail());
        Log.info("MessageTest: testFrom successful");
    }

    /**
     * Test of getTo method, of class com.funambol.mail.Message.
     */
    public void testTo() throws Exception, Exception {
        Log.info("MessageTest: testTo");
        Address[] add = new Address[1];
        add[0] = new Address(Address.TO, "address@funambol.com");
        m.setTo(add);
        assertEquals("address@funambol.com", m.getTo()[0].getEmail());
        Log.info("MessageTest: testTo successful");
    }

    /**
     * Test of getCc method, of class com.funambol.mail.Message.
     */
    public void testCc() throws Exception, Exception {
        Log.info("MessageTest: testCc");
        Address[] add = new Address[1];
        add[0] = new Address(Address.CC, "address@funambol.com");
        m.setCc(add);
        assertEquals("address@funambol.com", m.getCc()[0].getEmail());
        Log.info("MessageTest: testCc successful");
    }

    /**
     * Test of getBcc method, of class com.funambol.mail.Message.
     */
    public void testBcc() throws Exception, Exception {
        Log.info("MessageTest: testBcc");
        Address[] add = new Address[1];
        add[0] = new Address(Address.BCC, "address@funambol.com");
        m.setBcc(add);
        assertEquals("address@funambol.com", m.getBcc()[0].getEmail());
        Log.info("MessageTest: testBcc successful");
    }

    /**
     * Test of getReplyTo method, of class com.funambol.mail.Message.
     */
    public void testReplyTo() throws Exception, Exception {
        Log.info("MessageTest: testReplyTo");
        Address[] add = new Address[1];
        add[0] = new Address(Address.REPLYTO, "address@funambol.com");
        m.setReplyTo(add);
        assertEquals("address@funambol.com", m.getReplyTo()[0].getEmail());
        Log.info("MessageTest: testReplyTo successful");
    }
    
    /**
     * Test of getSubject method, of class com.funambol.mail.Message.
     */
    public void testSubject() throws Exception {
        Log.info("MessageTest: testSubject");
        m.setSubject("Message Subject");
        assertEquals("Message Subject", m.getSubject());
        Log.info("MessageTest: testSubject successful");
    }

    /**
     * Test of getSentDate method, of class com.funambol.mail.Message.
     */
    public void testSentDate() throws Exception {
        Log.info("MessageTest: testSentDate");
        Date d = new Date(System.currentTimeMillis());
        m.setSentDate(d);
        assertEquals(d, m.getSentDate());
        Log.info("MessageTest: testSentDate successful");
    }

    /**
     * Test of getSentTime method, of class com.funambol.mail.Message.
     */
    public void testSentTime() throws Exception {
        Log.info("MessageTest: testSentTime");
        Date d = new Date(System.currentTimeMillis());
        m.setSentDate(d);
        assertTrue(d.getTime() == m.getSentTime());
        Log.info("MessageTest: testSentTime successful");
    }

    /**
     * Test of getReceivedDate method, of class com.funambol.mail.Message.
     */
    public void testReceivedDate() throws Exception {
        Log.info("MessageTest: testReceivedDate");
        Date d = new Date(System.currentTimeMillis());
        m.setReceivedDate(d);
        assertEquals(d, m.getReceivedDate());
        Log.info("MessageTest: testReceivedDate successful");
    }

    /**
     * Test of getReceivedTime method, of class com.funambol.mail.Message.
     */
    public void testReceivedTime() throws Exception {
        Log.info("MessageTest: testReceivedTime");
        Date d = new Date(System.currentTimeMillis());
        m.setReceivedDate(d);
        assertTrue(d.getTime() == m.getReceivedTime());
        Log.info("MessageTest: testReceivedTime successful");
    }

    /**
     * Test of getParent method, of class com.funambol.mail.Message.
     */
    public void testParent() throws Exception {
        Log.info("MessageTest: testParent");
        Store store = createMsgStore();
        store.addFolder(new Folder("/Inbox", "inbox", new Date(), store));
        m.setParent(store.getFolder("/Inbox"));
        assertEquals("Inbox", m.getParent().getName());
        removeMsgStore(store);
        Log.info("MessageTest: testParent successful");
    }

    /**
     * Test of getContent method, of class com.funambol.mail.Message.
     */
    public void testContent() throws Exception {
        Log.info("MessageTest: testContent");
        m = createSampleMessage();
        m.setContent("Content of the message");
        assertEquals("Content of the message", m.getContent());
        Log.info("MessageTest: testContent successful");
    }

    /**
     * Test of getTextContent method, of class com.funambol.mail.Message.
     */
    public void testgetTextContent() throws Exception {
        Log.info("MessageTest: testgetTextContent");
        m = createSampleMessage();
        m.setContent("Content of the message");
        assertEquals("Content of the message", m.getTextContent());
        Log.info("MessageTest: testgetTextContent successful");
    }

    /**
     * Test of getNumberOfRecipients method, of class com.funambol.mail.Message.
     */
    public void testgetNumberOfRecipients() throws Exception, Exception {
        Log.info("MessageTest: testgetNumberOfRecipients");
        m = createSampleMessage();
        assertEquals(3, m.getNumberOfRecipients());
        Log.info("MessageTest: testgetNumberOfRecipients successful");
    }

    /**
     * Test of removeAllRecipients method, of class com.funambol.mail.Message.
     */
    public void testremoveAllRecipients() throws Exception {
        Log.info("MessageTest: testremoveAllRecipients");
        m = createSampleMessage();
        m.removeAllRecipients(Address.TO);
        m.removeAllRecipients(Address.CC);
        m.removeAllRecipients(Address.BCC);
        assertEquals(0, m.getNumberOfRecipients());
        Log.info("MessageTest: testremoveAllRecipients successful");
    }

    /**
     * Test of reloadMessage method, of class com.funambol.mail.Message.
     */
    public void testReloadMessage() throws Exception, Exception {
        Log.info("MessageTest: testReloadMessage");
        m = createSampleMessage();
        Message expected = m;
        Store store = createMsgStore();
        store.addFolder(new Folder("/Inbox", "inbox", new Date(), store));
        store.getFolder("/Inbox").appendMessage(m);
        m.reloadMessage();
        
        assertEquals(expected.getMessageId(), m.getMessageId());
        
        removeMsgStore(store);
        Log.info("MessageTest: testReloadMessage successful");
    }

    public void testgetHeader() throws Exception {

        Log.info("MessageTest: testgetHeader");
        m = createSampleMessage();
        
        boolean from = m.getHeader(m.FROM).equals("from@funambol.com"); 
        boolean to = m.getHeader(m.TO).equals("to@funambol.com"); 
        boolean cc = m.getHeader(m.CC).equals("cc@funambol.com");
        boolean bcc = m.getHeader(m.BCC).equals("bcc@funambol.com");
        boolean replyTo = m.getHeader(m.REPLYTO).equals("replyTo@funambol.com");
        
        assertTrue(from&&to&&cc&&bcc&&replyTo);
        Log.info("MessageTest: testgetHeader successful");
    }
    
    public void testgetAllHeaders() throws Exception {
        Log.info("MessageTest: testgetAllHeaders");

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
        }
        assertTrue(result);
        Log.info("MessageTest: testgetAllHeaders successful");
    }
    
    public void testaddHeader() throws Exception {
        Log.info("MessageTest: testaddHeader");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        //fix return null with return "" and EqualsIgnoreCase when retrieving
        //name
        assertEquals(m.getHeader("Dummy"), "dummy");
        Log.info("MessageTest: testaddHeader successful");
    }
    
    public void testsetHeader() throws Exception {
        Log.info("MessageTest: testsetHeader");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        m.setHeader("Dummy", "changed dummy");
        assertEquals(m.getHeader("Dummy"), "changed dummy");
        Log.info("MessageTest: testsetHeader successful");
    }
    
    public void testremoveHeader() throws Exception {
        Log.info("MessageTest: testremoveHeader");
        m = createSampleMessage();
        m.addHeader("Dummy", "dummy");
        m.removeHeader("dummy");
        assertTrue(m.getHeader("dummy")==null);
        Log.info("MessageTest: testremoveHeader successful");
    }
    
    public void testContentType() throws Exception {
        Log.info("MessageTest: testContentType");
        m = createSampleMessage();
        m.setContentType("dummy/type");
        assertEquals(m.getContentType(), "dummy/type");
        Log.info("MessageTest: testContentType successful");
    }

    public void testisMultiPart() throws Exception {
        Log.info("MessageTest: testisMultiPart");
        m = createSampleMessage();
        //fix isMultipart method - with startsWith("multipart");
        m.setContentType(m.MULTIPART + "/");
        assertTrue(m.isMultipart());
        Log.info("MessageTest: testisMultiPart successful");
    }

    private void testisText() throws Exception {
        Log.info("MessageTest: testisText");
        m = createSampleMessage();
        m.setContentType("text/dummy");
        assertTrue(m.isText());
        Log.info("MessageTest: testisText successful");
    }

    public void testisTextPlain() throws Exception {
        Log.info("MessageTest: testisTextPlain");
        m = createSampleMessage();
        m.setContentType(m.TEXT_PLAIN);
        assertTrue(m.isTextPlain());
        Log.info("MessageTest: testisTextPlain successful");
    }

    public void testisTextHtml() throws Exception {
        Log.info("MessageTest: testisTextHtml");
        m = createSampleMessage();
        m.setContentType("text/html");
        assertTrue(m.isTextHtml());
        Log.info("MessageTest: testisTextHtml successful");
    }
    
    public void testserialize() throws IOException, Exception {
        Log.info("MessageTest: testserialize");
        
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

        expectedDout.writeChar(Message.MESSAGE_ITEM_PREFIX);
        expectedDout.writeUTF(m.getMessageId());        
        mf.serialize(expectedDout);        
        ComplexSerializer.serializeHashTable(expectedDout, headers);        
        expectedDout.writeUTF(m.TEXT_PLAIN);
        expectedDout.writeLong(0);
        expectedDout.writeLong(0);
        ComplexSerializer.serializeObject(expectedDout, content);
        expectedDout.writeInt(composedMsgLength);
        
        assertTrue(isStreamEqual(expectedBaos, baos));
        Log.info("MessageTest: testserialize successful");
    }

    public void testdeserialize() throws Exception {
        Log.info("MessageTest: testdeserialize");
        
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
        
        m.serialize(dout);
        
        ByteArrayInputStream bais = new ByteArrayInputStream(baos.toByteArray());
        DataInputStream dis = new DataInputStream(bais);
        
        Message expected = new Message();

        // The first byte is not deserialized by the Message itself
        dis.readChar();
        
        expected.deserialize(dis);
        
        boolean result = true;
        
        result = expected.getMessageId().equals(msgID);
        if (!result) {
            fail("MsgID Failed");
        }
        
        result = expected.getFlags().getFlags()==mf.getFlags();
        if (!result) {
            fail("Flags Failed");
        }

        Vector headerVector = new Vector();
        String[] newHeaders = expected.getAllHeaders();
        for (int i=0; i<newHeaders.length; i++) {
            headerVector.addElement(newHeaders[i]);
        }
        
        for (int i=0; i<headers.length; i++) {
            result = headerVector.contains(headers[i]);
            if (!result) {
                break;
            }
        }
        if (!result) {
            fail("Headers Failed");
        }
        
        result = expected.getContentType().equals(m.TEXT_PLAIN);
        if (!result) {
            fail("Content Type Failed");
        }
        
        result = expected.getSentDate().getTime()==0;
        if (!result) {
            fail("Sent Date Failed");
        }
        
        result = expected.getReceivedDate().getTime()==0;
        if (!result) {
            fail("Received Date Failed");
        }
        
        result = expected.getContent().equals(content);
        if (!result) {
            fail("Content Failed");
        }
        assertTrue(result);

        result = expected.getComposedMessageLength()==composedMsgLength;
        if(!result){
            fail("composedMessageLength Failed");
        }
        Log.info("MessageTest: testdeserialize successful");
    }
    
    public void testSetGetComposedMessageLength() throws IOException, Exception {
        Log.info("MessageTest: testSetGetComposedMessageLength");
        
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
        Log.info("MessageTest: testSetGetComposedMessageLength successful");
    }
    
    public void testGetComposedMessageLength() throws IOException, Exception {
        Log.info("MessageTest: testGetComposedMessageLength");
        
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
        Log.info("MessageTest: testGetComposedMessageLength successful");
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

    private Store createMsgStore() throws MailException {
        Store store = StoreFactory.getStore();
        store.init(true);
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

    private void removeMsgStore(Store store) throws MailException {
        Folder[] f = store.list();
        for (int i=0; i<f.length; i++) {
            store.removeFolder(store.getFolder(f[i].getFullName()), true);
        }
        store=null;
    }
}
