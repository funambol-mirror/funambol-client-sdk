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
#include "base/ErrorHandler.h"

ErrorHandler::ErrorHandler() {
}
ErrorHandler::~ErrorHandler() {
}

/**
 * @brief Get last error
 *
 * @return Return the code of the last error occurred.
 */
int ErrorHandler::getLastError() {
    return lastError;
}

/**
 * @brief Reset last error attribute to the initial value
 *
 * @return Return the code of the previous value of last error.
 */
int ErrorHandler::resetError() {
    lastError = 0;
    return lastError;
}

