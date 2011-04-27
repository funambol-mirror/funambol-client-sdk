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

import com.funambol.platform.FileAdapter;
import com.funambol.sapisync.source.FileSyncSource;
import com.funambol.util.Log;

import java.util.Enumeration;
import java.util.Vector;

/**
 * Implements the AllItemsSorter interface by sorting media items through the
 * Quick Sort algorithm.
 */
public class MediaItemsSorter implements FileSyncSource.AllItemsSorter {

    private static final String TAG_LOG = "MediaItemsSorter";
    
    public Enumeration sort(Enumeration items) {
        try {
            // Prepare items to sort
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Preparing items to sort");
            }
            Vector fileItems = new Vector();
            while(items.hasMoreElements()) {
                String key = (String)items.nextElement();
                FileAdapter fa = new FileAdapter(key);
                fileItems.addElement(new FileItem(key, fa.lastModified()));
                fa.close();
            }

            // Sort the items
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Sorting items");
            }
            QuickSort sort = new QuickSort();
            sort.quicksort(fileItems, false);

            // Prepare result
            if(Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Preparing sort result");
            }
            Vector result = new Vector();
            for(int i=0; i<fileItems.size(); i++) {
                result.addElement(((FileItem)fileItems.elementAt(i)).getKey());
            }
            return result.elements();
        } catch(Throwable t) {
            Log.error(TAG_LOG, "Cannot sort items", t);
            return items;
        }
    }

    private class FileItem implements ComparableI {

        private String key = null;
        private long lastModified = 0;
        
        public FileItem(String key, long lastModified) {
            this.key = key;
            this.lastModified = lastModified;
        }

        public String getKey() {
            return key;
        }

        public long getLastModified() {
            return lastModified;
        }

        public int compareTo(Object item) {
            if((getLastModified() -
                    ((FileItem)item).getLastModified()) < 0) {
                return -1;
            } else if((getLastModified() -
                    ((FileItem)item).getLastModified()) > 0) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    private interface ComparableI {
        public int compareTo(Object item);
    }

    /**
     * Implements the Quick Sort algorithm on a Vector of CamparableI objects
     */
    public class QuickSort {

        private void quicksort(Vector v, int left, int right, boolean ascending) {
            int i, last;
            if (left >= right) {
                return;
            }
            swap(v, left, (left+right) / 2);
            last = left;
            for (i = left+1; i <= right; i++) {
                ComparableI ic = (ComparableI)v.elementAt(i);
                ComparableI icleft = (ComparableI)v.elementAt(left);
                if (ascending && ic.compareTo(icleft) < 0 ) {
                    swap(v, ++last, i);
                }
                else if (!ascending && ic.compareTo(icleft) > 0 ) {
                    swap(v, ++last, i);
                }
            }
            swap(v, left, last);
            quicksort(v, left, last-1,ascending);
            quicksort(v, last+1, right,ascending);
        }

        private void swap(Vector v, int i, int j) {
            Object tmp = v.elementAt(i);
            v.setElementAt(v.elementAt(j),i);
            v.setElementAt(tmp,j);
	}

        public void quicksort(Vector v, boolean ascending) {
            quicksort(v, 0, v.size()-1,ascending);
        }
    }
}

