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

#ifndef INCL_ABSTRACT_HTTP_CONNECTION
#define INCL_ABSTRACT_HTTP_CONNECTION
/** @cond API */

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/constants.h"
#include "http/constants.h"
#include "http/HttpAuthentication.h"
#include "http/Proxy.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/util/StringMap.h"
#include "inputStream/InputStream.h"
#include "inputStream/OutputStream.h"

BEGIN_FUNAMBOL_NAMESPACE


// default HTTP headers:
#define HTTP_HEADER_USER_AGENT                      "User-Agent"
#define HTTP_HEADER_ACCEPT                          "Accept"
#define HTTP_HEADER_ACCEPT_ENCODING                 "Accept-Encoding"
#define HTTP_HEADER_CONTENT_TYPE                    "Content-Type"
#define HTTP_HEADER_CONTENT_LENGTH                  "Content-Length"
#define HTTP_HEADER_UNCOMPRESSED_CONTENT_LENGTH     "Uncompressed-Content-Length"
#define HTTP_HEADER_SET_COOKIE                      "Set-Cookie"
#define HTTP_HEADER_COOKIE                          "Cookie"
#define HTTP_HEADER_AUTHORIZATION                   "Authorization"
#define HTTP_HEADER_CONTENT_ENCODING                "Content-Encoding"
#define HTTP_HEADER_TRANSFER_ENCODING               "Transfer-Encoding"
#define HTTP_HEADER_CONTENT_RANGE                   "Content-Range"


// custom HTTP headers:
#define HTTP_HEADER_X_FUNAMBOL_DEVICE_ID            "x-funambol-syncdeviceid"
#define HTTP_HEADER_X_FUNAMBOL_FILE_SIZE            "x-funambol-file-size"
#define HTTP_HEADER_X_FUNAMBOL_LUID                 "x-funambol-luid"


// Max chunk size for http request [bytes]
#define DEFAULT_REQUEST_MAX_CHUNK_SIZE  50000

// HTTP timeout to receive a response from the Server [seconds]
#define DEFAULT_HTTP_TIMEOUT            300


/**
 * Abstract class, it's a common interface for HTTPConnection classes.
 * It rapresents a generic class to manage HTTP connections, so it has methods
 * to open a connection, close it, and make HTTP requests (GET, PUT, POST).
 * There is an implementation of this interface for each platform (HTTPConnection)
 * since the APIs to manage HTTP connections are low level.
 * 
 * The caller should first open the connection via the method open(), specify the
 * request method, and then send the request calling request() method. The response
 * is returned in the outputStream passed.
 * If the request method is not specified, the default is POST.
 * 
 * Request headers can be set easily with the method setRequestHeader(). The 
 * headers returned in the response are available after the request completion, calling
 * the method getResponseHeader().
 */
class AbstractHttpConnection {

public:
    
    /// These are the possible request methods
    enum RequestMethod {
        MethodGet,
        MethodPost,
        MethodPut,
        MethodHead
    };

    enum ConnectionStatus {
        StatusNoError         =  0,
        StatusCancelledByUser = -1,
        StatusInternalError   = -2, 
        StatusReadingError    = -3,
        StatusWritingError    = -4,
        StatusInvalidParam    = -5,
        StatusNetworkError    = -6
    };

    /// Sets defaults, sets the userAgent.
    AbstractHttpConnection(const char* ua) : userAgent(ua)  {
        method              = MethodPost;
        auth                = NULL;
        chunkSize           = DEFAULT_REQUEST_MAX_CHUNK_SIZE;
        SSLVerifyServer     = true;
        SSLVerifyHost       = true;
        compression_enabled = false;
        keepalive           = false;
        timeout             = DEFAULT_HTTP_TIMEOUT; 
    }

    virtual ~AbstractHttpConnection() {}


    /**
     * Open the connection to the given url.
     * @param url    the connection url
     * @param method (optional) the request method, one of RequestMethod enum
     */
    virtual int open(const URL& url, RequestMethod method = MethodPost) = 0;

    /**
     * This method closes this connection. It does not close the corresponding
     * input and output stream which need to be closed separately (if they were
     * previously opened)
     */
    virtual int close() = 0;

    /**
     * Set the request method for the URL request, one of RequestMethod enum.
     * are legal, subject to protocol restrictions. The default method is POST.
     */
    void setRequestMethod(RequestMethod method) {
        this->method = method;
    }


    /**
     * Sends the request.
     * The method used must be set via setRequestMethod (default = POST).
     * The headers/authentication must already be set before calling this 
     * method (otherwise none is used).
     *
     * @param data      the data to be sent
     * @param response  the response returned 
     * @return          the HTTP Status Code returned or -1 if no status code can be discerned.
     */
    virtual int request(InputStream& data, OutputStream& response) = 0;


    /**
     * Sets the general request property. If a property with the key already exists,
     * overwrite its value with the new value.
     *
     * NOTE: HTTP requires all request properties which can legally have multiple instances
     * with the same key to use a comma-seperated list syntax which enables multiple
     * properties to be appended into a single property.
     *
     * @param key   the keyword by which the request is known (e.g., "Accept").
     * @param value the value associated with it.
     */
    virtual void setRequestHeader(const char* key, const char* value) {
        requestHeaders.put(key, value);
    }


    /**
     * Returns the value of the named response header field.
     *
     * @param   key name of a header field.
     * @return  the value of the named header field, or NULL string if there is no such field in the header.
     */
    virtual StringBuffer getResponseHeader(const char* key) {
        return responseHeaders[key];
    }
    
    /**
     * Sets the authentication object. The HttpConnection will only use 
     * authentication if this object is not NULL.
     * @param auth The authentication object to use.
     */
    virtual void setAuthentication(HttpAuthentication *auth) {
        this->auth = auth;
    }
    
    
    /// Sets the max chunk size, for HTTP request (outgoing data is chunked)
    void setChunkSize(const int size) { chunkSize = size; }
    int  getChunkSize()               { return chunkSize; }

	/**
	 * Change the URL the subsequent calls to setMessage() should
	 * use as target url.
	 *
	 * @param url the new target url
	 */
    virtual void setURL(const URL& newURL) { url = newURL; }

	/**
	 * Returns the url.
	 */
    virtual const URL& getURL() const { return url; }

	/**
	 * Sets the connection timeout
	 *
	 * @param t the new timeout in seconds
	 */
    virtual void setTimeout(unsigned int t) { timeout = t; }

	/**
	 * Returns the connection timeout
	 */
    virtual unsigned int getTimeout() const { return timeout; }
    
    /**
     * Enabled by default: the client refuses to establish the
     * connection unless the server presents a valid
     * certificate. Disabling this option considerably reduces the
     * security of SSL (man-in-the-middle attacks become possible) and
     * is not recommended.
     */
    virtual bool getSSLVerifyServer() const { return SSLVerifyServer; }
    virtual void setSSLVerifyServer(bool value) { SSLVerifyServer = value; }

    /**
     * Enabled by default: the client refuses to establish the
     * connection unless the server's certificate matches its host
     * name. In cases where the certificate still seems to be valid it
     * might make sense to disable this option and allow such
     * connections.
     */
    virtual bool getSSLVerifyHost() const { return SSLVerifyHost; }
    virtual void setSSLVerifyHost(bool value) { SSLVerifyHost = value; }

    /**
     * Enable HTTP compression 
     */
    virtual bool getCompression() const { return compression_enabled; }
    virtual void setCompression(bool value) { compression_enabled = value; }

    /**
     * Sets the keep-alive flag: the connection is not dropped on destructor.
     * Note: on some platforms this is not applicable, so it's just ignored.
     */
    virtual void setKeepAlive(bool val) { keepalive = val; }


protected:
    
	URL url;

    RequestMethod method;

    /// The property-value map of headers to send.
    StringMap requestHeaders;
    
    /// The property-value map of headers received.
    StringMap responseHeaders;
    
    /// The HTTP authentication. Auth type can be Basic or Digest.
    HttpAuthentication* auth;

    /// Max chunk size for http requests, in bytes
    int chunkSize;

	Proxy proxy;

	unsigned int timeout;

    StringBuffer userAgent;

    // SSL Parameters
    StringBuffer SSLServerCertificates;
    bool SSLVerifyServer;
    bool SSLVerifyHost;
    bool compression_enabled;

    /// If set to true, the connection is not dropped on destructor. Default = false. 
    /// Note: on some platforms this is not applicable, so it's just ignored.
    bool keepalive;
    
    
public:
    // ---- HTTP Utils methods (TODO: move to httpUtils?) ----
    
    /**
     * Parse and return the jsessionID string from the Set-Cookie header returned by server,
     * like "JSESSIONID=A37F56FC791DE3D5A25B0D31109C6D47"
     * @return the sessionID, like "A37F56FC791DE3D5A25B0D31109C6D47".
     *         In case of parse errors, an empty string is returned.
     */
    StringBuffer parseJSessionId(const StringBuffer& input) {
        
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
    
};

END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif

