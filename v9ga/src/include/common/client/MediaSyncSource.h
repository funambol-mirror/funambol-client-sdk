/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
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

#ifndef MEDIASOURCESYNC_H_
#define MEDIASOURCESYNC_H_

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "base/util/ItemContainer.h"
#include "spds/FileData.h"
#include "client/CacheSyncSource.h"
#include "client/FileSyncSource.h"
#include "http/HttpUploader.h"
#include "client/MediaSyncSourceParams.h"

BEGIN_NAMESPACE


/**
 * This class extends the FileSyncSource class, to define a special behavior for generic
 * "media files" to be synchronized between the Server and a mobile Client.
 * 
 * Differences from FileSyncSource are:
 * - cache file is stored inside the 'dir' folder (the folder under sync)
 * - in case the URL or username changes, the cache file is resetted
 * - manages a mapping between the LUID and the items keys (full paths), to ensure the items are not sent 2 times.
 *   This LUID_Map is stored inside the 'dir' folder (the folder under sync)
 */
class MediaSyncSource : public FileSyncSource
{
 
public:
    MediaSyncSource(const WCHAR* wname,
                   AbstractSyncSourceConfig* sc,
                   const StringBuffer& aDir, 
                   MediaSyncSourceParams mediaParams);

    ~MediaSyncSource();
    
    /**
     * Overrides FileSyncSource::beginSync().
     * Checks if the pictures cache is still valid, before starting a sync.
     * If not, the cache is cleared.
     */
    int beginSync();
    
    /// Overrides FileSyncSource::insertItem - implemented empty.
    int insertItem(SyncItem& item);
    
    /// Overrides FileSyncSource::modifyItem - implemented empty.
    int modifyItem(SyncItem& item);
    
    /// Overrides FileSyncSource::removeItem - implemented empty.
    int removeItem(SyncItem& item);
    
    /**
     * Overrides CacheSyncSource::getItemSignature().
     * Gets the signature of an item given its full path. 
     * The signature is the timestamp of last modification time.
     *
     * @param key  the key of the item (its full path and name)
     * @return     the signature of the selected item (last modification time)
     */
    StringBuffer getItemSignature(StringBuffer& key);
   
    /**
     * Overwrite the way to send the total number of items to the client.
     * This is because the MediaSyncSource has to remove some fake items in the
     * cache file (see fillItemModifications()), it was called by CacheSyncSource. 
     * It should not be overloaded...
     * @number of items that will be updated
     */
    virtual void fireClientTotalNumber(int number);
    
    /**
     * Overrides CacheSyncSource::setItemStatus().
     * The key received from Server is the item's LUID.
     * We need to retrieve the item's path from the LUID (full path is the key
     * fro the cache).
     */
    void setItemStatus(const WCHAR* wkey, int status, const char* command);
    
    /**
     * Overrides CacheSyncSource::endSync().
     * This is the key method for the MediaSyncSource class:
     * all the files (listed in the LUIDsToSend array) are uploaded via HTTP here,
     * using the HttpUploader module.
     * When each file upload is done, the cache file is updated.
     * The cache file is finally saved at the end.
     * 
     * @return the syncsourceReport's lastErrorCode
     */
    int endSync();
    
    /**
     * Called by the setItemStatus to decide if the code is an error code or not. 
     * Based on the result, is udpates the cache/LUIDsToSend or not.
     * 
     * Overrides CacheSyncSource::isErrorCode() in order to consider the code 418
     * (STC_ALREADY_EXISTS) as an error status: we don't want to send items twice!
     * 
     * @code the code to be analyzed
     * @return true if it is an error code, false otherwise
     */
    virtual bool isErrorCode(int code);  

    
protected:
    
    /// Contains parameters used by this class.
    MediaSyncSourceParams params;
    
    /// UTC string of the params::filterByDate (just for debugging).
    StringBuffer filterDateString;

    /**
     * It's a list of LUIDs to be sent in endSync(), via HTTP upload.
     * When the status of a metadata is received (setItemStatus() method), 
     * if the status code is OK the item's LUID is added to this list.
     * This list lives only for the current sync session, it is not stored anywhere.
     */
    ArrayList LUIDsToSend;
 
    /**
     * Map of: Full items Path <-> Items LUID
     * The LUID is an incremental number, different for every new item.
     * It's used as the SyncItem's key when sending the items to the Server, 
     * to make sure we never send 2 items with the same key.
     * The map is stored as a PropertyFile in the 'dir' folder.
     */
    KeyValueStore* LUIDMap;
    
    /**
     * Overrides CacheSyncSource::fillSyncItem().
     * The SyncItem key set is the LUID of this item.
     * It is used by the method getXXXItem to
     * complete the SyncItem.
     */
    virtual SyncItem* fillSyncItem(StringBuffer* key, const bool fillData = true);       

    /**
     * Overrides CacheSyncSource::getKeyAndSignature().
     * Utility method that populates the keyValuePair with 
     * the couple key/signature starting from the SyncItem.
     * The SyncItem key set is the LUID of this item.
     * Used in the addItem and updateItem
     *
     * @param item - IN:  the SyncItem
     * @param kvp  - OUT: the KeyValuePair to be populate
     */
    virtual void getKeyAndSignature(SyncItem& item, KeyValuePair& kvp);
    
    
    /**
     * Overrides CacheSyncSource::getFirstItem().
     * Return the first SyncItem of ALL.
     * It is used in case of slow sync and retrieve the entire data source content.
     * 
     * Compared to te CacheSyncSource implementation:
     *   - applies the (dynamic) filtering on the allItems keys (like the date filtering).
     */
    SyncItem* getFirstItem();
    
    /**
     * Overrides CacheSyncSource::getFirstNewItem().
     * Return the first SyncItem of NEW. It is used in case of fast sync
     * and retrieve the new data source content.
     * 
     * Compared to te CacheSyncSource implementation:
     *   - cleans up the special properties found in the MEDIA cache (url, username, swv).
     *   - applies the (dynamic) filtering on the newItems keys (like the date filtering).
     */
    virtual SyncItem* getFirstNewItem();
    

    /**
     * Used to filter outgoing items (overrides FileSyncSource::filterOutgoingItem)
     * Filters the cache files (*.dat).
     * NOTE: this is only STATIC filtering. Meaning that these filters CANNOT 
     *       be changed/disabled anytime by the Client, otherwise a change in 
     *       the filter may result in the Client to send deleted items for 
     *       files fitered out. See dynamicFilterItem() for dynamic filtering.
     * 
     * @param fullName  the full path + name of the file to check
     * @param st        reference to struct stat for current file
     * @return          true if the item has to be filtered out (skipped)
     *                  false if the item is ok
     */ 
    virtual bool filterOutgoingItem(const StringBuffer& fullName, struct stat& st);
    
    /**
     * Filters a specific item (dynamic filtering).
     * This is called AFTER the cache is created, so this is the place to
     * set a custom filter that may change or be disabled over time.
     * Can be reimplemented by derived classes to specify different filterings.
     * Current filters:
     *   1. files with size > filterBySize
     *   2. if the filterByDate is set, files with modification date < filterByDate
     *   
     * @param fileName   the full name of the file to be checked
     * @return           true if the file must be filtered out (not sent), false otherwise
     */
    virtual bool dynamicFilterItem(const StringBuffer& fileName);
    
    /**
     * Used to filter dynamically outgoing items.
     * Calls dynamicFilterItem() for all items in the passed array.
     * This method is called from getFirstItem and getFirstNewItem, just before firing 
     * the total number of client's items.
     * 
     * @param itemKeys  the Enumeration of items to be sent (newKeys for a fast sync,
     *                  allKeys for a slow sync), to apply the filter
     */
    void dynamicFilterItems(Enumeration* itemKeys);
    
    /**
     * Overrides CacheSyncSource::saveCache().
     * Saves the cache and the LUID_Map into the persistent store.
     * Adds the special properties (url, username, swv) to the cache file.
     * Set the files attribute to hidden (TODO).
     */
    virtual int saveCache();
    
    
    /**
     * Utility method to retrieve the LUID of an item, given its path.
     * This is called for outgoing items: we send a LUID as key to the Server.
     */
    StringBuffer getLUIDFromPath(const StringBuffer& path);
    
    /**
     * Utility method to retrieve the full path of an item, given its LUID.
     * This is called for incoming items and when receiving item's status: 
     * we need the full path to update the cache.
     * Note: returns a NULL StringBuffer if LUID not found in the LUIDMap.
     * 
     * @param luid  the item's LUID
     * @return      the item's full path (NULL StringBuffer if LUID not found)
     */
    StringBuffer getPathFromLUID(const StringBuffer& luid);
    
    /**
     * Used to set errors in the MediaSyncSource.
     * Will set the source state (error state), the source's last error 
     * code and source's last error message.
     * @param  errorCode  the error code
     */
    void setSourceError(const int errorCode);

    /**
     * Returns a new allocated HttpUploader.
     * It's used by the endSync() method, the returned object is then deleted.
     */
    virtual HttpUploader* getHttpUploader() {
        return new HttpUploader();
    }
    
private:
    
    /**
     * Used to store KeyValuePairs containing config parameters used by this class.
     * It is saved under the system config folder, with name "<sourcename>_params.ini".
     * Actually tho only parameter used is 'nextLUID', which is an incremental number
     * used as the item's key when sending items to the Server (to be sure the key is unique).
     */
    KeyValueStore* configParams;  
    
    /**
     * Read the URL, username and client sw version from the cache. 
     * If wrong (different from the passed ones) or missing, the current cache is not valid (true is returned).
     * This method is called by MediaSyncSource::beginSync().
     * 
     * @return  true if the cache file is valid, false if not.
     */
    bool checkCacheValidity();

    /**
     * Utility method: scans the LUIDMap and check if there's a LUID >= than the passed one.
     * If so, updates the params::nextLUID value and returns true.
     * This method is called just once in the constructor.
     */
    bool verifyNextLUIDValue();
    
    /**
     * Utility method: scans and compress the LUIDMap PropertyFile, removing
     * all entries that have no correspondence in the cache.
     * This method is called before saving the LUIDMap file at the end of sync
     * to avoid the propertyfile growing indefinitely.
     * @return  true if the LUIDMap was resized
     */
    bool refreshLUIDMap();
    
    
    /**
     * Reads the nextLUID value from the KeyValueStore, under config folder.
     * @return the nextLUID value read
     */
    const int readNextLUID();

    /**
     * Stores the nextLUID value in the KeyValueStore, under config folder.
     * This method is called in case a new item is found locally, and so 
     * a new LUID is generated for it.
     * @param nextLUID  the updated value of nextLUID. 
     */
    void saveNextLUID(const int nextLUID);
    
};

END_NAMESPACE

#endif /*MEDIASOURCESYNC_H_*/
