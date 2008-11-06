/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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


#include "base/fscapi.h"
#include "vocl/vCard/Address.h"


Address::Address() {
    postOfficeAddress = NULL;
    roomNumber        = NULL;
    street            = NULL;
    city              = NULL;
    state             = NULL;
    postalCode        = NULL;
    country           = NULL;
    label             = NULL;
}

Address::~Address() {
    if (postOfficeAddress) delete postOfficeAddress;
    if (roomNumber       ) delete roomNumber       ;
    if (street           ) delete street           ;
    if (city             ) delete city             ;
    if (state            ) delete state            ;
    if (postalCode       ) delete postalCode       ;
    if (country          ) delete country          ;
    if (label            ) delete label            ;
}


vCardProperty* Address::getPostOfficeAddress () {
    return postOfficeAddress;
}

void Address::setPostOfficeAddress (vCardProperty& p) {
    set(&postOfficeAddress, p);
}

vCardProperty* Address::getRoomNumber () {
    return roomNumber;
}

void Address::setRoomNumber(vCardProperty& p) {
    set(&roomNumber, p);
}

vCardProperty* Address::getStreet () {
    return street;
}

void Address::setStreet(vCardProperty& p) {
    set(&street, p);
}

vCardProperty* Address::getCity () {
    return city;
}

void Address::setCity(vCardProperty& p) {
    set(&city, p);
}

vCardProperty* Address::getState () {
    return state;
}

void Address::setState(vCardProperty& p) {
    set(&state, p);
}

vCardProperty* Address::getPostalCode () {
    return postalCode;
}

void Address::setPostalCode(vCardProperty& p) {
    set(&postalCode, p);
}

vCardProperty* Address::getCountry () {
    return country;
}

void Address::setCountry(vCardProperty& p) {
    set(&country, p);
}

vCardProperty* Address::getLabel () {
    return label;
}

void Address::setLabel(vCardProperty& p) {
    set(&label, p);
}

void Address::set(vCardProperty** oldProperty, vCardProperty& newProperty) {

    if (*oldProperty) {
        delete *oldProperty;
    }
    *oldProperty = newProperty.clone();
}

Address* Address::clone() {
    Address* ret = new Address();

    if (postOfficeAddress) {
        ret->setPostOfficeAddress(*postOfficeAddress);
    }
    if (roomNumber) {
        ret->setRoomNumber(*roomNumber);
    }
    if (street) {
        ret->setStreet(*street);
    }
    if (city) {
        ret->setCity(*city);
    }
    if (state) {
        ret->setState(*state);
    }
    if (postalCode) {
        ret->setPostalCode(*postalCode);
    }
    if (country) {
        ret->setCountry(*country);
    }
    if (label) {
        ret->setLabel(*label);
    }

    return ret;
}
