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
#include "syncml/core/ContentTypeParameter.h"
#include "base/globalsdef.h"

USE_NAMESPACE


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
