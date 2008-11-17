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


#include "syncml/core/Item.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Item::Item() {
   initialize();
}

Item::~Item() {
    if (target) { delete target; target = NULL; }
    if (source) { delete source; source = NULL; }
    if (meta  ) { delete meta  ; meta   = NULL; }
    if (data  ) { delete data  ; data   = NULL; }
    if (targetParent) { delete [] targetParent; targetParent = NULL; }
    if (sourceParent) { delete [] sourceParent; sourceParent = NULL; }
    moreData = false;

}


void Item::initialize() {
    target       = NULL;
    source       = NULL;
    targetParent = NULL;
    sourceParent = NULL;
    meta         = NULL;
    data         = NULL;
    moreData     = false;
}

/**
 * Creates a new Item object.
 *
 * @param target item target - NULL ALLOWED
 * @param source item source - NULL ALLOWED
 * @param targetParent item target parent - NULL ALLOWED (DEFAULT)
 * @param sourceParent item source parent - NULL ALLOWED (DEFAULT)
 * @param meta item meta data - NULL ALLOWED
 * @param data item data - NULL ALLOWED
 *
 */
Item::Item( Target* target,
            Source* source,
            char* tParent,
            char* sParent,
            Meta*   meta  ,
            ComplexData* data,
            bool moreData) {
    initialize();

    setTarget(target);
    setSource(source);
    setTargetParent(tParent);
    setSourceParent(sParent);
    setMeta(meta);
    setData(data);
    setMoreData(moreData);

}




/**
* Creates a new Item object.
*
* @param target item target - NULL ALLOWED
* @param source item source - NULL ALLOWED
* @param meta item meta data - NULL ALLOWED
* @param data item data - NULL ALLOWED
*
*/
Item::Item( Target* target,
            Source* source,
            Meta*   meta  ,
            ComplexData* data,
            bool moreData) {
    initialize();
    setTarget(target);
    setSource(source);
    setTargetParent(NULL);
    setSourceParent(NULL);
    setMeta(meta);
    setData(data);
    setMoreData(moreData);

}

/**
* Returns the item target
*
* @return the item target
*/
Target* Item::getTarget() {
    return target;
}

/**
* Sets the item target
*
* @param target the target
*
*/
void Item::setTarget(Target* target) {
    if (this->target) {
		delete this->target; this->target = NULL;
    }
    if (target) {
	    this->target = target->clone();
    }
}

/**
* Returns the item source
*
* @return the item source
*/
Source* Item::getSource() {
    return source;
}

/**
* Sets the item source
*
* @param source the source
*
*/
void Item::setSource(Source* source) {
    if (this->source) {
		delete this->source; this->source = NULL;
    }
    if (source) {
	    this->source = source->clone();
    }
}

/**
* Returns the item meta element
*
* @return the item meta element
*/
Meta* Item::getMeta() {
    return meta;
}

/**
* Sets the meta item
*
* @param meta the item meta element
*
*/
void Item::setMeta(Meta* meta) {
    if (this->meta) {
		delete this->meta; this->meta = NULL;
    }
    if (meta) {
	    this->meta = meta->clone();
    }
}

/**
* Returns the item data
*
* @return the item data
*
*/
ComplexData* Item::getData() {
    return data;
}

/**
* Sets the item data
*
* @param data the item data
*
*/
void Item::setData(ComplexData* data) {
    if (this->data) {
		delete this->data; this->data = NULL;
    }
    if (data) {
	    this->data = data->clone();
    }
}

/**
* Gets moreData property
*
* @return true if the data item is incomplete and has further chunks
*         to come, false otherwise
*/
bool Item::isMoreData() {
    return (moreData != NULL);
}

/**
* Gets the Boolean value of moreData
*
* @return true if the data item is incomplete and has further chunks
*         to come, false otherwise
*/
bool Item::getMoreData() {
    return moreData;
}

/**
* Sets the moreData property
*
* @param moreData the moreData property
*/
void Item::setMoreData(bool moreData) {
    this->moreData = moreData;
}

/**
 * Gets the taregtParent property
 *
 * @return the taregtParent property value
 */
const char* Item::getTargetParent() {
    return targetParent;
}

/**
 * Sets the taregtParent property
 *
 * @param parent the taregtParent property
 */
void Item::setTargetParent(const char*parent) {
    if (targetParent) {
        delete [] targetParent; targetParent = NULL;
    }
    targetParent = stringdup(parent);
}

/**
 * Gets the sourceParent property
 *
 * @return the sourceParent property value
 */
const char* Item::getSourceParent() {
    return sourceParent;
}

/**
 * Sets the sourceParent property
 *
 * @param parent the sourceParent property
 */
void Item::setSourceParent(const char*parent) {
    if (sourceParent) {
        delete [] sourceParent; sourceParent = NULL;
    }
    sourceParent = stringdup(parent);
}

/**
 * Item can be an element of an Array object
 */
ArrayElement* Item::clone() {
    //Item* ret = new Item(target, source, targetParent, sourceParent, meta, data, moreData);
    Item* ret = new Item(target, source, meta, data, moreData);
    return ret;
}
