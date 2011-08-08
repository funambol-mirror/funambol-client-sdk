/*
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

package com.funambol.sync.client;

import com.funambol.util.Log;

/**
 * This class represents the criteria used to limit the local storage space on a
 * device.
 */
public abstract class StorageLimit {

    protected static final String TAG_LOG = "StorageLimit";

    public void check(long size, String path, long currentlyAvailableBlocks, long totalUsableBlocks, int blockSize) 
    throws StorageLimitException {
        
        
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, currentSituation(path, currentlyAvailableBlocks, totalUsableBlocks, blockSize)); 
            Log.debug(TAG_LOG, criterion(totalUsableBlocks, blockSize) + " according to the current threshold (" + 
                    toString() + ")");
        }
        
        if (isOK(size, currentlyAvailableBlocks, totalUsableBlocks, blockSize)) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "There is enough storage space on " + path +
                        " for an item sized " + size + " bytes");
            }
            return;
            
        } else {
            if (Log.isLoggable(Log.ERROR)) {
                Log.error(TAG_LOG, "There is NOT enough storage space on " + path +
                        " for an item sized " + size + " bytes");
            }
            throw new StorageLimitException(size, this);
        }
    }
    
    /**
     * Returns a textual description of the current situation on the device at a
     * given path. This will be used for log purposes.
     * The method can be overridden if only a subset of the information is relevant
     * for a certain subtype of StorageLimit.
     * 
     * @param path the path where the media should be downloaded
     * @param currentlyAvailableBlocks memory blocks available
     * @param totalUsableBlocks total memory blocks that can be used by applications
     * @param blockSize size of an individual block
     * @return a sentence starting with a capital letter and ending with no punctuation
     */
    protected static String currentSituation(String path, long currentlyAvailableBlocks, long totalUsableBlocks, int blockSize) {
        return "There are currently " + currentlyAvailableBlocks + " available memory blocks" +
               " (" + currentlyAvailableBlocks * blockSize + " bytes)" +
               " on " + path +
               " out of " + totalUsableBlocks + " total blocks" + 
               " (" + totalUsableBlocks * blockSize + " bytes)" +
               " that can be used by applications";
    }
    
    /**
     * Returns a textual description of the criterion represented by this storage limit.
     * This will be used for log purposes.
     * The implementations of this method will ignore one or both arguments if they are
     * not relevant for a certain subtype of StorageLimit.
     * 
     * @param totalUsableBlocks total memory blocks that can be used by applications
     * @param blockSize size of an individual block
     * @return a sentence starting with a capital letter and ending with no punctuation
     */
    protected abstract String criterion(long totalUsableBlocks, int blockSize);
    
    /**
     * Checks whether downloading this item would violate the storage limit.
     * 
     * @param size the item's size
     * @param currentlyAvailableBlocks memory blocks available
     * @param totalUsableBlocks total memory blocks that can be used by applications
     * @param blockSize size of an individual block
     * @return true only if the item 
     */
    protected abstract boolean isOK(long size, long currentlyAvailableBlocks, long totalUsableBlocks, int blockSize);
    
    public abstract String toString();
}
