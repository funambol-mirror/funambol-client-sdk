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

#ifndef INCL_TRANSPORT_AGENT_REPLACEMENT
#define INCL_TRANSPORT_AGENT_REPLACEMENT


#include "http/constants.h"
#include "http/HTTPHeader.h"
#include "http/TransportAgent.h"
#include "http/TransportAgentFactory.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/**
 * This is a replacement of the TransportAgent Class (platform dependent) that can be
 * useful to check and modify every syncML message.
 *
 * Usage: extend this class and reimplement methods
 *   - beforeSendingMessage()
 *   - afterReceivingResponse()
 * These methods are called just before and after sending the data via a real
 * TransportAgent object, which is internally created using the TransportAgentFactory.
 * Inside these methods the client can check the syncML messages and also modify them.
 */
class TransportAgentReplacement : public TransportAgent {

protected:

    /**
     * Can be reimplemented by derived classes, to execute actions before sending a SyncML message.
     * @param msgToSend [IN-OUT] the syncML message formatted by SyncManager, to send to the Server
     */
    virtual void beforeSendingMessage(StringBuffer& msgToSend) = 0;

    /**
     * Can be reimplemented by derived classes, to execute actions after receiving a SyncML message.
     * @param msgReceived [IN-OUT] the syncML message received from Server, to be returned to SyncManager
     */
    virtual void afterReceivingResponse(StringBuffer& msgReceived) = 0;


public:

    /**
     * Creates a new real TransportAgent
     */
    TransportAgentReplacement();

    /**
     * Creates a new real TransportAgent, passing all params
     */
    TransportAgentReplacement(URL& url, 
                              Proxy& proxy, 
                              unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT,
                              unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE);

    /**
     * Deletes the internally owned realTransportAgent
     */
    virtual ~TransportAgentReplacement();


    /**
     * Sends the message via the realTransportAgent->sendMessage().
     * Calls beforeSendingMessage() before sending data and afterReceivingResponse()
     * when msg is returned by the Server.
     * @return new allocated buffer with the response from the Server
     */
    virtual char* sendMessage(const char* msg);


    // Forward SET calls to the real transportAgent
    virtual void setURL           (URL& newURL)             { realTransportAgent->setURL(newURL);       }
    virtual void setTimeout       (unsigned int t)          { realTransportAgent->setTimeout(t);        }
    virtual void setMaxMsgSize    (unsigned int t)          { realTransportAgent->setMaxMsgSize(t);     }
    virtual void setReadBufferSize(unsigned int t)          { realTransportAgent->setReadBufferSize(t); }
    virtual void setUserAgent(const char*  ua)              { realTransportAgent->setUserAgent(ua);     }
    virtual void setCompression(bool newCompression)        { realTransportAgent->setCompression(newCompression);  }
    virtual void setSSLServerCertificates(const char *value){ realTransportAgent->setSSLServerCertificates(value); }
    virtual void setSSLVerifyServer(bool value)             { realTransportAgent->setSSLVerifyServer(value); }
    virtual void setSSLVerifyHost(bool value)               { realTransportAgent->setSSLVerifyHost(value);   }


private:

    /// This is the real transportAgent for this platform, created new in the cosntructor.
    TransportAgent* realTransportAgent;

};

END_NAMESPACE

#endif

