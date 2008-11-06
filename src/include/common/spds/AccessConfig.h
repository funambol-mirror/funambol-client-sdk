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
#ifndef INCL_ACCESS_CONFIG
#define INCL_ACCESS_CONFIG
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"


/*
 * -------------------------- AccessConfig class -----------------------------
 * This class groups all configuration properties to estabilish a
 * connection with a sync server.
 * AccessConfig is a part of SyncManagerConfig (along with DeviceConfig
 * and an array of SyncSourceConfig).
 *
 * Class members:
 * --------------
 * username             : string for username
 * password             : string for password
 * useProxy             : Should the sync engine use a HTTP proxy?
 * proxyHost            : the host for proxy connection
 * proxyPort            : the port for proxy connection
 * proxyUsername        : the proxy username (if proxy needs authentication)
 * proxyPassword        : the proxy password (if proxy needs authentication)
 * beginTimestamp       : The beginSync timestamp
 * endTimestamp         : The endSync timestamp
 * firstTimeSyncMode    : The SyncMode that the sync engine should use
 *                        the first time a source is synced
 * serverNonce          : The server nonce value: from client to server
 * clientNonce          : The client nonce value: from server to client
 * serverID             : the server ID value
 * serverPWD            : the server password
 * clientAuthType       : the type of client authentication used by client
 * isServerAuthRequired : Does the server require authentication?
 * maxMsgSize           : The maximum message size (Byte) accepted for XML
 *                        messages received from server (server to client)
 * readBufferSize       : Specifies the value for the size of the buffer used
 *                        to store the incoming stream from server (byte)
 * userAgent            : The user agent string, will be attached to http
 *                        messages to identify the client on server side.
 *                        It shoud be a short description with the client
 *                        name plus its version
 * checkConn            : Do we need to check if the GPRS connection is available?          <-- **** still used? ****
 * responseTimeout      : The number of seconds of waiting response timeout
 * dirty                : The dirty flag, used to select which properties
 *                        have been modified. Not used by now (T.B.D)
 */
class AccessConfig {
    private:
        char*           username            ;
        char*           password            ;
        BOOL            useProxy            ;
        char*           proxyHost           ;
        int             proxyPort           ;
        char*           proxyUsername       ;
        char*           proxyPassword       ;
        char*           syncURL             ;
        unsigned long   beginTimestamp      ;
        unsigned long   endTimestamp        ;
        SyncMode        firstTimeSyncMode   ;
        char*           serverNonce         ;
        char*           clientNonce         ;
        char*           serverID            ;
        char*           serverPWD           ;
        char*           clientAuthType      ;
        char*           serverAuthType      ;
        BOOL            isServerAuthRequired;
        unsigned long   maxMsgSize          ;
        unsigned long   readBufferSize      ;
        char*           userAgent           ;
        BOOL            checkConn           ;
        unsigned int    responseTimeout     ;
        BOOL            compression         ;

        unsigned int dirty;

        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(char* * buf, const char*  v);

    public:

        AccessConfig();
        AccessConfig(AccessConfig& s);
        ~AccessConfig();

        /**
         * Returns the username value.
         *
         * @return The username value. The caller MUST NOT release
         *         the memory itself.
         *
         */
        const char*  getUsername() const;

        /**
         *  Sets the username value. The given data are copied in an internal
         *  buffer so that the caller is assured that the given address can be
         *  released after the call.
         *
         *  @param username the new username value
         */
        void setUsername(const char*  username);

        /**
         * Returns the password value.
         */
        const char*  getPassword() const;

        /**
         * Sets a new password value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param password the new password value
         */
        void setPassword(const char*  password);

        /**
         * Returns the SyncMode that the sync engine should use the first time
         * a source is synced
         */
        SyncMode getFirstTimeSyncMode() const;

        /**
         * Sets the SyncMode that the sync engine should use the first time
         * a source is synced
         *
         * @param syncMode the new sync mode
         */
        void setFirstTimeSyncMode(SyncMode syncMode);

        /**
         * Should the sync engine use a HTTP proxy?
         */
        BOOL getUseProxy() const;

        /**
         * Sets if the sync engine should use a HTTP proxy to access the server.
         *
         * @param useProxy FALSE for not use a proxy, TRUE otherwise
         */
        void setUseProxy(BOOL useProxy);

        /**
         * Returns the proxyHost value.
         */
        const char*  getProxyHost() const;

        /**
         * Sets a new proxyHost value.
         *
         * @param proxyHost the new proxyHost value
         */
        void setProxyHost(const char*  proxyHost);

        int getProxyPort() const;
        void setProxyPort(int v);

        /**
         * Returns the proxyUsername value.
         */
        const char* getProxyUsername() const;

        /**
         * Sets a new proxyUsername value.
         *
         * @param proxyUsername the new proxyUsername value
         */
        void setProxyUsername(const char*  proxyUsername);

        /**
         * Returns the proxyPassword value.
         */
        const char* getProxyPassword() const;

        /**
         * Sets a new proxyPassword value.
         *
         * @param proxyPassword the new proxyPassword value
         */
        void setProxyPassword(const char*  proxyPassword);

        /**
         * Returns the syncURL value. If the URL does not start with http://
         * (or HTTP://) or https:// (or HTTPS://), http:// is prepended to the
         * given string.
         */
        const char*  getSyncURL() const;

        /**
         * Sets a new the syncURL value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param syncURL the new syncURL value
         */
        void setSyncURL(const char*  syncURL);

        /**
         * Sets the new "beginSync" timestamp.
         *
         * @param timestamp the beginSync timestamp
         */
        void setBeginSync(unsigned long timestamp);

        /**
         * Returns the beginSync timestamp
         */
        unsigned long getBeginSync() const;

        /**
         * Sets the new "endSync" timestamp.
         *
         * @param timestamp the endSync timestamp
         */
        void setEndSync(unsigned long timestamp);

        /**
         * Returns the endSync timestamp
         */
        unsigned long getEndSync() const;

        BOOL getServerAuthRequired() const;

        void setServerAuthRequired(BOOL v);

        const char*  getClientAuthType() const;

        void setClientAuthType(const char*  v);

        const char*  getServerAuthType() const;

        void setServerAuthType(const char*  v);

        const char*  getServerPWD() const;

        void setServerPWD(const char*  v);

        const char*  getServerID() const;

        void setServerID(const char*  v);

        const char*  getServerNonce() const;

        void setServerNonce(const char*  v);

        const char*  getClientNonce() const;

        void setClientNonce(const char*  v);

        void setMaxMsgSize(unsigned long msgSize);

        unsigned long getMaxMsgSize() const;

        void setReadBufferSize(unsigned long bufferSize);

        unsigned long getReadBufferSize() const;

        const char*  getUserAgent() const;

        void setUserAgent(const char*  v);

        void setCompression(BOOL  v);

        BOOL  getCompression() const;

        //void setCompression(BOOL v);


        void setCheckConn(BOOL v);
        BOOL getCheckConn() const;

        void setResponseTimeout(unsigned int bufferSize)   ;
        unsigned int getResponseTimeout() const            ;

        /**
         * Has some of this values changed?
         */
        unsigned int getDirty() const;

        /**
         * Sets the values of this object with with the values from the given
         * AccessConfig source object.
         *
         * @param config the new value.
         */
        void assign(const AccessConfig& s);

        /*
         * Assign operator
         */
        AccessConfig& operator = (const AccessConfig& ac) {
            assign(ac);
            return *this;
        }

};

/** @endcond */
#endif
