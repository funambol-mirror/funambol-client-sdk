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


#include "syncml/core/Chal.h"

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

