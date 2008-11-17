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


#include "syncml/core/ComplexData.h"


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
