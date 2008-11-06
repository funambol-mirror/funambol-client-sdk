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

#ifndef INCL_FILE_SYNC_SOURCE
#define INCL_FILE_SYNC_SOURCE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"
#include "spdm/ManagementNode.h"
#include "base/util/ItemContainer.h"


#define ERR_FILE_SYSTEM             1
#define ERR_NO_FILES_TO_SYNC        2
#define ERR_BAD_FILE_CONTENT        3
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * Synchronizes the content of files in a certain directory and the
 * file attributes using a certain XML format.
 *
 * @todo document what that XML format is
 * @todo updateItem() is not implemented
 */

class FileSyncSource : public SyncSource {

protected:
    
    // The dir in which the files are and that are to be synced.
    char* dir;

    // The copy is protected
    FileSyncSource(SyncSource& s);
    
    // Return true if data correctly set: syncItem->getKey() contains
    // the file name relative to dir, copying its content into
    // the items data can be overriden by derived classes.
    virtual bool setItemData(SyncItem* syncItem);

    /**
     * must be called for each successfully added item
     *
     * @param item     the added item
     * @param key      the key of that item
     * @return SyncML status code, STC_ITEM_ADDED on success
     */
    int addedItem(SyncItem& item, const WCHAR* key);
      
public:
    FileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc);
    virtual ~FileSyncSource();
    
    /**
     * The directory synchronized by this source.
     *
     * @param p      an absolute or relative path to the directory
    */
    void setDir(const char* p);
    const char* getDir();
    
    /**
     * Tracking changes requires persistent storage: for each item sent
     * to the server a property is set to the item's modification time.
    *                        
     * The caller is responsible for storing these properties after
     * a successful sync and continues to own the node instance itself.
     *
     * During the next beginSync() the information will be used to
     * identify added, updated and deleted items.
    */
    void setFileNode(ManagementNode *mn) { fileNode = mn; }
    ManagementNode *getFileNode() { return fileNode; }

    /* SyncSource interface implementations follow */

    SyncItem* getFirstItem() { return getFirst(allItems); }
    SyncItem* getNextItem() { return getNext(allItems); }
    SyncItem* getFirstNewItem() { return getFirst(newItems); }
    SyncItem* getNextNewItem() { return getNext(newItems); }
    SyncItem* getFirstUpdatedItem() { return getFirst(updatedItems); }
    SyncItem* getNextUpdatedItem() { return getNext(updatedItems); }
    SyncItem* getFirstDeletedItem() { return getFirst(deletedItems, false); }
    SyncItem* getNextDeletedItem() { return getNext(deletedItems, false); }
    SyncItem* getFirstItemKey() { return getFirst(allItems, false); }
    SyncItem* getNextItemKey() { return getNext(allItems, false); }
    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);
    int deleteItem(SyncItem& item);
    void setItemStatus(const WCHAR* key, int status);
    int removeAllItems();
    int beginSync();
    int endSync();
    void assign(FileSyncSource& s);

  private:
    // Lists of all, new, update and deleted items
    // together with the current index.
    struct ItemIteratorContainer {
        ArrayList items;
        int index;
    } allItems, newItems, updatedItems, deletedItems;
    
    // an optional node in which file dates are stored to track changes
    ManagementNode* fileNode;
    
    /** returns time stored in fileNode for the given key, 0 if not found */
    unsigned long getServerModTime(const char* keystr);

    SyncItem* getFirst(ItemIteratorContainer& container, bool getData = true);
    SyncItem* getNext(ItemIteratorContainer& container, bool getData = true);
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
