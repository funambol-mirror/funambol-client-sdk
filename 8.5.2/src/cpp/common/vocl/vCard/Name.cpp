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


#include "base/fscapi.h"
#include "vocl/vCard/Name.h"
#include "base/globalsdef.h"

USE_NAMESPACE

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

