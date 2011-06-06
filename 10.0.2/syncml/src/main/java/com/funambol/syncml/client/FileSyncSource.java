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

import com.funambol.sync.SyncItem;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncAnchor;
import com.funambol.sync.client.RawFileSyncSource;
import com.funambol.sync.client.ChangesTracker;

import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.platform.FileAdapter;
import com.funambol.util.Log;
import com.funambol.util.Base64;

/**
 * An implementation of TrackableSyncSource, providing
 * the ability to sync briefcases (files). The source can handle both raw files
 * and OMA files (file objects). By default the source formats items according
 * to the OMA file object spec, but it is capable of receiving also raw files,
 * if their MIME type is not OMA file objects.
 */
public class FileSyncSource extends RawFileSyncSource {

    private static final String TAG_LOG = "FileSyncSource";

    protected class FileSyncItem extends RawFileSyncItem {

        protected String prologue;
        protected String epilogue;

        public FileSyncItem(String fileName, String key) throws IOException {
            super(fileName, key, null, SyncItem.STATE_NEW, null);
        }

        public FileSyncItem(String fileName, String key, String type, char state,
                            String parent) throws IOException {

            super(fileName, key, type, state, parent);
            FileAdapter file = new FileAdapter(fileName);

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
            // Release the file object
            file.close();
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
                os = super.getOutputStream();
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
            FileAdapter file = new FileAdapter(fileName);
            InputStream is = super.getInputStream();
            // If this item is a file object, we shall use the
            // FileObjectOutputStream
            if (SourceConfig.FILE_OBJECT_TYPE.equals(getType())) {
                is = new FileObjectInputStream(prologue, is, epilogue,
                                               (int)file.getSize());
            }
            return is;
        }

        // If we do not reimplement the getContent, it will return a null
        // content, but this is not used in the ss, so there's no need to
        // redefine it
    }

    protected String directory;
    protected String extensions[] = {};
    
    //------------------------------------------------------------- Constructors

    /**
     * FileSyncSource constructor: initialize source config
     */
    public FileSyncSource(SourceConfig config, ChangesTracker tracker, String directory) {

        super(config, tracker, directory);
    }

    protected void applyFileProperties(FileSyncItem fsi) throws IOException {
        OutputStream os = fsi.getOutputStream();
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
                fsi.setKey(directory + newName);
            }
        }
    }

    protected void applyFileObjectProperties(FileSyncItem fsi, FileObjectOutputStream foos) throws IOException {
        FileObject fo = foos.getFileObject();
        String newName = fo.getName();
        FileAdapter file = new FileAdapter(fsi.getFileName());
        if (newName != null) {
            // Rename the file
            file.rename(directory + newName);
        } else {
            Log.error(TAG_LOG, "The received item does not have a valid name.");
        }
        file.close();
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

