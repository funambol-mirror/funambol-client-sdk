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

#ifndef INCL_CURL_TRANSPORT_AGENT
    #define INCL_CURL_TRANSPORT_AGENT
/** @cond DEV */

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"

    #include "curl/curl.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection. It uses libcurl.
     */

    class CurlTransportAgent : public TransportAgent {
        CURL *easyhandle;
        struct curl_slist *slist;
        char proxyauth[DIM_USERNAME + 1 + DIM_PASSWORD];

        char *sendbuffer;
        size_t sent, sendbuffersize;
        static size_t sendData(void *ptr, size_t size, size_t nmemb, void *stream);

        char *responsebuffer;
        size_t received, responsebuffersize;
        static size_t receiveData(void *buffer, size_t size, size_t nmemb, void *stream);

        char curlerrortxt[CURL_ERROR_SIZE];

        static int debugCallback(CURL *easyhandle, curl_infotype type, char *data, size_t size, void *unused);

    public:
        CurlTransportAgent();
        CurlTransportAgent(URL& url, Proxy& proxy, unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT);
        ~CurlTransportAgent();

        char* sendMessage(const char* msg);
        void setUserAgent(const char* ua);

    };


END_NAMESPACE

/** @endcond */
#endif
