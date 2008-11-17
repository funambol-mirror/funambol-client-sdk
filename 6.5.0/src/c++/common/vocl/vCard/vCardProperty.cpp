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


#include "base/fscapi.h"
#include "base/util/utils.h"
#include "vocl/vCard/vCardProperty.h"


/**
 * This object represents a property for VCard and ICalendar object
 * (i.e. its value and its parameters)
 */

/**
 * Creates an empty property
 */
vCardProperty::vCardProperty (WCHAR* v) {
    encoding      = NULL;
    language      = NULL;
    chrset        = NULL;

    value  = (v) ? wstrdup(v) : NULL;
}

vCardProperty::~vCardProperty() {
    if (chrset) {
        delete [] chrset; chrset = NULL;
    }

    if (encoding) {
        delete [] encoding; encoding = NULL;
    }

    if (language) {
        delete [] language; language = NULL;
    }
    if (value) {
        delete [] value; value = NULL;
    }
}

WCHAR* vCardProperty::getEncoding (WCHAR* buf, int size) {
    if (buf == NULL) {
        return encoding;
    }

    if (size >= 0) {
        wcsncpy(buf, encoding, size);
    } else {
        wcscpy(buf, encoding);
    }

    return buf;
}

WCHAR* vCardProperty::getLanguage (WCHAR* buf, int size) {
    if (buf == NULL) {
        return language;
    }

    if (size >= 0) {
        wcsncpy(buf, language, size);
    } else {
        wcscpy(buf, language);
    }

    return buf;
}

WCHAR* vCardProperty::getValue (WCHAR* buf, int size) {
    if (buf == NULL) {
        return value;
    }

    if (size >= 0) {
        wcsncpy(buf, value, size);
    } else {
        wcscpy(buf, value);
    }

    return buf;
}

WCHAR* vCardProperty::getCharset (WCHAR* buf, int size) {
    if (buf == NULL) {
        return chrset;
    }

    if (size >= 0) {
        wcsncpy(buf, chrset, size);
    } else {
        wcscpy(buf, chrset);
    }

    return buf;
}

void vCardProperty::setEncoding (WCHAR* e) {
    set(&encoding, e);
}

void vCardProperty::setLanguage (WCHAR* l) {
    set(&language, l);
}

void vCardProperty::setValue (WCHAR* v) {
    set(&value, v);
}

void vCardProperty::setCharset (WCHAR* c) {
    set(&chrset, c);
}


void vCardProperty::set(WCHAR** property, WCHAR* v) {
    if (*property) {
        delete [] *property;
    }
    *property = (v) ? wstrdup(v) : NULL;
}

vCardProperty* vCardProperty::clone() {
    vCardProperty* ret = new vCardProperty();

    if (chrset)          ret->setCharset      (chrset       );
    if (encoding)        ret->setEncoding     (encoding     );
    if (language)        ret->setLanguage     (language     );
    if (value)           ret->setValue        (value        );

    return ret;
}
