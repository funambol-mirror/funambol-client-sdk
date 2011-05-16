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

#ifndef SAPI_SYNC_SOURCE
#define SAPI_SYNC_SOURCE

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncSourceConfig.h"
#include "sapi/SapiSyncItemInfo.h"
#include "sapi/UploadSapiSyncItem.h"
#include "sapi/DownloadSapiSyncItem.h"
#include "base/util/ArrayListEnumeration.h"
#include "base/util/KeyValueStore.h"
#include "spds/SyncSourceReport.h"
#include "base/util/PropertyFile.h"
#include "base/adapter/PlatformAdapter.h"
#include "spds/spdsutils.h"
#include "spds/AbstractSyncConfig.h"

BEGIN_FUNAMBOL_NAMESPACE


// Entry keys for the resume map
#define RESUME_UPLOAD               "upload"
#define RESUME_DOWNLOAD             "download"
#define PROPERTY_EXTENSION          "extension"
#ifndef PROPERTY_FOLDER_PATH
#define PROPERTY_FOLDER_PATH        "folderPath"
#endif
#ifndef PROPERTY_MEDIAHUB_PATH
#define PROPERTY_MEDIAHUB_PATH      "mediaHubPath"
#endif

typedef enum ESapiSyncSourceError
{
    ESSSNoErr = 0,
    ESSSOperationCancelled, // operation cancelled by user
    ESSSItemNotSupported,   // sync source can't add this type of item (not supported by source backend)
    ESSSNoSpaceLeft,        // no space available left during insert
    ESSSPermissionDenied,   // permission error (on source backend)
    ESSSInvalidItem,        // item can't be added because is not valid
    ESSSInternalError       // configuration/setup general errors
} ESSSError; // sss_err_t

typedef enum ESapiSyncSourceConflictResolutionType {
    EKeepLastModified,
    EKeepFromFirstList,
    EKeepFromSecondList,
    ERemoveFromFirstFromCache
} ESSSConflictResType;


/**
 * The super class that represents the SyncSource used for the SapiSyncManager
 */
class SapiSyncSource
{
private:

    /**
     * Called by the constructors to create and initialize the KeyValueStore base on a storage location that 
     * can be passed by the client or left to the sync source to be created in a default position.
     * @param sc     the syncSource's configuration
     * @param mappings the mappings KeyValueStore that can be passed by the client
     * @param cache the cache KeyValueStore that can be passed by the client
     * @param blacklist the KeyValueStore that can be passed by the client for storing blacklisted items
     * @param cacheLocation  the path to store cache/resume/mappings tables. By default this is
     *                       null and uses the one from the PlatformAdapter. If not null, it tries
     *                       to create it and if no success, it uses the one from PlatformAdapter
     */
    bool init(SyncSourceConfig& sc, KeyValueStore* mappings, KeyValueStore* cache, KeyValueStore* blacklist, 
              const char* storageLocation = NULL);


    /**
     * A filter date from which to filter the item by date in the incoming process
     */
    size_t incomingFilterDate;
    
    /**
     * A filter date from which to filter the item by date in the outgoing process
     */
    size_t outgoingFilterDate;
  
    /**
     * Looks for the SapiSyncItemInfo corresponding to this guid in all lists (ALL, NEW, MOD).
     * If found, the item is removed from the corresponding list to avoid uploading it twice.
     * @param luid  the luid to look for
     * @return      new allocated SapiSyncItemInfo corresponding to the luid, NULL if not found
     */
    SapiSyncItemInfo* copyAndRemoveSapiSyncItemInfo(const StringBuffer& luid);

    /**
     * Looks for the SapiSyncItemInfo corresponding to this guid in this list.
     * If found, the item is removed from the corresponding list to avoid uploading it twice.
     * @param luid the luid to look for
     * @param list the items list to look into
     * @return     new allocated SapiSyncItemInfo corresponding to the luid, NULL if not found
     */
    SapiSyncItemInfo* copyAndRemoveSapiSyncItemInfo(const StringBuffer& luid, ArrayListEnumeration* list);

    /**
     * Looks for the SapiSyncItemInfo corresponding to this guid in this list.
     * If found, a pointer to the item is just returned (no copy, no remove).
     * @param luid the luid to look for
     * @param list the items list to look into
     * @return     pointer to SapiSyncItemInfo corresponding to the luid, NULL if not found
     */
    SapiSyncItemInfo* getItemInfo(const StringBuffer& luid, ArrayListEnumeration* list);

public:

    /**
     * Constructs the SapiSyncSource.
     * 
     * @param sc    the syncSource's configuration
     * @param report the syncSource's report
     * @param incomingFilterDate a filter date from which to filter the item by date. 0 means no filter
     * @param outgoingFilterDate a filter date from which to filter the item by date. 0 means no filter
     * @param cacheLocation  the path to store cache/resume/mappings tables. By default this is
     *                       null and uses the one from the PlatformAdapter. If not null, it tries
     *                       to create it and if no success, it uses the one from PlatformAdapter
     */
    SapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& report, size_t incomingFilterDate, size_t outgoingFilterDate, const char* storageLocation = NULL);

    /**
     * Constructs the SapiSyncSource.
     * 
     * @param sc    the syncSource's configuration
     * @param report the syncSource's report
     * @param mappings the mappings KeyValueStore that can be passed by the client
     * @param cache the cache KeyValueStore that can be passed by the client
     * @param incomingFilterDate a filter date from which to filter the item by date. 0 means no filter
     * @param outgoingFilterDate a filter date from which to filter the item by date. 0 means no filter
     * @param cacheLocation  the path to store cache/resume/mappings tables. By default this is
     *                       null and uses the one from the PlatformAdapter. If not null, it tries
     *                       to create it and if no success, it uses the one from PlatformAdapter
     */
    SapiSyncSource(SyncSourceConfig& sc, 
                   SyncSourceReport& report,
                   KeyValueStore* mappings, 
                   KeyValueStore* cache,
                   KeyValueStore* blacklist,
                   size_t incomingFilterDate,
                   size_t outfoingFilterDate,
                   const char* cacheLocation = NULL);
                   

    virtual ~SapiSyncSource();

    /**
     * Return the sync source config to be used by the SapiSyncManager. It is the 
     * config instantiated by the client...
     */
    SyncSourceConfig& getConfig() { return config;     }
    KeyValueStore& getMappings()  { return *mappings;  }
    KeyValueStore& getResume()    { return *resume;    }
    KeyValueStore& getCache()     { return *cache;     }
    KeyValueStore& getBlacklist() { return *blackList; }
    SyncSourceReport& getReport() { return report;     }
    KeyValueStore& getServerDateMappings()  { return *serverDateMappings; }

    /**
     * Begins the synchronization for this source.
     * Check last sync time and initialize local items lists.
     *
     * The allItems list is always populated, and each item (SyncItemInfo) in
     * this list is filtered with the filterOutgoingItem() method.
     * The new/mod/delItems lists are populated only if client
     * and server are already in sync (comparing the allItems list with the cache).
     * The SapiSyncManager asks what the client has to do. 
     *
     * @param changes - false means to popuplate the allItemInfo. otherwise it checks the modification
     * @param mainConfig  reference to the AbstractSyncConfig, used to check if sync aborted
     * @return 0 if all ok.
     */
    virtual int beginSync(bool changes, AbstractSyncConfig& mainConfig);        

     /**
     * Checks the current filters to see if the item is 
     * supported and can be uploaded to server.
     * May be overridden by clients for custom implementations.
     * @param clientItemInfo the metadata of client item to check
     * @return          true if the item has to be filtered out (skipped)
     *                  false if the item is ok 
     */
    virtual bool filterOutgoingItem(SapiSyncItemInfo& clientItemInfo);
      
    /**
    * Prepares the list of new, mod, del items with the SapiSyncItemInfo objects.
    * Uses the already populated allItemInfo.
    * @param mainConfig  reference to the AbstractSyncConfig, used to check if sync aborted
    * @return   true if the new/mod/del are popuplated. false if the allItemInfo is empty          
    */
    virtual bool populateModifiedItemInfoList(AbstractSyncConfig& mainConfig);

    /**
     * This must populate the list with all the SyncItemInfo of the client.
     * It is used both to handle the first sync or to handle the modification items.
     * PhaseI is just for new items
     * @param mainConfig  reference to the AbstractSyncConfig, used to check if sync aborted
     */
    virtual bool populateAllItemInfoList(AbstractSyncConfig& mainConfig) = 0;    

    /**
     * Searches for a twin item in the local items lists.
     * This method may be overridden by clients to customize the
     * twin detection algorithm.
     * Default implementation checks the fileName and the file size.
     * If the fileName is different but the size is the same the item
     * is considered the same but there is a log at info level.
     * This implementation is fine for media like pictures, video, music
     * It can be different for files.
     *
     * NOTE: if twin found, the item is removed from the corresponding
     *       list of local items, since there's no need to sync it.
     *
     * @param serverItemInfo the metadata of server item to search
     * @return the local twin item if found, as a new allocated copy (must be deleted by the caller)
     *         NULL if the twin item is not found
     */
    virtual SapiSyncItemInfo* twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array = NULL);


    /**
     * Checks the current filters to see if the item is 
     * supported and can be stored locally.
     * May be overridden by clients for custom implementations.
     * @param serverItemInfo the metadata of server item to check
     * @param offsetTime difference between time on client and on server. It is
     *                   calculated clientTime - serverTime
     * @return true if the item must be filtered and not used by the manager
     */
    virtual bool filterIncomingItem(SapiSyncItemInfo& serverItemInfo, time_t offsetTime);

     /**
     * It is called by the SapiSyncManager when the upload phase has been completed
     */
    virtual int endUpload();

    // ------ upload -------

    /**
     * It return a new allocated InputStream (basically a FileInputStream) with the 
     * luid choosen by the client. It is used in the getItem method 
     */
    virtual InputStream* createInputStream(const char* luid) = 0;

    /**
     * Called by SapiSyncManager to resume an upload.
     * Returns a SapiSyncItem corresponding to the LUID, or
     * NULL if the luid has no corrispondence locally.
     * The SapiSyncItem contains the inputStream attached to 
     * the stream of item's data to upload.
     * @return new allocated SapiSyncItem / NULL if not found
     */
    UploadSapiSyncItem* getItem(const StringBuffer& luid);

    /**
     * Called by SapiSyncManager before uploading an item.
     * Returns the next UploadSapiSyncItem of allItems list.
     * The SapiSyncItem contains the inputStream attached to 
     * the stream of item's data to upload.
     * @param OUT if there is an error in updating SapiSyncItems the value is != 0
     * @return new allocated UploadSapiSyncItem / NULL if no more items
     */
    UploadSapiSyncItem* getNextItem(int* err);

    /**
     * Called by SapiSyncManager before uploading a new item.
     * Returns the next UploadSapiSyncItem of newItems list.
     * The SapiSyncItem contains the inputStream attached to 
     * the stream of item's data to upload.
     * @param OUT if there is an error in updating SapiSyncItems the value is != 0
     * @return new allocated UploadSapiSyncItem / NULL if no more items
     */
    virtual UploadSapiSyncItem* getNextNewItem(int* err);

    /**
     * Called by SapiSyncManager before uploading an updated item.
     * Returns the next UploadSapiSyncItem of modItems list.
     * The SapiSyncItem contains the inputStream attached to 
     * the stream of item's data to upload.
     * @param OUT if there is an error in updating SapiSyncItems the value is != 0
     * @return new allocated UploadSapiSyncItem / NULL if no more items
     */
    virtual UploadSapiSyncItem* getNextModItem(int* err);

    /**
     * Called by SapiSyncManager before uploading a deleted item.
     * Returns the next SapiSyncItemInfo of delItems list.
     * The SapiSyncItem's inputStream is NULL and not used for
     * deleted items (no data to send).
     * @param OUT if there is an error in updating SapiSyncItems the value is != 0
     * @return new allocated SapiSyncItem / NULL if no more items
     */
    virtual SapiSyncItemInfo* getNextDelItem(int* err);

    /**
     * Called by the sync engine with the status returned by the
     * server for a certain item that the client uploaded to the server.
     * It is used to update the cache.
     *
     * @param itemInfo - the SyncItemInfo of the item 
     * @param status   - the upload status returned by the server
     * @param command  - the command (ADD, MOD, DEL)
     */
    virtual void setItemStatus(SapiSyncItemInfo& itemInfo, int status, const char* command);    

    // ------ download phase ---------
    
    /**
     * It returns a new allocated OutputStream (basically a FileOutputStream).
     * The output is a stream where the SyncManager will be write the info from the server
     * The output stream could refer to something already existing or not. Based on this
     * the caller or this method may choose to set the offset to allow the syncmanager
     * to download from there on
     * @param the syncItemInfo where to get info to create the output to write on
     * @return the OutputStream to write on
     */
    virtual OutputStream* createOutputStream(SapiSyncItemInfo& itemInfo) = 0;

    /**
     * It must store the outputstream in the proper way depending on the device.     
     * @param - sapiSyncItem containing the outputStream that the client must store in the proper way
     * @param - errCode pointer to a ESapiSyncSourceError status code
     * @return - The key of the item stored in the device, empty string in case of error
     */
    virtual StringBuffer insertItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) = 0;
    
     /**
     * Default implementation to update item. By default it is not implemented.
     * @param the DownloadSapiSyncItem containing the Luid of the item to change.
     * @return empty string ("") if it is not implemented of if there is an error.
     *         the Luid of the updated item.
     * @param errCode pointer to a ESapiSyncSourceError status code
     */
    virtual StringBuffer changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate);

    /**
     * Default implementation to remove items. 
     * @param the local id used to identify the item to be removed
     * @return   => 0 means success. < 0 means error or not implemented
     *           -2: item not removed successfully 
     *           -1: default implementation (basically not implemented)
     *            0: item removed successfully
     *            1: item locally not found. It is considered as success by the way
     */
    virtual int removeItem(const StringBuffer& identifier);
     
    /**
     * Called by SapiSyncManager, before downloading a new item.
     * Creates a a new item in the local storage, given the
     * item's metadata. The returned DownloadSapiSyncItem contains an
     * outputStream ready to be written with the downloaded data.
     * If something was already downloaded, 
     *
     * @param  itemInfo the item's metadata info
     * @return a new allocated SapiSyncItem
     */
    DownloadSapiSyncItem* createItem(SapiSyncItemInfo& itemInfo);

    /**
     * Called by SapiSyncManager after downloading an item to add.
     * Stores the passed SapiSyncItem to the local storage.
     * Updates the cache file.
     * @param syncItem the item to add
     * @param errCode pointer to a ESapiSyncSourceError status code
     * @return the LUID of the new item added if no errors happen
     * on error an empty string buffer is returned and errCode is 
     * set to appropiate error
     */
    StringBuffer addItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode);

    /**
     * Called by SapiSyncManager after downloading an item to update.
     * Updates the cache file.
     * @param syncItem the item to update
     * @param errCode pointer to a ESapiSyncSourceError status code
     * @return the LUID of the item updated (may be changed). Empty if not yet implemented
     */
    StringBuffer updateItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode);

    /**
     * Removes a local item. Updates the cache file if correct.
     * @param OUT if there is an error in updating SapiSyncItems the value is != 0
     * @return 0 if no error > 0 other errors
     */
    int deleteItem(SapiSyncItemInfo& itemInfo);
    
    /**
     * It is called by the SapiSyncManager when the download phase has been completed
     */
    virtual int endDownload();

    /**
     * Called by the endSync method. Allows to do extra actions to the client if needed.
     * @return 0 if success.
     */
    virtual int closingSync() { return 0; }
    /**
     * Called by the sync engine at the end of sync.
     * Writes the source's last sync time if the sync is successful.
     * Cleanup the resume map and removes tmp files (if any).
     *
     * @return the syncsourceReport's lastErrorCode
     */
    int endSync();

    /**
     * Return the number of items in the interested list.
     * @param "ALL", "NEW", "MOD", "DEL" to get info from the allItems, newItems...
     * @return 0 if no element in the list or if the list is null
     *         -1 if the parameter is unknown
     */
    int getItemsNumber(const char* value);

    /**
     * Normalize the \\ into /
     */
    static void normalizeSlash(StringBuffer& s);
    /**
     * Get the internal list references. Can be used by the SapiSyncManager
     * @param list, "ALL", "NEW", "MOD", "DEL". Null if no one of them
     */
    ArrayListEnumeration* getItemsList(const char* list);

    /**
     * Return the installation timestamp passed in the constructor
     */
    size_t getOutgoingFilterDate() { return outgoingFilterDate; }
    
    /**
     * Return the installation timestamp passed in the constructor
     */
    size_t getIncomingFilterDate() { return incomingFilterDate; }

    /**
     * It should return if there is enough room to store the size passed as argument.
     * Default implmenation return always true.
     * @param the size to check if is is possible to be stored
     * @param [OUT] errorCode that can be returned by the check method
     * @return true if there is room, false otherwise
     */
    virtual bool isLocalStorageAvailable(unsigned long long size, int* errorCode) { return true; }

    /**
     * Should remove temporary data (usually stored into files) of partial download.
     * It is called by the SapiSyncManager when it doesn't want to keep some downloaded data.
     *
     * @param item   the identifier 
     * @return 0 if OK, a value otherwise, -2 if not implemented
     */
    virtual int cleanTemporaryDownloadedItem(const StringBuffer& item);    

    /**
     * Scans the local lists of NEW and DEL items, and checks if an item
     * has the same last modification date. If found, it is a rename (an updated item) so
     * the item is removed from NEW and DEL lists and added to the MOD list.
     * Called by SapiSyncManager during the prepareItems() phase.
     * Clients may redefine the behavior of this method for their needs.
     */
    virtual void localRenameChecks();

    /**
     * Scans the Server lists of MOD items, and checks if an item
     * has the same size (but different name) from the one existing locally.
     * If so, it is a rename and we can flag the item to avoid downloading the content data.
     * Called by SapiSyncManager during the prepareItems() phase.
     * Clients may redefine the behavior of this method for their needs.
     * @param modServerItems  the list of server items metadata to process
     */
    virtual void remoteRenameChecks(ArrayList& modServerItems);

    /**
     * Wrapper call for conflict resolution for client/server updates and deletes
     * @return 0 if no error
     */
    virtual int resolveConflicts(ArrayList* modServerItems, ArrayList* delServerItems, AbstractSyncConfig& config, time_t offsetTime);
 
    /**
     * Scans the GUID/LUID mappings and returns the GUID from the LUID.
     */
    StringBuffer getGuidFromLuid(const char* luid); 

    /**
     * Scans updated server items list checking if items are really updated server side or not.
     * Lookup into the ServerDate map where the last-update-time is stored for every item.
     */
     virtual int pruneModifiedItemsList(ArrayList* modServerItems);

     /**
     * Looks for the SapiSyncItemInfo corresponding to this guid in all lists (ALL, NEW, MOD).
     * If found, a pointer to the item is just returned (no copy, no remove).
     * @param luid  the luid to look for
     * @return      pointer to SapiSyncItemInfo corresponding to the luid, NULL if not found
     */
    SapiSyncItemInfo* getItemInfo(const StringBuffer& luid);

    /**
     * Validates the lists of NEW/MOD/DEL items from client to server.
     * Called by SapiSyncManager before upload starts, to meet the behavior of this source.
     * Default implementation clears the MOD and DEL lists, since only NEW items are
     * supported (derived classes may redefine this behavior).
     */
    virtual void validateLocalLists();

    /**
     * Validates the lists of NEW/MOD/DEL items from server to client.
     * Called by SapiSyncManager before download starts, to meet the behavior of this source.
     * Default implementation clears the MOD and DEL lists, since only NEW items are
     * supported (derived classes may redefine this behavior).
     * @param newServerItems the array of new server items
     * @param modServerItems the array of mod server items
     * @param delServerItems the array of del server items
     */
    virtual void validateRemoteLists(ArrayList* newServerItems, ArrayList* modServerItems, ArrayList* delServerItems);
 
    /**
     * Filters the item based on the extension of the filename. 
     * @return true if the item has to be filtered (skipped) false otherwise
     */
    bool filterByExtension(SapiSyncItemInfo& clientItemInfo);

protected:

     /**
     * Get an upload ite from the list and give it back as a copy. It removes the element from the listenumeration
     */
    UploadSapiSyncItem* getUploadItem(ArrayListEnumeration* list, int pos = 0);    
    
    /**
     * Filters the item searching its guid in blacklist. 
     * @return true if the item has to be filtered (skipped) false otherwise
     */
    bool filterFromBlackList(SapiSyncItemInfo& itemInfo);

    /**
     *
     * sort an enumeration by the modification date
     * Bubble-sort: warning, can be time consuming!
     *
     * @param list the list to sort
     * @param maxItemsToSort [Optional] the max number of items to do the sort, default = 100
     */  
    void sortByModificationTime(ArrayListEnumeration* list, int maxItemsToSort = 100);
    
    /**
     * Called by the setItemStatus to decide if the code is an error code or not.
     * Based on the result, is udpates the cache or not.
     *
     * @code the code to be analyzed
     * @return true if it is an error code, false otherwise
     */
    virtual bool isErrorCode(int code);

    /**
     * Used to set errors for this SyncSource.
     * Will set the source state (error state), the source's last error
     * code and source's last error message.
     * @param  errorCode  the error code
     */
    void setSourceError(const int errorCode);

    /**
     * Save the cache
     */
    int saveCache();

    /**
     * Update the cache temporaneally
     */
    int updateInCache(KeyValuePair& k, const char* action);

    /** 
     * add item to black list
     */
    int blackListItem(SapiSyncItemInfo* itemInfo);
    
    /**
     * Provides conflict resolution logic for sapiSyncManager client/server items list
     * @return 0 if no error
     */
    virtual int resolveConflictOnItemsList(ArrayList* clientItemList, ArrayList* serverItemsList, 
                ESSSConflictResType conflictResType, AbstractSyncConfig& config, time_t offsetTime);

    /**
     * Updates array lists of server/client SyncItems 
     * @return the total number of items updated -1 in case of error of abort
     */
    virtual int updateItemsList(ArrayList* clientModItems, ArrayList* serverItems, AbstractSyncConfig& config);

    /**
      * The syncsource's configuration, including custom 
      * params like filters and folder location to sync.
      */
    SyncSourceConfig& config;
    
    /**
     * The syncsource's report (owned by SyncReport)
     */
    SyncSourceReport& report;

    /**
     * Used to store a KeyValuePair containing the key and the command
     * associated to the item. It stores the cache:
     * - for every item successfully downloaded
     * - for every item successfully uploaded to the server
     */    
    KeyValueStore* cache; 

    /**
     * GUID-LUID mappings. Used by the SapiSyncManager.
     */
    KeyValueStore* mappings; 

    /**
     * GUID-serverDate mappings. Used by the SapiSyncManager.
     * serverDate is the upload time / last mod time on the server.
     */
    KeyValueStore* serverDateMappings;

    /**
     * Resume mappings. Used by the SapiSyncManager.
     * Each entry is a <type, ID> pair.
     * This object is a PropertyFile (stored on FS).
     */
    KeyValueStore* resume; 

    /**
     * Blacklist store: used to store blacklisted
     * items (items not supported by sync source for
     * which insertion will always fail)
     */
    KeyValueStore* blackList;
    
    /**
     * Enumeration of the new ItemInfo       
     */
    ArrayListEnumeration*  newItemInfo;

    /**
     * Enumeration of the updated ItemInfo  
     */
    ArrayListEnumeration*  updatedItemInfo;

    /**
     * Enumeration of the deleted ItemInfo  
     */
    ArrayListEnumeration*  deletedItemInfo;       
    
    /**
     * Used to store the ItemInfo of all items  
     */
    ArrayListEnumeration*  allItemInfo;

    /**
     * If false, it means we're synchronizing ALL items list (so 1st sync, twin detection,...)
     * If true, it means we're synchronizin NEW/MOD/DEL items changes from a specific date.
     * It's set during beginSync(), defined and passed by SapiSyncManager.
     */
    bool isSyncingItemChanges;
    
    /**
     * List of extensions as filter in output. If the first element is a !, then the values are the ones 
     * not allowed. Otherwise they are allowed.
     * ex: !,.jpg,.tiff -> only the .jpg and .tiff are not allowed and are removed
     * ex: .jpg,.tiff -> only the .jpg and .tiff are synced
     */
    ArrayList extension;    
    
};

END_FUNAMBOL_NAMESPACE

#endif 

