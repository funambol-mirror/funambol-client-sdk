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
#include "syncml/core/ContentTypeInfo.h"


ContentTypeInfo::ContentTypeInfo() {
     ctType = NULL;
     verCT = NULL;
}

ContentTypeInfo::~ContentTypeInfo() {
    if (ctType) {
        delete [] ctType; ctType = NULL;
    }
    if (verCT) {
        delete [] verCT; verCT = NULL;
    }

}

/**
 * Creates a new ContentTypeCapability object with the given content type
 * and versione
 *
 * @param ctType corresponds to &lt;CTType&gt; element in the SyncML
 *                    specification - NOT NULL
 * @param verCT corresponds to &lt;VerCT&gt; element in the SyncML
 *                specification - NOT NULL
 *
 */
ContentTypeInfo::ContentTypeInfo(const char* ctType, const char* verCT) {

    this->ctType = NULL;
    this->verCT  = NULL;

    if (ctType == NULL){
        // TBD
    }
    if (verCT == NULL){
        // TBD
    }
    setCTType(ctType);
    setVerCT(verCT);
}

/**
 * Gets the content type properties
 *
 * @return the content type properties
 */
const char* ContentTypeInfo::getCTType() {
    return ctType;
}

/**
 * Sets the content type properties
 *
 * @param ctType the content type properties
 */
void ContentTypeInfo::setCTType(const char* ctType) {
    if (this->ctType) {
        delete [] this->ctType; this->ctType = NULL;
    }
    this->ctType = stringdup(ctType);
}

/**
 * Gets the version of the content type
 *
 * @return the version of the content type
 */
const char* ContentTypeInfo::getVerCT() {
    return verCT;
}

/**
 * Sets the version of the content type
 *
 * @param verCT the version of the content type
 */
void ContentTypeInfo::setVerCT(const char* verCT) {
    if (this->verCT) {
        delete [] this->verCT; this->verCT = NULL;
    }
    this->verCT = stringdup(verCT);
}

ArrayElement* ContentTypeInfo::clone() {
    ContentTypeInfo* ret = new ContentTypeInfo(ctType, verCT);
    return ret;
}
