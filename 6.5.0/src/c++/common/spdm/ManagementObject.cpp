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


#include "base/util/KeyValuePair.h"
#include "spdm/ManagementObject.h"


ManagementObject::ManagementObject( const WCHAR* context,
                                    const WCHAR* name   )
: LeafManagementNode (context, name) {
}

ManagementObject::~ManagementObject() {
}

void ManagementObject::getPropertyValue(const WCHAR* p, WCHAR*v, int size) {
    KeyValuePair* property = NULL;

    int l = properties.size();
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties.get(i);
        if (wcscmp(property->getKey(), p) == 0) {
            wcsncpy(v, property->getValue(), size);
            return;
        }
    }

}

void ManagementObject::setPropertyValue(const WCHAR* p, const WCHAR*v) {
    KeyValuePair property((WCHAR*)p, v);

    properties.add(property);
}

ArrayElement* ManagementObject::clone() {
    ManagementObject* ret = new ManagementObject(context, name);

    KeyValuePair* property = NULL;

    int l = properties.size();
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties.get(i);
        ret->setPropertyValue(property->getKey(), property->getValue());
    }

    return ret;
}


ArrayList& ManagementObject::getProperties() {
    return properties;
}
