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
#include "vocl/vCard/TypedProperty.h"


TypedProperty::TypedProperty() {
    p = NULL;
    t = NULL;
}

TypedProperty::~TypedProperty() {
    if (p) {
        delete p; p = NULL;
    }
    if (t) {
        delete [] t;
        t = NULL;
    }
}

vCardProperty* TypedProperty::getProperty() {
    return p;
}

void TypedProperty::setProperty(vCardProperty& prop) {
    if (p) delete p;

    p = prop.clone();
}

WCHAR* TypedProperty::getType(WCHAR* buf, int size) {
    if (buf == NULL) {
        return t;
    }

    if (size >= 0) {
        wcsncpy(buf, t, size);
    } else {
        wcscpy(buf, t);
    }

    return buf;
}

void TypedProperty::setType(WCHAR* type) {
    if (t) delete [] t;
    t = wstrdup(type);
}
