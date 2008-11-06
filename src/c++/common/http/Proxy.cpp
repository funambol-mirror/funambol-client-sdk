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

#include "http/Proxy.h"

Proxy::Proxy() {
    setProxy(NULL, 0, NULL, NULL);
}


Proxy::Proxy(char* proxyHost, int proxyPort) {
    setProxy(proxyHost, proxyPort, NULL, NULL);
}


Proxy::Proxy(char* proxyHost, int proxyPort, char* proxyUser, char* proxyPassword){
    setProxy(proxyHost, proxyPort, proxyUser, proxyPassword);
}

void Proxy::setProxy(Proxy& newProxy) {
    setProxy(newProxy.host, newProxy.port, newProxy.user, newProxy.password);
}


void Proxy::setProxy(const char*proxyHost    ,
                     int      proxyPort    ,
                     const char*proxyUser    ,
                     const char*proxyPassword) {
    if (proxyHost != NULL) {
        strncpy((char*)host, (const char*)proxyHost, DIM_HOSTNAME);
        host[DIM_HOSTNAME-1] = 0;
    } else {
        memset(host, 0, DIM_HOSTNAME*sizeof(char));
        //strcpy((char*)host, (const char*)"");
    }

    if (proxyUser != NULL){
        strncpy((char*)user, (const char*)proxyUser, DIM_USERNAME);
        user[DIM_USERNAME-1] = 0;
    } else {
        memset(user, 0, DIM_USERNAME*sizeof(char));
        //strcpy((char*)user, (char*)"");
    }

    if (proxyPassword != NULL){
        strncpy((char*)password,(const char*) proxyPassword, DIM_PASSWORD);
        password[DIM_PASSWORD-1] = 0;
    } else {
        memset(password, 0, DIM_PASSWORD*sizeof(char));
        //strcpy((char*)password, (const char*)"");
    }
    port = proxyPort;
}
