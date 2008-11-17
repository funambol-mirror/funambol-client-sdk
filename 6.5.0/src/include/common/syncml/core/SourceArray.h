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


#ifndef INCL_SOURCE_ARRAY
#define INCL_SOURCE_ARRAY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Source.h"


/*
* Class used to create an ArrayList of Source. A Source object is not derived from ArrayElement
* so this wrapper class is used to obtain it.
*/
class SourceArray : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:

        Source* source;

    // ---------------------------------------------------------- Protected data
    public:

        SourceArray();
        ~SourceArray();

        /**
         * Creates a new SourceArray object given the source object
         *
         */
        SourceArray(Source* source);


        // ------------------------------------------------------ Public methods

        /**
         * Returns the source
         *
         * @return the source
         */
        Source* getSource();

        /**
         * Sets the source
         *
         * @param source the source
         *
         */
        void setSource(Source* source);

        ArrayElement* clone();

};

/** @endcond */
#endif
