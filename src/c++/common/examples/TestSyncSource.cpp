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

#include "examples/TestSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

USE_NAMESPACE

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

int setModifiedItems() {

    SyncItem item;
                StringBuffer val; 
                for (int ii = 0; ii<1; ii++){
                    val.append("This is the updated item one");
                }

    for (int i = 0; i < 4; ++i) {

        switch (i) {

            case 0:
                item.setKey(TEXT("item5"));
                item.setData(val.c_str()  , (val.length())*sizeof(char));
                newItems.add(item);
                break;

            case 1:
                item.setKey(TEXT("item1"));
                item.setData(val.c_str()  , (val.length())*sizeof(char));
                updatedItems.add(item);
                break;

            case 2:
                item.setKey(TEXT("item3"));
                item.setData(val.c_str()  , (val.length())*sizeof(char));
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
    char* val = toMultibyte(key);
    LOG.debug("key: %s, status: %i", val, status);
    if(val){
        delete [] val;
        val = 0;
    }
}

int TestSyncSource::addItem(SyncItem& item) {
    char* val = toMultibyte( item.getKey() );
    LOG.info("added item: %s", val);
    if(val){
        delete [] val;
        val = 0;
    }
    char *data = new char [item.getDataSize()];
    memcpy(data, item.getData(), item.getDataSize());
    data[item.getDataSize()] = 0;
    LOG.info("Data: %s", data);
    delete [] data;

    WCHAR *luid = new WCHAR[wcslen(item.getKey())+10];
    wsprintf(luid, TEXT("%s-luid"), item.getKey());
    item.setKey(luid);

    return 200;
}

int TestSyncSource::updateItem(SyncItem& item) {
    char* val = toMultibyte( item.getKey() );
    LOG.info("updated item: %s", val);
    if(val){
        delete [] val;
        val = 0;
    }

    char *data = new char [item.getDataSize()];
    memcpy(data, item.getData(), item.getDataSize());
    data[item.getDataSize()] = 0;
    LOG.info("Data: %s", data);
    delete [] data;

    return 200;
}

int TestSyncSource::deleteItem(SyncItem& item) {
    char* val = toMultibyte( item.getKey() );
    LOG.info("deleted item: %s", val);
    if(val){
        delete [] val;
        val = 0;
    }
    return 200;
}

int TestSyncSource::removeAllItems() {
    return 0;
}

int TestSyncSource::beginSync() {
    LOG.debug("Begin sync TestSyncSource");
    return 0;
}
int TestSyncSource::endSync() {
    LOG.debug("End sync TestSyncSource");
    return 0;
}

SyncItem* TestSyncSource::getFirstItemKey() {
    return NULL;
}

SyncItem* TestSyncSource::getNextItemKey() {
    return NULL;
}
