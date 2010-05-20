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

import com.funambol.storage.RmsRecordItem;
import com.funambol.util.ConsoleAppender;

import java.io.DataInputStream;
import java.io.IOException;

import junit.framework.*;

import com.funambol.util.Log;
import java.util.Date;
import java.util.Vector;

/**
 * A class to test operations related to messages and folders creation/deletion
 */
public class RMSStoreTest extends TestCase {

    private static final String FOLDER_PATH = "/Folder";
    private static final String FOLDER_ROLE = "folder";
    private static final Date FOLDER_CREATION = new Date();
    private static final String[] DEFAULT_ACCOUNT_FOLDERS = {"/inbox", "/outbox", "/sent", "/draft"};
    private static final int ACCOUNT_NUMBER = 5;
    private RMSStore store = null;
    private Folder folder = null;
    private int version = Store.LATEST_VERSION;
    
    /**
     * Unit test public constructor
     */
    public RMSStoreTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
    }

    
    public void setUp() {
        //Creates the store using the factory pattern
        store = (RMSStore) StoreFactory.getStore();
        //reset the store
        store.init(true);
        //create default folder
        folder = new Folder(FOLDER_PATH, FOLDER_ROLE, FOLDER_CREATION, store);
    }
    
    public void tearDown() {
        //reset the version test field
        version = Store.LATEST_VERSION;
        store.setVersion(version);
        
        //clear and nullify the store after the test
        store.removeFolder(store.getFolder(Folder.ROOT_FOLDER_PATH), true);
        store=null;
    }
    

    public void testGetVersion() throws Exception {
        Log.info("RMSStoreTest: testGetVersion");
        assertEquals(version, store.getVersion());
        Log.info("RMSStoreTest: testGetVersion Successful");
    }

    public void testSetVersion() throws Exception {
        Log.info("RMSStoreTest: testSetVersion");
        version = Store.VERSION_101;
        store.setVersion(version);
        assertEquals(version, store.getVersion());
        Log.info("RMSStoreTest: testSetVersion Successful");
    }

    public void testInitDoReset() throws Exception {
        Log.info("RMSStoreTest: testInitDoReset");
        
        store.init(true);

        Folder f = store.getFolder(Folder.ROOT_FOLDER_PATH);
        
        assertTrue(f!=null);
        
        Log.info("RMSStoreTest: testInitDoReset Successful");
    }
    
    public void testInitNoReset() throws Exception {
        Log.info("RMSStoreTest: testInitNoReset");
        store.init(false);
        
        Folder f = store.getFolder(Folder.ROOT_FOLDER_PATH);
        
        Folder[] folders = store.list();
        
        for (int i=0; i<folders.length; i++) {
            Log.debug("" + (i+1) +") " + folders[i].getFullName());
        }
        assertTrue(f!=null);
        Log.info("RMSStoreTest: testInitNoReset Successful");
    }

    public void testAddFolder() throws Exception {
        Log.info("RMSStoreTest: testAddFolder");
        
        Folder f = store.addFolder(folder);
        
        assertTrue(f.getFullName().equals(FOLDER_PATH) &&
                f.getCreated().equals(FOLDER_CREATION) &&
                f.getRole().equals(FOLDER_ROLE));

        store.removeFolder(f, true);
        
        Log.info("RMSStoreTest: testAddFolder successful");
    }
    
    public void testRemoveFolderGivenFolderObject() throws Exception {
        Log.info("RMSStoreTest: testRemoveFolderGivenFolderObject");
        
        Folder f = store.addFolder(folder);
        
        store.removeFolder(f, true);

        try {
            // It should throw a MailException
            folder = store.getFolder(FOLDER_PATH);
            assertTrue(false);
        } catch (MailException e) {
        }

        Log.info("RMSStoreTest: testRemoveFolderGivenFolderObject successful");
    }

    public void testRemoveFolderGivenFolderPath() throws Exception {
        Log.info("RMSStoreTest: testRemoveFolderGivenFolderPath");
        
        Folder f = store.addFolder(folder);
        
        store.removeFolder(FOLDER_PATH);

        try {
            // It should throw a MailException
            folder = store.getFolder(FOLDER_PATH);
            assertTrue(false);
        } catch (MailException e) {
        }

        Log.info("RMSStoreTest: testRemoveFolderGivenFolderPath successful");
    }

    public void testGetFolder() throws Exception {
        Log.info("RMSStoreTest: testGetFolder");
        
        Folder expected = store.addFolder(folder);
        
        Folder result = store.getFolder(FOLDER_PATH);
        
        assertTrue(expected.getFullName().equals(result.getFullName()) &&
                expected.getRole().equals(result.getRole()) &&
                expected.getCreated().equals(result.getCreated()));

        Log.info("RMSStoreTest: testGetFolder successful");
    }
    
    public void testGetLightFolder() throws Exception {
        Log.info("RMSStoreTest: testGetLightFolder");
        
        Folder expected = store.addFolder(new Folder("/Inbox", "", new Date(), store));
        Folder result = store.getFolder("/Inbox", true);

        assertTrue(expected.getFullName().equals(result.getFullName()) &&
                result.getRole().equals("inbox"));
                
        expected = store.addFolder(new Folder("/Outbox", "", new Date(), store));
        result = store.getFolder("/Outbox", true);

        assertTrue(expected.getFullName().equals(result.getFullName()) &&
                result.getRole().equals("outbox"));

        Log.info("RMSStoreTest: testGetLightFolder successful");
    }

    public void testFindFolders() throws Exception {
        Log.info("RMSStoreTest: testFindFolder");
        int folderNumber = 3;
        
        Folder[] folders = new Folder[folderNumber];
        for (int i = 0; i < folderNumber; i++) {
            folders[i] = new Folder(FOLDER_PATH + "_" + i, FOLDER_ROLE, FOLDER_CREATION, store);
            store.addFolder(folders[i]);
        }
        
        Folder[] result = store.findFolders(FOLDER_PATH);
        
        assertTrue(result.length == 3);

        Log.info("RMSStoreTest: testFindFolder successful");
    }
    
    public void testRetrieveSubfolderID() throws Exception {
        Log.info("RMSStoreTest: testRetrieveSubfolderID");

        Folder folder = new Folder(FOLDER_PATH, FOLDER_ROLE, FOLDER_CREATION, store);
        Folder folderChild1 = new Folder(FOLDER_PATH + "/child1", FOLDER_ROLE, FOLDER_CREATION, store);
        Folder folderChild2 = new Folder(FOLDER_PATH + "/child2", FOLDER_ROLE, FOLDER_CREATION, store);

        store.addFolder(folder);
        store.addFolder(folderChild1);
        store.addFolder(folderChild2);

        String childId1 = store.retrieveSubfolderID(FOLDER_PATH + "/child1", FOLDER_PATH);
        String childId2 = store.retrieveSubfolderID(FOLDER_PATH + "/child2", FOLDER_PATH);

        assertTrue(childId1.equals("1"));
        assertTrue(childId2.equals("2"));

        Log.info("RMSStoreTest: testRetrieveSubfolderID successful");
    }

    public void testList() throws Exception {
        Log.info("RMSStoreTest: testList");
        int listDepth = 3;
        
        Folder[] folders = new Folder[listDepth];
        for (int i = 0; i < listDepth; i++) {
            folders[i] = new Folder(FOLDER_PATH + "_" + i, FOLDER_ROLE, FOLDER_CREATION, store);
            store.addFolder(folders[i]);
        }
        
        Folder[] result = store.list();
        
        assertTrue(result.length==3);

        Log.info("RMSStoreTest: testList successful");
    }

    public void testListGivenPath() throws Exception {
        Log.info("RMSStoreTest: testListGivenPath");
        int listDepth = 3;
        
        Folder[] folders = new Folder[listDepth];
        for (int i = 0; i < listDepth; i++) {
            folders[i] = new Folder(FOLDER_PATH + "_" + i, FOLDER_ROLE, FOLDER_CREATION, store);
            store.addFolder(folders[i]);
        }
        
        Folder[] result = store.list("/");
        
        assertTrue(result.length==3);

        Log.info("RMSStoreTest: testListGivenPath successful");
    }

    public void testAddChild() throws Exception {
        Log.info("RMSStoreTest: testAddChild");

        Folder f = store.addFolder(folder);
        
        Message msg = new Message();
        
        int id = store.addChild(f.getFullName(), msg);
        
        String[] ids = store.getChildIDs(f.getFullName());
        
        assertTrue(id==Integer.parseInt(ids[0])&&ids.length==1);
        
        Log.info("RMSStoreTest: testAddChild successful");
    }

    public void testCountChilds() throws Exception {
        Log.info("RMSStoreTest: testCountChilds");
        int childrenNumber = 10;
        
        Folder f = store.addFolder(folder);
        
        Message[] msgs = new Message[childrenNumber];
        
        for (int i=0; i<childrenNumber; i++) {
            msgs[i] = new Message();
            store.addChild(f.getFullName(), msgs[i]);
        }
        
        assertTrue(store.countChilds(FOLDER_PATH) == childrenNumber);
        
        Log.info("RMSStoreTest: testCountChilds successful");
    }

    public void testCountChildsGivenPrefix() throws Exception {
        Log.info("RMSStoreTest: testCountChildsGivenPrefix");
        int childrenNumber = 10;

        Folder f = store.addFolder(folder);
        
        Message[] msgs = new Message[childrenNumber];
        
        for (int i = 0; i < childrenNumber; i++) {
            msgs[i] = new Message();
            store.addChild(f.getFullName(), msgs[i]);
        }
        
        assertTrue(store.countChilds(FOLDER_PATH, 'M') == childrenNumber);
        
        Log.info("RMSStoreTest: testCountChildsGivenPrefix successful");
    }

    public void testGetChildIDs() throws Exception {
        Log.info("RMSStoreTest: testGetChildIDs");
        int childrenNumber = 10;
        int[] childIDs = new int[10];
        
        Folder f = store.addFolder(folder);
        
        for (int i=0; i<childrenNumber; i++) {
            childIDs[i] = store.addChild(f.getFullName(), new Message());
        }
        
        assertTrue(store.countChilds(FOLDER_PATH, 'M') == childrenNumber &&
                isChildrenArrayMatching(childIDs, store.getChildIDs(FOLDER_PATH)));
       
        Log.info("RMSStoreTest: testGetChildIDs successful");
    }

    public void testReadChild() throws Exception {
        Log.info("RMSStoreTest: testReadChild");
        
        Folder f = store.addFolder(folder);
        
        Message msg = new Message();
        
        String msgId = "ID0123456789";
        msg.setMessageId(msgId);

        int id = store.addChild(f.getFullName(), msg);
        
        Message m = (Message) store.readChild(FOLDER_PATH, String.valueOf(id));
        
        assertTrue(m.getMessageId().equals(msgId));
        
        Log.info("RMSStoreTest: testReadChild successful");
    }

    public void testReadChildBytes() throws Exception {
        Log.info("RMSStoreTest: testReadChildBytes");

        Folder f = store.addFolder(folder);
        
        Message msg = new Message();
        
        String msgId = "ID0123456789";
        msg.setMessageId(msgId);
        
        int id = store.addChild(f.getFullName(), msg);

        DataInputStream result = store.readChildBytes(FOLDER_PATH, String.valueOf(id));

        Message m = new Message();
        try {
            m.deserialize(result);
        } catch (IOException ex) {
            Log.error("IO Exception occurred when deserializing test message");
        }
        
        assertTrue(m.getMessageId().equals(msgId));
        
        Log.info("RMSStoreTest: testReadChildBytes successful");
    }

    public void testGetChildren() throws Exception {
        Log.info("RMSStoreTest: testGetChildren");
        
        
        int childrenNumber = 10;
        int[] childIDs = new int[childrenNumber];

        Folder f = store.addFolder(folder);

        for (int i = 0; i < childrenNumber; i++) {
            childIDs[i] = store.addChild(f.getFullName(), new Message());
        }

        String[] readChildrenIDs = new String[childrenNumber];
        
        int count = 0;
        ChildrenEnumeration children = store.getChildren(FOLDER_PATH);
        while(children.hasMoreElements()) {
            readChildrenIDs[count] = String.valueOf(((RmsRecordItem)children.nextElement()).getRecordId());
            count++;
        }
        assertTrue(isChildrenArrayMatching(childIDs, readChildrenIDs));
        assertEquals(count, childrenNumber);

        Log.info("RMSStoreTest: testGetChildren successful");
    }

    public void testRemoveChild() throws Exception {
        Log.info("RMSStoreTest: testRemoveChild");

        Folder f = store.addFolder(folder);
        
        Message msg = new Message();
        
        int id = store.addChild(f.getFullName(), msg);
        
        store.removeChild(FOLDER_PATH, String.valueOf(id));
        
        assertTrue(store.countChilds(FOLDER_PATH, 'M') == 0);
        
        Log.info("RMSStoreTest: testRemoveChild successful");
    }

    public void testUpdateChild() throws Exception {
        Log.info("RMSStoreTest: testUpdateChild");

        Folder f = store.addFolder(folder);
        
        Message msg = new Message();
        
        int id = store.addChild(f.getFullName(), msg);
        
        String msgId = "ID0123456789";
        msg.setMessageId(msgId);
        
        store.updateChild(FOLDER_PATH, msg);
        
        Message result = store.getFolder(FOLDER_PATH).getMessage(String.valueOf(id));
        
        assertTrue(result.getMessageId().equals(msgId));
        
        Log.info("RMSStoreTest: testUpdateChild successful");
    }

    public void testAddFolderAccount() throws Exception {
        Log.info("RMSStoreTest: testAddFolderAccount");

        AccountFolder af = getDefaultAccount();

        store.addFolder(af);
        
        Folder[] expectedFolders = addDefaultFolders(af);
        
        Folder[] resultFolders = store.list(af.getFullName());
        
        boolean result = isFolderArrayMatching(resultFolders, expectedFolders);
        
        assertTrue(result);
        
        Log.info("RMSStoreTest: testAddFolderAccount successful");
    }

    public void testAddMultipleAccount() throws Exception {
        Log.info("RMSStoreTest: testAddMultipleAccount");
        
        AccountFolder[] accounts = getAccounts(); 

        //Add the accounts and the default folders for every account 
        for (int i=0; i<accounts.length; i++) {
            store.addFolder(accounts[i]);
            addDefaultFolders(accounts[i]);
        }

        //Create the result vector
        Vector result = new Vector();
        for (int i=0; i<accounts.length; i++) {
            Folder[] resultFolders = store.list(accounts[i].getFullName());
            
            for (int j=0; j<resultFolders.length; j++) {
                result.addElement(resultFolders[j].getFullName());
            }
        }
        
        //Create the expected result vector
        Vector expected = new Vector();
        for (int i=0; i<accounts.length; i++) {
            for (int j=0; j<DEFAULT_ACCOUNT_FOLDERS.length; j++) {
                String name = accounts[i].getFullName() + DEFAULT_ACCOUNT_FOLDERS[j];
                expected.addElement(name);
            }
        } 
        
        for (int i=0; i<expected.size(); i++) {
            assertTrue(result.contains(expected.elementAt(i)));
        }

        Log.info("RMSStoreTest: testAddMultipleAccount successful");
    }
    
    public void testAddMultipleAccountWithMessages() throws Exception {
        Log.info("RMSStoreTest: testAddMultipleAccountWithMessages");
        
        long initTime = System.currentTimeMillis();
        
        AccountFolder[] accounts = getAccounts(); 

        int msgNumber = 100;
        
        //Add the accounts and the default folders for every account 
        for (int i=0; i<accounts.length; i++) {
            store.addFolder(accounts[i]);
            addDefaultFolders(accounts[i]);
        }
        
        //Add N messages to each folders
        Folder[] resultFolders = null;
        for (int i=0; i<accounts.length; i++) {
            resultFolders = store.list(accounts[i].getFullName());
            for (int j=0; j<resultFolders.length; j++) {
                for (int k=0; k<msgNumber; k++) {
                    Message m = new Message();
                    m.setMessageId("Message_" + k);
                    m.setLaziness(Message.NO_LAZY);
                    resultFolders[j].appendMessage(m);
                }
            }
        }

        int expectedMsgNumber = msgNumber*accounts.length*resultFolders.length;
        
        int totalMsgNumber = 0;
        
        //Count all messages contained into the storage
        for (int i=0; i<accounts.length; i++) {
            resultFolders = store.list(accounts[i].getFullName());
            for (int j=0; j<resultFolders.length; j++) {
                totalMsgNumber += resultFolders[j].getMessageCount();
            }
        }
        
        assertTrue(expectedMsgNumber==totalMsgNumber);
        
        Log.info("RMSStoreTest: testAddMultipleAccountWithMessages successful - Total Time: " + (System.currentTimeMillis()-initTime));
    }


    public boolean isChildrenArrayMatching(int[] expected, String[] result) throws Exception {
        if (expected.length != result.length) {
            return false;
        }
        for (int i = 0; i < expected.length; i++) {
            boolean found = false;
            for (int j = 0; j < result.length; j++) {
                if (expected[i] != Integer.parseInt(result[j])) {
                    found = true;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }

    private AccountFolder getDefaultAccount() {
        return getAccounts()[0];
    }

    private AccountFolder[] getAccounts() {
        AccountFolder[] afs = new AccountFolder[ACCOUNT_NUMBER];

        for (int i = 0; i < ACCOUNT_NUMBER; i++) {
            afs[i] = new AccountFolder("/account_" + i,
                    new Date(),
                    store,
                    "account_" + i,
                    "someone@account_" + i + ".org");
        }

        return afs;
    }

    private Folder[] addDefaultFolders(Folder parent) {
        
        Folder[] folders = new Folder[DEFAULT_ACCOUNT_FOLDERS.length];

        for (int i = 0; i < DEFAULT_ACCOUNT_FOLDERS.length; i++) {
            folders[i] = new Folder(parent.getFullName() + DEFAULT_ACCOUNT_FOLDERS[i], 
                    FOLDER_ROLE, 
                    new Date(), 
                    store);
            store.addFolder(folders[i]);
        }
        
        return folders;
    }

    private boolean isFolderArrayMatching(Folder[] resultFolders, Folder[] expectedFolders) throws Exception {

        Vector expected = new Vector();

        for (int i = 0; i < DEFAULT_ACCOUNT_FOLDERS.length; i++) {
            expected.addElement(expectedFolders[i].getFullName());
        }

        boolean result = false;
        for (int i = 0; i < DEFAULT_ACCOUNT_FOLDERS.length; i++) {

            result = expected.contains(resultFolders[i].getFullName());

            if (!result) {
                assertTrue(false);
            }
        }

        return result;
    }
}