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
#include "syncml/core/CTPropParam.h"


CTPropParam::CTPropParam() {
    initialize();
}

void CTPropParam::initialize() {
    propName        = NULL;
    valEnum         = NULL;
    displayName     = NULL;
    dataType        = NULL;
    size            = 0;
    ctParameters    = NULL; //ContentTypeParameter[]
}

CTPropParam::~CTPropParam() {
    if (propName)        { delete [] propName;    propName      = NULL; }
    if (valEnum)         { valEnum->clear(); } //delete valEnum; valEnum = NULL; }       //String[]
    if (displayName)     { delete [] displayName; displayName   = NULL; }
    if (dataType)        { delete    dataType;                          }
    size            = 0;
    if (ctParameters)    { ctParameters->clear(); }//delete ctParameters; ctParameters = NULL;  } //ContentTypeParameter[]

}

CTPropParam::CTPropParam(char*   propName,
                        ArrayList* valEnum,
                        char*   displayName,
                        ArrayList* ctParameters) {
    initialize();
    setPropName(propName);
    setValEnum(valEnum);
    setContentTypeParameters(ctParameters);

    this->displayName  = stringdup(displayName);

}


/**
 * Creates a new ContentTypeProperty object with the given name, value and
 * display name
 *
 * @param propName corresponds to &lt;PropName&gt; element in the SyncML
 *                  specification - NOT NULL
 * @param dataType corresponds to &lt;DataType&gt; element in the SyncML
 *                  specification
 * @param size corresponds to &lt;Size&gt; element in the SyncML
 *                  specification
 * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML
 *                  specification
 * @param ctParameters the array of content type parameters - NOT NULL
 *
 */
CTPropParam::CTPropParam(char* propName,
                   char* dataType,
                   int size,
                   char* displayName,
                   ArrayList* ctParameters) {
    initialize();
    setPropName(propName);
    setContentTypeParameters(ctParameters);

    this->valEnum      = new ArrayList();
    this->dataType     = stringdup(dataType);
    this->size         = size;
    this->displayName  = stringdup(displayName);

}
// ---------------------------------------------------------- Public methods

/**
 * Gets the property name
 *
 * @return the property name
 */
const char* CTPropParam::getPropName() {
    return propName;
}

/**
 * Sets the property name
 *
 * @param propName the property name
 */
void CTPropParam::setPropName(const char*propName) {
    if (this->propName) {
        delete [] this->propName; this->propName = NULL;
    }
    this->propName = stringdup(propName);

}

/**
 * Gets the array of value for the property
 *
 * @return the array of value for the property
 */
ArrayList* CTPropParam::getValEnum() {
    return valEnum;
}

/**
 * Sets the array of enumerated value property
 *
 * @param valEnum the array of enumerated value property
 */
void CTPropParam::setValEnum(ArrayList* valEnum) {
    if (this->valEnum) {
		this->valEnum->clear();
    }
	this->valEnum = valEnum->clone();
}

/**
 * Gets the display name property
 *
 * @return the display name property
 */
const char* CTPropParam::getDisplayName() {
    return displayName;
}

/**
 * Sets the display name of a given content type property
 *
 * @param displayName the display name of a given content type property
 */
void CTPropParam::setDisplayName(const char*displayName) {
    if (this->displayName) {
        delete [] this->displayName; this->displayName = NULL;
    }
    this->displayName = stringdup(displayName);
}

/**
 * Gets the data type propeties
 *
 * @return the data type propeties
 */
const char* CTPropParam::getDataType() {
    return dataType;
}

/**
 * Sets the data type of a given content type property
 *
 * @param dataType the data type of a given content type property
 */
void CTPropParam::setDataType(const char*dataType) {
    if (this->dataType) {
        delete [] this->dataType; this->dataType = NULL;
    }
    this->dataType = stringdup(dataType);
}

/**
 * Gets the size propeties
 *
 * @return the size propeties
 */
int CTPropParam::getSize() {
    return size;
}

/**
 * Sets the size of a given content type property
 *
 * @param size the size of a given content type property
 *
 */
void CTPropParam::setSize(int size) {
    this->size = size;
}

/**
 * Gets the array of ContentTypeParameter
 *
 * @return the size propeties
 */
ArrayList* CTPropParam::getContentTypeParameters() {
     return ctParameters;
}

/**
 * Sets an array of content type properties
 *
 * @param ctParameters array of content type properties
 *
 */
void CTPropParam::setContentTypeParameters(ArrayList* ctParameters) {
    if (this->ctParameters) {
		this->ctParameters->clear();
    }
	this->ctParameters = ctParameters->clone();
}

ArrayElement* CTPropParam::clone() {
    CTPropParam* ret = new CTPropParam(propName, valEnum, displayName, ctParameters);
    return ret;
}
