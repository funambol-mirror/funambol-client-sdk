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


#include <http.h>
#include <httperr.h>

#include "base/util/stringUtils.h"
#include "http/errors.h"
#include "http/HttpDataSupplier.h"
#include "http/HttpConnection.h"
#include "event/FireEvent.h"

BEGIN_FUNAMBOL_NAMESPACE



HttpDataSupplier::HttpDataSupplier(HttpConnection* caller) : iHttpConnection(caller), 
                                                             iInputStream(NULL),
                                                             chunkToSend(NULL),
                                                             maxRequestChunkSize(DEFAULT_CHUNK_SIZE) 
{
}

HttpDataSupplier::~HttpDataSupplier() 
{
    delete [] chunkToSend;
}


void HttpDataSupplier::setInputStream(InputStream& stream)
{
    iInputStream = &stream;
    chunkToSend = NULL;
}


TBool HttpDataSupplier::GetNextDataPart(TPtrC8& aDataChunk)
{
    //LOG.debug("entering %s", __FUNCTION__);
    if (!iInputStream) {
        LOG.error("%s: internal error: input stream is NULL", __FUNCTION__);
        return ETrue;
    }
    if (maxRequestChunkSize == 0) {
        LOG.error("%s: maxRequestChunkSize = 0 (wrong SAPI configuration?)", __FUNCTION__);
        return ETrue;
    }
    
    // note: do not check the abort here (will panic KERN-EXEC 3)
    // It's done in ReleaseData(), once the chunkToSend is released.
    
    //LOG.debug("reading %d bytes from pos %d", maxRequestChunkSize, iInputStream->getPosition());
    chunkToSend = new char[maxRequestChunkSize];
    int readBytes = iInputStream->read(chunkToSend, maxRequestChunkSize);
    aDataChunk.Set((TUint8*)chunkToSend, readBytes);

    //LOG.debug("sending %d bytes", readBytes);
    fireTransportEvent((unsigned long)readBytes, DATA_SENT);

    if (iInputStream->eof()) {
        //LOG.debug("inputStream EOF");
        return ETrue;
    } else {
        //LOG.debug("inputStream not EOF: position = %d", iInputStream->getPosition());
        return EFalse;
    }
}

void HttpDataSupplier::ReleaseData()
{
    //LOG.debug("entering %s", __FUNCTION__);
    delete [] chunkToSend;
    chunkToSend = NULL;
    
    // Callback to abort the transaction
    if (iHttpConnection->checkToAbort()) {
        if (iInputStream) {
            LOG.debug("%d on %d bytes sent", iInputStream->getPosition(), iInputStream->getTotalSize());
        }
        iHttpConnection->cancelRequest();
        return;
    }
    
    // Notify the availability of more data to send (callback)
    if (iInputStream && !iInputStream->eof()) {
        iHttpConnection->notifyForNewBodyData();
    }
}

TInt HttpDataSupplier::OverallDataSize()
{
    //LOG.debug("entering %s", __FUNCTION__);
    if (iInputStream) {
        int bytesToSend = iInputStream->getTotalSize() - iInputStream->getPosition();
        //LOG.debug("HTTP overall data size (Content-Length) = %d", bytesToSend);
        return bytesToSend;
    }
    return 0;
}

TInt HttpDataSupplier::Reset()
{
    //LOG.debug("entering %s", __FUNCTION__);
    if (iInputStream) {
        iInputStream->reset();
    }
    return 0;
}


END_FUNAMBOL_NAMESPACE


