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


#include "syncml/core/Chal.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Chal::Chal() {
    initialize();
    meta = NULL;
}

Chal::~Chal() {
    if (meta) {
        delete meta; meta = NULL;
    }
}

Chal::Chal(Meta* meta) {
    initialize();
    this->meta = meta->clone();

    //
    // type and format are pointers to meta.type and meta.format
    // fields. They are fred in the destructor
    //
    const char* type         = meta->getType();
    const char* format       = meta->getFormat();

    if (type == NULL) {
        // TBD
    }
    if (format == NULL) {
        if (wcscmpIgnoreCase(type, AUTH_TYPE_BASIC)) {
            meta->setFormat(FORMAT_B64);
        } else if (wcscmpIgnoreCase(type, AUTH_TYPE_MD5)) {
            meta->setFormat(FORMAT_B64);
        } else {
            // TBD
        }
    }
}

void Chal::initialize() {
    meta = NULL;
}

// ---------------------------------------------------------- Public methods
/**
 * Gets the Meta property
 *
 * @return meta the Meta property
 */
Meta* Chal::getMeta() {
    return this->meta;
}

/**
 * Sets the Meta property
 *
 * @param meta the Meta property
 *
 */
void Chal::setMeta(Meta* meta) {
    if (this->meta) {
        delete this->meta; this->meta = NULL;
    }
    this->meta = meta->clone();
}

/**
 * Returns the nextNonce property or null
 *
 *  @return the nextNonce property or null
 */
NextNonce* Chal::getNextNonce() {
    return meta->getNextNonce();
}

void Chal::setNextNonce(NextNonce* nextNonce) {
    if (meta == NULL) {
        meta = new Meta();
    }
    meta->setNextNonce(nextNonce);
}
/**
 * Returns the authentication type
 *
 * @return authentication type.
 */
const char* Chal::getType() {
    return meta->getType();
}

/**
 * Returns the authentication format
 *
 * @return format the authentication format
 */
const char* Chal::getFormat() {
    return meta->getFormat();
}

/**
 * Creates a basic authentication challange.
 * This will have type = Cred.AUTH_TYPE_BASIC and
 * format = Constants.FORMAT_B64
 *
 * @return the newly created AuthenticationChallange
 */
Chal* Chal::getBasicChal() {
    Meta* m = new Meta();
    m->setType(AUTH_TYPE_BASIC);
    m->setFormat(FORMAT_B64);
    m->setNextNonce(NULL);
    return new Chal(m);
}

/**
 * Creates a MD5 authentication challange.
 * This will have type = Cred.AUTH_TYPE_MD5 and
 * format = Constants.FORMAT_B64
 *
 * @return the newly created AuthenticationChallange
 */
Chal* Chal::getMD5Chal() {
    Meta* m = new Meta();
    m->setType(AUTH_TYPE_MD5);
    m->setFormat(FORMAT_B64);
    m->setNextNonce(NULL);
    return new Chal(m);
}

Chal* Chal::clone() {
    Chal* ret = new Chal(meta);
    return ret;
}

