/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2010 Funambol, Inc.
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

#ifndef INCL_HTTP_CONNECTION
#define INCL_HTTP_CONNECTION
/** @cond DEV */

#include "base/globalsdef.h"
#include "http/URL.h"
#include "http/Proxy.h"
#include "http/AbstractHttpConnection.h"

BEGIN_FUNAMBOL_NAMESPACE

// FIXME: these must go to http/errors.h 
#define ERR_HTTP_TIME_OUT               ERR_TRANSPORT_BASE+ 7
#define ERR_HTTP_NOT_FOUND              ERR_TRANSPORT_BASE+60
#define ERR_HTTP_REQUEST_TIMEOUT        ERR_TRANSPORT_BASE+61
#define ERR_HTTP_INFLATE                ERR_TRANSPORT_BASE+70
#define ERR_HTTP_DEFLATE                ERR_TRANSPORT_BASE+71
#define ERROR_INTERNET_OFFLINE_MODE     0x0002                  // Not sure why it's not defined under wininet.h ...

#define DEFAULT_INTERNET_READ_BUFFER_SIZE  4096

class WString;

/**
 * The Windows implementation of AbstractHttpConnection, to manage HTTP connections.
 *
 */
class HttpConnection : public AbstractHttpConnection 
{
private:
    HINTERNET inet;			// WinInet root handle
	HINTERNET connection;	// HttpRequest connection handle
	HINTERNET req;

public:
	HttpConnection(const char* user_agent);
	~HttpConnection();

    /**
     * Open the connection to the given url.
     * Caller must always call close() at the end, if an HTTPConnection was opened.
     * @param url    the connection url
     * @param method (optional) the request method, one of RequestMethod enum
     */
    int open(const URL& url, RequestMethod method = MethodPost);

    /**
     * This method closes this connection. It does not close the corresponding
     * input and output stream which need to be closed separately (if they were
     * previously opened).
     * This must be called if the connection was previously opened with open().
     */
    int close();

    /**
     * Sends the request.
     * The method used must be set via setRequestMethod (default = POST).
     * The headers/authentication must already be set before calling this method.
     *
     * @param data      the data to be sent
     * @param response  the response returned
     * @return          the HTTP Status Code returned or -1 if no status code can be discerned.
     */
    int request(InputStream& data, OutputStream& response);
    int request(const char* data, OutputStream& response);

    /**
     * Checks if user decided to abort current HTTP transaction.
     * Checks the config abort flag.
     * @return  true if the user chosed to abort this process
     */
    bool checkToAbort();

    /**
     * Cancels the HTTP transaction,
     * It's used when the user aborted current sync in order to return the control to the client.
     */
    void cancelRequest();

private:
    void addAuthenticationHeaders();
    void writeHttpHeaders(WString &headers);
    int sendData(const char* data, int data_len);
	char* createHttpErrorMessage(DWORD errorCode);
    int readResponseHeaders();
    int readResponse(OutputStream& os);
};


END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif

