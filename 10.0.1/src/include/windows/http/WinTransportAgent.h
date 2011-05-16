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

#ifndef INCL_WIN_TRANSPORT_AGENT
#define INCL_WIN_TRANSPORT_AGENT
/** @cond DEV */

#include "base/fscapi.h"
#include "http/URL.h"
#include "http/Proxy.h"
#include "http/TransportAgent.h"
#include "http/HttpAuthentication.h"

/** Max number of attempts sending http requests. */
#define MAX_RETRIES                     3                       // Max number of attempts sending http requests.

// FIXME: should these go to http/errors.h ?
#define ERR_HTTP_TIME_OUT               ERR_TRANSPORT_BASE+ 7
#define ERR_HTTP_NOT_FOUND              ERR_TRANSPORT_BASE+60
#define ERR_HTTP_REQUEST_TIMEOUT        ERR_TRANSPORT_BASE+61
#define ERR_HTTP_INFLATE                ERR_TRANSPORT_BASE+70
#define ERR_HTTP_DEFLATE                ERR_TRANSPORT_BASE+71

#define ERROR_INTERNET_OFFLINE_MODE     0x0002                  // Not sure why it's not defined under wininet.h ...
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class WString;

typedef enum {
    HTTP_POST = 0,
    HTTP_GET    
} HTTPVerbs;

/**
 * This class is the transport agent responsible for messages exchange
 * over an HTTP connection.
 * This is a generic abtract class which is not bound to any paltform
 */
class WinTransportAgent : public TransportAgent {


public:
    WinTransportAgent();
    WinTransportAgent(const URL& url, Proxy& proxy,
                      unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT,
                      unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE);
    ~WinTransportAgent();

    /**
     * Sends the given SyncML message to the server specified
     * by the install property 'url'. Returns the server's response.
     * The response string has to be freed with delete [].
     * In case of an error, NULL is returned and lastErrorCode/Msg
     * is set.
     * @param msg  the message to send to the Server
     * @return     the message received from the Server
     */
    char*  sendMessage(const char*  msg);

    /**
    * Sends the given message without assuming it to be a zero terminated string
    * @param msg buffer to send to server
    * @param length number of bytes to be read from the buffer
    */
    char*  sendMessage(const char* msg, const unsigned int length);

    /**
     * Set the value of the verb to communicate in http. This value could be
     * HTTP_GET or HTTP_POST (default). 
     *
     */
    void setHttpVerb(HTTPVerbs value) { httpVerb = value; }

    /**
     * Set the authentication object to use to authenticate messages.
     *
     */
    void setAuthentication(HttpAuthentication *auth);

    /**
     * Static method: try to close the internet connection, if open.
     * If the trAgent is opened and stuck, this call will release the handle
     * with a connection error.
     */
    static int closeConnection();

private:
    bool isToDeflate;           // to be zipped
    bool isFirstMessage;        // first message is clear
    bool isToInflate;           // to be unzipped
    HttpAuthentication *auth;
    void addAuthenticationHeaders(HINTERNET hRequest);
	
    HTTPVerbs httpVerb;         // It uses the HTTP_GET and HTTP_POST (default).
    HTTPVerbs getHttpVerb() const { return httpVerb; }
    
    char* createHttpErrorMessage(DWORD errorCode);
    void  dumpMessage(const char* msg, const int msgLen);
    void  readHttpHeader(HINTERNET hRequest);
    void  writeHttpHeaders(WString &headers);

#ifdef _WIN32_WCE
public:
    void setKeepAlive() {keepalive = true;}
    bool keepalive;
#endif
};


END_NAMESPACE

/** @endcond */
#endif
