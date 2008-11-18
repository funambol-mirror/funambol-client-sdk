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
public class RMSStoreTest extends TestCase {

    private RMSStore store = null;
    private Folder currentFolder = null;
    private Message message = null;
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
    public RMSStoreTest() {
        super(17, "FolderTest");
        
        
        //Sets log level to debug
        Log.setLogLevel(Log.DEBUG);
        
        //Reset the environment
        Log.debug("Resetting environment...");
        store = (RMSStore) StoreFactory.getStore();
        Log.debug("System ready to be tested");
    }

    
    public void setUp() {
        store = (RMSStore) StoreFactory.getStore();
    }
    
    public void tearDown() {
        store=null;
    }
    
    public void test(int testNumber) throws Throwable {
        //updates the test number in order to correctly manage the setUp method
        test = testNumber;
        switch(testNumber) {
            //Creates a dummy folder
            case 0: 
                testCreateFolder();
                break;
            //Removes the dummy folder
            case 1: 
                testRemoveFolder();
                break;
            //initialize the store without resetting
            case 2: 
                testInit();
                break;
            //initialize the store with resetting option
            case 3: 
                testInitDoReset();    
                break;
            //List folders
            case 4: 
                testList();             
                break;
            //Compares a new reference to Inbox folder with an existing one
            case 5:
                testGetFolder();
                break;
            //Finds inbox and outbox folders
            case 6:
                testfindFolders();
                break;
            case 7:
                testaddMessage();
                break;
            case 8:
                testsaveMessage();
                break;
            case 9:
                testremoveMessage();
                break;
            case 10:
                testcountMessages();
                break;
            case 11:
                testgetMessageIDs();
                break;
            case 12:
                testgetMsgHeaders();
                break;
            case 13:
                testreadMessageByPath();
                break;
            case 14:
                testreadMessageByStream();
                break;
            case 15:
                testreadFirstMessage();
                break;
            case 16:
                testreadNextMessage();
                break;
            default:                        
                break;
        }
    }
    
    /**
     * Invoke the createFolder Method in order to create a dummy folder
     */
    private void testCreateFolder() throws AssertionFailedException {
        Log.debug("testCreateFolder");
        //Creates a dummy folder
        store.createFolder("Dummy");
        
        //checks the content invoking the proper method 
        //with boolean realStore set to false
        boolean result = checkRecordStoreContent(false);
        
        assertTrue(result);
        Log.debug("testCreateFolder OK");
        
    }
    
    /**
     * Invoke the removeFolder Method in order to remove a dummy folder
     */
    private void testRemoveFolder() throws AssertionFailedException {
        Log.debug("testRemoveFolder");
        //Creates a dummy folder
        store.createFolder("Dummy");
        store.removeFolder("/Dummy");
        String[] rs = RecordStore.listRecordStores();
        
        boolean result = true;
        
        //rs is null if no recordStore were found invoking 
        //String[] rs = RecordStore.listRecordStores();
        if (rs!=null) {
            for (int i=0; i<rs.length; i++) {
                result = !rs[i].equals("/Dummy");
                if (!result) {
                    break;
                }
            }
        }
        
        assertTrue(result);
    }
    
    /**
     * Invoke the init method the first time: boolean reset is set to false
     * @throws AssertionFailedException when the test fails
     */
    private void testInit() throws AssertionFailedException {
        Log.debug("testInit");
        //Invoke with param reset set to false 
        store.init(false);
        
        //checks the content invoking the proper method 
        //with boolean realStore set to true
        boolean testResult = checkRecordStoreContent(true);
        assertTrue(testResult);
    }

    /**
     * Invoke the init method the first time: boolean reset is set to false
     * @throws AssertionFailedException when the test fails
     */
    private void testInitDoReset() throws AssertionFailedException {
        Log.debug("testInitDoReset");
        //Invoke with param reset set to false 
        store.init(true);
        
        //checks the content invoking the proper method 
        //with boolean realStore set to true
        boolean testResult = checkRecordStoreContent(true);
        assertTrue(testResult);
    }

    
    /**
     * List folders currently in the store
     * @throws AssertionFailedException when the test fails
     */
    public void testList() throws AssertionFailedException {
        Log.debug("testList");
        boolean result = true;
        store.init(true);
        
        Folder[] rootFolders = store.list();
        
        String folderList[] = {
            rootFolders[0].getFullName(), 
            rootFolders[1].getFullName(), 
            rootFolders[2].getFullName(), 
            rootFolders[3].getFullName(), 
        };
        
        Vector expectedList = new Vector(4);
        
        expectedList.addElement("/" + Store.INBOX); 
        expectedList.addElement("/" + Store.OUTBOX); 
        expectedList.addElement("/" + Store.SENT); 
        expectedList.addElement("/" + Store.DRAFTS); 
        
        if (rootFolders.length==4) {
            for (int i=0; i<rootFolders.length; i++) {
                result = expectedList.contains(rootFolders[i].getFullName());
                if (!result) {
                    break;
                }
            }
        }
        
        assertTrue(result);
    }

    /**
     * Test the getFolder method onthe Inbox folder
     */
    private void testGetFolder() throws AssertionFailedException {
        Log.debug("getFolders");
        store.init(true);
        Folder expected = new Folder("/Inbox", store);
        Folder result = store.getFolder("/Inbox");
        boolean getResult = (expected.getName().equals(result.getName()))
                        &&(expected.getStore().equals(result.getStore()));
        assertTrue(getResult);
    }

    /**
     * Search for Inbox and Outbox folder passing the substring "box"
     */
    private void testfindFolders() throws AssertionFailedException {
        Log.debug("findFolders");
        store.init(true);
        Folder[] result = store.findFolders("box");
        Vector expected = new Vector(2);
        expected.addElement(new String("Inbox"));
        expected.addElement(new String("Outbox"));
        
        Log.debug(result[0].getName());
        Log.debug(result[1].getName());
        
        boolean getResult = 
                    expected.contains(result[0].getName())
                    && expected.contains(result[1].getName());
        assertTrue(getResult);
    }

    /**
     * Add message to the store: retrieves it by folder and test the addition
     */
    private void testaddMessage() throws AssertionFailedException {
        Log.debug("addMessage");
        store.init(true);
        
        Message[] m = createMultipleNoLazyMessages(1);
        
        store.addMessage("/Outbox", m[0]);
        
        Message result = store.getFolder("/Outbox").getMessage("1");
        Log.debug(m[0].getMessageId());
        Log.debug(result.getMessageId());
        
        assertTrue(result.getMessageId().equals(m[0].getMessageId()));
    }

    /**
     * Add a message, updates the same message and store it to the Drafts folder
     * Retrieves the correctly updated message
     */
    private void testsaveMessage() throws AssertionFailedException {
        Log.debug("saveMessage");
        store.init(true);
        
        Message[] m = createMultipleNoLazyMessages(1);
        
        store.addMessage("/Drafts", m[0]);
        
        Message toSave = store.getFolder("/Drafts").getMessage("1");
        toSave.setSubject("Subject Changed");
        
        store.saveMessage("/Drafts", toSave);
        
        Message result = store.getFolder("/Drafts").getMessage("1");
        
        Log.debug(m[0].getMessageId());
        Log.debug(result.getMessageId());
        Log.debug(result.getSubject());
        
        assertTrue(result.getMessageId().equals(m[0].getMessageId())
                              && result.getSubject().equals("Subject Changed"));
    }
    
    /**
     * Add 1 message to the Inbox folder, removes it and check that the
     * messages number is 0
     */
    private void testremoveMessage() throws AssertionFailedException {
        Log.debug("removeMessages");
        store.init(true);
        
        Message[] m = createMultipleNoLazyMessages(1);
        
        store.addMessage("/Inbox", m[0]);
        store.removeMessage("/Inbox", "1");
        
        int count = store.getFolder("/Inbox").getMessageCount();
        
        assertEquals(0, count);
    }
    
    /**
     * Put 10 new messages in the folder Inbox and counts them
     */
    private void testcountMessages() throws AssertionFailedException {
        Log.debug("countMessage");
        store.init(true);
        
        Message[] m = createMultipleNoLazyMessages(10);
        
        for (int i=0; i<10; i++) {
            store.addMessage("/Inbox", m[i]);
        }
        
        int count = store.getFolder("/Inbox").getMessageCount();
        
        assertEquals(10, count);
    }
    
    /**
     * Read all the IDs of INBOX messages
     */
    private void testgetMessageIDs() throws AssertionFailedException {
        Log.debug("testGetMessageIDs");
        
        int msgNumber = 3;
        store.init(true);
        
        Vector expectedIDs = new Vector();
        Message[] m = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<msgNumber; i++) {
            store.addMessage("/Inbox", m[i]);
            expectedIDs.addElement(new String(m[i].getMessageId()));
            Log.debug(m[i].getMessageId());
            Log.debug((String) expectedIDs.elementAt(i));
            
        }
        
        String[] id = store.getMessageIDs("/Inbox");
        
        boolean result = true;
        
        for (int i=0; i<id.length; i++) {
            Log.debug(m[i].getMessageId());
            Log.debug((String) expectedIDs.elementAt(i));
            result = m[i].getMessageId().equals((String) expectedIDs.elementAt(i));
            if (!result) {
                break;
            }
        }
        
        assertTrue(result);
    }

    /**
     * Read all Headers of INBOX messages
     */
    private void testgetMsgHeaders() throws AssertionFailedException {
        Log.debug("testGetMessageHeaders");
        
        store.init(true);
        
        Message m = createNoLazyMessage();
        
        store.addMessage("/Inbox", m);
        
        //fix cheange method name getMsgHeaders
        Message[] messages = store.getMsgHeaders("/Inbox");
        
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
    private void testreadMessageByPath() throws IOException, AssertionFailedException {
        Log.debug("Read Message");
        store.init(true);
        Message m = createNoLazyMessage();
        store.addMessage("/Inbox", m);
        
        Message storedMessage = store.readMessage("/Inbox", "1");
        
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
    public void testreadMessageByStream() throws AssertionFailedException, IOException {
        Log.debug("Read message with stream");
        store.init(true);
        Message m = createNoLazyMessage();
        store.addMessage("/Inbox", m);
        
        m.setParent(store.getFolder("/Inbox"));
        
        DataInputStream dis = store.readMessage(m);
        
        Message storedMessage = new Message();
        
        storedMessage.deserialize(dis);
        
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
     * invokes the method to have the first unsorted message present in the 
     * inbox. This method must be always called before perform a call to 
     * "readNextMessage".
     */
    private void testreadFirstMessage() throws AssertionFailedException {
        Log.debug("Read First Message");
        store.init(true);
        int msgNumber = 3;
        Message[] m = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<msgNumber; i++) {
            store.addMessage("/Inbox", m[i]);
        }
        
        //test if message record number = 1
        
        Message storedFirstMessage = store.readFirstMessage("/Inbox");
        
        assertTrue(storedFirstMessage.getRecordId()>0);
    }

    /**
     * Read the first available message in the inbox folder 
     */
    private void testreadNextMessage() throws AssertionFailedException {
        Log.debug("Read Next Message");
        store.init(true);
        int msgNumber = 3;
        Message[] m = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<msgNumber; i++) {
            store.addMessage("/Inbox", m[i]);
        }
        
        Message[] storedMessage = new Message[3];
        
        storedMessage[0] = store.readFirstMessage("/Inbox");
        storedMessage[1] = store.readNextMessage("/Inbox");
        storedMessage[2] = store.readNextMessage("/Inbox");
        
        //test if message record numbers are correct
        boolean result = true;
        for(int i=0; i<msgNumber; i++) {
            result = storedMessage[i].getRecordId()>0;
            if (!result) {
                break;
            }
        }
        
        assertTrue(result);
    }

    /**
     * creates a message with the NO_LAZY GlobalLaziness. 
     * @return Message the created message
     * @throws MailException
     */
    private Message createNoLazyMessage() throws MailException {
        Message m = new Message();
        
        m.setGlobalLaziness(Message.NO_LAZY);
        
        Address[] toList = new Address[1];
        Address[] ccList = new Address[1];
        Address[] bccList = new Address[1];
        
        toList[0] = new Address(Address.TO, "to@funambol.com");
        ccList[0] = new Address(Address.CC, "cc@funambol.com");
        bccList[0] = new Address(Address.BCC, "bcc@funambol.com");
        
        
        m.setFrom(new Address(Address.FROM, "from@funambol.com"));
        m.setTo(toList);
        m.setCc(ccList);
        m.setBcc(bccList);

        m.setSubject(new String("Message Subject"));
        m.setContent(new String("Message Content"));
        m.setMessageId("ID0");
        
        return m;
    }
    
    /**
     * creates a message array with the NO_LAZY GlobalLaziness. 
     * @return Message[] the created message
     * @throws MailException
     */
    private Message[] createMultipleNoLazyMessages(int number) {
        Message[] m = new Message[number];
        
        Address[] toList = new Address[1];
        Address[] ccList = new Address[1];
        Address[] bccList = new Address[1];
        
        toList[0] = new Address(Address.TO, "address@funambol.com");
        ccList[0] = new Address(Address.CC, "address@funambol.com");
        bccList[0] = new Address(Address.BCC, "address@funambol.com");
        
        for (int i=0; i<m.length; i++) {
            m[i] = new Message();
            m[i].setGlobalLaziness(Message.NO_LAZY);
            m[i].setFrom(new Address(Address.FROM, "from@funambol.com"));
            m[i].setTo(toList);
            m[i].setCc(ccList);
            m[i].setBcc(bccList);
        
            m[i].setSubject(new String("Message Subject " + i));
            m[i].setContent(new String("Message Content " + i));
            m[i].setMessageId("ID"+i);
            
        }
        
        return m;
    }

    /**
     * checks the Record store content in order to test the folder creation
     * methods of RMSStore class:
     * @param realStore true means we're running test 3, otherwise 1
     */
    private boolean checkRecordStoreContent(boolean realStore) 
                                               throws AssertionFailedException {
        //Checks for dummy or real RMSStore
        //Search for recordtores
        String[] rsNames = RecordStore.listRecordStores();
        
        //Put Recordstores' names into a vector
        Vector rsFound = new Vector();
        for (int i=0; i<rsNames.length; i++) {
            Log.debug(rsNames[i]);
            rsFound.addElement(rsNames[i]);
        }
        
        boolean testResult = false;
        
        Log.debug("Size is correct: testing content...");
        if (realStore) {
            testResult = rsFound.contains("/" + Store.INBOX)&&
                rsFound.contains("/" + Store.OUTBOX)&&
                rsFound.contains("/" + Store.SENT)&&
                rsFound.contains("/" + Store.DRAFTS);    
        } else {
            testResult = rsFound.contains("/Dummy");
        }
        
        if (testResult) {
            Log.debug("Folders correctly created");
        }
        
        return testResult;
    }

}

