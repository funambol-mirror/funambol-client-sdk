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

#ifndef INCL_TARGET
#define INCL_TARGET
/** @cond DEV */

#include "base/fscapi.h"


class Filter;  // forward declaration

class Target {

    // ------------------------------------------------------------ Private data
    private:
        char*  locURI;
        char*  locName;
        Filter*  filter;

        void set(const char*  locURI, const char*  locName, const Filter* filter);

    // ---------------------------------------------------------- Protected data
    public:


     /**
     * Creates a new Target object with the given locURI and locName
     *
     * @param locURI the locURI - NOT NULL
     * @param locName the locName - NULL
     * @param filter a filter to be applied for this target; it defaults to NULL
     *
     */
    Target(const char*  locURI, const char*  locName, const Filter* filter = NULL);

    /**
     * Creates a new Target object with the given locURI
     *
     * @param locURI the locURI - NOT NULL
     *
     */
    Target(const char*  locURI);

    Target();
    ~Target();


    // ---------------------------------------------------------- Public methods

    /** Gets locURI properties
     * @return locURI properties
     */
    const char*  getLocURI();

    /**
     * Sets locURI property
     * @param locURI the locURI
     */
    void setLocURI(const char*  locURI);

    /**
     * Gets locName properties
     * @return locName properties
     */
    const char*  getLocName();

    /**
     * Sets locName property
     * @param locName the locURI
     */
    void setLocName(const char*  locName);

    /**
     * Gets filter
     *
     * @return  the current filter's value
     *
     */
    Filter* getFilter();

    /**
     * Sets filter
     *
     * @param filter the new value
     *
     */
    void setFilter(Filter* filter);


    Target* clone();



};

/** @endcond */
#endif
