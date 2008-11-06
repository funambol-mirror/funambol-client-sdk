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

#include "syncml/core/MapItem.h"

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
