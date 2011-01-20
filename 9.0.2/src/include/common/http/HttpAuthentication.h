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

#ifndef INCL_HTTP_AUTHENTICATION
#define INCL_HTTP_AUTHENTICATION

#include "base/util/StringBuffer.h"
#include "http/HashProvider.h"
#include "http/URL.h"

BEGIN_NAMESPACE


class HttpAuthentication {
    
public:
    
    /// These are the possible authentication types
    enum AuthType {
        Basic,
        Digest          // MD5
    };
    
    /**
     * Construct an authentication object of a given type.
     * @param type  the auth type, one of enum AuthType
     */
    HttpAuthentication(AuthType authType) : type(authType) {}
    
    /**
     * Construct an authentication object of a given type, sets also username and password.
     * @param type  The auth type, one of enum AuthType
     * @param user  The username of the client to authenticate
     * @param pass  The corresponding password
     */
    HttpAuthentication(AuthType authType, const StringBuffer& user, const StringBuffer& pass) 
                      : type(authType), username(user), password(pass) {}
    
    virtual ~HttpAuthentication() {}

    virtual void setUsername(const StringBuffer& user) { username = user; }
    virtual void setPassword(const StringBuffer& pass) { password = pass; }
    virtual void setType    (const AuthType      typ)  { type     = typ;  }
    
    virtual StringBuffer getUsername() { return username; }
    virtual StringBuffer getPassword() { return password; }
    virtual AuthType     getType()     { return type;     }

    /**
     * Return generated authentication headers for the current username and password.
     *
     * @param authstr The authentication string from which the headers are generated
     * @param url The URL to authenticate against
     * @param hashProvider An object that provides a hashing function in order to hash the response
     * 
     * @return The authentication response headers
     */
    virtual StringBuffer getAuthenticationHeaders(const char* authStr, URL url, const HashProvider *hashProvider) = 0;

    /**
     * Return generated authentication headers for the current username and password.
     * Just calls getAuthenticationHeaders() with no params.
	 * @see getAuthenticationHeaders(const char*, URL, const HashProvider*)
     */
    virtual StringBuffer getAuthenticationHeaders() {
        return getAuthenticationHeaders(NULL, NULL, NULL);
    }
    
    
protected:
    
    /// The authentication type, one of enum AuthType.
    AuthType type;
    
    /// The username of the client to authenticate
    StringBuffer username;
    
    /// The password of the client to authenticate
    StringBuffer password;
    
};

END_NAMESPACE
#endif
