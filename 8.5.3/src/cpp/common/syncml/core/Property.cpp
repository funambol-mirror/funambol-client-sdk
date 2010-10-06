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

#include "base/util/utils.h"
#include "syncml/core/Property.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Property::Property() {
    propName    = NULL;
    dataType    = NULL;
    maxOccur    = -1;
    maxSize     = -1;
    noTruncate  = false;
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

Property::Property(char* p0, char* p1, long p2, long p3, bool p4, ArrayList* p5, char* p6, ArrayList* p7) {
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

void Property::setNoTruncate(bool /* p0 */) {
}

bool Property::isNoTruncate() {
    return noTruncate;
}

bool Property::getNoTruncate() {
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
