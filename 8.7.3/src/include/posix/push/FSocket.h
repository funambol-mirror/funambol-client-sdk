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

#ifndef INCL_FSOCKET
#define INCL_FSOCKET

#include "base/fscapi.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

#include "stdint.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class FSocket {

protected:
    FSocket();

public:
    virtual ~FSocket();

    /**
     Reads all available bytes from the socket, up to the maximum specified 
     by the second parameter. This call is synchronous and it blocks the
     caller until something is available or the max length is reached.
     Returns the actual number of bytes read (-1 if the socket cannot be
     read or in case of any network error).
    */
    virtual int32_t readBuffer(int8_t* buffer, int32_t maxLen);

    /**
     Writes the given buffer to the stream and flush it. The buffer length
     is specified by the second parameter.
     The method returns the number of written bytes. On success this value
     is the same as len. On errors it can be less than len and it specifies 
     the number of bytes written before a network error was encountered.
    */
    virtual int32_t writeBuffer(const int8_t* const buffer, int32_t len);

    /**
     Close this socket. After this operation the object can be released as
     any IO operation is invalid.
    */
    virtual void close();

public:

    // This method is the factory to create sockets

    /**
     Opens a socket connecting to the peer host on the given port.
     Returns a valid object if the connection can be establishd. Returns
     NULL if the socket cannot be created for any reason.

     This method is the factory to create sockets.
    */
    static FSocket* createSocket(const StringBuffer& peer, int32_t port);
    
    /**
     Set a custom FSocket used instead of the platform dependent one
     */
    static void setSocket(FSocket* custom) { customSocket = custom; }


private:
    bool isValid();
    static FSocket* customSocket;

    int unixSock;
    sockaddr_in unixAddr;

};

END_NAMESPACE

#endif
