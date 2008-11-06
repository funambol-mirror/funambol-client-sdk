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


#ifndef INCL_PIM_TYPED_PROPERTY
#define INCL_PIM_TYPED_PROPERTY
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "vocl/vCard/vCardProperty.h"

/**
 * An association between a property and a string representing the property type.
 *
 */

class TypedProperty : public ArrayElement {
    // ------------------------------------------------------------ Private data

    protected:
        vCardProperty* p;
        WCHAR*  t;

    // -------------------------------------------- Constructors and Destructors
    public:

        /**
         * Creates an empty TypedProperty
         */
        TypedProperty();
        virtual ~TypedProperty();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the property
         *
         * @return this property content
         */
        vCardProperty* getProperty();

        /**
         * Sets the property content. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setProperty(vCardProperty& p);

        /**
         * Returns the property type
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the property type for this property
         */
        WCHAR* getType(WCHAR* buf = NULL, int size = -1);

        /**
         * Sets the property type
         *
         * @param type the property type
         */
        void setType(WCHAR* type);

        /**
         * Creates a new instance of TypedProperty from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone() { return NULL;}; //FIXME
};

/** @endcond */
#endif
