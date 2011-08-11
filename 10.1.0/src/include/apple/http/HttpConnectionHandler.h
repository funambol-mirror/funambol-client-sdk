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

#ifndef __HTTP_CONNECTION_HANDLER_H__
#define __HTTP_CONNECTION_HANDLER_H__

#include "base/fscapi.h"

#if defined(FUN_IPHONE)
#include <SystemConfiguration/SystemConfiguration.h>
#include <SystemConfiguration/SCNetworkReachability.h>
#include <CFNetwork/CFNetwork.h>
#else
#include <CoreServices/CoreServices.h>
#endif

#include "http/URL.h"
#include "http/Proxy.h"
#include "http/TransportAgent.h"
#include "base/Log.h"
#include "http/HttpAuthentication.h"
#include "ioStream/OutputStream.h"

BEGIN_FUNAMBOL_NAMESPACE

class StreamDataHandle
{
    private:
        CFReadStreamRef responseStream; // CFHTTP request handle
        OutputStream* os;               // buffer to save request result 
        pthread_t mainThreadId;         // controller thread id
        pthread_t requestThreadId;      // request thread id

        StringBuffer  responseBuffer;   // buffer for non-streamed requests;
        
        CFRunLoopTimerRef timer;                    // timeout watchdog timer
        CFRunLoopTimerRef requestNotificationTimer; // progress notification timer

        bool stopStreamReading;
        bool timeoutWatchdogRunning;
        bool progressNotifierRunning;
        bool streamConnected;
        bool requestTimeout;

        size_t  readChunkSize;     // size of data chunk read from server 
        size_t  requestSize;       // payload length
        size_t  bytesUploaded;
        
        unsigned int timeout;         // timeout in seconds

    public:
        StreamDataHandle(CFReadStreamRef responseStream, OutputStream* os, size_t chunkSize, unsigned int timeout, size_t requestSize);
        // dtor: always remove timeout and progress handlers
        ~StreamDataHandle();            
     
        // timeout watchdog handlers
        void scheduleTimeoutWatchdog();
        void unscheduleTimeoutWatchdog();

        // upload progress notification handlers
        void startRequestProgressNotifier();
        void stopRequestProgressNotifier();
        
        pthread_t getThreadId() { return mainThreadId; }
        pthread_t getRequestThreadId() { return requestThreadId; } 

        StringBuffer* getResponseBuffer() { return &responseBuffer; }
        OutputStream* getOutputStream() { return os; }
        
        CFReadStreamRef getResponseStream() { return responseStream; }
        
        bool getStreamConnected() const { return streamConnected; }
        void setStreamConnected(bool flag) { streamConnected = flag; }
        
        bool getStopStreamReading() const { return stopStreamReading; }
        void setStopStreamReading(bool flag) { stopStreamReading = flag; }

        size_t getReadChunkSize() const { return readChunkSize; }
        unsigned int getRequestTimeout() const { return timeout; }
    
        size_t getBytesUploaded() const { return bytesUploaded; }
        void setBytesUploaded(size_t count) { bytesUploaded = count; }
        
        bool isTimeoutWatchdogRunning() const { return timeoutWatchdogRunning; }
        
    private:
        /**
         * Reads the HTTP response headers, and fills them in the responseHeaders stringMap.
         * Existing data in the responseHeaders map will be cleared calling this method.
         * @return    a newed responseHeaders stringMap (NULL in case of errors)
         */
        StringMap* parseHeaders();
};

typedef enum {
    E_SUCCESS = 0,
    E_THREAD_CREATE,
    E_THREAD_JOIN,
    E_ALREADY_RUNNING,
    E_NET_READING
} connection_handler_err_t;

class HttpConnectionHandler
{
    private:
        CFReadStreamRef stream;
        OutputStream* os;
        int requestSize;
        bool readerThreadRunning;
        StreamDataHandle* handle;
        StringBuffer response;
        unsigned int timeout;
        size_t readChunkSize;
        
    public:
        HttpConnectionHandler(unsigned int reqTimeout, size_t chunkSize);
        ~HttpConnectionHandler();
        
        int startConnectionHandler(CFReadStreamRef stream, size_t requestSize);
        int startConnectionHandler(CFReadStreamRef stream, OutputStream& os, size_t requestSize);
       
        void stopRequest();
         
        const char* getRequestReponse() const;

    private:
        int runRequest();
        int runStreamedRequest();
     
        static void* runRequestHandler(void* arg);
        static void* runStreamedRequestHandler(void* arg);
};

END_FUNAMBOL_NAMESPACE

#endif

