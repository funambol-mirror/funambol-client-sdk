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


/**
* This class represent a String class used to store a single element in a value.
* It can permit to store a string value and set it into an ArrayList.
*
*/

#ifndef INCL_STRING_ELEMENT
#define INCL_STRING_ELEMENT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

class StringElement : public ArrayElement {


     // ------------------------------------------------------------ Private data
    private:

        char*  value;
    // ---------------------------------------------------------- Protected data
    public:

        StringElement(const char*  value);
        ~StringElement();


        // ---------------------------------------------------------- Public methods
        /**
         * Gets the value of string element
         *
         * @return the value of string element
         */
        const char* getValue();


        /**
         * Sets the value of experimental meta information
         *
         * @param value the value of experimental meta information
         *
         */
        void setValue(const char*  value);

        ArrayElement* clone();

};

/** @endcond */
#endif
