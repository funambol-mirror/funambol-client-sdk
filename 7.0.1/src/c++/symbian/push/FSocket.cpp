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

#include "push/FSocket.h"
#include "base/SymbianLog.h"
#include "base/util/stringUtils.h"
#include "base/util/symbianUtils.h"
#include "base/FConnection.h"
#include "base/globalsdef.h"

USE_NAMESPACE

StringBuffer FSocket::lIP;



FSocket* FSocket::createSocket(const StringBuffer& peer, int32_t port) 
{
    return FSocket::NewL(peer, port);
}


FSocket* FSocket::NewL(const StringBuffer& peer, int32_t port)
{
    FSocket* self = FSocket::NewLC(peer, port);
    CleanupStack::Pop( self );
    
    if (self->getLastStatus() != KErrNone) {
        // Something wrong.
        LOG.debug("FSocket::NewL - error in ConstructL (status = %d)", self->getLastStatus());
        delete self;
        return NULL;
    }
    return self;
}

FSocket* FSocket::NewLC(const StringBuffer& peer, int32_t port)
{
    FSocket* self = new ( ELeave ) FSocket();
    CleanupStack::PushL( self );
    self->ConstructL(peer, port);

    return self;
}


void FSocket::ConstructL(const StringBuffer& peer, int32_t port) 
{
    //LOG.debug("FSocket::ConstructL");
    
    StringBuffer  errorMsg;
    RHostResolver resolver; 
    RBuf          serverName;
    TNameEntry    hostAddress;
    TInetAddr     address;
    TInt          res = KErrNone;

    serverName.Assign(stringBufferToNewBuf(peer));
    
    //
    // Get the connection manager instance
    //
    FConnection* connection = FConnection::getInstance();
    if (!connection) {
        iStatus = -1;
        errorMsg = "Error opening connection";
        goto error;
    }
    // Session is owned by FConnection!
    RSocketServ* session = connection->getSession();

    //
    // Open the Client Socket tcp/ip
    //
#ifdef __WINSCW__
    // WINSCW: simply open the socket
    res = iSocket.Open(*session, KAfInet, KSockStream, KProtocolInetTcp);
#else
    // GCCE: use the existing connection
    // If first time, connect to gprs
    if (!connection->isConnected()) {
        LOG.debug("FSocket: not connected, start new connection");
        if (connection->startConnection()) {
            iStatus = -1;
            errorMsg = "FSocket: error starting connection";
            goto error;
        }
    }
    RConnection* conn = connection->getConnection();
    
    LOG.debug("Opening socket and associate with existing connection");
    res = iSocket.Open(*session, KAfInet, KSockStream, KProtocolInetTcp, *conn);
    //LOG.debug("Socket opened (err = %d)", res);
#endif
    if (res != KErrNone) {
        iStatus = -1;
        errorMsg.sprintf("FSocket : Error opening socket. code %d", res);
        goto error;
    }
    
    
    // This works if serverName is the ip address, like "x.y.z.w"
    res = address.Input(serverName);
    
    if (res != KErrNone) {
        //
        // Try to resolve the host address. (On GCCE, use the existing RConnection)
        //
        LOG.debug("Resolve IP address...");
#ifdef __WINSCW__
        res = resolver.Open(*session, KAfInet, KProtocolInetTcp);
#else
        res = resolver.Open(*session, KAfInet, KProtocolInetTcp, *conn);
#endif
        if (res != KErrNone) {
            iStatus = -2;
            errorMsg.sprintf("FSocket: Host resolver open failed. code %d", res);
            goto error;
        }
        
        resolver.GetByName(serverName, hostAddress, iStatus);
        User::WaitForRequest(iStatus);
        resolver.Close();
        if (iStatus != KErrNone) {
            errorMsg.sprintf("FSocket: DNS lookup failed. code %d", iStatus.Int());
            goto error;
        }

        // Set the socket server address/port
        address = hostAddress().iAddr;
    }
    
    address.SetPort(port);
    
    
    // --- Connect to host ---
    LOG.debug("Socket connect...");
    iSocket.Connect(address, iStatus);
    User::WaitForRequest(iStatus);
    if (iStatus != KErrNone) {
        errorMsg.sprintf("FSocket: Failed to connect to Server. code %d", iStatus.Int()); 
        goto error;
    }

    serverName.Close();
    return;
    
error:
    LOG.error("%s", errorMsg.c_str());
    serverName.Close();
    return;
}



FSocket::FSocket() 
{
    iStatus = KErrNone;
}

FSocket::~FSocket() 
{
    close();
}



int32_t FSocket::writeBuffer(const int8_t* buffer, int32_t len) 
{
    // This doesn't copy the buffer in memory.
    TPtr8 data((TUint8*)buffer, len);
    data.SetLength(len);
    
    // Sends data to the remote host.
    iSocket.Write(data, iStatus);
    User::WaitForRequest(iStatus);

    if (iStatus == KErrNone) {
        return len;
    }
    else {
        LOG.error("FSocket: error writing on socket (status = %d)", iStatus.Int());
        return -1;
    }
}


int32_t FSocket::readBuffer(int8_t* buffer, int32_t maxLen) 
{
    StringBuffer errorMsg;
    
    RBuf8 data;
    data.CreateL(maxLen);
    
    // Receives data from a remote host and completes when data is available.
    TSockXfrLength len;
    iSocket.RecvOneOrMore(data, 0, iStatus, len);
    User::WaitForRequest(iStatus);
    
    TInt msgLen = len();
    
    if (iStatus == KErrNone) {
        if (msgLen <= maxLen) {
            // OK. Copy the message into the preallocated buffer.
            memcpy(buffer, data.Ptr(), msgLen);
            data.Close();
            return msgLen;
        }
        else {
            errorMsg.sprintf("FSocket: error reading, message too big (%d > %d) -> rejected.",
                             (int)msgLen, (int)maxLen);
            goto error;
        }
    }
    else if (iStatus == KErrEof) {
        // Either the remote connection is closed, or the socket has been shutdown.
        errorMsg = "FSocket: read interrupted (connection closed)";
        goto error;
    }
    else {
        errorMsg.sprintf("FSocket: error reading on socket (status = %d)", iStatus.Int());
        goto error;
    }

error:
    LOG.error("%s", errorMsg.c_str());
    buffer = NULL;
    data.Close();
    return -1;
}


void FSocket::close() 
{
    //LOG.debug("FSocket::close");
    // TODO: shutdown if I/O in progress?
    //iSocket.Shutdown(RSocket::EImmediate, iStatus);
    
    // Seems that this call causes a crash when startConnection failed for network error
    //iSocket.CancelAll();

    iSocket.Close();
}




const StringBuffer& FSocket::address() const {
    return lAddress;
}

const StringBuffer& FSocket::peerAddress() const {
    return pAddress;
}

const StringBuffer& FSocket::localIP() {
    return lIP;
}
