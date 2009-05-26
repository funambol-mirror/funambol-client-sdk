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


#include "syncml/core/ItemizedCommand.h"
#include "base/globalsdef.h"

USE_NAMESPACE


ItemizedCommand::ItemizedCommand() {
   initialize();

}
ItemizedCommand::~ItemizedCommand() {
    if (items) {
        /*items->clear();*/  
        delete items;        items = NULL;
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

    //if (items == NULL) {
    //    items = new ArrayList();
    //}

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

    //if (items == NULL) {
    //    items = new ArrayList();
    //}

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
		//this->items->clear(); 
                delete this->items;
                this->items = NULL;
        }
    if(items){
        this->items = items->clone();
    }else{
        this->items = new ArrayList();
    }

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
