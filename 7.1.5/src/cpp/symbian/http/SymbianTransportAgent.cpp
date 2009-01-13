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

#include <e32base.h>  // for ActiveSchedulerWait
#include <e32std.h>   // for Mem::Copy()

#include <http.h>
#include <httperr.h>

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/stringUtils.h"

#include "spds/constants.h"  // for some error status code?

#include "http/constants.h"
#include "http/errors.h"
#include "http/SymbianTransportAgent.h"

#include "base/FConnection.h"  // for connection management


USE_NAMESPACE

/**
 * CSymbianTransportAgent::NewL()
 * Creates instance of CSymbianTransportAgent.
 */
CSymbianTransportAgent* CSymbianTransportAgent::NewL(URL& aUrl,Proxy& aProxy,
                                                     unsigned int aResponseTimeout,
                                                     unsigned int aMaxMsgSize)
{
    CSymbianTransportAgent* self;
    self = CSymbianTransportAgent::NewLC(aUrl,aProxy,aResponseTimeout,aMaxMsgSize);
    CleanupStack::Pop( self );
    return self;
}

/**
 * CSymbianTransportAgent::NewLC()
 * Creates instance of CSymbianTransportAgent.
 */
CSymbianTransportAgent* CSymbianTransportAgent::NewLC(URL& aUrl,Proxy& aProxy,
                                                      unsigned int aResponseTimeout,
                                                      unsigned int /* aMaxMsgSize */)
{
    CSymbianTransportAgent* self;
    
    self = new (ELeave) CSymbianTransportAgent(aUrl, aProxy, aResponseTimeout);
    CleanupStack::PushL( self );
    self->ConstructL(aUrl);
    return self;
}

/**
 * CSymbianTransportAgent::CSymbianTransportAgent()
 * First phase constructor.
 */
CSymbianTransportAgent::CSymbianTransportAgent(URL& aUrl, Proxy& aProxy,
                                               unsigned int aResponseTimeout) : 
                                TransportAgent(aUrl, aProxy, aResponseTimeout),
                                iIap(0),
                                iResponseBody(NULL)
{
}

/**
 * CSymbianTransportAgent::~CSymbianTransportAgent()
 * Destructor.
 */
CSymbianTransportAgent::~CSymbianTransportAgent()
{
    //any remaining transactions that weren't complete are immediately cancelled.
    iHttpSession.Close();
    delete iPostBody;
    iPostBody = NULL;
    delete iResponseBody;
    iResponseBody = NULL;
    delete iASWait;
    iASWait = NULL;
}

/**
 * CSymbianTransportAgent::ConstructL()
 * Second phase construction.
 */
void CSymbianTransportAgent::ConstructL(URL& /* aUrl */)
{
   
    // Open RHTTPSession with default protocol ("HTTP/TCP")
    // Note that RHTTPSession (and consequently the whole 
    // of HTTP) depends on the active scheduler; a scheduler 
    // must be installed when the session is opened and it 
    // must be running if a transaction is actually to do anything. 
    
    // Get the connection manager instance
    FConnection* connection = FConnection::getInstance();
    if (!connection) {
        LOG.error("TransportAgent: no active connection; exiting");
        setError(ERR_HTTP, "No active connection");
        return;
    }
    
    // Session is owned by FConnection!
    RSocketServ* socketServ = connection->getSession();
    RConnection* rConnection = connection->getConnection();
    // reuse active connection, please see:
    // http://wiki.forum.nokia.com/index.php/CS000825_-_Using_an_already_active_connection
    TInt err;
    TRAP( err, iHttpSession.OpenL() );
    User::LeaveIfError( err );

    // Set the session's connection info...
    RStringPool strPool = iHttpSession.StringPool();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    // ...to use the socket server
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable() ),
                            THTTPHdrVal (socketServ->Handle()) );
    // ...to use the connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable() ),
                            THTTPHdrVal (REINTERPRET_CAST(TInt, rConnection)) );
     
    // Create the nested active scheduler
    // please see » Symbian OS v9.1 » Symbian OS reference » C++ component
    // reference » Base E32 » CActiveSchedulerWait
    iASWait = new (ELeave) CActiveSchedulerWait();
}

/**
 * CSymbianTransportAgent::MHFRunL()
 * Inherited from MHTTPTransactionCallback
 * Called by framework to pass transaction events.
 */
void CSymbianTransportAgent::MHFRunL( RHTTPTransaction aTransaction, 
                                const THTTPEvent& aEvent )
{

    switch ( aEvent.iStatus ) 
    {
        case THTTPEvent::EGotResponseHeaders:
        {
            // HTTP response headers have been received. Use
            // aTransaction.Response() to get the response. However, it's not
            // necessary to do anything with the response when this event occurs.
            // Get HTTP status code from header (e.g. 200)
            RHTTPResponse resp = aTransaction.Response();
            TInt status = resp.StatusCode();

            // Get status text (e.g. "OK")
            // TBuf<32> statusText;
            // statusText.Copy( resp.StatusText().DesC() );
        
            // Please see
            // Symbian OS v9.1 << Symbian OS reference >> C++ component reference
            // << Application protocols HTTP >> HTTPStatus
            // for other status methods and constants
            HTTPStatus httpStatus;
            if (httpStatus.IsClientError(status)) {
                // errors in range 4xx
                SetHttpClientError(status);
            } else if (httpStatus.IsServerError(status)) {
                // errors in range 5xx
                SetHttpServerError(status);
            }
        }
        break;

        case THTTPEvent::EGotResponseBodyData:
        {
            // Part (or all) of response's body data received. Use 
            // aTransaction.Response().Body()->GetNextDataPart() to get the actual
            // body data.
            // Get body data
            MHTTPDataSupplier* dataSupplier = aTransaction.Response().Body();
            TPtrC8 ptr;
            dataSupplier->GetNextDataPart(ptr);

            // Append to buffer
            if(!iResponseBody)
            {
                iResponseBody = HBufC8::NewL(ptr.Length());
                iResponseBody->Des().Copy(ptr);
            }
            else
            {
                iResponseBody = iResponseBody->ReAllocL(iResponseBody->Length()+ptr.Length());
                iResponseBody->Des().Append(ptr);
            }
            
            // release the body data
            dataSupplier->ReleaseData();

        }
        break;

        case THTTPEvent::EResponseComplete:
        {
            // Indicates that header & body of response is completely received.
            // No further action here needed.
        }
        break;

        case THTTPEvent::ESucceeded:
        {
            // Indicates that transaction succeeded. 
            // Transaction can be closed now. It's not needed anymore.
            iTransFailed = EFalse;
            aTransaction.Close();
            iASWait->AsyncStop();
        }
        break;

        case THTTPEvent::EFailed:
        {
            iTransFailed = ETrue;
            aTransaction.Close();
            iASWait->AsyncStop();
        }
        break;

        default:
            // There are more events in THTTPEvent, but they are not usually 
            // needed. However, event status smaller than zero should be handled 
            // correctly since it's error.
        {
            if ( aEvent.iStatus < 0 )
            {
                // Just close the transaction on errors
                iTransFailed = ETrue;
                aTransaction.Close();
                iASWait->AsyncStop();
            } else {
                // Other events are not errors (e.g. permanent and temporary redirections)
            }
        }
        break;
    }
}

/**
 * CSymbianTransportAgent::MHFRunError()
 * Inherited from MHTTPTransactionCallback
 * Called by framework when *leave* occurs in handling of transaction event.
 * These errors must be handled, or otherwise HTTP-CORE 6 panic is thrown.
 */
TInt CSymbianTransportAgent::MHFRunError( TInt aError, 
                                    RHTTPTransaction /*aTransaction*/, 
                                    const THTTPEvent& /*aEvent*/)
{
    // Handle error and return KErrNone.
    /*
    TRAPD( err, HandleRunErrorL( aError ) );
    if( err )
        Panic( EClientEngine );
    return KErrNone;
    */
    // Simply return the error
    return aError;
}

/**
 * CSymbianTransportAgent::ConnectL()
 */
void CSymbianTransportAgent::ConnectL()
{
// empty at this time
// to be defined if really needed    
}

/**
 * CSymbianTransportAgent::sendMessage()
 */
char* CSymbianTransportAgent::sendMessage(const char* msg)
{
    // Just to be sure the response buffer is ok
    //TO BE VERIFIED!!!!
    delete iResponseBody;
    iResponseBody = NULL; 
    
    // Set the POST body
    delete iPostBody;
    iPostBody = NULL;

    if (url.fullURL == NULL) {
        setErrorF(ERR_CONNECT, "Empty URL"); 
        LOG.error("Transport Agent: empty URL");
        return NULL;
    }

    char* response = NULL;
    RStringF method;
    RHTTPHeaders hdr;
    // Parse string to URI (as defined in RFC2396)
    TUriParser8 uri;
    HBufC8* fullUrl = charToNewBuf8(url.fullURL); 
    TInt parseErr = uri.Parse(*fullUrl);
    if (parseErr != KErrNone) {
        setErrorF(ERR_CONNECT, "Malformed URL"); 
        LOG.error("Transport Agent: malformed url");
        goto finally;
    }

    {
        TPtrC8 ptr(reinterpret_cast<const TUint8*>(msg));
        iPostBody = HBufC8::NewL(ptr.Length());
        iPostBody->Des().Copy(ptr);
    }
    
    FConnection* connection = FConnection::getInstance();
    if (connection->isConnected() == false) {
        int res = connection->startConnection();
        if (res) {
            setErrorF(ERR_CONNECT, "Connection error (%d): please check your internet settings.", res); 
            LOG.error("%s", getLastErrorMsg());
            goto finally;
        }
    }

    // Get request method string for HTTP POST
    method = iHttpSession.StringPool().StringF( HTTP::EPOST, RHTTPSession::GetTable());

    // Open transaction with previous method and parsed uri. This class will 
    // receive transaction events in MHFRunL and MHFRunError.
    iHttpTransaction = iHttpSession.OpenTransactionL( uri, *this, method );

    // Set headers for request; user agent, accepted content type and body's 
    // content type.
    hdr = iHttpTransaction.Request().GetHeaderCollection();
    SetHeaderL(hdr, HTTP::EUserAgent, _L8("Funambol Symbian SyncML client"));
    SetHeaderL(hdr, HTTP::EAccept, _L8("*/*"));
    SetHeaderL(hdr, HTTP::EContentType, _L8("application/vnd.syncml+xml"));

    LOG.debug("Sending message:");
    LOG.debug("%s", msg);

    // Set data supplier for POST body
    // Please see  » Symbian OS v9.1 » Symbian OS guide » Using HTTP Client »
    // Handling request body data for explanation on this part
    iDataSupplier = this;
    iHttpTransaction.Request().SetBody(*iDataSupplier);     

    // Submit the transaction. After this the framework will give transaction
    // events via MHFRunL and MHFRunError.
    iHttpTransaction.SubmitL();

    // Start the scheduler, once the transaction completes or is cancelled on an
    // error the scheduler will be stopped in the event handler
    // This is a trick to implement a synchronous method
    iASWait->Start();  // TO BE VERIFIED!!!!
    
    // when all is finished, return char* to be delete []
    if(!iTransFailed)
    {
        TInt length = iResponseBody->Des().Length();
        response = new char[length+1];
        Mem::Copy(response, iResponseBody->Ptr(), length);
        response[length] = '\0';

        LOG.debug("Message received:");
        LOG.debug("%s", response);
    }
    else
    {
        setErrorF(ERR_HTTP,
                  "HTTP request error: request timed out waiting for response");
        LOG.debug("HTTP response not received");
    }

finally:

    delete fullUrl;
    return response;
}

/**
 * CSymbianTransportAgent::SetHeaderL()
 * Sets a field-value pair to an HTTP header.
 */
void CSymbianTransportAgent::SetHeaderL(RHTTPHeaders aHeaders, 
                                        TInt aHdrField, 
                                        const TDesC8& aHdrValue)
{
    RStringF valStr = iHttpSession.StringPool().OpenFStringL(aHdrValue);
    CleanupClosePushL(valStr);
    THTTPHdrVal val(valStr);
    aHeaders.SetFieldL(iHttpSession.StringPool().StringF(aHdrField,
                                                         RHTTPSession::GetTable()),
                                                         val);
    CleanupStack::PopAndDestroy(&valStr);
}


// Please see  » Symbian OS v9.1 » Symbian OS guide » Using HTTP Client »
// Handling request body data for explanation on this part
TBool CSymbianTransportAgent::GetNextDataPart(TPtrC8& aDataChunk)
{
    aDataChunk.Set(iPostBody->Des());
    return ETrue;
}

void CSymbianTransportAgent::ReleaseData()
{
    // Clear out the submit buffer
    TPtr8 buff = iPostBody->Des();
    buff.Zero();
    // since we assumed a single chunk was enough
    // we don't need to notify HTTP of more data available
}

TInt CSymbianTransportAgent::OverallDataSize()
{
    return iPostBody->Des().Length();
}

TInt CSymbianTransportAgent::Reset()
{
    // reset not supported at this time
    return KErrNotSupported;
}


/**
 * CSymbianTransportAgent::SetIap()
 * Sets the IAP to be used in the HTTP connections.
 */
void CSymbianTransportAgent::SetIap(const TUint32& aIap)
{
    iIap = aIap;
}

/**
 * SetHttpClientError()
 * Set the http client error (5xx) in Funambol client-api.
 */
void CSymbianTransportAgent::SetHttpClientError(TInt aStatus)
{
    switch (aStatus)
    {
        case 401:
        {
            setErrorF(ERR_CREDENTIAL, "HTTP server error: %d. Wrong credential.", aStatus);
        }
        break;
        case 402:
        {
            setErrorF(PAYMENT_REQUIRED,
                      "HTTP server error: %d. Client not authenticated.",
                      aStatus);
        }
        break;
        case 403:
        {
            setErrorF(FORBIDDEN,
                      "HTTP server error: %d. Connection forbidden, client not activated.",
                      aStatus);
        }
        break;
        case 404:
        {
            //setErrorF(ERR_HTTP_NOT_FOUND,
            //          "HTTP request error: resource not found (status %d)",
            //          aStatus);
        }
        break;
        case 408:
        {
            //setErrorF(ERR_HTTP_REQUEST_TIMEOUT,
            //          "HTTP request error: server timed out waiting for request (status %d)",
            //          aStatus);
        }
        break;
        case 420:
        {
            setErrorF(ERR_CLIENT_NOT_NOTIFIABLE,
                      "HTTP server error: %d. Client not notifiable.",
                      aStatus);
        }
        break;
        case 421:
        {
            setErrorF(ERR_CTP_ALLOWED,
                      "HTTP server error: %d. Client not notifiable and CTP Server is available.",
                      aStatus);
        }
        break;
    
        default:
        {
        }
        break;
    }
}

/**
 * SetHttpServerError()
 * Set the http server error (5xx) in Funambol client-api.
 */
void CSymbianTransportAgent::SetHttpServerError(TInt aStatus)
{
    switch (aStatus)
    {
        case 500:
        {
            setErrorF(ERR_SERVER_ERROR,
                      "HTTP server error: %d. Server failure.",
                      aStatus);
        }
        break;
        default:
        {
            
        }
        break;
    }
}
