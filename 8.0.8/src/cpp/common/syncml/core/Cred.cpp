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


#include "syncml/core/Cred.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Cred::Cred() {
    authentication = NULL;
}

Cred::~Cred() {
    if (authentication) {
        delete authentication; authentication = NULL;
    }
}

Cred::Cred(Authentication* auth) {
    authentication = NULL;
    if (auth == NULL) {
        // TBD
    }
    this->authentication = auth->clone();

}

/**
 * Gets type property
 *
 * @return type property
 */
const char* Cred::getType() {
    return authentication ? authentication->getType() : NULL;
}

/**
 * Gets format property
 *
 * @return format property
 */
const char* Cred::getFormat() {
    return authentication ? authentication->getFormat() : NULL;
}

/**
 * Gets data property
 *
 * @return data property
 */
const char* Cred::getData() {
    return authentication ? authentication->getData() : NULL;
}

/**
 * Gets the username stored in this credential
 *
 * @return the username stored in this credential
 */
const char* Cred::getUsername() {
    return authentication ? authentication->getUsername() : NULL;
}


/**
 * Create and return the Authentication object corresponding to the given
 * type and data.
 *
 * @param type the type of the required Authentication object
 * @param data the data to be interpreted based on the type
 *
 * @return the corresponding Authentication object.
 */
Authentication* Cred::createAuthentication(char* data, char* type) {
    return new Authentication(data, type);
}

/**
 * Gets the Authentication object.
 *
 * @return authentication the authentication objects
 */
Authentication* Cred::getAuthentication() {
    return authentication;
}

/**
 * Sets the Authentication object.
 *
 * @param auth the new Authentication object
 *
 */
void Cred::setAuthentication(Authentication* auth) {
    if (this->authentication) {
        delete  this->authentication; this->authentication = NULL;
    }
    this->authentication = auth->clone();
}

Cred* Cred::clone() {
    Cred* ret = new Cred(authentication);
    return ret;
}
