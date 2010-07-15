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
#ifndef INCL_COMMON_ERRORS
#define INCL_COMMON_ERRORS
/** @cond DEV */

#include "spdm/errors.h"
#include "spds/errors.h"
#include "http/errors.h"

#define DIM_ERROR_MESSAGE 512

#define ERR_NONE        0
#define ERR_UNSPECIFIED 1

#define ERR_GENERIC_BASE        1000
#define ERR_NOT_ENOUGH_MEMORY   ERR_GENERIC_BASE
#define ERR_PARAMETER_IS_EMPTY  ERR_GENERIC_BASE + 1
#define ERR_PARAMETER_IS_NULL   ERR_GENERIC_BASE + 2
#define ERR_WRONG_PARAMETERS    ERR_GENERIC_BASE + 3

#define ERR_FILE_SYSTEM         ERR_GENERIC_BASE + 10
#define ERR_FILE_READ           ERR_GENERIC_BASE + 11
#define ERR_FILE_WRITE          ERR_GENERIC_BASE + 12
#define ERR_BAD_FILE_CONTENT    ERR_GENERIC_BASE + 13

#define ERR_SERVER_QUOTA_EXCEEDED   420
#define ERR_RESTART_SYNC            499

#define ERRMSG_B64_GARBAGE              "Garbage found, giving up"
#define ERRMSG_B64_ORPHANED_BITS        "Orphaned bits ignored"
#define ERRMSG_NOT_ENOUGH_MEMORY        "Not enough memory (%d bytes required)"

#define ERRMSG_FILE_SYSTEM      "Error accessing file system"
#define ERRMSG_FILE_READ        "Error reading file: %s"
#define ERRMSG_FILE_WRITE       "Error writing file: %s"
#define ERRMSG_BAD_FILE_CONTENT "Bad file content: %s"

/**
 * Reset the error message and code.
 */
void resetError();

/**
 * Set error message and code.
 */
void setError(int errorCode, const char *errorMessage);

/**
 * Set error message and code.
 */
void setErrorF(int errorCode, const char *msgFormat, ...);

/**
 * Retrieve the last error code.
 */
int getLastErrorCode();

/**
 * Retrieve the last error message.
 */
const char *getLastErrorMsg();

/** @endcond */
#endif
