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
#ifndef INCL_HTTP_ERRORS
#define INCL_HTTP_ERRORS
/** @cond DEV */

#define ERR_TRANSPORT_BASE              2000
#define ERR_NETWORK_INIT                ERR_TRANSPORT_BASE
#define ERR_CONNECT                     ERR_TRANSPORT_BASE+ 1
#define ERR_HOST_NOT_FOUND              ERR_TRANSPORT_BASE+ 2
#define ERR_READING_CONTENT             ERR_TRANSPORT_BASE+ 3
#define ERR_WRITING_CONTENT             ERR_TRANSPORT_BASE+ 4
#define ERR_HTTP                        ERR_TRANSPORT_BASE+50
#define ERR_HTTP_MISSING_CONTENT_LENGTH ERR_TRANSPORT_BASE+51
#define ERR_SERVER_ERROR                ERR_TRANSPORT_BASE+52

#define ERR_HTTP_STATUS_NOT_OK          ERR_TRANSPORT_BASE+53
#define ERR_HTTPS_INVALID_CA            ERR_TRANSPORT_BASE+54

#define ERR_CREDENTIAL                  401
#define ERR_CLIENT_NOT_NOTIFIABLE       420
#define ERR_CTP_ALLOWED                 421

#define ERR_TRANSPORT_LAST              2999

#define ERRMSG_NETWORK_INIT "Network initialization error"
#define ERRMSG_CONNECT "Connection failure"
#define ERRMSG_HOST_NOT_FOUND "Host not found"
#define ERRMSG_READING_CONTENT "Error reading content"
#define ERRMSG_WRITING_CONTENT "Error writing content"
#define ERRMSG_HTTP_MISSING_CONTENT_LENGTH "Missing Content-Length header"
#define ERRMSG_SERVER_ERROR ("Server error")
/** @endcond */
#endif
