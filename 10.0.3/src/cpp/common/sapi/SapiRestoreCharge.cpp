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

#include "sapi/SapiRestoreCharge.h"
#include "sapi/SapiRestoreChargeJsonParser.h"
#include "sapi/SapiMediaRequestManager.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/util/utils.h"


BEGIN_FUNAMBOL_NAMESPACE


static const char* sapiRestoreChargeUrlFmt = "%s/sapi/system/payment?action=buy&login=%s&password=%s&syncdeviceid=%s";

SapiRestoreCharge::SapiRestoreCharge(AbstractSyncConfig& c) : config(c), 
															httpConnection(new HttpConnection(c.getUserAgent())),
															auth(NULL),
															jsonSapiRestoreChargeParser(new SapiRestoreChargeJsonParser()),
															err(ESRCSuccess) {
}

SapiRestoreCharge::~SapiRestoreCharge() {}


int SapiRestoreCharge::doChargeMock() {

    sleepMilliSeconds(5800); // simulate some connection time
    StringBuffer username(config.getUsername());
   
    //err = ESMRGenericError;
    return 0; // no errors occured
}

//
// Calls the SAPI Restore Charge for payment of 'restore'
// Makes an http call to server, with json in post.
//
int SapiRestoreCharge::doCharge(const char* resource) {
	
    URL url(config.getSyncURL());
    StringBuffer host = url.getHostURL();

    StringBuffer username(config.getUsername());
    StringBuffer password(config.getPassword());
    StringBuffer deviceid(config.getDevID());

    if (username.empty() || password.empty() || deviceid.empty()) {
        LOG.error("%s: invalid parameters for SAPI Restore Charge (username or password or deviceID)", __FUNCTION__);
        return ESRCInvalidParam;
    }
    if (resource == NULL) {
        LOG.error("%s: NULL parameter 'resource' for SAPI Restore Charge", __FUNCTION__);
        return ESRCInvalidParam;
    }

	// format the json
	char* jsonBody;     // formatted JSON object
	if (jsonSapiRestoreChargeParser->formatRestoreChargeBody(resource, &jsonBody, true) == false) {
        LOG.error("%s: error formatting json object for restore charge", __FUNCTION__);
        return ESRCInvalidParam;
    }
	
	if (jsonBody) {
	    LOG.debug("Restore charge Json body:\n%s", jsonBody);
	}

    // Urlencode the deviceId parameter (may contain unacceptable chars)
    const char* deviceIdEncoded = URL::urlEncode(deviceid.c_str());

    //
	// URL: "http://localhost/sapi/payment.php?action=buy&login=test&password=test&syncdeviceid=IMEI:12345"
    //
	StringBuffer sapiRestoreChargeUrl;
	sapiRestoreChargeUrl.sprintf(sapiRestoreChargeUrlFmt, host.c_str(), username.c_str(), password.c_str(), deviceIdEncoded);
	URL requestUrl = sapiRestoreChargeUrl.c_str();
	delete [] deviceIdEncoded;
    
    // set HTTP request headers
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_TYPE, "application/json");
    //httpConnection->setRequestHeader(HTTP_HEADER_COOKIE, sessionIdCookie.c_str());
    
    //BasicAuthentication auth(config.getUsername(), config.getPassword());
    //httpConnection->setAuthentication(&auth);

    int status = httpConnection->open(requestUrl, AbstractHttpConnection::MethodPost, false);
    if ((status != 0)) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        return ESRCConnectionSetupError; // malformed URI, etc
    }
    
    StringOutputStream response;            // sapi response buffer
    status = httpConnection->request(jsonBody, response, false);
    if (response.getString().empty()) {
        LOG.debug("%s: No response returned", __FUNCTION__);
    }
    else {
        LOG.debug("%s: response returned: \n%s", __FUNCTION__, response.getString().c_str());
    }
    
    ESRCStatus ret = ESRCGenericHttpError;
    switch (status) 
    {
        case HTTP_OK:
            ret = ESRCSuccess;
            break;
            
        case HTTP_UNAUTHORIZED:
            ret = ESRCAccessDenied;
            break;
            
        case HTTP_FORBIDDEN:       // HTTP 403: not enough credit
            ret = ESRCInsufficientBalance;
            break;
   
        case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
        case HTTP_NOT_FOUND:                    // code 404 returns an error!
            ret = ESRCHTTPFunctionalityNotSupported;
            break;
   
        case HttpConnection::StatusNetworkError:
        case HttpConnection::StatusReadingError:
        case HttpConnection::StatusWritingError:
            ret = ESRCNetworkError;
            break;
        
        // handle here other cases that need
        // special return values 
        default:
            ret = ESRCGenericHttpError;
            break;
    }

    httpConnection->close();
    return ret;
}

END_FUNAMBOL_NAMESPACE
