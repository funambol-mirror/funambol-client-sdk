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
// @version $Id: Email.h,v 1.4 2007-06-06 08:36:11 mvitolo Exp $
//


#ifndef INCL_PIM_EMAIL
#define INCL_PIM_EMAIL
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "vocl/vCard/TypedProperty.h"

/**
 * An object representing a contact email
 *
 */

class Email : public TypedProperty {
    // ------------------------------------------------------------ Private data

    // -------------------------------------------- Constructors and Destructors
    public:

        /**
         * Creates an empty email
         */
        Email();
        ~Email();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the email address for this email
         *
         * @return the email address for this email
         */
        vCardProperty* getEmailAddress();

        /**
         * Sets the email address. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setEmailAddress(vCardProperty& p);

        /**
         * Creates a new instance of TypedProperty from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone() ;
};

/** @endcond */
#endif
