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

#ifndef INCL_CACHE_SYNC_SOURCE
#define INCL_CACHE_SYNC_SOURCE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/AbstractSyncSourceConfig.h"
#include "spds/SyncSourceReport.h"
#include "spds/SyncSource.h"
#include "syncml/core/TagNames.h"
#include "base/util/Enumeration.h"
#include "base/util/KeyValueStore.h"
#include "base/util/KeyValuePair.h"
#include "event/FireEvent.h"

BEGIN_NAMESPACE

/**
 * This class class implements the SyncSource interface, adding a method to
 * detect the changes in the local store since the last sync based on cache
 * files to make easier the implementation of new sync sources. 
 *
 * It requires an instance of a class implementing the KeyValueStore interface
 * to store the sync cache, made by pairs of LUID (the local id of the item) and
 * a fingerprint (default method is CRC of the content, but can be a timestamp
 * or any other way to detect a change on the item). By default, CacheSyncSource 
 * is able to obtain a PropertyFile (which implements KeyValueStore as a file), 
 * but if a more efficient way to store it is available for the platform, the
 * developer can create anoher store and pass it in the CacheSyncSource constructor
 * (see also the SQLKeyValueStore abstract class).
 *
 * The mandatory methods to implement are:
 * <li><b>getAllItemList</b>: returns a list of StringBuffer with all the keys
 *      of the items in the data store</li>
 * <li><b>insertItem</b>: adds a new item into the data store</li>
 * <li><b>modifyItem</b>: modifies an item in the data store</li>
 * <li><b>removeItem</b>: removes an item from the data store</li>
 * <li><b>removeAllItems</b>: removes all the items from the data store</li>
 * <li><b>getItemContent</b>: get the content of an item given the key</li>
 *
 * The optional methods, that can be overloaded to change the behavior of the
 * user sync source, are:
 * <li><b>getItemSignature</b>: get a fingerprint of the item, which is any
 *      string which allows to detech changes in the item</li>
 *
 */
class CacheSyncSource : public SyncSource {

private:
         
    /**
     * Used to store a KeyValuePair containing the key and the command
     * associated to the item. It stores the cache:
     * <li>during the slow sync. After the allKeys is populated, for every
     * item status sent back by the server, the cache is populated. It is possible
     * to write down when needed.
     * - during the two-way sync it is populated at the beginning to understand
     * the modification. This action populates the newKeys, updatedKeys and deletedKeys.
     * For every item status sent back by the server the cache is updated
     *    
     */    
    KeyValueStore* cache; 


protected:
    
    /**
     * Enumeration of the new keys
     */
    Enumeration*   newKeys;

    /**
     * Enumeration of the updated keys
     */
    Enumeration*   updatedKeys;

    /**
     * Enumeration of the deleted keys
     */
    Enumeration*   deletedKeys;       
    
    /**
     * Used to store the keys of all items for a slow based sync
     * It is an enumeration of StringBuffer keys
     */
    Enumeration* allKeys;
    
    
    /**
     * Fills the sync item given the key. It is used by the method getXXXItem to
     * complete the SyncItem.
     * @param fillData [OPTIONAL] if false, don't set the SyncItem data
     *                 (sets only the key, for deleted items). Default = true.
     */
    virtual SyncItem* fillSyncItem(StringBuffer* key, const bool fillData = true);       

    /**
     * Utility method that populates the keyValuePair with 
     * the couple key/signature starting from the SyncItem.
     * Used in the addItem and updateItem
     *
     * @param item - IN:  the SyncItem
     * @param kvp  - OUT: the KeyValuePair to be populate
     */
    virtual void getKeyAndSignature(SyncItem& item, KeyValuePair& kvp);
    
    
    /**
     * The way to calculate the cache is the follow:
     * loop on the current element against an array list
     * that has the cache. It is the copy of the original cache.
     * When an current element is found in the cache, it is removed
     * from the cache copy. At the end the remained element
     * in the cache are the deleted ones.
     * Called when the two-way sync is requested
     */
    virtual bool fillItemModifications();
    
  
    /**
     * Save the current cache into the persistent store. Which store depends on
     * the KeyValueStore passed in the constructor (a file by default).
     */
    virtual int saveCache();
  
    /**
     * Implementation of the SyncSource method addItem, it's called by the SyncManager
     * to add an item that the server has sent.
     * It calls the insertItem() method that must be implemented by the user. 
     * Also used to update the item 
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    int addItem(SyncItem& item);

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    int updateItem(SyncItem& item);

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    int deleteItem(SyncItem& item);

    /**
     * Used to update the cache adding, replacing or deleting. 
     * The KeyValuePair contains the pair UID/signature. It is provided
     * by the proper method who calls this. It udpates the cache 
     * that is in memory.
     * The action by default is Replace. 
     */
    int updateInCache(KeyValuePair& k, const char* action = REPLACE);
        
    /**
    * To insert in the cache. 
    */
    int insertInCache(KeyValuePair& k) {
        return updateInCache(k, ADD);
    }
    
    /**
    * To remove from cache
    */
    int removeFromCache(KeyValuePair& k) {
        return updateInCache(k, DEL);
    }    
    
    /**
     * Clear the cache using the removeAllProperties method.
     */
    int clearCache() {
        return (cache->removeAllProperties() || saveCache());
    }
    
    /**
     * Returns the value of the given property, from the cache.
     * @param prop - the property name
     * @return  A NULL StringBuffer in the returned implies that
     *          the property was not set. Otherwise the value it was
     *          set to is returned (which can be "", the empty string).
     */
    StringBuffer readCachePropertyValue(const char* prop);

public:

    /**
     * Constructor: create a CacheSyncSource with the specified name
     *
     * @param name   the name of the SyncSource
     * @param sc     configuration for the sync source: the instance
     *               must remain valid throughout the lifetime of the
     *               sync source because it keeps a reference to it
     *               and uses it as its own. A NULL pointer is allowed
     *               for unit testing outside of the sync framework;
     *               the sync source then references a global config
     *               instance to avoid crashes, but modifying that config
     *               will not make much sense. The pointer may also be
     *               set directly after creating the SyncSource, which
     *               is useful when a derived class creates the config
     *               in its own constructor.
     * @param cache  the store for the cache. Released by the CacheSyncSource
     *                
     */
    CacheSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc, 
                        KeyValueStore* cache = NULL);
        
    // Destructor
    virtual ~CacheSyncSource();         
                               
    /**
     * called by the sync engine with the status returned by the
     * server for a certain item that the client sent to the server.
     * It contains also the proper command associated to the item.
     * It is used to update the current array of cache.
     *
     * @param wkey     - the local key of the item (wide char)
     * @param status   - the SyncML status returned by the server
     * @param command  - the SyncML command associated to the item
     * 
     */
    virtual void setItemStatus(const WCHAR* wkey, int status, const char* command);       
    
    /**
     * Return the first SyncItem of all.
     * It is used in case of slow sync
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getFirstItem();

    /**
     * Return the next SyncItem of all.
     * It is used in case of slow sync
     * and retrieve the entire data source content.
     */
    SyncItem* getNextItem();

    /**
     * Return the first SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstNewItem();

    /**
     * Return the next SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextNewItem();

    /**
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstUpdatedItem();

    /**
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem();

    /**
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem();

    /**
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    SyncItem* getNextDeletedItem();
    
    /**
     * Indicates that all the server status of the current package 
     * of the client items has been processed by the engine.
     * This signal can be useful to update the modification arrays
     * NOT USED at the moment
     */
    void serverStatusPackageEnded() {};    
    
    /**
     * Indicates that all the client status of the current package 
     * of the server items that has been processed by the client and 
     * are going to be sent to the server.
     * This signal can be useful to update the modification arrays
     * NOT USED at the moment
     */
    void clientStatusPackageEnded() {};        

    /**
     * Check that the cache store is available before starting the sync.
     */
    virtual int beginSync();

    /**
     * In the first implementatation, in which serverStatusPackageEnded and 
     * clientStatusPackageEnded are not yet impelemented, the end sync
     * will udpate the whole cache status persistently.
     */
    virtual int endSync();       
    
    /**
    * Get the signature of an item given the key. The signature could be
    * a crc computation or a timestamp or whatever can identify uniquely the
    * content of an item. The default implementation uses a 
    * crc computation of the value. Overriding implementation could provide
    * something different like the timestamp or other...
    *
    * @param key    the key of the item. 
    * @return       the signature of the selected item
    */    
    virtual StringBuffer getItemSignature(StringBuffer& key);

    /**
    * Get the content of an item given the key. It is used to populate
    * the SyncItem before the engine uses it in the usual flow of the sync.      
    *
    * @param key      the local key of the item
    * @param size     OUT: the size of the content
    */
    virtual void* getItemContent(StringBuffer& key, size_t* size) = 0;
    
            
    /**
     * Returns an Enumeration containing the StringBuffer keys of all items. 
     *
     * It is used both for the full sync, where all items are sent to the server,
     * and for the fast sync to calculate the modification since the last 
     * successful sync.
     * 
     * @return a newly allocated Enumeration that is free'd by the CacheSyncSource
     *         CacheSyncSource. 
     *         Return NULL in case of error, an empty Enumeration
     *         if there are no items.     
     */
    virtual Enumeration* getAllItemList() = 0;      
    
    /**
     * Called by the sync engine to add an item that the server has sent.
     * The sync source is expected to add it to its database, then set the
     * key to the local key assigned to the new item. Alternatively
     * the sync source can match the new item against one of the existing
     * items and return that key.
     *
     * @param item  the item as sent by the server
     * @return      SyncML status code 
     */
    virtual int insertItem(SyncItem& item) = 0;

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item  the item as sent by the server
     * @return      SyncML status code
     */
    virtual int modifyItem(SyncItem& item) = 0;

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     *
     * @param item  the item as sent by the server
     * @return      SyncML status code
     */
    virtual int removeItem(SyncItem& item) = 0;
    
    /**
     * Called by the setItemStatus, addItem, updateItem, deleteItem to decide if
     * the code is an error code or not. Based on the result, is udpates the cache
     * or leave. Currently it consider good values the renge between 200 <= code < 300
     * and code = 418. The remaining code are errors.
     *
     * @code the code to be analyzed
     * @return true if it is an error code, false otherwise
     *
     */
    virtual bool isErrorCode(int code);  

    /**
     * Fires the total number of the item from client side to the associated listener.      
     * It is called after the getAllItemList and fillItemModifications. 
     * Currently it is not used...
     * @number the items the number of items
     */
    virtual void fireClientTotalNumber(int number);
};

END_NAMESPACE

/** @} */
/** @endcond */
#endif
