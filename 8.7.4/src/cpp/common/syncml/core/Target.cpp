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

#include "base/fscapi.h"
#include "syncml/core/Target.h"
#include "syncml/core/Filter.h" // note: do not remove
#include "base/globalsdef.h"

USE_NAMESPACE

Target::Target() : locURI(NULL), locName(NULL), filter(NULL) {
}

Target::~Target() {
    if (locURI) {
        delete [] locURI; locURI = NULL;
    }

    if (locName) {
        delete [] locName; locName = NULL;
    }

    if (filter) {
        delete filter;
    }
}
/**
 * Creates a new Target object with the given locURI and locName
 *
 * @param locURI the locURI - NOT NULL
 * @param locName the locName - NULL
 * @param filter a filter to be applied for this target; it defaults to NULL
 *
 */
Target::Target(const char* locURI, const char* locName, const Filter* filter)
    : locURI(NULL), locName(NULL), filter(NULL) {
    set(locURI, locName, filter);
}

/**
 * Creates a new Target object with the given locURI
 *
 * @param locURI the locURI - NOT NULL
 *
 */
Target::Target(const char* locURI) : locURI(NULL), locName(NULL), filter(NULL) {
    set(locURI, NULL, NULL);
}

void Target::set(const char* locURI, const char* locName, const Filter* filter) {
    setLocURI(locURI);
    setLocName(locName);
    setFilter((Filter*)filter);
}


// ---------------------------------------------------------- Public methods

/** Gets locURI properties
 * @return locURI properties
 */
const char* Target::getLocURI() {
    return locURI;
}

/**
 * Sets locURI property
 * @param locURI the locURI
 */
void Target::setLocURI(const char* locURI) {
    if (locURI == NULL) {
        // TBD
    }
    if (this->locURI) {
        delete [] this->locURI; this->locURI = NULL;
    }
    this->locURI = stringdup(locURI);
}

/**
 * Gets locName properties
 * @return locName properties
 */
const char* Target::getLocName() {
    return locName;
}

/**
 * Sets locName property
 * @param locName the locURI
 */
void Target::setLocName(const char* locName) {
    if (this->locName ) {
        delete [] this->locName ; this->locName  = NULL;
    }
    this->locName = stringdup(locName);
}


Target* Target::clone() {
    return new Target(locURI, locName, filter);
}

/*
 * Gets filter
 *
 * @return  the current filter's value
 *
 */
Filter* Target::getFilter() {
    return filter;
}

/*
 * Sets filter
 *
 * @param filter the new value
 *
 */
void Target::setFilter(Filter* filter) {
    if (this->filter) {
        delete this->filter; this->filter = NULL;
    }

    if (filter) {
        this->filter = filter->clone();
    }
}


