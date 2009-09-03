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



#ifndef INCL_BASE_DEBUG
#define INCL_BASE_DEBUG
/** @cond DEV */

#include "base/fscapi.h"

/**
 * This constants are for debug messages that won't be removed from the code
 * but will be used when the client is in debug mode.
 */
#define DBG_LOADING_CONN_SETTINGS "Loading connection settings from node %s"
#define DBG_LOADING_MANAGEMENT_OBJECT "Loading management object %s"
#define DBG_INVALID_ACCESS_SETTINGS "Access settings invalid record format!"
#define DBG_NODE_NOT_FOUND "Node not found"
#define DBG_LOADING_SOURCES "Loading sources subnodes from node %s"
#define DBG_INVALID_SOURCE_SETTINGS "Source settings invalid record format!"
#define DBG_SOURCE_CONFIGS_NOT_FOUND "No source configs were found, using default"
#define DBG_COMMITTING_SOURCE "Committing source %s (next: %s)"
#define DBG_COMMITTING_CHANGE "Committing %s operation on item %s"
#define DBG_READING_CONFIG_FROM_DM "Reading configuration settings from the management tree"
#define DBG_WRITING_CONFIG_TO_DM "Writing configuration settings to the management tree"
#define DBG_DELETING_ITEM "Deleting item with LUID: %ld - Status: %u"
#define DBG_ADDING_ITEM "Adding item with GUID: %s - LUID: %lu - Status: %u"
#define DBG_UPDATING_ITEM "Updating item with LUID: %lu - Status: %u"
#define DBG_ITEM_STATUS "Item: %s, Cmd: %s, Status code: %u"
#define DBG_NUMBER_OF_RECORD "The sync source %s contains %d items"
#define DBG_REMOVING_RECORD "Removing record in position %d (%ld)"
#define DBG_RESETTING_RECORD "Resetting record in position %d (%ld)"
#define DBG_RECORD_ATTTRS "Record info of %s"
#define DBG_DELETION_DETECTED "Detected deleted record: %ld"
#define DBG_CHANGE_DETECTED "Detected modified record: %ld"
#define DBG_RECORD_NOT_COMMITTED "Record in position %d (%ld) not committed due to error %d on %s"
#define DBG_READING "Reading ..."
#define DBG_READING_CONTENT "Reading content (buffer size: %ld)"
#define DBG_READING_BUFFER "Read %ld bytes"
#define DBG_READING_DONE "Reading done"
#define DBG_RECEIVED_CONTENT "Response:"
#define DBG_HTTP_HEADERS "Request HTTP headers:"
/** @endcond */
#endif
