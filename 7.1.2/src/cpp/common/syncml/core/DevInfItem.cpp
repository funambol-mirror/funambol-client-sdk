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


#include "syncml/core/DevInfItem.h"
#include "base/globalsdef.h"

USE_NAMESPACE


DevInfItem::DevInfItem() {
    target = NULL;
    source = NULL;
    meta   = NULL;
    data   = NULL;
}
DevInfItem::~DevInfItem() {
    if (target) { delete target; target = NULL; }
    if (source) { delete source; source = NULL; }
    if (meta)   { delete meta;   meta = NULL;   }
    if (data)   { delete data;   data = NULL;   }

}

/**
* Creates a new DevInfItem object.
*
* @param target item target - NULL ALLOWED
* @param source item source - NULL ALLOWED
* @param meta item meta data - NULL ALLOWED
* @param data item data - NULL ALLOWED
*
*/
DevInfItem::DevInfItem(Target*     target,
                       Source*     source,
                       Meta*       meta  ,
                       DevInfData* data  ) {

    setTarget(target);
    setSource(source);
    setMeta(meta);
    setDevInfData(data);
}

/**
* Returns the item target
*
* @return the item target
*/
Target* DevInfItem::getTarget() {
    return target;
}

/**
* Sets the item target
*
* @param target the target
*
*/
void DevInfItem::setTarget(Target* target) {
    if (this->target) {
        delete this->target; this->target = NULL;
    }
    this->target = target->clone();
}

/**
* Returns the item source
*
* @return the item source
*/
Source* DevInfItem::getSource() {
    return source;
}

/**
* Sets the item source
*
* @param source the source
*
*/
void DevInfItem::setSource(Source* source) {
    if (this->source) {
        delete this->source; this->source = NULL;
    }
    this->source = source->clone();
}

/**
* Returns the item meta element
*
* @return the item meta element
*/
Meta* DevInfItem::getMeta() {
    return meta;
}

/**
* Sets the meta item
*
* @param meta the item meta element
*
*/
void DevInfItem::setMeta(Meta* meta) {
    if (this->meta) {
        delete this->meta; this->meta = NULL;
    }
    this->meta = meta->clone();
}


/**
* Returns the item data
*
* @return the item data
*
*/
DevInfData* DevInfItem::getDevInfData() {
    return data;
}

/**
* Sets the item data
*
* @param data the item data
*
*/
void DevInfItem::setDevInfData(DevInfData* data) {
    if (this->data) {
        delete this->data; this->data = NULL;
    }
    this->data = data->clone();
}

ArrayElement* DevInfItem::clone() {
    DevInfItem* ret = new DevInfItem(target, source, meta, data);
    ret->setData(getData());
    ret->setMoreData(getMoreData());
    return ret;

}
