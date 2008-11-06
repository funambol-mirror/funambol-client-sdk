/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_CONTAINER
#define INCL_CONTAINER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/util/ArrayElement.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"

class ItemContainer{


private:
    ArrayList* allItems;
    ArrayList* newItems;
    ArrayList* updatedItems;
    ArrayList* deletedItems;

public:

    /**
     * Constructor: create a ItemContainer
     */
    ItemContainer();
    ~ItemContainer() ;

    /**
    * Function for ALL ITEMS ArrayList
    */

    void addItemToAllItems(SyncItem* syncItem);

    int getAllItemsSize();

    void resetAllItems() ;

    ArrayList* getAllItems();

    /**
    * Function for NEW ITEMS ArrayList
    */
    void addItemToNewItems(SyncItem* syncItem);

    int getNewItemsSize() ;

    void resetNewItems();

    ArrayList* getNewItems();

    /**
    * Function for UPDATED ITEMS ArrayList
    */
    void addItemToUpdatedItems(SyncItem* syncItem);

    int getUpdatedItemsSize() ;

    void resetUpdatedItems();

    ArrayList* getUpdatedItems();

    /**
    * Function for DELETED ITEMS ArrayList
    */
    void addItemToDeletedItems(SyncItem* syncItem) ;

    int getDeletedItemsSize() ;

    void resetDeletedItems() ;

    ArrayList* getDeletedItems();

};

/** @endcond */
#endif
