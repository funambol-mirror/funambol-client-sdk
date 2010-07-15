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

#ifndef INCL_HTTP_CONSTANTS
#define INCL_HTTP_CONSTANTS
/** @cond DEV */

#define METHOD_GET          TEXT("GET")
#define METHOD_POST         TEXT("POST")
#define USER_AGENT          "Funambol SyncML Client"
#define SYNCML_CONTENT_TYPE "application/vnd.syncml+xml"
#define SYNCML_WBXML_CONTENT_TYPE "application/vnd.syncml+wbxml"

#define HTTP_OK            200
#define HTTP_SERVER_ERROR  500
#define HTTP_UNAUTHORIZED  401
#define HTTP_ERROR         400
#define HTTP_NOT_FOUND     404

#define X_HTTP_420_IPPRV    420

#define DIM_URL_PROTOCOL       10
#define DIM_URL              2048
#define DIM_HOSTNAME           50
#define DIM_USERNAME          100
#define DIM_PASSWORD          100
#define DIM_BUFFER           4096
#define DIM_HEADER             64

#define STATUS_OK   200


/** @endcond */
#endif
