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
#include "syncml/core/ContentTypeParameter.h"


ContentTypeParameter::ContentTypeParameter() {
    paramName       = NULL;
    valEnum         = NULL;
    displayName     = NULL;
    dataType        = NULL;
    size            = 0;

}

ContentTypeParameter::~ContentTypeParameter() {
    if (paramName)      { delete [] paramName;      paramName   = NULL; }
    if (valEnum)        { valEnum->clear(); } //delete valEnum; valEnum = NULL; }
    if (displayName)    { delete [] displayName;    displayName = NULL; }
    if (dataType)       { delete [] dataType;       dataType    = NULL; }
    size            = 0;

}

 /**
 * Creates a new ContentTypeParameter object with the given name, value and
 * display name
 *
 * @param paramName corresponds to &lt;ParamName&gt; element in the SyncML
 *                  specification - NOT NULL
 * @param valEnum   corresponds to &lt;ValEnum&gt; element in the SyncML
 *                  specification
 * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML
 *                  specification
 *
 */
ContentTypeParameter::ContentTypeParameter(char* paramName,
                     ArrayList* valEnum,
                     char* displayName) {

    setParamName(paramName);
    setValEnum(valEnum);

    this->displayName = stringdup(displayName);

}

/**
 * Creates a new ContentTypeParameter object with the given name, data type,
 * size, display name
 *
 * @param paramName corresponds to &lt;ParamName&gt; element in the SyncML
 *                  specification - NOT NULL
 * @param dataType  corresponds to &lt;DataType&gt; element in the SyncML
 *                  specification
 * @param size      corresponds to &lt;Size&gt; element in the SyncML
 *                  specification
 * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML
 *                  specification
 *
 */
ContentTypeParameter::ContentTypeParameter(char* paramName,
                            char* dataType,
                            int size,
                            char* displayName) {

        setParamName(paramName);
        this->dataType    = stringdup(dataType);
        this->size        = size;
        this->displayName = stringdup(displayName);
}

/**
 * Gets the parameter name propeties
 *
 * @return the parameter name propeties
 */
const char* ContentTypeParameter::getParamName() {
    return paramName;
}

/**
 * Sets the param name property
 *
 * @param paramName the param name property
 */
void ContentTypeParameter::setParamName(const char*paramName) {
    if (this->paramName) {
        delete [] this->paramName; this->paramName = NULL;
    }
    this->paramName = stringdup(paramName);
}


/**
 * Gets the array of value for parameter
 *
 * @return the array of value for parameter
 */
ArrayList* ContentTypeParameter::getValEnum() {
    return valEnum;
}

/**
 * Sets the array of enumerated value property
 *
 * @param valEnum the array of enumerated value property
 */
void ContentTypeParameter::setValEnum(ArrayList* valEnum) {
    if (this->valEnum) {
		this->valEnum->clear();
    }
    if (valEnum) {
	    this->valEnum = valEnum->clone();
    }
}

/**
 * Gets the display name propeties
 *
 * @return the display name propeties
 */
const char* ContentTypeParameter::getDisplayName() {
    return displayName;
}

/**
 * Sets the display name of a given content type parameter
 *
 * @param displayName the display name of a given content type parameter
 *
 */
void ContentTypeParameter::setDisplayName(const char*displayName) {
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
const char* ContentTypeParameter::getDataType() {
    return dataType;
}

/**
 * Sets the data type of a given content type parameter
 *
 * @param dataType the data type of a given content type parameter
 *
 */
void ContentTypeParameter::setDataType(const char*dataType) {
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
int ContentTypeParameter::getSize() {
    return size;
}

/**
 * Sets the size of a given content type parameter
 *
 * @param size the size of a given content type parameter
 *
 */
void ContentTypeParameter::setSize(int size) {
    this->size = size;
}

ArrayElement* ContentTypeParameter::clone() {
    ContentTypeParameter* ret = new ContentTypeParameter(paramName, valEnum, displayName);
    return ret;
}
