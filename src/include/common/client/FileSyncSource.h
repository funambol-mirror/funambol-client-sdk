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
    FileSyncSource(const WCHAR* name, SyncSourceConfig* sc);
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
    SyncItem* getFirstDeletedItem() { return getFirst(deletedItems, FALSE); }
    SyncItem* getNextDeletedItem() { return getNext(deletedItems, FALSE); }
    SyncItem* getFirstItemKey() { return getFirst(allItems, FALSE); }
    SyncItem* getNextItemKey() { return getNext(allItems, FALSE); }
    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);
    int deleteItem(SyncItem& item);
    void setItemStatus(const WCHAR* key, int status);
    int beginSync();
    int endSync();
    void assign(FileSyncSource& s);
    ArrayElement* clone();

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

    SyncItem* getFirst(ItemIteratorContainer& container, BOOL getData = TRUE);
    SyncItem* getNext(ItemIteratorContainer& container, BOOL getData = TRUE);
};

/** @} */
/** @endcond */
#endif
