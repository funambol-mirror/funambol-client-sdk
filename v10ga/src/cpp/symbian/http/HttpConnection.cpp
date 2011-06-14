/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2010 Funambol, Inc.
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

#include "base/util/stringUtils.h"
#include "http/errors.h"
#include "http/HttpConnection.h"
#include "http/HttpDataSupplier.h"
#include "base/util/symbianUtils.h"
#include "ioStream/BufferInputStream.h"
#include "base/FConnection.h"  // for connection management


BEGIN_FUNAMBOL_NAMESPACE



HttpConnection::HttpConnection(const char* ua) : AbstractHttpConnection(ua)
{
    iTransFailed    = EFalse;
    iASWait         = NULL;
    iDataSupplier   = NULL;
    iLastHTTPStatus = 0;
    iOutputStream   = NULL;
}

HttpConnection::~HttpConnection()
{
    close();
}


int HttpConnection::open(const URL& url, RequestMethod method) 
{
    TRAPD(err, OpenL();)
    if (err) {
        LOG.error("Error (%d) opening HTTP session", err);
        return err;
    } 
    
    this->url = url;
    this->method = method;
    
    // Used to read the inputStream and send request body chunk by chunk.
    iDataSupplier = new HttpDataSupplier(this);
    
    return 0;
}


int HttpConnection::close()
{
    //any remaining transactions that weren't complete are immediately cancelled.
    LOG.debug("HttpConnection - closing HTTP session");
    iHttpSession.Close();
    
    delete iASWait;        
    iASWait = NULL;
    
    if (iDataSupplier) {
        delete iDataSupplier;
        iDataSupplier = NULL;
    }
    return 0;
}



int HttpConnection::request(InputStream& data, OutputStream& response)
{
    //LOG.debug("entering HttpConnection::request"); 
    HTTP::TStrings httpVerb = HTTP::EPOST;
    RStringF requestMethod;
    RHTTPHeaders hdr;
    iLastHTTPStatus = 0;
    
    // Check if user aborted current sync.
    if (checkToAbort()) {
        LOG.debug("Leaving...");
        User::Leave(KErrCancel);    // leave is trapped at SyncActiveObject::RunL().
    }
    
    if (url.fullURL == NULL) {
        LOG.error("HttpConnection::request error: empty URL");
        return -1;
    }
    
    // Parse URL string to symbian URI object
    TUriParser8 uri;
    HBufC8* fullUrl = charToNewBuf8(url.fullURL);
    TInt parseErr = uri.Parse(*fullUrl);        // note: fullURL is not copied, must exist till the transaction is closed!
    if (parseErr != KErrNone) {
        LOG.error("HttpConnection::request error: malformed URL: %s", url.fullURL);
        iLastHTTPStatus = -1;
        goto finally;
    }
    
    //
    // Link the input / output streams:
    //  - input: the request data (sent chunk by chunk via callback methods in HTTPDataSupplier)
    //  - output: the returned data (appended chunk by chunk via AO' MHFRunL)
    //
    iDataSupplier->setInputStream(data);
    iDataSupplier->setMaxRequestChunkSize(requestChunkSize);
    iOutputStream = &response;
    
    
    // Start a new connection (if not already connected)
    FConnection* connection = FConnection::getInstance();
    if (connection->isConnected() == false) {
        int res = connection->startConnection();
        if (res) {
            LOG.error("Connection error (%d): please check your internet settings.", res);
            iLastHTTPStatus = -1;
            goto finally;
        }
    }
    
    // Set the request method
    switch (method) {
        case MethodGet:   httpVerb = HTTP::EGET;    break;
        case MethodPost:  httpVerb = HTTP::EPOST;   break; 
        case MethodPut:   httpVerb = HTTP::EPUT;    break;
        case MethodHead:  httpVerb = HTTP::EHEAD;   break; 
    }
    requestMethod = iHttpSession.StringPool().StringF(httpVerb, RHTTPSession::GetTable());
    
    
    // Open transaction with previous method and parsed uri. This class will 
    // receive transaction events in MHFRunL and MHFRunError.
    LOG.debug("opening HTTP transaction (uri = %s, method = %s)", buf8ToStringBuffer(uri.UriDes()).c_str(), 
                                                                  buf8ToStringBuffer(requestMethod.DesC()).c_str());
    iHttpTransaction = iHttpSession.OpenTransactionL(uri, *this, requestMethod);
    
    // Set ALL the headers for the request.
    setRequestHeaders();
    
    if (httpVerb == HTTP::EPOST) {
        // POST: Link the data supplier to be invoked when the body to send is needed
        iHttpTransaction.Request().SetBody(*iDataSupplier);
    } 
    else {
        // GET, HEAD: no body is sent
        iHttpTransaction.Request().RemoveBody();
    }

    // -------------------------------------
    // Submit the transaction. After this the framework will give transaction
    // events via MHFRunL and MHFRunError.
    iHttpTransaction.SubmitL();

    // Start the scheduler, once the transaction completes or is cancelled on an
    // error the scheduler will be stopped in the event handler
    // This is a trick to implement a synchronous method
    iASWait->Start();
    // -------------------------------------
    
    
    // Check if user aborted current sync 
    if (checkToAbort()) {
        LOG.debug("Leaving...");
        User::Leave(KErrCancel);    // leave is trapped at SyncActiveObject::RunL().
    }
    else if (iTransFailed) {
        LOG.error("HTTP transaction failed (http status = %d)", iLastHTTPStatus);
        goto finally;
    }
    
    // If here, the request completed successfully. 
    LOG.debug("HTTP transaction done, %d bytes returned", response.size());
    //printHeaders(responseHeaders);
    
finally:

    // Transaction can be closed now. It's not needed anymore.
    LOG.debug("closing HTTP transaction");
    iHttpTransaction.Close();
    
    delete fullUrl;     // must be deleted at the end!
    
    //LOG.debug("exiting HttpConnection::request");
    return iLastHTTPStatus;
}

int HttpConnection::request(const char* data, OutputStream& response)
{
    int requestStatus = 0;
    BufferInputStream bufferStream(data);

    requestStatus = request(bufferStream, response);
    
    return requestStatus;
}

void HttpConnection::OpenL()
{
    // Opens the session using the default protocol HTTP/TCP. 
    // This function leaves with an apropriate code if the open failed.
    // note: a scheduler must be installed when the session is opened
    iHttpSession.OpenL();
    
    // try to use current active connection
    reuseActiveConnection();
     
    // Used to wait on the HTTP transaction to complete.
    iASWait = new (ELeave) CActiveSchedulerWait();
}


void HttpConnection::reuseActiveConnection() 
{
    // Get the connection manager instance
    // Session is owned by FConnection!
    FConnection* connection = FConnection::getInstance();
    if (!connection) {
        LOG.error("HttpConnection: no active connection; exiting");
        setError(ERR_HTTP, "No active connection");
        return;
    }
    
    RSocketServ* socketServ = connection->getSession();
    RConnection* rConnection = connection->getConnection();
    
    // Set the session's connection info...
    RStringPool strPool = iHttpSession.StringPool();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    // ...to use the socket server
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketServ,RHTTPSession::GetTable() ),
                            THTTPHdrVal (socketServ->Handle()) );
    // ...to use the connection
    connInfo.SetPropertyL ( strPool.StringF(HTTP::EHttpSocketConnection,RHTTPSession::GetTable() ),
                            THTTPHdrVal (REINTERPRET_CAST(TInt, rConnection)) );
}


void HttpConnection::notifyForNewBodyData()
{
    TRAPD(err, iHttpTransaction.NotifyNewRequestBodyPartL());
    if (err != KErrNone) {
        LOG.error("HTTP error: cannot notify for more data to send");
        iLastHTTPStatus = err;
    }
}


void HttpConnection::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
{
    //LOG.debug("entering HttpConnection::MHFRunL: iStatus = %d", aEvent.iStatus);
    switch ( aEvent.iStatus ) 
    {
        case THTTPEvent::EGotResponseHeaders:
        {
            //LOG.debug(" -> EGotResponseHeaders");
            // HTTP response has been received.
            if (checkToAbort()) {
                cancelRequest();
                break;
            }
            
            // Get the response status.
            RHTTPResponse resp = aTransaction.Response();
            iLastHTTPStatus = resp.StatusCode();
            StringBuffer statusText = buf8ToStringBuffer(resp.StatusText().DesC());
            LOG.debug("Returned HTTP status %d: \"%s\"", iLastHTTPStatus, statusText.c_str());
            
            // Read response headers
            readResponseHeaders();
            break;
        }
    
        case THTTPEvent::EGotResponseBodyData:
        {
            //LOG.debug(" -> EGotResponseBodyData: getting the response");
            // Part (or all) of response's body data received.
            // Get the response data, chunk by chunk.
            if (checkToAbort()) {
                cancelRequest();
                break;
            }
            
            MHTTPDataSupplier* dataSupplier = aTransaction.Response().Body();
            TPtrC8 ptr;
            dataSupplier->GetNextDataPart(ptr);   // returns Etrue if it's the last chunk received

            // Append to the output buffer
            TInt length = ptr.Length();
            if (length>0 && iOutputStream) {
                iOutputStream->write((const char*)ptr.Ptr(), length);
                LOG.debug("Message received (%d bytes)", iOutputStream->size());
            } 
            else {
                LOG.debug("HTTP response from server is of zero length");
            }            

            // Must always release the response body data.
            dataSupplier->ReleaseData();
            
            // If this is not the last part (lastChunk = false) the EGotResponseBodyData will be
            // recalled automatically.
            break;
        }

        case THTTPEvent::EResponseComplete:
        {
            //LOG.debug(" -> EResponseComplete");
            // Indicates that header & body of response is completely received.
            // No further action here needed.
            break;
        }

        case THTTPEvent::ESucceeded:
        {
            //LOG.debug(" -> ESucceeded: stopping the AScheduler");
            // Indicates that transaction succeeded. 
            iTransFailed = EFalse;
            iASWait->AsyncStop();
            break;
        }

        case THTTPEvent::EFailed:
        {
            //LOG.debug(" -> EFailed: stopping the AScheduler");
            iTransFailed = ETrue;
            iASWait->AsyncStop();
            break;
        }

        default:
        {
            // There are more events in THTTPEvent, but they are not usually 
            // needed. However, event status smaller than zero should be handled 
            // correctly since it's error.
            //LOG.debug(" -> default: code %d", aEvent.iStatus);
            if (aEvent.iStatus < 0) {
                printError(aEvent.iStatus);
                iTransFailed = ETrue;
                iASWait->AsyncStop();       // Signal the active scheduler on errors, to continue the process
            }
            else {
                // Not errors (e.g. permanent and temporary redirections)
                LOG.debug("Ignoring HTTP event status (%d)", aEvent.iStatus);
            }
            break;
        }
    }
}

TInt HttpConnection::MHFRunError( TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
{
    // Simply return the error
    LOG.error("HttpConnection MHFRunError - code %d", aError);
    iTransFailed = ETrue;
    return aError;
}


bool HttpConnection::checkToAbort()
{
    if (getLastErrorCode() == KErrCancel) {
        LOG.info("Aborting HTTP request!");
        return true;
    }
    return false;
}

void HttpConnection::cancelRequest()
{
    iTransFailed = ETrue;
    iHttpTransaction.Cancel();
    if (iASWait) {
        iASWait->AsyncStop();
    }
}


void HttpConnection::setRequestHeaders() 
{
    RHTTPHeaders headers = iHttpTransaction.Request().GetHeaderCollection();
    
    //
    // write ALL the request headers set by the client
    //
    KeyValuePair p;
    for (p=requestHeaders.front(); !p.null(); p=requestHeaders.next()) { 
        setHeader(headers, p.getKey(), p.getValue());
    }
    
    // Sets the User-Agent header (it's passed in the constructor)
    if (!userAgent.empty()) {
        setHeader(headers, HTTP_HEADER_USER_AGENT, userAgent.c_str());
    }
    
    //
    // write client authentication header (only basic is supported)
    //
    if (auth) {
        if (auth->getType() == HttpAuthentication::Basic) {
            StringBuffer value = auth->getAuthenticationHeaders();
            TRAPD(err, SetBasicAuthorizationL(headers, value);)
            if (err) {
                LOG.error("Error (%d) setting basic authorizazion \"%s: %s\"", err, HTTP_HEADER_AUTHORIZATION, value.c_str());
            }
        }
        else {
            LOG.error("Digest authentication not yet supported - please use Basic auth");
        }
    }
}

void HttpConnection::setHeader(RHTTPHeaders& aHeaders, const StringBuffer& property, const StringBuffer& value)
{
    if (property.null() || value.null()) {
        return;  // NULL strings means the header is not set: skip.
    }
    
    LOG.debug("Setting HTTP request header: \"%s: %s\"", property.c_str(), value.c_str());

    // default http headers
    TInt headerID = KErrNotFound;
    if (property == HTTP_HEADER_USER_AGENT) {
        headerID = HTTP::EUserAgent; 
    }
    else if (property == HTTP_HEADER_ACCEPT) {
        headerID = HTTP::EAccept; 
    }
    else if (property == HTTP_HEADER_ACCEPT_ENCODING) {
        headerID = HTTP::EAcceptEncoding; 
    }
    else if (property == HTTP_HEADER_CONTENT_TYPE) {
        headerID = HTTP::EContentType; 
    }
    else if (property == HTTP_HEADER_CONTENT_LENGTH) {
        // From Symbian C++ Dev library >> Using HTTP Client >> Headers >> Content-Length:
        // "The client should not set this header for any request: it will be ignored."
        return;
    }
    
    if (headerID != KErrNotFound) {
        TRAPD(err, SetDefaultHeaderL(aHeaders, headerID, value);)
        if (err) {
            LOG.error("Error (%d) setting default HTTP header \"%s: %s\"", err, property.c_str(), value.c_str());
        }
        return;
    }
    
    // custom http headers: custom fields are accepted as well
    TRAPD(err, SetCustomHeaderL(aHeaders, property, value);)
    if (err) {
        LOG.error("Error (%d) setting custom HTTP header \"%s: %s\"", err, property.c_str(), value.c_str());
    }
}


void HttpConnection::SetBasicAuthorizationL(RHTTPHeaders& aHeaders, const StringBuffer& credentials)
{
    LOG.debug("Setting HTTP Basic Authorizazion header");
    RStringPool iStrTb = iHttpSession.StringPool();
    HBufC8* bufValue = stringBufferToNewBuf8(credentials);
    RStringF basicCred = iHttpSession.StringPool().OpenFStringL(*bufValue);
    
    // 1st param: "Basic"
    THTTPHdrVal authVal(iStrTb.String(HTTP::EBasic, RHTTPSession::GetTable()));
    aHeaders.SetFieldL(iStrTb.StringF(HTTP::EAuthorization, RHTTPSession::GetTable()), authVal);
    
    // 2nd param: the credentials encoded in b64
    CleanupClosePushL(basicCred);
    authVal.SetStrF(basicCred);
    aHeaders.SetFieldL(iStrTb.StringF(HTTP::EAuthorization, RHTTPSession::GetTable()), authVal);
    CleanupStack::PopAndDestroy(&basicCred);
    
    basicCred.Close();
    delete bufValue;
}


void HttpConnection::SetDefaultHeaderL(RHTTPHeaders& aHeaders, const TInt fieldID, const StringBuffer& value)
{
    HBufC8* bufValue = stringBufferToNewBuf8(value);
    RStringF rValue  = iHttpSession.StringPool().OpenFStringL(*bufValue);
    
    CleanupClosePushL(rValue);
    THTTPHdrVal val(rValue);
    aHeaders.SetFieldL(iHttpSession.StringPool().StringF(fieldID, RHTTPSession::GetTable()), val);
    CleanupStack::PopAndDestroy(&rValue);
    
    rValue.Close();
    delete bufValue;
}

void HttpConnection::SetCustomHeaderL(RHTTPHeaders& aHeaders, const StringBuffer& property, const StringBuffer& value)
{
    HBufC8* bufProperty = stringBufferToNewBuf8(property);
    HBufC8* bufValue    = stringBufferToNewBuf8(value);
    RStringF rProperty  = iHttpSession.StringPool().OpenFStringL(*bufProperty);
    RStringF rValue     = iHttpSession.StringPool().OpenFStringL(*bufValue);
    
    CleanupClosePushL(rValue);
    THTTPHdrVal val(rValue);
    aHeaders.SetFieldL(rProperty, val);
    CleanupStack::PopAndDestroy(&rValue);
    
    rProperty.Close();
    rValue.Close();
    delete bufProperty;
    delete bufValue;
}


void HttpConnection::readResponseHeaders() 
{
    LOG.debug("Reading HTTP response headers");
    RHTTPHeaders headers = iHttpTransaction.Response().GetHeaderCollection();
    RStringPool strP = iHttpSession.StringPool();

    THTTPHdrFieldIter it = headers.Fields();
    while (it.AtEnd() == EFalse)
    {
        // Get the name of the next header field
        RStringTokenF fieldName = it();
        if (fieldName.IsNull()) {   // Check it does indeed exist
            ++it;
            continue;
        }
        RStringF fieldNameStr = strP.StringF(fieldName);
        THTTPHdrVal fieldVal;
        
        if (headers.GetField(fieldNameStr, 0, fieldVal) == KErrNone) {

            // Get name and value (note: wrong value type will panic)
            const char* key = (const char*)fieldNameStr.DesC().Ptr();
            StringBuffer val;
            if (fieldVal.Type() == THTTPHdrVal::KTIntVal) {
                val.sprintf("%d", fieldVal.Int());
            } 
            else if (fieldVal.Type() == THTTPHdrVal::KStrVal || 
                     fieldVal.Type() == THTTPHdrVal::KStrFVal) {
                val = (const char*)fieldVal.Str().DesC().Ptr();
            } 
            else {
                // DateTime type not supported
                ++it;
                continue;
            }
            
            //
            // Add to the headers map
            //
            responseHeaders.put(key, val);
            
            /*
            // Display realm for WWW-Authenticate header
            RStringF wwwAuth = strP.StringF(HTTP::EWWWAuthenticate, RHTTPSession::GetTable());
            if (fieldNameStr == wwwAuth) {
                // check the auth scheme is 'basic'
                RStringF basic = strP.StringF(HTTP::EBasic, RHTTPSession::GetTable());
                RStringF realm = strP.StringF(HTTP::ERealm, RHTTPSession::GetTable());
                THTTPHdrVal realmVal;
                if ( (fieldVal.StrF() == basic) && 
                     (!aHeaders.GetParam(wwwAuth, realm, realmVal)) ) {    // check the header has a 'realm' parameter  
                    RStringF realmValStr = strP.StringF(realmVal.StrF());
                    LOG.debug("www authentication, realm is: %s", buf8ToStringBuffer(realmValStr.DesC()).c_str());
                    realmValStr.Close();
                }
                basic.Close();
                realm.Close();
            }
            wwwAuth.Close();
            */
        }
        ++it;
        fieldNameStr.Close();
    }
}


void HttpConnection::printError(const TInt error)
{
    switch (error)
    {
        case KErrHttpInvalidUri:
            LOG.error("HTTP error: Invalid URI (code %d)", error);
            break;
        case KErrHttpRequestBodyMissing:
            LOG.error("HTTP error: A body is missing from a method that requires it (code %d)", error);
            break;
        default:
            StringBuffer msg = resolveErrorCode(error);
            LOG.error("HTTP error: %d (%s)", error, msg.c_str());
            break;
    }
}

void HttpConnection::printHeaders(StringMap& headersMap)
{
    if (headersMap.empty()) {
        return;
    }
    
    StringBuffer msg("HTTP headers:");
    KeyValuePair p;
    for (p=headersMap.front(); !p.null(); p=headersMap.next()) { 
        msg += "\n";
        msg += p.getKey();
        msg += ": ";
        msg += p.getValue();
    }
    LOG.debug("%s", msg.c_str());
}


END_FUNAMBOL_NAMESPACE


