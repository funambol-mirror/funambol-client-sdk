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

#include "base/util/utils.h"
#include "syncml/core/Property.h"


Property::Property() {
    propName    = NULL;
    dataType    = NULL;
    maxOccur    = -1;
    maxSize     = -1;
    noTruncate  = -1;  // -1 undefined, 0 FALSE, 1 TRUE
    valEnums    = NULL;
    displayName = NULL;
    propParams  = NULL;
}

Property::~Property() {
    if (propName)    delete [] propName   ;
    if (dataType)    delete [] dataType   ;
    if (displayName) delete [] displayName;

    if (propParams) delete propParams;
    if (valEnums)   delete valEnums  ;
}

Property::Property(char* p0, char* p1, long p2, long p3, BOOL p4, ArrayList* p5, char* p6, ArrayList* p7) {
    propName    = (p0) ? stringdup(p0) : NULL;
    dataType    = (p1) ? stringdup(p1) : NULL;
    maxOccur    = p2;
    maxSize     = p3;
    noTruncate  = p4;
    valEnums    = (p5) ? p5->clone() : NULL;
    displayName = (p6) ? stringdup(p6) : NULL;
    propParams  = (p7) ? p7->clone() : NULL;
}

const char* Property::getDisplayName() {
    return displayName;
}

void Property::setDisplayName(const char*displayName) {
    if (this->displayName) {
        delete this->displayName; this->displayName = NULL;
    }

    if (displayName) {
        this->displayName = stringdup(displayName);
    }
}

/*
 * Gets propName
 *
 * @return  the current propName's value
 *
 */
const char* Property::getPropName() {
    return propName;
}

/*
 * Sets propName
 *
 * @param propName the new value
 *
 */
void Property::setPropName(const char*propName) {
    if (this->propName) {
        delete this->propName; this->propName = NULL;
    }

    if (propName) {
        this->propName = stringdup(propName);
    }
}

/*
 * Gets dataType
 *
 * @return  the current dataType's value
 *
 */
const char* Property::getDataType() {
    return dataType;
}

/*
 * Sets dataType
 *
 * @param dataType the new value
 *
 */
void Property::setDataType(const char*dataType) {
    if (this->dataType) {
        delete this->dataType; this->dataType = NULL;
    }

    if (dataType) {
        this->dataType = stringdup(dataType);
    }
}

long Property::getMaxOccur() {
    return maxOccur;
}

void Property::setMaxOccur(long p0) {
    this->maxOccur = p0;
}

long Property::getMaxSize() {
    return maxSize;
}

void Property::setMaxSize(long p0) {
    maxSize = p0;
}

void Property::setNoTruncate(BOOL p0) {
}

BOOL Property::isNoTruncate() {
    return (noTruncate == TRUE);
}

BOOL Property::getNoTruncate() {
    return noTruncate;
}

ArrayList* Property::getValEnums() {
    return valEnums;
}

void Property::setValEnums(ArrayList* p0) {
    if (this->valEnums) {
        delete this->valEnums; this->valEnums = NULL;
    }

    if (p0) {
        this->valEnums = p0->clone();
    }
}

ArrayList* Property::getPropParams() {
    return propParams;
}

void Property::setPropParams(ArrayList* p0) {
    if (this->propParams) {
        delete this->propParams; this->propParams = NULL;
    }

    if (p0) {
        this->propParams = p0->clone();
    }
}

void Property::setPropParams(ArrayList& array) {
    setPropParams(&array);
}

ArrayElement* Property::clone() {
    return (ArrayElement*)new Property(propName, dataType, maxOccur, maxSize, noTruncate, valEnums, displayName, propParams);
}
