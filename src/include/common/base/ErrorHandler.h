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

#ifndef INCL_ERROR_HANDLER
#define INCL_ERROR_HANDLER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class ErrorHandler {

    public:
        ErrorHandler();
        virtual ~ErrorHandler();

        /**
         * @brief Handle a warning event.
         *        The sync engine continues the operations.
         *
         * @param code  An integer code defining the warning
         * @param msg   A message explaining the warning
         *
         * @return None.
         */
        virtual void warning(int code, const char*  msg) = 0;

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
        virtual bool error(int code, const char*  msg) = 0;

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
        virtual void fatalError(int code, const char*  msg) = 0;

        /**
         * @brief clone the error handler object.
         *
         * @return None
         */
        virtual ErrorHandler* clone() = 0;

        /**
         * @brief Get last error
         *
         * @return Return the code of the last error occurred.
         */
         int getLastError();

        /**
         * @brief Reset last error attribute to the initial value
         *
         * @return Return the code of the previous value of last error.
         */
         int resetError();

    private:
        int lastError;
};


END_NAMESPACE

/** @endcond */
#endif

