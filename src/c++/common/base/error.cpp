/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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

#include "base/fscapi.h"

int  lastErrorCode = ERR_NONE;
char lastErrorMsg[DIM_ERROR_MESSAGE];

// Reset error message and code
void resetError() {
    lastErrorCode = ERR_NONE;
    strcpy(lastErrorMsg, "");
}

// Set error message and code
void setError(int errorCode, const char *errorMessage) {
    lastErrorCode = errorCode;
    strcpy(lastErrorMsg, errorMessage);
}

// Set error message and code
void setErrorF(int errorCode, const char *msgFormat, ...) {
    lastErrorCode = errorCode;

    va_list argList;
    va_start(argList, msgFormat);
    vsprintf(lastErrorMsg, msgFormat, argList);
    va_end(argList);
}

// Retrieve last error code
int getLastErrorCode() {
    return lastErrorCode;
}

// Retrieve last error message
const char *getLastErrorMsg() {
    return lastErrorMsg;
}
