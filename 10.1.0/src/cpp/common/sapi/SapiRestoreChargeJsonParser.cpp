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

#include "sapi/SapiRestoreChargeJsonParser.h"
#include "base/Log.h"
#include "base/util/utils.h"

BEGIN_FUNAMBOL_NAMESPACE

SapiRestoreChargeJsonParser::SapiRestoreChargeJsonParser()
{
    errorCode.reset();
    errorMessage.reset();
}   

SapiRestoreChargeJsonParser::~SapiRestoreChargeJsonParser()
{
}


bool SapiRestoreChargeJsonParser::formatRestoreChargeBody(const char* resource, char **jsonString, bool prettyPrint)
{
    cJSON *root = NULL, *data = NULL; 
    
    if (resource == NULL) {
        LOG.error("%s: resource param missing", __FUNCTION__);
        return false;
    }
    
    if ((root = cJSON_CreateObject()) == NULL) {
        LOG.error("%s: error creating root JSON object", __FUNCTION__);
        return false;
    }
    
    if ((data = cJSON_CreateObject()) == NULL) {
       LOG.error("%s: error creating JSON object", __FUNCTION__);
       cJSON_Delete(root);
       return false;
    }

    cJSON_AddStringToObject(data, "service",  "sync");
    cJSON_AddStringToObject(data, "resource", resource);
    
    cJSON_AddItemToObject(root, "data", data);
    
    
	// making the json object in string format.
    if (prettyPrint) {
        *jsonString = cJSON_Print(root);
    } else {
        *jsonString = cJSON_PrintUnformatted(root);
    }
    
    cJSON_Delete(root); // release memory
   
    if (*jsonString == NULL) {
        LOG.error("%s: error formatting JSON object", __FUNCTION__);
        return false;
    }
    return true;
}



bool SapiRestoreChargeJsonParser::parseRestoreChargeResponse(const char* message, ESPStatus* errCode)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *sessionId = NULL;
    cJSON *responsetime = NULL;

    bool error = false;
    
    
    if ((message == NULL) || (strlen(message) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESPSInvalidMessage;
        
        return false;
    }
    
    if ((root = cJSON_Parse(message)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        *errCode = ESPSParseError;
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESPSNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESPSParseError;
        
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        *errCode = ESPSNoError;
        
        return false;
    } 

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESPSKeyMissing;
        
        return false;
    }

    cJSON_Delete(root);
	*errCode = ESPSNoError;
    return true;
}


ESPStatus SapiRestoreChargeJsonParser::checkErrorMessage(cJSON* root, bool* error_flag)
{
    cJSON *error = NULL,
          *code  = NULL,
          *message  = NULL;
    const char *errorMessageVal = NULL,
               *errorCodeVal    = NULL;
 
    // reset instance variables
    errorCode.reset();
    errorMessage.reset();
    
    *error_flag = false;
    
    if (root == NULL) {
        return ESPSInvalidMessage;
    }
    
    // check if cJSON object is an error message
    if ((error = cJSON_GetObjectItem(root, "error")) == NULL) {
        return ESPSNoError;
    }
   
    if ((code = cJSON_GetObjectItem(error, "code")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"code\" field", __FUNCTION__);
    
        return ESPSKeyMissing;
    }
    
    if ((message = cJSON_GetObjectItem(error, "message")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"message\" field", __FUNCTION__);
    
        return ESPSKeyMissing;
    }
    
    if ((errorCodeVal = code->valuestring) != NULL) {
        errorCode = errorCodeVal;
    }
    
    if ((errorMessageVal = message->valuestring) != NULL) {
        errorMessage = errorMessageVal;
    }
   
    *error_flag = true;
 
    return ESPSNoError;
}

END_FUNAMBOL_NAMESPACE
