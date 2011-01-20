/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

#ifndef __MSU_MANAGER_H__
#define __MSU_MANAGER_H__

#include "http/httpConnection.h"
#include "msu/MSUDeviceInfo.h"
#include "msu/JsonMSUMessage.h"

BEGIN_FUNAMBOL_NAMESPACE

typedef enum EMSUStatusCode 
{
    EMSUSuccess = 0,                            // OK
    EMSUConnectionSetupError,                   // Error setting up the connection
    EMSUInvalidCredentials,                     // Login authentication failed
    EMSUGenericHttpError,                       // Http error
    EMSUSapiInvalidResponse,                    // Bad SAPI response received
    EMSUSapiMessageParseError,                  // Error occurred parsing the JSON body received
    EMSUSapiMessageFormatError,                 // Error occurred formatting the JSON body to send
    EMSUInvalidCaptchaToken,                    // Signup failed because token provided is wrong
    EMSUInvalidUserId,                          // Signup failed because username provided is invalid
    EMSUInvalidPassword,                        // Signup failed because password provided is invalid
    EMSUInvalidParam,                           // An invalid parameter is passed
    EMSUWrongPassword,                          // User already exists (PRO-1113)
    EMSUNetworkError,                           // Network error (timeouts, etc..)
    EMSUGenericError
};

class MSUManager 
{
private:
    StringBuffer serverUrl;
    const char* userAgent;
    
    /**
     * It's the JSessionID stored between the getCaptcha and the signup phases.
     * If empty, no sessionId is used.
     */
    StringBuffer sessionId;
    
    JsonMSUMessage* jsonMessage;
    HttpConnection* httpConnection;
    
    static const char* sapiLoginUrlFmt;
    static const char* sapiGetCaptchaUrlFmt;
    static const char* sapiSignUpUrlFmt;
    
public:
    MSUManager(const char* url, const char* user_agent);
    ~MSUManager();
    
    EMSUStatusCode login(const char* username, const char* password);
    EMSUStatusCode getCaptchaUrl(char** captchaUrl);
    EMSUStatusCode getCaptcha(const char* captchaRequestUrl, unsigned char** captchaImage, int *captchaImageSize);
    
    /**
     * Executes the mobile signup process (HTTP POST) for a new user, providing the passed captcha token.
     * The new userId and password must be passed as parameters, and are included in the JSON body request.
     * The sessionID saved during the getCaptcha request is used, if existing.
     *
     * @param username  the new user id (the phone number)
     * @param password  the password for the new user
     * @param token     the captcha token inserted manually by the user
     * @param deviceIfo the device information required to fill the JSON body for the HTTP request
     * @return          the result of the signup process, one of EMSUStatusCode.
     */
    EMSUStatusCode signup(const char* username, const char* password, const char *token, MSUDeviceInfo* deviceInfo);
    
    const char* getJsonErrorDescription() const { return jsonMessage->getErrorMessage().c_str(); }
    const char* getJsonErrorCode()        const { return jsonMessage->getErrorCode().c_str();    }
};
        
END_FUNAMBOL_NAMESPACE

#endif // __MSU_MANAGER_H__
