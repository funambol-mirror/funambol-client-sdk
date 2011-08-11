/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2011 Funambol, Inc.
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

#ifndef __SAPI_RESTORE_CHARGE_JSON_PARSER_H__
#define __SAPI_RESTORE_CHARGE_JSON_PARSER_H__

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "cJSON.h"
#include "base/util/StringMap.h"

BEGIN_FUNAMBOL_NAMESPACE

typedef enum ESapiChargeParserStatus
{
    ESPSNoError = 0,
    ESPSInvalidArgument,
    ESPSParseError,
    ESPSInvalidMessage,
    ESPSKeyMissing,
    ESPSValueMissing
} ESPStatus;

/// json managing class for SAPI call "Pay for Restore service".
class SapiRestoreChargeJsonParser
{
    private:
        StringBuffer errorCode;
        StringBuffer errorMessage;

    public:
        SapiRestoreChargeJsonParser();
        ~SapiRestoreChargeJsonParser();

        /**
         * Formats the json object for SAPI call SAPIRestoreCharge, like:
         *
         * {data:
         *     {
         *       "service":"sync",
         *       "resource":"pim"
         *     }
         * }
         *
         * @param resource     the resource to request in the JSON body (i.e. "pim")
         * @param jsonString   [OUT] the formatted JSON body
         * @param prettyPrint  format type for json
         * @return             true if operation accomplished, false otherwise
         */
        bool formatRestoreChargeBody(const char* resource, char **jsonString, bool prettyPrint=false);


        ///
        /// Parses the json returned from SAPI call
        ///
        /// input parameters:  message   the json response from server
        ///                    errCode   output error code ( setted to ESPSNoError if all was ok).
        ///
        /// returns         :  true if all was ok. false if there was an error. (errCode contains the error code)
        ///
        bool parseRestoreChargeResponse(const char* message, ESPStatus* errCode);

        StringBuffer& getErrorCode()     { return errorCode;    }
        StringBuffer& getErrorMessage()  { return errorMessage; }

    private:
		int jsonObjArray2StringMap(cJSON* objJsonArray, StringMap* stringMap);  //converts from an array "name":"value" to string map
        ESPStatus checkErrorMessage(cJSON* objRoot, bool* error);

};

END_FUNAMBOL_NAMESPACE

#endif
