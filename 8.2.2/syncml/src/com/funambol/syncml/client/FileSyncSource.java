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
import java.util.Date;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncException;
import com.funambol.syncml.spds.SyncListener;

import com.funambol.syncml.protocol.SyncFilter;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncMLStatus;

import com.funambol.platform.FileAdapter;

import com.funambol.util.Log;
import com.funambol.util.Base64;
import com.funambol.util.DateUtil;
import com.funambol.util.StringUtil;
import com.funambol.util.XmlUtil;


/**
 * An implementation of TrackableSyncSource, providing
 * the ability to sync briefcases (files). The source can handle both raw files
 * and OMA files (file objects). By default the source formats items according
 * to the OMA file object spec, but it is capable of receiving also raw files,
 * if their MIME type is not OMA file objects.
 */
public class FileSyncSource extends TrackableSyncSource {

    protected class FileSyncItem extends SyncItem {
        private String fileName;
        private FileAdapter file;
        private OutputStream os = null;
        private String prologue;
        private String epilogue;

        public FileSyncItem(String fileName, String key) throws IOException {
            this(fileName, key, null, SyncItem.STATE_NEW, null);
        }

        public FileSyncItem(String fileName, String key, String type, char state,
                            String parent) throws IOException {

            super(key, type, state, parent);
            this.fileName = fileName;
            this.file = new FileAdapter(fileName);

            if (SourceConfig.FILE_OBJECT_TYPE.equals(getType())) {
                // Initialize the prologue
                FileObject fo = new FileObject();
                fo.setName(file.getName());
                fo.setModified(new Date(file.lastModified()));
                prologue = fo.formatPrologue();
                // Initialize the epilogue
                epilogue = fo.formatEpilogue();
                // Compute the size of the FileObject
                int bodySize = Base64.computeEncodedSize((int)file.getSize());
                // Set the size
                setObjectSize(prologue.length() + bodySize + epilogue.length());
            } else {
                // The size is the raw file size
                setObjectSize(file.getSize());
            }
        }

        /**
         * Creates a new output stream to write to. If the item type is
         * FileDataObject, then the output stream takes care of parsing the XML
         * part of the object and it fills a FileObject that can be retrieved
         * later. @see FileObjectOutputStream for more details
         * Note that the output stream is unique, so that is can be reused
         * across different syncml messages.
         */
        public OutputStream getOutputStream() throws IOException {
            if (os == null) {
                os = file.openOutputStream();
                // If this item is a file object, we shall use the
                // FileObjectOutputStream
                if (SourceConfig.FILE_OBJECT_TYPE.equals(getType())) {
                    FileObject fo = new FileObject();
                    os = new FileObjectOutputStream(fo, os);
                }
            }
            return os;
        }

        /**
         * Creates a new input stream to read from. If the source is configured
         * to handle File Data Object, then the stream returns the XML
         * description of the file. @see FileObjectInputStream for more details.
         */
        public InputStream getInputStream() throws IOException {
            InputStream is = file.openInputStream();
            // If this item is a file object, we shall use the
            // FileObjectOutputStream
            if (SourceConfig.FILE_OBJECT_TYPE.equals(getType())) {
                is = new FileObjectInputStream(prologue, is, epilogue,
                                               (int)file.getSize());
            }
            return is;
        }

        public FileAdapter getFile() {
            return file;
        }

        // If we do not reimplement the getContent, it will return a null
        // content, but this is not used in the ss, so there's no need to
        // redefine it
    }

    protected String directory;
    
    //------------------------------------------------------------- Constructors

    /**
     * FileSyncSource constructor: initialize source config
     */
    public FileSyncSource(SourceConfig config, ChangesTracker tracker, String directory) {

        super(config, tracker);
        this.directory = directory;
        // Set up the tracker
        this.tracker = tracker;
        tracker.setSyncSource(this);
    }

    public void beginSync(int syncMode) throws SyncException {
        super.beginSync(syncMode);
    }

    protected Enumeration getAllItemsKeys() throws SyncException {
        Log.trace("[FileSyncSource.getAllItemsKeys]");
        // Scan the briefcase directory and return all keys
        try {
            FileAdapter dir = new FileAdapter(directory);
            Enumeration files = dir.list(false);
            dir.close();
            // We use the full file name as key, so we need to scan all the
            // items and prepend the directory
            Vector keys = new Vector();
            while(files.hasMoreElements()) {
                String file = (String)files.nextElement();
                keys.addElement(directory + file);
            }
            return keys.elements();
        }
        catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    /**
     * Add an item to the local store. The item has already been received and
     * the content written into the output stream. The purpose of this method
     * is to simply apply the file object meta data properties to the file used
     * to store the output stream. In particular we set the proper name and
     * modification timestamp.
     *
     * @param item the received item
     * @throws SyncException if an error occurs while applying the file
     * attributes
     * 
     */
    public int addItem(SyncItem item) throws SyncException {
        String key = item.getKey();

        Log.debug("[FileSyncSource.addItem] " + key);

        // Update the item's key
        item.setKey(directory + key);

        // The stream has already been written, but we may need to rename the
        // underlying file, according to the FileObject metadata
        if (item instanceof FileSyncItem) {
            FileSyncItem fsi = (FileSyncItem)item;
            try {
                OutputStream os = item.getOutputStream();
                if (os instanceof FileObjectOutputStream) {
                    FileObjectOutputStream foos = (FileObjectOutputStream)os;
                    applyFileObjectProperties(fsi, foos);
                    // The key for this item must be updated with the real
                    // file name
                    FileObject fo = foos.getFileObject();
                    String newName = fo.getName();
                    // The name is mandatory, but we try to be more robust here
                    // and deal with items with no name
                    if (newName != null) {
                        item.setKey(directory + newName);
                    }
                }
                fsi.getFile().close();
                return SyncMLStatus.SUCCESS;
            } catch (Exception e) {
                Log.error("[FileSyncSource.addItem] Failed at applying file object properties");
                return SyncMLStatus.GENERIC_ERROR;
            }
        } else {
            return SyncMLStatus.GENERIC_ERROR;
        }
    }

    /**
     * Update an item in the local store. The item has already been received and
     * the content written into the output stream. The purpose of this method
     * is to simply apply the file object meta data properties to the file used
     * to store the output stream. In particular we set the proper name and
     * modification timestamp.
     *
     * @param item the received item
     * @throws SyncException if an error occurs while applying the file
     * attributes
     * 
     */
    public int updateItem(SyncItem item) throws SyncException {
        String key = item.getKey();

        Log.debug("[FileSyncSource.updateItem] " + key);

        // The stream has already been written, but we may need to rename the
        // underlying file, according to the FileObject metadata
        if (item instanceof FileSyncItem) {
            FileSyncItem fsi = (FileSyncItem)item;
            try {
                OutputStream os = item.getOutputStream();
                if (os instanceof FileObjectOutputStream) {
                    FileObjectOutputStream foos = (FileObjectOutputStream)os;
                    applyFileObjectProperties(fsi, foos);
                }
                fsi.getFile().close();
                return SyncMLStatus.SUCCESS;
            } catch (Exception e) {
                Log.error("[FileSyncSource.addItem] Failed at applying file object properties");
                return SyncMLStatus.GENERIC_ERROR;
            }
        } else {
            return SyncMLStatus.GENERIC_ERROR;
        }
    }

    /**
     * Delete an item from the local store.
     * @param key the item key
     * @throws SyncException if the operation fails for any reason
     */
    public int deleteItem(String key) throws SyncException {
        Log.debug("[FileSyncSource.deleteItem] " + key);

        String fileName = key;
        try {
            FileAdapter file = new FileAdapter(fileName);
            file.delete();
            return SyncMLStatus.SUCCESS;
        } catch (IOException ioe) {
            Log.error("[FileSyncSource.deleteItem] Cannot delete item " + fileName);
            return SyncMLStatus.GENERIC_ERROR;
        }
    }

    /**
     * TODO: is this still needed?
     * This is still kind of strange, we don't really need to get the item
     * content any longer but we just need to create a proper item from which
     * the content can be read
     */
    protected SyncItem getItemContent(final SyncItem item) throws SyncException {
        SourceConfig config = getConfig();
        String type = config.getType();
        // We send the item with the type of the SS
        String fileName = item.getKey();
        try {
            FileSyncItem fsi = new FileSyncItem(fileName, item.getKey(), type, item.getState(),
                                                item.getParent());
            return fsi;
        } catch (IOException ioe) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Cannot create FileSyncItem: " + ioe.toString());
        }
    }

    public SyncItem createSyncItem(String key, String type, char state,
                                   String parent, long size) throws SyncException {

        String fileName = directory + key;
        try {
            FileSyncItem item = new FileSyncItem(fileName, key, type, state, parent);
            return item;
        } catch (IOException ioe) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Cannot create FileSyncItem: " + ioe.toString());
        }
    }

    protected void applyFileObjectProperties(FileSyncItem fsi,
                                             FileObjectOutputStream foos) throws IOException
    {
        FileObject fo = foos.getFileObject();
        String newName = fo.getName();
        FileAdapter file = fsi.getFile();
        if (newName != null) {
            String oldName = file.getName();
            // Rename the file
            file.rename(directory + newName);
        } else {
            Log.error("The received item does not have a valid name.");
        }
        // Apply the modified date if present
        FileAdapter newFile = new FileAdapter(directory + newName);
        if (newFile != null) {
            Date lastModified = fo.getModified();
            if (newFile.isSetLastModifiedSupported() && lastModified != null) {
                newFile.setLastModified(lastModified.getTime());
            }
            newFile.close();
        }
    }
}

