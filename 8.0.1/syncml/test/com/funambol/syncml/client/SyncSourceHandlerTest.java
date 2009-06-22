/*
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

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.BasicSyncListener;
import com.funambol.util.SyncListener;
import com.funambol.util.Base64;
import com.funambol.util.ConsoleAppender;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueFileStore;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncException;
import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.syncml.client.BaseSyncSource;

import com.funambol.util.FunBasicTest;
import j2meunit.framework.*;

public class SyncSourceHandlerTest  extends FunBasicTest {

    private class TestSyncListener extends BasicSyncListener {

        private int numAddSent     = 0;
        private int numReplaceSent = 0;
        private int numDeleteSent  = 0;

        public void itemAddSent(Object item) {
            ++numAddSent;
        }

        public void itemReplaceSent(Object item) {
            ++numReplaceSent;
        }

        public void itemDeleteSent(Object item) {
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

    private class TestSyncSource extends BaseSyncSource {
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
    }

    public SyncSourceHandlerTest() throws Exception {
        super(6, "SyncSourceHandlerTest");
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void test(int testNumber) throws Throwable {

        switch(testNumber) {
            case 0:
                testAddItem();
                break;
            case 1:
                testUpdateItem();
                break;
            case 2:
                testGetAddCommand();
                break;
            case 3:
                testGetReplaceCommand();
                break;
            case 4:
                testGetDeleteCommand();
                break;
            case 5:
                testGetNextCommand();
                break;
            default:
                break;
        }
    }

    private void testAddItem() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024);

        // Add items and checks that the sync source addItem is invoked for each
        // small object, while large object are assmbled in memory and passed to
        // the sync source once completed

        // Simulate a complete item
        SyncItem item1 = new SyncItem("0");
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        handler.addItem(item1);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));

        // Simulate a partial item
        SyncItem item2 = new SyncItem("1");
        byte content2[] = new byte[2000];
        fillContent(content2, 'A');
        item2.setContent(content2);
        item2.setHasMoreData();
        handler.addItem(item2);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));

        // And now finalize it 
        SyncItem item3 = new SyncItem("2");
        byte content3[] = new byte[48];
        fillContent(content3, 'B');
        item3.setContent(content3);
        handler.addItem(item3);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item2.getKey()));
        assertTrue(ss.lastItemAdded().getContent().length == content2.length + content3.length);

        // Now perform the same test with a sync source using b64 encoding
        ss.getConfig().setEncoding(SyncSource.ENCODING_B64);

        // An encoded ss must be given encoded items
        item1.setContent(Base64.encode(item1.getContent()));
        handler.addItem(item1);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));

        item2.setContent(Base64.encode(item2.getContent()));
        handler.addItem(item2);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item1.getKey()));

        item3.setContent(Base64.encode(item3.getContent()));
        handler.addItem(item3);
        assertTrue(ss.lastItemAdded() != null);
        assertTrue(ss.lastItemAdded().getKey().equals(item2.getKey()));
        byte content[] = ss.lastItemAdded().getContent();
        content = Base64.decode(content);
        assertTrue(content.length == content2.length + content3.length);
    }

    private void testUpdateItem() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 2 * 1024);

        // Add items and checks that the sync source addItem is invoked for each
        // small object, while large object are assmbled in memory and passed to
        // the sync source once completed

        // Simulate a complete item
        SyncItem item1 = new SyncItem("0");
        byte content1[] = new byte[500];
        fillContent(content1, 'A');
        item1.setContent(content1);
        handler.updateItem(item1);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        // Simulate a partial item
        SyncItem item2 = new SyncItem("1");
        byte content2[] = new byte[2000];
        fillContent(content2, 'A');
        item2.setContent(content2);
        item2.setHasMoreData();
        handler.updateItem(item2);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        // And now finalize it 
        SyncItem item3 = new SyncItem("2");
        byte content3[] = new byte[48];
        fillContent(content3, 'B');
        item3.setContent(content3);
        handler.updateItem(item3);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item2.getKey()));
        assertTrue(ss.lastItemUpdated().getContent().length == content2.length + content3.length);

        // Now perform the same test with a sync source using b64 encoding
        ss.getConfig().setEncoding(SyncSource.ENCODING_B64);

        // An encoded ss must be given encoded items
        item1.setContent(Base64.encode(item1.getContent()));
        handler.updateItem(item1);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        item2.setContent(Base64.encode(item2.getContent()));
        handler.updateItem(item2);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item1.getKey()));

        item3.setContent(Base64.encode(item3.getContent()));
        handler.updateItem(item3);
        assertTrue(ss.lastItemUpdated() != null);
        assertTrue(ss.lastItemUpdated().getKey().equals(item2.getKey()));
        byte content[] = ss.lastItemUpdated().getContent();
        content = Base64.decode(content);
        assertTrue(content.length == content2.length + content3.length);
    }

    private void testGetAddCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512);
        Vector newItems = new Vector(3);
        SyncItem item0 = new SyncItem("0");
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1");
        byte content1[] = new byte[200];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2");
        byte content2[] = new byte[10];
        fillContent(content2, 'A');
        item2.setContent(content2);

        newItems.addElement(item0);
        newItems.addElement(item1);
        newItems.addElement(item2);

        ss.setNextNewItems(newItems);

        TestSyncListener listener = new TestSyncListener();
        StringBuffer cmdTag = new StringBuffer();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        boolean done = handler.getAddCommand(0, listener, cmdTag, cmdId);
        assertTrue(!done);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getAddSent() == 1);

        // The second message shall contain the other two items
        cmdTag = new StringBuffer();
        done = handler.getAddCommand(0, listener, cmdTag, cmdId);
        assertTrue(done);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getAddSent() == 3);
    }

    private void testGetReplaceCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512);
        Vector updItems = new Vector(3);
        SyncItem item0 = new SyncItem("0");
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1");
        byte content1[] = new byte[200];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2");
        byte content2[] = new byte[10];
        fillContent(content2, 'A');
        item2.setContent(content2);

        updItems.addElement(item0);
        updItems.addElement(item1);
        updItems.addElement(item2);

        ss.setNextUpdItems(updItems);

        TestSyncListener listener = new TestSyncListener();
        StringBuffer cmdTag = new StringBuffer();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        boolean done = handler.getReplaceCommand(0, listener, cmdTag, cmdId);
        assertTrue(!done);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 1);

        // The second message shall contain the other two items
        cmdTag = new StringBuffer();
        done = handler.getReplaceCommand(0, listener, cmdTag, cmdId);
        assertTrue(done);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 3);
    }

    private void testGetDeleteCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 200);
        Vector delItems = new Vector(3);
        SyncItem item0 = new SyncItem("WeNeedAVeryLongKeySoTheItemIsTheOnlyOne" +
                                      "ThatFitsInASingleMessageBecauseInDeletes" +
                                      "TheContentIsNotTransmitted");
        item0.setState(SyncItem.STATE_DELETED);

        SyncItem item1 = new SyncItem("1");
        item1.setState(SyncItem.STATE_DELETED);

        SyncItem item2 = new SyncItem("2");
        item2.setState(SyncItem.STATE_DELETED);

        delItems.addElement(item0);
        delItems.addElement(item1);
        delItems.addElement(item2);

        ss.setNextDelItems(delItems);

        TestSyncListener listener = new TestSyncListener();
        StringBuffer cmdTag = new StringBuffer();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        boolean done = handler.getDeleteCommand(0, listener, cmdTag, cmdId);
        assertTrue(!done);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getDeleteSent() == 1);

        // The second message shall contain the other two items
        cmdTag = new StringBuffer();
        done = handler.getDeleteCommand(0, listener, cmdTag, cmdId);
        assertTrue(done);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getDeleteSent() == 3);
    }

    private void testGetNextCommand() throws Throwable {
        SourceConfig config = new SourceConfig("Test", "application/*", "briefcase");
        config.setEncoding(SyncSource.ENCODING_NONE);
        TestSyncSource ss = new TestSyncSource(config);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(ss, 512);
        Vector items = new Vector();
        SyncItem item0 = new SyncItem("0");
        item0.setState(SyncItem.STATE_NEW);
        byte content0[] = new byte[320];
        fillContent(content0, 'A');
        item0.setContent(content0);

        SyncItem item1 = new SyncItem("1");
        item1.setState(SyncItem.STATE_NEW);
        byte content1[] = new byte[150];
        fillContent(content1, 'A');
        item1.setContent(content1);

        SyncItem item2 = new SyncItem("2");
        item2.setState(SyncItem.STATE_UPDATED);
        byte content2[] = new byte[50];
        fillContent(content2, 'A');
        item2.setContent(content2);

        items.addElement(item0);
        items.addElement(item1);
        items.addElement(item2);

        ss.setNextItems(items);

        TestSyncListener listener = new TestSyncListener();
        StringBuffer cmdTag = new StringBuffer();
        CmdId cmdId = new CmdId(0);

        // The first message shall contain only the first item
        boolean done = handler.getNextCommand(0, listener, cmdTag, cmdId);
        Log.debug(cmdTag.toString());
        assertTrue(!done);
        assertTrue(cmdId.getValue() == 1);
        assertTrue(listener.getReplaceSent() == 1);

        // The second message shall contain the other two items
        cmdTag = new StringBuffer();
        done = handler.getNextCommand(0, listener, cmdTag, cmdId);
        Log.debug(cmdTag.toString());
        assertTrue(done);
        assertTrue(cmdId.getValue() == 2);
        assertTrue(listener.getReplaceSent() == 3);
    }


    private void fillContent(byte content[], char filler) {
        for(int i=0;i<content.length;++i) {
            content[i] = (byte)filler;
        }
    }
}

