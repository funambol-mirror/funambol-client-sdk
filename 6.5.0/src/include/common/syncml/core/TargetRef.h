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


#ifndef INCL_TARGET_REF
#define INCL_TARGET_REF
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Target.h"



class TargetRef : public ArrayElement{

     // ------------------------------------------------------------ Private data
    private:
        char*  value;
        char*  query;
        Target* target;

        void initialize();

        /*
        * Used to set the query field in the clone method
        */
        void setQuery(const char* val);

    // ---------------------------------------------------------- Protected data
    public:

        TargetRef();
        ~TargetRef();

         /**
         * Creates a new TargetRef object given the referenced value. A null value
         * is considered an empty string
         *
         * @param value the referenced value - NULL ALLOWED
         *
         */
        TargetRef(const char*  value);

        /**
         * Creates a new TargetRef object from an existing target.
         *
         * @param target the target to extract the reference from - NOT NULL
         *
         *
         */
        TargetRef(Target* target);

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
         * Gets the Target property
         *
         * @return target the Target property
         */
        Target* getTarget();

        /**
         * Sets the Target property
         *
         * @param target the Target property
         */
        void setTarget(Target* target);

        ArrayElement* clone();
};

/** @endcond */
#endif
