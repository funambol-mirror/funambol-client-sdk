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
#include "spds/SyncMap.h"


SyncMap::SyncMap(const char *g, const char* l) {
    guid = stringdup(g);
    luid = stringdup(l);
}

SyncMap::~SyncMap() {
    if (guid) {
        delete [] guid;
    }

    if (luid) {
        delete [] luid;
    }
}

/*
 * Returns the luid of this mapping. If luid is NULL, the internal
 * buffer address is returned, otherwise the value is copied into the
 * given buffer and its pointer is returned.
 *
 * @param luid - the buffer where the luid is copied to. It must be
 *               big enough
 */
const char* SyncMap::getGUID() {
    return guid;
}


/*
 * Returns the luid of this mapping
 */
const char* SyncMap::getLUID() {
    return luid;
}

/**
 * Sets a new value for the GUID property. The value is copied in the
 * class internal memory.
 *
 * @param guid the new value
 */
void SyncMap::setGUID(const char* g) {
    if (guid) {
        delete [] guid;
    }

    if (g) {
        guid = stringdup(g);
    }
}

/**
 * Sets a new value for the LUID property. The value is copied in the
 * class internal memory.
 *
 * @param luid the new value
 */
void SyncMap::setLUID(const char* l) {
    if (luid) {
        delete [] luid;
    }

    if (l) {
        luid = stringdup(l);
    }
}

/**
 * Sets a new value for the LUID property (as unsigned int). It internally
 * calls setLUID(char*)
 *
 * @param luid the new value
 */
void SyncMap::setLUID(unsigned long l) {
    char ls[12];

    sprintf(ls, "%lu", l);
    setLUID(ls);
}

ArrayElement* SyncMap::clone() {
    SyncMap* ret = new SyncMap(this->getGUID(), this->getLUID());
    return ret;
}
