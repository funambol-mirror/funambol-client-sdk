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
#include "syncml/core/EMI.h"


EMI::EMI(char* value) {
    this->value = NULL;
    setValue(value);
}

EMI::~EMI() {
    if (value) {
        delete [] value; value = NULL;
    }
}


/**
 * Gets the value of experimental meta information
 *
 * @return the value of experimental meta information
 */
const char* EMI::getValue() {
    return value;
}

/**
 * Sets the value of experimental meta information
 *
 * @param value the value of experimental meta information
 *
 */
void EMI::setValue(const char*value) {
    if (value == NULL || strlen(value) == 0) {
        // tbd
    }
    this->value = stringdup(value);
}

ArrayElement* EMI::clone() {
    EMI* ret = NULL;
    if (this) {
        ret = new EMI(value);
    }
    return ret;
}
