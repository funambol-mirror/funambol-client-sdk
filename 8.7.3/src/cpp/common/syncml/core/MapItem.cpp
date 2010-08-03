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

#include "syncml/core/MapItem.h"
#include "base/globalsdef.h"

USE_NAMESPACE

/**
* This is for serialization purposes
*/
MapItem::MapItem() {

}

MapItem::~MapItem() {
    if (target) {
        delete target; target = NULL;
    }
    if (source) {
        delete source; source = NULL;
    }
}

/**
* Creates a MapItem object from its target and source.
*
*  @param target the mapping target - NOT NULL
*  @param source the mapping source - NOT NULL
*
*
*/
MapItem::MapItem(Target* target, Source* source) {
    initialize();
    setTarget(target);
    setSource(source);
}

void MapItem::initialize() {
    target = NULL;
    source = NULL;
}

/**
* Returns the MapItem's target
*
* @return Tthe MapItem's target
*
*/
Target* MapItem::getTarget() {
    return target;
}

/**
* Sets the MapItem's target
*
* @param target he MapItem's target - NOT NULL
*
*/
void MapItem::setTarget(Target* target) {
    if (target == NULL) {
        // TBD
    } else {
        if (this->target) {
            delete this->target; this->target = NULL;
        }
        this->target = target->clone();
    }

}

/**
* Returns the MapItem's source
*
* @return Tthe MapItem's source
*
*/
Source* MapItem::getSource() {
    return source;
}

/**
* Sets the MapItem's source
*
* @param source he MapItem's source - NOT NULL
*
*/
void MapItem::setSource(Source* source) {
    if (source == NULL) {
        // TBD
    } else {
        if (this->source) {
            delete this->source; this->source = NULL;
        }
        this->source = source->clone();
    }
}

ArrayElement* MapItem::clone() {
    MapItem* ret = new MapItem(target, source);
    return ret;
}
