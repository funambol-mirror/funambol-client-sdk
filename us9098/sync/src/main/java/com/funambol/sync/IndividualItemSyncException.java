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
 * This exception represents an exception that concerns one item whose
 * synchronization was not performed because of some special limitations on the
 * client. It was introduced for the case of items that are too large to be
 * saved on the internal memory of a BlackBerry.
 * The sync source can respond to this exception by skipping the item without
 * interrupting the sync nor the download/upload phase. In this sense it's a
 * special case of a NonBlockingSyncException because the sync can proceed; the
 * difference is that a NonBlockingSyncException is expected to stop the whole
 * download/upload phase (but not the whole sync session) and not just the
 * download/upload of one item.
 */
public class IndividualItemSyncException extends NonBlockingSyncException {

    /**
     * Constructs an instance of <code>IndividualItemSyncException</code>
     * with the specified detail message.
     *
     * @param code the error code.
     * @param msg the detail message.
     */
    public IndividualItemSyncException(int code, String msg) {
        super(code, msg);
    }

}
