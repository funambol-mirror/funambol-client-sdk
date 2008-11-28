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

#include "examples/TestSyncSource2.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/globalsdef.h"

USE_NAMESPACE

static int all      = -1;
static int cnew     = -1;
static int cupdated = -1;
static int cdeleted = -1;
ArrayList Items2;
ArrayList newItems2;
ArrayList updatedItems2;
ArrayList deletedItems2;


int setAllItems2() {

    SyncItem item;
    WCHAR name[64];
    char data[128];

    for (int i = 0; i < 4; ++i) {

        wsprintf(name, TEXT("%s%d"), TEXT("item"), i);
        sprintf(data, "This is item in test2 %d", i);

        item.setKey(name);
        item.setData(data  , (strlen(data))*sizeof(char));
        Items2.add(item);
    }
    return 0;
}

int setAllItems2Empty() {
    return 0;
}

int setModifiedItems2() {

    SyncItem item;

    for (int i = 0; i < 4; ++i) {

        switch (i) {

            case 0:
                item.setKey(TEXT("item5"));
                item.setData("This is a new item Four test2"  , 29*sizeof(char));
                newItems2.add(item);
                break;

            case 1:
                item.setKey(TEXT("item1"));
                item.setData("This is the updated item One test2"  , 34*sizeof(char));
                updatedItems2.add(item);
                break;

            case 2:
                item.setKey(TEXT("item3"));
                item.setData("This is the updated item Three test2", 36*sizeof(char));
                updatedItems2.add(item);
                break;

            case 3:

                item.setKey(TEXT("item4"));
                deletedItems2.add(item);
                break;

        }
    }

    return 0;
}

int setModifiedItems2Empty() {
    return 0;
}


TestSyncSource2::TestSyncSource2(const WCHAR* name, SyncSourceConfig *sc) : SyncSource(name, sc) {
}

TestSyncSource2::~TestSyncSource2() {
}

/*
* Return the first SyncItem of all. It is used in case of slow or refresh sync
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource2::getFirstItem() {

    setAllItems2();
    if (Items2.size() == 0) {
        return NULL;
    }
    all = 0;
    return (SyncItem*)Items2[all]->clone();
}

/*
* Return the next SyncItem of all. It is used in case of slow or refresh sync
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource2::getNextItem() {
    all++;
    if (all == Items2.size()) {
        return NULL;
    }
    return (SyncItem*)Items2.get(all)->clone();
}

SyncItem* TestSyncSource2::getFirstNewItem() {
    setModifiedItems2();
    if (newItems2.size() == 0) {
        return NULL;
    }
    cnew = 0;
    return (SyncItem*)newItems2.get(cnew)->clone();
}

SyncItem* TestSyncSource2::getNextNewItem() {
    cnew++;
    if (cnew == newItems2.size()) {
        return NULL;
    }
    return (SyncItem*)newItems2.get(cnew)->clone();
}

SyncItem* TestSyncSource2::getFirstUpdatedItem() {

    if (updatedItems2.size() == 0) {
        return NULL;
    }
    cupdated = 0;
    return (SyncItem*)updatedItems2.get(cupdated)->clone();
}

SyncItem* TestSyncSource2::getNextUpdatedItem() {
    cupdated++;
    if (cupdated == updatedItems2.size()) {
        return NULL;
    }
    return (SyncItem*)updatedItems2.get(cupdated)->clone();
}

SyncItem* TestSyncSource2::getFirstDeletedItem() {

    if (deletedItems2.size() == 0) {
        return NULL;
    }
    cdeleted = 0;
    return (SyncItem*)deletedItems2.get(cdeleted)->clone();
}

SyncItem* TestSyncSource2::getNextDeletedItem() {
    cdeleted++;
    if (cdeleted == deletedItems2.size()) {
        return NULL;
    }
    return (SyncItem*)deletedItems2.get(cdeleted)->clone();
}

void TestSyncSource2::setItemStatus(const WCHAR* key, int status) {
    LOG.debug("key: %s, status: %i", key, status);
}

int TestSyncSource2::addItem(SyncItem& item) {
    char* val = toMultibyte(item.getKey());
    LOG.info("added item: %s", val);
    if (val){
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
    delete [] luid;
    return 200;
}

int TestSyncSource2::updateItem(SyncItem& item) {
    char* val = toMultibyte( item.getKey() );
    LOG.info("updated item: %s", val);
    if (val){
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

int TestSyncSource2::deleteItem(SyncItem& item) {
    LOG.debug("deleted item: %s", item.getKey());
    return 200;
}

int TestSyncSource2::beginSync() {
    LOG.debug("Begin sync TestSyncSource2");
    return 0;
}
int TestSyncSource2::endSync() {
    LOG.debug("End sync TestSyncSource2");
    return 0;
}
