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
#include "vocl/vCard/Name.h"

Name::Name() {
    salutation  = NULL;
    firstName   = NULL;
    middleName  = NULL;
    lastName    = NULL;
    suffix      = NULL;
    displayName = NULL;
    nickname    = NULL;
}

Name::~Name() {
    if (salutation) {
        delete salutation; salutation = NULL;
    }
    if (firstName) {
        delete firstName; firstName = NULL;
    }
    if (middleName) {
        delete middleName; middleName = NULL;
    }
    if (lastName) {
        delete lastName; lastName = NULL;
    }
    if (suffix) {
        delete suffix; suffix = NULL;
    }
    if (displayName) {
        delete displayName; displayName = NULL;
    }
    if (nickname) {
        delete nickname; nickname = NULL;
    }
}

vCardProperty* Name::getSalutation () {
    return salutation;
}

void Name::setSalutation(vCardProperty& p) {
    set(&salutation, p);
}

vCardProperty* Name::getFirstName () {
    return firstName;
}

void Name::setFirstName(vCardProperty& p) {
    set(&firstName, p);
}

vCardProperty* Name::getMiddleName () {
    return middleName;
}

void Name::setMiddleName(vCardProperty& p) {
    set(&middleName, p);
}

vCardProperty* Name::getLastName () {
    return lastName;
}

void Name::setLastName(vCardProperty& p) {
    set(&lastName, p);
}

vCardProperty* Name::getSuffix () {
    return suffix;
}

void Name::setSuffix(vCardProperty& p) {
    set(&suffix, p);
}

vCardProperty* Name::getDisplayName () {
    return displayName;
}

void Name::setDisplayName(vCardProperty& p) {
    set(&displayName, p);
}

vCardProperty* Name::getNickname () {
    return nickname;
}

void Name::setNickname(vCardProperty& p) {
    set(&nickname, p);
}

Name* Name::clone() {
    Name* ret = new Name();

    if (salutation) {
        ret->setSalutation(*salutation);
    }
    if (firstName) {
        ret->setFirstName(*firstName);
    }
    if (middleName) {
        ret->setMiddleName(*middleName);
    }
    if (lastName) {
        ret->setLastName(*lastName);
    }
    if (suffix) {
        ret->setSuffix(*suffix);
    }
    if (displayName) {
        ret->setDisplayName(*displayName);
    }
    if (nickname) {
        ret->setNickname(*nickname);
    }

    return ret;
}

void Name::set(vCardProperty** oldProperty, vCardProperty& newProperty) {
    if (*oldProperty) delete *oldProperty;

    *oldProperty = newProperty.clone();
}

