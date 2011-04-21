/**
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

package com.funambol.syncml.spds;

import java.util.Vector;
import java.util.Hashtable;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Base64;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.BasicSyncListener;
import com.funambol.sync.SyncSource;
import com.funambol.sync.ResumableSource;
import com.funambol.sync.client.BaseSyncSource;

import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.syncml.protocol.SyncMLCommand;
import com.funambol.syncml.protocol.Item;
import com.funambol.syncml.protocol.Meta;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.Data;
import com.funambol.syncml.protocol.Source;
import com.funambol.syncml.protocol.SourceParent;
import com.funambol.syncml.protocol.TargetParent;

import junit.framework.*;

public class SyncSourceHandlerTest extends TestCase {

    private class TestSyncListener extends BasicSyncListener {

        private int numAddSent     = 0;
        private int numReplaceSent = 0;
        private int numDeleteSent  = 0;

        public void itemAddSendingEnded(String key, String parent) {
            ++numAddSent;
        }

        public void itemReplaceSendingEnded(String key, String parent) {
            ++numReplaceSent;
        }

        public void itemDeleteSent(SyncItem item) {
            ++numDeleteSent;
        }

        public int getAddSent() {
            return numAddSent;
        }

        public int getReplaceSent() {
            return numReplaceSent;
        }

        public int getDeleteSent() {
            return numDeleteSent;
        }
    }

    private class TestSyncSource extends BaseSyncSource implements ResumableSource {
        private SyncItem lastAdded     = null;
        private SyncItem lastUpdated   = null;
        private String   lastDeleted   = null;
        private Vector   nextNewItems  = new Vector();
        private Vector   nextUpdItems  = new Vector();
        private Vector   nextDelItems  = new Vector();
        private Vector   nextItems     = new Vector();
        private int      nextNewItemId = 0;
        private int      nextUpdItemId = 0;
        private int      nextDelItemId = 0;
        private int      nextItemId    = 0;
        private Hashtable deletedMap   = null;
        private Hashtable changedMap   = null;

        public TestSyncSource(SourceConfig config) {
            super(config);
        }

        public int addItem(SyncItem item) throws SyncException {
            lastAdded = item;
            return SyncMLStatus.SUCCESS;
        }

        public int updateItem(SyncItem item) throws SyncException {
            lastUpdated = item;
            return SyncMLStatus.SUCCESS;
        }
    
        public int deleteItem(String key) throws SyncException {
            lastDeleted = key;
            return SyncMLStatus.SUCCESS;
        }

        public SyncItem getNextNewItem() throws SyncException {
            if (nextNewItemId < nextNewItems.size()) {
                return (SyncItem) nextNewItems.elementAt(nextNewItemId++);
            } else {
                return null;
            }
        }

        public SyncItem getNextUpdatedItem() throws SyncException {
            if (nextUpdItemId < nextUpdItems.size()) {
                return (SyncItem) nextUpdItems.elementAt(nextUpdItemId++);
            } else {
                return null;
            }
        }

        public SyncItem getNextDeletedItem() throws SyncException {
            if (nextDelItemId < nextDelItems.size()) {
                return (SyncItem) nextDelItems.elementAt(nextDelItemId++);
            } else {
                return null;
            }
        }

         public SyncItem getNextItem() throws SyncException {
            if (nextItemId < nextItems.size()) {
                return (SyncItem) nextItems.elementAt(nextItemId++);
            } else {
                return null;
            }
        }

        public void cancel() {
        }

        protected void initAllItems() throws SyncException {
        }

        protected void initNewItems() throws SyncException {
        }

        protected void initUpdItems() throws SyncException {
        }

        protected void initDelItems() throws SyncException {
        }

        protected SyncItem getItemContent(final SyncItem item) throws SyncException {
            return null;
        }

        public SyncItem lastItemAdded() {
            return lastAdded;
        }

        public SyncItem lastItemUpdated() {
            return lastUpdated;
        }

        public void setNextNewItems(Vector items) {
            nextNewItems = items;
        }

        public void setNextUpdItems(Vector items) {
            nextUpdItems = items;
        }

        public void setNextDelItems(Vector items) {
            nextDelItems = items;
        }

        public void setNextItems(Vector items) {
            nextItems = items;
        }

        public boolean readyToResume() {
            return true;
        }

        public boolean hasChangedSinceLastSync(String key, long ts) {
            if (changedMap != null) {
                return changedMap.get(key) != null;
            } else {
                return false;
            }
        }

        public boolean exists(String key) {
            if (deletedMap == null) {
                return true;
            } else {
                return deletedMap.get(key) == null;
            }
        }

        public void setChangedMap(Hashtable changedMap) {
            this.changedMap = changedMap;
        }

        public void setDeletedMap(Hashtable deletedMap) {
            this.deletedMap = deletedMap;
        }

        public String getLuid(SyncItem item) {
            return null;
        }

        public long getPartiallyReceivedItemSize(String key) {
            return -1;
        }
    }

    private final String SYNCML_FOLDER_DATA = "<Data>" +
                                              "<![CDATA[" +
                                              "<Folder>" +
                                              "<name>Inbox</name>" +
                                              "<created>20090428T162654Z&lt;/created>" +
                                              "<role>inbox</role>" +
                                              "</Folder>" +
                                              "]]>" +
                                              "</Data>";

    public SyncSourceHandlerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void testAddItem() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        // Add items and checks that the sync source addItem is invoked for each
        // small object, while large object are assmbled in memory and passed to
        // the sync source once completed

        ItemsList items = new ItemsList();
        Vector syncItems;

        // Simulate a complete item
        SyncMLCommand add1Cmd = SyncMLCommand.newInstance(SyncML.TAG_ADD);
        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        items.addElement(add1Cmd, item1);

        // Simulate a partial item
        Chunk item2 = new Chunk("1", null, null, null, true);
        byte content2[] = new byte[2001];
        fillContent(content2, 'A');
        item2.setContent(content2);

        // Apply all the changes of this message
        syncItems = handler.applyChanges(items, null);
        handler.addUpdateChunk(item2, true);
        // Now check that the ss was properly updated
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));
        items.removeAllElements();

        // Now finalize the LO
        SyncMLCommand add3Cmd = SyncMLCommand.newInstance(SyncML.TAG_ADD);
        Chunk item3 = new Chunk("2", null, null, null, false);
        byte content3[] = new byte[48];
        fillContent(content3, 'B');
        item3.setContent(content3);
        items.addElement(add3Cmd, item3);

        syncItems = handler.applyChanges(items, null);
        assertTrue(ss.lastItemAdded() != null);

        assertTrue(ss.lastItemAdded().getKey().equals(item2.getKey()));
        assertTrue(ss.lastItemAdded().getContent().length == content2.length + content3.length);

        // Now perform the same test with a sync source using b64 encoding
        ss.getConfig().setEncoding(SyncSource.ENCODING_B64);
        items.removeAllElements();

        // An encoded ss must be given encoded items
        item1.setContent(Base64.encode(item1.getContent()));
        items.addElement(add1Cmd, item1);
        item2.setContent(Base64.encode(item2.getContent()));
        syncItems = handler.applyChanges(items, null);
        handler.addUpdateChunk(item2, true);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));
        items.removeAllElements();

        item3.setContent(Base64.encode(item3.getContent()));
        items.addElement(add3Cmd, item3);
        syncItems = handler.applyChanges(items, null);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item2.getKey()));
        byte content[] = ss.lastItemAdded().getContent();
        content = Base64.decode(content);
        assertTrue(content.length == content2.length + content3.length);
    }

    public void testUpdateItem() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        ItemsList items = new ItemsList();
        Vector syncItems = new Vector();

        // Add items and checks that the sync source addItem is invoked for each
        // small object, while large object are assmbled in memory and passed to
        // the sync source once completed

        // Simulate a complete item
        SyncMLCommand add1Cmd = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);
        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        items.addElement(add1Cmd, item1);

        // Simulate a partial item
        Chunk item2 = new Chunk("1", null, null, null, true);
        byte content2[] = new byte[2001];
        fillContent(content2, 'A');
        item2.setContent(content2);
        item2.setHasMoreData();
        syncItems = handler.applyChanges(items, null);
        handler.addUpdateChunk(item2, false);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        items.removeAllElements();

        // And now finalize it 
        SyncMLCommand add3Cmd = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);
        Chunk item3 = new Chunk("2", null, null, null, false);
        byte content3[] = new byte[48];
        fillContent(content3, 'B');
        item3.setContent(content3);
        items.addElement(add3Cmd, item3);
        syncItems = handler.applyChanges(items, null);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item2.getKey()));
        assertTrue(ss.lastItemUpdated().getContent().length == content2.length + content3.length);

        // Now perform the same test with a sync source using b64 encoding
        ss.getConfig().setEncoding(SyncSource.ENCODING_B64);
        items.removeAllElements();

        // An encoded ss must be given encoded items
        item1.setContent(Base64.encode(item1.getContent()));
        items.addElement(add1Cmd, item1);

        item2.setContent(Base64.encode(item2.getContent()));
        handler.applyChanges(items, null);
        handler.addUpdateChunk(item2, false);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        items.removeAllElements();
        item3.setContent(Base64.encode(item3.getContent()));
        items.addElement(add3Cmd, item3);
        syncItems = handler.applyChanges(items, null);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item2.getKey()));
        byte content[] = ss.lastItemUpdated().getContent();
        content = Base64.decode(content);
        assertTrue(content.length == content2.length + content3.length);
    }

    public void testGetAddCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 640, false);
        Vector newItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_NEW, null);
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_NEW, null);
        byte content1[] = new byte[180];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_NEW, null);
        byte content2[] = new byte[10];
        fillContent(content2, 'A');
        item2.setContent(content2);

        newItems.addElement(item0);
        newItems.addElement(item1);
        newItems.addElement(item2);

        ss.setNextNewItems(newItems);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);
        SyncMLCommand command = SyncMLCommand.newInstance("");

        // The first message shall contain only the first item
        SyncStatus syncStatus = new SyncStatus(ss.getName());
        int status = handler.getAddCommand(0, listener, command, cmdId, syncStatus);
        assertTrue(status == SyncSourceLOHandler.MORE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getAddSent() == 1);

        // The second message shall contain the other two items
        command = SyncMLCommand.newInstance("");
        status = handler.getAddCommand(0, listener, command, cmdId, syncStatus);
        assertTrue(status == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getAddSent() == 3);
    }

    public void testGetReplaceCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 640, false);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[180];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[10];
        fillContent(content2, 'A');
        item2.setContent(content2);

        updItems.addElement(item0);
        updItems.addElement(item1);
        updItems.addElement(item2);

        ss.setNextUpdItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        int status = handler.getReplaceCommand(0, listener, command, cmdId);
        assertTrue(status == SyncSourceLOHandler.MORE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 1);

        // The second message shall contain the other two items
        status = handler.getReplaceCommand(0, listener, command, cmdId);
        assertTrue(status == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 3);
    }

    public void testGetDeleteCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 400, false);
        Vector delItems = new Vector(3);
        SyncItem item0 = new SyncItem("WeNeedAVeryLongKeySoTheItemIsTheOnlyOne" +
                                      "ThatFitsInASingleMessageBecauseInDeletes" +
                                      "TheContentIsNotTransmitted", null, SyncItem.STATE_DELETED, null);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_DELETED, null);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_DELETED, null);

        delItems.addElement(item0);
        delItems.addElement(item1);
        delItems.addElement(item2);

        ss.setNextDelItems(delItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        // The first message shall contain the first items
        boolean done = handler.getDeleteCommand(0, listener, command, cmdId);
        assertTrue(!done);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getDeleteSent() == 2);

        // The second message shall contain the other item
        done = handler.getDeleteCommand(0, listener, command, cmdId);
        assertTrue(done);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getDeleteSent() == 3);
    }

    public void testGetNextCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 640, false);
        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[150];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[50];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        SyncStatus syncStatus = new SyncStatus("test");
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.MORE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 1);

        // The second message shall contain the other two items
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 3);
    }

    public void testGetNextCommandResume1() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncStatus syncStatus = new SyncStatus("test");
        syncStatus.addSentItem("0", SyncML.TAG_ADD);
        syncStatus.receivedItemStatus("0", 200);
        syncStatus.addSentItem("1", SyncML.TAG_ADD);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 4096, false);
        handler.setResume(true);

        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[150];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[50];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The message shall contain only two items: 1 and 2
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 2);
    }

    public void testGetNextCommandResume2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        Hashtable deletedItems = new Hashtable();
        deletedItems.put("0", "0");
        deletedItems.put("1", "1");
        ss.setDeletedMap(deletedItems);

        SyncStatus syncStatus = new SyncStatus("test");
        syncStatus.addSentItem("0", SyncML.TAG_ADD);
        syncStatus.receivedItemStatus("0", 200);
        syncStatus.addSentItem("1", SyncML.TAG_ADD);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 4096, false);
        handler.setResume(true);

        Vector items = new Vector();
        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[50];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain two deletes
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.MORE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getDeleteSent() == 2);
        assertTrue(SyncML.TAG_DELETE.equals(command.getName()));

        // The last message shall contain the third item
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);

        assertTrue(listener.getReplaceSent() == 1);
        assertTrue(SyncML.TAG_REPLACE.equals(command.getName()));
    }

    public void testGetNextCommandResume3() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        Hashtable deletedItems = new Hashtable();
        deletedItems.put("0", "0");
        ss.setDeletedMap(deletedItems);

        Hashtable changedItems = new Hashtable();
        changedItems.put("1", "1");
        ss.setChangedMap(changedItems);

        SyncStatus syncStatus = new SyncStatus("test");
        syncStatus.addSentItem("0", SyncML.TAG_ADD);
        syncStatus.receivedItemStatus("0", 200);
        syncStatus.addSentItem("1", SyncML.TAG_ADD);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 4096, false);
        handler.setResume(true);

        Vector items = new Vector();
        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[50];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[50];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain one delete
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.MORE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getDeleteSent() == 1);
        assertTrue(SyncML.TAG_DELETE.equals(command.getName()));

        // The last message shall contain the two items
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);

        assertTrue(listener.getReplaceSent() == 2);
        assertTrue(SyncML.TAG_REPLACE.equals(command.getName()));
    }


    /**
     * This method check if chunks decoding works properly.
     */
    public void testChunkdecoding() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "text/plain", "briefcase");
        config.setEncoding(SyncSource.ENCODING_B64);
        TestSyncSource ss = new TestSyncSource(config);

        byte content0Bytes[] = new byte[100];
        for(int i=0;i<content0Bytes.length;++i) {
            content0Bytes[i] = 'A';
        }
        String encodedContent0 = new String(Base64.encode(content0Bytes));

        String chunk0Content = encodedContent0.substring(0, 59);
        String chunk1Content = encodedContent0.substring(59);

        // Create an item with 100 bytes or so
        Item item0 = createTestItem("0", "text/plain", chunk0Content);
        item0.setMoreData(new Boolean(true));
        Item item1 = createTestItem("0", "text/plain", chunk1Content);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 640, false);
        TestSyncListener listener = new TestSyncListener();

        String formats[] = { "b64" };
        Chunk firstChunk = handler.getItem(item0, "text/plain", formats, null);
        Chunk secondChunk = handler.getItem(item1, "text/plain", formats, null);

        String decodedItem = new String(firstChunk.getContent()) + new String(secondChunk.getContent());
        String compare     = new String(content0Bytes);
        assertTrue(decodedItem.equals(compare));
    }

    /**
     * Check if the TargetParent property is correctly read from a SyncML command
     * @throws java.lang.Throwable
     */
    public void testTargetParent() throws Throwable {
        simpleSyncItemTest("101", "xyz01", null);
    }

    /**
     * Check if the SourceParent is correctly read and translated to the local
     * parent key.
     * @throws java.lang.Throwable
     */
    public void testSourceParent() throws Throwable {

        Hashtable hierarchy = new Hashtable();
        hierarchy.put("222", "2222");
        hierarchy.put("333", "3333");
        hierarchy.put("100", "xyz01"); // the parent mapping
        hierarchy.put("999", "9999");
        simpleSyncItemTest("101", "xyz01", hierarchy);
    }

    /**
     * Check if the SourceParent is correctly read and translated to the local
     * parent key. A SyncException must be thrown if the local parent key is not
     * found.
     * @throws java.lang.Throwable
     */
    public void testWrongSourceParent() throws Throwable {

        Hashtable hierarchy = new Hashtable();
        hierarchy.put("222", "2222");
        hierarchy.put("333", "3333");
        hierarchy.put("999", "9999");

        // The parent key is not mapped, the item must have a source parent set
        simpleSyncItemTest("101", null, "100", hierarchy);
    }

    /*
    public void dis_testCancelAddItem1() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        // Simulate a complete item
        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);

        handler.cancel();
        boolean interrupted = false;
        try {
            handler.addItem(item1);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void dis_testCancelAddItem2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        // Simulate a complete item
        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);

        Chunk item2 = new Chunk("1", null, null, null, false);
        byte content2[] = new byte[500];
        fillContent(content2, 'B');
        item2.setContent(content2);

        // Add the first item before interrupting the sync
        handler.addItem(item1);
        handler.cancel();
        boolean interrupted = false;
        try {
            handler.addItem(item1);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void dis_testCancelUpdateItem1() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        boolean interrupted = false;
        handler.cancel();
        try {
            handler.updateItem(item1);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void dis_testCancelUpdateItem2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024, false);

        Chunk item1 = new Chunk("0", null, null, null, false);
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        handler.updateItem(item1);

        Chunk item2 = new Chunk("1", null, null, null, false);
        byte content2[] = new byte[500];
        fillContent(content2, 'B');
        item2.setContent(content2);
        handler.cancel();

        boolean interrupted = false;
        try {
            handler.updateItem(item1);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }
    */

    public void testCancelGetAddCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector newItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_NEW, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        newItems.addElement(item0);
        ss.setNextNewItems(newItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        handler.cancel();

        boolean interrupted = false;
        try {
            SyncStatus syncStatus = new SyncStatus(ss.getName());
            handler.getAddCommand(0, listener, command, cmdId, syncStatus);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetAddCommand2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector newItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_NEW, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_NEW, null);
        byte content1[] = new byte[10];
        fillContent(content1, 'A');
        item1.setContent(content1);

        newItems.addElement(item0);
        newItems.addElement(item1);

        ss.setNextNewItems(newItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        SyncStatus syncStatus = new SyncStatus(ss.getName());
        int status = handler.getAddCommand(0, listener, command, cmdId, syncStatus);
        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getAddCommand(0, listener, command, cmdId, syncStatus);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetUpdateCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_NEW, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        updItems.addElement(item0);
        ss.setNextUpdItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getReplaceCommand(0, listener, command, cmdId);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetUpdateCommand2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_NEW, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_NEW, null);
        byte content1[] = new byte[10];
        fillContent(content1, 'A');
        item1.setContent(content1);

        updItems.addElement(item0);
        updItems.addElement(item1);

        ss.setNextUpdItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        int status = handler.getReplaceCommand(0, listener, command, cmdId);
        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getReplaceCommand(0, listener, command, cmdId);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetDeleteCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector delItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_DELETED, null);

        delItems.addElement(item0);
        ss.setNextDelItems(delItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getDeleteCommand(0, listener, command, cmdId);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetDeleteCommand2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector delItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_DELETED, null);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_NEW, null);

        delItems.addElement(item0);
        delItems.addElement(item1);

        ss.setNextDelItems(delItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        boolean done = handler.getDeleteCommand(0, listener, command, cmdId);
        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getDeleteCommand(0, listener, command, cmdId);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetNextCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        updItems.addElement(item0);
        ss.setNextItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        SyncMLCommand command = SyncMLCommand.newInstance("");
        CmdId cmdId = new CmdId(0);

        handler.cancel();

        boolean interrupted = false;
        try {
            SyncStatus syncStatus = new SyncStatus("test");
            int msgStatus[] = new int[1];
            handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testCancelGetNextCommand2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[10];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[10];
        fillContent(content1, 'B');
        item1.setContent(content1);

        updItems.addElement(item0);
        updItems.addElement(item1);

        ss.setNextItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        int msgStatus[] = new int[1];
        SyncStatus syncStatus = new SyncStatus("test");
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];
        handler.cancel();

        boolean interrupted = false;
        try {
            handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        } catch (SyncException se) {
            if (se.getCode() == SyncException.CANCELLED) {
                interrupted = true;
            }
        }
        assertTrue(interrupted);
    }

    public void testGetNextCommandWithItemsLimit1() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        // Simulate a max number of items per sync of 1
        config.setMaxItemsPerMessageInSlowSync(1);
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[32];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[16];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[16];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        SyncStatus syncStatus = new SyncStatus("test");
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.FLUSH);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 1);

        // The second message shall contain another item
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.FLUSH);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 2);

        // The third message shall contain the last item item
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 3);
        assertTrue(listener.getReplaceSent() == 3);
    }

    public void testGetNextCommandWithItemsLimit2() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        // Simulate a max number of items per sync of 2
        config.setMaxItemsPerMessageInSlowSync(2);
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);
        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[32];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[16];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[16];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first two items
        SyncStatus syncStatus = new SyncStatus("test");
        int msgStatus[] = new int[1];

        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];

        assertTrue(done == SyncSourceLOHandler.FLUSH);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 2);

        // The third message shall contain the last item item
        command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 3);
    }

    public void testGetNextCommandWithItemsLimit3() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        // Simulate a max number of items per sync of 3
        config.setMaxItemsPerMessageInSlowSync(3);
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 900, false);
        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0", null, SyncItem.STATE_UPDATED, null);
        byte content0[] = new byte[32];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1", null, SyncItem.STATE_UPDATED, null);
        byte content1[] = new byte[16];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2", null, SyncItem.STATE_UPDATED, null);
        byte content2[] = new byte[16];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain all three items
        SyncStatus syncStatus = new SyncStatus("test");
        int msgStatus[] = new int[1];
        SyncMLCommand command = handler.getNextCommand(0, listener, cmdId, syncStatus, msgStatus);
        int done = msgStatus[0];
        assertTrue(done == SyncSourceLOHandler.DONE);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 3);
    }


    private void simpleSyncItemTest (String expectedKey, String expectedParent,
                                     Hashtable hierarchy) throws Throwable {

        simpleSyncItemTest(expectedKey, expectedParent, null, hierarchy);
    }

    /**
     * Simple SyncItem test. Verify if the provided xml command is related to
     * the correct item key and parent key.
     * @param xmlCommand
     * @param expectedKey
     * @param expectedParent
     * @param hierarchy
     * @throws java.lang.Throwable
     */
    private void simpleSyncItemTest(String expectedKey, String expectedParent, String expectedSourceParent,
                                    Hashtable hierarchy) throws Throwable
    {

        SourceConfig config = new SourceConfig(SourceConfig.MAIL, SourceConfig.EMAIL_OBJECT_TYPE, SourceConfig.MAIL);
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512, false);

        Item i = createTestItem(expectedKey, "application/vnd.omads-folder+xml", "");
        if (expectedParent != null) {
            TargetParent tgtParent = new TargetParent(expectedParent);
            i.setTargetParent(tgtParent);
        }
        if (expectedSourceParent != null) {
            SourceParent srcParent = SourceParent.newInstance();
            srcParent.setLocURI(expectedSourceParent);
            i.setSourceParent(srcParent);
        }
        Chunk item = handler.getItem(i, "application/vnd.omads-folder+xml", null, hierarchy);

        assertTrue(item.getType().equals("application/vnd.omads-folder+xml"));
        assertTrue(item.getKey().equals(expectedKey));
        if (expectedParent != null) {
            assertTrue(item.getParent().equals(expectedParent));
        }
        if (expectedSourceParent != null) {
            assertTrue(item.getSourceParent().equals(expectedSourceParent));
        }
    }

    private void fillContent(byte content[], char filler) {
        for(int i=0;i<content.length;++i) {
            content[i] = (byte)filler;
        }
    }

    private Item createTestItem(String locUri, String format, String content) {
        Item item0 = Item.newInstance();
        Meta meta0 = Meta.newInstance();
        meta0.setFormat(format);
        Source source0 = Source.newInstance();
        source0.setLocURI(locUri);
        Data data0 = Data.newInstance(content);
        item0.setMeta(meta0);
        item0.setSource(source0);
        item0.setData(data0);

        return item0;
    }


}

