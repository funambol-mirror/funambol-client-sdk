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

#ifndef INCL_HTTP_CONNECTION
#define INCL_HTTP_CONNECTION
/** @cond DEV */

#include <coecntrl.h>
#include <http/mhttptransactioncallback.h>
#include <http/rhttpsession.h>
#include <http/rhttpheaders.h>

#include "base/globalsdef.h"
#include "http/URL.h"
#include "http/Proxy.h"
#include "http/AbstractHttpConnection.h"
#include "http/HttpDataSupplier.h"

BEGIN_FUNAMBOL_NAMESPACE


#define MAX_RETRIES                     3      // Max number of attempts sending http requests.


/**
 * The Symbian implementation of AbstractHttpConnection, to manage HTTP connections.
 * When opening the HTTP session, the GPRSConnection singleton is used in order to
 * reuse the active connection, if any.
 *
 * It's an active object: the HTTP transaction is an asyncronous call, it extends the
 * MHTTPTransactionCallback interface to receive proper callbacks for HTTP events.
 * In order to make the request call synchronous, a CActiveSchedulerWait is used: it
 * waits on the HTTP transaction to complete, and return the control to the caller.
 * To send the request body chunk by chunk, an HttpDataSupplier is used: it's an adapter
 * between the inputStream and the MHTTPDataSupplier, to read from the inputStream when
 * the HTTP framework asks for the next body part to send.
 * It checks regularly if the user aborted the sync: in this case, the request method
 * leaves with the code KErrorCancel(-3), so it should be trapped at higher level.
 */
class HttpConnection : public AbstractHttpConnection,
                       public CBase,
                       public MHTTPTransactionCallback {

public:

    HttpConnection(const char* ua);

    virtual ~HttpConnection();


    /**
     * Open the connection to the given url.
     * Caller must always call close() at the end, if an HTTPConnection was opened.
     * @param url    the connection url
     * @param method (optional) the request method, one of RequestMethod enum
     */
    virtual int open(const URL& url, RequestMethod method = MethodPost, bool log_request=true);

    /**
     * This method closes this connection. It does not close the corresponding
     * input and output stream which need to be closed separately (if they were
     * previously opened).
     * This must be called if the connection was previously opened with open().
     */
    virtual int close();

    /**
     * Sends the request.
     * The method used must be set via setRequestMethod (default = POST).
     * The headers/authentication must already be set before calling this method.
     *
     * @param data      the data to be sent
     * @param response  the response returned
     * @return          the HTTP Status Code returned or -1 if no status code can be discerned.
     */
    virtual int request(InputStream& data, OutputStream& response, bool log_request=true);
    int request(const char* data, OutputStream& response, bool log_request=true);

    /**
     * Checks if user decided to abort current HTTP transaction.
     * Checks the lastErrorCode (TODO: use config abort flag)
     * @return  true if the user chosed to abort this process
     */
    bool checkToAbort();

    /**
     * Cancels the HTTP transaction and signals the active scheduler,
     * in order to exit from the Active Object and return the control to the client.
     * It's used when the user aborted current sync.
     */
    void cancelRequest();

    /**
     * Callback method, from HttpDataSupplier.
     * Notifies HTTP of the availability of more request body data,
     * when submitting body data in several parts.
     */
    void notifyForNewBodyData();


private:

    RHTTPSession               iHttpSession;
    RHTTPTransaction           iHttpTransaction;
    TBool                      iTransFailed;
    CActiveSchedulerWait*      iASWait;

    /// Linked with the inputStream, used to retrieve body data to send, chunk by chunk.
    HttpDataSupplier*          iDataSupplier;

    /// The output stream where the HTTP response is written.
    OutputStream*              iOutputStream;

    /// The status code of the HTTP transaction.
    TInt                       iLastHTTPStatus;


    /**
     * Opens the http session (calls reuseActiveConnection) and creates
     * a new iASWait object.
     * Uses the FConnection to get the already active connection, if exists.
     * An active scheduler must be installed before calling this method.
     * Leaves if the open session failed.
     */
    void OpenL();

    /**
     * Uses an already active RConnection owned by singleton FConnection,
     * this saves resources and memory.
     * http://wiki.forum.nokia.com/index.php/CS000825_-_Using_an_already_active_connection
     */
    void reuseActiveConnectionL();

    /**
     * Sets all the headers for the HTTP request to send.
     * It reads from requestHeaders map, so the headers must be already set by the client.
     * If one of these headers is not set, it will just be skipped.
     */
    void setRequestHeaders();

    /**
     * Reads all the headers from the HTTP response received.
     * It sets the responseHeaders map, so the client will be able to retrieve them
     * calling the getResponseHeader() method after this call.
     * It's called by the active scheduler once the response is obtained.
     */
    void readResponseHeaders();

    /**
      * Set a field-value pair in the passed HTTP header collection.
      * It can be a default HTTP header, or a custom header.
      * Null properties or null values will not be set (just ignored).
      * @param aHeaders   [IN-OUT] header where field-value pair is written.
      * @param property  header name
      * @param value     header value
      */
    void setHeader(RHTTPHeaders& aHeaders, const StringBuffer& property, const StringBuffer& value);

    /**
     * Sets the Basic access authorization, for the HTTP request to send.
     * Please see Symbian C++ Dev library >> Using HTTP Client >> Headers >> Authorization
     * @param aHeaders     [IN-OUT] header where field-value pair is written.
     * @param credentials  the credentials to set, the value that is assigned for the field
     *                     Expected already in the form: "base64(username:password)"
     */
    void SetBasicAuthorizationL(RHTTPHeaders& aHeaders, const StringBuffer& credentials);

    /**
      * Set a field-value pair for the given HTTP header.
      * It's used only for DEFAULT HTTP headers, given their ID. Can leave.
      * @param aHeaders   [IN-OUT] header where field-value pair is written.
      * @param fieldID    field id of the header, used to lookup the RHTTPSession string table
      * @param value      value that is assigned for the field.
      */
     void SetDefaultHeaderL(RHTTPHeaders& aHeaders, const TInt fieldID, const StringBuffer& value);

     /**
       * Set a field-value pair for the given HTTP header.
       * It's used only for CUSTOM HTTP headers, given the header name and value. Can leave.
       * @param aHeaders   [IN-OUT] header where field-value pair is written.
      * @param property    header name
      * @param value       header value
       */
    void SetCustomHeaderL(RHTTPHeaders& aHeaders, const StringBuffer& property, const StringBuffer& value);



    void SetCookieHeader(RHTTPHeaders& aHeaders, const StringBuffer& cookie_name, const StringBuffer& cookie_value);

     /// Just logs useful info for most common transaction errors.
     void printError(const TInt status);

     /// Just logs @debug all the headers.
     void printHeaders(StringMap& headersMap);


    //
    // ------------- From MHTTPTransctionCallback -------------
    //
    /**
     * Called by framework to notify about transaction events.
     * @param aTransaction: Transaction, where the event occured.
     * @param aEvent:       Occured event.
     */
    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    /**
     * Called by framework when *leave* occurs in handling of transaction event.
     * @param aError:       The leave code that occured.
     * @param aTransaction: The transaction that was being processed when leave occured.
     * @param aEvent:       The event that was being processed when leave occured.
     * @return KErrNone,    if the error was handled. Otherwise the value of aError, or
     *                      some other error value. Returning error value causes causes
     *                      HTTP-CORE 6 panic.
     */
    TInt MHFRunError( TInt aError,
                      RHTTPTransaction aTransaction,
                      const THTTPEvent& aEvent );

};


END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif

