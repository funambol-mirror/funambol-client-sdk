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

#ifndef INCL_LOG_ERROR_HANDLER
#define INCL_LOG_ERROR_HANDLER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/ErrorHandler.h"

class LogErrorHandler: public ErrorHandler {

    public:
        LogErrorHandler();
        ~LogErrorHandler();

        /**
         * @brief Handle a warning event.
         *        The sync engine continues the operations.
         *
         * @param code  An integer code defining the warning
         * @param msg   A message explaining the warning
         *
         * @return None.
         */
        void warning(int code, const char*  msg);

        /**
         * @brief Handle an error condition
         *        The sync engine may continue or not the operations,
         *        depending on client response.
         *
         * @param code  An integer code defining the error
         * @param msg   A message explaining the error
         *
         * @return true if the synchronization must be stopped.
         */
        bool error(int code, const char*  msg);

        /**
         * @brief Handle a fatal error
         *        The synchronization process cannot be
         *        completed.
         *
         * @param code  An integer code defining the error
         * @param msg   A message explaining the error
         *
         * @return None
         */
        void fatalError(int code, const char*  msg);

        /**
         * @brief clone the error handler object.
         *
         * @return None
         */
        ErrorHandler* clone();
};

/** @endcond */
#endif

