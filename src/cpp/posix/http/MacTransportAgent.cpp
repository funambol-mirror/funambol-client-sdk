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
#if defined(FUN_IPHONE)
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCNetworkReachability.h>
//#if TARGET_IPHONE_SIMULATOR
//#include <CoreServices/CoreServices.h>
//#else
#include <CFNetwork/CFNetwork.h>
#else
#include <CoreServices/CoreServices.h>
//#include <CFNetwork/CFNetwork.h>
#endif

#include "http/MacTransportAgent.h"
#include "http/constants.h"

#include "base/util/utils.h"
#include "base/util/StringBuffer.h"


USE_NAMESPACE

MacTransportAgent::MacTransportAgent() : TransportAgent() {}

MacTransportAgent::~MacTransportAgent() {}

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
MacTransportAgent::MacTransportAgent(URL& newURL, Proxy& newProxy, unsigned int maxResponseTimeout)
: TransportAgent(newURL, newProxy, maxResponseTimeout)
{}



/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
char* MacTransportAgent::sendMessage(const char* msg){
    LOG.debug("MacTransportAgent::sendMessage begin");
    if(!msg) {
        LOG.error("MacTransportAgent::sendMessage error: NULL message.");
        setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: NULL message.");
        return NULL;
    }
    
    bool gotflags = true;
    bool isReachable = true;
    bool noConnectionRequired = true; 
    
    StringBuffer result;
    CFIndex bytesRead = 1;
    int statusCode = -1;
    
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
        
        LOG.debug("Sending HTTP Request: %s", msg);
        
        // Construct some headers
        CFStringRef headerFieldName = CFSTR("Content-Type");
        CFStringRef headerFieldValue = CFSTR("application/vnd.syncml+xml");
        
        // Construct URL
        CFStringRef CFurl =  CFStringCreateWithCString(NULL, url.fullURL, kCFStringEncodingUTF8);
        CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, CFurl, NULL);
        
        CFStringRef requestMethod = CFSTR("POST");
        
        CFHTTPMessageRef httpRequest =
        CFHTTPMessageCreateRequest(kCFAllocatorDefault, requestMethod, myURL, kCFHTTPVersion1_1);
        CFStringRef useragent = CFStringCreateWithCString(NULL, getUserAgent(), kCFStringEncodingUTF8);
        CFHTTPMessageSetHeaderFieldValue(httpRequest, CFSTR("user-agent"),  useragent);
        if(!httpRequest){
            LOG.error("MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
            setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
            
            CFRelease(headerFieldName);
            CFRelease(headerFieldValue);
            CFRelease(CFurl);
            CFRelease(myURL);
            CFRelease(httpRequest);
            CFRelease(requestMethod);
            CFRelease(useragent);
            return ret;
        }
        
        CFDataRef bodyData = CFDataCreate(kCFAllocatorDefault, (const UInt8*)msg, strlen(msg));	
        if (!bodyData){
            LOG.error("MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
            setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
            CFRelease(headerFieldName);
            CFRelease(headerFieldValue);
            CFRelease(CFurl);
            CFRelease(myURL);
            CFRelease(httpRequest);
            CFRelease(bodyData);
            CFRelease(requestMethod);
            CFRelease(useragent);
            return ret;
        }        
        CFHTTPMessageSetBody(httpRequest, bodyData);
        CFHTTPMessageSetHeaderFieldValue(httpRequest, headerFieldName, headerFieldValue);
        
        CFReadStreamRef responseStream = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, httpRequest);
        
        if (!CFReadStreamOpen(responseStream)) {//Sends request
            LOG.error("Failed to send HTTP request...");
        }
        
        
        
#define READ_SIZE 1000
        
        UInt8   buffer[READ_SIZE];
        while ( (bytesRead = CFReadStreamRead(responseStream, buffer, READ_SIZE-1)) > 0)
        {
            //   Convert what was read to a C-string
            buffer[bytesRead] = 0;
            //   Append it to the reply string
            result.append((const char*)buffer);
        }
        
        CFHTTPMessageRef reply = (CFHTTPMessageRef) CFReadStreamCopyProperty( responseStream, kCFStreamPropertyHTTPResponseHeader);
        
        
        // Pull the status code from the headers
        if (reply) {
            statusCode = CFHTTPMessageGetResponseStatusCode(reply);
            CFRelease(reply);
        }
        
        
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
            }        case -1: {                    // no connection (TODO: implement retry)
                setErrorF(ERR_SERVER_ERROR, "Network error in server receiving data. ");
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
                
            default: {
                setErrorF(statusCode, "HTTP request error: status received = %d", statusCode);
                LOG.error("%s", getLastErrorMsg());
            }
        }
        
            CFRelease(headerFieldName);
            CFRelease(headerFieldValue);
            CFRelease(CFurl);
            CFRelease(myURL);
            CFRelease(httpRequest);
            CFRelease(bodyData);
            CFRelease(responseStream);
            CFRelease(requestMethod);
            CFRelease(useragent);
        LOG.debug("MacTransportAgent::sendMessage end");    
        return ret;
    }else{
        setErrorF(ERR_CONNECT, "Network error: the attempt to connect to the server failed -> exit");
        LOG.debug("%s", getLastErrorMsg());
        LOG.debug("MacTransportAgent::sendMessage end");
        return NULL;
    }
}

#endif

