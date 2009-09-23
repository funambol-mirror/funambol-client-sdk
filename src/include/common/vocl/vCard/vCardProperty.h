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


//
// @author Stefano Fornari @ Funambol
// @version $Id: vCardProperty.h,v 1.8 2008-04-11 08:17:13 marco_garatti Exp $
//
#include <string.h>

#ifndef INCL_PIM_PROPERTY
#define INCL_PIM_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
        void set(WCHAR** property, const WCHAR* v);

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
        void setEncoding (const WCHAR* encoding);

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


END_NAMESPACE

/** @endcond */
#endif
