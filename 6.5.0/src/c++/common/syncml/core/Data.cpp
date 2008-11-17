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
#include "syncml/core/Data.h"


Data::Data() {
    initialize();
}

Data::~Data() {
    if (data) {
        delete [] data; data = NULL;
    }
}

/**
* Creates a new Data object with the given data value
*
* @param data the data value
*
*/
Data::Data(const char* data) {
    initialize();
    setData(data);
}

void Data::initialize() {
    data = NULL;
}

/**
* Creates a new Data object with the given data value
*
* @param data the data value
*
*/
Data::Data(long data) {
    initialize();
    char tmp[DIM_64];
    sprintf(tmp, "%i", data);
    setData(tmp);
}

// ---------------------------------------------------------- Public methods

/**
* Sets the data property
*
* @param data the data property
*/
void Data::setData(const char* data) {
    if (this->data) {
        delete [] this->data; this->data = NULL;
    }

    this->data = stringdup(data);
}

/**
* Gets the data properties
*
* @return the data properties
*/
const char* Data::getData() {
    return data;
}

Data* Data::clone() {
    Data* ret = new Data(data);
    return ret;
}
