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

#include "base/Log.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/DigestAuthentication.h"
#include "event/FireEvent.h"

USE_NAMESPACE

/**
 * Construct a digest authentication object with the given username and password.
 *
 * @param name The username of the client to authenticate
 * @param password The corresponding password
 */
DigestAuthentication::DigestAuthentication(const StringBuffer& user, const StringBuffer& pass) 
                                           : HttpAuthentication(Digest, user, pass) {
}


/**
 * Return generated authentication headers for the current username and password.
 *
 * @param authstr The authentication string from which the headers are generated
 * @param url The URL to authenticate against
 * @param hashProvider An object that provides a hashing function in order to hash the response
 * 
 * @return The authentication response headers
 */
StringBuffer DigestAuthentication::getAuthenticationHeaders(const char* authstr, URL url, const HashProvider *hashProvider) {
	processAuthInfo(authstr);
	return generateAuthResponseString(url, hashProvider);
}

/**
 * Extract a property from a string of equal-sign delimited properties.
 *
 * @param authstr The authentication string of properties
 * @param prop The property whose value is to be returned
 *
 * @return the extracted value of the given property, if it exists
 */
StringBuffer DigestAuthentication::extractDigestProp(const char* authstr, const char* prop) {
	StringBuffer result = "";

	const char* start;
	const char* end;
    const char* ptr;

    char * temp = new char[100];
	sprintf(temp, "%s=\"", prop);
    
    // Find start of prop
	start = strstr(authstr, temp);
    if (start == NULL) {
        return "";   
    }
    start = start + strlen(temp);
    end = start;

    delete [] temp;

    while ((ptr = strstr(end, "\"")) != NULL) {
        if (*(ptr-1) != '\\') {
            end = ptr;
            break;
        }
    }

    // Hit end of authstr
    if (ptr == NULL) {
        end = start + strlen(start);
    }

    int len = end - start;
    result = "";
    result.append(start);
	return result.substr(0, len);
}

/**
 * Get the required authentication information from the given authentication string.
 *
 * @param authstr The authentication string that contains the necessary components of digest authentication.
 */
void DigestAuthentication::processAuthInfo(const char* authstr) {
	if (strstr(authstr, "Digest") != authstr) {
		return;
	}
	this->realm = extractDigestProp(authstr, "realm");
	this->qop = extractDigestProp(authstr, "qop");
	this->nonce = extractDigestProp(authstr, "nonce");
	this->opaque = extractDigestProp(authstr, "opaque");
}

/**
 * Generate an authentication response header.
 *
 * @param hashProvider The implementation of HashProvider that provides a valid hashing function for digest authentication
 * @param url The URL to authenticate against
 *
 * @return an authentication response header
 */
StringBuffer DigestAuthentication::generateAuthResponseString(URL url, const HashProvider *hashProvider) {
	if (!hashProvider) {
		return NULL; // TODO Throw an exception if there is no hash provider?
	}

	StringBuffer nc = "00000001";
    StringBuffer cnonce;
	StringBuffer ha1;
	StringBuffer ha2;
	StringBuffer finalhash;

    StringBuffer key = "";
    key.sprintf("%d:%d", rand, rand);
	cnonce = hashProvider->getHash(key.c_str());

	StringBuffer temp = "";
	temp.append(this->username.c_str()).append(":")
	    .append(this->realm).append(":")
	    .append(this->password.c_str());
	ha1 = hashProvider->getHash(temp.c_str());

	temp = "POST:";
	temp.append(url.resource);
	ha2 = hashProvider->getHash(temp.c_str());

    StringBuffer tohash = "";
	tohash.append(ha1).append(":")
        .append(this->nonce).append(":")
        .append(nc).append(":")
        .append(cnonce).append(":")
        .append(this->qop).append(":")
        .append(ha2);

	finalhash = hashProvider->getHash(tohash);

	StringBuffer response = "";
    response.sprintf("Digest username=\"%s\", realm=\"%s\", "
        "nonce=\"%s\", uri=\"%s\", qop=\"%s\", nc=\"%s\", "
        "cnonce=\"%s\", response=\"%s\", opaque=\"%s\"",
        this->username.c_str(),
        this->realm.c_str(),
        this->nonce.c_str(),
        url.resource,
        this->qop.c_str(),
        nc.c_str(),
        cnonce.c_str(),
        finalhash.c_str(),
        this->opaque.c_str());

	return response;
}

