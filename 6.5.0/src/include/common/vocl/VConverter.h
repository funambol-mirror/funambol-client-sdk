/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_VIRTUAL_CONVERTER
#define INCL_VIRTUAL_CONVERTER
/** @cond DEV */

#include "VObject.h"

class VConverter{

public:
    static VObject* parse(const WCHAR* buffer);

private:
    static VProperty* readFieldHeader(WCHAR* buffer);
    static bool readFieldBody(WCHAR* buffer, VProperty* property);

    // Extract the parameter of certain properties, e.g. "BEGIN:" or "VERSION:".
    // The result is a pointer into buffCopy, which is expected to have
    // buffCopyLen wchars and will be reallocated if necessary.
    static WCHAR* extractObjectProperty(const WCHAR* buffer, const WCHAR *property,
                                          WCHAR * &buffCopy, size_t &buffCopyLen);

    // extractObjectType() and extractObjectVersion() contain static buffers,
    // copy the result before calling these functions again!
    static WCHAR* extractObjectType(const WCHAR* buffer);
    static WCHAR* extractObjectVersion(const WCHAR* buffer);
    static bool extractGroup(WCHAR* propertyName, WCHAR* propertyGroup);

};
/** @endcond */
#endif
