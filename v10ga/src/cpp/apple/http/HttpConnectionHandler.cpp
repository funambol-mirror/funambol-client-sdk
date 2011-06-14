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


#include "base/fscapi.h"

#if FUN_TRANSPORT_AGENT == FUN_MAC_TRANSPORT_AGENT

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include "base/util/StringUtils.h"
#include "http/HttpConnectionHandler.h"
#include "http/constants.h"
#include "base/util/utils.h"
#include "base/util/KeyValuePair.h"
#include "base/util/StringBuffer.h"
#include "event/FireEvent.h"
#include <pthread.h>
#include <inttypes.h>

BEGIN_FUNAMBOL_NAMESPACE

#define READ_MAX_BUFFER_SIZE    1024 * 4 // 4Kb
#define DEFAULT_REQUEST_TIMEOUT 90        // timeout in secs.

// main request thread cleanup handler
static void stopRequestThread(void *arg);
// reader thread cleanup handler
static void stopReaderThread(void *arg);

static void stopMainThread(StreamDataHandle* streamHandle);

// reader thread fuctions
static void* readerThread(void *arg);
static void* readerStreamThread(void *arg);

// callback called by progress notification runloop
static void progressNotifier(CFRunLoopTimerRef timer, void *info);
// connection timeout callback to stop reader thread
static void timeoutHandler(CFRunLoopTimerRef timer, void *info);

HttpConnectionHandler::HttpConnectionHandler(unsigned int reqTimeout, size_t chunkSize) : 
    stream(NULL), os(NULL), requestSize(0), readerThreadRunning(false), handle(NULL), 
    timeout(reqTimeout > 0 ? reqTimeout : DEFAULT_REQUEST_TIMEOUT),
    readChunkSize(chunkSize > 0 ? chunkSize : READ_MAX_BUFFER_SIZE) {}

HttpConnectionHandler::~HttpConnectionHandler() 
{
    if (handle) {        
        if (readerThreadRunning) {
            stopReaderThread((void *)handle);
        }

        delete handle;
    }
}

int HttpConnectionHandler::startConnectionHandler(CFReadStreamRef readStream, size_t reqSize)
{
    int status = 0;
    int ret = E_SUCCESS; 
    void* thread_ret_val = 0;
    pthread_t request_tid = 0;
    
    if (readerThreadRunning) {
        LOG.error("%s: reader thread already running", __FUNCTION__);
        
        return E_ALREADY_RUNNING;
    }

    readerThreadRunning = true;

    // set PTHREAD_CANCEL_DEFERRED to enable cleanup handlers
    // over pthread_cancel() calls
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    
    // clear out previous buffer
    response.reset();

    stream = readStream;
    os = NULL;
    requestSize = reqSize;
    
    // set stopReaderThread as cleanup handler for pthread_cancel
    pthread_cleanup_push(stopRequestThread, (void *) this);
    
    // create reader thread (this will open connection sending HTTP request
    // and will read the response).
    if ((status = pthread_create(&request_tid, NULL, runRequestHandler, 
            reinterpret_cast<void *>(this))) != 0) {
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__,
                            strerror(status));
        readerThreadRunning = false;
        
        ret = E_THREAD_CREATE;
        
        return ret;
    }

    // wait for completion of readerThread
    if ((status = pthread_join(request_tid, &thread_ret_val)) != 0) {
        readerThreadRunning = false;
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__, strerror(status));
        
        ret = E_THREAD_JOIN;
        
        return ret;
    } 
    
    pthread_cleanup_pop(0);
    
    readerThreadRunning = false;
    if (thread_ret_val == PTHREAD_CANCELED) {
        ret = ERR_CONNECTION_TIMEOUT;
    } else {
        ret = (intptr_t)thread_ret_val;
    }
   
    LOG.debug("%s: HTTP request status: %d", __FUNCTION__, ret);

    return ret;    
}

void* HttpConnectionHandler::runRequestHandler(void* arg)
{
    int status = 0;
    HttpConnectionHandler* connectionHandler = reinterpret_cast<HttpConnectionHandler*>(arg); 
 
    status = connectionHandler->runRequest();
    
    return (void *)status;
}


int HttpConnectionHandler::runRequest()
{
    int status = 0;
    int ret = E_SUCCESS; 
    void* thread_ret_val = 0;
    pthread_t tid;
    
    // set PTHREAD_CANCEL_DEFERRED to enable cleanup handlers
    // over pthread_cancel() calls
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    if (handle) {
        delete handle;
    }
   
    LOG.debug("%s: sending request with connection parameters: timeout: %u read chunk size: %lu", 
                __FUNCTION__, timeout, readChunkSize);
    
    handle = new StreamDataHandle(stream, NULL, readChunkSize, timeout, requestSize);
   
    // set stopReaderThread as cleanup handler for pthread_cancel
    pthread_cleanup_push(stopReaderThread, (void *) handle);
            
    tid = handle->getRequestThreadId();
    
    // create reader thread (this will open connection sending HTTP request
    // and will read the response).
    if ((status = pthread_create(&tid, NULL, readerThread, (void *)handle)) != 0) {
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__,
                            strerror(status));
        readerThreadRunning = false;
        delete handle;
        handle = NULL;
        
        ret = E_THREAD_CREATE;
        
        return ret;
    }

    // wait for completion of readerThread
    if ((status = pthread_join(tid, &thread_ret_val)) != 0) {
        readerThreadRunning = false;
        delete handle;
        handle = NULL;
        
        LOG.error("%s: error joning reader thread: %s", __FUNCTION__, strerror(status));
        
        ret = E_THREAD_JOIN;
        
        return ret;
    } 
    
    // remove thread cleanup handler
    pthread_cleanup_pop(0);
  
    ret = (intptr_t)thread_ret_val;
    LOG.debug("%s: HTTP request status: %d", __FUNCTION__, ret);
  
    response = *(handle->getResponseBuffer());
    
    delete handle;
    handle = NULL;

    return ret;    
}

int HttpConnectionHandler::startConnectionHandler(CFReadStreamRef readStream, OutputStream& outputStream, size_t reqSize)
{
    int status = 0;
    int ret = E_SUCCESS; 
    void* thread_ret_val = 0;
    pthread_t request_tid = 0;
    
    if (readerThreadRunning) {
        LOG.error("%s: reader thread already running", __FUNCTION__);
        
        return E_ALREADY_RUNNING;
    }

    readerThreadRunning = true;
    // clear out previous buffer
    response.reset();

    stream = readStream;
    os = &outputStream;
    requestSize = reqSize;
    
    // set PTHREAD_CANCEL_DEFERRED to enable cleanup handlers
    // over pthread_cancel() calls
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    // set stopReaderThread as cleanup handler for pthread_cancel
    pthread_cleanup_push(stopRequestThread, (void *) this);
    
    if ((status = pthread_create(&request_tid, NULL, runStreamedRequestHandler, 
            reinterpret_cast<void *>(this))) != 0) {
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__,
                            strerror(status));
        readerThreadRunning = false;
        
        ret = E_THREAD_CREATE;
        
        return ret;
    }

    // wait for completion of readerThread
    if ((status = pthread_join(request_tid, &thread_ret_val)) != 0) {
        readerThreadRunning = false;
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__, strerror(status));
        
        ret = E_THREAD_JOIN;
        
        return ret;
    } 

    pthread_cleanup_pop(0);
    
    readerThreadRunning = false;  
    
    if (thread_ret_val == PTHREAD_CANCELED) {
        ret = ERR_CONNECTION_TIMEOUT;
    } else {
        ret = (intptr_t)thread_ret_val;
    }
    
    LOG.debug("%s: HTTP request status: %d", __FUNCTION__, ret);
        
    return ret;    
}

void* HttpConnectionHandler::runStreamedRequestHandler(void* arg)
{
    int status = 0;
    HttpConnectionHandler* connectionHandler = reinterpret_cast<HttpConnectionHandler*>(arg); 
 
    status = connectionHandler->runStreamedRequest();
    
    return (void *)status;
}

int HttpConnectionHandler::runStreamedRequest()
{
    int status = 0;
    int ret = E_SUCCESS; 
    void* thread_ret_val = 0;
    pthread_t tid;
    
    if (handle) {
        delete handle;
    }    

    LOG.debug("%s: sending request with connection parameters: timeout: %u read chunk size: %lu", 
                __FUNCTION__, timeout, readChunkSize);
    handle = new StreamDataHandle(stream, os, readChunkSize, timeout, requestSize);
    
    // set stopReaderThread as cleanup handler for pthread_cancel
    pthread_cleanup_push(stopReaderThread, (void *) handle);
    
    tid = handle->getRequestThreadId();
    
    // create reader thread (this will open connection sending HTTP request
    // and will read the response).
    if ((status = pthread_create(&tid, NULL, readerStreamThread, (void *)handle)) != 0) {
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__, strerror(status));
        readerThreadRunning = false;
        delete handle;
        handle = NULL;
        
        ret = E_THREAD_CREATE;
        
        return ret;
    }

    // wait for completion of readerThread
    if ((status = pthread_join(tid, &thread_ret_val)) != 0) {
        readerThreadRunning = false;
        delete handle;
        handle = NULL;
        
        LOG.error("%s: error creating reader thread: %s", __FUNCTION__, strerror(status));
        
        ret = E_THREAD_JOIN;
        
        return ret;
    } 

    // remove thread cleanup handler
    pthread_cleanup_pop(0);
    
    ret = (intptr_t)thread_ret_val;
    
    delete handle;
    handle = NULL;
    
    return ret;    
}

void HttpConnectionHandler::stopRequest()
{
    if (handle) {
        stopReaderThread(handle);
    }
}

const char* HttpConnectionHandler::getRequestReponse() const
{
    return response.c_str();
}

//
// StreamDataHandle class
//
StreamDataHandle::StreamDataHandle(CFReadStreamRef respStream, OutputStream* oStream, 
                    size_t chunkSize, unsigned int timeout, size_t reqSize) : 
        responseStream(respStream), os(oStream), readChunkSize(chunkSize), 
        requestSize(reqSize), timeout(timeout), timer(NULL), requestNotificationTimer(NULL), 
        requestTimeout(false), timeoutWatchdogRunning(false),
        progressNotifierRunning(false), stopStreamReading(false),
        bytesUploaded(0)
         
{
    mainThreadId = pthread_self();
}
  
StreamDataHandle::~StreamDataHandle()
{
    stopRequestProgressNotifier();
    unscheduleTimeoutWatchdog();
}

void StreamDataHandle::scheduleTimeoutWatchdog()
{
    CFAllocatorRef allocator = kCFAllocatorDefault;
    CFRunLoopTimerContext context = { 0, (void *)this, NULL, NULL, NULL };
    CFTimeInterval interval = 0;
    CFOptionFlags flags = 0;
    CFIndex order = 0;

    // FIXME: add mutex
    if (timeoutWatchdogRunning == false) {
        CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent() + timeout;
        CFRunLoopTimerCallBack callback = (CFRunLoopTimerCallBack)timeoutHandler;

        if (timer) {
            LOG.info("%s: restarting timer...", __FUNCTION__);
            CFRunLoopTimerInvalidate(timer);
        }

        // create timer
        timer = CFRunLoopTimerCreate(allocator, fireDate, interval, flags, order, callback, &context);
        
        // star timer with notification callback
        CFRunLoopAddTimer(CFRunLoopGetMain(), timer, kCFRunLoopDefaultMode);
        timeoutWatchdogRunning = true;
    }
}

void StreamDataHandle::unscheduleTimeoutWatchdog()
{
    // FIXME: add mutex
    if (timeoutWatchdogRunning) {
        timeoutWatchdogRunning = false;
        
        if (timer) {
            CFRunLoopTimerInvalidate(timer);
            timer = NULL;
        }
    }
}

void StreamDataHandle::startRequestProgressNotifier()
{
    CFAllocatorRef allocator = kCFAllocatorDefault;
    CFRunLoopTimerContext context = { 0, (void *)this, NULL, NULL, NULL };
    CFTimeInterval interval = 1;
    CFOptionFlags flags = 0;
    CFIndex order = 0;

    // add mutex
    if (progressNotifierRunning == false) {
        CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent() + 1;
        CFRunLoopTimerCallBack callback = (CFRunLoopTimerCallBack)progressNotifier;

        if (requestNotificationTimer) {
            LOG.info("%s: restarting upload progress timer...", __FUNCTION__);
            CFRunLoopTimerInvalidate(requestNotificationTimer);
        }

        // create timer
        requestNotificationTimer = CFRunLoopTimerCreate(allocator, fireDate, interval, flags, order, callback, &context);
        
        // star timer with notification callback
        CFRunLoopAddTimer(CFRunLoopGetMain(), requestNotificationTimer, kCFRunLoopDefaultMode);
        progressNotifierRunning = true;
    }
}

void StreamDataHandle::stopRequestProgressNotifier()
{
    if (progressNotifierRunning) {
        if (requestNotificationTimer) {
            CFRunLoopTimerInvalidate(requestNotificationTimer);
            requestNotificationTimer = NULL;
        }
        progressNotifierRunning = false;
    }
}

StringMap* StreamDataHandle::parseHeaders()
{
    CFHTTPMessageRef serverReply = NULL;
    StringMap* responseHeaders = NULL;      // associative array for server response headers
    
    serverReply = (CFHTTPMessageRef) CFReadStreamCopyProperty(responseStream, kCFStreamPropertyHTTPResponseHeader);

    // Pull the status code from the headers
    if (serverReply == NULL) {
        LOG.error("%s: error getting server reply from response stream", __FUNCTION__);

        return responseHeaders;
    }
    
    CFDictionaryRef headers = CFHTTPMessageCopyAllHeaderFields(serverReply);
    
    if (!headers) {
        LOG.info("%s: no HTTP  headers in server response");
        return responseHeaders;
    }
 
    responseHeaders = new StringMap();
    responseHeaders->clear();
 
    int count = CFDictionaryGetCount(headers);
    if (count == 0) {
        // nothing to do
        return responseHeaders;
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
        
        responseHeaders->put(k.c_str(), v.c_str());
        
        CFRelease(hdrKey);
        CFRelease(hdrVal);
    }

    return responseHeaders;
}

/**
 * HTTP connection handler within a thread: open a connection 
 * sending HTTP request and read server response
 */ 
static void* readerThread(void *arg)
{
    StreamDataHandle* streamHandle = (StreamDataHandle *)arg;
    int read_size = streamHandle->getReadChunkSize();
    int ret = E_SUCCESS; 
    UInt8   buffer[read_size];
    CFIndex bytesRead = 0;
    StringBuffer contentLengthHdr;
    CFReadStreamRef responseStream = NULL;
    
    streamHandle->setStreamConnected(false); 
    responseStream = streamHandle->getResponseStream();
    
    // open connection and send request
    if (!CFReadStreamOpen(responseStream)) {
        LOG.error("%s: failed to send HTTP request...", __FUNCTION__);
        ret = ERR_CONNECT;
        
        return (void *)ret;
    }
   
    streamHandle->setStreamConnected(true); 
    LOG.debug("%s: HTTP request sent", __FUNCTION__);
    
    streamHandle->setStopStreamReading(false);
    memset(buffer, 0, read_size);
    
    LOG.debug("%s: reading data from server...", __FUNCTION__);
    streamHandle->scheduleTimeoutWatchdog();

    /*
    StringMap* responseHdrs = NULL;
   
    if ((responseHdrs = streamHandle->parseHeaders())) {
        contentLengthHdr = responseHdrs->get("Content-Length");
        if (contentLengthHdr.empty() == false) {
            //fireTransportEvent(atol(contentLengthHdr.c_str()), RECEIVE_DATA_BEGIN);
        }
        
        delete responseHdrs;
    }
    */
    streamHandle->scheduleTimeoutWatchdog();
    
    StringBuffer* responseBuffer = streamHandle->getResponseBuffer();
    
    // server reponse read loop
    while ((bytesRead = CFReadStreamRead(responseStream, buffer, read_size -1)) > 0) {
        streamHandle->unscheduleTimeoutWatchdog();
        pthread_testcancel();
        
        if (streamHandle->getStopStreamReading()) {
            break;
        }
        
        buffer[bytesRead] = 0;
        
        // Append it to the reply string
        responseBuffer->append((const char*)buffer);
        memset(buffer, 0, read_size);
        streamHandle->scheduleTimeoutWatchdog();
    }

    CFReadStreamClose(responseStream);
    streamHandle->unscheduleTimeoutWatchdog();
    
    LOG.debug("%s: data read from server completed", __FUNCTION__);
 
    return (void *)ret;
}

static void* readerStreamThread(void *arg)
{
    StreamDataHandle* streamHandle = (StreamDataHandle *)arg;
    int ret = E_SUCCESS;
    int read_size = streamHandle->getReadChunkSize(); 
    UInt8   buffer[read_size];
    CFIndex bytesRead = 0;
    size_t totalBytesRead = 0;
    bool writeToStreamCompleted = false;
    CFReadStreamRef responseStream = NULL;
    OutputStream* os = NULL;
    
    streamHandle->setStreamConnected(false);    
    streamHandle->startRequestProgressNotifier();
    
    responseStream = streamHandle->getResponseStream();
    os = streamHandle->getOutputStream(); 
    
    // open connection and send request (this will start upload of the associated stream)
    if (!CFReadStreamOpen(responseStream)) {
        LOG.error("%s: failed to send HTTP request...", __FUNCTION__);
        ret = ERR_CONNECT;
        streamHandle->stopRequestProgressNotifier();
        streamHandle->unscheduleTimeoutWatchdog();
    
        return (void *)ret;
    }
    
    memset(buffer, 0, read_size);
           
    // CFReadStreamRead will block until CFReadStreamRead call has completed sending data 
    while ((bytesRead = CFReadStreamRead(responseStream, buffer, read_size - 1)) > 0) {
        streamHandle->unscheduleTimeoutWatchdog();
       
        if (writeToStreamCompleted == false) {
            streamHandle->stopRequestProgressNotifier();
            /*
            StringMap* responseHdrs = NULL;
            if ((responseHdrs = streamHandle->parseHeaders())) {
                StringBuffer contentLengthVal = responseHdrs->get("Content-Length");
                if (contentLengthVal.empty() == false) {
                    fireTransportEvent(atol(contentLengthVal.c_str()), RECEIVE_DATA_BEGIN);
                }
                
                delete responseHdrs;
            }
            */
            LOG.debug("%s: HTTP request sent", __FUNCTION__);
            streamHandle->setStreamConnected(true);
            streamHandle->setStopStreamReading(false);
            
            LOG.debug("reading data from server...");
            writeToStreamCompleted = true;
        }
        
        pthread_testcancel();
        
        if (streamHandle->getStopStreamReading()) {
            break;
        }
    
        buffer[bytesRead] = 0;
        os->write((const void*)buffer, (int)bytesRead);
        
        fireTransportEvent((int)bytesRead, DATA_RECEIVED);
        totalBytesRead += (int)bytesRead;
        memset(buffer, 0, read_size);
        streamHandle->scheduleTimeoutWatchdog();
    } 
    
    if (bytesRead < 0) {
        LOG.error("%s: error reading from HTTP stream", __FUNCTION__);
        ret = E_NET_READING;
    }
    
    CFReadStreamClose(responseStream);
    streamHandle->unscheduleTimeoutWatchdog();

    fireTransportEvent(totalBytesRead, RECEIVE_DATA_END);
    
    return (void *)ret;
}

void stopRequestThread(void *arg)
{
    HttpConnectionHandler* connectionHandle = (HttpConnectionHandler *)arg;
    
    connectionHandle->stopRequest();
}

/**
 * thread cleanup handler (called on cancellation request 
 * sent to main thread): tries to stop the reader loop
 * and finally sends to thread SIGUSR1 signal (this is 
 * needed to avoid to be blocked on CFReadStreamOpen)
 */ 
void stopReaderThread(void *arg)
{
    StreamDataHandle* streamHandle = (StreamDataHandle *)arg;
    int thread_status = 0;
    pthread_t tid;
    CFReadStreamRef responseStream = NULL;
    
    if (streamHandle == NULL) {
        LOG.error("%s: invalid data handle", __FUNCTION__);
        
        return;
    }
    
    responseStream = streamHandle->getResponseStream();
    
    LOG.debug("%s: stopping run loop timer", __FUNCTION__);
    streamHandle->unscheduleTimeoutWatchdog();
    
    LOG.debug("%s: stopping progress notification timer", __FUNCTION__);
    streamHandle->stopRequestProgressNotifier();
    
    if (streamHandle->getStreamConnected() == true) {
        streamHandle->setStopStreamReading(true);
    }
    
    tid = streamHandle->getThreadId();
    
    if (tid) {
        if ((thread_status = pthread_kill(tid, 0)) == 0) {
            LOG.debug("cancelling HTTP request thread");
            pthread_kill(tid, SIGUSR1);
        } else if (thread_status == ESRCH) {
            LOG.debug("HTTP request thread already ended");
        } 
    }
    
    CFReadStreamClose(responseStream); 
}

void stopMainThread(StreamDataHandle* streamHandle)
{
    pthread_t tid;
    
    if (streamHandle) {
        tid = streamHandle->getThreadId();
        
        if (tid) {
            pthread_cancel(tid);
        }
    }
}

void timeoutHandler(CFRunLoopTimerRef timer, void *info)
{
    StreamDataHandle* streamHandle = (StreamDataHandle *)info;
    
    LOG.debug("removing runloop timer...");
    streamHandle->unscheduleTimeoutWatchdog();
    LOG.debug("removing progress notifier...");
    streamHandle->stopRequestProgressNotifier();
    LOG.debug("stopping main thread");
    stopMainThread(streamHandle);
}

void progressNotifier(CFRunLoopTimerRef timer, void *info)
{
    StreamDataHandle* streamHandle = (StreamDataHandle *)info;
    size_t bytesUploaded = streamHandle->getBytesUploaded();
    
    if (streamHandle) {
        int totalBytesUploaded = 0;
        CFReadStreamRef responseStream = streamHandle->getResponseStream();
        
        if (streamHandle->isTimeoutWatchdogRunning() == false) {
            streamHandle->scheduleTimeoutWatchdog();
        } 
        
        CFNumberRef bytesUploadedRef = (CFNumberRef)CFReadStreamCopyProperty(responseStream, kCFStreamPropertyHTTPRequestBytesWrittenCount);
        
        if (CFNumberGetValue(bytesUploadedRef,  kCFNumberIntType, (void *)&totalBytesUploaded)) {            
            if (bytesUploaded != totalBytesUploaded) {
                streamHandle->unscheduleTimeoutWatchdog();
                // fire transport notification: get bytes uploaded from last notification 
                // subtracting streamHandle->bytesUploaded from bytesUploaded counter (total 
                // bytes count from kCFStreamPropertyHTTPRequestBytesWrittenCount)
                fireTransportEvent((unsigned long)(totalBytesUploaded - bytesUploaded), DATA_SENT);
                streamHandle->setBytesUploaded(totalBytesUploaded);
            }
        }
    }
}

END_FUNAMBOL_NAMESPACE

#endif
