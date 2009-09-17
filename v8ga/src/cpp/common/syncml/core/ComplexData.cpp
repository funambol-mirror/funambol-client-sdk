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


#include "syncml/core/ComplexData.h"
#include "base/globalsdef.h"

USE_NAMESPACE


ComplexData::ComplexData() {
    initialize();
}

ComplexData::~ComplexData() {
    if (anchor) {
        delete anchor; anchor = NULL;
    }
    if (devInf) {
        delete devInf; devInf = NULL;
    }
    if (properties) {
        delete properties; properties = NULL;
    }
}

void ComplexData::initialize() {
    anchor     = NULL;
    devInf     = NULL;
    properties = NULL;
}

/**
* Creates a Data object from the given anchors string.
*
* @param data the data
*
*/
ComplexData::ComplexData(const char* data) : Data(data) {
    initialize();
}

// ---------------------------------------------------------- Public methods

/**
* Gets the Anchor object property
*
* @return anchor the Anchor object
*/
Anchor* ComplexData::getAnchor() {
    return anchor;
}

/**
* Sets the Anchor object property
*
* @param anchor the Anchor object
*/
void ComplexData::setAnchor(Anchor* anchor) {
    if (anchor == NULL) {
        // TBD
    } else {
        if (this->anchor) {
            delete this->anchor; this->anchor = NULL;
        }
        this->anchor = anchor->clone();
    }
}

/**
* Gets the DevInf object property
*
* @return devInf the DevInf object property
*/
DevInf* ComplexData::getDevInf() {
    return devInf;
}

/**
* Sets the DevInf object property
*
* @param devInf the DevInf object property
*
*/
void ComplexData::setDevInf(DevInf* devInf) {
    if (devInf == NULL) {
        // TBD
    } else {
        if (this->devInf) {
            delete this->devInf; this->devInf = NULL;
        }
        this->devInf = devInf->clone();
    }

}

/*
 * Gets properties
 *
 * @return  the current properties's value
 *
 */
ArrayList* ComplexData::getProperties() {
    return properties;
}

/*
 * Sets properties
 *
 * @param properties the new value
 *
 */
void ComplexData::setProperties(ArrayList* properties) {
    if (this->properties) {
        delete this->properties; this->properties = NULL;
    }

    if (properties) {
        this->properties = properties->clone();
    }
}


ComplexData* ComplexData::clone() {
    ComplexData* ret = new ComplexData(data);
    if (getAnchor()) {
        ret->setAnchor(getAnchor());
    }
    if (getDevInf()) {
        ret->setDevInf(getDevInf());
    }

    if (properties) {
        ret->setProperties(properties);
    }

    return ret;
}
