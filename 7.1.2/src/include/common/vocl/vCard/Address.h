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
// @version $Id: Address.h,v 1.7 2008-04-11 08:17:13 marco_garatti Exp $
//

#ifndef INCL_PIM_ADDRESS
#define INCL_PIM_ADDRESS
/** @cond DEV */

#include "vocl/vCard/vCardProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * An object representing an address
 */
class Address {

    // ------------------------------------------------------------ Private data

    private:
        vCardProperty* postOfficeAddress;
        vCardProperty* roomNumber;
        vCardProperty* street;
        vCardProperty* city;
        vCardProperty* state;
        vCardProperty* postalCode;
        vCardProperty* country;
        vCardProperty* label;

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
        /**
         * Creates an empty address
         */
        Address();
        ~Address();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the post office of this address
         *
         * @return the post office of this address or NULL if not defined
         */
        vCardProperty* getPostOfficeAddress ();

        /**
         * Sets the address post office. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPostOfficeAddress(vCardProperty& p);

        /**
         * Returns the address room number of this address
         *
         * @return the room number of this address or NULL if not defined
         */
        vCardProperty* getRoomNumber () ;

        /**
         * Sets the address room number. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setRoomNumber(vCardProperty& p);

        /**
         * Returns the street of this address
         *
         * @return the street of this address or NULL if not defined
         */
        vCardProperty* getStreet () ;

        /**
         * Sets the address street. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setStreet(vCardProperty& p);

        /**
         * Returns the city of this address
         *
         * @return the city of this address or NULL if not defined
         */
        vCardProperty* getCity () ;

        /**
         * Sets the address city. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setCity(vCardProperty& p);

        /**
         * Returns the state of this address
         *
         * @return the state of this address or NULL if not defined
         */
        vCardProperty* getState () ;

        /**
         * Sets the address state. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setState(vCardProperty& p);

        /**
         * Returns the postal code of this address
         *
         * @return the postal code of this address or NULL if not defined
         */
        vCardProperty* getPostalCode () ;

        /**
         * Sets the address post office. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPostalCode(vCardProperty& p);

        /**
         * Returns the country of this address
         *
         * @return the country of this address or NULL if not defined
         */
        vCardProperty* getCountry () ;

        /**
         * Sets the address country. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setCountry(vCardProperty& p);

        /**
         * Returns the label of this address
         *
         * @return the label of this address or NULL if not defined
         */
        vCardProperty* getLabel () ;

        /**
         * Sets the address label. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setLabel(vCardProperty& p);

        /**
         * Creates and returns a new Address object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        Address* clone();

};


END_NAMESPACE

/** @endcond */
#endif
