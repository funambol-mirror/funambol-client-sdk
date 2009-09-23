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


#ifndef INCL_PIM_TYPED_PROPERTY
#define INCL_PIM_TYPED_PROPERTY
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "vocl/vCard/vCardProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
        void setType(const WCHAR* type);

        /**
         * Creates a new instance of TypedProperty from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone() { return NULL;}; //FIXME
};


END_NAMESPACE

/** @endcond */
#endif
