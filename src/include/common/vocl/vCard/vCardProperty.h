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


//
// @author Stefano Fornari @ Funambol
// @version $Id: vCardProperty.h,v 1.6 2007-06-06 08:36:11 mvitolo Exp $
//
#include <string.h>

#ifndef INCL_PIM_PROPERTY
#define INCL_PIM_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"

/**
 * This object represents a property for VCard and ICalendar object
 * (i.e. its value and its parameters)
 */
class vCardProperty {

    // ------------------------------------------------------------ Private data

    private:
        WCHAR* encoding     ;
        WCHAR* language     ;
        WCHAR* value        ;
        WCHAR* chrset       ;

        /**
         * Sets internal members releasing the currently allocated memory (if
         * any was allocate). The passed value is duplicated so that the caller
         * can independently release it.
         *
         * @param property the address of the pointer to set to the new
         *                 allocated memory
         * @param v the value to set into the property
         */
        void set(WCHAR** property, WCHAR* v);

    // -------------------------------------------- Constructors and Destructors
    public:
        /**
         * Creates property without parameters but with the specified value
         */
        vCardProperty (WCHAR* v = NULL);

        ~vCardProperty();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the encoding parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the encoding parameter of this property
         */
        WCHAR* getEncoding (WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the language parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the language parameter of this property
         */
        WCHAR* getLanguage (WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the value parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the value parameter of this property
         */
        WCHAR* getValue (WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the charset parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the charset parameter of this property
         */
        WCHAR* getCharset (WCHAR* buf = NULL, int size = -1);

        /**
         * Sets the encoding parameter of this property
         *
         * @param encoding the encoding to set
         */
        void setEncoding (WCHAR* encoding);

        /**
         * Sets the language parameter of this property
         *
         * @param language the language to set
         */
        void setLanguage (WCHAR* language);

        /**
         * Sets the value parameter of this property
         *
         * @param value the value to set
         */
        void setValue (WCHAR* value);

        /**
         * Sets the charset parameter of this property
         *
         * @param chrset the charset to set
         */
        void setCharset (WCHAR* chrset);


        /**
         * Creates and returns a new Property object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        vCardProperty* clone();
};

/** @endcond */
#endif
