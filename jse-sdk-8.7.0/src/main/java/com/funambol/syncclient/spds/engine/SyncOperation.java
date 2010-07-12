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

package com.funambol.syncclient.spds.engine;

import java.io.*;

import java.security.Principal;

/**
 * This class represents a synchronization operation to perform on a
 * <i>SyncSource</i>.
 * <p>
 * A <i>SyncOperation</i> can represent the following actions
 * <ul>
 * <li>new
 * <li>delete
 * <li>udate
 * <li>conflict
 * <li>nop (do nothing)
 * </ul>
 *
 *
 * @version $Id: SyncOperation.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncOperation {

    public static final char NEW      = 'N';
    public static final char DELETE   = 'D';
    public static final char UPDATE   = 'U';
    public static final char CONFLICT = 'O';
    public static final char NOP      = '-';

    private SyncItem syncItem = null;
    private char operation    = 0;

    /**
     * Rturns the item associated to the operation
     *
     * @return the <i>SyncItem</i> associated to the operation
     */
    public SyncItem getSyncItem() {
        return this.syncItem;
    }

    /**
     * Returns the operation this object represents.
     *
     * @return the operation this object represents.
     */
    public char getOperation() {
        return this.operation;
    }

    /**
     * Creates a new <i>SyncOperation</i> given the <i>SyncItem</i> and the
     * operation.
     *
     * @param syncItem the syncItem
     * @param operation the operation
     */
    public SyncOperation(SyncItem  syncItem,
                             char operation) {

            this.syncItem  = syncItem;
            this.operation = operation;
    }
}
