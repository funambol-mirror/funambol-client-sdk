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


#include "syncml/core/Mem.h"

Mem::Mem(BOOL sharedMem, long freeMem, long freeID) {
    this->freeMem = 0;
    this->freeID  = 0;
    this->sharedMem = NULL;

    setFreeMem(freeMem);
    setFreeID(freeID);
    if ((sharedMem == NULL) || (sharedMem != TRUE && sharedMem != FALSE)) {
        this->sharedMem = NULL;
    } else {
        this->sharedMem = sharedMem;
    }

}

Mem::~Mem() {}

BOOL Mem::isSharedMem() {
    return (sharedMem != NULL);
}

/**
 * Sets the memoryShared status
 *
 * @param sharedMem the new memoryShared status
 */
void Mem::setSharedMem(BOOL sharedMem) {
    if ((sharedMem == NULL) || (sharedMem != TRUE && sharedMem != FALSE)) {
        this->sharedMem = NULL;
    } else {
        this->sharedMem = sharedMem;
    }
}

/**
 * Gets the Boolean shared memory property
 *
 * @return sharedMem the Boolean shared memory property
 */
BOOL Mem::getSharedMem() {

    return sharedMem;
}

/**
 * Returns the freeMem property (in bytes)
 *
 * @return the freeMem property
 *
 */
long Mem::getFreeMem() {
    return freeMem;
}

/**
 * Sets the freeMem property.
 *
 * @param freeMem the freeMem value (>= 0)
 *
 */
void Mem::setFreeMem(long freeMem) {
    if (freeMem < 0) {
        // tbd
    }
    this->freeMem = freeMem;
}

/**
 * Returns the number of available item IDs (>= 0)
 *
 * @return the number of available item IDs (>= 0)
 *
 */
long Mem::getFreeID() {
    return freeID;
}

/**
 * Sets the freeID property.
 *
 * @param freeID the freeIDCount value (>= 0)
 *
 */
void Mem::setFreeID(long freeID) {
    if (freeID < 0) {
        // tbd
    }
    this->freeID = freeID;
}

Mem* Mem::clone() {
    Mem* ret = NULL;
    if (this) {
        ret = new Mem(sharedMem, freeMem, freeID);
    }
    return ret;

}
