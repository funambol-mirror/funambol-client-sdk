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

#ifndef INCL_MANY_ITEMS_TEST_SYNC_SOURCE
#define INCL_MANY_ITEMS_TEST_SYNC_SOURCE
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"
#include "spds/SyncSourceConfig.h"
#include "base/globalsdef.h"


BEGIN_NAMESPACE


/**
 * Simple SyncSource to generate N new items (vCard or vCal), for integration tests.
 * The number of new items to generate can be passed in constructor (default = 100).
 * The new items are generated both in case of slow-sync and two-way-sync.
 */
class  ManyItemsTestSyncSource : public SyncSource {

private:
    
    // The number of new items created during sync (passed in constructor)
    int numNewItems;

    // internal counter to make the item's key unique
    int count;

    // it returns a unique vCard 2.1 
    StringBuffer getNewCard();

    // it returns a unique vCal 1.0
    StringBuffer getNewCal();


public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     * @param sc   - the SyncSourceConfig
     * @param numItems - the number of new items created during sync, default = 100
     */
    ManyItemsTestSyncSource(const WCHAR* name, SyncSourceConfig *sc, int numItems = 100);

    ~ManyItemsTestSyncSource() {}


    SyncItem* getFirstItem()        { return getNextItem(); }

    /// Returns a new SyncItem, until count = numNewItems.
    SyncItem* getNextItem()         { return getNextNewItem(); }
 
    SyncItem* getFirstNewItem()     { return getNextNewItem(); }

    /// Returns a new SyncItem, until count = numNewItems.
    SyncItem* getNextNewItem();

    SyncItem* getFirstUpdatedItem() { return NULL; }
    SyncItem* getNextUpdatedItem()  { return NULL; }
    SyncItem* getFirstDeletedItem() { return NULL; }
    SyncItem* getNextDeletedItem()  { return NULL; }

    void setItemStatus(const WCHAR* key, int status);

    int addItem   (SyncItem& item);
    int updateItem(SyncItem& item)  { return STC_OK; }
    int deleteItem(SyncItem& item)  { return STC_OK; }
    
    int removeAllItems()            { return 0; }
    int beginSync();
    int endSync();

    ArrayElement* clone()           { return 0; }
};



END_NAMESPACE

/** @endcond */
#endif
