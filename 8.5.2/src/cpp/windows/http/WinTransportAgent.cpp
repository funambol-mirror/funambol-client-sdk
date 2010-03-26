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

/*
 How to test SSL connections
 ----------------------------

 On the server:
 1) create the keystore:
    %JAVA_HOME%\bin\keytool -genkey -alias tomcat -keyalg RSA
 2) In $CATALINA_HOME/conf/server.xml uncomment the lines:
    <Connector className="org.apache.catalina.connector.http.HttpConnector"
               port="8443" minProcessors="5" maxProcessors="75"
               enableLookups="true"
               acceptCount="10" debug="0" scheme="https" secure="true">
      <Factory className="org.apache.catalina.net.SSLServerSocketFactory" clientAuth="false" protocol="TLS"/>
    </Connector>
 2) Export the certificate from the key store:
    %JAVA_HOME%\bin\keytool -export -alias tomcat -file myroot.cer

 On the client:
  [for _WIN32_WCE]
   1)  Copy myroot.cer in a device/emulator directory
   2) Click on it to import the certificate as a trusted CA
  [for WIN32]
   1) Connect (via https) to the server using a web-browser (type "https://<server_address>:8443)
   2) Accept and install the certificate sent from the server
*/

#include "base/Log.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/WinTransportAgent.h"
#include "event/FireEvent.h"
#include "base/util/StringBuffer.h"

#ifdef _WIN32_WCE
#include "http/GPRSConnection.h"
#endif


#define ENTERING(func) // LOG.debug("Entering %ls", func);
#define EXITING(func)  // LOG.debug("Exiting %ls", func);

#ifdef USE_ZLIB
#include "zlib.h"
#include "base/globalsdef.h"

USE_NAMESPACE
#endif

/**
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url    the url where messages will be sent with sendMessage()
 * @param proxy  proxy information or NULL if no proxy should be used
 */
WinTransportAgent::WinTransportAgent(URL& newURL, Proxy& newProxy,
                                     unsigned int maxResponseTimeout,
                                     unsigned int maxmsgsize)
                                     // Use base class constructor to initialize common attributes
                                     : TransportAgent(newURL,
                                     newProxy,
                                     maxResponseTimeout,
                                     maxmsgsize) {

    if (maxResponseTimeout == 0) {
        setTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setTimeout(maxResponseTimeout);
    }

    isToDeflate    = false;
    isFirstMessage = true;
    isToInflate    = false;
    setHttpVerb(HTTP_POST);

#ifdef _WIN32_WCE
    // used by default. check connection before...
    if (!EstablishConnection()) {

#  ifdef WIN32_PLATFORM_PSPC
        //lastErrorCode = ERR_INTERNET_CONNECTION_MISSING;
        //sprintf(lastErrorMsg, "%s: %d",
        //    "Internet Connection Missing",
        //    ERR_INTERNET_CONNECTION_MISSING);
        setErrorF(ERR_INTERNET_CONNECTION_MISSING, "%s: %d",
              "Internet Connection Missing",
              ERR_INTERNET_CONNECTION_MISSING);
#  else
        LOG.error("Warning: internet connection missing.");
#  endif  // #ifdef WIN32_PLATFORM_PSPC
    }
#endif  // #ifdef _WIN32_WCE

}

WinTransportAgent::~WinTransportAgent(){}



/*
 * Sends the given SyncML message to the server specified
 * by the install property 'url'. Returns the server's response.
 * The response string has to be freed with delete [].
 * In case of an error, NULL is returned and lastErrorCode/Msg
 * is set.
 */
char* WinTransportAgent::sendMessage(const char* msg) {

    LOG.debug("%s", msg);
    //LOG.debugHideDataContent(msg);
    ENTERING(L"TransportAgent::sendMessage");

#ifdef USE_ZLIB
    // This is the locally allocated buffer for the compressed message.
    // Must be deleted after send.
    Bytef* compr   = NULL;
    WCHAR* wbuffer = NULL;
    WCHAR* buffer  = NULL;
#endif

    char* bufferA = new char[readBufferSize+1];
    int status = -1;
    unsigned int contentLength = 0;
    WCHAR* wurlHost      = NULL;
    WCHAR* wurlResource  = NULL;
    char*  p             = NULL;
    char*  response      = NULL;

    HINTERNET inet       = NULL,
              connection = NULL,
              request    = NULL;


    // Check sending msg and host.
    if (!msg) {
        //lastErrorCode = ERR_NETWORK_INIT;
        //sprintf(lastErrorMsg, "TransportAgent::sendMessage error: NULL message.");
        setError(ERR_NETWORK_INIT, "TransportAgent::sendMessage error: NULL message.");
        goto exit;
    }
    if (!(url.host) || strlen(url.host) == 0) {
        //lastErrorCode = ERR_HOST_NOT_FOUND;
        //sprintf(lastErrorMsg, "TransportAgent::sendMessage error: %s.", ERRMSG_HOST_NOT_FOUND);
        setErrorF(ERR_HOST_NOT_FOUND, "TransportAgent::sendMessage error: %s.", ERRMSG_HOST_NOT_FOUND);
        goto exit;
    }

    DWORD size  = 0,
          read  = 0,
          flags = INTERNET_FLAG_RELOAD |
                  INTERNET_FLAG_NO_CACHE_WRITE |
                  INTERNET_FLAG_KEEP_CONNECTION |           // This is necessary if authentication is required.
                  INTERNET_FLAG_NO_COOKIES;                 // This is used to avoid possible server errors on successive sessions.

	LPCWSTR acceptTypes[2] = {TEXT("*/*"), NULL};


    // Set flags for secure connection (https).
    if (url.isSecure()) {
        flags = flags
              | INTERNET_FLAG_SECURE
              | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
              | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
              ;
    }
    
    //
    // Open Internet connection.
    //
    WCHAR* ua = toWideChar(userAgent);
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
	if (ua) {delete [] ua; ua = NULL; }

    if (!inet) {
        //lastErrorCode = ERR_NETWORK_INIT;
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        //sprintf (lastErrorMsg, "InternetOpen Error: %d - %s", code, tmp);
        setErrorF(ERR_NETWORK_INIT, "InternetOpen Error: %d - %s", code, tmp);
		delete [] tmp;
        goto exit;
    }
    LOG.debug("Connecting to %s:%d", url.host, url.port);


    //
    // Open an HTTP session for a specified site by using lpszServer.
    //
    wurlHost = toWideChar(url.host);
    if (!(connection = InternetConnect (inet,
                                        wurlHost,
                                        url.port,
                                        NULL, // username
                                        NULL, // password
                                        INTERNET_SERVICE_HTTP,
                                        0,
                                        0))) {
        //lastErrorCode = ERR_CONNECT;
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        //sprintf (lastErrorMsg, "InternetConnect Error: %d - %s", code, tmp);
        setErrorF(ERR_CONNECT, "InternetConnect Error: %d - %s", code, tmp);
        delete [] tmp;
        goto exit;
    }
    LOG.debug("Requesting resource %s", url.resource);

    //
    // Open an HTTP request handle. By default it uses the POST method (see constructor).
    // it could be set the GET if someone would use it. The api doesn't use
    // GET at all.
	//    
    const WCHAR* http_verb = METHOD_POST;
    if (getHttpVerb() == HTTP_GET) {
        http_verb = METHOD_GET;
    }
    wurlResource = toWideChar(url.resource);
    if (!(request = HttpOpenRequest (connection,
                                     http_verb,
                                     wurlResource,
                                     HTTP_VERSION,
                                     NULL,
                                     acceptTypes,
                                     flags, 0))) {
        //lastErrorCode = ERR_CONNECT;
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        //sprintf (lastErrorMsg, "HttpOpenRequest Error: %d - %s", code, tmp);
        setErrorF(ERR_CONNECT, "HttpOpenRequest Error: %d - %s", code, tmp);
        delete [] tmp;
        goto exit;
    }


    //
    // Prepares headers
    //
    WCHAR headers[512];
    contentLength = strlen(msg);

    // Msg to send is the original msg by default.
    // If compression is enabled, it will be switched to
    // compr. We don't want to touch this pointer, so
    // it's const (msg is also const).
    const void* msgToSend = (const void*)msg;

#ifdef USE_ZLIB
    if(compression){
        // This is the locally allocated buffer for the compressed message.
        // Must be deleted after send.

	    //
	    // Say the client can accept the zipped content but the first message is clear
	    //
	    if (isFirstMessage || !isToDeflate) {
	        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d\r\nAccept-Encoding: deflate"),
	                          SYNCML_CONTENT_TYPE, contentLength);
	        isFirstMessage = false;
	    }
	    else if (isToDeflate) {
	        //
	        // DEFLATE (compress data)
	        //
	        uLong comprLen = contentLength;
	        compr = new Bytef[contentLength];
	
	        // Compresses the source buffer into the destination buffer.
	        int err = compress(compr, &comprLen, (Bytef*)msg, contentLength);
	        if (err != Z_OK) {
	            //lastErrorCode = ERR_HTTP_DEFLATE;
	            //sprintf(lastErrorMsg, "ZLIB: error occurred compressing data.");
                    setError(ERR_HTTP_DEFLATE, "ZLIB: error occurred compressing data.");
	            delete [] compr;
	            compr = NULL;
	            goto exit;
	        }
	
	        // Msg to send is the compressed data.
	        msgToSend = (const void*)compr;
	        int uncomprLenght = contentLength;
	        contentLength = comprLen;
	
	        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nUncompressed-Content-Length: %d\r\nContent-Encoding: deflate"),
	                          SYNCML_CONTENT_TYPE, contentLength, uncomprLenght);
	    }
    }
    else {
        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);
    } //end if compression
#else
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);
#endif


    // Timeout to receive a rensponse from server (default = 5 min).
    DWORD timeoutMsec = timeout*1000;
    InternetSetOption(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMsec, sizeof(DWORD));

    // if the client allows to sync over https even if the server
    // has an invalid certificate, the flag is false. By default it is true
    if (getSSLVerifyServer() == false) {
        DWORD dwFlags, dwBuffLen = sizeof(dwFlags);
        InternetQueryOption (request, INTERNET_OPTION_SECURITY_FLAGS,
            (LPVOID)&dwFlags, &dwBuffLen);    
        dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        InternetSetOption (request, INTERNET_OPTION_SECURITY_FLAGS,
                        &dwFlags, sizeof (dwFlags));        
    }

    //
    // Try MAX_RETRIES times to send http request, in case of network errors
    //
    DWORD errorCode = 0;
    int numretries;
    for (numretries=0; numretries < MAX_RETRIES; numretries++) {

        //
        // Send a request to the HTTP server.
        //
        fireTransportEvent(contentLength, SEND_DATA_BEGIN);
        if (!HttpSendRequest(request, headers, wcslen(headers), (LPVOID)msgToSend, contentLength)) {

            errorCode = GetLastError();
            char* tmp = createHttpErrorMessage(errorCode);
            setErrorF(GetLastError(), "HttpSendRequest error %d: %s", errorCode, tmp);
            LOG.info("%s", getLastErrorMsg());
            delete [] tmp; tmp = NULL;
            //
            // The certificate is not trusted. Send the right error code to the
            // client
            //
            if (errorCode == ERROR_INTERNET_INVALID_CA) {
                
                setError(ERR_HTTPS_INVALID_CA, "The certificate is invalid");
                LOG.error("%s", getLastErrorMsg());
                                                
                // try to understand a bit more on the certificate
                INTERNET_CERTIFICATE_INFO   certificateInfo;
                DWORD                       certInfoLength = sizeof(INTERNET_CERTIFICATE_INFO);                
                if (TRUE == InternetQueryOption(request, 
                                                INTERNET_OPTION_SECURITY_CERTIFICATE_STRUCT, 
                                                &certificateInfo, &certInfoLength)) {
                    
                    char* subj   = (char*)certificateInfo.lpszSubjectInfo;
                    char* issuer = (char*)certificateInfo.lpszIssuerInfo;    
                    LOG.debug("Cert Subject %s", subj);
                    LOG.debug("Cert Issuer %s",  issuer);

                } else {                        
                    LOG.debug("Cannot retrieve info about the certificate");
                }     
                goto exit;                
            }
            
            if (errorCode == ERROR_INTERNET_OFFLINE_MODE) {                     // 00002 -> retry
                LOG.debug("Offline mode detected: go-online and retry...");
                WCHAR* wurl = toWideChar(url.fullURL);
                InternetGoOnline(wurl, NULL, NULL);
                delete [] wurl;
                resetError();
                continue;
            }
            else if (errorCode == ERROR_INTERNET_TIMEOUT ||                     // 12002 -> out code 2007
                     errorCode == ERROR_INTERNET_INCORRECT_HANDLE_STATE) {      // 12019 -> out code 2007
                //lastErrorCode = ERR_HTTP_TIME_OUT;
                //sprintf(lastErrorMsg, "Network error: the request has timed out -> exit.");
                setError(ERR_HTTP_TIME_OUT, "Network error: the request has timed out -> exit.");
                LOG.debug("%s", getLastErrorMsg());
                goto exit;
            }
            else if (errorCode == ERROR_INTERNET_CANNOT_CONNECT) {              // 12029 -> out code 2001
                //lastErrorCode = ERR_CONNECT;
                //sprintf(lastErrorMsg, "Network error: the attempt to connect to the server failed -> exit");
                setError(ERR_CONNECT, "Network error: the attempt to connect to the server failed -> exit"); 
                LOG.debug("%s", getLastErrorMsg());
                goto exit;
            }
            // Other network error: retry.
            LOG.info("Network error writing data from client: retry %i time...", numretries + 1);
            resetError();
            continue;
        }

        LOG.debug("%s", MESSAGE_SENT);
        fireTransportEvent(contentLength, SEND_DATA_END);


        //
        // Check the status code.
        //
        size = sizeof(status);
        HttpQueryInfo (request,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&status,
                       (LPDWORD)&size,
                       NULL);
        
        // OK: status 200
        if (status == HTTP_STATUS_OK) {
        	LOG.debug("Data sent successfully to server. Server responds OK");
            break;
        }

        #if defined(WIN32) && !defined(_WIN32_WCE)
        //
        // Proxy Authentication Required (407) / Server Authentication Required (401).
        // Need to set username/password.
        //
        else if(status == HTTP_STATUS_PROXY_AUTH_REQ ||
                status == HTTP_STATUS_DENIED) {
            LOG.debug("HTTP Authentication required.");
            DWORD dwError;

            // Automatic authentication (user/pass stored in win reg key).
            if (strcmp(proxy.user, "") && strcmp(proxy.password, "")) {
                WCHAR* wUser = toWideChar(proxy.user);
                WCHAR* wPwd  = toWideChar(proxy.password);

                InternetSetOption(request, INTERNET_OPTION_PROXY_USERNAME, wUser, wcslen(wUser)+1);
                InternetSetOption(request, INTERNET_OPTION_PROXY_PASSWORD, wPwd,  wcslen(wPwd)+1);

                delete [] wUser;
                delete [] wPwd;
                dwError = ERROR_INTERNET_FORCE_RETRY;
            }

            // Prompt dialog box.
            else {
                dwError = InternetErrorDlg(GetDesktopWindow(), request, NULL,
                                           FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
                                           FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
                                           FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
                                           NULL);
            }

            if (dwError == ERROR_INTERNET_FORCE_RETRY) {
                continue;
            }
            else {
                LOG.error("HTTP Authentication failed.");
                break;
            }
        }
        #endif  // #if defined(WIN32) && !defined(_WIN32_WCE)
        
        else if (status == HTTP_ERROR) {                    // 400 bad request error. retry to send the message
            LOG.info("Network error in server receiving data. "
                     "Server responds 400: retry %i time...", numretries + 1);
            continue;
        }
        else if (status == HTTP_STATUS_SERVER_ERROR ) {     // 500 -> out code 2052
            setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }

        #ifdef _WIN32_WCE
        // To handle the http error code for the tcp/ip notification with wrong credential
        else if (status == ERR_CREDENTIAL) {                // 401 -> out code 401
            setErrorF(ERR_CREDENTIAL, "HTTP server error: %d. Wrong credential.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        // To handle the http error code for the tcp/ip notification when payment required
        else if (status == PAYMENT_REQUIRED) {              // 402 -> out code 402
            setErrorF(PAYMENT_REQUIRED, "HTTP server error: %d. Client not authenticated.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        // To handle the http error code for the tcp/ip notification when client not activated (forbidden)
        else if (status == FORBIDDEN) {                     // 403 -> out code 403
            setErrorF(FORBIDDEN, "HTTP server error: %d. Connection forbidden, client not activated.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        // to handle the http error code for the tcp/ip notification and client not notifiable
        else if (status == ERR_CLIENT_NOT_NOTIFIABLE) {     // 420 -> out code 420
            setErrorF(ERR_CLIENT_NOT_NOTIFIABLE, "HTTP server error: %d. Client not notifiable.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        // to handle the http error code for the tcp/ip notification and client not notifiable
        // code 421 is returned by newer Funambol Server to say "you're allowed to start CTP"
        else if (status == ERR_CTP_ALLOWED) {               // 421 -> out code 421
            setErrorF(ERR_CTP_ALLOWED, "HTTP server error: %d. Client not notifiable and CTP Server is available.", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        #endif
        
        else if (status == HTTP_STATUS_NOT_FOUND) {         // 404 -> out code 2060
            setErrorF(ERR_HTTP_NOT_FOUND, "HTTP request error: resource not found (status %d)", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        else if (status == HTTP_STATUS_REQUEST_TIMEOUT) {   // 408 -> out code 2061
            setErrorF(ERR_HTTP_REQUEST_TIMEOUT, "HTTP request error: server timed out waiting for request (status %d)", status);
            LOG.debug("%s", getLastErrorMsg());
            goto exit;
        }
        else {
            // Other HTTP errors -> OUT
            //lastErrorCode = ERR_HTTP_STATUS_NOT_OK;         // else -> out code 2053
            DWORD code = GetLastError();
            char* tmp = createHttpErrorMessage(code);
            setErrorF(ERR_HTTP_STATUS_NOT_OK, "HTTP request error: status received = %d): %s (code %d)", status, tmp, code);
		    LOG.debug("%s", getLastErrorMsg());
		    delete [] tmp;
		    goto exit;
        }
    } // for(numretries = 0; numretries < MAX_RETRIES; numretries++)
    

    // Too much retries -> exit
    if (numretries == MAX_RETRIES) {                        // Network error -> out code 2001
        setErrorF(ERR_CONNECT, "HTTP request error: %d attempts failed.", numretries); 
        LOG.error("%s", getLastErrorMsg());
        goto exit;
    }

    //Initialize response
    contentLength=0;
    HttpQueryInfo (request,
                   HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&contentLength,
                   (LPDWORD)&size,
                   NULL);



#ifdef USE_ZLIB
    int uncompressedContentLenght = 0;

    // Release the send buffer (also set msgToSend to NULL, to
    // avoid leaving a dangling pointer around.
    if (compr) {
        delete [] compr; compr = NULL;
        msgToSend = NULL;
    }

    //
    // Read headers: get contentLenght/Uncompressed-Content-Length.
    //
    wbuffer = new WCHAR[1024];
    DWORD ddsize = 1024;
    if (!HttpQueryInfo(request,HTTP_QUERY_RAW_HEADERS_CRLF ,(LPVOID)wbuffer,&ddsize,NULL)) {
        if (ERROR_HTTP_HEADER_NOT_FOUND == GetLastError()) {
            isToDeflate = false;
        }
    }
    LOG.debug("Header: %ls", wbuffer);
    delete [] wbuffer; wbuffer = NULL;

    // isToDeflate to be set
    DWORD dwSize = 512;
    buffer = new WCHAR[dwSize];
    memset(buffer, 0, dwSize*sizeof(WCHAR));

    wcscpy(buffer, TEXT("Accept-Encoding"));
    HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToDeflate = false;
    } else {
        isToDeflate = true;
    }

    memset(buffer, 0, dwSize*sizeof(WCHAR));
    wcscpy(buffer, TEXT("Content-Encoding"));
    HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToInflate = false;
    } else {
        if (wcscmp(buffer, TEXT("deflate")) == 0)
            isToInflate = true;
        else
            isToInflate = false;
    }

    if(isToInflate) {
        memset(buffer, 0, dwSize*sizeof(WCHAR));
        wcscpy(buffer, TEXT("Uncompressed-Content-Length"));

        HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
        if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
            LOG.error("Error reading 'Uncompressed-Content-Length' header.");
            uncompressedContentLenght = -1;
        }
        else {
            uncompressedContentLenght = wcstol(buffer, NULL, 10);
            LOG.debug("Uncompressed-Content-Length: %ld", uncompressedContentLenght);
        }

        // Check header value, use MAX_MSG_SIZE if not valid.
        if(uncompressedContentLenght <= 0) {
            LOG.error("Invalid value, using max message size.");
            uncompressedContentLenght = maxmsgsize * 2;
        }
    }

    delete [] buffer;
    buffer = NULL;

#endif


//
// ====================================== Reading Response ======================================
//
    LOG.debug("%s", READING_RESPONSE);
    LOG.debug("Content-length: %u", contentLength);

    if (contentLength <= 0) {
        LOG.debug("Undefined content-length = %u. Using the maxMsgSize = %u.", contentLength, maxmsgsize);
        contentLength = maxmsgsize;
    }

    // Allocate a block of memory for response read.
    response = new char[contentLength+1];
    if (response == NULL) {
        //lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        //sprintf(lastErrorMsg, "Not enough memory to allocate a buffer for the server response: %d required.", contentLength);
        setErrorF(ERR_NOT_ENOUGH_MEMORY, "Not enough memory to allocate a buffer for the server response: %d required.", contentLength);
        LOG.error("%s", getLastErrorMsg());
        goto exit;
    }
    memset(response, 0, contentLength);
	p = response;
    int realResponseLenght = 0;

    // Fire Data Received Transport Event.
    fireTransportEvent(contentLength, RECEIVE_DATA_BEGIN);

    do {
        if (!InternetReadFile(request, (LPVOID)bufferA, readBufferSize, &read)) {
            DWORD code = GetLastError();
            //lastErrorCode = ERR_READING_CONTENT;
            char* tmp = createHttpErrorMessage(code);
            //sprintf(lastErrorMsg, "InternetReadFile Error: %d - %s", code, tmp);
            setErrorF(ERR_READING_CONTENT, "InternetReadFile Error: %d - %s", code, tmp);
            delete [] tmp;
            goto exit;
		}

        // Sanity check: some proxy could send additional bytes.
        // Correct 'read' value to be sure we won't overflow the 'response' buffer.
        if ((realResponseLenght + read) > contentLength) {
            LOG.debug("Warning! %d bytes read -> truncating data to content-lenght = %d.", (realResponseLenght + read), contentLength);
            read = contentLength - realResponseLenght;
        }

        if (read > 0) {
            memcpy(p, bufferA, read);               // Note: memcopy exactly the bytes read (could be no readable chars...)
            p += read;
            realResponseLenght += read;

            // Fire Data Received Transport Event
            fireTransportEvent(read, DATA_RECEIVED);
        }

    } while (read);

    // free read buffer
    delete [] bufferA; bufferA = NULL;

    if (realResponseLenght <= 0) {
        //lastErrorCode = ERR_READING_CONTENT;
        //sprintf(lastErrorMsg, "Error reading HTTP response from Server: received data of size = %d.", realResponseLenght);
        setErrorF(ERR_READING_CONTENT, "Error reading HTTP response from Server: received data of size = %d.", realResponseLenght);
        goto exit;
    }

    // Log bytes read if different from content length
    // (should be already the same...)
    if (realResponseLenght != contentLength) {
        LOG.info("Bytes read: ", realResponseLenght);
    	contentLength = realResponseLenght;
    }
    response[contentLength] = 0;

    // Fire Receive Data End Transport Event
    fireTransportEvent(contentLength, RECEIVE_DATA_END);

    //------------------------------------------------------------- Response read

#ifdef USE_ZLIB

    if (isToInflate) {
        //
        // INFLATE (decompress data)
        //
        uLong uncomprLen = uncompressedContentLenght;
        Bytef* uncompr = new Bytef[uncomprLen + 1];

        // Decompresses the source buffer into the destination buffer.
        int err = uncompress(uncompr, &uncomprLen, (Bytef*)response, contentLength);

        if (err == Z_OK) {
            delete [] response;
            response = (char*)uncompr;
            response[uncompressedContentLenght] = 0;
        }
        else if (err < 0) {
            // Save the msg to file, for debugging...
            dumpMessage(response, contentLength);

            LOG.error("Error from zlib: %s", zError(err));
            delete [] response;
            response = NULL;
            status = ERR_HTTP_INFLATE;
            setError(ERR_HTTP_INFLATE, "ZLIB: error occurred decompressing data from Server.");

            goto exit;
        }
    }

#endif

    LOG.debug("Response read:\n%s", response);
    //LOG.debug("Response read:");
    //LOG.debugHideDataContent(response);


exit:
    // Close the Internet handles.
    if (inet) {
        InternetCloseHandle (inet);
    }
    if (connection) {
        InternetCloseHandle (connection);
    }
    if (request) {
        InternetCloseHandle (request);
    }

    if ((status != STATUS_OK) && (response !=NULL)) {
        delete [] response; response = NULL;
    }
    if (wurlHost)     delete [] wurlHost;
    if (wurlResource) delete [] wurlResource;
    if (bufferA)      delete [] bufferA;

#ifdef USE_ZLIB
    if (compr)        delete [] compr;
    if (buffer)       delete [] buffer;
    if (wbuffer)      delete [] wbuffer;
#endif
    EXITING(L"TransportAgent::sendMessage");

    return response;
}


/**
 * Utility function to retrieve the correspondant message for the Wininet error code passed.
 * Pointer returned is allocated new, must be freed by caller.
 * @param errorCode  the code of the last error
 * @return           the error message for the passed code, new allocated buffer
 */
char* WinTransportAgent::createHttpErrorMessage(DWORD errorCode) {

    WCHAR* errorMessage = new WCHAR[512];
    memset(errorMessage, 0, 512);

    FormatMessage(
                FORMAT_MESSAGE_FROM_HMODULE,
                GetModuleHandle(L"wininet.dll"),
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
                errorMessage,
                512,
                NULL);

    if (!errorMessage || wcslen(errorMessage) == 0) {
        wsprintf(errorMessage, L"Unknown error.");
    }

    char* ret = toMultibyte(errorMessage);
    if (errorMessage) delete [] errorMessage;
    return ret;
}



/** 
 * Saves the msg passed into a file under "\dump" for debugging purpose.
 * The message is saved as a binary file.
 */
void WinTransportAgent::dumpMessage(const char* msg, const int msgLen) {

    if (msgLen == 0) {
        return;
    }
    char fileName[100], now[30];
    SYSTEMTIME sys_time;
    GetLocalTime(&sys_time);
    sprintf(now, "%04d-%02d-%02d_%02d.%02d.%02d", sys_time.wYear, sys_time.wMonth, sys_time.wDay,
                                                  sys_time.wHour, sys_time.wMinute, sys_time.wSecond);

    // TODO: store dump files under installDir instead of root folder?
    CreateDirectory(L"\\dump", NULL);
    sprintf(fileName, "\\dump\\%s.dmp", now);
    saveFile(fileName, msg, msgLen, true);
}

