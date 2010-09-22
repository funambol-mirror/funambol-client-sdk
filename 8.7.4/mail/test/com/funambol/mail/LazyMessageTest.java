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

import junit.framework.*;
import java.util.Date;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

/**
 * A class to test operations related to messages and folders creation/deletion
 */
public class LazyMessageTest extends TestCase {

    private static final String INBOX = "/Inbox";
    private static final String OUTBOX = "/Outbox";
    private static final String DRAFTS = "/Drafts";

    private Folder currentFolder = null;
    private Store store = null;
    private Message m = null;

    /**
     * Unit test public constructor
     */
    public LazyMessageTest(String name) {
        super(name);
        
        //Sets log level to debug
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);

        store = StoreFactory.getStore();
    }
    
    public void setUp() {
        store = StoreFactory.getStore();
        store.init(true);
        store.addFolder(new Folder(INBOX, "Inbox", new Date(), store));
        store.addFolder(new Folder(OUTBOX, "Outbox", new Date(), store));
        store.addFolder(new Folder(DRAFTS, "Drafts", new Date(), store));
    }
    
    public void tearDown() {
        store = null;
        m = null;
    }
    
    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    public void testaddLazyHeadersMessage() throws Exception {
        Log.info("LazyMessageTest: testaddLazyHeadersMessage");
        
        Folder f = store.getFolder(OUTBOX);
        m = createMessage(f, Message.LAZY_HEADERS);
        
        store.addChild(OUTBOX, m);
        
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
        Log.info("LazyMessageTest: testaddLazyHeadersMessage successful");
    }

    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    public void testaddLazyContentMessage() throws Exception {
        Log.info("LazyMessageTest: testaddLazyContentMessage");
        
        m = createMessage(store.getFolder(OUTBOX), Message.LAZY_CONTENT);
        
        store.addChild(OUTBOX, m);
    
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
        Log.info("LazyMessageTest: testaddLazyContentMessage successful");
    }

    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    public void testaddFullLazyMessage() throws Exception {
        Log.info("LazyMessageTest: testaddFullLazyMessage");
        
        m = createMessage(store.getFolder(OUTBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        store.addChild(OUTBOX, m);
    
        Message result = store.getFolder(OUTBOX).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId()));
        Log.info("LazyMessageTest: testaddFullLazyMessage successful");
    }

    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    public void testsaveLazyHeadersMessage() throws Exception {
        Log.info("LazyMessageTest: testsaveLazyHeadersMessage");
        
        Folder f = store.getFolder(DRAFTS);
        
        m = createMessage(f, Message.LAZY_HEADERS);
        
        f.appendMessage(m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        toSave.setSubject("Subject Changed");
        
        store.updateChild(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
        Log.info("LazyMessageTest: testsaveLazyHeadersMessage successful");
    }
    
    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    public void testsaveLazyContentMessage() throws Exception {
        Log.info("LazyMessageTest: testsaveLazyContentMessage");
        
        m = createMessage(store.getFolder(DRAFTS), Message.LAZY_CONTENT);
        
        store.addChild(DRAFTS, m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        toSave.setParent(store.getFolder(DRAFTS));
        toSave.setSubject("Subject Changed");
        
        store.updateChild(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
        Log.info("LazyMessageTest: testsaveLazyContentMessage successful");
    }
    
    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    public void testsaveFullLazyMessage() throws Exception {
        Log.info("LazyMessageTest: testsaveFullLazyMessage");
        
        m = createMessage(store.getFolder(DRAFTS), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addChild(DRAFTS, m);
        
        Message toSave = store.getFolder(DRAFTS).getMessage("1");
        
        toSave.setParent(store.getFolder(DRAFTS));
        toSave.setSubject("Subject Changed");
        
        store.updateChild(DRAFTS, toSave);
        
        Message result = store.getFolder(DRAFTS).getMessage("1");
        
        assertTrue(result.getMessageId().equals(m.getMessageId())
                              && result.getSubject().equals("Subject Changed"));
        Log.info("LazyMessageTest: testsaveFullLazyMessage successful");
    }
    
    /**
     * Read all Headers of INBOX messages
     */
    public void testgetLazyHeadersMsgHeaders() throws Exception {
        Log.info("LazyMessageTest: testgetLazyHeadersMsgHeaders");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addChild(INBOX, m);

        ChildrenEnumeration children = store.getFolder(INBOX).getChildren();
        Message msg = (Message)children.nextElement();
        msg.setParent(store.getFolder(INBOX));
        
        String emailFrom = msg.getFrom().getEmail();        
        String emailTo = msg.getTo()[0].getEmail();        
        String emailCc = msg.getCc()[0].getEmail();        
        String emailBcc = msg.getBcc()[0].getEmail();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        Log.info("LazyMessageTest: testgetLazyHeadersMsgHeaders successful");
    }

    /**
     * Read all Headers of INBOX messages
     */
    public void testgetLazyContentMsgHeaders() throws Exception {
        Log.info("LazyMessageTest: testgetLazyContentMsgHeaders");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        ChildrenEnumeration children = store.getFolder(INBOX).getChildren();
        Message msg = (Message)children.nextElement();
        msg.setParent(store.getFolder(INBOX));

        String emailFrom = msg.getFrom().getEmail();
        String emailTo = msg.getTo()[0].getEmail();
        String emailCc = msg.getCc()[0].getEmail();
        String emailBcc = msg.getBcc()[0].getEmail();
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        Log.info("LazyMessageTest: testgetLazyContentMsgHeaders successful");
    }

    /**
     * Read all Headers of INBOX messages
     */
    public void testgetFullLazyMsgHeaders() throws Exception {
        Log.info("LazyMessageTest: testgetFullLazyMsgHeaders");
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        ChildrenEnumeration children = store.getFolder(INBOX).getChildren();
        Message msg = (Message)children.nextElement();
        msg.setParent(store.getFolder(INBOX));
        
        String emailFrom = msg.getFrom().getEmail();
        String emailTo = msg.getTo()[0].getEmail();
        String emailCc = msg.getCc()[0].getEmail();
        String emailBcc = msg.getBcc()[0].getEmail();     
        
        assertTrue("from@funambol.com".equals(emailFrom)
                   && "to@funambol.com".equals(emailTo)
                   && "cc@funambol.com".equals(emailCc)
                   && "bcc@funambol.com".equals(emailBcc)
                   );
        Log.info("LazyMessageTest: testgetFullLazyMsgHeaders successful");
    }

    
    
    /**
     * Read a message passing the path of the inbox folder
     */
    public void testreadLazyHeadersByMessageByPath() throws Exception {
        Log.info("LazyMessageTest: testreadLazyHeadersByMessageByPath");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = (Message)store.readChild(INBOX, "1");
        
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
        Log.info("LazyMessageTest: testreadLazyHeadersByMessageByPath successful");
    }
    
    /**
     * Read a message passing the path of the inbox folder
     */
    public void testreadLazyContentByMessageByPath() throws Exception {
        Log.info("LazyMessageTest: testreadLazyContentByMessageByPath");

        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = (Message)store.readChild(INBOX, "1");
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
        Log.info("LazyMessageTest: testreadLazyContentByMessageByPath successful");
    }
    
    /**
     * Read a message passing the path of the inbox folder
     */
    public void testreadFullLazyByMessageByPath() throws Exception {
        Log.info("LazyMessageTest: testreadFullLazyByMessageByPath");
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        Message storedMessage = (Message)store.readChild(INBOX, "1");
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
        Log.info("LazyMessageTest: testreadFullLazyByMessageByPath successful");
    }
    
    /**
     * Read a message passing the message reference 
     */
    public void testreadLazyHeadersMessageByStream() throws Exception {
        Log.info("LazyMessageTest: testreadLazyHeadersMessageByStream");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_HEADERS);
        
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readChildBytes(m.getParent().getFullName(),
                Integer.toString(m.getRecordId()));
        
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
        Log.info("LazyMessageTest: testreadLazyHeadersMessageByStream successful");
    }

    
    /**
     * Read a message passing the message reference 
     */
    public void testreadLazyContentMessageByStream() throws Exception {
        Log.info("LazyMessageTest: testreadLazyContentMessageByStream");
        
        m = createMessage(store.getFolder(INBOX), Message.LAZY_CONTENT);
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readChildBytes(m.getParent().getFullName(),
                Integer.toString(m.getRecordId()));
        
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
        Log.info("LazyMessageTest: testreadLazyContentMessageByStream successful");
    }

    /**
     * Read a message passing the message reference 
     */
    public void testreadFullLazyMessageByStream() throws Exception {
        Log.info("LazyMessageTest: testreadFullLazyMessageByStream");
        
        m = createMessage(store.getFolder(INBOX), 
                                     Message.LAZY_HEADERS|Message.LAZY_CONTENT);
        store.addChild(INBOX, m);
        
        m.setParent(store.getFolder(INBOX));
        
        DataInputStream dis = store.readChildBytes(m.getParent().getFullName(),
                Integer.toString(m.getRecordId()));
        
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
        Log.info("LazyMessageTest: testreadFullLazyMessageByStream successful");
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
}

