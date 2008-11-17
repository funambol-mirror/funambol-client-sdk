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

#ifndef INCL_TRANSPORT_AGENT
    #define INCL_TRANSPORT_AGENT
/** @cond DEV */

    #include "base/fscapi.h"

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
        char userAgent[128];
        BOOL compression;

    public:
        TransportAgent();
        TransportAgent(URL& url,
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
        virtual void setURL(URL& newURL);

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

        virtual void setCompression(BOOL newCompression);
        virtual BOOL getCompression();

        virtual const char* getUserAgent();

        /**
         * Returns the buffer size
         */
        virtual unsigned int getReadBufferSize();


        /*
         * Sends the given SyncML message to the server specified
         * by the install property 'url'. Returns the server's response.
         * The response string has to be freed with delete [].
         * In case of an error, NULL is returned and lastErrorCode/Msg
         * is set.
         */
        virtual char*  sendMessage(const char*  msg) = 0;

    };

/** @endcond */
#endif
