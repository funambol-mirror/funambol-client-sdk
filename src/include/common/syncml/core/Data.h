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


#ifndef INCL_DATA
#define INCL_DATA
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/Constants.h"


class Data {

     // ---------------------------------------------------------- Protected data
    protected:
        char*  data;
        void initialize();

    // ---------------------------------------------------------- Protected data
    public:

    Data();
    ~Data();

    /**
     * Creates a new Data object with the given data value
     *
     * @param data the data value
     *
     */
    Data(const char*  data);

    /**
     * Creates a new Data object with the given data value
     *
     * @param data the data value
     *
     */
    Data(long data);

    // ---------------------------------------------------------- Public methods

    /**
     * Sets the data property
     *
     * @param data the data property
     */
    void setData(const char*  data);

    /**
     * Gets the data properties
     *
     * @return the data properties
     */
    const char* getData();

    Data* clone();
};

/** @endcond */
#endif
