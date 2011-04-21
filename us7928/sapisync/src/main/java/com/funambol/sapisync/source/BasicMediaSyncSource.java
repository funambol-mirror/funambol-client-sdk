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

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.ChangesTracker;

/**
 * Basic sync source for all media types (pictures, files, videos etc).
 * It's abstract and all method / fields common to Android/Blackberry media
 * implementation should be put here to avoid code duplication  
 */
public abstract class BasicMediaSyncSource extends JSONSyncSource {

    private long maxItemSize;
    private long oldestItemTimestamp;

    public static final long NO_LIMIT_ON_ITEM_SIZE = 0;
    public static final long NO_LIMIT_ON_ITEM_AGE = 0;
   
    /**
     * @param config the source configuration
     * @param syncConfig the global sync configuration
     * @param tracker the changes tracker
     * @param maxItemSize the max item size used to filter out outgoing items
     * (set to NO_LIMIT_ON_ITEM_SIZE to avoid filtering)
     * @param oldestItemTimestamp the timestamp used to filter out old outgoing
     * items (set to NO_LIMIT_ON_ITEM_AGE to avoid filtering)
     */
    public BasicMediaSyncSource(
            SourceConfig config,
            ChangesTracker tracker,
            long maxItemSize,
            long oldestItemTimestamp) {
        super(config, tracker);

        this.maxItemSize = maxItemSize;
        this.oldestItemTimestamp = oldestItemTimestamp;
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


}
