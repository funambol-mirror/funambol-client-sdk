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


#include "syncml/core/Item.h"


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
    moreData = FALSE;

}


void Item::initialize() {
    target       = NULL;
    source       = NULL;
    targetParent = NULL;
    sourceParent = NULL;
    meta         = NULL;
    data         = NULL;
    moreData     = FALSE;
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
            BOOL moreData) {
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
            BOOL moreData) {
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
BOOL Item::isMoreData() {
    return (moreData != NULL);
}

/**
* Gets the Boolean value of moreData
*
* @return true if the data item is incomplete and has further chunks
*         to come, false otherwise
*/
BOOL Item::getMoreData() {
    return moreData;
}

/**
* Sets the moreData property
*
* @param moreData the moreData property
*/
void Item::setMoreData(BOOL moreData) {
    if ((moreData == NULL) || (moreData != TRUE && moreData != FALSE)) {
        this->moreData = NULL;
    } else {
        this->moreData = moreData;
    }
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
