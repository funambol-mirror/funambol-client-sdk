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

#include "StringElement.h"

StringElement::StringElement(WCHAR* v) {
    value = (v) ? stringdup(v) : NULL;
}

StringElement::~StringElement() {
    if (value) {
        delete [] value; value = NULL;
    }
}

void StringElement::setValue(WCHAR* v) {
    if (value) {
        delete [] value;
    }
    value = (v) ? stringdup(v) : NULL;
}

WCHAR* StringElement::getValue(WCHAR* buf, int size) {

    if (buf == NULL) {
        return value;
    }

    if (size >= 0) {
        wcsncpy(buf, value, size);
    }
    else {
        wcscpy(buf, value);
    }

    return buf;
}

ArrayElement* StringElement::clone() {

    StringElement* clone = new StringElement(value);
    return (ArrayElement*)clone;

    return NULL;
}