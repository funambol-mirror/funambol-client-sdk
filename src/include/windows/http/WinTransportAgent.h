/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
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

/** Max number of attempts sending http requests. */
#define MAX_RETRIES                     3                       // Max number of attempts sending http requests.

// FIXME: should these go to http/errors.h ?
#define ERR_HTTP_TIME_OUT               ERR_TRANSPORT_BASE+ 7
#define ERR_HTTP_NOT_FOUND              ERR_TRANSPORT_BASE+60
#define ERR_HTTP_REQUEST_TIMEOUT        ERR_TRANSPORT_BASE+61
#define ERR_HTTP_INFLATE                ERR_TRANSPORT_BASE+70
#define ERR_HTTP_DEFLATE                ERR_TRANSPORT_BASE+71

#define ERROR_INTERNET_OFFLINE_MODE     0x0002                  // Not sure why it's not defined under wininet.h ...


/**
 * This class is the transport agent responsible for messages exchange
 * over an HTTP connection.
 * This is a generic abtract class which is not bound to any paltform
 */
class WinTransportAgent : public TransportAgent {


public:
    WinTransportAgent();
    WinTransportAgent(URL& url, Proxy& proxy,
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

private:
    BOOL isToDeflate;           // to be zipped
    BOOL isFirstMessage;        // first message is clear
    BOOL isToInflate;           // to be unzipped

    char* createHttpErrorMessage(DWORD errorCode);
};

/** @endcond */
#endif