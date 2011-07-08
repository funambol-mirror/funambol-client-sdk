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

package com.funambol.client.source;

import java.io.IOException;
import java.io.OutputStream;

import com.funambol.client.customization.Customization;
import com.funambol.platform.FileAdapter;
import com.funambol.platform.FileSystemInfo;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SyncException;
import com.funambol.sync.NonBlockingSyncException;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.client.StorageLimitException;
import com.funambol.sync.client.StorageLimit;
import com.funambol.sapisync.source.FileSyncSource;
import com.funambol.util.Log;


public class FunambolFileSyncSource extends FileSyncSource {

    private static final String TAG_LOG = "FunambolFileSyncSource";

    protected Customization customization;
    
    //------------------------------------------------------------- Constructors

    public FunambolFileSyncSource(SourceConfig config, ChangesTracker tracker, 
            String directory, String tempDirectory, long maxItemSize,
            long oldestItemTimestamp, Customization customization) {
        super(config, tracker, directory, tempDirectory, maxItemSize, oldestItemTimestamp);
        this.customization = customization;
    }

    public void beginSync(int syncMode, boolean resume) throws SyncException {
        // Ensure that the directory to sync exists
        String sdCardRoot = FileSystemInfo.getSDCardRoot();
        if(sdCardRoot != null) {
            if(getDirectory().startsWith(sdCardRoot) && !FileSystemInfo.isSDCardAvailable()) {
                // The directory to synchronize is on the sd card but actually
                // it is not available
                throw new SyncException(SyncException.SD_CARD_UNAVAILABLE,
                        "The sd card is not available");
            }
        }
        try {
            // Create the default folder if it doesn't exist
            FileAdapter d = new FileAdapter(getDirectory());
            if(!d.exists()) {
                d.mkdir();
            }
            d.close();
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Cannot create directory to sync: " + getDirectory(), ex);
        }
        super.beginSync(syncMode, resume);
    }
    
    /**
     * @throws a SyncException if the quota on server is reached
     */
    public void setItemStatus(String key, int status) throws SyncException {
        if (status == SyncSource.SERVER_FULL_ERROR_STATUS) {
            // The user reached his quota on the server
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Server is full");
            }
            throw new SyncException(SyncException.DEVICE_FULL, "Server is full");
        }
        super.setItemStatus(key, status);
    }

    protected OutputStream getDownloadOutputStream(String name, long size, boolean isUpdate,
            boolean isThumbnail, boolean append) throws IOException {
        try {
            grantStorageSpaceFor(tempDirectory, size); // TODO What if isUpdate is true?
        } catch (StorageLimitException sle) {
            throw sle.getCorrespondingSyncException();
        }
        return super.getDownloadOutputStream(name, size, isUpdate, isThumbnail, append);
    }

    /**
     * @throws StorageLimitException if size 
     */
    protected void grantStorageSpaceFor(String path, long size)
            throws StorageLimitException, IOException {
        StorageLimit threshold = customization.getStorageLimit();
        Log.trace(TAG_LOG, "Checking storage space before downloading item");
        FileSystemInfo fsInfo = new FileSystemInfo(path);
        threshold.check(size, path,
                fsInfo.getAvailableBlocks(),
                fsInfo.getTotalUsableBlocks(),
                fsInfo.getBlockSize());
    }
}

