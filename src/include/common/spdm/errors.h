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
#ifndef INCL_SPDM_ERRORS
#define INCL_SPDM_ERRORS
/** @cond DEV */

#define ERR_INVALID_CONTEXT             10000
#define ERR_SOURCE_DEFINITION_NOT_FOUND 10001
#define ERR_DM_TREE_NOT_AVAILABLE       10002
#define ERR_NO_SOURCE_TO_SYNC           10003
#define ERR_ITEM_ERROR                  10004

#define ERRMSG_INVALID_CONTEXT       "Invalid context: %s"
#define ERRMSG_DM_TREE_NOT_AVAILABLE "Unable to access the DM Tree"
#define ERRMSG_SOURCE_DEFINITION_NOT_FOUND "Configuration not found for source %s"
#define ERRMSG_NO_SOURCE_TO_SYNC "No sources to synchronize"
#define ERRMSG_ITEM_ERROR           "An error occurred on one or more items."

/** @endcond */
#endif
