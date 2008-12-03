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

package com.funambol.syncml.spds;

import com.funambol.util.CodedException;

/**
 * This exception represents the base exception for synchronization
 * related error conditions.
 *
 */
public class SyncException extends CodedException {

    // Protocol codes 

    /** Generic error caused by the client */
    public static final int CLIENT_ERROR      = 400;
    /** Authentication error from remote server */
    public static final int AUTH_ERROR        = 401;
    /** Error accessing a remote resource */
    public static final int ACCESS_ERROR      = 404;
    /** Can not open connection error */
    public static final int CONN_NOT_FOUND    = 406;
    /** Response data null */
    public static final int DATA_NULL         = 407;
    /** Malformed URL error */
    public static final int ILLEGAL_ARGUMENT  = 409;
    /** Generic server error */
    public static final int SERVER_ERROR      = 500;
    /** Server busy: another sync may be in progress */
    public static final int SERVER_BUSY       = 503;
    /** Processing error in the backend connector */
    public static final int BACKEND_ERROR     = 506;
    /** User not authorized */
    public static final int FORBIDDEN_ERROR   = 403;
    /** Source URI not found on server */
    public static final int NOT_FOUND_URI_ERROR = 405;

   
    
    

    // Client codes

    /**
     * Constructs an instance of <code>SyncException</code>
     * with the specified detail message.
     *
     * @param code the error code.
     * @param msg the detail message.
     */
    public SyncException(int code, String msg) {
        super(code, msg);
    }

}