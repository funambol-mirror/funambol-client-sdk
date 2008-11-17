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
#include "syncml/core/ContentTypeInfo.h"
#include "base/globalsdef.h"

USE_NAMESPACE


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
