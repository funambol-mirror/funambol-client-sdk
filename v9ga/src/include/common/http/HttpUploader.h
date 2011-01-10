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

#ifndef INCL_HTTP_UPLOADER
#define INCL_HTTP_UPLOADER
/** @cond DEV */

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/constants.h"
#include "http/constants.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "inputStream/InputStream.h"
#include "inputStream/StringOutputStream.h"
#include "http/URL.h"
#include "http/HttpConnection.h"

BEGIN_NAMESPACE


#define UPLOAD_URL_RESOURCE     "sapi/media"

/**
 * This module is used to upload a given amount of data via an HTTP POST.
 * The destination URL is fixed, it's composed based on the params set:
 *   "http://<serverURL>[:port]/upload/<sourceURI>?LUID=<luid>"
 * The params username and password are used for the HTTP Basic Authentication:
 *   "Authorization: Basic <b64(username:password)>"
 * The deviceID is added in the http headers: "deviceId: <deviceID>"
 * All the params MUST be set before calling upload() method.
 *
 * This class used by MediaSyncSource, to upload media files at the end of the sync.
 */
class HttpUploader {

private:
    StringBuffer syncUrl;       /**< The Sync Server URL, to compose the HTTP destination URL. */
    StringBuffer sourceURI;     /**< The sourceURI, to compose the HTTP destination URL. */
    StringBuffer username;      /**< The current username, for HTTP authentication. */
    StringBuffer password;      /**< The current password, for HTTP authentication.. */
    StringBuffer deviceID;      /**< The device id, specified in HTTP headers. */
    StringBuffer userAgent;     /**< The user agent, specified in HTTP headers. */
    int partialUploadedData;    /**< The partial data already uplaoded in a previous session **/
    int totalDataToUpload;      /**< The total data to be uploaded. If > 0, it is used in the header
                                     instead of the value from inputStream.getTotalBytes 
                                     Usually it must be used in conjunction with partialUplaodedData **/
    
    bool useSessionID;          /**< if false, we never use sessionID (it's optional) */
    StringBuffer sessionID;     /**< The session id, returned by the server */

    int maxRequestChunkSize;    /**< The max chunk size, for the HTTP reqest (outgoing) */

    bool keepalive;             /**< If set to true, the connection is not dropped on destructor. Default = false. */

public:

    HttpUploader();
    virtual ~HttpUploader() { }

    /**
     * Makes an HTTP POST request, to the destination URL.
     * Note: url and sourceURI must be set before calling this method.
     *
     * @param luid         the LUID of item to send, used to compose the HTTP destination URL
     * @param inputStream  pointer to the input stream, ready to read the data to send
     *                     (usually it's a fileInputStream)
     * @return             the response status for this HTTP request (200 = OK)
     */
    virtual int upload(const StringBuffer& luid, InputStream* inputStream);


    void setSyncUrl      (const StringBuffer& v) { syncUrl   = v; }
    void setSourceURI    (const StringBuffer& v) { sourceURI = v; }
    void setUsername     (const StringBuffer& v) { username  = v; }
    void setPassword     (const StringBuffer& v) { password  = v; }
    void setDeviceID     (const StringBuffer& v) { deviceID  = v; }
    void setUserAgent    (const StringBuffer& v) { userAgent = v; }
    void setUseSessionID (const bool v)       { useSessionID = v; }
    void setMaxRequestChunkSize(const int v) { maxRequestChunkSize = v; }
    void setPartialUploadedData (int v)     { partialUploadedData = v; }
    void setTotalDataToUpload (int v)       { totalDataToUpload = v; }
    /**
     * Sets the keep-alive flag: the connection is not dropped at the end of each upload.
     * Note: on some platforms this is not applicable, so it's just ignored.
     */
    virtual void setKeepAlive(bool val) { keepalive = val; }

    StringBuffer getSessionID() {
        return sessionID;
    }

protected:

    /// Compose and return the destination URL for the upload.
    StringBuffer composeURL();

    /// Sets all the headers for the http request.
    void setRequestHeaders(const StringBuffer& luid, HttpConnection& httpConnection, InputStream& inputStream);

    /**
     * Returns a new allocated HttpConnection.
     * It's used by the upload() method, the returned object is then deleted.
     */
    virtual HttpConnection* getHttpConnection() {
        return new HttpConnection(userAgent);
    }
};

END_NAMESPACE

/** @endcond */
#endif

