/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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

#include <stdlib.h>
#include <string.h>

#include "http/constants.h"
#include "http/HTTPHeader.h"

/*
 * Creates a HTTPHeader from a chars buffer. See HTTP protocol specification
 * (RFC 2616) for details. Note that the internal fields are pointers to
 * buffer positions, so the scope of the HTTPHeader object must be the
 * same of the passed in.buffer.
 *
 * @parfam buf - IN - the buffer containing the header data
 */
HTTPHeader::HTTPHeader(char* buf) {
    char *p1 = NULL;
    char *p2 = NULL;
    char *p3 = NULL;

    int l;

    size = (unsigned int)-1;

    //
    // The header boundary is the first CRLFCRLF couple.
    //
    p1 = strstr(buf, "\r\n\r\n");

    if (p1 == NULL) {
        status = (unsigned int)-1;
        statusMessage = MSG_BAD_PROTOCOL;

        goto finally;
    }

    content = p1 + 4;
    size = content - buf;
    *p1 = 0;

    //
    // Gets the status line
    //
    p1 = strstr(buf, "\r\n");
    if (p1 != NULL) {
        *p1 = 0; p1 += 2; // now p1 points to the next line
    }

    version = strtok(buf, " ");  // version
    if (version == NULL) {
        status = (unsigned int)-1;
        statusMessage = MSG_BAD_PROTOCOL;

        goto finally;
    }

    p2 = strtok(NULL, " "); // status
    if (p2 == NULL) {
        status = (unsigned int)-1;
        statusMessage = MSG_BAD_PROTOCOL;

        goto finally;
    }
    status = atoi(p2);

    statusMessage = strtok(NULL, " "); // this is optional

    //
    // Reads the headers (p1 points to the begining of the line)
    // Headers are in the form:
    //
    // <headername>: <header content>CRLN
    //
    //
    headersCount = 0;
    while ((p1 != NULL) && (*p1) && (headersCount < DIM_HEADERS)) {
        p2 = strstr(p1, "\r\n");
        if (p2 != NULL) {
            *p2 = 0; p2 += 2; // terminates this line and goes to the next one
        }

        //
        // remove heading spaces
        //
        while (*p1 == ' ') {
            ++p1;
        }
        p3 = strchr(p1, ':');
        if (p3 != NULL) {
            *p3 = 0; p3 += 1;  // p3 now points to the value

            //
            // remove tailing spaces in the header
            //
            l = strlen(p1)-1;
            while ((l >= 0 ) && (p1[l] == ' ')) {
                p1[l--] = 0;
            }

            //
            // remove heading spaces in the value
            //
            while(*p3 == ' ') {
                ++p3;
            }

            //
            // Now we have header and value
            //
            headers[headersCount][0] = p1;
            headers[headersCount][1] = p3;

            ++headersCount;
        }
        p1 = p2;
    }

finally:

    return;

}

const char* HTTPHeader::getVersion() {
    return version;
}

unsigned int HTTPHeader::getStatus() {
    return status;
}

const char* HTTPHeader::getStatusMessage() {
    return statusMessage;
}

const char* HTTPHeader::getContent() {
    return content;
}

unsigned int HTTPHeader::getHeadersCount() {
    return headersCount;
}

char** HTTPHeader::getHeader(unsigned int index) {
    if ((index < 0) || (index >= headersCount)) {
        return NULL;
    }

    return headers[index];
}

const char* HTTPHeader::getHeaderValue(const char* header) {
    char h1[DIM_HEADER], h2[DIM_HEADER];
    unsigned int j;

    strncpy(h1, header, DIM_HEADER);
    h1[DIM_HEADER-1] = 0;

    for (unsigned int i=0; i<headersCount; ++i) {
        strncpy(h2, headers[i][0], DIM_HEADER);
        h2[DIM_HEADER-1] = 0;

        //
        // We wanna do a case insensitive comparison
        //
        for (j=0; j<strlen(h1); ++j) {
            h1[j] = tolower(h1[j]);
        }
        for (j=0; j<strlen(h2); ++j) {
            h2[j] = tolower(h2[j]);
        }

        if (strcmp(h1, h2) == 0) {
            return headers[i][1];
        }
    }

    //
    // Not found
    //
    return NULL;
}

/*
 * Returns the content lenght specified with the header Content-length.
 * If the header is not found, -1 is returned.
 *
 */
int HTTPHeader::getContentLength() {
    const char *length = getHeaderValue("content-length");

    if (length == NULL) {
        return -1;
    }

    return atoi(length);
}

/*
 * Get the size in bytes of this HTTP header
 */
unsigned int HTTPHeader::getSize() {
    return size;
}
