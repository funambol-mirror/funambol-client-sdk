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

#ifndef INCL_TARGET
#define INCL_TARGET
/** @cond DEV */

#include "base/fscapi.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


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


END_NAMESPACE

/** @endcond */
#endif
