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
#include "syncml/core/Source.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Source::Source() {
    locName = NULL;
    locURI  = NULL;
}
Source::~Source() {
    if (locURI) {
        delete [] locURI; locURI = NULL;
    }

    if (locName) {
        delete [] locName; locName = NULL;
    }
}

/**
 * Creates a new Source object given its URI and display name.
 *
 * @param locURI the source URI - NOT NULL
 * @param locName the source display name - NULL ALLOWED
 *
 */
Source::Source(const char* locURI, const char* locName) {
    this->locName  = NULL;
    this->locURI   = NULL;
    set(locURI, locName);
}

/**
 * Creates a new Source object given its URI
 *
 * @param locURI the source URI - NOT NULL
 *
 */
Source::Source(const char* locURI) {
    this->locName  = NULL;
    this->locURI   = NULL;
    set(locURI, NULL);
}


void Source::set(const char* locURI, const char* locName) {
    setLocURI(locURI);
    if (this->locName) {
        delete [] this->locName; this->locName = NULL;

    }
    this->locName = stringdup(locName);

}


/**
 * Returns the source URI value
 *
 * @return the source URI value
 */
const char* Source::getLocURI() {
        return locURI;
    }

/**
 * Sets the source URI
 *
 * @param locURI the source URI - NOT NULL
 *
 */
void Source::setLocURI(const char* locURI) {
    if (locURI == NULL) {
        // TBD
    }
    if (this->locURI) {
        delete [] this->locURI; this->locURI = NULL;
    }
    this->locURI = stringdup(locURI);
}

/**
 * Returns the source display name
 *
 * @return the source display name
 *
 */
const char* Source::getLocName() {
        return locName;
    }

/**
 * Sets the local name property
 *
 * @param locName the local name property
 *
 */
void Source::setLocName(const char* locName) {
    if (this->locName ) {
        delete [] this->locName ; this->locName  = NULL;
    }
    this->locName = stringdup(locName);
}


Source* Source::clone() {

    Source* ret = new Source(locURI, locName);
    return ret;
}
