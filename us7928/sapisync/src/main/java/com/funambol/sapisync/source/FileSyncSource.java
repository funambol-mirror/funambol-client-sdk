/**
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.sapisync.source;

import java.util.Enumeration;
import java.util.Vector;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;

import com.funambol.sync.SyncItem;
import com.funambol.sync.TwinDetectionSource;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.client.ChangesTracker;

import com.funambol.platform.FileAdapter;
import com.funambol.org.json.me.JSONException;
import com.funambol.sync.ResumableSource;
import com.funambol.sync.SyncSource;
import com.funambol.util.Base64;
import com.funambol.util.Log;


public class FileSyncSource extends BasicMediaSyncSource implements
        TwinDetectionSource, ResumableSource {

    private static final String TAG_LOG = "FileSyncSource";

    protected String directory;
    protected String tempDirectory;
    protected String extensions[] = {};

    private int totalItemsCount = -1;
    
    //------------------------------------------------------------- Constructors

    /**
     * FileSyncSource constructor: initialize source config
     * 
     * @param config
     * @param tracker
     * @param directory the directory being synchronized
     * @param tempDirectory the directory holding temporary files being
     * downloaded
     * @param maxItemSize max allowed size for item when upload. Must be
     *   specified in bytes and {@link BasicMediaSyncSource#NO_LIMIT_ON_ITEM_SIZE}
     *   could be used to remove this filter
     * @param oldestItemTimestamp items older that this timestamp will no be
     *   uploaded. {@link BasicMediaSyncSource#NO_LIMIT_ON_ITEM_AGE} could
     *   be used to remove this filter
     */
    public FileSyncSource(SourceConfig config,
            ChangesTracker tracker, String directory,
            String tempDirectory,
            long maxItemSize,
            long oldestItemTimestamp) {
        super(config, tracker, maxItemSize, oldestItemTimestamp);
        this.directory = directory;
        this.tempDirectory = tempDirectory;
    }

    /**
     * @return the directory to synchronize
     */
    public String getDirectory() {
        return directory;
    }

    /**
     * Twin detection implementation
     * @param item
     * @return the twin sync item, whose key is the LUID
     */
    public SyncItem findTwin(SyncItem item) {

        if(item instanceof JSONSyncItem) {

            JSONFileObject json = ((JSONSyncItem)item).getJSONFileObject();
            String fileName = json.getName();
            String fullName = getFileFullName(fileName);

            // Does this existing in our directory?
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Checking for twin for: " + fileName);
            }
            FileAdapter fa = null;
            try {
                fa = new FileAdapter(fullName);
                if (fa.exists() && fa.getSize() == json.getSize()) {
                    if (Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Twin found");
                    }
                    item.setKey(fullName);
                    return item;
                }
            } catch (Throwable t) {
                Log.error(TAG_LOG, "Cannot check for twins", t);
            } finally {
                if (fa != null) {
                    try {
                        fa.close();
                    } catch (IOException ioe) {
                    }
                }
            }
        }
        // No twin found
        return null;
    }

    public void setTempDirectory(String directory) {
        tempDirectory = directory;
    }

    protected Enumeration getAllItemsKeys() throws SyncException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getAllItemsKeys");
        }
        totalItemsCount = 0;
        // Scan the briefcase directory and return all keys
        try {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "directory: " + directory);
            }
            FileAdapter dir = new FileAdapter(directory, true);
            Enumeration files = dir.list(false, false /* Filters hidden files */);
            dir.close();
            // We use the full file name as key, so we need to scan all the
            // items and prepend the directory
            Vector keys = new Vector();

            while(files.hasMoreElements()) {
                String file = (String)files.nextElement();
                if (!isFileFilteredOut(file)) {
                    String fullName = getFileFullName(file);
                    keys.addElement(fullName);
                    totalItemsCount++;
                }
            }
            return keys.elements();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot get list of files", e);
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    /**
     * Returns the total items count. Please make sure to call getAllItemsKeys
     * before.
     * 
     * @return
     * @throws SyncException
     */
    protected int getAllItemsCount() throws SyncException {
        return totalItemsCount;
    }

    protected String getFileNameFromKey(String key) {
        String fileName = key.substring(key.lastIndexOf('/')+1);
        return fileName;
    }

    protected SyncItem getItemContent(SyncItem item) throws SyncException {
        FileAdapter file = null;
        try {
            String fileFullName = item.getKey();
            String fileName = getFileNameFromKey(fileFullName);
            file = new FileAdapter(fileFullName);

            long size     = file.getSize();
            long modified = file.lastModified();

            JSONFileObject jsonFileObject = new JSONFileObject();
            jsonFileObject.setName(fileName);
            jsonFileObject.setSize(size);
            jsonFileObject.setCreationdate(modified);
            jsonFileObject.setLastModifiedDate(modified);
            jsonFileObject.setMimetype("application/octet-stream");

            FileSyncItem syncItem = new FileSyncItem(fileFullName, item.getKey(),
                    getConfig().getType(), item.getState(), item.getParent(),
                    jsonFileObject);

            return syncItem;
            
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR,
                                    "Cannot create SyncItem: " + e.toString());
        } finally {
            try {
                if(file != null) {
                    file.close();
                }
            } catch(IOException ex) { }
        }
    }

    private class FileSyncItem extends JSONSyncItem {

        private String fileName;
        
        public FileSyncItem(String fileName, String key, String type, 
                char state, String parent, JSONFileObject jsonFileObject)
                throws JSONException {
            super(key, type, state, parent, jsonFileObject);
            this.fileName = fileName;
        }

        public OutputStream getOutputStream() throws IOException {
            FileAdapter file = new FileAdapter(fileName);
            OutputStream os = file.openOutputStream();
            file.close();
            return os;
        }

        public InputStream getInputStream() throws IOException {
            FileAdapter file = new FileAdapter(fileName);
            InputStream is = file.openInputStream();
            file.close();
            return is;
        }

        public long getObjectSize() {
            try {
                FileAdapter file = new FileAdapter(fileName);
                long size = file.getSize();
                file.close();
                return size;
            } catch(IOException ex) {
                Log.error(TAG_LOG, "Failed to read file size", ex);
                return 0;
            }
        }
    }

    protected int addItem(SyncItem item) throws SyncException {
        if(Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "addItem");
        }
        JSONSyncItem jsonSyncItem = (JSONSyncItem)item;

        try {
            // Move the file from the temporary directory to the final one
            String tempFileName = createTempFileName(jsonSyncItem.getContentName());
            String fullName = getFileFullName(jsonSyncItem.getContentName());
            renameTempFile(tempFileName, fullName);
            // Set the item key
            item.setKey(fullName);
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "key set to:" + fullName);
            }
            return SyncSource.SUCCESS_STATUS;
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot rename temporary file", ioe);
            throw new SyncException(SyncException.CLIENT_ERROR, "Cannot rename temporary file");
        }
    }

    protected void renameTempFile(String tempFileName, String fullName) throws IOException {
        // Move the file from the temporary directory to the final one
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Renaming " + tempFileName + " to " + fullName);
        }
        FileAdapter tempFile = new FileAdapter(tempFileName);
        tempFile.rename(fullName);
    }

    protected OutputStream getDownloadOutputStream(String name, long size, boolean isUpdate,
            boolean isThumbnail, boolean append) throws IOException {
        String tempFileName = createTempFileName(name);
        if(Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getDownloadOutputStream: " + tempFileName);
        }
        FileAdapter file = createTempFile(tempFileName);
        OutputStream os = file.openOutputStream(append);
        file.close();
        return os;
    }

    protected FileAdapter createTempFile(String tempFileName) throws IOException {
        FileAdapter file = new FileAdapter(tempFileName);
        return file;
    }

    /**
     * Delete an item from the local store.
     * @param key the item key
     * @throws SyncException if the operation fails for any reason
     */
    public int deleteItem(String key) throws SyncException {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Deleting item " + key);
        }
        FileAdapter fa = null;
        try {
            fa = new FileAdapter(key);
            if (fa.exists()) {
                fa.delete();
            }
            return SyncSource.SUCCESS_STATUS;
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot delete item", e);
            return SyncSource.ERROR_STATUS;
        } finally {
            if(fa != null) {
                try {
                    fa.close();
                } catch(IOException ex) {}
            }
        }
    }

    protected void deleteAllItems() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "removeAllItems");
        }
        // Scan the briefcase directory and return all keys
        try {
            FileAdapter dir = new FileAdapter(directory);
            Enumeration files = dir.list(false);
            dir.close();
            // We use the full file name as key, so we need to scan all the
            // items and prepend the directory
            while(files.hasMoreElements()) {
                String fileName = (String)files.nextElement();
                String fullName = getFileFullName(fileName);
                FileAdapter file = new FileAdapter(fullName);
                file.delete();
                file.close();
            }
            //at the end, empty the tracker
            tracker.reset();
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    protected String createTempFileName(String name) throws IOException {
        try {
            StringBuffer res = new StringBuffer(tempDirectory);
            if (!tempDirectory.endsWith("/")) {
                res.append("/");
            }
            res.append(new String(Base64.encode(name.getBytes("UTF-8"))));
            return res.toString();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot create temp file name", e);
            throw new IOException("Cannot create temp file");
        }
    }



    public void setSupportedExtensions(String[] extensions) {
        this.extensions = extensions;
    }

    /**
     * Return whether a given file is filtered out by the SyncSource.
     * @param filename
     * @return true if the file is not OK, false if the file is OK
     */
    public boolean isFileFilteredOut(String name) {
        
        String fullName = getFileFullName(name);
        // As long as there's no reason to filter out this file, variable reason
        // will remain null:
        String reason = null; // if it gets a value, the item is not OK
        FileAdapter file = null;
        try {
            // Filter hidden files
            file = new FileAdapter(fullName);
            if (file.isHidden()) {
                reason = "it is hidden";
            } else {
                // Filter files according to standard media sync criteria
                if (isItemFilteredOut(file.getSize(), file.lastModified())) {
                    reason = "it is too large or too old";
                }
            }
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Cannot check file: " + name, ex);
        } finally {
            if(file != null) {
                try {
                    file.close();
                } catch(Exception ex) { }
            }
        }
        // Filter by extension
        if (reason == null && extensions != null && extensions.length > 0) {
            reason = "its extension is not accepted";
            name = name.toLowerCase();
            boolean matchExtension = false;
            for(int i=0;i<extensions.length;++i) {
                String ext = extensions[i].toLowerCase();
                matchExtension = name.endsWith(ext);
                if(matchExtension) {
                    reason = null;
                    break;
                }
            }
        }
        if (reason != null) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Filtering file " + fullName + " because " +
                        reason);
            }
            return true;
        } else {
            return false;
        }
    }

    public String getFileFullName(String name) {
        StringBuffer fullname = new StringBuffer();
        fullname.append(directory);
        if(!directory.endsWith("/")) {
            fullname.append("/");
        }
        fullname.append(name);
        return fullname.toString();
    }

    public boolean readyToResume() {
        return true;
    }

    public boolean exists(String luid) {
        FileAdapter fa = null;
        try {
            fa = new FileAdapter(luid);
            return fa.exists();
        } catch(Throwable t) {
            return false;
        } finally {
            if(fa != null) {
                try {
                    fa.close();
                } catch(Exception ex) { }
            }
        }
    }

    // TODO: to be implemented when updates are propagated
    public boolean hasChangedSinceLastSync(String key, long lastSyncStartTime) {
        return false;
    }

    public long getPartiallyReceivedItemSize(String luid) {

        FileAdapter fa = null;
        try {
            String tempFileName = createTempFileName(getFileNameFromKey(luid));
            fa = new FileAdapter(tempFileName);
            if (!fa.exists()) {
                return -1;
            }
            return fa.getSize();
        } catch (Exception e) {
            return -1;
        } finally {
            if(fa != null) {
                try {
                    fa.close();
                } catch(Exception ex) { }
            }
        }
    }

    public String getLuid(SyncItem item) {
        JSONFileObject json = ((JSONSyncItem)item).getJSONFileObject();
        String fileName = json.getName();

        String localFullName = getFileFullName(fileName);
        return localFullName;
    }
}

