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

package com.funambol.sync;

/**
 * <code>Filter</code> represents a generic filter and can have different types:
 *  <ul>
 *      <li>DATE_RECENT_TYPE: Items whose date is more recent than a given
 *      threshold</li>
 *      <li>ITEMS_COUNT_TYPE: Max number of items (without a particular
 *      order)</li>
 *      <li>DATE_WINDOW_TYPE: All the items that fall into the window</li>
 *      <li>COUNT_RECENT_TYPE: Max number of items starting from the most recent
 *      ones</li>
 *  </ul>
 */
public class Filter {

    /**
     * Items whose date is more recent than a given threshold
     */
    public static final int DATE_RECENT_TYPE = 0;

    /**
     * Max number of items (without a particular order)
     */ 
    public static final int ITEMS_COUNT_TYPE = 1;

    /**
     * All the items that fall into the window
     */
    public static final int DATE_WINDOW_TYPE = 2;

    private int type;

    private int count;
    private long date;

    private boolean enabled;

    /**
     * Creates a new Filter of the given type. This Filter will be enabled by
     * default.
     * @param type
     */
    public Filter(int type) {
        this(type, -1, -1);
    }

    public Filter(int type, long date, int count) {
        this.type = type;
        this.date = date;
        this.count = count;
        this.enabled = true;
    }

    public int getType() {
        return type;
    }

    public long getDate() {
        return date;
    }

    public void setDate(long value) {
        date = value;
    }

    public int getCount() {
        return count;
    }

    public void setCount(int value) {
        count = value;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public boolean isEnabled() {
        return enabled;
    }
}
    
