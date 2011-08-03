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

#ifndef INCL_BASE_MESSAGES
#define INCL_BASE_MESSAGES
/** @cond DEV */

#include "base/fscapi.h"

//
// NOTE: these messages are client specific and targeted to the users (for
// example to being displayed to the user). They may replace corresponding
// ERRMSG_*** messages which are instead log-oriented
//

#define MSG_CONFIG_SUCCESSFULLY_READ    "Configuration successfully read"
#define MSG_INITIALIZATATION_MESSAGE    "Initialization message:"
#define MSG_UNKNOWN                     "Unknown"
#define MSG_PREPARING_SYNC              "Preparing synchronization of %s..."
#define MSG_SYNC_URL                    "Synchronization URL: %s"
#define MSG_NOT_AUTHORIZED              "Sorry, you are not authorized to synchronize. Check the username/password settings."
#define MSG_DB_NOT_FOUND                "Remote database %s not found. Check the remote settings."
#define MSG_INVALID_URL                 "Invalid synchronization URL. Check the connection settings."
#define MSG_MODIFICATION_MESSAGE        "Modification message:"
#define MSG_OUT_OF_MEMORY               "Out of memory!"
#define MSG_APPLYING_MODIFICATIONS      "Applying server modifications: %ld new item(s), %ld updated item(s), %ld deleted item(s)"
#define MSG_ADDING_ITEMS                "Adding %ld item(s)"
#define MSG_UPDATING_ITEMS              "Updating %ld item(s)"
#define MSG_DELETING_ITEMS              "Deleting %ld item(s)"
#define MSG_SYNCHRONIZING               "Synchronizing..."
#define MSG_SLOW_SYNC                   "Performing slow synchronization"
#define MSG_TWOWAY_SYNC                 "Performing two-way synchronization"
#define MSG_SYNC_FAIL                   "Synchronization failed"
#define MSG_COMMITTING_CHANGES          "Committing changes"
#define MSG_SYNC_SUCCESS                "Synchronization successful"
#define MSG_FULL_DATE_TIME              "%s, %s, %s"


// messages for the logging
#define INITIALIZING                            "Initializing"
#define INITIALIZATION_DONE                     "Initialization done"
#define SERVER_ALERT_CODE                       "The server alert code for %s is %i"
#define SYNCHRONIZING                           "Synchronizing %s"
#define PREPARING_FAST_SYNC                     "Preparing fast sync for %s"
#define PREPARING_SLOW_SYNC                     "Preparing slow sync for %s"
#define PREPARING_SYNC_REFRESH_FROM_SERVER      "Preparing refresh from server sync for %s"
#define PREPARING_SYNC_ONE_WAY_FROM_SERVER      "Preparing one way from server sync for %s"
#define DETECTED_SLOW                           "Detected %i items"
#define DETECTED_FAST                           "Detected %i new items, %i updated items, %i deleted items"
#define SENDING_MODIFICATION                    "Sending modifications"
#define SENDING_ALERT                           "Sending alert to get server modifications"
#define RETURNED_NUM_ITEMS                      "Returned %i new items, %i updated items, %i deleted items for %s"
#define MODIFICATION_DONE                       "Modification done"
#define SENDING_MAPPING                         "Sending mapping"
#define SYNCHRONIZATION_DONE                    "Synchronization done"
#define RESPURI                                 "url from response to inizialization-message: %s"
#define MESSAGE_SENT                            "Message sent"
#define READING_RESPONSE                        "Reading response..."

/** @endcond */
#endif
