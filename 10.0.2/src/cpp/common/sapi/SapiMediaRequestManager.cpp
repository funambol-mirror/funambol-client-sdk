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

#include "sapi/SapiMediaRequestManager.h"
#include "sapi/SapiMediaJsonParser.h"
#include "http/HttpConnection.h"
#include "http/BasicAuthentication.h"
#include "http/URL.h"
#include "http/BasicAuthentication.h"
#include "ioStream/BufferInputStream.h"
#include "ioStream/StringOutputStream.h"
#include "ioStream/BufferOutputStream.h"
#include "event/FireEvent.h"

BEGIN_FUNAMBOL_NAMESPACE

// static data for SAPI media request URLs
static const char* getCountUriFmt     = "%s/sapi/media/%s?action=count&responsetime=true";
static const char* getUriFmt          = "%s/sapi/media/%s?action=get&responsetime=true&exif=none";
static const char* getUriWithIdsFmt   = "%s/sapi/media/%s?action=get&id=%s&responsetime=true&exif=none";
static const char* getWithLimitUriFmt = "%s/sapi/media/%s?action=get&limit=%d&responsetime=true&exif=none";
static const char* getWithOffsetUriFmt = "%s/sapi/media/%s?action=get&offset=%d&responsetime=true&exif=none";
static const char* getWithLimitAndOffsetUrlFmt = "%s/sapi/media/%s?action=get&limit=%d&offset=%d&responsetime=true&exif=none"; 
static const char* getChangesUrlFmt   = "%s/sapi/profile/changes?action=get&from=%s&type=%s&responsetime=true";
static const char* getChangesUrlUserPassFmt   = "%s/sapi/profile/changes?action=get&from=%s&type=%s&responsetime=true&login=%s&password=%s";
static const char* getQuotaInfoUrlFmt = "%s/sapi/media/%s?action=get-storage-space&responsetime=true";
static const char* sapiLoginUrlFmt    = "%s/sapi/login?action=login&login=%s&password=%s&responsetime=true&syncdeviceid=%s";
static const char* sapiLogoutUrlFmt   = "%s/sapi/login?action=logout&responsetime=true";

// new SAPI interface for updates
static const char* saveItemMetaDataFmt = "%s/sapi/upload/%s?action=save-metadata&responsetime=true";
static const char* saveItemData        = "%s/sapi/upload/%s?action=save&lastupdate=true";


// sapi media url access tokens 
static const char* sapiPicturesUrlToken = "picture";
static const char* sapiVideosUrlToken = "video";
static const char* sapiFilesUrlToken = "file";
static const char* sapiContactsUrlToken = "contact";


static const char* sapiJsonArrayPicturesKey = "pictures";
static const char* sapiJsonArrayVideosKey = "videos";
static const char* sapiJsonArrayFilesKey = "files";
static const char* sapiJsonArrayContactsKey = "contacts";

// sapi error codes
static const char* sapiSecurityException         = "SEC-1000";
static const char* sapiUserIdMissing             = "SEC-1001";
static const char* sapiSessionAlreadyOpen        = "SEC-1002";
static const char* sapiInvalidAuthSchema         = "SEC-1004";

static const char* sapiUnknownMediaException     = "MED-1000"; 
static const char* sapiInvalidContentRange       = "MED-1006";
static const char* sapiUserQuotaReached          = "MED-1007";

static const char* sapiPictureIdUsedWithLimit    = "PIC-1003";
static const char* sapiUnableToRetrievePicture   = "PIC-1005";

static const char* sapiInvalidDataTypeOrFormat   = "COM-1008";
static const char* sapiOperationNotSupported     = "COM-1005";
static const char* sapiMissingRequiredParameter  = "COM-1011";
static const char* sapiInvalidLUID               = "COM-1014";

/* 
 * SAPI errors code not yet used
 *
static const char* sapiFileTooLarge              = "PIC-1007";
static const char* sapiInvalidValidationKey      = "SEC-1003";
static const char* sapiMediaIdWithLimit          = "MED-1003";
static const char* sapiErrorRetrievingMediaItem  = "MED-1005";
static const char* sapiPictureGenericError       = "PIC-1000";
static const char* sapiNoPicturesSpecfied        = "PIC-1001";
static const char* sapiInvalidFileName           = "COM-1010";
static const char* sapiInvalidEmptyDataParameter = "COM-1013";
static const char* sapiInvalidDeviceId           = "COM-1015";
*/

SapiMediaRequestManager::SapiMediaRequestManager(const char* url, SapiMediaSourceType mediaSourceType, const char* user_agent, 
                                                 const char* user_name, const char* pass) : 
    serverUrl(url),
    sourceType(mediaSourceType),
    httpConnection(new HttpConnection(user_agent)),
    jsonSapiMediaObjectParser(new SapiMediaJsonParser()),
    auth(NULL),
    sapiMediaSourceName(NULL),
    username(user_name), password(pass)
{
    sessionID.reset();
    
    switch (sourceType) {
        case ESapiMediaSourcePictures:
            sapiMediaSourceName = sapiPicturesUrlToken;
            break;
        case ESapiMediaSourceVideos:
            sapiMediaSourceName = sapiVideosUrlToken;
            break;
        case ESapiMediaSourceFiles:
            sapiMediaSourceName = sapiFilesUrlToken;
            break;
        case ESapiMediaSourceContacts:
            sapiMediaSourceName = sapiContactsUrlToken;
            break;
        default:
            sapiMediaSourceName = NULL;
            break;
    }
}

SapiMediaRequestManager::~SapiMediaRequestManager()
{
    delete httpConnection;
    delete jsonSapiMediaObjectParser;
    delete auth;
}

ESapiMediaRequestStatus SapiMediaRequestManager::getItemsCount(int *itemCount)
{
    int status = 0;
    StringBuffer itemsCountRequestUrl;
    URL requestUrl;
    const char* itemsCountJsonObject = NULL; // JSON object from server
    StringOutputStream response;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInvalidParam;
    }
    
    *itemCount = 0;

    itemsCountRequestUrl.sprintf(getCountUriFmt, serverUrl.c_str(), sapiMediaSourceName);
    requestUrl.setURL(itemsCountRequestUrl);

    setSessionAuthParams();
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");

    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
   
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending SAPI media count request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;

            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    if ((itemsCountJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid SAPI media count response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }

    LOG.debug("%s: server response = %s", __FUNCTION__, itemsCountJsonObject);

    if (jsonSapiMediaObjectParser->parseItemCountObject(itemsCountJsonObject, itemCount, &parserStatus) == false) {
        const char* errorCode = jsonSapiMediaObjectParser->getErrorCode().c_str();
        const char* errorMsg  = jsonSapiMediaObjectParser->getErrorMessage().c_str();
        
        if (!errorCode || !errorMsg) {
            switch (parserStatus) {
                case ESMPKeyMissing:
                    // is some required field of JSON object has not been found
                    // consider the SAPI on server as not supported 
                    return ESMRSapiNotSupported;
                
                default:
                    return ESMRSapiMessageParseError;
            }
        }

        LOG.error("SAPI error %s: %s", errorCode, errorMsg);

        if (!strcmp(errorCode, sapiSecurityException)) {               
            return ESMRSecurityException;
        } else if (!strcmp(errorCode, sapiUserIdMissing)) {
            return ESMRUserIdMissing;
        } else if (!strcmp(errorCode, sapiSessionAlreadyOpen)) {             
            return ESMRSessionAlreadyOpen;
        } else if (!strcmp(errorCode, sapiInvalidAuthSchema)) {
            return ESMRInvalidAuthSchema;
        } else if (!strcmp(errorCode, sapiOperationNotSupported)) {
            return ESMRSapiNotSupported;
        }
        
        return ESMRSapiMessageParseError; 
    }
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::getAllItems(ArrayList& sapiItemInfoList, time_t* responseTime, int limit, int offset)
{
    int status = 0;
    StringBuffer itemsListRequestUrl;
    URL requestUrl;
    const char* itemsListJsonObject = NULL; // JSON object from server
    const char* itemsArrayKey = NULL;
    StringOutputStream response;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        LOG.error("%s: sapi source name unset", __FUNCTION__);
        return ESMRInvalidParam;
    }
    
    if ((limit == 0) && (offset == 0)) {
        itemsListRequestUrl.sprintf(getUriFmt, serverUrl.c_str(), sapiMediaSourceName);
    } else if (offset == 0) { // paged get request
        itemsListRequestUrl.sprintf(getWithLimitUriFmt, serverUrl.c_str(), sapiMediaSourceName, limit);
    } else if (limit == 0) {  // only offset       
        itemsListRequestUrl.sprintf(getWithOffsetUriFmt, serverUrl.c_str(), sapiMediaSourceName, offset);
    } else { // both parameters speficied
        itemsListRequestUrl.sprintf(getWithLimitAndOffsetUrlFmt, serverUrl.c_str(), sapiMediaSourceName, limit, offset);
    }
    
    requestUrl.setURL(itemsListRequestUrl);
    
    setSessionAuthParams();
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");

    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
   
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending SAPI media count request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;

            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    if ((itemsListJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid SAPI media count response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }

    LOG.debug("response returned = %s", itemsListJsonObject);

    switch (sourceType) {
        case ESapiMediaSourcePictures:
            itemsArrayKey = sapiJsonArrayPicturesKey; 
            break;
        case ESapiMediaSourceVideos:
            itemsArrayKey = sapiJsonArrayVideosKey;
            break;
        case ESapiMediaSourceFiles:
            itemsArrayKey = sapiJsonArrayFilesKey;
            break;
        case ESapiMediaSourceContacts:
            itemsArrayKey = sapiJsonArrayContactsKey;
            break;
        default:
            break;
    }
    
    if (itemsArrayKey == NULL) {
        LOG.error("%s: can't get valid source name token for json object parse", __FUNCTION__);
        
        return ESMRInternalError; 
    }
    
    if (jsonSapiMediaObjectParser->parseItemsListObject(itemsListJsonObject, itemsArrayKey, sapiItemInfoList, responseTime, &parserStatus) == false) {
        const char* errorCode = jsonSapiMediaObjectParser->getErrorCode().c_str();
        const char* errorMsg  = jsonSapiMediaObjectParser->getErrorMessage().c_str();
        
        if (!errorCode || !errorMsg) {
            switch (parserStatus) {
                case ESMPKeyMissing:
                    // is some required field of JSON object has not been found
                    // consider the SAPI on server as not supported 
                    return ESMRSapiNotSupported;
                
                default:
                    return ESMRSapiMessageParseError;
            }
        }

        LOG.error("SAPI error %s: %s", errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiSecurityException)) {               
            return ESMRSecurityException;
        } else if (!strcmp(errorCode, sapiUserIdMissing)) {
            return ESMRUserIdMissing;
        } else if (!strcmp(errorCode, sapiSessionAlreadyOpen)) {             
            return ESMRSessionAlreadyOpen;
        } else if (!strcmp(errorCode, sapiInvalidAuthSchema)) {
            return ESMRInvalidAuthSchema;
        } else if (!strcmp(errorCode, sapiPictureIdUsedWithLimit)) {
            return ESMRPictureIdUsedWithLimit;
        } else if (!strcmp(errorCode, sapiUnableToRetrievePicture)) {
            return ESMRUnableToRetrievePicture;
        } else if (!strcmp(errorCode, sapiInvalidDataTypeOrFormat)) {
            return ESMRInvalidDataTypeOrFormat;
        } else if (!strcmp(errorCode, sapiOperationNotSupported)) {
            return ESMRSapiNotSupported;
        }
        
        return ESMRSapiMessageParseError; 
    }
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::getItemsFromId(ArrayList& itemsInfoList, const ArrayList& itemsIDs)
{
    int status = 0;
    StringBuffer itemsListRequestUrl;
    URL requestUrl;
    const char* itemsArrayKey = NULL;  
    char* itemsIdsListJsonObject;     // formatted JSON object with items ids
    char* itemsIdsListEncoded = NULL; // urlencoded formatted JSON object with items ids
    const char* itemsListJsonObject = NULL;  // sapi reponse with list of items info JSON objects  
    StringOutputStream response;
    time_t requestTime;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInvalidParam;
    }
    
    if (itemsIDs.size() == 0) {
        LOG.error("%s: list of items id is empty", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if (jsonSapiMediaObjectParser->formatItemsListObject(itemsIDs, &itemsIdsListJsonObject) == false) {
        LOG.error("%s: error formatting json object for items list", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if ((itemsIdsListEncoded = URL::urlEncode(itemsIdsListJsonObject)) == NULL) {
        LOG.error("%s: error url encoding formatted json object with items list", __FUNCTION__);
        free(itemsIdsListJsonObject);
        
        return ESMRInvalidParam;
    }
    
    itemsListRequestUrl.sprintf(getUriWithIdsFmt, serverUrl.c_str(), sapiMediaSourceName, itemsIdsListEncoded);
    
    free(itemsIdsListJsonObject);
    free(itemsIdsListEncoded);
    
    requestUrl.setURL(itemsListRequestUrl);
    
    setSessionAuthParams();
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");

    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
   
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending SAPI media count request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
                
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:   
                return ESMRHTTPFunctionalityNotSupported;
            
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    if ((itemsListJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid SAPI media count response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }

    LOG.debug("response returned = %s", itemsListJsonObject);

    switch (sourceType) {
        case ESapiMediaSourcePictures:
            itemsArrayKey = sapiJsonArrayPicturesKey; 
            break;
        case ESapiMediaSourceVideos:
            itemsArrayKey = sapiJsonArrayVideosKey;
            break;
        case ESapiMediaSourceFiles:
            itemsArrayKey = sapiJsonArrayFilesKey;
            break;
        case ESapiMediaSourceContacts:
            itemsArrayKey = sapiJsonArrayContactsKey;
            break;
        default:
            break;
    }
    
    if (itemsArrayKey == NULL) {
        LOG.error("%s: can't get valid source name token for json object parse", __FUNCTION__);
        
        return ESMRInternalError; 
    }
    
    if (jsonSapiMediaObjectParser->parseItemsListObject(itemsListJsonObject, itemsArrayKey, 
                                        itemsInfoList, &requestTime, &parserStatus) == false) {
        const char* errorCode = jsonSapiMediaObjectParser->getErrorCode().c_str();
        const char* errorMsg  = jsonSapiMediaObjectParser->getErrorMessage().c_str();
        
        if (!errorCode || !errorMsg) {
            LOG.error("%s: error parsing sapi return message", __FUNCTION__);
            switch (parserStatus) {
                case ESMPKeyMissing:
                    // is some required field of JSON object has not been found
                    // consider the SAPI on server as not supported 
                    return ESMRSapiNotSupported;
                
                default:
                    return ESMRSapiMessageParseError;
            }
        }

        LOG.error("%s: SAPI error %s: %s", __FUNCTION__, errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiSecurityException)) {               
            return ESMRSecurityException;
        } else if (!strcmp(errorCode, sapiUserIdMissing)) {
            return ESMRUserIdMissing;
        } else if (!strcmp(errorCode, sapiSessionAlreadyOpen)) {             
            return ESMRSessionAlreadyOpen;
        } else if (!strcmp(errorCode, sapiInvalidAuthSchema)) {
            return ESMRInvalidAuthSchema;
        } else if (!strcmp(errorCode, sapiPictureIdUsedWithLimit)) {
            return ESMRPictureIdUsedWithLimit;
        } else if (!strcmp(errorCode, sapiUnableToRetrievePicture)) {
            return ESMRUnableToRetrievePicture;
        } else if (!strcmp(errorCode, sapiInvalidDataTypeOrFormat)) {
            return ESMRInvalidDataTypeOrFormat;
        } else if (!strcmp(errorCode, sapiOperationNotSupported)) {
            return ESMRSapiNotSupported;
        }
        
        return ESMRSapiMessageParseError; 
    }
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::getItemsChanges(ArrayList& newIDs, ArrayList& modIDs, 
                                                                 ArrayList& delIDs, const StringBuffer& fromDate, 
                                                                 time_t* reponseTimestamp) {
    return getItemsChanges(newIDs, modIDs, delIDs, fromDate, reponseTimestamp, "", "");
    
}


ESapiMediaRequestStatus SapiMediaRequestManager::getItemsChanges(ArrayList& newIDs, ArrayList& modIDs, 
                                    ArrayList& delIDs, const StringBuffer& fromDate, time_t* reponseTimestamp,
                                    const StringBuffer& username, const StringBuffer& password) 
{
    int status = 0;
    StringBuffer itemChangesRequestUrl;
    URL requestUrl;
    const char* itemsChangesJsonObject = NULL; // JSON object from server
    const char* sourceTokenName = NULL;
    StringOutputStream response;
    StringBuffer date = fromDate;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInvalidParam;
    }
    
    if (date.empty()) {
        LOG.debug("%s: timestamp empty: getting all changes from server", __FUNCTION__);
        date = "19700101T000000Z";
    }
    
    if (username.empty() && password.empty()) {
        itemChangesRequestUrl.sprintf(getChangesUrlFmt, serverUrl.c_str(), date.c_str(), sapiMediaSourceName);
        setSessionAuthParams();
    } else {
        itemChangesRequestUrl.sprintf(getChangesUrlUserPassFmt, 
                                      serverUrl.c_str(), date.c_str(), sapiMediaSourceName, username.c_str(), password.c_str());
    }

    
    requestUrl.setURL(itemChangesRequestUrl);

    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");

    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
   
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending SAPI media count request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
        
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    if ((itemsChangesJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }

    LOG.debug("response returned = %s", itemsChangesJsonObject);

    switch (sourceType) {
        case ESapiMediaSourcePictures:
            sourceTokenName = sapiPicturesUrlToken; 
            break;
        case ESapiMediaSourceVideos:
            sourceTokenName = sapiVideosUrlToken;
            break;
        case ESapiMediaSourceFiles:
            sourceTokenName = sapiFilesUrlToken;
            break;
        case ESapiMediaSourceContacts:
            sourceTokenName = sapiContactsUrlToken;
            break;
        default:
            break;
    }
    
    if (sourceTokenName == NULL) {
        LOG.error("%s: can't get valid source name token for json object parse", __FUNCTION__);
        
        return ESMRInternalError; 
    }
    
    *reponseTimestamp = 0L;
    
    if (jsonSapiMediaObjectParser->parseItemsChangesObject(itemsChangesJsonObject, 
                sourceTokenName, newIDs, modIDs, delIDs, reponseTimestamp, &parserStatus) == false) {
        LOG.error("%s: error parsing sapi json object", __FUNCTION__);
   
        switch (parserStatus) {
            case ESMPKeyMissing:
                // is some required field of JSON object has not been found
                // consider the SAPI on server as not supported 
                return ESMRSapiNotSupported;
                
            default:
                return ESMRSapiMessageParseError;
        } 
    }
  
    return ESMRSuccess;
}
       

ESapiMediaRequestStatus SapiMediaRequestManager::uploadItemMetaData(UploadSapiSyncItem* item)
{
    char* itemJsonMetaData;
    const char* itemMetaDataUploadJson = NULL;
    SapiSyncItemInfo* itemInfo = NULL;
    StringBuffer itemId;
    StringBuffer itemMetaDataAddRequestUrl;
    URL requestUrl;
    StringOutputStream response;
    int status = 0;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInternalError;
    }
    
    if (item == NULL) {
        LOG.error("%s: invalid upload sapi item", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if ((itemInfo = item->getSapiSyncItemInfo()) == NULL) {
        LOG.error("%s: invalid upload sapi item", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if ((jsonSapiMediaObjectParser->formatMediaItemMetaData(itemInfo, &itemJsonMetaData)) == false) {
        LOG.error("%s: error formatting item meta data as json object", __FUNCTION__);
    
        return ESMRSapiMessageFormatError;
    }
    
    LOG.debug("JSON request body to send:\n%s", itemJsonMetaData);
   
    itemMetaDataAddRequestUrl.sprintf(saveItemMetaDataFmt, serverUrl.c_str(), sapiMediaSourceName);
    
    requestUrl.setURL(itemMetaDataAddRequestUrl);
    
    setSessionAuthParams();
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT, "*/*");
    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_TYPE, "application/json");

    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodPost)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        
        free(itemJsonMetaData);
        
        return ESMRConnectionSetupError;
    }
    
    if ((status = httpConnection->request(itemJsonMetaData, response)) != HTTP_OK) {
        LOG.error("%s: error sending upload request", __FUNCTION__);
        httpConnection->close();
        free(itemJsonMetaData);
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;

            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }
    
    free(itemJsonMetaData);
    httpConnection->close();
    
    if ((itemMetaDataUploadJson = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid empty response for sapi item metadata upload", __FUNCTION__);
        
        return ESMRSapiInvalidResponse;
    }
    
    LOG.debug("%s: sapi add item metadata request response = %s", __FUNCTION__, itemMetaDataUploadJson);
    
    time_t lastUpdate = 0;  // unused
    if (jsonSapiMediaObjectParser->parseMediaAddItem(itemMetaDataUploadJson, itemId, &lastUpdate, &parserStatus) == false) {
        const char* errorCode = jsonSapiMediaObjectParser->getErrorCode().c_str();
        const char* errorMsg  = jsonSapiMediaObjectParser->getErrorMessage().c_str();
        
        if (!errorCode || !errorMsg) {
            LOG.error("%s: error parsing sapi return message", __FUNCTION__);
            switch (parserStatus) {
                case ESMPKeyMissing:
                    // is some required field of JSON object has not been found
                    // consider the SAPI on server as not supported 
                    return ESMRSapiNotSupported;
                    
                default:
                    return ESMRSapiMessageParseError;
            }
        }

        LOG.error("%s: SAPI error %s: %s", __FUNCTION__, errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiSecurityException)) {               
            return ESMRSecurityException;
        } else if (!strcmp(errorCode, sapiUserIdMissing)) {
            return ESMRUserIdMissing;
        } else if (!strcmp(errorCode, sapiSessionAlreadyOpen)) {             
            return ESMRSessionAlreadyOpen;
        } else if (!strcmp(errorCode, sapiInvalidAuthSchema)) {
            return ESMRInvalidAuthSchema;
        } else if (!strcmp(errorCode, sapiUnknownMediaException)) {
            return ESMRUnknownMediaException;
        } else if (!strcmp(errorCode, sapiInvalidContentRange)) {
            return ESMRInvalidContentRange;
        } else if (!strcmp(errorCode, sapiMissingRequiredParameter)) {
            return ESMRMissingRequiredParameter;
        } else if (!strcmp(errorCode, sapiInvalidLUID)) {
            return ESMRInvalidLUID;
        } else if (!strcmp(errorCode, sapiInvalidLUID)) {
            return ESMRInvalidLUID;
        } else if (!strcmp(errorCode, sapiUserQuotaReached)) {
            return ESMRQuotaExceeded;
        } else if (!strcmp(errorCode, sapiOperationNotSupported)) {
            return ESMRSapiNotSupported;
        }

        return ESMRSapiMessageParseError;
    }
    
    if (itemId.empty() == false) {
        LOG.debug("%s: setting item id '%s'", __FUNCTION__, itemId.c_str());
    
        itemInfo->setGuid(itemId.c_str());
    }
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::uploadItemData(UploadSapiSyncItem* item, time_t* lastUpdate)
{
    int status = 0;
    SapiSyncItemInfo* itemInfo = NULL;
    const char* itemId = NULL, *itemContentType = NULL;
    const char* itemUploadResultJsonObject = NULL;
    InputStream* itemDataStream = NULL;
    StringOutputStream response;
    StringBuffer itemUploadUrl;
    StringBuffer itemGuid;
    URL requestUrl;
    size_t dataSize = 0, partialSize = 0;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInternalError;
    }
        
    if (item == NULL) {
        LOG.error("%s: invalid sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }

    if ((itemInfo = item->getSapiSyncItemInfo()) == NULL) {
        LOG.error("%s: no item info found in sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }
    
    itemId = itemInfo->getGuid();
    if ((itemId == NULL) && (strlen(itemId) == 0)) {
        LOG.error("%s: no guid found in item info", __FUNCTION__);
    
        return ESMRInvalidParam;
    }
    
    itemContentType = itemInfo->getContentType();
     
    if ((itemDataStream = item->getStream()) == NULL) {
        LOG.error("%s: invalid output stream in sync item", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if ((dataSize = itemDataStream->getTotalSize()) == 0) {
        LOG.error("%s: invalid sync item stream (zero size stream attached)", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    partialSize = itemDataStream->getPosition();
    
    itemUploadUrl.sprintf(saveItemData, serverUrl.c_str(), sapiMediaSourceName);
    requestUrl.setURL(itemUploadUrl);

    fireTransportEvent(itemInfo->getSize(), SEND_DATA_BEGIN);

    // set requst headers
    setSessionAuthParams();
    httpConnection->setKeepAlive(false);
    if (partialSize > 0) {
        StringBuffer contentRange;
        contentRange.sprintf("bytes %lu-%lu/%lu", partialSize, dataSize - 1, dataSize);
        httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_RANGE, contentRange.c_str());
        fireTransportEvent(partialSize, DATA_ALREADY_COMPLETED);
     }

    if ((itemContentType != NULL) && (strlen(itemContentType) > 0)) {
        httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_TYPE, itemContentType);
    } else {
        httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_TYPE, "application/octet-stream");
    }
    
    // set Funambol mandatory custom headers
    httpConnection->setRequestHeader(HTTP_HEADER_X_FUNAMBOL_ID, itemId);
    StringBuffer dsize;
    dsize.sprintf("%lu", dataSize);
    httpConnection->setRequestHeader(HTTP_HEADER_X_FUNAMBOL_FILE_SIZE, dsize.c_str());
    
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodPost)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    status = httpConnection->request(*itemDataStream, response);
    
    if ((status != HTTP_OK) && (status != HTTP_PARTIAL_CONTENT)) {
        LOG.error("%s: error sending upload request", __FUNCTION__);
        httpConnection->close();

        fireTransportEvent(0, SEND_DATA_END);
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
        
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }
    
    httpConnection->close();

    fireTransportEvent(item->getSapiSyncItemInfo()->getSize(), SEND_DATA_END);

    if ((itemUploadResultJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }
    
    LOG.debug("%s: sapi item add result = %s", __FUNCTION__, response.getString().c_str());
    
    if (jsonSapiMediaObjectParser->parseMediaAddItem(itemUploadResultJsonObject, itemGuid, lastUpdate, &parserStatus) == false) {
        const char* errorCode = jsonSapiMediaObjectParser->getErrorCode().c_str();
        const char* errorMsg  = jsonSapiMediaObjectParser->getErrorMessage().c_str();
        
        if (!errorCode || !errorMsg) {
            LOG.error("%s: error parsing sapi return message", __FUNCTION__);
           
            switch (parserStatus) {
                case ESMPKeyMissing:
                    // is some required field of JSON object has not been found
                    // consider the SAPI on server as not supported 
                    return ESMRSapiNotSupported;
                    
                default:
                    return ESMRSapiMessageParseError;
            } 
        }

        LOG.error("%s: SAPI error %s: %s", __FUNCTION__, errorCode, errorMsg);
        
        if (!strcmp(errorCode, sapiSecurityException)) {               
            return ESMRSecurityException;
        } else if (!strcmp(errorCode, sapiUserIdMissing)) {
            return ESMRUserIdMissing;
        } else if (!strcmp(errorCode, sapiSessionAlreadyOpen)) {             
            return ESMRSessionAlreadyOpen;
        } else if (!strcmp(errorCode, sapiInvalidAuthSchema)) {
            return ESMRInvalidAuthSchema;
        } else if (!strcmp(errorCode, sapiUnknownMediaException)) {
            return ESMRUnknownMediaException;
        } else if (!strcmp(errorCode, sapiInvalidContentRange)) {
            return ESMRInvalidContentRange;
        } else if (!strcmp(errorCode, sapiMissingRequiredParameter)) {
            return ESMRMissingRequiredParameter;
        } else if (!strcmp(errorCode, sapiInvalidLUID)) {
            return ESMRInvalidLUID;
        } else if (!strcmp(errorCode, sapiInvalidLUID)) {
            return ESMRInvalidLUID;
        } else if (!strcmp(errorCode, sapiUserQuotaReached)) {
            return ESMRQuotaExceeded;
        } else if (!strcmp(errorCode, sapiOperationNotSupported)) {
            return ESMRSapiNotSupported;
        }

        return ESMRSapiMessageParseError;
    }
    
    if (itemGuid != itemId) {
        LOG.error("%s: sapi add item error: item id mismatch!", __FUNCTION__);
        return ESMRSapiInvalidResponse;
    }
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::getItemResumeInfo(UploadSapiSyncItem* item, size_t* offset)
{
    int status = 0;
    SapiSyncItemInfo* itemInfo = NULL;
    const char* itemId = NULL;
    InputStream* itemDataStream = NULL;
    StringOutputStream response;
    StringBuffer itemResumeInfoUrl;
    StringBuffer contentRange;
    URL requestUrl;
    size_t dataSize = 0;
    
    *offset = 0;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInternalError;
    }
        
    if (item == NULL) {
        LOG.error("%s: invalid sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }

    if ((itemInfo = item->getSapiSyncItemInfo()) == NULL) {
        LOG.error("%s: no item info found in sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }
    
    if ((itemDataStream = item->getStream()) == NULL) {
        LOG.error("%s: invalid output stream in sync item", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    if ((dataSize = itemDataStream->getTotalSize()) == 0) {
        LOG.error("%s: invalid sync item stream (zero size stream attached)", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    itemId = itemInfo->getGuid();
    if ((itemId == NULL) && (strlen(itemId) == 0)) {
        LOG.error("%s: no guid found in item info", __FUNCTION__);
    
        return ESMRInvalidParam;
    }

    itemResumeInfoUrl.sprintf(saveItemData, serverUrl.c_str(), sapiMediaSourceName);
    requestUrl.setURL(itemResumeInfoUrl);

    // set requst headers
    setSessionAuthParams();
    httpConnection->setKeepAlive(false);
    
    contentRange.sprintf("bytes */%lu", dataSize);

    httpConnection->setRequestHeader(HTTP_HEADER_CONTENT_RANGE, contentRange.c_str());
    // set Funambol mandatory custom headers
    httpConnection->setRequestHeader(HTTP_HEADER_X_FUNAMBOL_ID, itemId);

    StringBuffer dsize;
    dsize.sprintf("%lu", dataSize);
    httpConnection->setRequestHeader(HTTP_HEADER_X_FUNAMBOL_FILE_SIZE, dsize.c_str());
    
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
     
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    status = httpConnection->request(NULL, response);
    
    if (status == HTTP_OK) {
        LOG.debug("%s: item upload was complete", __FUNCTION__);
        *offset = dataSize;
    } else if (status == HTTP_RESUME_INCOMPLETE) {
        StringBuffer rangeHdr = httpConnection->getResponseHeader("Range");
        ArrayList rangeValues;
        
        rangeHdr.split(rangeValues, "-");
        
        if (rangeValues.size() != 2) {
            LOG.error("%s: error parsing HTTP range headers", __FUNCTION__);
            httpConnection->close();
            
            return ESMRGenericHttpError;
        } else {
            StringBuffer* rangeOffset = static_cast<StringBuffer *>(rangeValues.get(1));
        
            if ((rangeOffset == NULL) || (rangeOffset->empty())) {
                LOG.error("%s: error parsing HTTP range headers", __FUNCTION__);
                httpConnection->close();
                
                return ESMRGenericHttpError;
            }
            
            *offset = atol(rangeOffset->c_str());
            LOG.debug("%s: item offset set to: %lu", __FUNCTION__, *offset);
        }
    } else {
        LOG.error("%s: error sending HTTP item resume info request [HTTP code: %d]", __FUNCTION__, status);
        httpConnection->close();

        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
        
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            default:
                break;
        }
       
        return ESMRGenericHttpError;
    }

    httpConnection->close();

    return ESMRSuccess;
}


ESapiMediaRequestStatus SapiMediaRequestManager::downloadItem(DownloadSapiSyncItem* item)
{
    int status = 0;
    SapiSyncItemInfo* itemInfo = NULL;
    OutputStream* itemDataStream = NULL;
    const char* itemUrlFmt = "%s%s";
    const char* itemUrl    = NULL;
    StringBuffer itemServerUrl;
    StringBuffer itemRequestUrl;
    URL requestUrl;
    size_t partialSize = 0;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInternalError;
    }
    
    if (item == NULL) {
        LOG.error("%s: invalid sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }

    if ((itemInfo = item->getSapiSyncItemInfo()) == NULL) {
        LOG.error("%s: no item info found in sync item", __FUNCTION__);
         
        return ESMRInvalidParam;
    }
    
    // get server name from item info  
    itemServerUrl = itemInfo->getServerUrl();
    itemUrl = itemInfo->getUrl();
    
    if ((itemUrl == NULL) || (strlen(itemUrl) == 0)) {
        LOG.error("%s: no download url found in item info", __FUNCTION__);
         
        return ESMRInvalidParam;
    }
    
    if ((itemDataStream = item->getStream()) == NULL) {
        LOG.error("%s: invalid output stream in sync item", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
   
    // format request URL: if item server url is not set, use default (set from client)
    itemRequestUrl.sprintf(itemUrlFmt, itemServerUrl.empty() ? serverUrl.c_str() : itemServerUrl.c_str(), itemUrl);
    
    LOG.debug("%s: media item url to fetch: %s", __FUNCTION__, itemRequestUrl.c_str());
     
    requestUrl.setURL(itemRequestUrl);

    setSessionAuthParams();
    httpConnection->setKeepAlive(false);
    
    // check if output stream has already data set and we have to make a resume request
    partialSize = itemDataStream->size();
    
    if (partialSize > 0) {
        StringBuffer contentRange;
        size_t dataSize = 0;
        
        // get total sapi item size from item info
        if ((dataSize = itemInfo->getSize()) == 0) {
            LOG.error("%s: item info has zero size - can't complete download request", __FUNCTION__);
            return ESMRInvalidParam;
        }

        // some checks on the partial data size
        if (partialSize == dataSize) {
            LOG.debug("%s: partial item is already the whole content, no download is done", __FUNCTION__);
            return ESMRSuccess;
        }
        else if (partialSize > dataSize) {
            LOG.error("%s: partial item downloaded is bigger (%d bytes) than the final item size (%d bytes)", 
                __FUNCTION__, partialSize, dataSize);
            LOG.info("item '%s' can't be resumed, will be downloaded from scratch next time", itemInfo->getName().c_str());
            return ESMRInvalidContentRange;
        }
        
        contentRange.sprintf("bytes=%lu-%lu", partialSize, dataSize - 1);
        httpConnection->setRequestHeader(HTTP_HEADER_RANGE, contentRange.c_str());
    }
    
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        
        return ESMRConnectionSetupError; // malformed URI, etc
    }
    
    // fire the begin of the download with the size of the data to download
    fireTransportEvent(itemInfo->getSize(), RECEIVE_DATA_BEGIN);
    if (partialSize > 0) {
        fireTransportEvent(partialSize, DATA_ALREADY_COMPLETED);
    }

    status = httpConnection->request(NULL, *itemDataStream);
    
    if ((status != HTTP_OK) && (status != HTTP_PARTIAL_CONTENT)) {
        LOG.error("%s: error dowloading item (http status = %d)", __FUNCTION__, status);
        httpConnection->close();
        
        fireTransportEvent(0, RECEIVE_DATA_END);

        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;

            case HTTP_NOT_FOUND:
                return ESMRErrorRetrievingMediaItem;

            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            case HTTP_RANGE_ERROR:
                return ESMRInvalidContentRange;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    fireTransportEvent(item->getSapiSyncItemInfo()->getSize(), RECEIVE_DATA_END);

    return ESMRSuccess;
}


ESapiMediaRequestStatus SapiMediaRequestManager::getQuotaInfo(unsigned long long* free, unsigned long long* quota)
{
    int status = 0;
    StringBuffer quotaInfoUrl;
    URL requestUrl;
    const char* quotaInfoJsonObject = NULL; // JSON object from server
    StringOutputStream response;
    ESMPStatus parserStatus;
    
    if (sapiMediaSourceName == NULL) {
        return ESMRInvalidParam;
    }
    
    quotaInfoUrl.sprintf(getQuotaInfoUrlFmt, serverUrl.c_str(), sapiMediaSourceName);
    requestUrl.setURL(quotaInfoUrl);

    setSessionAuthParams();
    httpConnection->setKeepAlive(false);
  
    if ((status = httpConnection->open(requestUrl, HttpConnection::MethodGet)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
   
        return ESMRConnectionSetupError; // malformed URI, etc
    }

    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        LOG.error("%s: error sending SAPI media count request", __FUNCTION__);
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;

            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
           
            case HttpConnection::StatusTimeoutError:
                return ESMRRequestTimeout;

            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericError;
        }
    }

    httpConnection->close();

    if ((quotaInfoJsonObject = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi response", __FUNCTION__);

        return ESMRSapiInvalidResponse;
    }

    LOG.debug("response returned = %s", quotaInfoJsonObject);

    if (jsonSapiMediaObjectParser->parseQuotaInfoObject(quotaInfoJsonObject, free, quota, &parserStatus) == false) {
        LOG.error("%s: error parsing sapi json object", __FUNCTION__);
   
        switch (parserStatus) {
            case ESMPKeyMissing:
                // is some required field of JSON object has not been found
                // consider the SAPI on server as not supported 
                return ESMRSapiNotSupported;
                
            default:
                return ESMRSapiMessageParseError;
        }
    }
  
    return ESMRSuccess;
}

//
// SAPI authentication methods
//

ESapiMediaRequestStatus SapiMediaRequestManager::login(const char* device_id, time_t* serverTime, unsigned long * expiretime, StringMap* sourcesStringMap, StringMap* propertyStringMap)
{
    int status = 0;
    StringOutputStream response;            // sapi response buffer
    StringBuffer sapiLoginUrl;
    URL requestUrl;
    const char* sapiLoginResponse = NULL;
    ESMPStatus parserStatus;
    
    sessionID.reset();
    
    // check params
    if ((device_id == NULL) || (strlen(device_id) == 0)) {
        LOG.error("%s: invalid device id parameter for SAPI login", __FUNCTION__);
        
        return ESMRInvalidParam;
    }
    
    // check class members 
    if (((username == NULL) || (password == NULL)) ||
            ((strlen(username) == 0) || (strlen(password) == 0))) {
        LOG.error("%s: invalid parameters for SAPI login", __FUNCTION__);
        
        return ESMRInvalidParam;
    }

    // Urlencode the deviceId parameter (may contain unacceptable chars)
    const char* deviceIdEncoded = URL::urlEncode(device_id);
    sapiLoginUrl.sprintf(sapiLoginUrlFmt, serverUrl.c_str(), username, password, deviceIdEncoded);
    delete [] deviceIdEncoded;
    
    // Request additional "details" for Service profiling
    if (sourcesStringMap != NULL && propertyStringMap != NULL) {
        sapiLoginUrl.append("&details=true");
    }

    requestUrl = sapiLoginUrl.c_str();
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    
    if ((status = httpConnection->open(requestUrl, AbstractHttpConnection::MethodGet, false)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        return ESMRConnectionSetupError; // malformed URI, etc
    }
    
    if ((status = httpConnection->request(NULL, response, false)) != HTTP_OK) {
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
       
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
            
            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericHttpError;
        }
    }
    
    httpConnection->close();
        
    if ((sapiLoginResponse = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi login response", __FUNCTION__);
        
        return ESMRSapiInvalidResponse;
    }
    
    LOG.debug("response returned = %s", sapiLoginResponse);
    
    // get session ID from JSON object

	

    if ((status = jsonSapiMediaObjectParser->parseLogin(sapiLoginResponse, sessionID, serverTime, &parserStatus, expiretime, sourcesStringMap, propertyStringMap)) == false) {
        LOG.error("%s: error parsing sapi login response", __FUNCTION__);
        
        switch (parserStatus) {
            case ESMPKeyMissing:
                // is some required field of JSON object has not been found
                // consider the SAPI on server as not supported 
                return ESMRSapiNotSupported;
                
            default:
                return ESMRSapiMessageParseError;
        }
    }
    
    LOG.debug("%s: SAPI session id: \"%s\"", __FUNCTION__, sessionID.c_str());
    
    return ESMRSuccess;
}

ESapiMediaRequestStatus SapiMediaRequestManager::logout()
{
    int status = 0;
    StringOutputStream response;            // sapi response buffer
    StringBuffer logoutUrl;
    URL requestUrl;
    const char* sapiLogoutResponse = NULL;
    
    logoutUrl.sprintf(sapiLogoutUrlFmt, serverUrl.c_str());
    
    requestUrl.setURL(logoutUrl.c_str());
    
    httpConnection->setKeepAlive(false);
    httpConnection->setRequestHeader(HTTP_HEADER_ACCEPT,      "*/*");
    
    setSessionAuthParams();
    
    if ((status = httpConnection->open(requestUrl)) != 0) {
        LOG.error("%s: error opening connection", __FUNCTION__);
        
        return ESMRConnectionSetupError; // malformed URI, etc
    }
    
    if ((status = httpConnection->request(NULL, response)) != HTTP_OK) {
        httpConnection->close();
        
        switch (status) {
            case HTTP_UNAUTHORIZED:
                return ESMRAccessDenied;
       
            case HTTP_FUNCTIONALITY_NOT_SUPPORTED:
            case HTTP_NOT_FOUND:
                return ESMRHTTPFunctionalityNotSupported;
       
            case HttpConnection::StatusNetworkError:
            case HttpConnection::StatusReadingError:
            case HttpConnection::StatusWritingError:
                return ESMRNetworkError;
            
            // handle here other cases that need
            // special return values 
            default:
                return ESMRGenericHttpError;
        }
    }
    
    httpConnection->close();
        
    if ((sapiLogoutResponse = response.getString().c_str()) == NULL) {
        LOG.error("%s: invalid sapi logout response", __FUNCTION__);
        
        return ESMRSapiInvalidResponse;
    }
    
    LOG.debug("response returned = %s", sapiLogoutResponse);
    
    return ESMRSuccess;
}

/**
 * manage SAPI authentication via jsessionid cookie or basic auth
 */
void SapiMediaRequestManager::setSessionAuthParams()
{
    if (httpConnection) {
        if (sessionID.empty()) {
            if (auth == NULL) {
                auth = new BasicAuthentication(username, password);
            }
            
            httpConnection->setAuthentication(auth);
        } else {
            StringBuffer sessionIdCookie;
            
            sessionIdCookie.sprintf("JSESSIONID=%s", sessionID.c_str());
            httpConnection->setRequestHeader(HTTP_HEADER_COOKIE, sessionIdCookie.c_str());
        }
    }
}

void SapiMediaRequestManager::setRequestTimeout(const int timeout) {
    httpConnection->setRequestTimeout(timeout);
}

void SapiMediaRequestManager::setResponseTimeout(const int timeout) {
    httpConnection->setResponseTimeout(timeout);
}

void SapiMediaRequestManager::setUploadChunkSize(const int size) {
    httpConnection->setRequestChunkSize(size);
}

void SapiMediaRequestManager::setDownloadChunkSize(const int size) {
    httpConnection->setResponseChunkSize(size);
}


END_FUNAMBOL_NAMESPACE
