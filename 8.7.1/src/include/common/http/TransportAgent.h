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

#ifndef INCL_TRANSPORT_AGENT
#define INCL_TRANSPORT_AGENT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"

#include "http/URL.h"
#include "http/Proxy.h"

//
// number of seconds of waiting response timeout.
//
#define DEFAULT_MAX_TIMEOUT 300

//
// The max_msg_size parameter. Default is 512k.
// The value is expressed in byte
//
#define DEFAULT_MAX_MSG_SIZE 512000

//
// This is the default value for the size of the buffer used to store the
// incoming stram from server. It is expressed in byte
//
#define DEFAULT_INTERNET_READ_BUFFER_SIZE  4096
#include "base/globalsdef.h"

#include "base/util/StringMap.h"

BEGIN_NAMESPACE

// Names for properties supported by TransportAgent
#define TA_PropertyAcceptEncoding               "Accept-Encoding"
#define TA_PropertyContentType                  "Content-Type"
#define TA_PropertyContentLength                "Content-Length"
#define TA_PropertyUncompressedContentLength    "Uncompressed-Content-Length"
#define TA_PropertyUserAgent                    "User-Agent"
#define TA_PropertyContentEncoding              "Content-Encoding"

/*
 * This class is the transport agent responsible for messages exchange
 * over an HTTP connection.
 * This is a generic abtract class which is not bound to any paltform
 */
class TransportAgent {

    protected:
        URL url;
        Proxy proxy;

        unsigned int timeout;
        unsigned int maxmsgsize;
        unsigned int readBufferSize;
        StringBuffer userAgent;
        bool compression;

        // SSL Parameters
        StringBuffer SSLServerCertificates;
        bool SSLVerifyServer;
        bool SSLVerifyHost;

        unsigned int responseSize;

        StringMap requestProperties; 
        StringMap responseProperties;
        int responseCode;

    public:
        TransportAgent();
        TransportAgent(const URL& url,
                Proxy& proxy,
                unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT,
                unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE);

        virtual ~TransportAgent();

        /*
         * Change the URL the subsequent calls to setMessage() should
         * use as target url.
         *
         * @param url the new target url
         */
        virtual void setURL(const URL& newURL);

        /*
         * Returns the url.
         */
        virtual URL& getURL();

        /**
         * Sets the connection timeout
         *
         * @param t the new timeout in seconds
         */
        virtual void setTimeout(unsigned int t);

        /**
         * Returns the connection timeout
         */
        virtual unsigned int getTimeout();

        /**
         * Sets the max msg size
         *
         * @param t the new msx msg size in bytes
         */
        virtual void setMaxMsgSize(unsigned int t);

        /**
         * Returns the max msg size
         */
        virtual unsigned int getMaxMsgSize();

        /**
         * Sets the buffer size
         *
         * @param t the buffer size size in bytes
         */
        virtual void setReadBufferSize(unsigned int t);

        virtual void setUserAgent(const char*  ua);

        virtual void setCompression(bool newCompression);
        virtual bool getCompression();

        virtual const char* getUserAgent();

        /**
         * Set MIME type of the received message.
         *
         * @param mime mime type of the received response
         */
        void setResponseMime(const char *mime);

        /**
         * Returns MIME type of the response message.
         *
         * Returned string should be deleted.
         */
        const char* getResponseMime();

        /**
         * Returns property of the response message
         *
         * Returned string should be deleted.
         */
        const char* getResponseProperty(const char *pname);

        /**
         * Returns size of the server response (header size not included)
         */
        unsigned int getResponseSize();

        /**
         * Sets response code
         */
        void setResponseCode(int respCode);

        /**
         * Returns response code
         */
        int getResponseCode();

        /**
         * Returns the buffer size
         */
        virtual unsigned int getReadBufferSize();

        /**
         * A platform specific string specifying the location of the
         * certificates used to authenticate the server. When empty, the
         * system's default location will be searched.
         */
        virtual const char* getSSLServerCertificates() const {
            return SSLServerCertificates.c_str();
        }
        virtual void setSSLServerCertificates(const char *value) {
            SSLServerCertificates = value ? value : ""; 
        }

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
         * Sends the given SyncML message to the server specified
         * by the install property 'url'. Returns the server's response.
         * The response string has to be freed with delete [].
         * In case of an error, NULL is returned and lastErrorCode/Msg
         * is set.
         */
        virtual char* sendMessage(const char*  msg) = 0;

        /**
         * Sends the given message without assuming it to be a zero terminated string
         * @param msg buffer to send to server
         * @param length number of bytes to be read from the buffer
         */
        virtual char* sendMessage(const char* data, const unsigned int size) = 0;

        /**
         * Set specific property of the transport layer
         */
        virtual void  setProperty(const char *propName, const char * const propValue);

        /**
         * Query server using (e.g. HTTP GET method)
         */
        virtual char* query(ArrayList& httpHeaders, long* protocolResponseCode);

};


END_NAMESPACE

/** @endcond */
#endif
