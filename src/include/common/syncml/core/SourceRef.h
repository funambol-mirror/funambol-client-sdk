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


#ifndef INCL_SOURCE_REF
#define INCL_SOURCE_REF
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Source.h"


class SourceRef : public ArrayElement{

     // ------------------------------------------------------------ Private data
    private:
         char*  value;
         Source*  source;

    // ---------------------------------------------------------- Protected data
    public:
        SourceRef();
        ~SourceRef();

        /**
         * Creates a new SourceRef object given the referenced value. A null value
         * is considered an empty string
         *
         * @param value the referenced value - NULL ALLOWED
         *
         */
        SourceRef(const char*  value);

        /**
         * Creates a new SourceRef object from an existing Source.
         *
         * @param source the source to extract the reference from - NOT NULL
         *
         *
         */
        SourceRef(Source* source);

        // ---------------------------------------------------------- Public methods

        /**
         * Returns the value
         *
         * @return the value
         */
        const char*  getValue();

        /**
         * Sets the reference value. If value is null, the empty string is adopted.
         *
         * @param value the reference value - NULL
         */
        void setValue(const char*  value);

        /**
         * Gets the Source property
         *
         * @return source the Source object property
         */
        Source* getSource();

        /**
         * Sets the Source property
         *
         * @param source the Source object property - NOT NULL
         */
        void setSource(Source* source);

        ArrayElement* clone();

};

/** @endcond */
#endif
