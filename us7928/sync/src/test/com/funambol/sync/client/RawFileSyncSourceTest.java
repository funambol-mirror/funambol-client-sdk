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

package com.funambol.sync.client;

import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;
import java.util.Vector;
import java.util.Enumeration;

import com.funambol.platform.FileAdapter;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueFileStore;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncSource;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Base64;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

import junit.framework.*;

public class RawFileSyncSourceTest extends TestCase {
    private StringKeyValueStore store;
    private RawFileSyncSource   source;
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

        public void begin(int syncMode, boolean resume) throws TrackerException {
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

        public void setItemsStatus(Vector items) throws TrackerException  {
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

        public void empty() throws TrackerException {
        }

        public boolean supportsResume() {
            return false;
        }

        public boolean hasChangedSinceLastSync(String key, long ts) {
            return true;
        }
    }

    public RawFileSyncSourceTest(String name) {
        super(name);

        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.TRACE);

        // Depending on the running platform we have different values for the
        // "root" directory that we want to use
        directory = System.getProperty("java.io.tmpdir");
        if (directory == null) {
            directory = "file:///root1";
        }
    }

    public void setUp() {
    }

    private void prepareTestDirectory(String baseDir, String testDir) throws IOException {

        FileAdapter fa = new FileAdapter(baseDir + '/' + testDir);
        if (fa.exists()) {
            Enumeration files = fa.list(false);
            while(files.hasMoreElements()) {
                String file = (String)files.nextElement();
                String fullFile = baseDir + '/' + testDir + '/' + file;
                FileAdapter fa1 = new FileAdapter(fullFile);
                try {
                    fa1.delete();
                } catch (Exception e) {
                    Log.error("Exception while deleting: " + fullFile + " -- " + e.toString());
                }
                fa1.close();
            }
        } else {
            fa.mkdir();
        }
        fa.close();

        directory = baseDir + '/' + testDir + '/';
        tracker = new TestTracker();
        config = new SourceConfig(SourceConfig.BRIEFCASE,SourceConfig.FILE_OBJECT_TYPE , "briefcase");
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug("directory = " + directory);
        }
        source = new RawFileSyncSource(config, tracker, directory);
    }

    public void testSlowSyncSimple() throws Throwable {

        prepareTestDirectory(directory, "test0");
        // Populate the directory with one file
        String content = "This is a small file";
        createFile("test-0.txt", content);
        tracker.addItem(new SyncItem("test-0.txt"));

        source.beginSync(SyncSource.FULL_SYNC, false);
        SyncItem item = source.getNextItem();
        assertTrue(item != null);
        assertTrue(source.getNextItem() == null);

        InputStream is = item.getInputStream();
        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        int value = is.read();
        while (value != -1) {
            bos.write(value);
            value = is.read();
        }
        String itemContent = bos.toString();

        // The date formatted depends on the current date, so we do not consider
        // it in the comparison
        String item1 = content;
        String item2 = itemContent;
        assertTrue(content.equals(itemContent));
        source.endSync();
    }

    public void testRound1() throws Throwable {

        if (Log.isLoggable(Log.INFO)) {
            Log.info("################# RawFileSyncSourceTest.testRound1");
        }
        prepareTestDirectory(directory, "test0");
        // Populate the directory with one file
        String content = "This is a small file";
        createFile("test-0.txt", content);
        tracker.addItem(new SyncItem("test-0.txt"));

        source.beginSync(SyncSource.FULL_SYNC, false);
        SyncItem item = source.getNextItem();
        assertTrue(item != null);
        assertTrue(source.getNextItem() == null);

        InputStream is = item.getInputStream();
        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        int value = is.read();
        while (value != -1) {
            bos.write(value);
            value = is.read();
        }
        String itemContent = bos.toString();

        source.endSync();

        // Now start another sync and add this item
        FileAdapter oldFile = new FileAdapter(directory+"test-0.txt");
        try {
            oldFile.delete();
        } catch (Exception e) {
            Log.error("Exception while deleting: " + directory+"test-0.txt" + " -- " + e.toString());
        }
        
        source.beginSync(SyncSource.INCREMENTAL_SYNC, false);
        SyncItem sitem = source.createSyncItem("test-0.txt", SourceConfig.BRIEFCASE_TYPE,
                                              SyncItem.STATE_NEW,
                                              "", 0);
        OutputStream os = sitem.getOutputStream();
        os.write(itemContent.getBytes());
        os.close();
        source.addItem(sitem);
        source.endSync();
        FileAdapter newFile = new FileAdapter(directory+"test-0.txt");
        assertTrue(newFile.exists());
    }

    public void testSyncSendLargeObject() throws Throwable {
        // Populate the directory with one file
        prepareTestDirectory(directory, "test1");
        byte content[] = new byte[26*1024];
        String fileContent = new String(content);
        createFile("test-0.txt", fileContent);
        tracker.addItem(new SyncItem("test-0.txt"));

        source.beginSync(SyncSource.FULL_SYNC, false);
        SyncItem chunk1 = source.getNextItem();
        assertTrue(chunk1 != null);
        assertTrue(source.getNextItem() == null);
        source.endSync();
    }

    private void createFile(String fileName, String content) throws Throwable {
        FileAdapter fa = new FileAdapter(directory + "/" + fileName);
        OutputStream os = fa.openOutputStream(false);
        os.write(content.getBytes());
        os.close();
        fa.close();
    }
}

