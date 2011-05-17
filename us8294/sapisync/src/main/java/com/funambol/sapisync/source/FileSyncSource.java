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
import com.funambol.sync.SyncException;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.ResumableSource;
import com.funambol.sync.SyncSource;

import com.funambol.platform.FileAdapter;
import com.funambol.org.json.me.JSONException;
import com.funambol.util.Log;
import java.util.Hashtable;


public class FileSyncSource extends JSONSyncSource implements
        TwinDetectionSource, ResumableSource {

    private static final String TAG_LOG = "FileSyncSource";

    protected String directory;
    protected String tempDirectory;
    protected String extensions[] = {};

    private int totalItemsCount = -1;

    private AllItemsSorter itemsSorter = null;

    private long maxItemSize;
    private long oldestItemTimestamp;

    public static final long NO_LIMIT_ON_ITEM_SIZE = 0;
    public static final long NO_LIMIT_ON_ITEM_AGE = 0;

    /**
     * This is a cache used to keep track of the file items metadata, in order
     * to read them from the file system once.
     */
    private Hashtable fileItemsMetadata = null;

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
    public FileSyncSource(SourceConfig config, ChangesTracker tracker, String directory,
                          String tempDirectory, long maxItemSize, long oldestItemTimestamp)
    {
        super(config, tracker);
        this.directory = directory;
        this.tempDirectory = tempDirectory;
        this.maxItemSize = maxItemSize;
        this.oldestItemTimestamp = oldestItemTimestamp;
    }

    /**
     * @return the directory to synchronize
     */
    public String getDirectory() {
        return directory;
    }

    /**
     * Sets a specific AllItemsSorter that will be used by the getAllItemsKeys
     * to sort the returned items.
     * @param sorter
     */
    public void setAllItemsSorter(AllItemsSorter sorter) {
        itemsSorter = sorter;
    }

    public void beginSync(int syncMode, boolean resume) throws SyncException {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Initializing items metadata cache");
        }
        fileItemsMetadata = new Hashtable();
        if(itemsSorter != null) {
            itemsSorter.setItemsMetadata(fileItemsMetadata);
        }
        super.beginSync(syncMode, resume);
    }

    public void endSync() throws SyncException {
        super.endSync();
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Resetting items metadata cache");
        }
        if(fileItemsMetadata != null) {
            fileItemsMetadata.clear();
            fileItemsMetadata = null;
        }
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
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Found file " + file);
                }
                String fullName = getFileFullName(file);
                keys.addElement(fullName);
                // We better filter by extension the counter, so we have the
                // proper number of items to be returned
                if (isSupportedExtension(file)) {
                    totalItemsCount++;
                } else {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "File not counted in total count because of its extension");
                    }
                }
            }

            Enumeration result = keys.elements();
            if(itemsSorter != null) {
                if(Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Sorting all items keys");
                }
                result = itemsSorter.sort(result, totalItemsCount, syncMode);
            }
            return result;
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
            jsonFileObject.setMimetype(getContentTypeFromFileName(fileName));

            FileSyncItem syncItem = new FileSyncItem(fileFullName, item.getKey(),
                    getConfig().getType(), item.getState(), item.getParent(),
                    jsonFileObject);

            // Set the item old key to handle renames
            if(getTracker() instanceof CacheTrackerWithRenames) {
                CacheTrackerWithRenames tracker = (CacheTrackerWithRenames)getTracker();
                if(tracker.isRenamedItem(item.getKey())) {
                    String oldKey = tracker.getRenamedFileName(item.getKey());
                    if(Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Setting item old key: " + oldKey);
                    }
                    syncItem.setOldKey(oldKey);
                    if(oldKey != null) {
                        syncItem.setItemKeyUpdated(true);
                    }
                } else {
                    syncItem.setOldKey(null);
                    syncItem.setItemKeyUpdated(false);
                }
            }

            // Check if the sync item content has been updated. 
            if(getTracker() instanceof CacheTrackerWithRenames) {
                CacheTrackerWithRenames tracker = (CacheTrackerWithRenames)getTracker();
                if(tracker.isRenamedItem(item.getKey())) {
                    boolean itemUpdated = tracker.isRenamedItemUpdated(syncItem.getOldKey(), syncItem.getKey());
                    if(Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Setting item content updated: " + itemUpdated);
                    }
                    syncItem.setItemContentUpdated(itemUpdated);
                }
            }
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

        public long getLastModified() {
            try {
                FileAdapter file = new FileAdapter(fileName);
                long lastModified = file.lastModified();
                file.close();
                return lastModified;
            } catch(IOException ex) {
                Log.error(TAG_LOG, "Failed to get file last modification time", ex);
                return -1;
            }
        }
    }

    protected int addItem(SyncItem item) throws SyncException {
        if(Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "addItem");
        }
        JSONSyncItem jsonSyncItem = (JSONSyncItem)item;
        try {
            String fullName = getFileFullName(jsonSyncItem.getContentName());

            FileAdapter tgtFile = new FileAdapter(fullName);
            if (tgtFile.exists()) {
                // This is the case where the client and the server have a file
                // with the very same name but different content. In this case
                // we rename the destination file
                fullName = createUniqueFileName(fullName);
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Changing target file name to avoid clashing " + fullName);
                }
            }
            tgtFile.close();

            item.setKey(fullName);
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "key set to:" + fullName);
            }
            // This is a new file, rename the temp file
            String sourceFileName = createTempFileName(jsonSyncItem.getContentName());
            renameTempFile(sourceFileName, fullName);

            super.addItem(item);
            return SyncSource.SUCCESS_STATUS;
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot rename temporary file", ioe);
            throw new SyncException(SyncException.CLIENT_ERROR, "Cannot rename temporary file");
        }
    }

    private String createUniqueFileName(String origFileName) throws IOException {
        // Search for the extension
        int lastPeriodIdx = origFileName.lastIndexOf('.');
        String prefix = "";
        String suffix = "";
        if (lastPeriodIdx == -1) {
            prefix = origFileName;
        } else {
            prefix = origFileName.substring(0, lastPeriodIdx);
            if (lastPeriodIdx < origFileName.length() - 1) {
                suffix = origFileName.substring(lastPeriodIdx + 1);
            }
        }
        // Search for a possible file name
        for(int i=0;i<1000;++i) {
            StringBuffer n = new StringBuffer();
            n.append(prefix).append("-").append(i).append(".").append(suffix);
            String newName = n.toString();
            FileAdapter f = new FileAdapter(newName);
            try {
                if (!f.exists()) {
                    return newName;
                }
            } finally {
                f.close();
            }
        }
        return origFileName;
    }

    protected int updateItem(SyncItem item) throws SyncException {
        if(Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "updateItem");
        }
        JSONSyncItem jsonSyncItem = (JSONSyncItem)item;
        try {
            String fullName = getFileFullName(jsonSyncItem.getContentName());
            item.setKey(fullName);
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "key set to:" + fullName);
            }
            if (jsonSyncItem.isItemKeyUpdated()) {
                // Update the tracker of the renamed item
                // Must be done before renaming the file since the rename
                // event will be notified to the tracker itself
                getTracker().removeItem(new SyncItem(jsonSyncItem.getOldKey(),
                        null, SyncItem.STATE_DELETED, null));
                getTracker().removeItem(new SyncItem(jsonSyncItem.getKey(),
                        null, SyncItem.STATE_NEW, null));
            }
            if (jsonSyncItem.isItemContentUpdated()) {
                // The new content has been downloaded into a temporary file
                String sourceFileName = createTempFileName(jsonSyncItem.getContentName());
                renameTempFile(sourceFileName, fullName);
                if (jsonSyncItem.isItemKeyUpdated()) {
                    // We shall remove the old file
                    String oldFileName = jsonSyncItem.getOldKey();
                    FileAdapter fa = new FileAdapter(oldFileName);
                    fa.delete();
                }
            } else if (jsonSyncItem.isItemKeyUpdated()) {
                // This is just a rename
                String sourceFileName = jsonSyncItem.getOldKey();
                renameTempFile(sourceFileName, fullName);
            }
            super.updateItem(item);
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
            res.append(name).append(".part__");
            return res.toString();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot create temp file name", e);
            throw new IOException("Cannot create temp file");
        }
    }

    public void setSupportedExtensions(String[] extensions) {
        this.extensions = extensions;
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
    
    /**
     * Checks if file must be filtered out
     * 
     * @param key full path of the file to check
     * @param removed specified if the given file was deleted
     */
    public boolean filterOutgoingItem(String key, boolean removed) {
        boolean filterOutItem = super.filterOutgoingItem(key, removed);
        if (filterOutItem){
            return filterOutItem;
        }

        // On removed files we cannot reason much as we lost their meta info, so
        // we just propagate their changes
        if (removed) {
            return false;
        }
        
        // As long as there's no reason to filter out this file, variable reason
        // will remain null:
        String reason = null; // if it gets a value, the item is not OK

        FileItemMetadata metadata = null;
        if(fileItemsMetadata != null && fileItemsMetadata.get(key) != null) {
            metadata = (FileItemMetadata)fileItemsMetadata.get(key);
        }
        if(metadata == null) {
            FileAdapter fa = null;
            try {
                fa = new FileAdapter(key);
                metadata = new FileItemMetadata(fa.getSize(),
                            fa.lastModified(), fa.isHidden());
                if(fileItemsMetadata != null) {
                    fileItemsMetadata.put(key, metadata);
                }
            } catch(IOException ex) {
                Log.error(TAG_LOG, "Cannot read file: " + key, ex);
            } finally {
                if(fa != null) {
                    try {
                        fa.close();
                    } catch(Exception ex) { }
                }
            }
        }

        if(metadata != null) {
            if (metadata.isHidden()) {
                reason = "it is hidden";
            } else if (isOutsideSizeOrDateRange(metadata.getSize(), metadata.getLastModified())) {
                reason = "it is too large or too old";
            }
        } else {
            Log.error(TAG_LOG, "Cannot check file metadata" + key);
        }
        
        // Filter by extension
        if (reason == null && extensions != null && extensions.length > 0) {
            if (!isSupportedExtension(key)) {
                reason = "its extension is not accepted";
            }
        }
        if (reason != null) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Filtering file " + key + " because " + reason);
            }
            return true;
        } else {
            return false;
        }
    }

    /**
     * Check if a filename has an extension which is supported (belongs to the
     * source supported extensions)
     */
    public boolean isSupportedExtension(String name) {
        // If there are no valid extensions defined, then the source does not
        // apply any filter
        if (extensions == null || extensions.length == 0) {
            return true;
        }
        name = name.toLowerCase();
        for(int i=0;i<extensions.length;++i) {
            String ext = extensions[i].toLowerCase();
            if (name.endsWith(ext)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Analyzes the item and searches if it must be filtered out
     * (i.e. size too big, content not supported etc)
     *
     * Used by {@link FileSyncSource} and by {@link MediaSyncSource}
     *
     * @return true if the item must be filtered out, otherwise false
     */
    protected boolean isOutsideSizeOrDateRange(long itemSize, long lastModifiedTimestamp) {
        if ((maxItemSize != NO_LIMIT_ON_ITEM_SIZE) &&
                (itemSize > maxItemSize)) {
            return true;
        }

        // In the first sync we do not filter by timestamp because in the first
        // sync we send a fixed number of items
        if (syncMode != SyncSource.FULL_SYNC && syncMode != SyncSource.FULL_UPLOAD) {
            if ((getOldestItemTimestamp() != NO_LIMIT_ON_ITEM_AGE) &&
                    (lastModifiedTimestamp < getOldestItemTimestamp())) {
                return true;
            }
        }
        return false;
    }

    /**
     * @return the oldestItemTimestamp
     */
    public long getOldestItemTimestamp() {
        return oldestItemTimestamp;
    }

    /**
     * Generally called when a source configuration changes
     *
     * @param value the oldestItemTimestamp to set
     */
    public void setOldestItemTimestamp(long value) {
        this.oldestItemTimestamp = value;
    }

    private String getContentTypeFromFileName(String fileName) {
        int start = fileName.indexOf('.');
        String extension = fileName.substring(start+1);
        if(extension.equalsIgnoreCase("jpg") ||
                extension.equalsIgnoreCase("jpeg") ||
                extension.equalsIgnoreCase("jpe")) {
            return "image/jpeg";
        } else if(extension.equalsIgnoreCase("gif")) {
            return "image/gif";
        } else if(extension.equalsIgnoreCase("png")) {
            return "image/png";
        } else if(extension.equalsIgnoreCase("svg")) {
            return "image/svg+xml";
        } else if(extension.equalsIgnoreCase("3gp")) {
            return "video/3gpp";
        } else if(extension.equalsIgnoreCase("mp4")) {
            return "video/mp4";
        } else if(extension.equalsIgnoreCase("avi")) {
            return "video/avi";
        } else {
            // generic mime type for a file
            return "application/octet-stream";
        }
    }

    /**
     * Can be used to define a sorter to be used in the getAllItemsKeys method
     */
    public interface AllItemsSorter {

        public Enumeration sort(Enumeration items, int totalItemsCount, int syncMode);

        public void setItemsMetadata(Hashtable itemsMetadata);
    }

}

