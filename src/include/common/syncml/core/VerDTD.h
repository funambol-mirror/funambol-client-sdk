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


#ifndef INCL_VER_DTD
#define INCL_VER_DTD
/** @cond DEV */

#include "base/fscapi.h"


class VerDTD {

     // ------------------------------------------------------------ Private data
    private:
         char*  value;

    public:

    // ------------------------------------------------------------ Constructors

    /**
     * In order to expose the server configuration like WS this constructor
     * must be public
     */
    VerDTD();
    ~VerDTD();

    /**
     * Creates a new VerDTD object with the given value
     *
     * @param value the version - NOT NULL
     *
     */
    VerDTD(const char*  value);

    // ---------------------------------------------------------- Public methods

    /**
     * Gets value properties
     *
     * @return value properties
     */
    const char* getValue();

    /**
     * Sets the version of DTD
     *
     * @param value the version of DTD
     */
    void setValue(const char* value);

    VerDTD* clone();

};

/** @endcond */
#endif
