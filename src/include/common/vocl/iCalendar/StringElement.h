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


#ifndef INCL_STRING_ELEMENT
#define INCL_STRING_ELEMENT
/** @cond DEV */

#include "ArrayElement.h"
#include <string.h>

class StringElement : public ArrayElement {

private:
    WCHAR* value;

public:
    StringElement(WCHAR* v = NULL);
    ~StringElement();
    ArrayElement* clone();
    void setValue (WCHAR* value);
    WCHAR* getValue(WCHAR* buf = NULL, int size = -1);
};
/** @endcond */
#endif