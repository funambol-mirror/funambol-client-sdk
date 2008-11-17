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

#include "base/fscapi.h"
#include "syncml/core/Target.h"
#include "syncml/core/Filter.h" // note: do not remove

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


