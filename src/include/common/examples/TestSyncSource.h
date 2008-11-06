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

#ifndef INCL_TEST_SYNC_SOURCE
#define INCL_TEST_SYNC_SOURCE
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"

class  TestSyncSource : public SyncSource {

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     * @param sc   - the SyncSourceConfig
     */
    TestSyncSource(const WCHAR* name, SyncSourceConfig *sc) ;

    // TestSyncSource
    ~TestSyncSource();

    /*
     * Return the first SyncItem of all.
     * It is used in case of slow or refresh sync
     * and retrieve the entire data source content.
     */
    SyncItem* getFirstItem();

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync
     * and retrieve the entire data source content.
     */
    SyncItem* getNextItem();

    /*
     * Return the first SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstNewItem();

    /*
     * Return the next SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextNewItem();

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstUpdatedItem() ;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem();

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem();

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */

    SyncItem* getFirstItemKey();

    /*
     * Return the key of the next SyncItem of all.
     * It is used in case of refresh sync
     * and retrieve all the keys of the data source.
     */
    SyncItem* getNextItemKey();


    SyncItem* getNextDeletedItem();

    void setItemStatus(const WCHAR* key, int status);

    int addItem(SyncItem& item);

    int updateItem(SyncItem& item);

    int deleteItem(SyncItem& item);

    int beginSync();

    int endSync();

    ArrayElement* clone() { return 0; }
};

/** @endcond */
#endif
