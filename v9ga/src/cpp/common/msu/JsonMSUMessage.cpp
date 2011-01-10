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

#include "msu/JsonMSUMessage.h"
#include "msu/MSUDeviceInfo.h"
#include "base/Log.h"
#include "cJSON.h"

BEGIN_FUNAMBOL_NAMESPACE

static bool checkErrorMessage(cJSON* root, StringBuffer& errorCode, StringBuffer& errorMessage);

JsonMSUMessage::JsonMSUMessage()
{
}

JsonMSUMessage::~JsonMSUMessage()
{
}


bool JsonMSUMessage::parseLogin(const char* message)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *sessionId = NULL;
   
    if ((message == NULL) || (strlen(message) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        
        return false;
    }
    
    if ((root = cJSON_Parse(message)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        
        return false;
    }
   
    if (checkErrorMessage(root, errorCode, errorMessage)) {
        return false;
    }
   
    // security check: verify if SAPI JSON object is well formed
    // and contains the jsession ID parameter
   
    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    if ((sessionId = cJSON_GetObjectItem(data, "jsessionid")) == NULL) {
        LOG.error("%s: no session id in JSON object", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    cJSON_Delete(root);
    
    return true;
}

bool JsonMSUMessage::parseCaptchaUrl(const char* message, char** captchaUrlStr)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *captchaUrl = NULL;
    cJSON *portalUrl = NULL;
    cJSON *imagePath = NULL;
    cJSON *captchaStatus = NULL;
    
    int captchaStatusValue = cJSON_True;
    const char* portalUrlStr = NULL;
    const char* imagePathStr = NULL;
    int urlLen = 0;
    
    *captchaUrlStr = NULL;
    
    if ((message == NULL) || (strlen(message) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        
        return false;
    }
    
    if ((root = cJSON_Parse(message)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        
        return false;
    }
    
    if (checkErrorMessage(root, errorCode, errorMessage)) {
        return false;
    }
    
    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: error parsing JSON message: no data field", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    if ((captchaUrl = cJSON_GetObjectItem(data, "captchaurl")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find captcha url", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
  
    if ((captchaStatus = cJSON_GetObjectItem(captchaUrl, "active")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find captcha image status", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
  
    if ((captchaStatusValue = captchaStatus->type) == cJSON_False) {
        LOG.error("%s: captcha image is not active", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    if ((portalUrl = cJSON_GetObjectItem(captchaUrl, "portalurl")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find captcha portal url", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
   
    if ((imagePath = cJSON_GetObjectItem(captchaUrl, "imagepath")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find captcha image path", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
   
    if (((portalUrlStr = portalUrl->valuestring) == NULL) ||
            ((imagePathStr = imagePath->valuestring) == NULL)) {
        LOG.error("%s: invalid captcha url parameters", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    urlLen = strlen(portalUrlStr) + strlen(imagePathStr) + 1;
    
    *captchaUrlStr = new char [urlLen + 1];
    sprintf(*captchaUrlStr, "%s%s", portalUrlStr, imagePathStr);
    
    cJSON_Delete(root);

    return true;
}

bool JsonMSUMessage::parseSignUp(const char* message)
{
    cJSON *root = NULL, *data = NULL;
    cJSON *status = NULL,
          *user   = NULL;
    int userStatus = cJSON_True;
    
    if ((message == NULL) || (strlen(message) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        
        return false;
    }
    
    if ((root = cJSON_Parse(message)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        
        return false;
    }
   
    if (checkErrorMessage(root, errorCode, errorMessage)) {
        cJSON_Delete(root);
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
       
        return false;
    }
    
    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find \"data\" field", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
  
        
    if ((user = cJSON_GetObjectItem(data, "user")) == NULL) {
        LOG.error("%s: error parsing JSON message: can't find \"user\" field", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    if ((status = cJSON_GetObjectItem(user, "active")) == NULL) {
        cJSON_Delete(root);
        LOG.error("%s: error parsing JSON message: can't find \"status\" field", __FUNCTION__);
      
        return false;
    }
    
    if ((userStatus = status->type) == cJSON_False) {
        LOG.error("%s: user status is not active", __FUNCTION__);
        cJSON_Delete(root);

        return false;
    }
    
    cJSON_Delete(root);
    
    return true;
}


const char* JsonMSUMessage::formatSignUp(const MSUDeviceInfo *deviceInfo, bool prettyPrint)
{
    char* formattedObject = NULL;
    cJSON *root = NULL, 
          *data = NULL, 
          *user  = NULL;
    const char *phonenumber  = NULL,
               *password     = NULL,
               *platform     = NULL,
               *manufacturer = NULL,
               *model        = NULL,
               *carrier      = NULL,
               *countrya2    = NULL;
               
    if (deviceInfo == NULL) {
        LOG.error("%s: device info is not set");
    
        return formattedObject;
    }
    
    if ((root = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        
        return formattedObject;
    }
   
    if ((data = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        
        return formattedObject;
    }
     
    if ((user = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        cJSON_Delete(root);

        return formattedObject;
    }
    
    if ((phonenumber = deviceInfo->getPhoneNumber())) {
        cJSON_AddStringToObject(user, "phonenumber", phonenumber);
    }
   
    if ((password = deviceInfo->getPassword())) {
        cJSON_AddStringToObject(user, "password", password);
    }
   
    if ((platform = deviceInfo->getPlatform())) {
        cJSON_AddStringToObject(user, "platform", platform);
    }
    
    if ((manufacturer = deviceInfo->getManufacturer())) {
        cJSON_AddStringToObject(user, "manufacturer", manufacturer);
    }
    
    if ((model = deviceInfo->getModel())) {
        cJSON_AddStringToObject(user, "model", model);
    }
    
    if ((carrier = deviceInfo->getCarrier())) {
        cJSON_AddStringToObject(user, "carrier", carrier);
    }
    
    if ((countrya2 = deviceInfo->getCountryCodeA2())) {
        cJSON_AddStringToObject(user, "countrya2", countrya2);
    }
    
    cJSON_AddItemToObject(root, "data", data);
    cJSON_AddItemToObject(data, "user", user);
    
    if (prettyPrint) {
        formattedObject = cJSON_Print(root);
    } else {
        formattedObject = cJSON_PrintUnformatted(root);
    }
    
    if (formattedObject == NULL) {
        LOG.error("%s: error formatting JSON object", __FUNCTION__);
    }
    
    cJSON_Delete(root);
    
    return formattedObject;
}

bool checkErrorMessage(cJSON* root, StringBuffer& errorCode, StringBuffer& errorMessage)
{
    cJSON *error = NULL,
          *code  = NULL,
          *message  = NULL;
    const char *errorMessageVal = NULL,
               *errorCodeVal    = NULL;
 
    // reset instance variables
    errorCode.reset();
    errorMessage.reset();
    
    if (root == NULL) {
        return false;
    }
    
    // check if cJSON object is an error message
    if ((error = cJSON_GetObjectItem(root, "error")) == NULL) {
        return false;
    }
   
    if ((code = cJSON_GetObjectItem(error, "code")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"code\" field", __FUNCTION__);
    
        return false;
    }
    
    if ((message = cJSON_GetObjectItem(error, "message")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"message\" field", __FUNCTION__);
    
        return false;
    }
    
    if ((errorCodeVal = code->valuestring) != NULL) {
        errorCode = errorCodeVal;
    }
    
    if ((errorMessageVal = message->valuestring) != NULL) {
        errorMessage = errorMessageVal;
    }
    
    return true;
}

END_FUNAMBOL_NAMESPACE
