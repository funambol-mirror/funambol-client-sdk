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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#ifndef INCL_CTP_CONFIG
#define INCL_CTP_CONFIG
/** @cond DEV */

#include "base/fscapi.h"
#include "client/DMTClientConfig.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/// This is the root for the configuration
#define APPLICATION_URI                 "Funambol"

/// This is the context for the CTP push parameters
#define CONTEXT_PUSH_CTP                "/push/ctp"    


class CTPConfig : public DMTClientConfig {

private:
    
    /**
    * This key enables the push feature. It is a bitmask, 
    * with bit 0 enabling the STP and bit 1 enabling the CTP.
    * 0: None
    * 1: STP
    * 2: CTP
    * 3: STP with fallback CTP
    */
    int32_t push;

    /**
    * This key enables the polling feature if the value is 
    * greater than 0, otherwise the polling is disabled.
    */
    int32_t polling;
    
    /**
    * Enable or disable the feature of storing the pending 
    * push requests during a sync.
    */
    bool queuePush;

    /**
    * The time interval, in seconds, after which the 
    * client will restore the connection.
    */
    int32_t ctpRetry;

    /**
    * The time interval, in seconds, for the maximum 
    * retry timeout (see failure handling).
    */
    int32_t maxCtpRetry;
    
    /**
    * The time, in seconds, for which the client 
    * shall wait for the server response, after sending a command.        
    */
    int32_t ctpCmdTimeout;
    
    /**
    * The time, in seconds, for which the connection with 
    * the server shall be kept open once it has been established. 
    * A value of zero indicates that the connection must be 
    * kept open forever.
    */
    int32_t ctpConnTimeout;

    /**
    * TCP/IP port used by the server to listen for CTP requests. 
    */
    int32_t ctpPort;

    ///
    /// the nonce used for the MD5 auth. The nonce is sent by the server
    /// in b64 encoding. It's the same nonce used for synchronization, so
    /// the value is linked directly to 'accessConfig::serverNonce'.
    //char* ctpNonce;

    /**
    * The interval of the heartbeat
    */
    int32_t ctpReady;

    /**
    * The url to contact. From DM
    */
    StringBuffer urlTo;
    
    /**
    * The url populated after a JUMP server command.
    * it is in format <(ipAddress|address):port>
    */
    StringBuffer urlFrom;

    /**
    * Timeout (seconds) for the address-change notification sync.
    * CTP will wait until this sync ends, after this timeout the
    * CTP is started.
    */
    int32_t notifyTimeout;

    StringBuffer checkPrefix(char* url);

public:        
    
    /**
    * Constructor
    */
    CTPConfig(const char* application_uri);
    
    /**
    * Destructor
    */
    ~CTPConfig();

    /**
    * Read the parameter configuration from the registry
    */
    void readCTPConfig();
    
    /**
    * Save the parameter configuration to the registry.
    * Actually the only needed parameter to be saved is the
    * nonce sent by the server
    */
    void saveCTPConfig();
    
    /**
    * set/get method for push parameter
    */
    void setPush(int32_t v) { push = v; }
    int getPush()           { return push; }

    /**
    * set/get method for polling parameter
    */
    int32_t getPolling()       { return polling; }
    void setPolling(int32_t v) { polling = v; }
    
    /**
    * set/get method for queuePush parameter
    */
    bool getQueuePush()       { return queuePush; }
    void setQueuePush(bool v) { queuePush = v; }
    
    /**
    * set/get method for ctpRetry parameter
    */
    int32_t getCtpRetry()       { return ctpRetry; }
    void setCtpRetry(int32_t v) { ctpRetry = v; }
    
    /**
    * set/get method for maxCtpRetry parameter
    */
    int32_t getMaxCtpRetry()       { return maxCtpRetry; }
    void setMaxCtpRetry(int32_t v) { maxCtpRetry = v; }
    
    /**
    * set/get method for ctpCmdTimeout parameter
    */
    int32_t getCtpCmdTimeout()       { return ctpCmdTimeout; }
    void setCtpCmdTimeout(int32_t v) { ctpCmdTimeout = v; }
    
    /**
    * set/get method for ctpConnTimeout parameter
    */
    int32_t getCtpConnTimeout()       { return ctpConnTimeout; }
    void setCtpConnTimeout(int32_t v) { ctpConnTimeout = v; }
    
    /**
    * set/get method for ctpPort parameter
    */
    int32_t getCtpPort()       { return ctpPort; }
    void setCtpPort(int32_t v) { ctpPort = v; }
    
    /**
    * set/get method for ctpReady parameter
    */
    int32_t getCtpReady()       { return ctpReady; }
    void setCtpReady(int32_t v) { ctpReady = v; }
    

    /**
    * set/get method for nonce parameter: it's the 'accessConfig::clientNonce' so
    * we ridirect to that property.
    */
    const char* getCtpNonce()       { return accessConfig.getClientNonce(); }
    void setCtpNonce(const char* v) { accessConfig.setClientNonce(v); }

    /**
    * set/get method for urlTo parameter
    */
    StringBuffer& getUrlTo()      { return urlTo; }
    void setUrlTo(StringBuffer v) { urlTo = v; }

    /**
    * set/get method for urlFrom parameter
    */
    StringBuffer& getUrlFrom()      { return urlFrom; }
    void setUrlFrom(StringBuffer v) { urlFrom = v; }

    /**
    * set/get method for notifyTimeout parameter
    */
    int getNotifyTimeout()       { return notifyTimeout; }
    void setNotifyTimeout(int v) { notifyTimeout = v; }


    /**
    * get the host name form the url
    */
    StringBuffer getHostName(StringBuffer syncUrl);
    
    /**
    * get the host port form the url. If the port doesn't exist
    * it return 0
    */
    int getHostPort(StringBuffer syncUrl);
    
private:
    char* decodePassword(const char* password);
    StringBuffer encodePassword(const char* password);
};



END_NAMESPACE

/** @endcond */
#endif
