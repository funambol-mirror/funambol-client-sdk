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
// @version $Id: Name.h,v 1.7 2008-04-11 08:17:13 marco_garatti Exp $
//

#ifndef INCL_PIM_NAME
#define INCL_PIM_NAME
/** @cond DEV */

#include "vocl/vCard/vCardProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif
