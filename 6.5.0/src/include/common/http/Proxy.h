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
#ifndef INCL_HTTP_PROXY
#define INCL_HTTP_PROXY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "http/constants.h"


class Proxy {

    public:
        char host    [DIM_HOSTNAME];
        char user    [DIM_USERNAME];
        char password[DIM_PASSWORD];
        int     port;

        Proxy();
        Proxy(char*  host, int port);

        Proxy(char*  host, int port, char*  user, char*  password);

        void setProxy(Proxy& proxy);
        void setProxy(const char* proxyHost, int proxyPort, const char* proxyUser, const char* proxyPassword);

        Proxy& operator= (Proxy& proxy) { setProxy(proxy); return *this;}
    };
/** @endcond */
#endif
