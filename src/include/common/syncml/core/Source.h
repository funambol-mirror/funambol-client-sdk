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


#ifndef INCL_SOURCE
#define INCL_SOURCE
/** @cond DEV */

#include "base/fscapi.h"

class Source {

     // ------------------------------------------------------------ Private data
    private:
        char*  locURI;
        char*  locName;

        void set(const char*  locURI, const char*  locName);

    // ---------------------------------------------------------- Protected data
    public:

        Source();
        ~Source();

        /**
         * Creates a new Source object given its URI and display name.
         *
         * @param locURI the source URI - NOT NULL
         * @param locName the source display name - NULL ALLOWED
         *
         */
        Source(const char*  locURI, const char*  locName);

        /**
         * Creates a new Source object given its URI
         *
         * @param locURI the source URI - NOT NULL
         *
         */
        Source(const char*  locURI);

        // ------------------------------------------------------ Public methods

        /**
         * Returns the source URI value
         *
         * @return the source URI value
         */
        const char*  getLocURI();

        /**
         * Sets the source URI
         *
         * @param locURI the source URI - NOT NULL
         *
         */
        void setLocURI(const char*  locURI);

        /**
         * Returns the source display name
         *
         * @return the source display name
         *
         */
        const char*  getLocName();

        /**
         * Sets the local name property
         *
         * @param locName the local name property
         *
         */
        void setLocName(const char*  locName);

        Source* clone();

};

/** @endcond */
#endif
