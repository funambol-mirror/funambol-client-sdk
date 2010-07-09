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

#include "base/Log.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/util/WString.h"
#include "base/util/KeyValuePair.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/HttpConnection.h"
#include "event/FireEvent.h"
#include "http/WinDigestAuthHashProvider.h"
#include <Wincrypt.h>

#ifdef _WIN32_WCE
#include "http/GPRSConnection.h"
#endif


#ifdef USE_ZLIB
#include "zlib.h"
#include "base/globalsdef.h"
#endif

BEGIN_FUNAMBOL_NAMESPACE

HttpConnection::HttpConnection(const char* user_agent) : 
                AbstractHttpConnection(user_agent), inet(NULL), connection(NULL), req(NULL) 
{
#ifdef _WIN32_WCE
    // used by default. check connection before...
    if (!EstablishConnection()) {
        #ifdef WIN32_PLATFORM_PSPC
        setErrorF(ERR_INTERNET_CONNECTION_MISSING, "%s: %d", "Internet Connection Missing", ERR_INTERNET_CONNECTION_MISSING);
        #else
        LOG.error("Warning: internet connection missing.");
        #endif
    }
#endif   
}

HttpConnection::~HttpConnection()
{
    close();

#ifdef _WIN32_WCE
    if (!keepalive) { 
        DropConnection();
    }
    keepalive = false;
#endif
}


int HttpConnection::open(const URL& url, RequestMethod method) 
{
    WCHAR* wurlHost      = NULL;
    WCHAR* wurlResource  = NULL;
    WString headers;
    const WCHAR* http_verb = METHOD_POST;
    WCHAR* ua = NULL;
    DWORD flags = INTERNET_FLAG_RELOAD |
        INTERNET_FLAG_NO_CACHE_WRITE |
        INTERNET_FLAG_KEEP_CONNECTION |           // This is necessary if authentication is required.
        INTERNET_FLAG_NO_COOKIES;                 // This is used to avoid possible server errors on successive sessions.

    LPCWSTR acceptTypes[2] = {TEXT("*/*"), NULL};
    int ret = 0;

    
    if ((url.fullURL == NULL) || (strlen(url.fullURL) == 0)) {
        setErrorF(ERR_HOST_NOT_FOUND, "%s: %s.", __FUNCTION__, ERRMSG_HOST_NOT_FOUND);

        return 1;
    }

    if (url.isSecure()) {
        flags = flags| INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
            | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    }

    ua = toWideChar(userAgent);

    // open connection.
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);

    if (ua) {
        delete [] ua; 
        ua = NULL; 
    }

    if (!inet) {
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        setErrorF(ERR_NETWORK_INIT, "InternetOpen Error: %d - %s", code, tmp);
        delete [] tmp;

        return 1;
    }

    LOG.debug("Connecting to %s", url.host);
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
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        setErrorF(ERR_CONNECT, "InternetConnect Error: %d - %s", code, tmp);
        delete [] tmp;
        return 1;
    }
    //
    // Open an HTTP request handle. By default it uses the POST method (see constructor).
    // it could be set the GET if someone would use it. The api doesn't use
    // GET at all.
    //    
    switch (method) {
        case MethodGet:   
            http_verb = METHOD_GET;
            break;
        case MethodPost:  
            http_verb = METHOD_POST;
            break;
        default:
            // HTTP method not supported...
            LOG.error("%s: unsupported HTTP request type", __FUNCTION__);

            return 1;
    }

    LOG.debug("%s: HTTP verb type: %S", __FUNCTION__, http_verb);
    wurlResource = toWideChar(url.resource);
    LOG.debug("%s: HTTP url resource: %S", __FUNCTION__, wurlResource);

    if (!(req = HttpOpenRequest(connection, http_verb, wurlResource, HTTP_VERSION,
        NULL, acceptTypes, flags, 0))) {
            DWORD code = GetLastError();
            char* tmp = createHttpErrorMessage(code);
            setErrorF(ERR_CONNECT, "HttpOpenRequest Error: %d - %s", code, tmp);
            delete [] tmp;
            ret = 1;
    }

    delete [] wurlResource;

    return ret;
}


int HttpConnection::close()
{
    // Close the Internet handles.
    if (inet) {
        InternetCloseHandle (inet);
    }

    if (connection) {
        InternetCloseHandle (connection);
    }

    if (req) {
        InternetCloseHandle (req);
    }

	return 0;
}

int HttpConnection::request(InputStream& data, OutputStream& response)
{
    int readBytes = 0; 
    int totalBytesRead = 0;
    DWORD bytesWritten = 0;
    int ret = HTTP_STATUS_OK;
    WString headers;
    bool sendDataAtOnce = false;
    char *chunkToSend = NULL;
    int contentLen = 0; 
    INTERNET_BUFFERS BufferIn = {0};
    DWORD errorCode = 0;

#ifdef USE_ZLIB
    Bytef* cBuf = NULL;
    uLong  cBufLen  = 0;
    int uncompressedContentLen = 0;
#endif

    // Timeout to receive a rensponse from server (default = 5 min).
    DWORD timeoutMsec = timeout*1000;
    InternetSetOption(req, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMsec, sizeof(DWORD));

    if (auth) {
        if (auth->getType() == HttpAuthentication::Basic) {
           StringBuffer authCred = auth->getAuthenticationHeaders();
           StringBuffer authHeader;
           authHeader.sprintf("Basic %s", authCred.c_str());

           setRequestHeader(HTTP_HEADER_AUTHORIZATION, authHeader);
        } else {
            LOG.error("%s: authentication type not supported [%d]", __FUNCTION__, auth->getType());
    
            return StatusInternalError;
        }
    }

    // For user agent, content length and accept encoding, override property
    // values, even if set by the caller.
    setRequestHeader(HTTP_HEADER_USER_AGENT, userAgent);
    
    contentLen = data.getTotalSize();

#ifdef USE_ZLIB
    if (compression_enabled) {
        chunkToSend = new char [contentLen];   

        if (data.read((void *)chunkToSend, contentLen) != contentLen) {
            LOG.error("error reading data from input stream");
                
            delete [] chunkToSend;

            return StatusInternalError;
        }

        sendDataAtOnce = true;

        cBufLen = contentLen;

        // DEFLATE (compress data)
        cBuf =  new Bytef[contentLen];
 
        // compress the source buffer into the destination buffer.
        int err = compress(cBuf, &cBufLen, (Bytef*)chunkToSend, contentLen);

        if (err != Z_OK) {
            LOG.error("%s: error compressing data buffer [%d]", __FUNCTION__, err);

            setError(ERR_HTTP_DEFLATE, "ZLIB: error occurred compressing data.");
            delete [] chunkToSend;
            delete [] cBuf;

            return StatusInternalError;
        }
        
        uncompressedContentLen = contentLen;
        contentLen = cBufLen;

        setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, StringBuffer().append(contentLen));
        setRequestHeader(HTTP_HEADER_ACCEPT_ENCODING, "deflate");
        setRequestHeader(HTTP_HEADER_CONTENT_ENCODING, "deflate");
        setRequestHeader(HTTP_HEADER_UNCOMPRESSED_CONTENT_LENGTH, StringBuffer().append(uncompressedContentLen));
    } else {
        setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, StringBuffer().append(contentLen));
    }
#else
    setRequestHeader(HTTP_HEADER_CONTENT_LENGTH, StringBuffer().append(contentLen));
#endif

    writeHttpHeaders(headers);

    LOG.debug("Request header:\n\n%ls", headers.c_str());

    // if the client allows to sync over https even if the server
    // has an invalid certificate, the flag is false. By default it is true
    if (getSSLVerifyServer() == false) {
        DWORD dwFlags, dwBuffLen = sizeof(dwFlags);
        InternetQueryOption (req, INTERNET_OPTION_SECURITY_FLAGS,
                                                         (LPVOID)&dwFlags, &dwBuffLen);    
        dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        InternetSetOption (req, INTERNET_OPTION_SECURITY_FLAGS,
                                                   &dwFlags, sizeof (dwFlags));        
    }
    
    BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
    BufferIn.Next = NULL;
    BufferIn.lpcszHeader = headers.c_str();
    BufferIn.dwHeadersLength = headers.length(); 
    BufferIn.dwHeadersTotal = 0;
    BufferIn.lpvBuffer = NULL;
    BufferIn.dwBufferLength = 0;
    BufferIn.dwBufferTotal = contentLen;
    BufferIn.dwOffsetLow = 0;
    BufferIn.dwOffsetHigh = 0;

    if (!HttpSendRequestEx(req, &BufferIn, NULL, HSR_INITIATE, 0)) {
        LOG.error("%s: error on HttpSendRequestEx %lu\n", __FUNCTION__, GetLastError());
        return StatusInternalError;
    }

#ifdef USE_ZLIB
    if (compression_enabled) {        
        if (sendData((const char *)cBuf, cBufLen) != 0) {
            delete [] cBuf;
            delete [] chunkToSend;

            return StatusWritingError;
        }

        delete [] cBuf;
    }
#endif

    if (!sendDataAtOnce) {
        chunkToSend = new char [chunkSize];   

        while ((readBytes = data.read((void *)chunkToSend, chunkSize))) {
            if (sendData(chunkToSend, readBytes) != 0) {
                delete [] chunkToSend;

                return StatusWritingError;
            }
        }
    }

    delete [] chunkToSend;

    if (!HttpEndRequest(req, NULL, 0, 0)) {
        LOG.error("%s: HttpEndRequest failed", __FUNCTION__);

        return StatusInternalError;
    }

    if ((ret = readResponseHeaders()) != HTTP_STATUS_OK) {
        return ret;
    }

    if (readResponse(response) != 0) {
        return StatusReadingError;
    }

    return ret;
}

int HttpConnection::readResponseHeaders()
{
    DWORD status, size;

    // Check the status code.
    size = sizeof(status);
    HttpQueryInfo (req,
        HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
        (LPDWORD)&status,
        (LPDWORD)&size,
        NULL);

    // OK: status 200
    if (status == HTTP_STATUS_OK) {
        LOG.debug("data sent successfully: server responds OK");
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
            if (strcmp(proxy.user, "") && strcmp(proxy.password, "") && !auth) {
                WCHAR* wUser = toWideChar(proxy.user);
                WCHAR* wPwd  = toWideChar(proxy.password);

                InternetSetOption(req, INTERNET_OPTION_PROXY_USERNAME, wUser, wcslen(wUser)+1);
                InternetSetOption(req, INTERNET_OPTION_PROXY_PASSWORD, wPwd,  wcslen(wPwd)+1);

                delete [] wUser;
                delete [] wPwd;
                dwError = ERROR_INTERNET_FORCE_RETRY;
            }

            // Prompt dialog box.
            else if (!auth) {
                dwError = InternetErrorDlg(GetDesktopWindow(), req, NULL,
                    FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
                    FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
                    FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
                    NULL);
            }

            if (dwError == ERROR_INTERNET_FORCE_RETRY) {
                return dwError;    
            }
            else {
                LOG.error("HTTP Authentication failed.");
                return dwError;
            }
    }
#endif  // #if defined(WIN32) && !defined(_WIN32_WCE)

    else {
        LOG.error("[HttpConnection]: server returned status %d", status);
        return status;
    }


    // Read Http headers -----------------------------------------------------------------
    WCHAR *wbuffer = new WCHAR[1024];
    DWORD  ddsize = 1024;
	StringBuffer headerString;

    responseHeaders.clear();

    if (HttpQueryInfo(req, HTTP_QUERY_RAW_HEADERS_CRLF ,(LPVOID)wbuffer, &ddsize, NULL)) {
		headerString.convert(wbuffer);
		LOG.debug("Response Headers:", headerString.c_str());

		ArrayList headers;
		headerString.split(headers, "\r\n");

		StringBuffer *prop;

		for(ArrayElement* e=headers.front(); e; e=headers.next()) {
			prop = dynamic_cast<StringBuffer *>(e);
			if(prop->empty()) continue;

			size_t colon = prop->find(":");
			if (colon != StringBuffer::npos) {
				StringBuffer key = prop->substr(0, colon);
				StringBuffer value = prop->substr(colon+1);
				responseHeaders.put(key.trim(),value.trim());
				LOG.debug("\t%s : %s", key.c_str(), value.c_str());
			}
			else {
				LOG.debug("\t%s", prop->c_str());
			}
		}
    } else {
        LOG.error("[HttpConnection] Error reading response headers: %d", GetLastError());
        return StatusReadingError;
    }

    return status;
}

int HttpConnection::readResponse(OutputStream& os)
{
    char* responseBuffer  = NULL;
    char* zResponseBuffer = NULL;
    bool writeDataAtOnce = false;
    DWORD read = 0;
    int ret = 0;
    bool inflate = false;

#ifdef USE_ZLIB
    DWORD contentLength = 0;
    DWORD uncompressedContentLength = 0;
    DWORD size   = 512;
  
    if (compression_enabled) {

        //
        // get response length
        //
        StringBuffer val = getResponseHeader(HTTP_HEADER_CONTENT_LENGTH);
        if (val.empty()) {
            LOG.error("error reading %s from HTTP headers", HTTP_HEADER_CONTENT_LENGTH);
            return StatusInvalidParam;
        }
        contentLength = atoi(val.c_str());
        if (contentLength <= 0) {
            LOG.error("error reading %s from HTTP headers: %d", HTTP_HEADER_CONTENT_LENGTH, contentLength);
            return StatusInvalidParam;
        }

        //
        // get response encoding
        //
        val = getResponseHeader(HTTP_HEADER_CONTENT_ENCODING);
        if (val.empty()) {
            LOG.error("error reading %s from HTTP headers", HTTP_HEADER_CONTENT_ENCODING);
            return StatusInvalidParam;
        }
        if (val == "deflate") {
            inflate = true;
        }

        if (inflate) {
            //
            // get uncompressed content length
            //
            val = getResponseHeader(HTTP_HEADER_UNCOMPRESSED_CONTENT_LENGTH);
            if (val.empty()) {
                LOG.error("error reading %s from HTTP headers", HTTP_HEADER_UNCOMPRESSED_CONTENT_LENGTH);
                return StatusInvalidParam;
            }
            uncompressedContentLength = atoi(val.c_str());
            if (uncompressedContentLength <= 0) {
                LOG.error("error reading %s from HTTP headers: %d", HTTP_HEADER_UNCOMPRESSED_CONTENT_LENGTH, uncompressedContentLength);
                return StatusInvalidParam;
            }

            zResponseBuffer = new char[contentLength + 1];
        }
    }
#endif

    // Allocate a block of memory for response read.
    responseBuffer = new char[chunkSize + 1];
    memset(responseBuffer, 0, chunkSize);
    int zOffset = 0;
  
    do {
        if (!InternetReadFile(req, (LPVOID)responseBuffer, chunkSize, &read)) {
            ret = StatusReadingError;
            break;
        }

        if (read > 0) {
            if (inflate) {
                memcpy(zResponseBuffer + zOffset, responseBuffer, read);
                zOffset += read;
            } else {
                os.write(responseBuffer, read);
            }
        }
    } while (read);

    delete [] responseBuffer; 


    if (inflate) {
        uLong uncomprLen = uncompressedContentLength;
        Bytef* uncompr = new Bytef[uncompressedContentLength + 1];

        //
        // Decompresses the source buffer into the destination buffer.
        //
        int err = uncompress(uncompr, &uncomprLen, (Bytef*)zResponseBuffer, contentLength);

        if (err == Z_OK) {
            char* response = (char*)uncompr;
            response[uncompressedContentLength] = 0;
            os.write(response, uncompressedContentLength);
        } else  {
            LOG.error("Error in zlib uncompress: %s", zError(err));
            ret = StatusInternalError;
        }
        delete [] uncompr;
    }


    delete [] zResponseBuffer;
    return ret;
}

int HttpConnection::sendData(const char* data, int data_len)
{
    int totalBytesRead = 0;
    int ret = 0;
    DWORD bytesWritten = 0;
    DWORD errorCode = 0;

    if (!InternetWriteFile(req, data, data_len, &bytesWritten)) {
        errorCode = GetLastError();

        char* tmp = createHttpErrorMessage(errorCode);
        setErrorF(errorCode, "InternetWriteFile error %d: %s", errorCode, tmp);
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
            if (TRUE == InternetQueryOption(req, INTERNET_OPTION_SECURITY_CERTIFICATE_STRUCT, 
                &certificateInfo, &certInfoLength)) {

                    char* subj   = (char*)certificateInfo.lpszSubjectInfo;
                    char* issuer = (char*)certificateInfo.lpszIssuerInfo;    
                    LOG.debug("Cert Subject %s", subj);
                    LOG.debug("Cert Issuer %s",  issuer);

            } else {                        
                LOG.debug("Cannot retrieve info about the certificate");
            }     
        } else if (errorCode == ERROR_INTERNET_OFFLINE_MODE) {                     // 00002 -> retry
            LOG.debug("Offline mode detected: go-online and retry...");
            WCHAR* wurl = toWideChar(url.fullURL);
            InternetGoOnline(wurl, NULL, NULL);
            delete [] wurl;
        } else if (errorCode == ERROR_INTERNET_TIMEOUT ||                     // 12002 -> out code 2007
            errorCode == ERROR_INTERNET_INCORRECT_HANDLE_STATE) {      // 12019 -> out code 2007
                setError(ERR_HTTP_TIME_OUT, "Network error: the request has timed out -> exit.");
                LOG.debug("%s", getLastErrorMsg());
        } else if (errorCode == ERROR_INTERNET_CANNOT_CONNECT) {              // 12029 -> out code 2001
            setError(ERR_CONNECT, "Network error: the attempt to connect to the server failed -> exit"); 
            LOG.debug("%s", getLastErrorMsg());
        }
        // Other network error: retry.
        LOG.info("Network error writing data from client");
        resetError();
    }

    return errorCode;
}

/**
 * Utility function to retrieve the correspondant message for the Wininet error code passed.
 * Pointer returned is allocated new, must be freed by caller.
 * @param errorCode  the code of the last error
 * @return           the error message for the passed code, new allocated buffer
 */
char* HttpConnection::createHttpErrorMessage(DWORD errorCode) 
{
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
 * Add authentication headers generated by the authentication object to the request.
 * Headers are added with the setProperty method.
 *
 * @param hRequest The request to add authenticatino headers to
 */
void HttpConnection::addAuthenticationHeaders() 
{
    DWORD dwStatus;
    DWORD cbStatus = sizeof(dwStatus);
    BOOL fRet;
    WCHAR szScheme[256];
    DWORD dwIndex = 0;
    DWORD cbScheme = sizeof(szScheme);
    DWORD dwFlags;
    StringBuffer authresponse;

    
    HttpSendRequest(req, L"", 0, "", 0);
    HttpQueryInfo
    (
        req,
        HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_STATUS_CODE,
        &dwStatus,
        &cbStatus,
        NULL
    );

    switch (dwStatus) {
        case HTTP_STATUS_DENIED:
            dwFlags = HTTP_QUERY_WWW_AUTHENTICATE;
            break;          
        default:
            return;
    }

    fRet = HttpQueryInfo(req, dwFlags, szScheme, &cbScheme, &dwIndex);
    if (fRet) {
        HashProvider *hashProvider = new WinDigestAuthHashProvider();
        authresponse = auth->getAuthenticationHeaders(toMultibyte(szScheme), url, hashProvider);
        responseHeaders.put("Authorization", authresponse);
    }
}


void HttpConnection::writeHttpHeaders(WString &headers)
{
    KeyValuePair p;
    headers = TEXT("");

    for (p=requestHeaders.front(); !p.null(); p=requestHeaders.next()) {
        StringBuffer prop;
        prop.sprintf("%s: %s\r\n", p.getKey().c_str(),
            p.getValue().c_str());

        WString wprop;
        wprop = prop;
        headers += wprop;
    }
}

END_FUNAMBOL_NAMESPACE