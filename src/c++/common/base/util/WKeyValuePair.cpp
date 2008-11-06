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
#include "base/util/WKeyValuePair.h"

WKeyValuePair::WKeyValuePair(const WCHAR* key, const WCHAR* value) {
    k = (key  ) ? wstrdup(key  ) : NULL;
    v = (value) ? wstrdup(value) : NULL;
}

WKeyValuePair::~WKeyValuePair() {
    if (k) delete [] k; k = NULL;
    if (v) delete [] v; v = NULL;
}

void WKeyValuePair::setKey(const WCHAR* key) {
    if (k) delete[] k; k = NULL;

    k = (key) ? wstrdup(key) : NULL;
}

const WCHAR* WKeyValuePair::getKey() {
    return k;
}

const WCHAR* WKeyValuePair::getValue() {
    return v;
}

void WKeyValuePair::setValue(const WCHAR* value) {
    if (v) delete[] v; v = NULL;

    k = (value) ? wstrdup(value) : NULL;
}

ArrayElement* WKeyValuePair::clone() {
    return new WKeyValuePair(k, v);
}
