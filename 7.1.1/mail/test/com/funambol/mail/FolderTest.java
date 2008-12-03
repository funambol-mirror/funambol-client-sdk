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

import com.funambol.mail.*;
import com.funambol.util.Log;
import jmunit.framework.cldc10.*;

public class FolderTest extends TestCase {

    private Store store = null;

    private Folder f = null;

    private static final String FOLDER = "/Folder";
    /**
     * Test of appendMessage method, of class com.funambol.mail.Folder.
     */
    public void testappendMessage() throws AssertionFailedException, Exception {
        Log.debug("Append Message");
        Message m = createNoLazyMessage();
        
        f.appendMessage(m);
        
        //Fix comments: not msgid but record id
        Message storedMessage = store.readMessage(FOLDER, "1");
        
        assertTrue(compareMessages(storedMessage, m));
    }

    /**
     * Test of updateMessage method, of class com.funambol.mail.Folder.
     */
    public void testupdateMessage() throws AssertionFailedException, Exception {
        Log.debug("Update Message");
        String newSubject = "Another Subject";
        Message m = createNoLazyMessage();
        f.appendMessage(m);
        
        m.setSubject(newSubject);
        
        f.updateMessage(m);
        
        Message storedMessage = store.readMessage(FOLDER, "1");
        
        assertTrue(storedMessage.getMessageId().equals(m.getMessageId())
                     && storedMessage.getSubject().equals(newSubject)
                     );
    }

    /**
     * Test of getMessage method, of class com.funambol.mail.Folder.
     */
    public void testgetMessage() throws AssertionFailedException, Exception {
        Log.debug("Get Message");
        Message m = createNoLazyMessage();
        f.appendMessage(m);
        
        Message result = f.getMessage("1");
        
        assertTrue(compareMessages(m, result));
        
    }

    /**
     * Test of getMsgHeaders method, of class com.funambol.mail.Folder.
     */
    public void testgetMsgHeaders() throws AssertionFailedException, Exception {
        Log.debug("Get Message Headers");
        Message m = createNoLazyMessage();
        f.appendMessage(m);
        
        Message[] result = f.getMsgHeaders();
        
        assertTrue(compareMsgHeaders(m, result[0]));
    }

    /**
     * Test of getFirstMessage method, of class com.funambol.mail.Folder.
     */
    public void testgetFirstMessage() throws AssertionFailedException, Exception {
        Log.debug("Get First Message");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        Message[] resultMessages = new Message[3];
        resultMessages[0] = f.getFirstMessage();
        
        assertTrue(resultMessages[0].getRecordId()>0);
    }

    /**
     * Test of getNextMessage method, of class com.funambol.mail.Folder.
     */
    public void testgetNextMessage() throws AssertionFailedException, Exception {
        Log.debug("Get Next Message");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        boolean result = true;
        Message[] resultMessages = new Message[3];
        for (int i=0; i<resultMessages.length; i++) {
            Log.debug("" + i);
            resultMessages[i] = f.getMessage(String.valueOf(i+1));
            Log.debug("" + i);
            
            if (resultMessages[i].getRecordId()<0) {
                result = false;
                break;
            }
        }
        
        assertTrue(result);
    }

    /**
     * Test of deleteMessage method, of class com.funambol.mail.Folder.
     */
    public void testdeleteMessageByRecordID() throws AssertionFailedException, Exception {
        Log.debug("Delete Message by Record number");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        f.deleteMessage("1");
        
        assertEquals(msgNumber-1, f.getMessageCount());
    }

    /**
     * Test of deleteMessage method, of class com.funambol.mail.Folder.
     */
    public void testdeleteMessageByMessage() throws AssertionFailedException, Exception {
        Log.debug("Delete Message by Message reference");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        f.deleteMessage(messages[0]) ;
        
        assertEquals(msgNumber-1, f.getMessageCount());
    }

    /**
     * Test of deleteMessage method, of class com.funambol.mail.Folder.
     */
    public void testdeleteMessageByIndex() throws AssertionFailedException, Exception {
        Log.debug("Delete Message by Index");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        f.deleteMessage("1");
        
        assertEquals(msgNumber-1, f.getMessageCount());
    }

    /**
     * Test of getMessageCount method, of class com.funambol.mail.Folder.
     */
    public void testgetMessageCount() throws AssertionFailedException, Exception {
        Log.debug("Get Messages count");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(3);
        
        for (int i=0; i<messages.length; i++) {
            f.appendMessage(messages[i]);
        }
        
        assertEquals(msgNumber, f.getMessageCount());
    }

    /**
     * Test of getParent method, of class com.funambol.mail.Folder.
     */
    public void testgetParent() throws AssertionFailedException, Exception {
        Log.debug("Get Parent");
        store.createFolder(FOLDER + "/newFolder");
        Folder fnew = store.getFolder(FOLDER + "/newFolder");
        
        Log.debug(fnew.getParent().getFullName());
        assertEquals(FOLDER, fnew.getParent().getFullName());
        store.removeFolder(FOLDER + "/newFolder");
    }

    /**
     * Test of getSeparator method, of class com.funambol.mail.Folder.
     */
    public void testgetSeparator() throws AssertionFailedException {
        Log.debug("Get Separator");
        assertEquals('/', f.getSeparator());
    }

    /**
     * Test of list method, of class com.funambol.mail.Folder.
     */
    public void testlist() throws AssertionFailedException, Exception {
        Log.debug("test list");
        
        Folder[] flist = f.list();
        
        Log.debug(flist[0].getFullName());
        assertEquals(FOLDER, flist[0].getFullName());
    }

    /**
     * Test of getStore method, of class com.funambol.mail.Folder.
     */
    public void testgetStore() throws AssertionFailedException {
        Log.debug("Get Store");
        Store store1 = f.getStore();
        assertEquals(store, store1);
    }

    /**
     * Test of getFullName method, of class com.funambol.mail.Folder.
     */
    public void testgetFullName() throws AssertionFailedException {
        Log.debug("Get Full Name");
        assertEquals(FOLDER, f.getFullName());
    }

    /**
     * Test of getName method, of class com.funambol.mail.Folder.
     */
    public void testgetName() throws AssertionFailedException {
        Log.debug("Get Name");
        assertEquals(FOLDER.substring(1), f.getName());
    }

    /**
     * Test of getFolder method, of class com.funambol.mail.Folder.
     */
    public void testgetFolder() throws AssertionFailedException, Exception {
        Log.debug("Get Folder");
        
        store.createFolder(FOLDER + "/newFolder");
        //fix the method name: returns subfolders
        Folder f1 = f.getFolder("newFolder");
        
        Log.debug(f1.getFullName());
        assertEquals(FOLDER + "/newFolder", f1.getFullName());
        store.removeFolder(FOLDER + "/newFolder");
    }

    public FolderTest() {
        super(17,"FolderTest");
        Log.setLogLevel(Log.DEBUG);
    }

    public void setUp() {
        store = StoreFactory.getStore();
        store.createFolder(FOLDER);
        f = store.getFolder(FOLDER);
    }

    public void tearDown() {
        store.removeFolder(FOLDER);
        store = null;
        f = null;
    }

    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0:testappendMessage();break;
            case 1:testupdateMessage();break;
            case 2:testgetMessage();break;
            case 3:testgetMsgHeaders();break;
            case 4:testgetFirstMessage();break;
            case 5:testgetNextMessage();break;
            case 6:testdeleteMessageByIndex();break;
            case 7:testdeleteMessageByMessage();break;
            case 8:testdeleteMessageByRecordID();break;
            case 9:testgetMessageCount();break;
            case 10:testgetParent();break;
            case 11:testgetSeparator();break;
            case 12:testlist();break;
            case 13:testgetStore();break;
            case 14:testgetFullName();break;
            case 15:testgetName();break;
            case 16:testgetFolder();break;
            default: break;
        }
    }
    
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

    private boolean compareMessages(Message m1, Message m2) {
        return (m1.getMessageId().equals(m2.getMessageId())
                && m1.getFrom().getEmail().equals(m2.getFrom().getEmail())
                && m1.getTo()[0].getEmail().equals(m2.getTo()[0].getEmail())
                && m1.getCc()[0].getEmail().equals(m2.getCc()[0].getEmail())
                && m1.getBcc()[0].getEmail().equals(m2.getBcc()[0].getEmail())
                && m1.getSubject().equals(m2.getSubject())
                && m1.getContent().equals(m2.getContent())
                && m1.getFlags().getFlags()==m2.getFlags().getFlags()
                );
    }

    private boolean compareMsgHeaders(Message m1, Message m2) {
        return (
                m1.getFrom().getEmail().equals(m2.getFrom().getEmail())
                && m1.getTo()[0].getEmail().equals(m2.getTo()[0].getEmail())
                && m1.getCc()[0].getEmail().equals(m2.getCc()[0].getEmail())
                && m1.getBcc()[0].getEmail().equals(m2.getBcc()[0].getEmail())
                );
    }
}
