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

#include <errno.h>
#include <string.h>

#include "FSocket.h"
#include "base/globalsdef.h"

USE_NAMESPACE

StringBuffer FSocket::lIP;

FSocket::FSocket() : unixSock ( -1 )
{
    memset ( &unixAddr, 0, sizeof ( unixAddr ) );
}

FSocket::~FSocket() {
    close();
}

bool FSocket::isValid() {
    return unixSock != -1;
}

FSocket* FSocket::createSocket(const StringBuffer& peer, int32_t port) {
    int sock = socket ( AF_INET, SOCK_STREAM, 0 );

    if ( sock == -1 )
        return NULL;

    // TIME_WAIT - argh
    int on = 1;
    if ( setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 ) {
        return NULL;
    }

    sockaddr_in addr;
    memset((void*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons ( port );
    //int status = inet_pton ( AF_INET, "85.18.37.27", &addr.sin_addr );
    struct hostent *he = gethostbyname(peer.c_str());
    if (he == NULL) { // do some error checking
        return NULL;
    }

    int status = inet_pton (AF_INET, inet_ntoa(*(struct in_addr*)he->h_addr),
                            &addr.sin_addr);
    if ( errno == EAFNOSUPPORT ) {
        return NULL;
    }

    status = ::connect ( sock, ( sockaddr * ) &addr, sizeof ( addr ) );

    if ( status == 0 ) {
        FSocket* newSocket = new FSocket();
        newSocket->unixSock = sock;
        newSocket->unixAddr = addr;
        return newSocket;
    } else {
        return NULL;
    }
}


int32_t FSocket::writeBuffer(const int8_t* const buffer, int32_t len)
{
    int32_t status = ::send ( unixSock, buffer, len, MSG_NOSIGNAL );
    return status;
}


int32_t FSocket::readBuffer(int8_t* buffer, int32_t maxLen)
{
    memset ( buffer, 0, maxLen );
    int32_t status = ::recv ( unixSock, buffer, maxLen, 0 );

    if ( status < 0 ) {
        return -1;
    } else if ( status == 0 ) {
        return 0;
    } else {
        return status;
    }
}

const StringBuffer& FSocket::address() const {
    return lAddress;
}

const StringBuffer& FSocket::peerAddress() const {
    return pAddress;
}


void FSocket::close() {
    if ( isValid() )
        ::close ( unixSock );
}

const StringBuffer& FSocket::localIP() {
    return lIP;
}





