/**
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

package com.funambol.sapisync.source.util;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.funambol.platform.FileAdapter;
import com.funambol.sapisync.source.FileItemMetadata;
import com.funambol.sapisync.source.FileSyncSource;
import com.funambol.util.ComparableI;
import com.funambol.util.QuickSort;
import com.funambol.util.Log;


/**
 * Implements the AllItemsSorter interface by sorting media items through the
 * Quick Sort algorithm.
 */
public class MediaItemsSorter implements FileSyncSource.ItemsSorter {

    private static final String TAG_LOG = "MediaItemsSorter";

    private boolean mostRecentFirst;
    private Hashtable itemsMetadata;
    
    public MediaItemsSorter(boolean mostRecentFirst) {
        this.mostRecentFirst = mostRecentFirst;
    }

    public void setItemsMetadata(Hashtable itemsMetadata) {
        this.itemsMetadata = itemsMetadata;
    }
    
    public Enumeration sort(Enumeration items, int totalItemsCount) {
        try {
            // Sorting requires at least 2 elements
            if(totalItemsCount < 2) {
                return items;
            }
            // Prepare items to sort
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Preparing items to sort");
            }
            Vector itemsKeys = new Vector(totalItemsCount);
            Vector itemsLastModified = new Vector(totalItemsCount);
            while(items.hasMoreElements()) {
                String key = (String)items.nextElement();
                itemsKeys.addElement(key);
                FileItemMetadata metadata = null;
                if(itemsMetadata != null && itemsMetadata.get(key) != null) {
                    metadata = (FileItemMetadata)itemsMetadata.get(key);
                }
                if(metadata == null) {
                    FileAdapter fa = new FileAdapter(key);
                    if(itemsMetadata != null) {
                        metadata = new FileItemMetadata(fa.getSize(),
                                fa.lastModified(), fa.isHidden());
                        itemsMetadata.put(key, metadata);
                    }
                    fa.close();
                }
                itemsLastModified.addElement(
                        new FileItem(metadata.getLastModified()));
            }

            // Sort the items
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Sorting items");
            }
            QuickSort sort = new QuickSort();
            sort.quicksort(itemsLastModified, itemsKeys, !mostRecentFirst);

            // Prepare result
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Preparing sort result");
            }
            return itemsKeys.elements();
        } catch(Throwable t) {
            Log.error(TAG_LOG, "Cannot sort items", t);
            return items;
        }
    }

    private class FileItem implements ComparableI {

        private long lastModified = 0;

        public FileItem(long lastModified) {
            this.lastModified = lastModified;
        }

        public long getLastModified() {
            return lastModified;
        }

        public int compareTo(Object item) {
            if (getLastModified() == ((FileItem)item).getLastModified()) {
                return 0;
            } else if (getLastModified() > ((FileItem)item).getLastModified()) {
                return 1;
            } else {
                return -1;
            }
        }
    }

}

