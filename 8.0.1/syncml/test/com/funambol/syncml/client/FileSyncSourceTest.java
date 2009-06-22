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

package com.funambol.syncml.client;

import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.util.Vector;
import java.util.Enumeration;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueFileStore;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncException;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import com.funambol.util.FunBasicTest;
import j2meunit.framework.*;

public class FileSyncSourceTest extends FunBasicTest {
    private StringKeyValueStore store;
    private FileSyncSource      source;
    private TestTracker         tracker;
    private String              directory;
    private SourceConfig        config;

    private class TestTracker implements ChangesTracker {

        private Vector newItems = new Vector();
        private Vector delItems = new Vector();
        private Vector updItems = new Vector();
        private Vector allItems = new Vector();

        public TestTracker() {
        }

        public void setSyncSource(TrackableSyncSource ss) {
        }

        public void begin() throws TrackerException {
        }

        public void end() throws TrackerException {
        }

        public Enumeration getNewItems() throws TrackerException {
            return newItems.elements();
        }

        public int getNewItemsCount() throws TrackerException {
            return newItems.size();
        }

        public Enumeration getUpdatedItems() throws TrackerException {
            return updItems.elements();
        }

        public int getUpdatedItemsCount() throws TrackerException {
            return updItems.size();
        }

        public Enumeration getDeletedItems() throws TrackerException {
            return delItems.elements();
        }

        public int getDeletedItemsCount() throws TrackerException {
            return delItems.size();
        }

        public void setItemStatus(String key, int status) throws TrackerException  {
        }

        public void reset() throws TrackerException {
        }

        public boolean removeItem(SyncItem item) throws TrackerException {
            return false;
        }

        public void addNewItem(SyncItem item) {
            newItems.addElement(item);
        }

        public void addUpdItem(SyncItem item) {
            updItems.addElement(item);
        }

        public void addDelItem(SyncItem item) {
            delItems.addElement(item);
        }

        public void addItem(SyncItem item) {
            allItems.addElement(item);
        }
    }

    public FileSyncSourceTest() throws Exception {
        super(3, "FileSyncSourceTest");

        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.TRACE);

        directory = "file:///root1";
    }

    public void setUp() {
        directory = "file:///root1";
    }

    private void prepareTestDirectory(String baseDir, String testDir) throws IOException {

        FileConnection fc = (FileConnection) Connector.open(baseDir + '/' + testDir,
                                                            Connector.READ_WRITE);
        if (fc.exists()) {
            Enumeration files = fc.list();
            while(files.hasMoreElements()) {
                String file = (String)files.nextElement();
                String fullFile = baseDir + '/' + testDir + '/' + file;
                FileConnection fc1 = (FileConnection) Connector.open(fullFile,
                                                                     Connector.READ_WRITE);
                try {
                    fc1.delete();
                } catch (Exception e) {
                    Log.error("Exception while deleting: " + fullFile + " -- " + e.toString());
                }
                fc1.close();
            }
        } else {
            fc.mkdir();
        }
        fc.close();

        directory = baseDir + '/' + testDir + '/';
        tracker = new TestTracker();
        config = new SourceConfig(SourceConfig.BRIEFCASE,"application/*", "briefcase");
        Log.debug("directory = " + directory);
        source = new FileSyncSource(config, tracker, directory);
    }

    public void test(int testNumber) throws Throwable {
        switch (testNumber) {
            case 0:
                testSlowSyncSimple();
                break;
            case 1:
                testFastSyncSimple();
                break;
            case 2:
                testSyncSendLargeObject();
                break;
        }
    }

    private void testSlowSyncSimple() throws Throwable {

        prepareTestDirectory(directory, "test0");
        // Populate the directory with one file
        String content = "This is a small file";
        createFile("test-0.txt", content);
        tracker.addItem(new SyncItem("test-0.txt"));

        source.beginSync(SyncML.ALERT_CODE_SLOW);
        SyncItem item = source.getNextItem();
        assertTrue(item != null);
        assertTrue(source.getNextItem() == null);
        byte itemContent[] = item.getContent();
        assertTrue(itemContent != null);
        assertTrue(content.equals(new String(itemContent)));
        source.endSync();
    }


    private void testFastSyncSimple() throws Throwable {
    }


    private void testSyncSendLargeObject() throws Throwable {
        // Populate the directory with one file
        prepareTestDirectory(directory, "test1");
        byte content[] = new byte[26*1024];
        String fileContent = new String(content);
        createFile("test-0.txt", fileContent);
        tracker.addItem(new SyncItem("test-0.txt"));

        source.beginSync(SyncML.ALERT_CODE_SLOW);
        SyncItem chunk1 = source.getNextItem();
        assertTrue(chunk1 != null);
        assertTrue(chunk1.hasMoreData());
        SyncItem chunk2 = source.getNextItem();
        assertTrue(chunk2 != null);
        assertTrue(!chunk2.hasMoreData());
        assertTrue(source.getNextItem() == null);
        assertTrue(fileContent.equals(new String(chunk1.getContent()) + new String(chunk2.getContent())));
        source.endSync();
    }

    private void createFile(String fileName, String content) throws Throwable {
        FileConnection fc = (FileConnection) Connector.open(directory + "/" + fileName,
                                                            Connector.READ_WRITE);
        OutputStream os;
        if (!fc.exists()) {
            fc.create();
            os = fc.openOutputStream();
        } else {
            // Truncate and open
            fc.truncate(0);
            os = fc.openOutputStream();
        }
        os.write(content.getBytes());
        os.close();
        fc.close();
    }
}

