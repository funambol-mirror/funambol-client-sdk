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
// @version $Id: Name.h,v 1.4 2007-06-06 08:36:11 mvitolo Exp $
//

#ifndef INCL_PIM_NAME
#define INCL_PIM_NAME
/** @cond DEV */

#include "vCardProperty.h"

class Name {

    // ------------------------------------------------------------ Private data

    private:
        vCardProperty* salutation ;
        vCardProperty* firstName  ;
        vCardProperty* middleName ;
        vCardProperty* lastName   ;
        vCardProperty* suffix     ;
        vCardProperty* displayName;
        vCardProperty* nickname   ;

        /**
         * Sets the given property address to a clone of the given property. If
         * oldProperty is not null the pointed object is deleted.
         *
         * @param oldProperty Property** to the address of the property to set
         * @param newProperty the new property to set
         */
        void set(vCardProperty** oldProperty, vCardProperty& newProperty);

    // -------------------------------------------- Constructors and Destructors

    public:
        Name();
        ~Name();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the salutation for this name
         *
         * @return the salutation for this name or NULL if not specified
         */
        vCardProperty* getSalutation ();

        /**
         * Sets the salutation. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setSalutation(vCardProperty& p);

        /**
         * Returns the first name for this name
         *
         * @return the first name for this name or NULL if not specified
         */
        vCardProperty* getFirstName ();

        /**
         * Sets the first name. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setFirstName(vCardProperty& p);

        /**
         * Returns the middle name for this name
         *
         * @return the middle name for this name or NULL if not specified
         */
        vCardProperty* getMiddleName ();

        /**
         * Sets the middle name. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setMiddleName(vCardProperty& p);

        /**
         * Returns the last name for this name
         *
         * @return the last name for this name or NULL if not specified
         */
        vCardProperty* getLastName ();

        /**
         * Sets the last name. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setLastName(vCardProperty& p);

        /**
         * Returns the suffix for this name
         *
         * @return the suffix for this name or NULL if not specified
         */
        vCardProperty* getSuffix ();

        /**
         * Sets the suffix. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setSuffix(vCardProperty& p);

        /**
         * Returns the display name for this name
         *
         * @return the display name for this name or NULL if not specified
         */
        vCardProperty* getDisplayName ();

        /**
         * Sets the display name. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setDisplayName(vCardProperty& p);

        /**
         * Returns the nickname for this name
         *
         * @return the nickname for this name or NULL if not specified
         */
        vCardProperty* getNickname ();

        /**
         * Sets the nickname. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setNickname(vCardProperty& p);

        /**
         * Creates and returns a new Name object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        Name* clone();
};

/** @endcond */
#endif
