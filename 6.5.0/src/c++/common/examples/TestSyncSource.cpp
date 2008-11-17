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

#include "examples/TestSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"

static int all      = -1;
static int cnew     = -1;
static int cupdated = -1;
static int cdeleted = -1;
ArrayList items;
ArrayList newItems;
ArrayList updatedItems;
ArrayList deletedItems;


int setAllItems() {

    SyncItem item;
    WCHAR name[64];
    char data[128];

    for (int i = 0; i < 4; ++i) {

        wsprintf(name, TEXT("%s%d"), TEXT("item"), i);
        sprintf(data, "This is item %d", i);

        item.setKey(name);
        item.setData(data  , (strlen(data))*sizeof(char));
        items.add(item);
    }
    return 0;
}

int setAllItemsEmpty() {
    return 0;
}

int setModifiedItems() {

    SyncItem item;

    for (int i = 0; i < 4; ++i) {

        switch (i) {

            case 0:
                item.setKey(TEXT("item5"));
                item.setData("This is a new item Four"  , 23*sizeof(char));
                newItems.add(item);
                break;

            case 1:
                item.setKey(TEXT("item1"));
                item.setData("This is the updated item one"  , 28*sizeof(char));
                updatedItems.add(item);
                break;

            case 2:
                item.setKey(TEXT("item3"));
                item.setData("This is the updated item Three", 30*sizeof(char));
                updatedItems.add(item);
                break;

            case 3:

                item.setKey(TEXT("item4"));
                deletedItems.add(item);
                break;

        }
    }

    return 0;
}

int setModifiedItemsEmpty() {
    return 0;
}


TestSyncSource::TestSyncSource(const WCHAR* name, SyncSourceConfig *sc) : SyncSource(name, sc) {
}

TestSyncSource::~TestSyncSource() {
}

/*
* Return the first SyncItem of all. It is used in case of slow or refresh sync
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource::getFirstItem() {

    setAllItems();
    if (items.size() == 0) {
        return NULL;
    }
    all = 0;
    return (SyncItem*)items[all]->clone();
}

/*
* Return the next SyncItem of all. It is used in case of slow or refresh sync
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource::getNextItem() {
    all++;
    if (all == items.size()) {
        return NULL;
    }
    return (SyncItem*)items.get(all)->clone();
}

SyncItem* TestSyncSource::getFirstNewItem() {
    setModifiedItems();
    if (newItems.size() == 0) {
        return NULL;
    }
    cnew = 0;
    return (SyncItem*)newItems.get(cnew)->clone();
}

SyncItem* TestSyncSource::getNextNewItem() {
    cnew++;
    if (cnew == newItems.size()) {
        return NULL;
    }
    return (SyncItem*)newItems.get(cnew)->clone();
}

SyncItem* TestSyncSource::getFirstUpdatedItem() {

    if (updatedItems.size() == 0) {
        return NULL;
    }
    cupdated = 0;
    return (SyncItem*)updatedItems.get(cupdated)->clone();
}

SyncItem* TestSyncSource::getNextUpdatedItem() {
    cupdated++;
    if (cupdated == updatedItems.size()) {
        return NULL;
    }
    return (SyncItem*)updatedItems.get(cupdated)->clone();
}

SyncItem* TestSyncSource::getFirstDeletedItem() {

    if (deletedItems.size() == 0) {
        return NULL;
    }
    cdeleted = 0;
    return (SyncItem*)deletedItems.get(cdeleted)->clone();
}

SyncItem* TestSyncSource::getNextDeletedItem() {
    cdeleted++;
    if (cdeleted == deletedItems.size()) {
        return NULL;
    }
    return (SyncItem*)deletedItems.get(cdeleted)->clone();
}

void TestSyncSource::setItemStatus(const WCHAR* key, int status) {
    sprintf(logmsg, "key: %ls, status: %i", key, status);
    LOG.debug(logmsg);
}

int TestSyncSource::addItem(SyncItem& item) {
    sprintf(logmsg, "added item: %ls", item.getKey());
    LOG.info(logmsg);

    LOG.info("Data:");
    char *data = new char [item.getDataSize()];
    memcpy(data, item.getData(), item.getDataSize());
    data[item.getDataSize()] = 0;
    LOG.info(data);
    delete [] data;

    WCHAR *luid = new WCHAR[wcslen(item.getKey())+10];
    wsprintf(luid, TEXT("%s-luid"), item.getKey());
    item.setKey(luid);

    return 200;
}

int TestSyncSource::updateItem(SyncItem& item) {
    sprintf(logmsg, "updated item: %ls", item.getKey());
    LOG.info(logmsg);

    LOG.info("Data:");
    char *data = new char [item.getDataSize()];
    memcpy(data, item.getData(), item.getDataSize());
    data[item.getDataSize()] = 0;
    LOG.info(data);
    delete [] data;

    return 200;
}

int TestSyncSource::deleteItem(SyncItem& item) {
    sprintf(logmsg, "deleted item: %ls", item.getKey());
    LOG.debug(logmsg);
    return 200;
}

int TestSyncSource::beginSync() {
    sprintf(logmsg, "Begin sync TestSyncSource");
    LOG.debug(logmsg);
    return 0;
}
int TestSyncSource::endSync() {
    sprintf(logmsg, "End sync TestSyncSource");
    LOG.debug(logmsg);
    return 0;
}

SyncItem* TestSyncSource::getFirstItemKey() {
    return NULL;
}

SyncItem* TestSyncSource::getNextItemKey() {
    return NULL;
}
