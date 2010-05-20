/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "syncml/core/NextNonce.h"
#include "base/globalsdef.h"

USE_NAMESPACE

NextNonce::NextNonce() {
    initialize();
}

NextNonce::NextNonce(const void* value, unsigned long size) {
    initialize();
    setValue(value, size);
}

NextNonce::NextNonce(const char* wvalue) {
    initialize();
    setWValue(wvalue);
}

NextNonce::~NextNonce() {
    delete [] wvalue;
    delete [] value;
    size = -1;
}

void NextNonce::initialize() {
    wvalue = NULL;
    value = NULL;
    size = -1;
}

void* NextNonce::setValue(const void* argValue, unsigned long argSize) {
    delete  value;
    value = NULL;
    if (argValue == NULL) {
        size = 0;
        return NULL;
    }

    value = new char[argSize];
    if (value == NULL) {
        return NULL;
    }

    size = argSize;
    memcpy(value, argValue, size);

    return value;

}

void* NextNonce::getValue() {
    return value;
}

long NextNonce::getValueSize() {
    return size;
}

void NextNonce::setWValue(const char*wnonce) {
    delete [] wvalue;
    wvalue = stringdup(wnonce);

    if (wnonce) {
        unsigned long len = 0;
        len = strlen(wnonce);
        char* b64tmp = new char[len];
        len = b64_decode(b64tmp, wnonce);

        setValue(b64tmp, len);

        delete [] b64tmp; b64tmp = NULL;
    }

}



const char* NextNonce::getValueAsBase64() {

    if (value == NULL)
        return NULL;

    char* b64Cred = NULL;
    int c = ((size/3+1)<<2) + 1;
    unsigned int len = 0;

    b64Cred = new char[c];
    len = b64_encode(b64Cred, value, size);
    b64Cred[len] = 0;

    return b64Cred;

}

NextNonce* NextNonce::clone() {
    NextNonce* ret = NULL;
    if (this) {
        ret = new NextNonce(value, size);
    }
    return ret;
}

