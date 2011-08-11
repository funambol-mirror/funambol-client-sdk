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

#include "HttpConnection.h"
#include "HttpConnectionHandler.h"
#include "base/util/StringUtils.h"
#include "ioStream/AppleBufferInputStream.h"

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

BEGIN_FUNAMBOL_NAMESPACE

HttpConnection::HttpConnection(const char* user_agent) : AbstractHttpConnection(user_agent),
                                                         clientRequest(NULL),
                                                         responseStream(NULL), 
                                                         gotflags(true), 
                                                         isReachable(true),
                                                         noConnectionRequired(true),
                                                         http_verb(NULL),
                                                         contentLen(0)
{}

HttpConnection::~HttpConnection()
{
    close();
}

int HttpConnection::open(const URL& url, RequestMethod method, bool log_request)
{
    CFStringRef CFurl = nil;
    CFURLRef requestURL = nil;
        
    if ((url.fullURL == NULL) || (strlen(url.fullURL) == 0)) {
        setErrorF(ERR_HTTP_INVALID_URL, "%s - can't open connection: invalid url.", __FUNCTION__);
        LOG.error("%s: can't open connection: invalid url.", __FUNCTION__);
        requestHeaders.clear();
    
        return 1;
    }

    switch (method) {
        case MethodGet:
            http_verb = CFSTR(METHOD_GET);
            break;
        case MethodPost:
            http_verb = CFSTR(METHOD_POST);
            break;
        default:
            requestHeaders.clear();

            // HTTP method not supported...
            LOG.error("%s: unsupported HTTP request type", __FUNCTION__);
            return 1;
    }
    
    this->url = url;

    // Construct URL
    CFurl =  CFStringCreateWithCString(NULL, url.fullURL, kCFStringEncodingUTF8);
    requestURL = CFURLCreateWithString(kCFAllocatorDefault, CFurl, NULL);
    clientRequest = CFHTTPMessageCreateRequest(kCFAllocatorDefault, http_verb, requestURL, kCFHTTPVersion1_1);

    CFRelease(CFurl);
    CFRelease(requestURL);

    if (!clientRequest){
        LOG.error("%s: error can't create HTTP request", __FUNCTION__);
        setErrorF(ERR_NETWORK_INIT, "%s: error: can't create HTTP request", __FUNCTION__);
        requestHeaders.clear();
    
        return 1;
    }

#if defined(FUN_IPHONE)    
    SCNetworkReachabilityFlags flags;
    SCNetworkReachabilityRef   scnReachRef = SCNetworkReachabilityCreateWithName(kCFAllocatorDefault, url.host);

    gotflags = SCNetworkReachabilityGetFlags(scnReachRef, &flags);
    isReachable = flags & kSCNetworkReachabilityFlagsReachable;
    noConnectionRequired = !(flags & kSCNetworkReachabilityFlagsConnectionRequired);
    
    if ((flags & kSCNetworkReachabilityFlagsIsWWAN)) {
        noConnectionRequired = true;
    }

    CFRelease(scnReachRef);
#endif

    return 0;
}

int HttpConnection::request(InputStream& stream, OutputStream& response, bool log_request)
{
    int ret = STATUS_OK;
    AppleInputStream* data = dynamic_cast<AppleInputStream *>(&stream);
    CFReadStreamRef dataStream = NULL;
    size_t requestPayloadSize = 0;
    StringBuffer contentLenStr;
    
    if (data == NULL) {
        return StatusInternalError;
    }
    
    if (gotflags && isReachable && noConnectionRequired) {
        KeyValuePair headersKvPair;
        
        if (http_verb == NULL) {
            LOG.error("%s: can't create HTTP request: request method not defined");
            return StatusInternalError;
        }
        
        if (log_request) {
            LOG.debug("%s: requesting resource %s at %s:%d", __FUNCTION__, url.resource, url.host, url.port);
        }
        
        // check if we have a valid stream associated to the request: 
        // get data size from attached stream (considering the current stream offset)
        if ((requestPayloadSize = (data->getTotalSize() - data->getPosition())) > 0) {
            dataStream = data->getStream();
        } else {
            // get content length set in HTTP body
            requestPayloadSize = contentLen;
        }
        
        // set HTTP headers:
        setRequestHeader(HTTP_HEADER_USER_AGENT, userAgent);

        if (requestPayloadSize > 0) {
            LOG.debug("%s: setting HTTP content length header request (size: %d)", __FUNCTION__, requestPayloadSize);
            contentLenStr.sprintf("%ld", requestPayloadSize);
            setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, contentLenStr.c_str());
        }
        
        if (log_request) {
            LOG.debug("Request header:");
        }
        
        for (headersKvPair = requestHeaders.front(); !headersKvPair.null(); headersKvPair = requestHeaders.next()){
            const char* key = headersKvPair.getKey().c_str();
            const char* val = headersKvPair.getValue().c_str();
            
            if ((key == NULL) || (val == NULL)) {
                LOG.info("%s: null value in KeyValuePair", __FUNCTION__);
                continue;
            }
            
            CFStringRef hdrKey = CFStringCreateWithCString(NULL, key, kCFStringEncodingUTF8);
            CFStringRef hdrVal = CFStringCreateWithCString(NULL, val, kCFStringEncodingUTF8);

            CFHTTPMessageSetHeaderFieldValue(clientRequest, hdrKey, hdrVal);
            
            if (log_request) {
                LOG.debug("    %s: %s", key, val);
            }
            
            CFRelease(hdrKey);
            CFRelease(hdrVal);
        }
    
        if (auth) {
            if (!addHttpAuthentication(&clientRequest)) {
                LOG.error("Failed to add HTTP authentication information...");
                CFRelease(clientRequest);
                return StatusInternalError;
            }
        }

        if ((ret = sendRequest(dataStream, response, requestPayloadSize)) != StatusNoError) {
            return ret;
        }
        
        ret = getResponse();
    } else {
        setErrorF(ERR_CONNECT, "network error: can't connect to the server");
        LOG.error("%s", "network error: can't connect to the server");
        ret = StatusNetworkError;
    }

    return ret;
}

int HttpConnection::request(const char* data, OutputStream& response, bool log_request)
{         
    int requestStatus = 0;
    CFDataRef bodyData = NULL;
    AppleBufferInputStream inputStream("");
    
    if (data) {
        size_t dataLen = strlen(data);
        
        if (log_request) {
            LOG.debug("%s: request body: %s", __FUNCTION__, data);
        }
        
        bodyData = CFDataCreate(kCFAllocatorDefault, (const UInt8*)data, dataLen);
    
        if (!bodyData){
            LOG.error("%s: error in CFDataCreate", __FUNCTION__);
  
            return StatusInternalError;
        }
        
        contentLen = dataLen;
        CFHTTPMessageSetBody(clientRequest, bodyData);
    }
    
    requestStatus = request(inputStream, response, log_request);
  
    if (bodyData) {
        CFRelease(bodyData);
    }
    
    return requestStatus;
}

int HttpConnection::close()
{
    if (clientRequest) {
        CFRelease(clientRequest);
        clientRequest = NULL;
    }
    
    if (responseStream) {
        CFRelease(responseStream);
        responseStream = NULL;
    }

    // reset connection data 
    contentLen = 0;
    requestHeaders.clear();
    responseHeaders.clear();
    
    return 0;
}

int HttpConnection::sendRequest(CFReadStreamRef requestBodyStream, OutputStream& os, size_t requestPayloadSize)
{
    int status = StatusNoError;
    
    HttpConnectionHandler* handler = new HttpConnectionHandler(requestTimeout,responseChunkSize);
        
    if (requestBodyStream) {
        responseStream = CFReadStreamCreateForStreamedHTTPRequest(kCFAllocatorDefault, clientRequest, requestBodyStream);
    } else {
        responseStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, clientRequest);
    }
    
    if (responseStream == NULL) {
        LOG.error("%s: error creating HTTP data stream for upload", __FUNCTION__);
        delete handler;
        
        return StatusInternalError;
    }
    
    if ((status = handler->startConnectionHandler(responseStream, os, requestPayloadSize)) != 0) {
        if (status == ERR_CONNECT) {
            LOG.error("%s: connection failed", __FUNCTION__);
            status = StatusNetworkError;
        } else if (status == ERR_CONNECTION_TIMEOUT) {
            LOG.error("%s: connection timeout", __FUNCTION__);
            status = StatusTimeoutError;
        } else if (status = E_NET_READING) {
            LOG.error("%s: error reading http response", __FUNCTION__);
            status = StatusReadingError;
        } else {
            LOG.error("%s: fatal error connecting to remote host", __FUNCTION__);
            status = StatusInternalError;
        }
    } else {
        status = StatusNoError;
    }
    
    delete handler;
    // the response body is in the outputStream "os"
    return status;
}

int HttpConnection::getResponse()
{
    CFHTTPMessageRef serverReply = NULL;
    int statusCode = STATUS_OK;
   
    serverReply = (CFHTTPMessageRef) CFReadStreamCopyProperty(responseStream, kCFStreamPropertyHTTPResponseHeader);

    // Pull the status code from the headers
    if (serverReply == NULL) {
        LOG.error("%s: errore getting server reply", __FUNCTION__);

        return StatusReadingError;
    }

    // Read the HTTP response status code
    statusCode = CFHTTPMessageGetResponseStatusCode(serverReply);
    // Read all the HTTP response headers
    readResponseHeaders(serverReply);

    CFRelease(serverReply);
    CFReadStreamClose(responseStream);    

    LOG.debug("%s: HTTP message status code: %d", __FUNCTION__, statusCode);

    switch (statusCode) {
        case -1: {                    // connection error -> out code 2001
            setErrorF(ERR_CONNECT, "Network error in server receiving data");
            LOG.error("Network error in server receiving data");
            break;
        }
        case 400: { 
            setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
            LOG.error("HTTP server error: %d. Server failure.", statusCode);

            break;
        }
        case 500: {
            setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
            LOG.error("HTTP server error: %d. Server failure.", statusCode);
            break;
        }
        case 501: {
            setErrorF(HTTP_FUNCTIONALITY_NOT_SUPPORTED, 
                      "HTTP server error: %d. The server does not support the functionality needed to fulfill this request ().", 
                      statusCode);
            LOG.error("HTTP server error: %d. Server failure.", statusCode);
            break;
        }
        case 404: {
            setErrorF(ERR_HTTP_NOT_FOUND, "HTTP request error: resource not found (status %d)", statusCode);
            LOG.error("HTTP request error: resource not found (status %d)", statusCode);

            break;
        }
        case 408: {
            setErrorF(ERR_HTTP_REQUEST_TIMEOUT, "HTTP request error: server timed out waiting for request (status %d)", statusCode);
            LOG.debug("HTTP request error: server timed out waiting for request (status %d)", statusCode);

            break;
        }
        case 401: {   // Authentication failed
            setErrorF(401, "Authentication failed");
            LOG.error("Authentication failed");
            break;
        }
        default: {
            if (isErrorStatus(statusCode)) {
                statusCode = StatusNetworkError;
                setErrorF(statusCode, "HTTP request error: status received = %d", statusCode);
                LOG.error("HTTP request error: status received = %d", statusCode);
            } else {
                LOG.debug("HTTP request successful: code %d", statusCode);
            }
            
            break;
        }
    }

    return statusCode;
}

bool HttpConnection::addHttpAuthentication(CFHTTPMessageRef* request)
{
    if (!auth) {
        return false;
    }
    
    StringBuffer key(HTTP_HEADER_AUTHORIZATION);
    StringBuffer val;
    if (auth->getType() == HttpAuthentication::Basic) {
        val.sprintf("Basic %s", auth->getAuthenticationHeaders().c_str());
    }
    else {
        LOG.error("Digest authentication not yet supported - please use Basic auth");
        return false;
    }
    
    CFStringRef hdrKey = CFStringCreateWithCString(NULL, key, kCFStringEncodingUTF8);
    CFStringRef hdrVal = CFStringCreateWithCString(NULL, val, kCFStringEncodingUTF8);
    
    CFHTTPMessageSetHeaderFieldValue(clientRequest, hdrKey, hdrVal);
    LOG.debug("    %s: %s", key.c_str(), val.c_str());
    
    CFRelease(hdrKey);
    CFRelease(hdrVal);
    
    return true;
}

int HttpConnection::readResponseHeaders(CFHTTPMessageRef response)
{
    if (!response) {
        return -1;
    }

    responseHeaders.clear();
    
    CFDictionaryRef headers = CFHTTPMessageCopyAllHeaderFields(response);
    if (!headers) {
        LOG.error("Could not read the HTTP response headers");
        return -2;
    }
    
    int count = CFDictionaryGetCount(headers);
    if (count == 0) {
        // nothing to do
        return 0;
    }
    
    // Allocate the arrays of keys and values
    CFStringRef keys[count];
    CFStringRef values[count];
    for (int i=0; i<count; i++) {
        keys[i] = CFSTR("");
        values[i] = CFSTR("");
    }
    
    // Get the headers pairs and fill the stringMap
    CFDictionaryGetKeysAndValues(headers, (const void**)keys, (const void**)values);
    for (int i=0; i<count; i++) 
    {
        CFStringRef hdrKey = keys[i];
        CFStringRef hdrVal = values[i]; 
        if (!hdrKey || !hdrVal) continue;

        StringBuffer k = CFString2StringBuffer(hdrKey);
        StringBuffer v = CFString2StringBuffer(hdrVal);
        
        responseHeaders.put(k.c_str(), v.c_str());
        
        CFRelease(hdrKey);
        CFRelease(hdrVal);
    }

    return count;
}

END_FUNAMBOL_NAMESPACE
