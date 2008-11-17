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

#ifndef INCL_BRIEFCASE_CACHE_SYNC_SOURCE
#define INCL_BRIEFCASE_CACHE_SYNC_SOURCE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spdm/ManagementNode.h"
#include "base/util/ItemContainer.h"
#include "spds/FileData.h"
#include "client/CacheSyncSource.h"


#define DEFAULT_BRIEFCASE_DIR   "."

/**
 * Synchronizes the content of files in a certain directory and handles
 * the modification and cache or listener structure
 *
 */
class BriefcaseCacheSyncSource : public CacheSyncSource {

private:   
    
    StringBuffer dir;

protected:
   
    // The copy is protected
    //BriefcaseSyncSource(const BriefcaseSyncSource& s);
    
public:

    BriefcaseCacheSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc);
    ~BriefcaseCacheSyncSource();
      
    void assign(BriefcaseCacheSyncSource& s);
    
    /**
    * set/get the directory where to sync the files
    */
    void setDir(const char* p) { dir = p; }
    const StringBuffer& getDir() { return dir; };
    
    /**
    * Save the file inside the filesystem. 
    * Return 0 is success otherwise failure
    */
    int saveFileData(FileData& file);
    
    /**
    * Save the file inside the filesystem. 
    * It doesn't use the FileData SyncMLobject but
    * it uses only the data as inside
    *
    * @param item       the item from which get the data
    * @param isUpdate   it says if the item must to be add (false)
    *                   or updated (true)
    *                        
    */
    int saveFileData(SyncItem& item, bool isUpdate);

    /**
    * Parse the file object
    * Return 0 is success otherwise failure
    */    
    int parseFileData(FileData& file, SyncItem& item);

    /**
    * Get the list of all the keys stored in a StringBuffer. It reads all the 
    * files name in the directory. The directory is set in the sync source.
    */
    Enumeration* getAllItemList();
    
    /**
     * Removes all the item of the sync source. It is called 
     * by the engine in the case of a refresh from server to clean      
     * all the client items before receiving the server ones.
     */
    int removeAllItems();

    /**
     * Called by the sync engine to add an item that the server has sent.
     * The sync source is expected to add it to its database, then set the
     * key to the local key assigned to the new item. Alternatively
     * the sync source can match the new item against one of the existing
     * items and return that key.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    int insertItem(SyncItem& item);
    
    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    int modifyItem(SyncItem& item);

     /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     *
     * @param item    the item as sent by the server
     */
    int removeItem(SyncItem& item);
    
     /**
    * Get the content of an item given the key. It is used to populate
    * the SyncItem before the engine uses it in the usual flow of the sync.
    * It is used also by the itemHandler if needed 
    * (i.e. in the cache implementation)
    *
    * @param key      the local key of the item
    * @param size     OUT: the size of the content
    */
    void* getItemContent(StringBuffer& key, size_t* size);
      
        
};

/** @} */
/** @endcond */
#endif
