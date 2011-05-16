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

#include "msu/MSUManager.h"
#include "http/URL.h"
#include "http/BasicAuthentication.h"
#include "ioStream/BufferInputStream.h"
#include "ioStream/StringOutputStream.h"
#include "ioStream/BufferOutputStream.h"

BEGIN_FUNAMBOL_NAMESPACE

const char* MSUManager::sapiLoginUrlFmt                  = "/sapi/login?action=login&login=%s&password=%s";
const char* MSUManager::sapiGetCaptchaUrlFmt             = "/sapi/system/captcha?action=get-url&mobile=true";
const char* MSUManager::sapiSignUpUrlFmt                 = "/sapi/mobile?action=signup&token=%s";
const char* MSUManager::sapiSignUpUrlFmtNoCaptcha        = "/sapi/mobile?action=signup";
const char* MSUManager::sapiCheckValidityFmt    = "/sapi/profile?action=validate";


// See Funambol SAPI Design Doc:
static const char* sapiErrorInvalidUserId      = "PRO-1106";       // "Invalid UserId"
static const char* sapiErrorInvalidPassword    = "PRO-1115";       // "Invalid Password"
static const char* sapiErrorExistingUserName   = "PRO-1113";       // "You can't specify an existing user name"
static const char* sapiErrorInvalidCaptcha     = "PRO-1126";       // "Invalid CAPTCHA token"
static const char* sapiErrorInvalidPhoneId     = "PRO-1128";       // "Invalid Phone UserId"
static const char* sapiErrorInvalidPhoneNumber = "COM-1001";       // "Phone number provided is not valid"
static const char* sapiErrorInvalidCountryCode = "COM-1017";       // "Country code of phone number is not valid"

MSUManager::MSUManager(const char* url, const char* user_agent) : serverUrl(url),
                      userAgent(user_agent),
                      httpConnection(new HttpConnection(userAgent)),
                      jsonMessage(new JsonMSUMessage())
{
}

MSUManager::~MSUManager()
{
    delete jsonMessage;
    delete httpConnection;
}        
        
EMSUStatusCode MSUManager::login(const char* username, const char* password)
{
    int status = 0;
    StringOutputStream response;            // sapi response buffer
    StringBuffer loginUrl;
    StringBuffer sapiLoginUrl;
    URL requestUrl;
    const char* sapiLoginResponse = NULL;
    
    sessionId = "";
    
    if (((username == NULL) || (password == NULL)) ||
            ((strlen(username) == 0) || (strlen(password) == 0))) {
        LOG.error("%s: invalid parameters", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
    sapiLoginUrl.sprintf(sapiLoginUrlFmt, username, password);
    loginUrl.sprintf("%s%s", serverUrl.c_str(), sapiLoginUrl.c_str());
    
    requestUrl.setURL(loginUrl.c_str());
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_LENGTH,  0);
    
    if ((status = httpConnection->open(requestUrl)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
            
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
            
            // handle here other cases that need
            // special return values 
            default:
                return EMSUGenericHttpError;
        }
    }
    
    httpConnection->close();
        
    if ((sapiLoginResponse = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi login response", __FUNCTION__);
        
        return EMSUSapiInvalidResponse;
    }
    
    LOG.debug("response returned = %s", sapiLoginResponse);
    
    // check SAPI reponse message
    if ((status = jsonMessage->parseLogin(sapiLoginResponse)) == false) {
        LOG.error("%s: error parsing sapi login response", __FUNCTION__);
        
        return EMSUSapiMessageParseError;
    }
    
    return EMSUSuccess;
}

EMSUStatusCode MSUManager::getCaptchaUrl(char** captchaUrl)
{
    int status = 0;
    StringOutputStream response;            // sapi response buffer
    StringBuffer captchaRequestUrl;
    URL requestUrl;
    const char* captchaUrlJsonObject = NULL; // JSON object with captcha URL 
    
    sessionId = "";
    *captchaUrl = NULL;
    
    if (serverUrl.empty()) {
        LOG.error("%s: invalid parameters", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
    captchaRequestUrl.sprintf("%s%s", serverUrl.c_str(), sapiGetCaptchaUrlFmt);
    
    requestUrl.setURL(captchaRequestUrl.c_str());
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_LENGTH,  0);
    
    if ((status = httpConnection->open(requestUrl)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending captcha URL request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
            
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
            
            // handle here other cases that need
            // special return values 
            default:
                return EMSUGenericHttpError;
        }
    }
    
    httpConnection->close();
        
    if ((captchaUrlJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi captcha URL response", __FUNCTION__);
        
        return EMSUSapiInvalidResponse;
    }
    
    LOG.debug("response returned = %s", captchaUrlJsonObject);
    
    if (jsonMessage->parseCaptchaUrl(captchaUrlJsonObject, captchaUrl) == false) {
        LOG.error("%s: error parsing sapi captcha URL json object", __FUNCTION__);
        
        return EMSUSapiMessageParseError;
    }
    
    LOG.debug("captcha URL from SAPI Json response: %s", *captchaUrl);
    
    return EMSUSuccess;
}

EMSUStatusCode MSUManager::getCaptcha(const char* captchaRequestUrl, unsigned char** captchaImage, int *captchaImageSize)
{
    int status = 0;
    BufferOutputStream imageData; // sapi response buffer
    URL requestUrl;
    int imageSize = 0;
    
    *captchaImage = NULL;
    *captchaImageSize = 0;
    sessionId = "";
    
    if (((captchaRequestUrl == NULL) || (strlen(captchaRequestUrl) == 0)) || 
            serverUrl.empty()) {
        LOG.error("%s: invalid parameters", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
    requestUrl.setURL(captchaRequestUrl);
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_LENGTH,  0);
    
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    if ((status = httpConnection->request(NULL, imageData)) != HTTP_OK) {
        LOG.error("%s: error sending captcha URL request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
            
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
            
            // handle here other cases that need
            // special return values 
            default:
                return EMSUGenericHttpError;
        }
    }
    
    // Save the JSessionID returned by the server.
    // Will be used for the signup.
    StringBuffer hdr = httpConnection->getResponseHeader(HTTP_HEADER_SET_COOKIE);
    if (!hdr.empty()) {
        sessionId = httpConnection->parseJSessionId(hdr);
        LOG.debug("%s: saved session id: %s", __FUNCTION__, sessionId.c_str());
    }
    
    httpConnection->close();
    
    imageSize = imageData.size();
    
    if (imageSize) {
        *captchaImageSize = imageSize;
        *captchaImage = new unsigned char [imageSize];
    
        memcpy((void *)*captchaImage, (void *)imageData.getData(), imageSize);
    }
    
    return EMSUSuccess;
}
       
EMSUStatusCode MSUManager::signup(const char* username, const char* password, const char *token, MSUDeviceInfo* deviceInfo)
{
    // safe checks
    if (((username == NULL) || (password == NULL)) ||
        ((strlen(username) == 0) || (strlen(password) == 0))) {
        LOG.error("%s: invalid parameters (invalid user name or password)", __FUNCTION__);
    
        return EMSUInvalidParam;
    }
    
    if ( (token == NULL) || (deviceInfo == NULL) || (strlen(token) == 0) ) {
        LOG.error("%s: invalid parameters (invalid token or device info is missing)", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
    // Set the username/password inserted by the user
    deviceInfo->setPhoneNumber(username);
    deviceInfo->setPassword(password);
    
    
    //
    // Format the signup request body
    //
    const char* requestBody = jsonMessage->formatSignUp(deviceInfo, false);
    if (requestBody == NULL) {
        LOG.error("%s: error formatting signup request", __FUNCTION__);
        return EMSUSapiMessageFormatError;
    }
    
    StringBuffer body(requestBody);
    delete [] requestBody;

    //
    // Format the signup request URL
    //
    StringBuffer signupUrl;
    StringBuffer sapiSignupUrl;
    URL requestUrl; 

    sapiSignupUrl.sprintf(sapiSignUpUrlFmt, token);
    signupUrl.sprintf("%s%s", serverUrl.c_str(), sapiSignupUrl.c_str());
    requestUrl.setURL(signupUrl.c_str());
    
    //
    // Setup and open connection
    //
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    //httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, inputStream.getTotalSize());
    if (!sessionId.empty()) {
        StringBuffer sessionIdCookie;
        sessionIdCookie.sprintf("JSESSIONID=%s", sessionId.c_str());
        // must use the same jsessionID used during the getCaptha phase
        httpConnection->setRequestHeader(HTTP_HEADER_COOKIE, sessionIdCookie.c_str());
    }
    
    int status = 0;
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodPost)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    //
    // Send the HTTP POST request
    //
    StringOutputStream response;
    if ((status = httpConnection->request(body.c_str(), response)) != HTTP_OK) 
    {
        LOG.error("%s: error sending signup request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
                
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
                
            default:
                // handle here other cases that need
                // special return values
                return EMSUGenericHttpError;
        }
    }
    
    httpConnection->close();
    
    if (response.getString().empty()) {
        LOG.error("%s: empty sapi signup response", __FUNCTION__);
        return EMSUSapiInvalidResponse;
    }
    
    //
    // Check the response
    //
    const char* sapiResponse = response.getString().c_str();
    LOG.debug("response returned = %s", sapiResponse);
    
    if ((status = jsonMessage->parseSignUp(sapiResponse)) == false) 
    {
        const char* errorCode = jsonMessage->getErrorCode().c_str();
        const char* errorMsg  = jsonMessage->getErrorMessage().c_str();
        if (!errorCode || !errorMsg) {
            return EMSUSapiMessageParseError;
        }
        
        LOG.error("SAPI error %s: %s", errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiErrorInvalidCaptcha)) {                  // Invalid captcha: PRO-1126
            return EMSUInvalidCaptchaToken;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidUserId)   ||            // Invalid user id: PRO-1106, PRO-1128, COM-1001
                 !strcmp(errorCode, sapiErrorInvalidPhoneId)  ||
                 !strcmp(errorCode, sapiErrorInvalidPhoneNumber)) {
            return EMSUInvalidUserId;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidPassword)) {     // Invalid password (PRO-1115)           
            return EMSUInvalidPassword;
        }
        else if (!strcmp(errorCode, sapiErrorExistingUserName)) {    // Existing user name (PRO-1113)
            return EMSUWrongPassword;
        }
        
        // all other errors
        return EMSUSapiMessageParseError;
    }
    
    return EMSUSuccess;
}

EMSUStatusCode MSUManager::signupNoCaptcha(const char* username, const char* password, MSUDeviceInfo* deviceInfo)
{
    // safe checks
    if (((username == NULL) || (password == NULL)) ||
        ((strlen(username) == 0) || (strlen(password) == 0))) {
        LOG.error("%s: invalid parameters (invalid user name or password)", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
     if ( (deviceInfo == NULL) ) {
     LOG.error("%s: invalid parameters (invalid device info is missing)", __FUNCTION__);
     
     return EMSUInvalidParam;
     }
    
    // Set the username/password inserted by the user
    deviceInfo->setPhoneNumber(username);
    deviceInfo->setPassword(password);
    
    
    //
    // Format the signup request body
    //
    const char* requestBody = jsonMessage->formatSignUp(deviceInfo, false);
    if (requestBody == NULL) {
        LOG.error("%s: error formatting signup request", __FUNCTION__);
        return EMSUSapiMessageFormatError;
    }
    
    StringBuffer body(requestBody);
    delete [] requestBody;
    
    //
    // Format the signup request URL
    //
    StringBuffer signupUrl;
    StringBuffer sapiSignupUrl;
    URL requestUrl; 
    
    sapiSignupUrl.assign(sapiSignUpUrlFmtNoCaptcha);
    signupUrl.sprintf("%s%s", serverUrl.c_str(), sapiSignupUrl.c_str());
    requestUrl.setURL(signupUrl.c_str());
    
    LOG.info("signup url: %s", signupUrl.c_str());
    LOG.info("sapisignup url: %s", sapiSignupUrl.c_str());
    
    //
    // Setup and open connection
    //
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    //httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, inputStream.getTotalSize());
    if (!sessionId.empty()) {
        StringBuffer sessionIdCookie;
        sessionIdCookie.sprintf("JSESSIONID=%s", sessionId.c_str());
        // must use the same jsessionID used during the getCaptha phase
        httpConnection->setRequestHeader(HTTP_HEADER_COOKIE, sessionIdCookie.c_str());
    }
    
    int status = 0;
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodPost)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    //
    // Send the HTTP POST request
    //
    StringOutputStream response;
    if ((status = httpConnection->request(body.c_str(), response)) != HTTP_OK) 
    {
        LOG.error("%s: error sending signup request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
                
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
                
            default:
                // handle here other cases that need
                // special return values
                return EMSUGenericHttpError;
        }
    }
    
    httpConnection->close();
    
    if (response.getString().empty()) {
        LOG.error("%s: empty sapi signup response", __FUNCTION__);
        return EMSUSapiInvalidResponse;
    }
    
    //
    // Check the response
    //
    const char* sapiResponse = response.getString().c_str();
    LOG.debug("response returned = %s", sapiResponse);
    
    if ((status = jsonMessage->parseSignUp(sapiResponse)) == false) 
    {
        const char* errorCode = jsonMessage->getErrorCode().c_str();
        const char* errorMsg  = jsonMessage->getErrorMessage().c_str();
        if (!errorCode || !errorMsg) {
            return EMSUSapiMessageParseError;
        }
        
        LOG.error("SAPI error %s: %s", errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiErrorInvalidCaptcha)) {                  // Invalid captcha: PRO-1126
            return EMSUInvalidCaptchaToken;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidUserId)   ||            // Invalid user id: PRO-1106, PRO-1128, COM-1001
                 !strcmp(errorCode, sapiErrorInvalidPhoneId)  ||
                 !strcmp(errorCode, sapiErrorInvalidPhoneNumber)) {
            return EMSUInvalidUserId;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidPassword)) {     // Invalid password (PRO-1115)           
            return EMSUInvalidPassword;
        }
        else if (!strcmp(errorCode, sapiErrorExistingUserName)) {    // Existing user name (PRO-1113)
            return EMSUWrongPassword;
        }
        
        // all other errors
        return EMSUSapiMessageParseError;
    }
    
    return EMSUSuccess;
}

EMSUStatusCode MSUManager::checkFieldsValidity(const char* username, const char* password) {
    
    // safe checks
    if (((username == NULL) || (password == NULL)) ||
        ((strlen(username) == 0) || (strlen(password) == 0))) {
        LOG.error("%s: invalid parameters (invalid user name or password)", __FUNCTION__);
        
        return EMSUInvalidParam;
    }
    
    //
    // Format the signup request body
    //
    const char* requestBody = jsonMessage->formatFieldsToCheckValidity(username, password);
    if (requestBody == NULL) {
        LOG.error("%s: error formatting signup request", __FUNCTION__);
        return EMSUSapiMessageFormatError;
    }
    
    StringBuffer body(requestBody);
    delete [] requestBody;
    
    //
    // Format the signup request URL
    //
    StringBuffer signupUrl;
    StringBuffer sapiSignupUrl(sapiCheckValidityFmt);
    URL requestUrl; 
    
    signupUrl.sprintf("%s%s", serverUrl.c_str(), sapiSignupUrl.c_str());
    requestUrl.setURL(signupUrl.c_str());
    
    //
    // Setup and open connection
    //
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");
        
    int status = 0;
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodPost)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        httpConnection->close();
        
        return EMSUConnectionSetupError; // malformed URI, etc
    }
    
    //
    // Send the HTTP POST request
    //
    StringOutputStream response;
    if ((status = httpConnection->request(body.c_str(), response)) != HTTP_OK) 
    {
        LOG.error("%s: error sending signup request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return EMSUInvalidCredentials;
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
                return EMSUHTTPFunctionalityNotSupported;
                
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return EMSUNetworkError;
                
            default:
                // handle here other cases that need
                // special return values
                return EMSUGenericHttpError;
        }
    }
    
    httpConnection->close();
    
    if (response.getString().empty()) {
        LOG.error("%s: empty sapi signup response", __FUNCTION__);
        return EMSUSapiInvalidResponse;
    }
    
    //
    // Check the response
    //
    const char* sapiResponse = response.getString().c_str();
   
    LOG.debug("response returned = %s", sapiResponse);
    
    if ((status = jsonMessage->parseFieldsToCheckValidity(sapiResponse)) == false) 
    {
        const char* errorCode = jsonMessage->getErrorCode().c_str();
        const char* errorMsg  = jsonMessage->getErrorMessage().c_str();
        if (!errorCode || !errorMsg) {
            return EMSUSapiMessageParseError;
        }
        
        LOG.error("SAPI error %s: %s", errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiErrorInvalidUserId)   ||            // Invalid user id: PRO-1106, PRO-1128, COM-1001
                 !strcmp(errorCode, sapiErrorInvalidPhoneId)  ||
                 !strcmp(errorCode, sapiErrorInvalidPhoneNumber)) {
            return EMSUInvalidUserId;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidPassword)) {     // Invalid password (PRO-1115)           
            return EMSUInvalidPassword;
        }
        else if (!strcmp(errorCode, sapiErrorExistingUserName)) {    // Existing user name (PRO-1113)
            return EMSUWrongPassword;
        }
        else if (!strcmp(errorCode, sapiErrorInvalidCountryCode)) {    // Existing user name (COM-1017)
            return EMSUInvalidCountryCode;
        }
        
        // all other errors
        return EMSUSapiMessageParseError;
    }
    
    return EMSUSuccess;
    
    
}

END_FUNAMBOL_NAMESPACE
