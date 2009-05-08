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

#include "base/fscapi.h"

#if FUN_TRANSPORT_AGENT == FUN_MOZ_TRANSPORT_AGENT

#include "http/MozillaTransportAgent.h"

#include "nsXPCOM.h"
#include "nsCOMPtr.h"
#include "nsIXMLHttpRequest.h"
#include "nsVariant.h"
#include "nsStringAPI.h"
#include "nsIComponentManager.h"

#include "base/util/utils.h"
#include "http/constants.h"
#include "event/FireEvent.h"

USE_NAMESPACE

/*
 * This is the Mozilla implementation of the TransportAgent object
 * It makes use of the Mozilla xpcom components for making http requests
 */

MozillaTransportAgent::MozillaTransportAgent() : TransportAgent() {}

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
MozillaTransportAgent::MozillaTransportAgent(URL& newURL, Proxy& newProxy, unsigned int maxResponseTimeout)
: TransportAgent(newURL, newProxy, maxResponseTimeout)
{ }

MozillaTransportAgent::~MozillaTransportAgent() 
{ }

/*
 * Sends the given SyncML message to the server specified
 * by the install property 'url'. Returns the server's response.
 * The response string has to be freed with delete [].
 * In case of an error, NULL is returned and lastErrorCode/Msg
 * is set.
 */
char* MozillaTransportAgent::sendMessage(const char* msg) 
{
    nsCOMPtr<nsIXMLHttpRequest> httpRequest;
    nsCOMPtr<nsVariant>         messageBody;

    nsCOMPtr<nsIComponentManager> compManager;
    NS_GetComponentManager(getter_AddRefs(compManager));

    // Instantiate a new XMLHttpRequest object
    if(!NS_SUCCEEDED(compManager->CreateInstanceByContractID(NS_XMLHTTPREQUEST_CONTRACTID,
                                                             0,
                                                             NS_GET_IID(nsIXMLHttpRequest), 
                                                             getter_AddRefs(httpRequest))))
    {
        // there is no http service
        LOG.error("MozillaTransportAgent::sendMessage error: XMLHttpRequest service not found.");
        setError(ERR_NETWORK_INIT, "Error: XMLHttpRequest service not found.");
        return 0;
    }
    if(!msg) 
    {
        LOG.error("MozillaTransportAgent::sendMessage error: NULL message.");
        setError(ERR_NETWORK_INIT, "Error: NULL message.");
        return 0;
    }
    if (!(url.host) || strlen(url.host) == 0) 
    {
        LOG.error("MozillaTransportAgent::sendMessage error: %s.", ERRMSG_HOST_NOT_FOUND);
        setErrorF(ERR_HOST_NOT_FOUND, "Error: %s.", ERRMSG_HOST_NOT_FOUND);
        return 0;
    }

    nsCString    contentType("application/vnd.syncml+xml");
    nsString     responseText;
    char*        retResponse = 0;
    unsigned int statusCode;
    nsCString    statusText;
    int          readyState;
    unsigned int contentLength = 0;

    //
    // Preparing request
    //
    LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);
    LOG.debug("SetRequestHeader Content-Type: %s", contentType.get());
    LOG.debug("SetRequestHeader User-Agent: %s", userAgent);

    LOG.debug("Sending HTTP Request: %s", msg);

    // open a new http request
    if(!NS_SUCCEEDED(httpRequest->OpenRequest(nsCString("POST"), 
                             nsCString(url.fullURL),
                             PR_FALSE, 
                             NS_LITERAL_STRING(""), 
                             NS_LITERAL_STRING(""))))
    {
        LOG.error("MozillaTransportAgent::sendMessage error: %s.", ERRMSG_NETWORK_INIT);
        setErrorF(ERR_NETWORK_INIT, "Error: %s.", ERRMSG_NETWORK_INIT);
        return 0;
    }

    // set request headers
    httpRequest->SetRequestHeader(nsCString("Content-Type"), contentType);
    httpRequest->SetRequestHeader(nsCString("User-Agent"), nsCString(userAgent));

    // Instantiate a new message body as nsVariant obj
    compManager->CreateInstanceByContractID(NS_VARIANT_CONTRACTID,
                                            0,
                                            NS_GET_IID(nsVariant), 
                                            getter_AddRefs(messageBody));
    messageBody->SetAsString(msg);
    contentLength = strlen(msg);

    //
    // Sending request
    //
    
    fireTransportEvent(contentLength, SEND_DATA_BEGIN);
    
    if(!NS_SUCCEEDED(httpRequest->Send(messageBody)))
    {
        // cannot send http request
        LOG.error("MozillaTransportAgent::sendMessage error: Cannot send http request");
        setErrorF(ERR_CONNECT, "Error: Cannot send http request.");
        return 0;
    }
    
    fireTransportEvent(contentLength, SEND_DATA_END);

    httpRequest->GetReadyState(&readyState);
    if(readyState != 4) // 4 - readyState COMPLETED
    {
        // http request not completed
        LOG.error("MozillaTransportAgent::sendMessage error: Http request not completed");
        setErrorF(ERR_CONNECT, "Error: %s.", ERRMSG_CONNECT);
        return 0;
    }

    //
    // Reading response
    //

    // get status code and text
    httpRequest->GetStatus(&statusCode);
    httpRequest->GetStatusText(statusText);

    LOG.debug("Status Code: %d", statusCode);
    LOG.debug("Status Text: %s", statusText.get());

    switch (statusCode) {

        case 200: {
            LOG.debug("Data sent succesfully to server. Server responds OK");
            
            httpRequest->GetResponseText(responseText);
            retResponse = stringdup(NS_ConvertUTF16toUTF8(responseText).get());

            contentLength = strlen(retResponse);
            fireTransportEvent(contentLength, RECEIVE_DATA_END);

            break;
        }
        case 400: {                    
            setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
            LOG.error("%s", getLastErrorMsg());
            
            break;
        }
        case 404: {         
            setErrorF(ERR_HTTP_NOT_FOUND, "HTTP request error: resource not found (status %d).", statusCode);
            LOG.error("%s", getLastErrorMsg());
            
            break;
        }
        case 408: {   
            setErrorF(ERR_HTTP_REQUEST_TIMEOUT, "HTTP request error: server timed out waiting for request (status %d).", statusCode);
            LOG.error("%s", getLastErrorMsg());

            break;
        }
        case 500: {     
            setErrorF(ERR_SERVER_ERROR, "HTTP server error: %d. Server failure.", statusCode);
            LOG.error("%s", getLastErrorMsg());

            break;
        }
        default: {
            setErrorF(statusCode, "HTTP request error: status received = %d.", statusCode);
            LOG.error("%s", getLastErrorMsg());
        }
    }

    LOG.debug("Http Response received: %s", retResponse);
   
    return retResponse;
}

#endif
