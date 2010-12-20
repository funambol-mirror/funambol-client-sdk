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

#ifndef __HTTP_CONNECTION_H__
#define __HTTP_CONNECTION_H__

#include "base/fscapi.h"

#if defined(FUN_IPHONE)
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCNetworkReachability.h>
#include <CFNetwork/CFNetwork.h>
#else
#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "http/URL.h"
#include "http/Proxy.h"
#include "http/TransportAgent.h"
#include "base/Log.h"
#include "http/HttpAuthentication.h"
#include "http/AbstractHttpConnection.h"
#include "inputStream/AppleInputStream.h"

#define ERR_HTTP_TIME_OUT               ERR_TRANSPORT_BASE+ 7
#define ERR_HTTP_NOT_FOUND              ERR_TRANSPORT_BASE+60
#define ERR_HTTP_REQUEST_TIMEOUT        ERR_TRANSPORT_BASE+61
#define ERR_HTTP_INFLATE                ERR_TRANSPORT_BASE+70
#define ERR_HTTP_DEFLATE                ERR_TRANSPORT_BASE+71
#define ERR_HTTP_INVALID_URL            ERR_TRANSPORT_BASE+72

BEGIN_FUNAMBOL_NAMESPACE

class HttpConnection : public AbstractHttpConnection
{
    private:
        CFHTTPMessageRef clientRequest;
        CFReadStreamRef responseStream;
        
        bool gotflags;
        bool isReachable;
        bool noConnectionRequired;
        CFStringRef http_verb;

    public:
    	HttpConnection(const char* user_agent);
	    ~HttpConnection();

        /**
         * Open the connection to the given url.
         * Caller must always call close() at the end, if an HTTPConnection was opened.
         * @param url    the connection url
         * @param method (optional) the request method, one of RequestMethod enum
         */
        int open(const URL& url, RequestMethod method = MethodPost);

        /**
         * This method closes this connection. It does not close the corresponding
         * input and output stream which need to be closed separately (if they were
         * previously opened).
         * This must be called if the connection was previously opened with open().
         */
        int close();

        /**
         * Sends the request.
         * The method used must be set via setRequestMethod (default = POST).
         * The headers/authentication must already be set before calling this method.
         *
         * @param data      the data to be sent
         * @param response  the response returned
         * @return          the HTTP Status Code returned or -1 if no status code can be discerned.
         */
        int request(InputStream& data, OutputStream& response);
        int request(const char* data, OutputStream& response);

    private:
        bool addHttpAuthentication(CFHTTPMessageRef* request);
        int  sendData(CFReadStreamRef requestBodyStream, OutputStream& os);
    
        /**
         * Reads the http response status and headers.
         * The error is set accordingly checking the status. responseHeaders stringMap is filled
         * with all the response headers found.
         * @return the HTTP response status code
         */
        int getResponse();
    
    /**
     * Reads the HTTP response headers, and fills them in the responseHeaders stringMap.
     * Existing data in the responseHeaders map will be cleared calling this method.
     * @param response  the CFHTTPMessageRef response to analyze
     * @return          the number of headers read (>0) or a value < 0 in case of errors
     */
    int readResponseHeaders(CFHTTPMessageRef response);
};

END_FUNAMBOL_NAMESPACE

#endif
