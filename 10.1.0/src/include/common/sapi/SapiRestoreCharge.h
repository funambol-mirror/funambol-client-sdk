/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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

#ifndef SAPI_RESTORE_CHARGE__
#define SAPI_RESTORE_CHARGE__

/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "base/util/StringMap.h"
#include "spds/constants.h"
#include "spds/AbstractSyncConfig.h"

#include "http/HttpConnection.h"
#include "http/BasicAuthentication.h"
#include "http/URL.h"
#include "ioStream/BufferInputStream.h"
#include "ioStream/StringOutputStream.h"
#include "ioStream/BufferOutputStream.h"

//#include "event/FireEvent.h"



BEGIN_FUNAMBOL_NAMESPACE


typedef enum ESapiRestoreChargeStatus 
{
    ESRCSuccess = 0,
    ESRCConnectionSetupError,               // Error setting up the connection
    ESRCAccessDenied,                   
    ESRCGenericHttpError,                   // Http error
    ESRCSapiInvalidResponse,                // Bad SAPI response received
    ESRCSapiMessageParseError,              // Error occurred parsing the JSON body received
    ESRCSapiMessageFormatError,             // Error occurred formatting the JSON body to send
    ESRCInvalidParam,                       // An invalid parameter is passed
    ESRCNetworkError,                       // Network error 
    ESRCRequestTimeout,                         
    ESRCHTTPFunctionalityNotSupported,      // 501 error from the server (check fields on server not supporting this sapi)
    ESRCSapiNotSupported,                   // sapi incompatible version on server (missing needed parameters in json responses, etc..) 
    
    ESRCInsufficientBalance                 // Not enough credit to complete the action (HTTP 403)
} ESRCStatus;


class SapiRestoreChargeJsonParser;

/**
 * Used to charge the user for restore service.
 * It executes a SAPI "restore charge" parsing the json response.
*/
class SapiRestoreCharge {

public:

    /**
     * Constructor
     * @param c  configuration object to read all required param for SAPI restore charge
     */
	SapiRestoreCharge(AbstractSyncConfig& c);
    

    /// Destructor
    virtual ~SapiRestoreCharge();

    /**
     * Executes the SAPI charge for restore service
     * @param resource  the resource to request for the restore charge (i.e. "pim")
     * @return 0  if sapi was SUCCESSFUL
     *        -1  if there was some errors
     */
    int doCharge(const char* resource);

    /**
     * this is the mocked version for SAPI Restore Charge , used for testing.
     * SapiServiceProfiling::doCharge() calls this method.
     * @return 0  if login was sucessuful
     *        -1  if there was some errors
     */
    int doChargeMock();


private:
	SapiRestoreChargeJsonParser* jsonSapiRestoreChargeParser;
    HttpConnection* httpConnection;
    HttpAuthentication* auth;

    AbstractSyncConfig& config;  /// The configuration object, containing all sync settings.
    ESRCStatus err;  /// The error code
};


END_FUNAMBOL_NAMESPACE

/** @} */
/** @endcond */
#endif

