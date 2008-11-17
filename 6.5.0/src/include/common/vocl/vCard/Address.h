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
// @version $Id: Address.h,v 1.4 2007-06-06 08:36:11 mvitolo Exp $
//

#ifndef INCL_PIM_ADDRESS
#define INCL_PIM_ADDRESS
/** @cond DEV */

#include "vCardProperty.h"

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

/** @endcond */
#endif
