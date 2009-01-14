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


#include "base/util/ItemContainer.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/globalsdef.h"

USE_NAMESPACE


ItemContainer::ItemContainer() {
    allItems = new ArrayList();
    newItems = new ArrayList();
    updatedItems = new ArrayList();
    deletedItems = new ArrayList();
}

ItemContainer::~ItemContainer() {
    allItems->clear();
    newItems->clear();
    updatedItems->clear();
    deletedItems->clear();
}
/**
* Function for ALL ITEMS ArrayList
*/

void ItemContainer::addItemToAllItems(SyncItem* syncItem) {
    if (syncItem == NULL)
        return;
    allItems->add(*syncItem);
}

int ItemContainer::getAllItemsSize() {
    return allItems->size();
}

void ItemContainer::resetAllItems() {
    allItems->clear();
}

ArrayList* ItemContainer::getAllItems() {
    return allItems;
}

/**
* Function for NEW ITEMS ArrayList
*/
void ItemContainer::addItemToNewItems(SyncItem* syncItem) {
    if (syncItem == NULL)
        return;
    newItems->add(*syncItem);
}

int ItemContainer::getNewItemsSize() {
    return newItems->size();
}

void ItemContainer::resetNewItems() {
    newItems->clear();
}


ArrayList* ItemContainer::getNewItems() {
    return newItems;
}

/**
* Function for UPDATED ITEMS ArrayList
*/
void ItemContainer::addItemToUpdatedItems(SyncItem* syncItem) {
    if (syncItem == NULL)
        return;
    updatedItems->add(*syncItem);
}

int ItemContainer::getUpdatedItemsSize() {
    return updatedItems->size();
}

void ItemContainer::resetUpdatedItems() {
    updatedItems->clear();
}


ArrayList* ItemContainer::getUpdatedItems() {
    return updatedItems;
}

/**
* Function for DELETED ITEMS ArrayList
*/
void ItemContainer::addItemToDeletedItems(SyncItem* syncItem) {
    if (syncItem == NULL)
        return;
    deletedItems->add(*syncItem);
}

int ItemContainer::getDeletedItemsSize() {
    return deletedItems->size();
}

void ItemContainer::resetDeletedItems() {
    deletedItems->clear();
}


ArrayList* ItemContainer::getDeletedItems() {
    return deletedItems;
}


