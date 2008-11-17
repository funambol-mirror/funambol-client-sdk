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
#include "syncml/core/VerDTD.h"


VerDTD::VerDTD() {
    value = NULL;
}

VerDTD::~VerDTD() {
    if (value) {
        delete [] value; value = NULL;
    }
}

/**
 * Creates a new VerDTD object with the given value
 *
 * @param value the version - NOT NULL
 *
 */
VerDTD::VerDTD(const char* value) {
    this->value = stringdup(value);
}

// ---------------------------------------------------------- Public methods

/**
 * Gets value properties
 *
 * @return value properties
 */
const char* VerDTD::getValue() {
    return value;
}

/**
 * Sets the version of DTD
 *
 * @param value the version of DTD
 */
void VerDTD::setValue(const char*value) {
    if (this->value) {
        delete [] this->value; this->value = NULL;
    }
    this->value = stringdup(value);
}

VerDTD* VerDTD::clone() {
    VerDTD* ret = new VerDTD(value);
    return ret;

}
