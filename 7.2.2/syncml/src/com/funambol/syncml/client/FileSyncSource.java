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

import java.util.Enumeration;
import java.util.Vector;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncException;

import com.funambol.syncml.protocol.SyncFilter;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncMLStatus;

import com.funambol.util.Log;
import com.funambol.util.SyncListener;


/**
 * An implementation of TrackableSyncSource, providing
 * the ability sync briefcases (files)
 */
public class FileSyncSource extends TrackableSyncSource {

    private String directory;
    private LargeObject outgoingLo;
    // TEMP TODO FIXME
    private int maxMsgSize = 32 * 1024;
    
    //------------------------------------------------------------- Constructors

    /**
     * FileSyncSource constructor: initialize source config
     */
    public FileSyncSource(SourceConfig config, ChangesTracker tracker, String directory) {

        super(config, tracker);
        this.directory = directory;
        // Set up the tracker
        this.tracker = tracker;
        this.outgoingLo  = null;
        tracker.setSyncSource(this);
        config.setEncoding(this.ENCODING_B64);
    }

    public void beginSync(int syncMode) throws SyncException {
        super.beginSync(syncMode);
        // Reset LO info
        this.outgoingLo  = null;
    }

    protected Enumeration getAllItemsKeys() throws SyncException {
        Log.trace("[FileSyncSource.getAllItemsKeys]");
        // Scan the briefcase directory and return all keys
        try {
            FileConnection fc = (FileConnection) Connector.open(directory, Connector.READ);
            Enumeration files = fc.list();
            Vector fileList = new Vector();
            while(files.hasMoreElements()) {
                String file = (String)files.nextElement();
                // We must filter out directories (no recursion at the moment)
                FileConnection fc1 = (FileConnection) Connector.open(directory + file, Connector.READ);
                if (!fc1.isDirectory()) {
                    fileList.addElement(file);
                }
                fc1.close();
            }
            fc.close();
            return fileList.elements();
        }catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    public SyncItem getNextItem() throws SyncException {
        Log.trace("[FileSyncSource.getNextItem]");
        try {
            if (outgoingLo != null) {
                // A large object is being sent, fetch the next chunk
                // and terminates the lo if necessary
                SyncItem item = getNextChunk(outgoingLo, true);
                return item;
            } else {
                SyncItem item = super.getNextItem();
                // Check if this item needs to be split in chunks
                if (item != null) {
                    long totalSize = mustBeSplit(item);
                    if (totalSize != -1) {
                        String fileName = directory + item.getKey();
                        outgoingLo = new LargeObject(item.getKey(), fileName);
                        item = getNextChunk(outgoingLo, true);
                        item.setLODeclaredSize(totalSize);
                    }
                }
                return item;
            }
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    public SyncItem getNextNewItem() throws SyncException {
        Log.trace("[FileSyncSource.getNextNewItem]");
        try {
            if (outgoingLo != null) {
                // A large object is being sent, fetch the next chunk
                // and terminates the lo if necessary
                SyncItem item = getNextChunk(outgoingLo, true);
                return item;
            } else {
                SyncItem newItem = super.getNextNewItem();
                // Check if this item needs to be split in chunks
                if (newItem != null) {
                    long totalSize = mustBeSplit(newItem);
                    if (totalSize != -1) {
                        String fileName = directory + newItem.getKey();
                        outgoingLo = new LargeObject(newItem.getKey(), fileName);
                        newItem = getNextChunk(outgoingLo, true);
                        newItem.setLODeclaredSize(totalSize);
                    }
                }
                return newItem;
            }
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    public SyncItem getNextUpdatedItem() throws SyncException {
        Log.trace("[FileSyncSource.getNextUpdatedItem]");
        try {
            if (outgoingLo != null) {
                // A large object is being sent, fetch the next chunk
                // and terminates the lo if necessary
                SyncItem item = getNextChunk(outgoingLo, true);
                return item;
            } else {
                SyncItem updItem = super.getNextUpdatedItem();
                // Check if this item needs to be split in chunks
                if (updItem != null) {
                    long totalSize = mustBeSplit(updItem);
                    if (totalSize != -1) {
                        String fileName = directory + updItem.getKey();
                        outgoingLo = new LargeObject(updItem.getKey(), fileName);
                        updItem = getNextChunk(outgoingLo, true);
                        updItem.setLODeclaredSize(totalSize);
                    }
                }
                return updItem;
            }
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    private SyncItem getNextChunk(LargeObject lo, boolean getContent) throws IOException {
        Log.trace("[FileSyncSource.getNextChunk]");
        byte chunk[] = lo.getNextChunk(maxMsgSize);
        lo.setChunkContent(chunk);
        SyncItem item = new SyncItem(lo.getKey());
        if (getContent) {
            item.setContent(chunk);
        }
        item.setChunkNumber(lo.getChunkNumber());
        if (!lo.last()) {
            item.setHasMoreData();
        } else {
            // Prepare to send the next real item at the next
            // round
            outgoingLo = null;
        }
        return item;
    }

    public int addItem(SyncItem item) throws SyncException {
        return SyncMLStatus.GENERIC_ERROR;
    }

    public int updateItem(SyncItem item) throws SyncException {
        return SyncMLStatus.GENERIC_ERROR;
    }
    
    public int deleteItem(String key) throws SyncException {
        return SyncMLStatus.GENERIC_ERROR;
    }

    protected SyncItem getItemContent(final SyncItem item) throws SyncException {
        Log.debug("[FileSyncSource.getItemContent] " + item.getKey());
        // If the item size is > maxMsgSize then we split the item in chunks
        SyncItem newItem = new SyncItem(item);
        if (outgoingLo != null) {
            Log.trace("chunk number = " + item.getChunkNumber());
            Log.trace("chunk number = " + outgoingLo.getChunkNumber());
            if (item.getChunkNumber() == outgoingLo.getChunkNumber()) {
                newItem.setContent(outgoingLo.getChunkContent());
            } else {
                // TODO not supported now!!!
            }
        } else {
            try {
                byte[] content = readFile(item.getKey());
                newItem.setContent(content);
            } catch (IOException e) {
                throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
            }
        }
        return newItem;
    }

    private byte[] readFile(String fileName) throws IOException {

        FileConnection fc   = (FileConnection) Connector.open(directory + fileName, Connector.READ);
        InputStream    is   = fc.openInputStream();
        int            size = (int)fc.fileSize();
        
        byte content[] = new byte[size];
        is.read(content);
        is.close();
        fc.close();
        return content;
    }

    private long computeB64Size(long origSize) {
        long rem  = origSize % 3;
        long size;
        if (rem == 0) {
            size = 4 * (origSize / 3);
        } else {
            size = 4 * ((origSize / 3) + 1);
        }
        return size;
    }

    private long computeReverseB64Size(long desiredSize) {

        long actualSize = (desiredSize * 3) / 4;
        for(long i=0;i<3;i++) {
            if ((actualSize + i) % 3 == 0) {
                return actualSize + i;
            }
        }
        // Should never get here
        return -1;
    }

    private long mustBeSplit(SyncItem item) throws IOException {
        Log.trace("[FileSyncSource.mustBeSplit] " + item.getKey());
        String fileName = directory + item.getKey();
        FileConnection fc = (FileConnection) Connector.open(fileName, Connector.READ);
        long b64Size = computeB64Size((int)fc.fileSize());
        fc.close();
        if (b64Size > (long)maxMsgSize) {
            return b64Size;
        } else {
            return -1;
        }
    }

    private class LargeObject {
        private int offset;
        private String  fileName;
        private boolean last;
        private String  key;
        private int chunkNumber;
        private byte chunkContent[];
        private FileConnection fc;
        private InputStream is;
        private boolean initialized;

        public LargeObject(String key, String fileName) {
            this.key = key;
            this.fileName = fileName;
            this.offset   = 0;
            this.chunkNumber = -1;
            this.chunkContent = null;
            this.initialized = false;
        }

        public byte[] getNextChunk(int maxMsgSize) throws IOException {

            Log.trace("[LargeObject.getNextChunk]");
            chunkNumber++;

            if (!initialized) {
                fc = (FileConnection) Connector.open(fileName, Connector.READ);
                is = fc.openInputStream();
                initialized = true;
            }

            long fileSize = fc.fileSize();
            Log.trace("fileSize = " + fileSize);
            Log.trace("offset = " + offset);

            long rem = fileSize - offset;
            long chunkSize;
            if (computeB64Size(rem) < maxMsgSize) {
                // This is the last chunk
                last = true;
                chunkSize = rem;
            } else {
                last = false;
                // We must provide a number of bytes that are
                // multiple of 3, so that Base64 encoding can be
                // done on chunks
                chunkSize = computeReverseB64Size(maxMsgSize);
            }

            // Now read chunkSize bytes
            byte chunk[] = new byte[(int)chunkSize];
            Log.trace("read " + chunkSize);
            is.read(chunk); 
            offset += chunkSize;
            if (last) {
                is.close();
                fc.close();
            }
            return chunk;
        }

        public int getChunkNumber() {
            return chunkNumber;
        }

        public String getKey() {
            return key;
        }

        public boolean last() {
            return last;
        }

        public void setChunkContent(byte[] chunkContent) {
            this.chunkContent = chunkContent;
        }

        public byte[] getChunkContent() {
            return chunkContent;
        }

    }
}

