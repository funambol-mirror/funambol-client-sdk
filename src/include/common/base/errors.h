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
#ifndef INCL_COMMON_ERRORS
#define INCL_COMMON_ERRORS
/** @cond DEV */

#include "spdm/errors.h"
#include "spds/errors.h"
#include "http/errors.h"

#define DIM_ERROR_MESSAGE 512

#define ERR_NONE        0
#define ERR_UNSPECIFIED 1
#define ERR_NOT_ENOUGH_MEMORY   1000
#define ERR_PARAMETER_IS_EMPTY  1001
#define ERR_PARAMETER_IS_NULL   1002
#define ERR_WRONG_PARAMETERS    1003

#define ERRMSG_B64_GARBAGE              "Garbage found, giving up"
#define ERRMSG_B64_ORPHANED_BITS        "Orphaned bits ignored"
#define ERRMSG_NOT_ENOUGH_MEMORY        "Not enough memory (%d bytes required)"
   
/* ************************ DEPRECATED ***************************
 * Do not access these variable directly anymore, they will be hidden
 * in the future. Use the access methods instead.
 */
extern int  lastErrorCode;
extern char lastErrorMsg[];

/**
 * Reset the error message and code.
 */
void resetError();

/**
 * Set error message and code.
 */
void setError(int errorCode, const char *errorMessage);

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
