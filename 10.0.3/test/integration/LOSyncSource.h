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

#ifndef INCL_LO_SYNC_SOURCE
#define INCL_LO_SYNC_SOURCE
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"
#include "spds/SyncSourceConfig.h"
#include "base/globalsdef.h"

#include "event/SetListener.h"

#include "event/SyncItemEvent.h"
#include "event/SyncItemListener.h"

BEGIN_NAMESPACE

class  LOSyncSource : public SyncSource {

private:

    int count;    
    // it returns a unique card 
    char* getNewCard(bool isAdd);
    
    // to load sif file
    bool useSif;

    // to send items during a slow sync
    bool useSlowSync;

    StringBuffer getSyncItemName();

    bool useAdd;
    bool useUpdate;

    bool useDataEncoding;

public:
    
    void setUseSif(bool v) { useSif = v; }
    bool getUseSif() { return useSif; }

    void setUseSlowSync(bool v) { useSlowSync = v; }
    bool getUseSlowSync() { return useSlowSync; }
    
    void setUseAdd(bool v) { useAdd = v; }
    bool getUseAdd() { return useAdd; }

    void setUseUpdate(bool v) { useUpdate = v; }
    bool getUseUpdate() { return useUpdate; }

    void setUseDataEncoding(bool v) { useDataEncoding = v; }
    bool getUseDataEncoding() { return useDataEncoding; }
    
    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     * @param sc   - the SyncSourceConfig
     */
    LOSyncSource(const WCHAR* name, SyncSourceConfig *sc) ;

   // 
    ~LOSyncSource() {}

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
    SyncItem* getFirstUpdatedItem();

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem();


    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem(){ return NULL; }

    SyncItem* getNextDeletedItem(){ return NULL; }

    void setItemStatus(const WCHAR* key, int status);

    int addItem(SyncItem& item);

    int updateItem(SyncItem& item) { return 200; }

    int deleteItem(SyncItem& item) { return 200; }
    
    int removeAllItems() { return 0; }
    int beginSync();

    int endSync();

    ArrayElement* clone() { return 0; }
};

END_NAMESPACE

/** @endcond */
#endif
