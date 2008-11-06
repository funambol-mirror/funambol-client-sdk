/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


// This class represents an HTTP header as read from a buffer or stream
//
#include "base/fscapi.h"

#ifndef INCL_HTTP_HEADER
    #define INCL_HTTP_HEADER
/** @cond DEV */

    #define DIM_HEADERS 50
    #define MSG_BAD_PROTOCOL "BAD PROTOCOL"

    class HTTPHeader {
    private:
        char* version;
        unsigned int status;
        char* statusMessage;
        char* content;
        unsigned int headersCount;
        char* headers[DIM_HEADERS][2]; // up to DIM_HEADERS headers
        unsigned int size;

    public:
        HTTPHeader(char* buf);

        /*
         * Get the size in bytes of this HTTP header
         */
        unsigned int getSize();
        const char* getVersion();
        unsigned int getStatus();
        const char* getStatusMessage();
        const char* getContent();
        unsigned int getHeadersCount();
        char** getHeader(unsigned int index);
        const char* getHeaderValue(const char* header);

        /*
         * Returns the content lenght specified with the header Content-length.
         * If the header is not found, -1 is returned.
         *
         */
        int getContentLength();
    };
/** @endcond */
#endif
