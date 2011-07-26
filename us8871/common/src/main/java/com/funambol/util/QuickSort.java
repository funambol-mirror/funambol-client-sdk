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

package com.funambol.util;

import java.util.Date;

import java.util.Vector;

/**
 * Implements the Quick Sort algorithm on a Vector of Camparable objects
 */
public class QuickSort {

    private Vector v2 = null;
    
    private void quicksort(Vector v, int left, int right, boolean ascending) {
        int i, last;
        if (left >= right) {
            return;
        }
        swap(v, left, (left+right) / 2);
        last = left;
        for (i = left+1; i <= right; i++) {
            int compRes = compareElements(v.elementAt(i), v.elementAt(left));
            if (ascending && compRes < 0 ) {
                swap(v, ++last, i);
            }
            else if (!ascending && compRes > 0 ) {
                swap(v, ++last, i);
            }
        }
        swap(v, left, last);
        quicksort(v, left, last-1,ascending);
        quicksort(v, last+1, right,ascending);
    }

    protected int compareElements(Object v1, Object v2) {
        // Damn microedition platforms... on these platforms we do not have a
        // standard Comparable interface. Our API defines a ComparableI interface
        // identical to the standard one. If you need to sort your own items,
        // you can make them implement Comparable (use import
        // com.funambol.util.*).
        // If on the other hand you need to sort standard classes like String,
        // Long and so on, this piece of code has some specialized code but not
        // all cases are covered

        if (v1 instanceof ComparableI && v2 instanceof ComparableI) {
            ComparableI ic = (ComparableI)v1;
            ComparableI icleft = (ComparableI)v2;
            return ic.compareTo(icleft);
        } else if (v1 instanceof String && v2 instanceof String) {
            String ic = (String)v1;
            String icleft = (String)v2;
            return ic.compareTo(icleft);
        } else if (v1 instanceof Long && v2 instanceof Long) {
            Long ic = (Long)v1;
            Long icleft = (Long)v2;

            long icVal = ic.longValue();
            long icleftVal = icleft.longValue();
            if (icVal == icleftVal) {
                return 0;
            } else if (icVal > icleftVal) {
                return 1;
            } else {
                return -1;
            }
        } else if (v1 instanceof Integer && v2 instanceof Integer) {
            Integer ic = (Integer)v1;
            Integer icleft = (Integer)v2;

            int icVal = ic.intValue();
            int icleftVal = icleft.intValue();
            if (icVal == icleftVal) {
                return 0;
            } else if (icVal > icleftVal) {
                return 1;
            } else {
                return -1;
            }
        } else if (v1 instanceof Date && v2 instanceof Date) {
            Date ic = (Date)v1;
            Date icleft = (Date)v2;
            long icVal = ic.getTime();
            long icleftVal = icleft.getTime();
            if (icVal == icleftVal) {
                return 0;
            } else if (icVal > icleftVal) {
                return 1;
            } else {
                return -1;
            }
        } else {
            throw new IllegalArgumentException("Unknwon objects to be compared");
        }
    }

    private void swap(Vector v, int i, int j) {
        swapSingle(v, i, j);
        if(v2 != null) {
            swapSingle(v2, i, j);
        }
    }

    private void swapSingle(Vector v, int i, int j) {
        Object tmp = v.elementAt(i);
        v.setElementAt(v.elementAt(j),i);
        v.setElementAt(tmp,j);
    }

    public void quicksort(Vector v, Vector v2, boolean ascending) {

        if (v.size() <= 1) {
            return;
        }
        this.v2 = v2;
        quicksort(v, 0, v.size()-1,ascending);
    }
}

