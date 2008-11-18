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

#ifndef INCL_SPDS_ERRORS
#define INCL_SPDS_ERRORS
/** @cond DEV */

#define ERR_PROTOCOL_ERROR      400
#define ERR_AUTH_NOT_AUTHORIZED 401
#define ERR_AUTH_EXPIRED        402
#define ERR_NOT_FOUND           404
#define ERR_AUTH_REQUIRED       407
#define ERR_SERVER_FAILURE      500

#define ERR_DT_BASE             800
// data transformer not supported
#define ERR_DT_UNKNOWN          ERR_DT_BASE+0
// data transformer error
#define ERR_DT_FAILURE          ERR_DT_BASE+1

// StartSync return codes
// Another sync in progress
#define ERR_ANOTHER_SYNC        810


#define ERRMSG_SERVER_FAILURE   "The server returned error code %d"
#define ERRMSG_DT_UNKNOWN       "Data transformer '%s' unknown"
#define ERRMSG_DT_FAILURE       "Data transformer error: %lx"

#define ERRMSG_ANOTHER_SYNC    "Another sync in progress"

/** @endcond */
#endif
