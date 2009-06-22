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
#include "syncml/core/PropParam.h"
#include "base/globalsdef.h"

USE_NAMESPACE


PropParam::PropParam() {
    paramName = dataType = displayName = NULL;
    valEnums = NULL;
}

PropParam::~PropParam() {
    if (paramName  ) delete [] paramName  ;
    if (dataType   ) delete [] dataType   ;
    if (displayName) delete [] displayName;
    if (valEnums)    delete    valEnums   ;
}


PropParam::PropParam(char* paramName, char* dataType, ArrayList* valEnums, char* displayName) {
    this->paramName   = NULL;
    this->dataType    = NULL;
    this->displayName = NULL;
    this->valEnums    = NULL;

    setParamName  (paramName  );
    setDataType   (dataType   );
    setValEnums   (valEnums   );
    setDisplayName(displayName);
}


/*
 * Gets displayName
 *
 * @return  the current displayName's value
 *
 */
const char* PropParam::getDisplayName() {
    return displayName;
}

/*
 * Sets displayName
 *
 * @param displayName the new value
 *
 */
void PropParam::setDisplayName(const char*displayName) {
    if (this->displayName) {
        delete [] this->displayName; this->displayName = NULL;
    }

    if (displayName) {
        this->displayName = stringdup(displayName);
    }
}


/*
 * Gets paramName
 *
 * @return  the current paramName's value
 *
 */
const char* PropParam::getParamName() {
    return paramName;
}

/*
 * Sets paramName
 *
 * @param paramName the new value
 *
 */
void PropParam::setParamName(const char*paramName) {
    if (this->paramName) {
        delete [] this->paramName; this->paramName = NULL;
    }

    if (paramName) {
        this->paramName = stringdup(paramName);
    }
}

/*
 * Gets dataType
 *
 * @return  the current dataType's value
 *
 */
const char* PropParam::getDataType() {
    return dataType;
}

/*
 * Sets dataType
 *
 * @param dataType the new value
 *
 */
void PropParam::setDataType(const char*dataType) {
    if (this->dataType) {
        delete [] this->dataType; this->dataType = NULL;
    }

    if (dataType) {
        this->dataType = stringdup(dataType);
    }
}


ArrayList* PropParam::getValEnums() {
    return valEnums;
}

void PropParam::setValEnums(ArrayList* p0) {
    if (this->valEnums) {
        delete this->valEnums; this->valEnums = NULL;
    }

    if (p0) {
        this->valEnums = p0->clone();
    }

}

ArrayElement* PropParam::clone() {
  return (ArrayElement*)new PropParam(paramName, dataType, valEnums, displayName);
}

