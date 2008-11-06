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


#include "syncml/core/ItemizedCommand.h"


ItemizedCommand::ItemizedCommand() {
   initialize();

}
ItemizedCommand::~ItemizedCommand() {
    if (items) {
        items->clear(); // delete items;        items = NULL;
    }
    if (meta) {
        delete meta; meta = NULL;
    }

}
/**
* Create a new ItemizedCommand object with the given commandIdentifier,
* meta object and an array of item
*
* @param cmdID the command identifier - NOT NULL
* @param meta the meta object
* @param items an array of item - NOT NULL
*
*/
ItemizedCommand::ItemizedCommand(CmdID* cmdID, Meta* meta, ArrayList* items) : AbstractCommand(cmdID) {
    initialize();

    if (cmdID == NULL) {
        // TBD
    }

    if (items == NULL) {
        items = new ArrayList();
    }

    setMeta(meta);
    setItems(items);
}

/**
* Create a new ItemizedCommand object with the given commandIdentifier
* and an array of item
*
* @param cmdID the command identifier - NOT NULL
* @param items an array of item - NOT NULL
*
*/
ItemizedCommand::ItemizedCommand(CmdID*  cmdID, ArrayList* items) : AbstractCommand(cmdID) {

    initialize();
    if (cmdID == NULL) {
        // TBD
    }

    if (items == NULL) {
        items = new ArrayList();
    }

    setMeta(NULL);
    setItems(items);

}

void ItemizedCommand::initialize() {
    items = NULL;  // Item[]
    meta  = NULL;
}

/**
* Gets the array of items
*
* @return the array of items
*/
ArrayList* ItemizedCommand::getItems() {
    return items;
}

/**
* Sets an array of Item object
*
* @param items an array of Item object
*/
void ItemizedCommand::setItems(ArrayList* items) {
    if (this->items) {
		this->items->clear(); this->items = NULL;
    }
	this->items = items->clone();

}

/**
* Gets the Meta object
*
* @return the Meta object
*/
Meta* ItemizedCommand::getMeta() {
    return meta;
}

/**
* Sets the Meta object
*
* @param meta the Meta object
*
*/
void ItemizedCommand::setMeta(Meta* meta) {
    if (this->meta) {
        delete this->meta; this->meta = NULL;
    }
    if (meta) {
        this->meta = meta->clone();
    } else {
        this->meta = NULL;
    }
}
