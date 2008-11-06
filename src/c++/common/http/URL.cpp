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


#include "base/util/utils.h"
#include "http/URL.h"


/*
 * Creates a new URL object from a string representation of the URL. The URL
 * must be in the form:
 *
 * <protocol>://<hostname>[:<port>]/<resource>
 *
 * If a parsing error occurs, fullURL is set to an empty string ("").
 *
 * @param url the URL
 */
URL::URL(const char* url) : fullURL(NULL), protocol(NULL), host(NULL), resource(NULL){
    setURL(url);
}

/*
 * Default constructs
 */
URL::URL() : fullURL(NULL), protocol(NULL), host(NULL), resource(NULL) {
}

URL::~URL() {
    if (fullURL) {
        delete [] fullURL; fullURL = NULL;
    }

    if (protocol) {
        delete [] protocol; protocol = NULL;
    }

    if (host) {
        delete [] host; host = NULL;
    }

    if (resource) {
        delete [] resource; resource = NULL;
    }

}

void URL::setURL(URL& url) {
    setURL(url.fullURL, url.protocol, url.host, url.resource, url.port);
}

void URL::setURL(const char*u, const char*p, const char*h, const char*r, unsigned int port) {
    if (fullURL) {
        delete [] fullURL; fullURL = NULL;
    }

    if (u) {
        fullURL = stringdup(u);
    }

    if (protocol) {
        delete [] protocol; protocol = NULL;
    }

    if (p) {
        protocol = stringdup(p);
    }

    if (host) {
        delete [] host; host = NULL;
    }

    if (h) {
        host = stringdup(h);
    }

    if (resource) {
        delete [] resource; resource = NULL;
    }

    if (r) {
        resource = stringdup(r);
    }

    if (port == -1) {
        //
        // use default ports
        //
        this->port = isSecure() ? 443 : 80;
    } else {
        this->port = port;
    }
}

void URL::setURL(const char* url) {
    if ((url == NULL) || (strlen(url) == 0)) {
        return;
    }

    int size;

    char* s = NULL;
    char* q = NULL;

    //
    // protocol (mandatory)
    //
    s = strstr((char*)url, "://");
    if ((s == NULL) || (s == url)) {
        return;
    }

    size = s-url;
    char* p = new char[size+1];
    strncpy(p, url, size);  p[size] = 0;

    //
    // server (mandatory)
    // and
    // port (optional)
    //
    s += 3;
    q = strstr(s, "/");
    if (q == NULL) {
        size = strlen(s);
    } else {
        size = q-s;
    }
    char* h = new char[size+1];
    strncpy(h, s, size); h[size] = 0;

    unsigned int port = (unsigned int)-1;
    s = strstr(h, ":");
    if (s) {
        port = strtol(s+1, NULL, 10);
        *s = 0;
    }

    //
    // resource
    //
    size = q ? strlen(q) : 0;
    char* r = new char[size+1];
    if (size) strncpy(r, q, size); r[size] = 0;

    //
    // fullURL
    //
    size = strlen(url);
    char* u = new char[size+1];
    strcpy(u, url);

    setURL(u, p, h, r, port);

    //
    // frees all pointer
    //
    if (p != NULL)
        delete [] p;
    if (u != NULL)
        delete [] u;
    if (h != NULL)
        delete [] h;
    if (r != NULL)
        delete [] r;


}

URL& URL::operator= (URL& url) {
    setURL(url); return *this;
}

URL& URL::operator= (const char* url) {
    setURL(url); return *this;
}

BOOL URL::isSecure() {
   char* t = strtolower(protocol);

   BOOL ret = (strcmp(t, "https") == 0);

   delete [] t; t = NULL;

   return ret;
}
