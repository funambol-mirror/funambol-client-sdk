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
#ifndef INCL_HTTP_URL
    #define INCL_HTTP_URL
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "http/constants.h"

    class __declspec(dllexport) URL {

    public:
        char*  fullURL ;
        char*  protocol;
        char*  host    ;
        char*  resource;
        int    port    ;

        URL();
        URL(const char* url);
        ~URL();

        void setURL(URL& url);
        void setURL(const char*  url);

        BOOL isSecure();

        URL& operator= (URL& url);
        URL& operator= (const char*  url);

    protected:
        void setURL(const char* u, const char* p, const char* h, const char* r, unsigned int port);
    };
/** @endcond */
#endif
