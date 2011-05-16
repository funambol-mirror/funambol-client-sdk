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

#ifndef INCL_HTTP_DATA_SUPPLIER
#define INCL_HTTP_DATA_SUPPLIER
/** @cond DEV */

#include <coecntrl.h>
#include <http/mhttpdatasupplier.h>
#include "base/globalsdef.h"
#include "http/AbstractHttpConnection.h"


BEGIN_FUNAMBOL_NAMESPACE

// FORWARD DECLARATIONS
class HttpConnection;


/**
 * A data supplier - This class is used by the client to supply request
 * body data to HTTP in POST transactions.
 * It's used by HttpConnection (see constructor), and the body data
 * to supply is read directly from an inputStream.
 * 
 * The caller must set the input stream (setInputStream) before submitting
 * the HTTP transaction.
 * The HTTP framework will opportunely callback the GetNextDataPart() and ReleaseData()
 * during the HTTP transaction, to get the input stream data chunk by chunk
 *  and send over the network.
 * The chunk size can be specified via the setMaxRequestChunkSize() method.
 * 
 * For each chunk, a check is done in case the user aborted the current process.
 * In this case, the HTTPConnection callback method cancelRequest() is invoked.
 */
class HttpDataSupplier : public MHTTPDataSupplier {

public:

    HttpDataSupplier(HttpConnection* caller);

    virtual ~HttpDataSupplier();

    /**
     * Sets the internal InputStream pointer to this one.
     * It will be user to read data when the callback method GetNextDataPart() is
     * called by the HTTP frameork.
     * Note: the input stream is NOT closed by this class, it's just used.
     */
    void setInputStream(InputStream& stream);

    /// Sets the max chunk size, for HTTP request (outgoing data is chunked)
    void setMaxRequestChunkSize(const int size) { maxRequestChunkSize = size; }
    
private:
    
    InputStream*     iInputStream;
    
    char*            chunkToSend;
    
    /// Max chunk size for http request, in bytes
    int              maxRequestChunkSize;

    /// The parent HttpConnection that created this object, for callbacks.
    HttpConnection*  iHttpConnection;
    
    
    //
    // ------------- From MHttpDataSupplier -------------
    //
    /**
     * Obtain a data part from the supplier.  The data is guaranteed
     * to survive until a call is made to ReleaseData().
     * @param aDataPart - the data part
     * @return ETrue if this is the last part. EFalse otherwise
     */
    TBool GetNextDataPart(TPtrC8& aDataChunk);

    /**
     * Release the current data part being held at the data
     * supplier. This call indicates to the supplier that the part
     * is no longer needed, and another one can be supplied, if appropriate.
     */
    void ReleaseData();

    /**
     * Obtain the overall size of the data being supplied, if known
     * to the supplier.  Where a body of data is supplied in several
     * parts this size will be the sum of all the part sizes. If
     * the size is not known, KErrNotFound is returned; in this case
     * the client must use the return code of GetNextDataPart to find
     * out when the data is complete.
     *
     * @return A size in bytes, or KErrNotFound if the size is not known.
     */
    TInt OverallDataSize();

    /**
     * Reset the data supplier.  This indicates to the data supplier that it should
     * return to the first part of the data.  This could be used in a situation where
     * the data consumer has encountered an error and needs the data to be supplied
     * afresh.  Even if the last part has been supplied (i.e. GetNextDataPart has
     * returned ETrue), the data supplier should reset to the first part.
     *
     * If the supplier cannot reset it should return an error code; otherwise it should
     * return KErrNone, where the reset will be assumed to have succeeded
     */
    TInt Reset();
};


END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif

