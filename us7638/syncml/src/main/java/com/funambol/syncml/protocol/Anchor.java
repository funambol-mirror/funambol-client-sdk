/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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


package com.funambol.syncml.protocol;

/**
 * Corresponds to the &lt;Anchor&gt; tag in the metainfo spec
 */

public class Anchor {

    // ------------------------------------------------------------ Private data
    private String last;
    private String next;

    // ------------------------------------------------------------ Constructors
    /** For serialization purposes */
    public Anchor() {}

    /**
     * Creates a new Anchor object
     *
     * @param last the synchronization anchor for the previous synchronization
     *             session
     * @param next the synchronization anchor for the current synchronization
     *             session - NOT NULL
     *
     */
    public Anchor(final String last, final String next) {
        setNext(next);
        setLast(last);
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Gets the last property
     *
     * @return the last property
     */
    public String getLast() {
        return last;
    }

    /**
     * Sets the last property
     *
     * @param last the last property
     *
     */
    public void setLast(String last) {
        this.last = last;
    }

    /**
     * Gets the next property
     *
     * @return the next property
     */
    public String getNext() {
        return next;
    }

    /**
     * Sets the next property
     *
     * @param next the next property
     *
     */
    public void setNext(String next) {
        this.next = next;
    }

    /**
     * Returns a string representation
     * @return a string representation
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();

        sb.append(super.toString()).append("[last=").append(last)
          .append(",next=").append(next).append(']');

        return sb.toString();
    }
}
