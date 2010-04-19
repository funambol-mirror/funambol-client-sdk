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


#include "base/util/utils.h"
#include "http/URL.h"
#include "base/globalsdef.h"

USE_NAMESPACE


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

/*
 * Copy constructor
 */
URL::URL(const URL& url) {
    this->fullURL   = stringdup(url.fullURL);
    this->host      = stringdup(url.host);
    this->port      = url.port;
    this->protocol  = stringdup(url.protocol);
    this->resource  = stringdup(url.resource);
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

void URL::setURL(const char*u, const char*p, const char*h, const char*r, int port) {
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

bool URL::isSecure() {
   char* t = strtolower(protocol);

   bool ret = (strcmp(t, "https") == 0);

   delete [] t; t = NULL;

   return ret;
}
