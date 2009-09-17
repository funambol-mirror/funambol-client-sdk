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



#include "base/util/utils.h"
#include "spds/SyncMap.h"
#include "base/globalsdef.h"

USE_NAMESPACE


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
