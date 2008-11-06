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
#include "syncml/core/CTTypeSupported.h"


CTTypeSupported::CTTypeSupported(){
    initialize();
}

CTTypeSupported::~CTTypeSupported() {
    if (ctType) {
        delete [] ctType; ctType = NULL;
    }
    if (ctPropParams) {
        ctPropParams->clear();  //delete ctPropParams; ctPropParams = NULL;
    }
}

/**
 * Creates a new CTTypeSupported object with the given information
 *
 * @param ctType an String CTType - NOT NULL
 * @param ctPropParams the array of content type properties and/or content
 *                     content type parameters - NOT NULL
 *
 */
CTTypeSupported::CTTypeSupported(char* ctType, ArrayList* ctPropParams ) {
    initialize();
    setCTType(ctType);
    setCTPropParams(ctPropParams);
}

void CTTypeSupported::initialize() {
    ctType       = NULL;
    ctPropParams = NULL;
}


/**
 * Get a CTType String
 *
 * @return a CTType String
 */
const char* CTTypeSupported::getCTType() {
    return ctType;
}

/**
 * Sets a CTType object
 *
 * @param ctType a CTType object
 */
void CTTypeSupported::setCTType(const char*ctType) {
     if (this->ctType) {
        delete [] this->ctType; this->ctType = NULL;
    }
    this->ctType = stringdup(ctType);
}

/**
 * Gets an array of content type properties and parameters
 *
 * @return an array of content type properties and parameters
 *
 */
ArrayList* CTTypeSupported::getCTPropParams() {
    return ctPropParams;
}

/**
 * Sets an array of content type properties and parameters
 *
 * @param ctPropParams array of content type properties and parameters
 *
 */
void CTTypeSupported::setCTPropParams(ArrayList* ctPropParams) {
     if (this->ctPropParams) {
		this->ctPropParams->clear();
    }
	this->ctPropParams = ctPropParams->clone();

}

ArrayElement* CTTypeSupported::clone() {
    CTTypeSupported* ret = new CTTypeSupported(ctType, ctPropParams);
    return ret;
}
