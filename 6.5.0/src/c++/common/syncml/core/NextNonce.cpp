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

#include "syncml/core/NextNonce.h"

NextNonce::NextNonce() {
    initialize();
}

NextNonce::NextNonce(void* value, unsigned long size) {
    initialize();
    setValue(value, size);
}

NextNonce::NextNonce(char* wvalue) {
    initialize();
    setWValue(wvalue);
}

NextNonce::~NextNonce() {
    if (wvalue) {
        delete [] wvalue; wvalue = NULL;
    }
    if (value) {
        delete [] value; value = NULL;
    }
    size = -1;
}

void NextNonce::initialize() {
    wvalue = NULL;
    value = NULL;
    size = -1;
}

void* NextNonce::setValue(void* argValue, unsigned long argSize) {
     if (value) {
        delete  value; value = NULL;
    }
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
    if (wvalue) {
        delete [] wvalue; wvalue = NULL;
    }
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
