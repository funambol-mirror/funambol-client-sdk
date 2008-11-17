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


#include "base/util/ItemContainer.h"
#include "base/util/utils.h"
#include "base/Log.h"


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


