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

#include "base/fscapi.h"

#if FUN_TRANSPORT_AGENT == FUN_CURL_TRANSPORT_AGENT

#include "base/posixlog.h"
#include "base/messages.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/CurlTransportAgent.h"
#include "base/globalsdef.h"
#include "base/util/KeyValuePair.h"

BEGIN_FUNAMBOL_NAMESPACE

/*
 * This is the libcurl implementation of the TransportAgent object
 */

/*
 * Singleton: initialize libcurl once per program
 */
class CurlInit {
    static CURLcode initres;
    static CurlInit singleton;

    CurlInit() { initres = curl_global_init(CURL_GLOBAL_ALL); }
    ~CurlInit() { curl_global_cleanup(); initres = CURLE_FAILED_INIT; }

public:
    static CURL *easy_init() { return !initres ? curl_easy_init() : NULL; }
};

CURLcode CurlInit::initres;
CurlInit CurlInit::singleton;

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
CurlTransportAgent::CurlTransportAgent(const URL& newURL, Proxy& newProxy,
                                       unsigned int maxResponseTimeout) :
                                       TransportAgent(newURL, newProxy, maxResponseTimeout){
    easyhandle = CurlInit::easy_init();

    if (easyhandle) {
        curl_easy_setopt(easyhandle, CURLOPT_HEADERFUNCTION, responseHeader);
        curl_easy_setopt(easyhandle, CURLOPT_WRITEHEADER, this);
        curl_easy_setopt(easyhandle, CURLOPT_DEBUGFUNCTION, debugCallback);
        curl_easy_setopt(easyhandle, CURLOPT_VERBOSE, LOG.getLevel() ? true : false);
        curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, true);
        curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, receiveData);
        curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(easyhandle, CURLOPT_READFUNCTION, sendData);
        curl_easy_setopt(easyhandle, CURLOPT_READDATA, this);
        curl_easy_setopt(easyhandle, CURLOPT_ERRORBUFFER, this->curlerrortxt );
        curl_easy_setopt(easyhandle, CURLOPT_AUTOREFERER, true);
        curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, true);
        if (proxy.host[0]) {
            curl_easy_setopt(easyhandle, CURLOPT_PROXY, proxy.host);
            if (proxy.port) {
                curl_easy_setopt(easyhandle, CURLOPT_PROXYPORT, proxy.port);
            }
            if (proxy.user) {
                sprintf(proxyauth, "%s:%s", proxy.user, proxy.password);
                curl_easy_setopt(easyhandle, CURLOPT_PROXYUSERPWD, proxyauth);
            }
        }
    }
    setUserAgent("Funambol POSIX SyncML client");
}

void CurlTransportAgent::setUserAgent(const char*ua) {
    if (ua) {
        TransportAgent::setUserAgent(ua);
        if (easyhandle) {
            curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, userAgent.c_str());
        }
    }
}


CurlTransportAgent::~CurlTransportAgent() {
    if (easyhandle) {
        curl_easy_cleanup(easyhandle);
    }
}

size_t CurlTransportAgent::receiveData(void *buffer, size_t size, size_t nmemb, void *stream) {
    CurlTransportAgent *agent = (CurlTransportAgent *)stream;
    size_t curr = size * nmemb;

    if (agent->received + curr + 1 > agent->responsebuffersize) {
        size_t newbuffersize = agent->responsebuffersize + max(10 * 1024, (curr + 1 + 1024) / 1024 * 1024);
        char *newbuffer = new char[newbuffersize];
        memcpy(newbuffer, agent->responsebuffer, agent->received);
        delete [] agent->responsebuffer;
        agent->responsebuffer = newbuffer;
        agent->responsebuffersize = newbuffersize;
    }
    memcpy(agent->responsebuffer + agent->received,
           buffer,
           curr);
    agent->received += curr;
    agent->responsebuffer[agent->received] = 0;
    return curr;
}

size_t CurlTransportAgent::responseHeader(void *buffer, size_t size, size_t nmemb, void *stream) {
    CurlTransportAgent *agent = (CurlTransportAgent *)stream;
    size_t curr = size * nmemb;

    char cbf[256] = {0};
    if (curr < 256) {
        memcpy(cbf, buffer, curr);

        // find HTTP property with ':' as a delimiter
        const char *propName  = strtok(cbf, ":");
        const char *value = strtok(NULL, ":");
        char *propValue = NULL;

        if (value && strlen(value)) {
            int len=0;
            bool skipchars = true;
            propValue = new char[strlen(value)+1];
            // skip first blank space ' ' and trailings \r\n
            while (*value != '\r' && *value != '\n' && *value != '\0') {
                skipchars = skipchars && (' ' == *value);
                if (skipchars) {
                    value++;
                    continue;
                }

                propValue[len] = *value;
                value++;
                len++;
            }
            propValue[len] = 0;
        }

        agent->responseProperties.put(propName, propValue);
        if (propValue)
            delete [] propValue;
    }
    return curr;
}

size_t CurlTransportAgent::sendData(void *buffer, size_t size, size_t nmemb, void *stream) {
    CurlTransportAgent *agent = (CurlTransportAgent *)stream;
    size_t curr = min(size * nmemb, agent->sendbuffersize - agent->sent);

    memcpy(buffer, agent->sendbuffer + agent->sent, curr);
    agent->sent += curr;
    return curr;
}

int CurlTransportAgent::debugCallback(CURL *easyhandle, curl_infotype type, char *data, size_t size, void *unused)
{
    bool isData = type == CURLINFO_DATA_IN || type == CURLINFO_DATA_OUT;

    if (LOG.getLevel() >= LOG_LEVEL_DEBUG &&
        !isData) {
        POSIX_LOG.setPrefix(type == CURLINFO_TEXT ? "libcurl info: " :
                            type == CURLINFO_HEADER_IN ? "header in: " :
                            type == CURLINFO_HEADER_OUT ? "header out: " :
                            NULL);
        // ignore trailing line break, LOG.debug() will add it automatically
        int logsize = (int)size;
        if (logsize >= 2 && data[logsize - 2] == '\r' && data[logsize - 1] == '\n') {
            logsize -= 2;
        } else if (logsize >= 1 && data[logsize -1] == '\n') {
            logsize -= 1;
        }
        LOG.debug("%.*s",
                  logsize,
                  data);
        POSIX_LOG.setPrefix(NULL);
    }
    return 0;
}

char* CurlTransportAgent::sendMessage(const char* msg) {
    LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);
    POSIX_LOG.setPrefix("data out: ");
    LOG.debug("%s", msg);
    POSIX_LOG.setPrefix(NULL);

    return sendBuffer(msg, strlen(msg));
}

/*
 * msg is not expected to be a zero terminated string !!!
 */
char*  CurlTransportAgent::sendMessage(const char* msg, unsigned int length) {
    LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);
    POSIX_LOG.setPrefix("data out: ");
    LOG.debug("=== %d bytes ===", length); 
    //hexDump(msg, length); // TODO
    POSIX_LOG.setPrefix(NULL);

    return sendBuffer(msg, length);
}

char * CurlTransportAgent::sendBuffer(const void * data, unsigned int size) {
    LOG.debug("CurlTransportAgent::sendBuffer begin");
    if (!easyhandle) {
        setError(ERR_NETWORK_INIT, "libcurl error init error");
        LOG.error("%s", getLastErrorMsg());
        return NULL;
    }

    curl_slist *slist=NULL;
    char *response = NULL;
    long res_code = -1;
    CURLcode code;

    if (!requestProperties.empty()) {
        LOG.debug("Request header:");
        KeyValuePair p;
        for (p=requestProperties.front(); !p.null(); p=requestProperties.next()){
            StringBuffer property(p.getKey());
            property.append(": ").append(p.getValue());
            slist = curl_slist_append(slist, property.c_str());
            LOG.debug("    %s: %s", p.getKey().c_str(), p.getValue().c_str());
        }
    }
    responseProperties.clear();

    // Disable "Expect: 100": it is usually used to check without
    // transmitting the data that the recipient is willing to accept
    // the POST . With SyncML servers that should be the case, so the
    // check just adds one round-trip and worse, "Expect: 100" is not
    // supported by some proxies, causing a complete failure.
    slist = curl_slist_append(slist, "Expect:");
    responsebuffersize = 64 * 1024;
    responsebuffer = new char[responsebuffersize];
    received = 0;
    responsebuffer[0] = 0;
    // todo? url.resource
    const char *certificates = getSSLServerCertificates();
    if ((code = curl_easy_setopt(easyhandle, CURLOPT_POST, true)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_URL, url.fullURL)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDSIZE, size)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, slist)) ||
        /*
         * slightly cheating here: when CURLOPT_CAINFO was set before, we don't unset it because
         * we don't know what the default is
         */
        (certificates[0] && (code = curl_easy_setopt(easyhandle, CURLOPT_CAINFO, certificates))) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYPEER, (long)SSLVerifyServer)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYHOST, (long)(SSLVerifyHost ? 2 : 0))) ||
        (code = curl_easy_perform(easyhandle))) {
        delete [] responsebuffer;
        setErrorF(ERR_HTTP, "libcurl error %d, %.250s", code, curlerrortxt);
        LOG.error("%s", getLastErrorMsg());
    } else {
        response = responsebuffer;

        if (curl_easy_getinfo(easyhandle, CURLINFO_RESPONSE_CODE, &res_code) != CURLE_OK)
        {
            res_code = -1;
        }

        POSIX_LOG.setPrefix("data in: ");
        LOG.debug("=== %d bytes ===\n%s",
                  (int)strlen(response),
                  response);
    }
    POSIX_LOG.setPrefix(NULL);
    responseSize = received;

    setResponseCode(res_code);

    responsebuffer = NULL;
    responsebuffersize = 0;

    if (slist) {
        curl_slist_free_all(slist);
    }
    LOG.debug("CurlTransportAgent::sendBuffer end");    
    return response;
}


char* CurlTransportAgent::query(ArrayList& httpHeaders, long* protocolResponseCode){
    if (!easyhandle) {
        setError(ERR_NETWORK_INIT, "libcurl error init error");
        return NULL;
    }

    curl_slist *slist=NULL;
    char *response = NULL;
    CURLcode code;

    if (!httpHeaders.isEmpty())
    {
    	StringBuffer* el = (StringBuffer*)httpHeaders.front();
		while (el)
		{
			slist = curl_slist_append(slist, el->c_str());
			el = (StringBuffer*)httpHeaders.next();
		}
    }
    responsebuffersize = 64 * 1024;
    responsebuffer = new char[responsebuffersize];
    received = 0;
    responsebuffer[0] = 0;
    // todo? url.resource
    const char *certificates = getSSLServerCertificates();
    if ((code = curl_easy_setopt(easyhandle, CURLOPT_HTTPGET, true)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_URL, url.fullURL)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, slist)) ||
        /*
         * slightly cheating here: when CURLOPT_CAINFO was set before, we don't unset it because
         * we don't know what the default is
         */
        (certificates[0] && (code = curl_easy_setopt(easyhandle, CURLOPT_CAINFO, certificates))) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYPEER, (long)SSLVerifyServer)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYHOST, (long)(SSLVerifyHost ? 2 : 0))) ||
        (code = curl_easy_perform(easyhandle))) {
        delete [] responsebuffer;
        setErrorF(ERR_HTTP, "libcurl error %d, %.250s", code, curlerrortxt);
    } else {
        response = responsebuffer;

    }
    responseSize = received;

    long res_code = 0;
    if (protocolResponseCode)
    {
        *protocolResponseCode = (curl_easy_getinfo(easyhandle, CURLINFO_RESPONSE_CODE, &res_code) == CURLE_OK)
			? res_code : 0;
    }

    responsebuffer = NULL;
    responsebuffersize = 0;

    if (slist) {
        curl_slist_free_all(slist);
    }

    return response;
}

END_FUNAMBOL_NAMESPACE

#endif

