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
#ifndef SYMBIANTRANSPORTAGENT_H_
#define SYMBIANTRANSPORTAGENT_H_

#include <coecntrl.h>

#include "base/globalsdef.h"
#include <http/mhttptransactioncallback.h>
#include <http/mhttpdatasupplier.h>

#include <http/rhttpsession.h>
#include <http/rhttpheaders.h>

#include "http/URL.h"
#include "http/Proxy.h"
#include "http/TransportAgent.h"

BEGIN_NAMESPACE

/**
* CSymbianTransportAgent
* Provides simple interface to Symbian HTTP Client API.
* Link against http.lib, commdb.lib
*/
class CSymbianTransportAgent : public CBase, 
                               public MHTTPTransactionCallback,
                               public MHTTPDataSupplier,
                               public TransportAgent
{
    public:
        /**
         * NewL()
         * Create a CSymbianTransportAgent object.
         * @return A pointer to the created instance of CWebClientEngine
         */
        static CSymbianTransportAgent* NewL(URL& aUrl,Proxy& aProxy,
                             unsigned int aResponseTimeout = DEFAULT_MAX_TIMEOUT,
                             unsigned int aMaxMsgSize = DEFAULT_MAX_MSG_SIZE);

        /**
         * NewLC()
         * Create a CSymbianTransportAgent object.
         * @return A pointer to the created instance of CSymbianTransportAgent
         */
        static CSymbianTransportAgent* NewLC(URL& aUrl,Proxy& aProxy,
                           unsigned int aResponseTimeout = DEFAULT_MAX_TIMEOUT,
                           unsigned int aMaxMsgSize = DEFAULT_MAX_MSG_SIZE);

        /**
         * ~CSymbianTransportAgent()
         * Destroy the object
         */
        ~CSymbianTransportAgent();
        
        /**
         * ConnectL()
         * Open an internet connection.
         */
        void ConnectL();

        /**
         * SetIap()
         * Set a preferred IAP.
         */
        void SetIap(const TUint32& aIap);

    private:
        /**
         * ConstructL()
         * Perform the second phase construction of a CSymbianTransportAgent object.
         */
        void ConstructL(URL& aUrl);

        /**
         * CSymbianTransportAgent()
         * Perform the first phase of two phase construction.
         */
        CSymbianTransportAgent(URL& aUrl, Proxy& aProxy,
                               unsigned int aResponseTimeout);
        
        /**
         * SetHeaderL()
          * Set a field-value pair for the given HTTP header.
          * @param aHeaders header where field-value pair is written.
          * @param aHdrField field that is written to the header.
          * @param aHdrValue value that is assigned for the field.
          */
         void SetHeaderL(RHTTPHeaders aHeaders, 
                          TInt aHdrField, 
                          const TDesC8& aHdrValue);
         

        
    /**
     * From MHTTPTransctionCallback
     */
    private:
        /**
         * MHFRunL()
         * Called by framework to notify about transaction events.
         * @param aTransaction: Transaction, where the event occured.
         * @param aEvent:       Occured event.
         */
        void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

        /**
         * MHFRunError()
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


         /**
         * SetHttpClientError()
         * Set the http client error (4xx) in Funambol client-api
         * @param aStatus response status code.
         */
        void SetHttpClientError(TInt aStatus);
        
        /**
         * SetHttpServerError()
         * Set the http server error (5xx) in Funambol client-api
         * @param aStatus response status code.
         */
        void SetHttpServerError(TInt aStatus);

    /*
     * From TransportAgent
     */
    public:
        /**
         * sendMessage()
         */
        char* sendMessage(const char* msg);

    /*
     * From MHttpDataSupplier
     */
    public:
        TBool GetNextDataPart(TPtrC8& aDataChunk);
        void ReleaseData();
        TInt OverallDataSize();
        TInt Reset();

        
    private: // Data
    
        TUint32                    iIap;

        RHTTPSession               iHttpSession;
        RHTTPTransaction           iHttpTransaction;
        MHTTPDataSupplier*         iDataSupplier;
        HBufC8*                    iPostBody;
        HBufC8*                    iResponseBody;
        TBool                      iTransFailed;
        CActiveSchedulerWait*      iASWait;
};

END_NAMESPACE

#endif //SYMBIANTRANSPORTAGENT_H_
