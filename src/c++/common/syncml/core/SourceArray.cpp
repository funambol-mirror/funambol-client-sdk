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


#include "syncml/core/SourceArray.h"


SourceArray::SourceArray() {
    source = NULL;
}
SourceArray::~SourceArray() {
    if (source) {
        delete source; source = NULL;
    }
}

/**
 * Creates a new SourceArray object given the source
 *
 * @param source the source
 *
 */
SourceArray::SourceArray(Source* source) {
    this->source  = NULL;
    setSource(source);
}


/**
 * Returns the source
 *
 * @return the source
 */
Source* SourceArray::getSource() {
        return source;
}

/**
 * Sets the source
 *
 * @param source the source
 *
 */
void SourceArray::setSource(Source* source) {

    if (this->source) {
        delete this->source; this->source = NULL;
    }
    if (source) {
        this->source = source->clone();
    }
}

ArrayElement* SourceArray::clone() {

    SourceArray* ret = new SourceArray(source);
    return ret;
}
