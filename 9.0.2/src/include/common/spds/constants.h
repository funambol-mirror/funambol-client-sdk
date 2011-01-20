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



#ifndef INCL_SPDS_CONSTANTS
#define INCL_SPDS_CONSTANTS
/** @cond DEV */

#include "base/fscapi.h"

#define DIM_SOURCE_NAME      128
#define DIM_SOURCE_URI        64
#define DIM_KEY              256
#define DIM_MIME_TYPE         64
#define DIM_USERNAME         100
#define DIM_PASSWORD         100
#define DIM_DEVICE_ID         50
#define DIM_SERVERNAME       100
#define DIM_SYNC_MODES_LIST   64
#define DIM_SYNC_MODE         16
#define DIM_ANCHOR            32

#define ERR_REPRESENTATION       700
#define SYNC_ABORTED_BY_CLIENT   701


#define SYNC_MODE_NONE                          "none"
#define SYNC_MODE_TWO_WAY                       "two-way"
#define SYNC_MODE_ONE_WAY_FROM_CLIENT           "one-way-from-client"
#define SYNC_MODE_ONE_WAY_FROM_SERVER           "one-way-from-server"
#define SYNC_MODE_REFRESH_FROM_SERVER           "refresh-from-server"
#define SYNC_MODE_REFRESH_FROM_CLIENT           "refresh-from-client"
#define SYNC_MODE_SMART_ONE_WAY_FROM_CLIENT     "smart-one-way-from-client"
#define SYNC_MODE_SMART_ONE_WAY_FROM_SERVER     "smart-one-way-from-server"



typedef enum SyncMode{
        SYNC_NONE                                   = 000,
        SYNC_TWO_WAY                                = 200,
        SYNC_SLOW                                   = 201,
        SYNC_ONE_WAY_FROM_CLIENT                    = 202,
        SYNC_REFRESH_FROM_CLIENT                    = 203,
        SYNC_ONE_WAY_FROM_SERVER                    = 204,
        SYNC_REFRESH_FROM_SERVER                    = 205,
        SYNC_TWO_WAY_BY_SERVER                      = 206,
        SYNC_ONE_WAY_FROM_CLIENT_BY_SERVER          = 207,
        SYNC_REFRESH_FROM_CLIENT_BY_SERVER          = 208,
        SYNC_ONE_WAY_FROM_SERVER_BY_SERVER          = 209,
        SYNC_REFRESH_FROM_SERVER_BY_SERVER          = 210,
        //---Funambol extension-----------------
        SYNC_SMART_ONE_WAY_FROM_CLIENT              = 250,
        SYNC_SMART_ONE_WAY_FROM_SERVER              = 251,
        SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT  = 252,
        SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_SERVER  = 253,
        SYNC_ADDR_CHANGE_NOTIFICATION               = 745
} SyncMode;

/// Used by devInfo sync capabilities
static const struct {
    SyncMode mode;
    int type;
} syncCapsMapping[] = {
    { SYNC_TWO_WAY,                                1 }, // Support of 'two-way sync'
    { SYNC_SLOW,                                   2 }, // Support of 'slow two-way sync'
    { SYNC_ONE_WAY_FROM_CLIENT,                    3 }, // Support of 'one-way sync from client only'
    { SYNC_REFRESH_FROM_CLIENT,                    4 }, // Support of 'refresh sync from client only'
    { SYNC_ONE_WAY_FROM_SERVER,                    5 }, // Support of 'one-way sync from server only'
    { SYNC_REFRESH_FROM_SERVER,                    6 }, // Support of 'refresh sync from server only'
    //                                             7,   // Support of 'server alerted sync'
    { SYNC_SMART_ONE_WAY_FROM_CLIENT,             50 }, // Support of 'smart-one-way sync from client only'
    { SYNC_SMART_ONE_WAY_FROM_SERVER,             51 }, // Support of 'smart-one-way sync from client only'
    { SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT, 52 }, // Support of 'incremental-smart-one-way sync from server only'
    { SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_SERVER, 53 }, // Support of 'incremental-smart-one-way sync from server only'
    { SYNC_NONE,                                  -1 }
};

#define CONTEXT_SPDS_SYNCML  "/spds/syncml"
#define CONTEXT_SERVER       "/server"
#define CONTEXT_SPDS_SOURCES "/spds/sources"
#define CONTEXT_CONN         "/Conn"
#define CONTEXT_AUTH         "/Auth"
#define CONTEXT_DEV_INFO     "/DevInfo"
#define CONTEXT_DEV_DETAIL   "/DevDetail"
#define CONTEXT_EXT          "/Ext"
#define CONTEXT_DATASTORES   "/DataStores"

//-------------------------------------------------------
// Status codes

#define IN_PROGRESS                                   101
#define OK                                            200
#define ITEM_ADDED                                    201
#define ACCEPTED_FOR_PROCESSING                       202
#define NONAUTHORITATIVE_RESPONSE                     203
#define NO_CONTENT                                    204
#define RESET_CONTENT                                 205
#define PARTIAL_CONTENT                               206
#define CONFLICT_RESOLVED_WITH_MERGE                  207
#define CONFLICT_RESOLVED_WITH_CLIENT_COMMAND_WINNING 208
#define CONFLICT_RESOLVED_WITH_DUPLICATE              209
#define DELETE_WITHOUT_ARCHIVE                        210
#define ITEM_NOT_DELETED                              211
#define AUTHENTICATION_ACCEPTED                       212
#define CHUNKED_ITEM_ACCEPTED                         213
#define OPERATION_CANCELLED_OK                        214
#define NOT_EXECUTED                                  215
#define ATOMIC_ROLLBACK_OK                            216
#define MULTIPLE_CHOICES                              300
#define MOVED_PERMANENTLY                             301
#define FOUND                                         302
#define SEE_ANOTHER_URI                               303
#define NOT_MODIFIED                                  304
#define USE_PROXY                                     305
#define BAD_REQUEST                                   400
#define INVALID_CREDENTIALS                           401
#define PAYMENT_REQUIRED                              402
#define FORBIDDEN                                     403
#define NOT_FOUND                                     404
#define COMMAND_NOT_ALLOWED                           405
#define OPTIONAL_FEATURE_NOT_SUPPORTED                406
#define MISSING_CREDENTIALS                           407
#define REQUEST_TIMEOUT                               408
#define UPDATE_CONFLICT                               409
#define GONE                                          410
#define SIZE_REQUIRED                                 411
#define INCOMPLETE_COMMAND                            412
#define REQUESTED_ENTITY_TOO_LARGE                    413
#define URI_TOO_LONG                                  414
#define UNSUPPORTED_MEDIA_TYPE                        415
#define REQUESTED_SIZE_TOO_BIG                        416
#define RETRY_LATER                                   417
#define ALREADY_EXISTS                                418
#define CONFLICT_RESOLVED_WITH_SERVER_DATA            419
#define DEVICE_FULL                                   420
#define UNKNOWN_SEARCH_GRAMMAR                        421
#define BAD_CGI_SCRIPT                                422
#define SOFT_DELETE_CONFLICT                          423
#define OBJECT_SIZE_MISMATCH                          424
#define PERMISSION_DENIED                             425
#define COMMAND_FAILED                                500
#define COMMAND_NOT_IMPLEMENTED                       501
#define BAD_GATEWAY                                   502
#define SERVICE_UNAVAILABLE                           503
#define GATEWAY_TIMEOUT                               504
#define VERSION_NOT_SUPPORTED                         505
#define PROCESSING_ERROR                              506
#define ATOMIC_FAILED                                 507
#define REFRESH_REQUIRED                              508
#define RECIPIENT_EXCEPTION_RESERVED1                 509
#define DATASTORE_FAILURE                             510
#define SERVER_FAILURE                                511
#define SYNCHRONIZATION_FAILED                        512
#define PROTOCOL_VERSION_NOT_SUPPORTED                513
#define OPERATION_CANCELLED                           514
#define ATOMIC_ROLLBACK_FAILED                        516
#define ATOMIC_RESPONSE_TOO_LARGE_TO_FIT              517

// ------------------------------------------------------

//-------------------------------------------------------
// Commands

#define COMMAND_ADD     "Add"
#define COMMAND_DELETE  "Delete"
#define COMMAND_GET     "Get"
#define COMMAND_PUT     "Put"
#define COMMAND_REPLACE "Replace"
#define COMMAND_RESULTS "Results"
#define COMMAND_STATUS  "Status"
#define COMMAND_SYNC    "Sync"

//-------------------------------------------------------

/** @endcond */
#endif
