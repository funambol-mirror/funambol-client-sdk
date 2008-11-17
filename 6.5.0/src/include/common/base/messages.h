/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
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
