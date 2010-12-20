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
#ifndef INCL_ABSTRACT_SYNC_CONFIG
#define INCL_ABSTRACT_SYNC_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "spds/AbstractSyncSourceConfig.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class provides access to all attributes of the client and its
 * sources which the sync engine needs to read and (in a few cases)
 * also write.
 *
 * @todo The documentation of these properties was copied more or less
 * verbatim from the older AccessConfig.h and DeviceConfig.h and does not
 * really explain much about these properties. It should be considerably
 * enhanced.
 */
class AbstractSyncConfig {
 public:

    virtual ~AbstractSyncConfig() {}

    virtual AbstractSyncSourceConfig* getAbstractSyncSourceConfig(const char* name) = 0;
    virtual AbstractSyncSourceConfig* getAbstractSyncSourceConfig(unsigned int i) = 0;
    virtual unsigned int getAbstractSyncSourceConfigsCount() const = 0;

    /**
     * Returns the username value.
     *
     * @return The username value. The caller MUST NOT release
     *         the memory itself.
     *
     */
    virtual const char*  getUsername() const = 0;

    /**
     * Returns the password value.
     */
    virtual const char*  getPassword() const = 0;

    /**
     * Should the sync engine use a HTTP proxy?
     */
    virtual bool getUseProxy() const = 0;

    /**
     * Returns the proxyHost value.
     */
    virtual const char*  getProxyHost() const = 0;

    virtual int getProxyPort() const = 0;

    /**
     * Returns the proxyUsername value.
     */
    virtual const char* getProxyUsername() const = 0;

    /**
     * Returns the proxyPassword value.
     */
    virtual const char* getProxyPassword() const = 0;

    /**
     * Returns the syncURL value. If the URL does not start with http://
     * (or HTTP://) or https:// (or HTTPS://), http:// is prepended to the
     * given string.
     */
    virtual const char*  getSyncURL() const = 0;

    /**
     * Sets the new "beginSync" timestamp.
     *
     * The client can use this value if it wants to, but
     * it doesn't have to store it persistently.
     *
     * @param timestamp the beginSync timestamp
     */
    virtual void setBeginSync(unsigned long /* timestamp */) {}

    /**
     * Sets the new "endSync" timestamp.
     *
     * The client can use this value if it wants to, but
     * it doesn't have to store it persistently.
     *
     * @param timestamp the endSync timestamp
     */
    virtual void setEndSync(unsigned long /* timestamp */) {}

    /** Does the server require authentication? */
    virtual bool getServerAuthRequired() const = 0;
    
    /**
     * the type of client authentication used by client:
     * - syncml:basic-auth
     * - syncml:md5-auth
     */
    virtual const char*  getClientAuthType() const = 0;

    /** same as for client? */
    virtual const char*  getServerAuthType() const = 0;

    /** the server password */
    virtual const char*  getServerPWD() const = 0;

    /** the server ID value */
    virtual const char*  getServerID() const = 0;

    /** The server nonce value: from client to server */
    virtual const char*  getServerNonce() const = 0;
    virtual void setServerNonce(const char*  v) = 0;

    /** The client nonce value: from server to client */
    virtual const char*  getClientNonce() const = 0;
    virtual void setClientNonce(const char*  v) = 0;

    /**
     * The maximum message size (Byte) accepted for XML
     * messages received from server (server to client)
     */
    virtual unsigned long getMaxMsgSize() const = 0;

    /**
     * Specifies the value for the size of the buffer used to store
     * the incoming stream from server (byte).
     */
    virtual unsigned long getReadBufferSize() const = 0;

    /**
     * The user agent string, will be attached to http messages to
     * identify the client on server side.  It should be a short
     * description with the client name plus its version
     */
    virtual const char*  getUserAgent() const = 0;

    /**
     * A platform specific string specifying the location of the
     * certificates used to authenticate the server. When empty, the
     * system's default location will be searched.
     */
    virtual const char* getSSLServerCertificates() const { return ""; }

    /**
     * Enabled by default: the client refuses to establish the
     * connection unless the server presents a valid
     * certificate. Disabling this option considerably reduces the
     * security of SSL (man-in-the-middle attacks become possible) and
     * is not recommended.
     */
    virtual bool  getSSLVerifyServer() const { return true; }

    /**
     * Enabled by default: the client refuses to establish the
     * connection unless the server's certificate matches its host
     * name. In cases where the certificate still seems to be valid it
     * might make sense to disable this option and allow such
     * connections.
     */
    virtual bool  getSSLVerifyHost() const { return true; }

    /** True if transport-level compression is enabled */
    virtual bool  getCompression() const = 0;

    /** The number of seconds of waiting response timeout */
    virtual unsigned int getResponseTimeout() const = 0;

    /**
     * Specifies the name of the manufacturer of the device. This
     * property is optional.
     */
    virtual const char*  getMan() const = 0;

    /**
     * Specifies the model name or model number of the device. This
     * property is optional.
     */
    virtual const char*  getMod() const = 0;

    /**
     * Specifies the OEM (Original Equipment Manufacturer) of the
     * device. This property is optional.
     */
    virtual const char*  getOem() const = 0;

    /**
     * Specifies the firmware version of the device. This property is
     * optional.
     */
    virtual const char*  getFwv() const = 0;

    /**
     * Specifies the software version of the device.
     * This property is optional.
     */
    virtual const char*  getSwv() const = 0;

    /**
     * Specifies the hardware version of the device.
     * This property is optional.
     */
    virtual const char*  getHwv() const = 0;

    /**
     * Specifies the identifier of the source synchronization device.
     * The content information MUST specify an identifier which is unique
     * among all devices accessing the same data on a SyncML server.
     */
    virtual const char*  getDevID() const = 0;

    /**
     * Specifies the type of the source synchronization device. Type
     * values for this element type can be e.g. "pager", "handheld",
     * "pda", "phone", "smartphone", "server", "workstation". Other
     * values can also be specified. This property is mandatory.
     */
    virtual const char*  getDevType() const = 0;

    /** Specifies the implemented DS version. This property is optional. */
    virtual const char*  getDsV() const = 0;

    /**
     * Specifies that the device supports UTC based time.
     * If utc = true, the server SHOULD send time in UTC format,
     * else MUST send in local time.
     */
    virtual bool getUtc() const = 0;

    /**
     * Specifies that the device supports handling of large objects.
     */
    virtual bool getLoSupport() const = 0;

    /**
     * Specifies that the device supports number of changes.
     * Must be false.
     */
    virtual bool getNocSupport() const = 0;

    /**
     * Specifies the maximum object size allowed by the device.
     * Value 0 means "no maxObjSize set".
     */
    virtual unsigned int getMaxObjSize() const = 0;

    /**
     * This is a hash value generated from all properties that are used
     * for the <DevInf> element, plus the syncURL property. If the hash
     * changes between sync sessions the information has changed and must
     * be resent.
     */
    virtual const char*  getDevInfHash() const = 0;
    virtual void setDevInfHash(const char *hash) = 0;

    /**
     * Specifies if devinfo should be sent
     */
    virtual bool getSendDevInfo() const = 0;


    //
    // Following methods are used to store Server device capabilities (devInf) when received
    //

    /**
     * Specifies if Server devinfo should be asked even if not necessary.
     * Can be used by Clients to force asking Server caps.
     */
    virtual bool getForceServerDevInfo() const = 0;

    /// Specifies the Server software version.
    virtual const char* getServerSwv() const = 0;
    virtual void setServerSwv(const char* v) = 0;

    /**
     * Specifies the Server URL correspondant to the devInf obtained.
     * It's used to check if the Server URL changed from last time, in this
     * case the Server devInf are obsolete (will ask them again).
     */
    virtual const char* getServerLastSyncURL() const = 0;
    virtual void setServerLastSyncURL(const char* v) = 0;

    /// Specifies the name of the manufacturer of the Server.
    virtual void setServerMan(const char* v) = 0;

    /// Specifies the model name or model number of the Server.
    virtual void setServerMod(const char* v) = 0;

    /// Specifies the OEM (Original Equipment Manufacturer) of the Server.
    virtual void setServerOem(const char* v) = 0;

    /// Specifies the firmware version of the Server.
    virtual void setServerFwv(const char* v) = 0;

    /// Specifies the hardware version of the Server.
    virtual void setServerHwv(const char* v) = 0;

    /// Specifies the Server identifier.
    virtual void setServerDevID(const char* v) = 0;

    /// Specifies the Server type (expected "server")
    virtual void setServerDevType(const char* v) = 0;

    /// Specifies if the Server supports UTC based time.
    virtual void setServerUtc(const bool v) = 0;

    /// Specifies if the Server supports handling of large objects.
    virtual void setServerLoSupport(const bool v) = 0;

    /// Specifies if the Server supports number of changes.
    virtual void setServerNocSupport(const bool v) = 0;

    /// Specifies the DTD version of the Server (expected "1.2")
    virtual void setServerVerDTD(const char* v) = 0;

    /// Specifies if the Server supports the Smart Slow sync.
    virtual void setServerSmartSlowSync(const int  v) = 0;
    
    /// Specifies if the Server supports MEA.
    virtual void setServerMultipleEmailAccount(const int  v) = 0;

    /// Specifies the array of DataStores supported by the Server.
    virtual void setServerDataStores(const ArrayList* dataStores) = 0;

    /// Returns the array of DataStores supported by the Server.
    virtual const ArrayList* getServerDataStores() const = 0;
    
    /// Specifies if the Server supports upload of media files via HTTP.
    virtual void setServerMediaHttpUpload(const bool v) = 0;
   
    // set the source uri list comma separated for replace or not the item
    virtual void setServerNoFieldLevelReplace(const char* v) = 0;

    // get info about if the client has set to force to exit.
    virtual bool isToAbort() = 0;

    /// Returns the global error code of last sync
    virtual int getLastGlobalError() = 0;

    /// Sets the global last error code
    virtual void setLastGlobalError(const int val) = 0;

};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
