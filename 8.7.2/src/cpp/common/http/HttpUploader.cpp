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


#include "base/util/utils.h"
#include "http/HttpUploader.h"
#include "http/BasicAuthentication.h"
#include "base/globalsdef.h"
#include "inputStream/BufferInputStream.h"
#include "inputStream/StringOutputStream.h"

BEGIN_FUNAMBOL_NAMESPACE


/**
 * This is a small class used to test HttpConnection class, platform indipendent.
 */
class TestHttpConnection {
    
private:
    HttpConnection httpConnection;

public:
    TestHttpConnection() : httpConnection("test UserAgent") {}
    ~TestHttpConnection() {}

    /**
     * Tests a GET on a specific URL, prints the response.
     */
    void testGET(const URL& testURL) 
    {
        LOG.debug("test GET on %s", testURL.fullURL);
        int ret = httpConnection.open(testURL, HttpConnection::MethodGet);
        LOG.debug("open, ret = %d", ret);
        
        BufferInputStream inputStream("");
        StringOutputStream outputStream;
        
        httpConnection.setRequestHeader(HTTP_HEADER_ACCEPT,          "*/*");
        httpConnection.setRequestHeader(HTTP_HEADER_CONTENT_LENGTH,  0);

        ret = httpConnection.request(inputStream, outputStream);
        LOG.debug("request, ret = %d", ret);
        LOG.debug("response = \n%s", outputStream.getString().c_str());
        
        httpConnection.close();
    }
};



HttpUploader::HttpUploader() {
    useSessionID = false;
    maxRequestChunkSize = DEFAULT_REQUEST_MAX_CHUNK_SIZE;
    keepalive = false;
}



int HttpUploader::upload(const StringBuffer& luid, InputStream* inputStream) 
{
    int status = 0;

    // safe checks
    if (!inputStream || !inputStream->getTotalSize()) {
        LOG.error("upload error: no data to transfer");
        return 1;
    }
    if (luid.empty() || syncUrl.empty()  || sourceURI.empty()) {
        LOG.error("upload error: some params are not set");
        return 2;
    }
    
    StringBuffer fullUrl = composeURL();
    URL url(fullUrl.c_str());
    HttpConnection* httpConnection = getHttpConnection();
   
    httpConnection->setCompression(false);
    status = httpConnection->open(url, HttpConnection::MethodPost);
    
    if (status) { 
        delete httpConnection;

        return status;
    }

    httpConnection->setKeepAlive(keepalive);
    httpConnection->setChunkSize(maxRequestChunkSize);
   
    // Set headers (use basic auth)
    HttpAuthentication* auth = new BasicAuthentication(username, password);
    httpConnection->setAuthentication(auth);
    setRequestHeaders(luid, *httpConnection, *inputStream);
    
    // Send the HTTP request
    StringOutputStream response;
    status = httpConnection->request(*inputStream, response);
    LOG.debug("response returned = %s", response.getString().c_str());
    
    // Manage response headers
    if (useSessionID) {
        // Server returns the jsessionId in the Set-Cookie header, can be used for 
        // the subsequent calls of upload().
        StringBuffer hdr = httpConnection->getResponseHeader(HTTP_HEADER_SET_COOKIE);
        sessionID = parseJSessionId(hdr);
    }
    
    httpConnection->close();
    
    delete auth;
    delete httpConnection;
    return status;
}


void HttpUploader::setRequestHeaders(const StringBuffer& luid, HttpConnection& httpConnection, InputStream& inputStream) {
    
    StringBuffer dataSize;
    dataSize.sprintf("%d", inputStream.getTotalSize());
    
    httpConnection.setRequestHeader(HTTP_HEADER_ACCEPT,         "*/*");
    httpConnection.setRequestHeader(HTTP_HEADER_CONTENT_TYPE,   "application/octet-stream");

    // set transfer enconding to chunked
    //httpConnection.setRequestHeader(HTTP_HEADER_TRANSFER_ENCODING, "chunked");

    // set Funambol mandatory custom headers
    httpConnection.setRequestHeader(HTTP_HEADER_X_FUNAMBOL_FILE_SIZE, dataSize);
    httpConnection.setRequestHeader(HTTP_HEADER_X_FUNAMBOL_DEVICE_ID, deviceID);
    httpConnection.setRequestHeader(HTTP_HEADER_X_FUNAMBOL_LUID, luid);
}


StringBuffer HttpUploader::composeURL() {
    
    // Get the host & port info
    URL url;
    url.setURL(syncUrl.c_str());
    StringBuffer port(":80");
    if (url.port != 0) { 
        port = ":"; 
        port += itow(url.port); 
    }
    
    // Compose the url
    StringBuffer ret(url.protocol);
    ret += "://"; 
    ret += url.host;
    ret += port;
    ret += "/";
    ret += UPLOAD_URL_RESOURCE;
    ret += "/";
    ret += sourceURI;

    // Add parameters
    if (useSessionID && !sessionID.empty()) {
        ret += ";jsession=";
        ret += sessionID; 
    }
    ret += "?action=content-upload";
    
    //LOG.debug("destination url = %s", ret.c_str());

    return ret;
}

StringBuffer HttpUploader::parseJSessionId(const StringBuffer& input) {

    StringBuffer ret;
    if (input.empty()) {
        return ret;
    }

    // Get the right attribute to parse (there may be more attributes)
    // example of input: "JSESSIONID=75A5395778290093FAEEA6589D1F23D6; Path=/"
    bool found = false;
    StringBuffer attribute;
    ArrayList attributes;
    input.split(attributes, ";");
    for (int i=0; i<attributes.size(); i++) {
        attribute = *(StringBuffer*)attributes[i];
        attribute.trim();
        if (attribute.ifind("JSESSIONID") != StringBuffer::npos) {
            found = true;
            break;
        }
    }

    if (found) {
        ArrayList tokens;
        attribute.split(tokens, "=");

        StringBuffer* prop = (StringBuffer*)tokens.get(0);
        if (prop && prop->icmp("JSESSIONID")) {
            StringBuffer* val = (StringBuffer*)tokens.get(1);
            if (val && !val->empty()) {
                ret = *val;
            }
        }
    }
    return ret;
}


END_FUNAMBOL_NAMESPACE
