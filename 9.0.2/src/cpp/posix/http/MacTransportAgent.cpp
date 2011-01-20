/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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


#include "base/fscapi.h"

#if FUN_TRANSPORT_AGENT == FUN_MAC_TRANSPORT_AGENT

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include "http/MacTransportAgent.h"
#include "http/HttpConnectionHandler.h"
#include "http/constants.h"

#include "base/util/utils.h"
#include "base/util/KeyValuePair.h"
#include "base/util/StringBuffer.h"
#include "event/FireEvent.h"
#include <pthread.h>

BEGIN_FUNAMBOL_NAMESPACE

MacTransportAgent::MacTransportAgent() : TransportAgent() {
    auth = NULL;
}

MacTransportAgent::~MacTransportAgent() {}

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
MacTransportAgent::MacTransportAgent(const URL& newURL, Proxy& newProxy, unsigned int maxResponseTimeout)
: TransportAgent(newURL, newProxy, maxResponseTimeout)
{
    auth = NULL;
}


// TODO
// consider moving both sendMessage and sendBuffer to basic class
char* MacTransportAgent::sendMessage(const char* msg) {
	return sendBuffer(msg, strlen(msg));
}

/*
 * msg is not expected to be a zero terminated string !!!
 */
char* MacTransportAgent::sendMessage(const char* msg, const unsigned int length) {
	return sendBuffer(msg, length);
}


char * MacTransportAgent::sendBuffer(const void * data, const unsigned int size) 
{
    LOG.debug("MacTransportAgent::sendBuffer begin");
    if(!data) {
        LOG.error("MacTransportAgent::sendBuffer error: NULL message.");
        setError(ERR_NETWORK_INIT, "MacTransportAgent::sendBuffer error: NULL message.");
        return NULL;
    }

    responseSize = 0;
    responseProperties.clear();
    bool gotflags = true;
    bool isReachable = true;
    bool noConnectionRequired = true; 
    HttpConnectionHandler* handler = NULL;
    int numretries = 0;
    StringBuffer result;   // server reply buffer
    int statusCode = -1;

    LOG.debug("Request: %s", (const char*)data);
    
#if defined(FUN_IPHONE)    
    SCNetworkReachabilityFlags        flags;
    SCNetworkReachabilityRef scnReachRef = SCNetworkReachabilityCreateWithName(kCFAllocatorDefault, url.host);

    gotflags = SCNetworkReachabilityGetFlags(scnReachRef, &flags);
    isReachable = flags & kSCNetworkReachabilityFlagsReachable;
    noConnectionRequired = !(flags & kSCNetworkReachabilityFlagsConnectionRequired);
    if ((flags & kSCNetworkReachabilityFlagsIsWWAN)) {
        noConnectionRequired = true;
    }
    CFRelease(scnReachRef);
#endif

    if ( gotflags && isReachable && noConnectionRequired ){
        char* ret=0;
        LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);

        // Construct URL
        CFStringRef CFurl =  CFStringCreateWithCString(NULL, url.fullURL, kCFStringEncodingUTF8);
        CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, CFurl, NULL);

        CFHTTPMessageRef httpRequest =
            CFHTTPMessageCreateRequest(kCFAllocatorDefault, CFSTR("POST"), myURL, kCFHTTPVersion1_1);
        CFRelease(CFurl);
        CFRelease(myURL);

        if(!httpRequest){
            LOG.error("MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
            setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");

            CFRelease(httpRequest);
            return ret;
        }

        CFDataRef bodyData = CFDataCreate(kCFAllocatorDefault, (const UInt8*)data, size);	
        if (!bodyData){
            LOG.error("MacTransportAgent::sendMessage error: CFDataCreate Error.");
            setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFDataCreate Error.");

            CFRelease(httpRequest);
            CFRelease(bodyData);
            return ret;
        }        
        CFHTTPMessageSetBody(httpRequest, bodyData);

        // For user agent, content length and accept encoding, override property
        // values, even if set by the caller.
        setProperty(TA_PropertyUserAgent, getUserAgent());
        setProperty(TA_PropertyContentLength, StringBuffer().append(size));
        
        if (auth) {
            if (!addHttpAuthentication(httpRequest)) {
                LOG.error("Failed to add HTTP authentication information...");
            }
        }
        
        // set HTTP header properties
        LOG.debug("Request header:");

        KeyValuePair p;
        for (p=requestProperties.front(); !p.null(); p=requestProperties.next()){
            const char* key = p.getKey().c_str();
            const char* val = p.getValue().c_str();
            
            if ((key == NULL) || (val == NULL)) {
                LOG.info("%s: null value in KeyValuePair", __FUNCTION__);
                continue;
            }
            
            CFStringRef hdrKey =
                CFStringCreateWithCString(NULL, key, kCFStringEncodingUTF8);
            CFStringRef hdrVal =
                CFStringCreateWithCString(NULL, val, kCFStringEncodingUTF8);
            CFHTTPMessageSetHeaderFieldValue(httpRequest, hdrKey, hdrVal);
            LOG.debug("    %s: %s", key, val);

            CFRelease(hdrKey);
            CFRelease(hdrVal);
        }
        responseProperties.clear();

        fireTransportEvent(size, SEND_DATA_BEGIN);

        CFReadStreamRef responseStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, httpRequest);
        
        // set PTHREAD_CANCEL_DEFERRED to enable cleanup handlers
        // over pthread_cancel() calls
        pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

        // Try MAX_RETRIES times to send http request, in case of network errors
        handler = new HttpConnectionHandler();
        for (numretries=0; numretries < MAX_RETRIES; numretries++) {
            int res = 0;
            
            if ((res = handler->startConnectionHandler(responseStream, size)) != 0) {
                if (res == ERR_CONNECT) {
                    LOG.error("connection failed");
                    
                    continue;
                } else {
                    LOG.error("fatal error connecting to remote host");
                    break;
                }
            }
            
            result = handler->getRequestReponse();
            
            CFHTTPMessageRef reply = (CFHTTPMessageRef) CFReadStreamCopyProperty( responseStream, kCFStreamPropertyHTTPResponseHeader);

            // Pull the status code from the headers
            if (reply) {
                statusCode = CFHTTPMessageGetResponseStatusCode(reply);

                // get properties from http header
                LOG.debug("Response header:");
                CFDictionaryRef dictHeaderFields = CFHTTPMessageCopyAllHeaderFields(reply);
                if (dictHeaderFields) {
                    NSEnumerator* e = [(NSDictionary*)dictHeaderFields keyEnumerator];
                    NSString* k;
                    while (k = [e nextObject]) {
                        NSString *v = [(NSDictionary*)dictHeaderFields objectForKey:k];
                        responseProperties.put([k UTF8String], [v UTF8String]);
                        LOG.debug("\t%s: %s", [k UTF8String], [v UTF8String]);
                    }
                    CFRelease(dictHeaderFields);
                }
                CFRelease(reply);
            }

            if (statusCode == -1) {                     //  -> retry
                LOG.debug("Offline mode detected: go-online and retry...");
                LOG.debug("Network error: let's wait 5 seconds before retry");
                resetError();
                sleep(5);
            }else if (statusCode == 400) {
                LOG.debug("Network error: let's wait 1 second before retry");
                sleep(1);
            }else if(statusCode == 401 || statusCode == 402 || statusCode == 403 ||
                     statusCode == 407 || statusCode >= 500 || (statusCode >= 200 && statusCode < 300)) { //don't retry in these cases 500, 401, 402, 403, 407
                goto exit;
            }

            // Other network error: retry.
            LOG.info("Network error writing data from client: retry %i time... for statuscode %i", numretries + 1, statusCode);
            continue;
        }
        
    exit:
        delete handler;
      
        fireTransportEvent(result.length(), RECEIVE_DATA_END);

        setResponseCode(statusCode);
        LOG.debug("Status Code: %d", statusCode);
        LOG.debug("Result: %s", result.c_str());


        switch (statusCode) {
            case 0: {
                        LOG.debug("Http request successful.");

                        // No errors, copy the response
                        // TODO: avoid byte copy
                        ret = stringdup(result.c_str());

                        break;
                    }
            case 200: {
                          LOG.debug("Http request successful.");

                          // No errors, copy the response
                          // TODO: avoid byte copy
                          ret = stringdup(result.c_str());

                          break;
                      }        
            case -1: {                    // connection error -> out code 2001
                         setErrorF(ERR_CONNECT, "Network error in server receiving data. ");
                         LOG.error("%s", getLastErrorMsg());

                         break;
                     }
            case 400: {                    // 400 bad request error. TODO: retry to send the message
                          setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
                          LOG.debug("%s", getLastErrorMsg());

                          break;
                      }
            case 500: {     // 500 -> out code 2052
                          setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
                          LOG.debug("%s", getLastErrorMsg());
                          break;
                      }
            case 404: {         // 404 -> out code 2060
                          setErrorF(ERR_HTTP_NOT_FOUND, "HTTP request error: resource not found (status %d)", statusCode);
                          LOG.debug("%s", getLastErrorMsg());
                          break;
                      }
            case 408: {   // 408 -> out code 2061
                          setErrorF(ERR_HTTP_REQUEST_TIMEOUT, "HTTP request error: server timed out waiting for request (status %d)", statusCode);
                          LOG.debug("%s", getLastErrorMsg());
                          break;
                      }
            case 401: {   // Authentication failed
                          setErrorF(401, "Authentication failed");
                          LOG.debug("%s", getLastErrorMsg());
                          ret = stringdup(result.c_str());
                          break;
                      }

            default: {
                         setErrorF(statusCode, "HTTP request error: status received = %d", statusCode);
                         LOG.error("%s", getLastErrorMsg());
                     }
        }

        CFRelease(httpRequest);
        CFRelease(bodyData);
        CFRelease(responseStream);
        LOG.debug("MacTransportAgent::sendBuffer end");    
        return ret;
    }else{
        setErrorF(ERR_CONNECT, "Network error: the attempt to connect to the server failed -> exit");
        LOG.debug("%s", getLastErrorMsg());
        LOG.debug("MacTransportAgent::sendBuffer end");
        return NULL;
    }
}

/*
 * Set the authentication object to enable
 * authenticating the http request.
 */
void MacTransportAgent::setAuthentication(HttpAuthentication *httpAuth) {
    auth = httpAuth;
}

/**
 * Add authentication headers to the given HTTP request.
 *
 * @param request The HTTP request that needs authentication headers.
 *
 * @return whether or not the authentication headers were successfully added to the request.
 */
bool MacTransportAgent::addHttpAuthentication(CFHTTPMessageRef request) 
{
    if (!auth) {
        return false;
    }
    
    StringBuffer key("Authorization");
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
    
    CFHTTPMessageSetHeaderFieldValue(request, hdrKey, hdrVal);
    LOG.debug("    %s: %s", key.c_str(), val.c_str());
    
    CFRelease(hdrKey);
    CFRelease(hdrVal);
    
    return true;
}

END_FUNAMBOL_NAMESPACE

#endif

