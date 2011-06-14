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

#include "sapi/SapiMediaJsonParser.h"
#include "base/Log.h"
#include "base/util/utils.h"

BEGIN_FUNAMBOL_NAMESPACE

SapiMediaJsonParser::SapiMediaJsonParser()
{
    errorCode.reset();
    errorMessage.reset();
}   

SapiMediaJsonParser::~SapiMediaJsonParser()
{
}

bool SapiMediaJsonParser::parseItemCountObject(const char* jsonObj, int* count, ESMPStatus* errCode)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *itemsCount = NULL;
    bool error = false;

    *count = 0;

    if ((jsonObj == NULL) || (strlen(jsonObj) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
    
    if ((root = cJSON_Parse(jsonObj)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        *errCode = ESMPParseError;
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        *errCode = ESMPNoError;
        
        return false;
    } 

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    if ((itemsCount = cJSON_GetObjectItem(data, "count")) == NULL) {
        LOG.error("%s: missing \"count\" field in JSON object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    LOG.debug("%s: item count from json object: %d", __FUNCTION__, itemsCount->valueint);

    *count = (int)itemsCount->valueint;

    cJSON_Delete(root);
    
    return true;
}

bool SapiMediaJsonParser::parseItemsListObject(const char* itemsListJsonObject, const char* sourceName, 
                            ArrayList& sapiItemInfoList, time_t* responseTime, ESMPStatus* errCode)
{
    cJSON *root = NULL,
          *data = NULL,
          *itemsArray = NULL,
          *responsetime = NULL,
          *serverUrl = NULL;
          
    int arraySize = 0;
    bool error = false;
    char* downloadServerUrl = NULL;
    
    if ((itemsListJsonObject == NULL) || (strlen(itemsListJsonObject) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESMPInvalidMessage;
      
        return false;
    }
    
    if ((sourceName == NULL) || (strlen(sourceName) == 0)) {
        LOG.error("%s: missing source name parameter", __FUNCTION__);
        
        return false;
    }
    
    if ((root = cJSON_Parse(itemsListJsonObject)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
    
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
    
        return false;
    } 

    if ((responsetime = cJSON_GetObjectItem(root, "responsetime")) != NULL) {
        
        // reponse time from server are in millisecs
        *responseTime = static_cast<time_t>(responsetime->valueint / 1000);
        
        // to be removed. try to see if it is a string...
        if (*responseTime == 0) {
             *responseTime = (atoll(responsetime->valuestring) / 1000);
        }
    } else {
        LOG.error("%s: responsetime parameter missing in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    if ((serverUrl = cJSON_GetObjectItem(data, "mediaserverurl")) != NULL) {
        downloadServerUrl = serverUrl->valuestring;
    }
   
    if ((itemsArray = cJSON_GetObjectItem(data, sourceName)) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    arraySize = cJSON_GetArraySize(itemsArray);
    
    if (arraySize) {
        for (int i = 0; i < arraySize; i++) {
            cJSON* sourceItem = cJSON_GetArrayItem(itemsArray, i);
            
            if (sourceItem) {
                SapiSyncItemInfo *sourceItemInfo = parseSourceItem(sourceItem, downloadServerUrl);
                
                if (sourceItemInfo) {
                    sapiItemInfoList.add(static_cast<ArrayElement&>(*sourceItemInfo));
                } else {
                    LOG.error("%s: error getting source item info from json object");
                }
            } else {
                LOG.error("%s: error getting source item for json object array", __FUNCTION__); 
            }
        }
    }
    
    cJSON_Delete(root);
    
    return true;
}

bool SapiMediaJsonParser::parseItemsChangesObject(const char* itemsChangesJsonObject, const char* sourceName, 
        ArrayList& newIDs, ArrayList& modIDs, ArrayList& delIDs, time_t* responseTimestamp, ESMPStatus* errCode)
{
    cJSON *root = NULL,
          *data = NULL,
          *responsetime = NULL,
          *itemsList         = NULL,
          *newItemsArray     = NULL,
          *updatedItemsArray = NULL,
          *deletedItemsArray = NULL;
          
    bool error = false;
    const char *newItemsKey     = "N", // see SAPI developer guide @section 2.5.42
               *updatedItemsKey = "U",
               *deletedItemsKey = "D";
    int arraySize = 0;
    
    if ((itemsChangesJsonObject == NULL) || (strlen(itemsChangesJsonObject) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESMPInvalidMessage;
      
        return false;
    }

     if ((sourceName == NULL) || (strlen(sourceName) == 0)) {
        LOG.error("%s: missing source name parameter", __FUNCTION__);
        *errCode = ESMPInvalidArgument;
      
        return false;
    }
    
    if ((root = cJSON_Parse(itemsChangesJsonObject)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        *errCode = ESMPParseError;
      
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPInvalidMessage;
      
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        *errCode = ESMPNoError;
      
        return false;
    } 

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
      
        return false;
    }

    if ((responsetime = cJSON_GetObjectItem(root, "responsetime")) != NULL) {
        
        // reponse time from server are in millisecs
        *responseTimestamp = static_cast<time_t>(responsetime->valueint / 1000);
        // to be removed. try to see if it is a string...
        if (*responseTimestamp == 0) {
            *responseTimestamp = (atoll(responsetime->valuestring) / 1000);
        }
    } else {
        LOG.error("%s: missing responsetime field from json message", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    if ((arraySize = cJSON_GetArraySize(data)) == 0) {
        LOG.info("%s: json object has no items set in array", __FUNCTION__);        
        cJSON_Delete(root);

        return true;
    }

    if ((itemsList = cJSON_GetObjectItem(data, sourceName)) == NULL) {
        LOG.error("%s: missing source data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    if ((newItemsArray = cJSON_GetObjectItem(itemsList, newItemsKey)) != NULL) {
        if (parseItemsChangeArray(newItemsArray, newIDs) != ESMPNoError) {
            LOG.error("%s: error parsing item changes json array", __FUNCTION__);
            cJSON_Delete(root);

            return false;
        }
    }
    
    if ((updatedItemsArray = cJSON_GetObjectItem(itemsList, updatedItemsKey)) != NULL) {
        if (parseItemsChangeArray(updatedItemsArray, modIDs) != ESMPNoError) {
            LOG.error("%s: error parsing item changes json array", __FUNCTION__);
            cJSON_Delete(root);

            return false;
        }
    }
    
    if ((deletedItemsArray = cJSON_GetObjectItem(itemsList, deletedItemsKey)) != NULL) {
        if (parseItemsChangeArray(deletedItemsArray, delIDs) != ESMPNoError) {
            LOG.error("%s: error parsing item changes json array", __FUNCTION__);
            cJSON_Delete(root);

            return false;
        }
    }
    
    cJSON_Delete(root);

    return true;
}
    
bool SapiMediaJsonParser::parseQuotaInfoObject(const char* quotaInfoJsonObject, unsigned long long* free, unsigned long long* quota, ESMPStatus* errCode)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *freeNode = NULL;
    cJSON *quotaNode = NULL;
    bool error = false;

    *free = 0;
    *quota = 0;

    if ((quotaInfoJsonObject == NULL) || (strlen(quotaInfoJsonObject) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
    
    if ((root = cJSON_Parse(quotaInfoJsonObject)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        *errCode = ESMPNoError;
        
        return false;
    } 

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    if ((freeNode = cJSON_GetObjectItem(data, "free")) == NULL) {
        LOG.error("%s: missing \"free\" field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    if ((quotaNode = cJSON_GetObjectItem(data, "quota")) == NULL) {
        LOG.error("%s: missing \"free\" field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    *free  = (unsigned long long)freeNode->valueint;
    *quota = (unsigned long long)quotaNode->valueint;
    
    cJSON_Delete(root);

    return true;
}

bool SapiMediaJsonParser::parseMediaAddItem(const char* itemMetaDataUploadJson, StringBuffer& itemId, time_t* lastUpdate, ESMPStatus* errCode)
{
    cJSON *root = NULL;
    cJSON *id = NULL;
    cJSON *date = NULL;
    const char* id_value = NULL;
    const char* date_value = NULL;
    bool error = false;

    // clear out id buffer
    itemId.reset();
    
    if (itemMetaDataUploadJson == NULL) {
        LOG.error("%s: json formatted object parameter in empty", __FUNCTION__);
        
        return false;
    }
    
    if ((root = cJSON_Parse(itemMetaDataUploadJson)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
    
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        
        return false;
    } 
    
    // possibile json reponses: 
    // for uploads: {"success":"Item uploaded successfully","id":"410919","lastupdate":1302275919}
    // for updates: {"success":"Item uploaded successfully","lastupdate":1302275919}
    // see section 3.10 "Upload binary files" of funambol SAPI developer guide
    if ((id = cJSON_GetObjectItem(root, "id")) != NULL) {
        id_value = id->valuestring;
    
        if ((id_value == NULL) || (strlen(id_value) == 0)) {
            LOG.error("%s: empty id field in json object", __FUNCTION__);
            cJSON_Delete(root);
            *errCode = ESMPValueMissing;
            return false;
        }
        
        itemId.assign(id_value);
    }

    // Get the "lastupdate" property = the upload time on the server
    if ((date = cJSON_GetObjectItem(root, "lastupdate")) != NULL) {
        // upload time from server is in millisecs
        *lastUpdate = static_cast<time_t>((date->valueint/1000));
        //*lastUpdate = static_cast<time_t>(date->valueint);
        // to be removed. try to see if it is a string...
        if (*lastUpdate == 0) {
            *lastUpdate = atoll(date->valuestring);
        }
    } 
    else {
        //LOG.error("%s: missing 'lastupdate' field from json message", __FUNCTION__);
        *lastUpdate = 0;
        //cJSON_Delete(root);
        //*errCode = ESMPKeyMissing;
        //return false;
    }
    
    cJSON_Delete(root);
    
    return true;
}

bool SapiMediaJsonParser::formatItemsListObject(const ArrayList& itemsIDs, char **itemsListJsonObject, bool prettyPrint)
{
    cJSON *root = NULL, *ids = NULL; 
    int itemsCount = 0;
    
    if ((itemsCount = itemsIDs.size()) == 0) {
        return false;
    }
    
    if ((root = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        
        return false;
    }
   
    if ((ids = cJSON_CreateArray()) == NULL) {
        LOG.error("error creating JSON object");
        cJSON_Delete(root);
        
        return false;
    }
     
    for (int i = 0; i < itemsCount; i++) {
        StringBuffer* itemIdStr = static_cast<StringBuffer* >(itemsIDs.get(i));
        
        if ((itemIdStr) && (itemIdStr->empty() == false)) {
            int itemId = atoi(itemIdStr->c_str());
            cJSON* jsonItemId = cJSON_CreateInt((long)itemId);
            
            cJSON_AddItemToArray(ids, jsonItemId);
        }
    }
    
    cJSON_AddItemToObject(root, "ids", ids);
    
    if (prettyPrint) {
        *itemsListJsonObject = cJSON_Print(root);
    } else {
        *itemsListJsonObject = cJSON_PrintUnformatted(root);
    }
    
    cJSON_Delete(root);
   
    if (*itemsListJsonObject == NULL) {
        LOG.error("%s: error formatting JSON object", __FUNCTION__);
        return false;
    }
    
    return true;
}

bool SapiMediaJsonParser::formatMediaItemMetaData(SapiSyncItemInfo* itemInfo, char** itemJsonMetaData, bool prettyPrint)
{
    cJSON *root = NULL, *data = NULL;
    const char* itemGuid = NULL;
    const char* itemName = NULL;
    const char* itemContentType = NULL;
    size_t itemSize = 0;
    time_t itemCreationDate = 0, itemModificationDate = 0;
    StringBuffer utcDate;
    
    if (itemInfo == NULL) {
        return false;
    }
    
    if ((root = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        
        return false;
    }
 
     if ((data = cJSON_CreateObject()) == NULL) {
        LOG.error("error creating JSON object");
        cJSON_Delete(root);
        
        return false;
    }

    itemGuid = itemInfo->getGuid();
    
    if ((itemGuid != NULL) && (strlen(itemGuid) > 0)) {
        LOG.debug("%s: formatting item GUID '%s' into json message",
            __FUNCTION__, itemGuid);
            
        cJSON_AddStringToObject(data, "id", itemGuid);
    }
    
    itemName = itemInfo->getName();
    if ((itemName == NULL) || (strlen(itemName) == 0)) {
        LOG.error("%s: missing name in item info", __FUNCTION__);
        cJSON_Delete(root);
        
        return false;
    }
        
    cJSON_AddStringToObject(data, "name", itemName);
    
    itemContentType = itemInfo->getContentType();
    if (itemContentType) {
        cJSON_AddStringToObject(data, "contenttype", itemContentType);
    }
    
    if ((itemSize = itemInfo->getSize()) == 0) {
        LOG.error("%s: invalid size parameter in item info", __FUNCTION__);
        cJSON_Delete(root);
        
        return false;
    }
        
    cJSON_AddIntToObject(data, "size", itemSize);
    
    itemCreationDate = itemInfo->getCreationDate();
    
    utcDate = unixTimeToString((unsigned long)itemCreationDate, true);
    if (utcDate.empty()) {
        LOG.error("%s: error convering item creation date in UTC string", __FUNCTION__);
        
        cJSON_Delete(root);
        
        return false;
    }
    
    cJSON_AddStringToObject(data, "creationdate", utcDate.c_str());
    
    itemModificationDate = itemInfo->getModificationDate();
    utcDate = unixTimeToString((unsigned long)itemModificationDate, true);
    if (utcDate.empty()) {
        LOG.error("%s: error convering item modification date in UTC string", __FUNCTION__);
        
        cJSON_Delete(root);
        
        return false;
    }
    
    cJSON_AddStringToObject(data, "modificationdate", utcDate.c_str());
    
    cJSON_AddItemToObject(root, "data", data);
    
    if (prettyPrint) {
        *itemJsonMetaData = cJSON_Print(root);
    } else {
        *itemJsonMetaData = cJSON_PrintUnformatted(root);
    }
   
    cJSON_Delete(root);
   
    if ((*itemJsonMetaData == NULL) || (strlen(*itemJsonMetaData) == 0)) {
        LOG.error("%s: error formatting JSON object", __FUNCTION__);
    
        return false;
    }
    
    return true;
}

        
ESMPStatus SapiMediaJsonParser::parseItemsChangeArray(cJSON* itemsArray, ArrayList& idList)
{
    int arraySize = 0;
 
    if (itemsArray == NULL) {
        LOG.error("%s: invalid json array object", __FUNCTION__);
        
        return ESMPInvalidArgument;
    }
    
    arraySize = cJSON_GetArraySize(itemsArray);
    
    if (arraySize) {
        for (int i = 0; i < arraySize; i++) {
            cJSON* itemID = cJSON_GetArrayItem(itemsArray, i);
            
            if (itemID) {                
                StringBuffer s = StringBuffer().sprintf("%d", static_cast<int>(itemID->valueint));
                
                idList.add(s);
            } else {
                LOG.error("%s: error getting source item id in json object array", __FUNCTION__); 
                
                return ESMPInvalidMessage;
            }
        }
    }

    return ESMPNoError;
}

SapiSyncItemInfo* SapiMediaJsonParser::parseSourceItem(cJSON* sourceItem, const char* downloadServerUrl)
{
    cJSON *guid = NULL,
          *url = NULL,
          *modification_date = NULL,
          *creation_date = NULL,
          *size = NULL,
          *name = NULL;
    
    const char* guid_value = NULL;
    const char* url_value = NULL;
    const char* name_value = NULL;
    size_t size_value = 0;
    time_t creation_date_value = 0,
           modification_date_value = 0;
    
    SapiSyncItemInfo* itemInfo = NULL;
    
    if (sourceItem == NULL) {
        return itemInfo;
    }
    
    if ((guid = cJSON_GetObjectItem(sourceItem, "id")) == NULL) {
        LOG.error("%s: missing \"guid\" field in json object", __FUNCTION__);
   
        return itemInfo;
    }
    
    guid_value = guid->valuestring;
    
    if ((url = cJSON_GetObjectItem(sourceItem, "url")) == NULL) {
        LOG.error("%s: missing \"url\" field in json object", __FUNCTION__);
        
        return itemInfo;
    }
    
    url_value = url->valuestring;
    
    if ((name = cJSON_GetObjectItem(sourceItem, "name")) == NULL) {
        LOG.error("%s: missing \"name\" field in json object", __FUNCTION__);
   
        return itemInfo;
    }
    
    name_value = name->valuestring;
    
    if ((size = cJSON_GetObjectItem(sourceItem, "size")) == NULL) {
        LOG.error("%s: missing \"size\" field in json object", __FUNCTION__);
   
        return itemInfo;
    }
    
    size_value = static_cast<size_t>(size->valueint);
    
    if ((modification_date = cJSON_GetObjectItem(sourceItem, "date")) != NULL) {
        modification_date_value = static_cast<time_t>((modification_date->valueint/1000));
        //LOG.debug("%s: modification time: %lu", __FUNCTION__, modification_date_value);
    }
    
    if ((creation_date = cJSON_GetObjectItem(sourceItem, "creationdate")) != NULL) {
        creation_date_value = static_cast<time_t>((creation_date->valueint/1000));
    }
    
    itemInfo = new SapiSyncItemInfo(guid_value, NULL, name_value, size_value, downloadServerUrl, url_value, NULL, 
        creation_date_value, modification_date_value);
    
    return itemInfo;
}

bool SapiMediaJsonParser::parseLogin(const char* message, StringBuffer& sessionID, time_t* responseTime, ESMPStatus* errCode)
{
    cJSON *root = NULL;
    cJSON *data = NULL;
    cJSON *sessionId = NULL;
    cJSON *responsetime = NULL;

    bool error = false;
    
    sessionID.reset();
    
    if ((message == NULL) || (strlen(message) == 0)) {
        LOG.error("%s: invalid JSON message", __FUNCTION__);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
    
    if ((root = cJSON_Parse(message)) == NULL) {
        LOG.error("%s: error parsing JSON message", __FUNCTION__);
        *errCode = ESMPParseError;
        
        return false;
    }
   
    if (checkErrorMessage(root, &error) != ESMPNoError) {
        LOG.error("%s: error parsing json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPParseError;
        
        return false;
    }
  
    if (error) {
        cJSON_Delete(root);
        *errCode = ESMPNoError;
        
        return false;
    } 

    if ((responsetime = cJSON_GetObjectItem(root, "responsetime")) != NULL) {
        
        if (responseTime != NULL) {
            // reponse time from server are in millisecs
            *responseTime = static_cast<time_t>(responsetime->valueint / 1000);
        }
    } else {
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    if ((data = cJSON_GetObjectItem(root, "data")) == NULL) {
        LOG.error("%s: missing data field in json object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }
    
    if ((sessionId = cJSON_GetObjectItem(data, "jsessionid")) == NULL) {
        LOG.error("%s: no session id in JSON object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPKeyMissing;
        
        return false;
    }

    if (sessionId->valuestring == NULL) {
        LOG.error("%s: invalid session id in JSON object", __FUNCTION__);
        cJSON_Delete(root);
        *errCode = ESMPInvalidMessage;
        
        return false;
    }
    
    sessionID = sessionId->valuestring;
    
    cJSON_Delete(root);

    return true;
}


ESMPStatus SapiMediaJsonParser::checkErrorMessage(cJSON* root, bool* error_flag)
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
        return ESMPInvalidMessage;
    }
    
    // check if cJSON object is an error message
    if ((error = cJSON_GetObjectItem(root, "error")) == NULL) {
        return ESMPNoError;
    }
   
    if ((code = cJSON_GetObjectItem(error, "code")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"code\" field", __FUNCTION__);
    
        return ESMPKeyMissing;
    }
    
    if ((message = cJSON_GetObjectItem(error, "message")) == NULL) {
        LOG.error("%s: error parsing JSON message: no \"message\" field", __FUNCTION__);
    
        return ESMPKeyMissing;
    }
    
    if ((errorCodeVal = code->valuestring) != NULL) {
        errorCode = errorCodeVal;
    }
    
    if ((errorMessageVal = message->valuestring) != NULL) {
        errorMessage = errorMessageVal;
    }
   
    *error_flag = true;
 
    return ESMPNoError;
}

END_FUNAMBOL_NAMESPACE
