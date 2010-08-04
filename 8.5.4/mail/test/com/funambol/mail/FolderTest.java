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
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;
import junit.framework.*;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.util.*;

public class FolderTest extends TestCase {

    private Store store = null;

    private Folder folder = null;

    private final String FOLDER_FULLNAME = "/Folder";

    public FolderTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
    }

    public void setUp() {
        store = StoreFactory.getStore();
        // Create root folder
        store.init(true);
        store.addFolder(new Folder(FOLDER_FULLNAME, "inbox", new Date(), store));
        folder = store.getFolder(FOLDER_FULLNAME);
    }

    public void tearDown() {
        store.removeFolder(store.getFolder(FOLDER_FULLNAME), true);
        store = null;
        folder = null;
    }

    /**
     * Test of getRole method, of class com.funambol.mail.Folder.
     */
    public void testgetRole() throws Exception {
        Log.info("FolderTest: testgetRole");
        assertTrue(folder.getRole().equals("inbox"));
        Log.info("FolderTest: testgetRole successful");
    }
    
    /**
     * Test of getRole method, of class com.funambol.mail.Folder in the case the contained role is null.
     */
    public void testgetRole_null() throws Exception {
        Log.info("FolderTest: testgetRole_null");
        
        store.addFolder(new Folder("/Inbox", "fake_role", new Date(), store));
        folder = store.getFolder("/Inbox", true);
        assertTrue(folder.getRole().equals("inbox"));
        
        store.addFolder(new Folder("/Outbox", "fake_role", new Date(), store));
        folder = store.getFolder("/Outbox", true);
        assertTrue(folder.getRole().equals("outbox"));
        
        store.addFolder(new Folder("/Drafts", "fake_role", new Date(), store));
        folder = store.getFolder("/Drafts", true);
        assertTrue(folder.getRole().equals("drafts"));
        
        store.addFolder(new Folder("/Sent", "fake_role", new Date(), store));
        folder = store.getFolder("/Sent", true);
        assertTrue(folder.getRole().equals("sent"));
        
        Log.info("FolderTest: testgetRole_null successful");
    }
    
    /**
     * Test of appendMessage method, of class com.funambol.mail.Folder.
     */
    public void testappendMessage() throws Exception {
        Log.info("FolderTest: testappendMessage");
        Message m = createNoLazyMessage();
        
        folder.appendMessage(m);
        
        Message storedMessage = (Message)store.readChild(FOLDER_FULLNAME, "1");
        
        assertTrue(compareMessages(storedMessage, m));
        Log.info("FolderTest: testappendMessage successful");
    }

    /**
     * Test of updateMessage method, of class com.funambol.mail.Folder.
     */
    public void testupdateMessage() throws Exception {
        Log.info("FolderTest: testupdateMessage");
        String newSubject = "Another Subject";
        Message m = createNoLazyMessage();
        folder.appendMessage(m);
        
        m.setSubject(newSubject);
        
        folder.updateMessage(m);
        
        Message storedMessage = (Message)store.readChild(FOLDER_FULLNAME, "1");
        
        assertTrue(storedMessage.getMessageId().equals(m.getMessageId())
                     && storedMessage.getSubject().equals(newSubject)
                     );
        Log.info("FolderTest: testupdateMessage successful");
    }

    /**
     * Test of getMessage method, of class com.funambol.mail.Folder.
     */
    public void testgetMessage() throws Exception {
        Log.info("FolderTest: testgetMessage");
        Message m = createNoLazyMessage();
        folder.appendMessage(m);
        
        Message result = folder.getMessage("1");
        
        assertTrue(compareMessages(m, result));
        Log.info("FolderTest: testgetMessage successful");
    }

    /**
     * Test of getChildren method, of class com.funambol.mail.Folder.
     *
     * All the folder children are Message items
     */
    public void testgetChildren_Message() throws Exception {
        Log.info("FolderTest: testgetChildren_Message");
        
        int msgNumber = 20;
        
        Message[] messages = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<messages.length; i++) {
            folder.appendMessage(messages[i]);
        }
        
        ChildrenEnumeration children = folder.getChildren();
        boolean found = false;
        int count = 0;
        
        while(children.hasMoreElements()) {
            
            found = false;
            count++;
            
            Object item = children.nextElement();
            assertTrue(item instanceof Message);
            
            Message msg = (Message)item;
            for (int i=0; i<messages.length; i++) {
                if(compareMessages(messages[i], msg)) {
                    found = true;
                }
            }
            assertTrue(found);
        }
        assertEquals(count, msgNumber);
        try { children.close(); } catch(Exception ex) { assertTrue(false); }
        
        Log.info("FolderTest: testgetChildren_Message successful");
    }
    
    /**
     * Test of getChildren method, of class com.funambol.mail.Folder.
     *
     * All the folder children are Folder items
     */
    public void testgetChildren_Folder() throws Exception {
        Log.info("FolderTest: testgetChildren_Folder");
        
        Folder[] subfolders = new Folder[4];
        subfolders[0] = new Folder(FOLDER_FULLNAME + "/Inbox", "inbox", new Date(), store);
        subfolders[1] = new Folder(FOLDER_FULLNAME + "/Outbox", "outbox", new Date(), store);
        subfolders[2] = new Folder(FOLDER_FULLNAME + "/Drafts", "drafts", new Date(), store);
        subfolders[3] = new Folder(FOLDER_FULLNAME + "/Sent", "sent", new Date(), store);
        
        store.addFolder(subfolders[0]);
        store.addFolder(subfolders[1]);
        store.addFolder(subfolders[2]);
        store.addFolder(subfolders[3]);

        ChildrenEnumeration children = folder.getChildren();
        boolean found = false;
        int count = 0;
        
        while(children.hasMoreElements()) {
            
            found = false;
            count++;
            
            Object item = children.nextElement();
            assertTrue(item instanceof Folder);
            
            Folder folder = (Folder)item;
            for (int i=0; i<subfolders.length; i++) {
                if(subfolders[i].getFullName().equals(folder.getFullName())) {
                    found = true;
                }
            }
            assertTrue(found);
        }
        assertEquals(count, 4);
        try { children.close(); } catch(Exception ex) { assertTrue(false); }
        
        Log.info("FolderTest: testgetChildren_Folder successful");
    }

    /**
     * Test of deleteMessage method, of class com.funambol.mail.Folder.
     */
    public void testdeleteMessageByRecordID() throws Exception {
        Log.info("FolderTest: testdeleteMessageByRecordID");
        int msgNumber = 10;
        Message[] messages = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<messages.length; i++) {
            folder.appendMessage(messages[i]);
        }
        
        folder.deleteMessage("1");
        
        assertEquals(msgNumber-1, folder.getMessageCount());

        ChildrenEnumeration children = folder.getChildren();
        int count = 0;
        while(children.hasMoreElements()) {
            Message msg = (Message)children.nextElement();
            assertTrue(msg.getRecordId() != 1);
            count++;
        }
        assertEquals(count, msgNumber-1);
        try { children.close(); } catch(Exception ex) { assertTrue(false); }
        
        Log.info("FolderTest: testdeleteMessageByRecordID successful");
    }

    /**
     * Test of deleteMessage method, of class com.funambol.mail.Folder.
     */
    public void testdeleteMessageByMessage() throws Exception {
        Log.info("FolderTest: testdeleteMessageByMessage");
        int msgNumber = 10;
        Message[] messages = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<messages.length; i++) {
            folder.appendMessage(messages[i]);
        }
        
        int msgId = messages[0].getRecordId();
        
        folder.deleteMessage(messages[0]) ;
        
        assertEquals(msgNumber-1, folder.getMessageCount());
        
        ChildrenEnumeration children = folder.getChildren();
        int count = 0;
        while(children.hasMoreElements()) {
            Message msg = (Message)children.nextElement();
            assertTrue(msg.getRecordId() != msgId);
            count++;
        }
        assertEquals(count, msgNumber-1);
        try { children.close(); } catch(Exception ex) { assertTrue(false); }
        
        Log.info("FolderTest: testdeleteMessageByMessage successful");
    }

    /**
     * Test of getMessageCount method, of class com.funambol.mail.Folder.
     */
    public void testgetMessageCount() throws Exception {
        Log.info("FolderTest: testgetMessageCount");
        int msgNumber = 3;
        Message[] messages = createMultipleNoLazyMessages(msgNumber);
        
        for (int i=0; i<messages.length; i++) {
            folder.appendMessage(messages[i]);
        }
        
        assertEquals(msgNumber, folder.getMessageCount());
        Log.info("FolderTest: testgetMessageCount successful");
    }

    /**
     * Test of getParent method, of class com.funambol.mail.Folder.
     */
    public void testgetParent() throws Exception {
        Log.info("FolderTest: testgetParent");
        store.addFolder(new Folder(FOLDER_FULLNAME + "/newFolder", "inbox", new Date(), store));
        Folder fnew = store.getFolder(FOLDER_FULLNAME + "/newFolder");
        
        assertEquals(FOLDER_FULLNAME, fnew.getParent().getFullName());
        store.removeFolder(store.getFolder(FOLDER_FULLNAME + "/newFolder"), false);
        Log.info("FolderTest: testgetParent successful");
    }

    /**
     * Test of getSeparator method, of class com.funambol.mail.Folder.
     */
    public void testgetSeparator() throws Exception {
        Log.info("FolderTest: testgetSeparator");
        assertEquals('/', folder.getSeparator());
        Log.info("FolderTest: testgetSeparator successful");
    }

    /**
     * Test of list method, of class com.funambol.mail.Folder.
     */
    public void testlist() throws Exception {
        Log.info("FolderTest: testlist");

        store.addFolder(new Folder(FOLDER_FULLNAME + "/newFolder1", "inbox", new Date(), store));
        store.addFolder(new Folder(FOLDER_FULLNAME + "/newFolder2", "inbox", new Date(), store));
        Folder[] flist = folder.list();

        assertEquals(FOLDER_FULLNAME + "/newFolder2", flist[0].getFullName());
        assertEquals(FOLDER_FULLNAME + "/newFolder1", flist[1].getFullName());
        
        Log.info("FolderTest: testlist successful");
    }

    /**
     * Test of getFullName method, of class com.funambol.mail.Folder.
     */
    public void testgetFullName() throws Exception {
        Log.info("FolderTest: testgetFullName");
        assertEquals(FOLDER_FULLNAME, folder.getFullName());
        Log.info("FolderTest: testgetFullName successful");
    }

    /**
     * Test of getName method, of class com.funambol.mail.Folder.
     */
    public void testgetName() throws Exception {
        Log.info("FolderTest: testgetName");
        assertEquals(FOLDER_FULLNAME.substring(1), folder.getName());
        Log.info("FolderTest: testgetName successful");
    }

    /**
     * Test of serialize method, of class com.funambol.mail.Folder.
     */
    public void testserialize() throws Exception { 

        Log.info("FolderTest: testserialize");
        
        Folder folder = new Folder("/Parent/Children", "inbox", new Date(1240907915), store);
        store.addFolder(new Folder("/Parent", "inbox", new Date(), store));

        // test attributes
        assertTrue(folder.getName().equals("Children"));
        assertTrue(folder.getRole().equals("inbox"));
        assertTrue(folder.getCreated().getTime() == 1240907915);
        assertTrue(folder.getFullName().equals("/Parent/Children"));

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        folder.serialize(dout);

        //Creates expected result
        ByteArrayOutputStream expectedBaos = new ByteArrayOutputStream();
        DataOutputStream expectedDout = new DataOutputStream(expectedBaos);

        expectedDout.writeChar(Folder.FOLDER_ITEM_PREFIX);
        expectedDout.writeUTF(folder.getFullName());
        expectedDout.writeUTF(folder.getRole());
        expectedDout.writeLong(folder.getCreated().getTime());

        assertTrue(isStreamEqual(baos, expectedBaos));
        Log.info("FolderTest: testserialize successful");
    }

    /**
     * Test of deserialize method, of class com.funambol.mail.Folder.
     */
    public void testdeserialize() throws Exception {

        Log.info("FolderTest: testdeserialize");

        Folder folder = new Folder("/Parent/Children", "inbox", new Date(1240907915), store);
        store.addFolder(new Folder("/Parent", "inbox", new Date(), store));

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        folder.serialize(dout);

        ByteArrayInputStream bais = new ByteArrayInputStream(baos.toByteArray());
        DataInputStream dis = new DataInputStream(bais);
        Folder expFolder = new Folder(null, null, null, null);

        // The first byte is not deserialized by the Folder itself
        dis.readChar();
        expFolder.deserialize(dis);

        assertTrue(folder.getName().equals(expFolder.getName()));
        assertTrue(folder.getRole().equals(expFolder.getRole()));
        assertTrue(folder.getCreated().equals(expFolder.getCreated()));
        assertTrue(folder.getFullName().equals(expFolder.getFullName()));
        Log.info("FolderTest: testdeserialize successful");
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

    /**
     * Compare actual and expected Byte array byte by byte.
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
        return ret;
    }
}
