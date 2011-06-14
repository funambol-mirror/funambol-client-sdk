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

#ifndef __SAPI_MEDIA_REQUEST_MANAGER_H__
#define __SAPI_MEDIA_REQUEST_MANAGER_H__

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"
#include "sapi/UploadSapiSyncItem.h"
#include "sapi/DownloadSapiSyncItem.h"

BEGIN_FUNAMBOL_NAMESPACE

typedef enum ESapiMediaRequestStatus 
{
    ESMRSuccess = 0,
    ESMRConnectionSetupError,               // Error setting up the connection
    ESMRAccessDenied,                   
    ESMRGenericHttpError,                   // Http error
    ESMRSapiInvalidResponse,                // Bad SAPI response received
    ESMRSapiMessageParseError,              // Error occurred parsing the JSON body received
    ESMRSapiMessageFormatError,             // Error occurred formatting the JSON body to send
    ESMRInvalidParam,                       // An invalid parameter is passed
    ESMRNetworkError,                       // Network error 
    ESMRRequestTimeout,                         
    ESMRHTTPFunctionalityNotSupported,      // 501 error from the server (check fields on server not supporting this sapi)
    ESMRSapiNotSupported,                   // sapi incompatible version on server (missing needed parameters in json responses, etc..) 
    
    // SAPI request failure codes
    ESMRSecurityException,
    ESMRUserIdMissing,
    ESMRSessionAlreadyOpen,
    ESMRInvalidValidationKey,
    ESMRInvalidAuthSchema,

    ESMRUnknownMediaException, 
    ESMRMediaIdWithLimit,
    ESMRErrorRetrievingMediaItem,
    ESMRInvalidContentRange,

    ESMRPictureGenericError,
    ESMRNoPicturesSpecfied,
    ESMRPictureIdUsedWithLimit,
    ESMRUnableToRetrievePicture,
    ESMRQuotaExceeded,
    ESMRFileTooLarge,

    ESMRInvalidDataTypeOrFormat,
    ESMRInvalidFileName,
    ESMRMissingRequiredParameter,
    ESMRInvalidEmptyDataParameter,
    ESMRInvalidLUID,
    ESMRInvalidDeviceId,
    
    // generic error
    ESMRInternalError,                       // internal data consistency error
    ESMRGenericError
} ESMRStatus;

typedef enum ESapiMediaSourceType
{
    ESapiMediaSourceUndefined  = 0,
    ESapiMediaSourcePictures,
    ESapiMediaSourceVideos,
    ESapiMediaSourceFiles
} SapiMediaSourceType;

class HttpConnection;
class SapiMediaJsonParser;
class HttpAuthentication;

class SapiMediaRequestManager
{
    private:    
        StringBuffer serverUrl;             
        SapiMediaSourceType sourceType;    // source name that will be used in all sapi requests
 
        SapiMediaJsonParser* jsonSapiMediaObjectParser;
        HttpConnection* httpConnection;
        HttpAuthentication* auth;
        
        const char* sapiMediaSourceName;
        const char* username;
        const char* password;
        
        StringBuffer sessionID;
        
    public:
        SapiMediaRequestManager(const char* url, SapiMediaSourceType mediaSourceType, const char* user_agent, 
                                const char* username, const char* password);
        virtual ~SapiMediaRequestManager();

        virtual ESapiMediaRequestStatus getItemsCount(int *count);
        virtual ESapiMediaRequestStatus getAllItems(ArrayList& sapiItemInfoList, time_t* responseTime, int limit = 0, int offset = 0);
        virtual ESapiMediaRequestStatus getItemsFromId(ArrayList& items, const ArrayList& itemsIDs);
        virtual ESapiMediaRequestStatus getItemsChanges(ArrayList& newIDs, ArrayList& modIDs, ArrayList& delIDs, 
                                                        const StringBuffer& fromDate, time_t* requestTimestamp);
        virtual ESapiMediaRequestStatus downloadItem(DownloadSapiSyncItem* item);
        virtual ESapiMediaRequestStatus getItemResumeInfo(UploadSapiSyncItem* item, size_t* offset);
        virtual ESapiMediaRequestStatus getQuotaInfo(unsigned long long* free, unsigned long long* quota);
        
        // item upload/update methods
        virtual ESapiMediaRequestStatus uploadItemMetaData(UploadSapiSyncItem* item);
        virtual ESapiMediaRequestStatus uploadItemData(UploadSapiSyncItem* item, time_t* lastUpdate);

        // SAPI session set up methods
        virtual ESapiMediaRequestStatus login(const char* device_id, time_t* serverTime = NULL);
        virtual ESapiMediaRequestStatus logout();

        /**
         * Sets the timeout for the http requests (upload) into httpConnection.
         * @param timeout  the timeout, in seconds
         */
        void setRequestTimeout(const int timeout);

        /**
         * Sets the timeout for the http responses from the server (download) into httpConnection.
         * @param timeout  the timeout, in seconds
         */
        void setResponseTimeout(const int timeout);
        /**
         * Sets the upload http chunk size, for http requests into httpConnection.
         * @param size  the chunk size, in bytes
         */
        void setUploadChunkSize(const int size);

        /**
         * Sets the download http chunk size, for http responses into httpConnection.
         * @param size  the chunk size, in bytes
         */
        void setDownloadChunkSize(const int size);
        
    private:
        void setSessionAuthParams();
        
};

END_FUNAMBOL_NAMESPACE

#endif
