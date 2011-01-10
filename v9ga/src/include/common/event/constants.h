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

#ifndef INCL_EVENT_CONSTANTS
#define INCL_EVENT_CONSTANTS
/** @cond DEV */

//
// Event Codes
//

//Sync Events
#define SYNC_BEGIN                  0x0001
#define SYNC_END                    0x0002
#define SYNC_ERROR                  0x0003
#define SEND_INITIALIZATION         0x0004
#define SEND_MODIFICATION           0x0005
#define SEND_FINALIZATION           0x0006

//Transport Events
#define SEND_DATA_BEGIN             0x0011
#define SEND_DATA_END               0x0012
#define RECEIVE_DATA_BEGIN          0x0013
#define DATA_RECEIVED               0x0014
#define RECEIVE_DATA_END            0x0015

//Sync Source Events
#define SYNC_SOURCE_BEGIN              0x0021
#define SYNC_SOURCE_END                0x0022
#define SYNC_SOURCE_SYNCMODE_REQUESTED 0x0023
#define SYNC_SOURCE_TOTAL_CLIENT_ITEMS 0x0024
#define SYNC_SOURCE_TOTAL_SERVER_ITEMS 0x0025
#define SYNC_SOURCE_SERVER_BEGIN       0x0026
#define SYNC_SOURCE_SERVER_END         0x0027

//Sync Item Events
#define ITEM_ADDED_BY_SERVER        0x0031
#define ITEM_DELETED_BY_SERVER      0x0032
#define ITEM_UPDATED_BY_SERVER      0x0033
#define ITEM_ADDED_BY_CLIENT        0x0034
#define ITEM_DELETED_BY_CLIENT      0x0035
#define ITEM_UPDATED_BY_CLIENT      0x0036
#define ITEM_UPLOADED_BY_CLIENT     0x0037

//Sync Status Events
#define CLIENT_STATUS               0x0041
#define SERVER_STATUS               0x0042


#define DIM_ERROR_MSG               1024
#define DIM_COMMAND                 128

//Listener Families
#define SYNC_LISTENER               0
#define SYNC_SOURCE_LISTENER        1
#define SYNC_STATUS_LISTENER        2
#define SYNC_TRANSPORT_LISTENER     3
#define SYNC_ITEM_LISTENER          4


/** @endcond */
#endif
