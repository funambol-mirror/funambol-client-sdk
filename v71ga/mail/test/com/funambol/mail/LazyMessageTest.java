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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import javax.microedition.lcdui.List;

import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreNotFoundException;

import jmunit.framework.cldc10.TestCase;
import jmunit.framework.cldc10.TestSuite;

import javax.microedition.rms.RecordStoreException;
import jmunit.framework.cldc10.AssertionFailedException;

import com.funambol.util.Log;

/**
 * A class to test operations related to messages and folders creation/deletion
 */
public class LazyMessageTest extends TestCase {

    private static final String INBOX = "/Inbox";
    private static final String OUTBOX = "/Outbox";
    private static final String DRAFTS = "/Drafts";
    
    private RMSStore store = null;
    private Folder currentFolder = null;
    private Message m = null;
    private int version;
    
    //Keeps trace of the test number; used in setUp method to 
    private int test;

    //If the user doesn't exit the system before running the test twice
    //The recordStore must be cleared
    private boolean repeatTest=false;
    
    private int testNumber = 0;
    
    /**
     * Unit test public constructor
     */
    public LazyMessageTest() {
        super(15, "FolderTest");
        
        
        //Sets log level to debug
        Log.setLogLevel(Log.DEBUG);
        
        //Reset the environment
        Log.debug("Resetting environment...");
        store = (RMSStore) StoreFactory.getStore();
        Log.debug("System ready to be tested");
    }

    
    public void setUp() {
        store = (RMSStore) StoreFactory.getStore();
        store.init(true);
    }
    
    public void tearDown() {
        store=null;
        m=null;
    }
    
    public void test(int testNumber) throws Throwable {
        //updates the test number in order to correctly manage the setUp method
        test = testNumber;
        switch(testNumber) {
            case 0:
                testaddLazyHeadersMessage();
                break;
            case 1:
                testaddLazyContentMessage();
                break;
            case 2:
                testaddFullLazyMessage();
                break;
            case 3:
                testsaveLazyHeadersMessage();
                break;
            case 4:
                testsaveLazyContentMessage();
                break;
            case 5:
                testsaveFullLazyMessage();
                break;
            case 6:
                testgetLazyHeadersMsgHeaders();
                break;
            case 7:
                testgetLazyContentMsgHeaders();
                break;
            case 8:
                testgetFullLazyMsgHeaders();
                break;
            case 9:
                testreadLazyHeadersByMessageByPath();
                break;
            case 10:
                testreadLazyContentByMessageByPath();
                break;
            case 11:
                testreadFullLazyByMessageByPath();
                break;
            case 12:
                testreadLazyHeadersMessageByStream();
                break;
            case 13:
                testreadLazyContentMessageByStream();
                break;
            case 14:
                testreadFullLazyMessageByStream();
                break;
            default:                        
                break;
        }
    }
    
    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    private void testaddLazyHeadersMessage() throws AssertionFailedException {
        Log.debug("***addLazyHeadersMessage***");
        
        Folder f = store.getFolder(OUTBOX);
        m = createMessage(f, Message.LAZY_HEADERS);
        
        store.addMessage(OUTBOX, m);
        
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
    }

    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    private void testaddLazyContentMessage() throws AssertionFailedException {
        Log.debug("***addLazyContentMessage***");
        
        m = createMessage(store.getFolder(OUTBOX), Message.LAZY_CONTENT);
        
        store.addMessage(OUTBOX, m);
    
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        Log.debug(m.getMessageId());
        Log.debug(result.getMessageId());
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
    }

    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    private void testaddFullLazyMessage() throws AssertionFailedException {
        Log.debug("***addFullLazyMessage***");
        
        m = createMessage(store.getFolder(OUTBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addMessage(OUTBOX, m);
    
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        Log.debug(m.getMessageId());
        Log.debug(result.getMessageId());
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
    }

    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    private void testsaveLazyHeadersMessage() throws AssertionFailedException {
        Log.debug("saveLazyHeadersMessage");
        
        Folder f = store.getFolder(DRAFTS);
        
        m = createMessage(f, Message.LAZY_HEADERS);
        
        f.appendMessage(m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        
        toSave.setSubject("Subject Changed");
        
        store.saveMessage(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
    }
    
    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    private void testsaveLazyContentMessage() throws AssertionFailedException {
        Log.debug("saveLazyContentMessage");
        
        m = createMessage(store.getFolder(DRAFTS), Message.LAZY_CONTENT);
        
        store.addMessage(DRAFTS, m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        toSave.setParent(store.getFolder(DRAFTS));
        
        toSave.setSubject("Subject Changed");
        
        store.saveMessage(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
    }
    
    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    private void testsaveFullLazyMessage() throws AssertionFailedException {
        Log.debug("saveFullLazyMessage");
        
        m = createMessage(store.getFolder(DRAFTS), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addMessage(DRAFTS, m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        toSave.setParent(store.getFolder(DRAFTS));
        
        toSave.setSubject("Subject Changed");
        
        store.saveMessage(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
    }
    
    /**
     * Read all Headers of INBOX messages
     */
    private void testgetLazyHeadersMsgHeaders() throws AssertionFailedException {
        Log.debug("testGetLazyHeadersMessageHeaders");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addMessage(INBOX, m);
        
        Message[] messages = store.getMsgHeaders(INBOX);
        
        messages[0].setParent(store.getFolder(INBOX));
        
        String emailFrom = messages[0].getFrom().getEmail();
        
        String emailTo = messages[0].getTo()[0].getEmail();
        
        String emailCc = messages[0].getCc()[0].getEmail();
        
        String emailBcc = messages[0].getBcc()[0].getEmail();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        
    }

    /**
     * Read all Headers of INBOX messages
     */
    private void testgetLazyContentMsgHeaders() throws AssertionFailedException {
        Log.debug("testGetLazyContentMessageHeaders");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        //fix cheange method name getMsgHeaders
        Message[] messages = store.getMsgHeaders(INBOX);
        
        messages[0].setParent(store.getFolder(INBOX));
        
        String emailFrom = messages[0].getFrom().getEmail();
        String emailTo = messages[0].getTo()[0].getEmail();
        String emailCc = messages[0].getCc()[0].getEmail();
        String emailBcc = messages[0].getBcc()[0].getEmail();
        
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        
    }

    /**
     * Read all Headers of INBOX messages
     */
    private void testgetFullLazyMsgHeaders() throws AssertionFailedException {
        Log.debug("testGetFullLazyMessageHeaders");
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        //fix cheange method name getMsgHeaders
        Message[] messages = store.getMsgHeaders(INBOX);
        
        messages[0].setParent(store.getFolder(INBOX));
        
        String emailFrom = messages[0].getFrom().getEmail();
        String emailTo = messages[0].getTo()[0].getEmail();
        String emailCc = messages[0].getCc()[0].getEmail();
        String emailBcc = messages[0].getBcc()[0].getEmail();
        
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        
    }

    
    
    /**
     * Read a message passing the path of the inbox folder
     */
    private void testreadLazyHeadersByMessageByPath() throws IOException, AssertionFailedException {
        Log.debug("Read lazy headers message by path");
        
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = store.readMessage(INBOX, "1");
        
        storedMessage.setParent(store.getFolder(INBOX));
        
        String emailFrom = storedMessage.getFrom().getEmail();
        String emailTo = storedMessage.getTo()[0].getEmail();
        String emailCc = storedMessage.getCc()[0].getEmail();
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        String subject = storedMessage.getSubject();
        String content = (String) storedMessage.getContent();
        String msgId = storedMessage.getMessageId();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }
    
    /**
     * Read a message passing the path of the inbox folder
     */
    private void testreadLazyContentByMessageByPath() throws IOException, AssertionFailedException {
        Log.debug("Read Lazy Content Message by path");
        
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = store.readMessage(INBOX, "1");
        storedMessage.setParent(store.getFolder(INBOX));
        
        String emailFrom = storedMessage.getFrom().getEmail();
        String emailTo = storedMessage.getTo()[0].getEmail();
        String emailCc = storedMessage.getCc()[0].getEmail();
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        String subject = storedMessage.getSubject();
        String content = (String) storedMessage.getContent();
        String msgId = storedMessage.getMessageId();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }
    
    /**
     * Read a message passing the path of the inbox folder
     */
    private void testreadFullLazyByMessageByPath() throws IOException, AssertionFailedException {
        Log.debug("Read Full Lazy Message by path");
        
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = store.readMessage(INBOX, "1");
        storedMessage.setParent(store.getFolder(INBOX));
        
        String emailFrom = storedMessage.getFrom().getEmail();
        String emailTo = storedMessage.getTo()[0].getEmail();
        String emailCc = storedMessage.getCc()[0].getEmail();
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        String subject = storedMessage.getSubject();
        String content = (String) storedMessage.getContent();
        String msgId = storedMessage.getMessageId();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }
    
    /**
     * Read a message passing the message reference 
     */
    public void testreadLazyHeadersMessageByStream() throws AssertionFailedException, IOException {
        Log.debug("Read lazy headers message with stream");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readMessage(m);
        
        Message storedMessage = new Message();
        
        storedMessage.setParent(store.getFolder(INBOX));
        
        storedMessage.deserialize(dis);
        
        storedMessage.setRecordId(1);
        
        String emailFrom = storedMessage.getFrom().getEmail();
        
        String emailTo = storedMessage.getTo()[0].getEmail();
        
        String emailCc = storedMessage.getCc()[0].getEmail();
        
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        
        String subject = storedMessage.getSubject();
        
        String content = (String) storedMessage.getContent();
        
        String msgId = storedMessage.getMessageId();
        
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }

    
    /**
     * Read a message passing the message reference 
     */
    public void testreadLazyContentMessageByStream() throws AssertionFailedException, IOException {
        Log.debug("Read lazy content message with stream");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readMessage(m);
        
        Message storedMessage = new Message();
        
        storedMessage.setParent(store.getFolder(INBOX));
        
        storedMessage.deserialize(dis);
        
        storedMessage.setRecordId(1);
        
        String emailFrom = storedMessage.getFrom().getEmail();
        String emailTo = storedMessage.getTo()[0].getEmail();
        String emailCc = storedMessage.getCc()[0].getEmail();
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        String subject = storedMessage.getSubject();
        String content = (String) storedMessage.getContent();
        String msgId = storedMessage.getMessageId();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }

    /**
     * Read a message passing the message reference 
     */
    public void testreadFullLazyMessageByStream() throws AssertionFailedException, IOException {
        Log.debug("Read full lazy message with stream");
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        store.addMessage(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readMessage(m);
        
        Message storedMessage = new Message();
        
        storedMessage.setParent(store.getFolder(INBOX));
        
        storedMessage.deserialize(dis);
        
        storedMessage.setRecordId(1);
        
        String emailFrom = storedMessage.getFrom().getEmail();
        String emailTo = storedMessage.getTo()[0].getEmail();
        String emailCc = storedMessage.getCc()[0].getEmail();
        String emailBcc = storedMessage.getBcc()[0].getEmail();
        String subject = storedMessage.getSubject();
        String content = (String) storedMessage.getContent();
        String msgId = storedMessage.getMessageId();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   && "Message Subject".equals(subject)
                   && "Message Content".equals(content)
                   && "ID0".equals(msgId)
                   );
        
    }

    
    /**
     * creates a message with the given GlobalLaziness. 
     * @return Message the created message
     * @throws MailException
     */
    private Message createMessage(Folder f, int laziness) throws MailException {
        
        Message message = new Message();
        
        message.setLaziness(laziness);
        
        Address[] toList = new Address[1];
        Address[] ccList = new Address[1];
        Address[] bccList = new Address[1];
        
        toList[0] = new Address(Address.TO, "to@funambol.com");
        ccList[0] = new Address(Address.CC, "cc@funambol.com");
        bccList[0] = new Address(Address.BCC, "bcc@funambol.com");
        
        message.setFrom(new Address(Address.FROM, "from@funambol.com"));
        message.setTo(toList);
        message.setCc(ccList);
        message.setBcc(bccList);
        
        message.setSubject(new String("Message Subject"));
        message.setContent(new String("Message Content"));
        message.setMessageId("ID0");
        
        return message;
    }
    
    /**
     * creates a message array with the NO_LAZY GlobalLaziness. 
     * Useful to add additonal tests to this class
     * @return Message[] the created message
     * @throws MailException
     */
    private Message[] createMultipleLazyMessages(int number, Folder f, int laziness) {
        Message[] messages = new Message[number];
        
        Address[] toList = new Address[1];
        Address[] ccList = new Address[1];
        Address[] bccList = new Address[1];
        
        toList[0] = new Address(Address.TO, "address@funambol.com");
        ccList[0] = new Address(Address.CC, "address@funambol.com");
        bccList[0] = new Address(Address.BCC, "address@funambol.com");
        
        for (int i=0; i<messages.length; i++) {
            messages[i] = new Message();
            messages[i].setParent(f);
            messages[i].setLaziness(laziness);
            messages[i].setFrom(new Address(Address.FROM, "from@funambol.com"));
            messages[i].setTo(toList);
            messages[i].setCc(ccList);
            messages[i].setBcc(bccList);
        
            messages[i].setSubject(new String("Message Subject " + i));
            messages[i].setContent(new String("Message Content " + i));
            messages[i].setMessageId("ID"+i);
            
        }
        
        return messages;
    }
}

