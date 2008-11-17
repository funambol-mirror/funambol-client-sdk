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

#include "base/util/utils.h"
#include "syncml/core/VerProto.h"


VerProto::VerProto() {
    version = NULL;
}
VerProto::~VerProto() {
    if (version) {
        delete [] version; version = NULL;
    }
}

/**
 * Creates a new VerProto object from its version.
 *
 * @param version the protocol version - NOT NULL
 *
 */
VerProto::VerProto(const char* version) {
    this->version = NULL;
    setVersion(version);
}

/**
 * Returns the protocol version.
 *
 * @return the protocol version - NOT NULL
 *
 */
const char* VerProto::getVersion() {
    return version;
}

/**
 * Sets the protol version.
 *
 * @param version the protocol version - NOT NULL
 *
 */
void VerProto::setVersion(const char*version) {

    if (version == NULL) {
        // TBD
    }
    if (this->version) {
        delete [] this->version; this->version = NULL;
    }
    this->version = stringdup(version);
}

VerProto* VerProto::clone() {
    VerProto* ret = new VerProto(version);
    return ret;
}
