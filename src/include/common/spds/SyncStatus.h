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

// SyncML Status Codes definition
#ifndef INCL_SYNC_STATUS
#define INCL_SYNC_STATUS
/** @cond DEV */

enum SyncStatus {
    STC_INVALID_CODE                                  = 0,
    // Informational 1xx
    STC_IN_PROGRESS                                   = 101,
    // Successful 2xx
    STC_OK                                            = 200,
    STC_ITEM_ADDED                                    = 201,
    STC_ACCEPTED_FOR_PROCESSING                       = 202,
    STC_NONAUTHORITATIVE_RESPONSE                     = 203,
    STC_NO_CONTENT                                    = 204,
    STC_RESET_CONTENT                                 = 205,
    STC_PARTIAL_CONTENT                               = 206,
    STC_CONFLICT_RESOLVED_WITH_MERGE                  = 207,
    STC_CONFLICT_RESOLVED_WITH_CLIENT_COMMAND_WINNING = 208,
    STC_CONFLICT_RESOLVED_WITH_DUPLICATE              = 209,
    STC_DELETE_WITHOUT_ARCHIVE                        = 210,
    STC_ITEM_NOT_DELETED                              = 211,
    STC_AUTHENTICATION_ACCEPTED                       = 212,
    STC_CHUNKED_ITEM_ACCEPTED                         = 213,
    STC_OPERATION_CANCELLED_OK                        = 214,
    STC_NOT_EXECUTED                                  = 215,
    STC_ATOMIC_ROLLBACK_OK                            = 216,
    // Redirection 3xx
    STC_MULTIPLE_CHOICES                              = 300,
    STC_MOVED_PERMANENTLY                             = 301,
    STC_FOUND                                         = 302,
    STC_SEE_ANOTHER_URI                               = 303,
    STC_NOT_MODIFIED                                  = 304,
    STC_USE_PROXY                                     = 305,
    // Originator Exceptions 4xx
    STC_BAD_REQUEST                                   = 400,
    STC_INVALID_CREDENTIALS                           = 401,
    STC_PAYMENT_REQUIRED                              = 402,
    STC_FORBIDDEN                                     = 403,
    STC_NOT_FOUND                                     = 404,
    STC_COMMAND_NOT_ALLOWED                           = 405,
    STC_OPTIONAL_FEATURE_NOT_SUPPORTED                = 406,
    STC_MISSING_CREDENTIALS                           = 407,
    STC_REQUEST_TIMEOUT                               = 408,
    STC_UPDATE_CONFLICT                               = 409,
    STC_GONE                                          = 410,
    STC_SIZE_REQUIRED                                 = 411,
    STC_INCOMPLETE_COMMAND                            = 412,
    STC_REQUESTED_ENTITY_TOO_LARGE                    = 413,
    STC_URI_TOO_LONG                                  = 414,
    STC_UNSUPPORTED_MEDIA_TYPE                        = 415,
    STC_REQUESTED_SIZE_TOO_BIG                        = 416,
    STC_RETRY_LATER                                   = 417,
    STC_ALREADY_EXISTS                                = 418,
    STC_CONFLICT_RESOLVED_WITH_SERVER_DATA            = 419,
    STC_DEVICE_FULL                                   = 420,
    STC_UNKNOWN_SEARCH_GRAMMAR                        = 421,
    STC_BAD_CGI_SCRIPT                                = 422,
    STC_SOFT_DELETE_CONFLICT                          = 423,
    STC_OBJECT_SIZE_MISMATCH                          = 424,
    STC_PERMISSION_DENIED                             = 425,
    // Recipient Exceptions 5xx
    STC_COMMAND_FAILED                                = 500,
    STC_COMMAND_NOT_IMPLEMENTED                       = 501,
    STC_BAD_GATEWAY                                   = 502,
    STC_SERVICE_UNAVAILABLE                           = 503,
    STC_GATEWAY_TIMEOUT                               = 504,
    STC_VERSION_NOT_SUPPORTED                         = 505,
    STC_PROCESSING_ERROR                              = 506,
    STC_ATOMIC_FAILED                                 = 507,
    STC_REFRESH_REQUIRED                              = 508,
    STC_RECIPIENT_EXCEPTION_RESERVED1                 = 509,
    STC_DATASTORE_FAILURE                             = 510,
    STC_SERVER_FAILURE                                = 511,
    STC_SYNCHRONIZATION_FAILED                        = 512,
    STC_PROTOCOL_VERSION_NOT_SUPPORTED                = 513,
    STC_OPERATION_CANCELLED                           = 514,
    STC_ATOMIC_ROLLBACK_FAILED                        = 516,
    STC_ATOMIC_RESPONSE_TOO_LARGE_TO_FIT              = 517,
    /**
     * These codes are application specific; they are
     * not part of the SyncML protocol.
     */
    STC_SESSION_EXPIRED                               = 10000,
    STC_SESSION_ABORTED                               = 20000
};

/** @endcond */
#endif

