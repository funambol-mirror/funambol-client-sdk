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

/**
* This class represent a String class used to store a single element in a value.
* It can permit to store a string value and set it into an ArrayList.
*
*/

#include "base/util/utils.h"
#include "syncml/core/StringElement.h"

StringElement::StringElement(const char* value) {
    this->value = NULL;
    setValue(value);
}

StringElement::~StringElement() {
    if (value) {
        delete [] value; value = NULL;
    }
}


/**
 * Gets the value of string element
 *
 * @return the value of string element
 */
const char* StringElement::getValue() {
    return value;
}

/**
 * Sets the value of string element
 *
 * @param value the value of string element
 *
 */
void StringElement::setValue(const char* value) {
    if (value) {
        delete [] this->value; this->value = NULL;
    }
    this->value = stringdup(value);
}

ArrayElement* StringElement::clone() {
    StringElement* ret = new StringElement(value);
    return ret;
}
