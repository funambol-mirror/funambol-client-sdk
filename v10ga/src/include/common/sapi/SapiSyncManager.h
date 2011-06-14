/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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

#ifndef SAPI_SYNC_MANAGER
#define SAPI_SYNC_MANAGER

/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/AbstractSyncConfig.h"
#include "spds/SyncReport.h"

#include "sapi/SapiSyncSource.h"
#include "sapi/SapiMediaRequestManager.h"

BEGIN_FUNAMBOL_NAMESPACE


/// Max number of items metadata to retrieve for each sapi "getAll" call.
#define SAPI_PAGING_LIMIT       100

/// Max number of items metadata to retrieve for each sapi "get IDs" call.
#define SAPI_PAGING_LIMIT_IDS   200

/// Separator for resume map value, which contains many fields (resumed item info)
#define RESUME_MAP_FIELD_SEPARATOR ","


/**
 * Enumeration of possible error codes for SapiSyncManager.
 */
typedef enum ESapiSyncManagerError {
    ESSMSuccess = 0,
    ESSMCanceled,
    ESSMSapiNotSupported,
    ESSMConfigError,
    ESSMSapiError,
    ESSMBeginSyncError,
    ESSMEndSyncError,
    ESSMGetItemError,
    ESSMSetItemError,
    ESSMItemNotSupportedBySource,
    ESSMNetworkError,
    ESSMAuthenticationError,
    ESSMServerQuotaExceeded,
    ESSMClientQuotaExceeded,
    ESSMMediaHubPathNotFound,
    ESSMGenericSyncError

} ESSMError;

/**
 * Core class of the SAPI synchronization, used for media sources sharing.
 * It handles the synch of one single sapiSyncSource at time (passed in
 * the constructor) and executes the upload/download for
 * all required items to put the source in sync with the server.
 *
 * It it used by the client, which should calls the interface methods like this:
 *   1.  beginSync  (gets the lists of items metadata on both sides and checks dupes)
 *   2a. upload     (starts the upload of local items)
 *   2b. download   (starts the download of server items)
 *   3.  endSync    (closes the sync process)
 * 
 * The client is responsible to save the source's configuration at the end of
 * the upload and download, for instance to save timestamps and filter values.
 * 
 * This class owns the lists of server items, while the lists of clients items
 * are handled by the syncsource itself and can be returned to the SapiSyncManager
 * when necessary.
 * This class handles the GUID/LUID mappings and the resume map for the syncsource,
 * this way the syncsources don't need to manage them at all.
 * A SapiRequestManager object is used as an interface for all SAPI requests
 * to the server, like getting the server items info or upload/download data.
 *
 * The twin detection method is implemented in the syncsource, this way the algorithm
 * can be customized for the need of the source, and also for different platforms.
 */
class SapiSyncManager {

public:

    /**
     * Initialize a new sapi sync manager. Parameters provided to it
     * have to remain valid while this sync manager exists.
     *
     * @param s  the sapi syncsource to sync
     * @param c  required configuration
     */
    SapiSyncManager(SapiSyncSource& s, AbstractSyncConfig& c);

    /// Destructor
    virtual ~SapiSyncManager();

    /**
     * Begins the synchronization: fills the lists of Server items info.
     * The source's beginSync is first called to fill (& filter) also client's items info.
     * Server items to download are get here, based on the source's filterings:
     *   1. no filtering OR filter by number (strongest):
     *      all items are retrieved (sapi getAll)
     *   2. filter by date: 
     *      only the items with mod date after this one will be retrieved (sapi getChanges)
     *
     * The flag 'isSyncingItemChanges' is set here, and it will distinguish the 
     * whole sync flow, based on the ALL items list (1.) or NEW/MOD/DEL items lists (2.)
     * 
     * Calls prepareItems() at the end, in order to process the server/client lists of
     * metadata and avoid syncing an item twice.
     * At the end of this method, the lists contain exactly the items to exchage, 
     * both client and server side. Fires the 2 totalItems events at the end.
     */
    int beginSync();


    /**
     * Starts the upload of local items to the server.
     * It must be called after beginSync() and prepareItems(),
     * so the lists of items to upload are ready.
     * @return  one of ESapiSyncManagerError error codes (0 if no error)
     */
    int upload();

    /**
     * Starts the download of server items to the client.
     * It must be called after beginSync() and prepareItems(),
     * so the lists of items to download are ready.
     * @return  one of ESapiSyncManagerError error codes (0 if no error)
     */
    int download();


    /**
     * Performs the commit phase of the synchronization.
     */
    int endSync();


protected:

    /**
     * Gets all item info (metadata) from the Server.
     * Populates the list of ALL items, called by beginSync().
     * @param fromDate the date to filter server items (empty string if no filterByDate is set)
     * @return 0 if no error
     */
    int getAllServerItems(const StringBuffer& fromDate);

    /**
     * Gets changed item info (metadata) from the Server, from a given date.
     * Populates the lists of NEW/MOD/DEL items, called by beginSync().
     * @param fromDate  the date to ask for Server changes (UTC string format)
     * @return 0 if no error
     */
    int getServerChanges(const StringBuffer& fromDate);

    /**
     * Gets changed item info (metadata) from the Server, from a given array of items IDs.
     * Called by getServerChanges() for NEW and MOD Server items lists.
     * Paging the requests (see SAPI_PAGING_LIMIT_IDS for the limit): Server changes 
     * may be a lot, the getItemsFromId request can't be longer than 2K.
     * 
     * @param items    [IN-OUT] the list of items metadata returned by the Server
     * @param itemsIDs the Server items IDs to ask the metadata
     * @return         0 if no error
     */
    int getServerItemsFromIds(ArrayList& items, const ArrayList& itemsIDs);


    /**
     * Wrapper call for sapi sync source conflict resolution 
     * on client/server updates and deletes
     *
     * @return 0 if no error
     */
    int resolveConflicts();
 
    /**
     * Prepare the lists of Server items for the synchronization, in order
     * to avoid syncing an item twice (media items are big).
     * In case of 1st sync the twin-detection is executed, otherwise a
     * check in the GUID/LUID map is done.
     * At the end of this method, the lists contain exactly 
     * the items to exchage, both client and server side.
     * Fires the 2 totalItems events at the end.
     */
    int prepareItems();

    /**
     * Processes the lists of ALL items, both server and client side,
     * to avoid syncing an item twice (media items are big).
     * For both lists:
     *   1. Exclude items already synced (check GUID/LUID map)
     *   2. Exclude items existing locally (TWIN DETECTION)
     *   3. Exclude items that don't verify current filters (if any) <- only for server list
     *
     * This method is called by prepareItems(), at the end the lists of ALL
     * items are ready for upload/download.
     * @return 0 if no error
     */
    int prepareAllItemsLists();

    /**
     * Processes the lists of NEW/MOD/DEL items, both server and client side,
     * to avoid syncing an item twice (media items are big).
     *   1. Checks the GUID/LUID maps for NEW/MOD incoming items
     *   2. Twin detection on NEW items
     *   3. Conflicts resolution for NEW/MOD items
     *   4. Rename check for outgoing items
     *   5. Apply filters on both incoming and outgoing items
     *
     * This method is called by prepareItems(), in case of syncing item changes
     * @return 0 if no error
     */
    int prepareChangedItemsLists();

    /**
     * Checks the GUID/LUID maps for NEW/MOD incoming items.
     *   - NEW items already synced and same size -> discard item
     *   - NEW items already synced and different size -> treat it as an update
     *   - MOD items not already synced are treated as NEW
     * @return 0 if no error
     */
    int checkMappings();

    /**
     * Apply S2C filtering on all items for the desired list of incoming items.
     * @param list  the desired list, one of: "ALL", "NEW", "MOD", "DEL"
     * @return 0 if no error
     */
    int filterIncomingItems(const char* list);

    /**
     * Apply C2S filtering on all items for the desired list of outgoing items.
     * @param list  the desired list, one of: "ALL", "NEW", "MOD", "DEL"
     * @return 0 if no error
     */
    int filterOutgoingItems(const char* list);


    /**
     * Reads the resume map and starts the upload for each entry found, with type "upload".
     * @return the result of resume operation, 0 if no error
     */
    int resumeUploads();

    /**
     * Resumes the upload of a single local item, from client to Server.
     * Called by resumeUploads() for every entry found in the resume table.
     * @param oldItemInfo  the info of the item to resume, as they were saved last sync
     *                     in the resume map
     * @return the upload result, 0 if no error
     */
    int resumeUpload(UploadSapiSyncItem& resumeItem, SapiSyncItemInfo& oldItemInfo, const StringBuffer& command);

    /**
     * Uploads one single local item to the server.
     * Called by upload() for each item.
     * @param  command  the SAPI action to execute, one of:
                        COMMAND_ADD, COMMAND_REPLACE, COMMAND_DELETE
     * @param isResume  if true, the item is partially uploaded (resume)
     * @return  the upload result, 0 if no error
     */
    int uploadItem(UploadSapiSyncItem& clientItem, const StringBuffer& command, bool isResume = false);

    /**
     * Reads the resume map and starts the download for each entry found, with type "download".
     * @return the result of resume operation, 0 if no error
     */
    int resumeDownloads();

    /**
     * Resumes the download of a single item, from server to client.
     * Called by download() for every entry found in the resume table.
     * @param oldItemInfo  the info of the item to resume, as they were saved last sync
     *                     in the resume map
     * @return the download result, 0 if no error
     */
    int resumeDownload(SapiSyncItemInfo& oldItemInfo);

    /**
     * Downloads one single item from the server.
     * Called by download() for each item.
     * @param  command  the local action to execute, one of: COMMAND_ADD, COMMAND_REPLACE
     *                  (COMMAND_DELETE is not expected, does not require a download)
     * @param isResume  if true, the item is partially downloaded (resume)
     * @return the download result, 0 if no error
     */
    int downloadItem(SapiSyncItemInfo& serverItem, const StringBuffer& command, bool isResume = false);


    //
    // ------------------------ retry mechanism ---------------------------
    //
    /**
     * Retries the sapiMediaRequestManager::getItemsChanges(), called in case of network error.
     * It will retry the sapi call until the error is not a network error.
     * The max number of retries is defined by the config param 'maxRetriesOnError'.
     * If the config param 'sleepTimeOnRetry' is set, a sleep will be applied
     * before each attempt (event SYNC_SOURCE_RETRY is fired).
     */
    ESMRStatus retryGetItemsChanges(ArrayList& newIDs, ArrayList& modIDs, ArrayList& delIDs, 
                                    const StringBuffer& fromDate);

    /**
     * Retries the sapiMediaRequestManager::getItemsFromId(), called in case of network error.
     * It will retry the sapi call until the error is not a network error.
     * The max number of retries is defined by the config param 'maxRetriesOnError'.
     * If the config param 'sleepTimeOnRetry' is set, a sleep will be applied
     * before each attempt (event SYNC_SOURCE_RETRY is fired).
     */
    ESMRStatus retryGetItemsFromId(ArrayList& items, const ArrayList& itemsIDs);

    /**
     * Retries the sapiMediaRequestManager::getAllItems(), called in case of network error.
     * It will retry the sapi call until the error is not a network error.
     * The max number of retries is defined by the config param 'maxRetriesOnError'.
     * If the config param 'sleepTimeOnRetry' is set, a sleep will be applied
     * before each attempt (event SYNC_SOURCE_RETRY is fired).
     */
    ESMRStatus retryGetAllItems(ArrayList& items, int limit, int offset);

    /**
     * Retries to upload again the client item, called in case of network error.
     * It will retry the sapi call until the error is not a network error.
     * The max number of retries is defined by the config param 'maxRetriesOnError'.
     * If the config param 'sleepTimeOnRetry' is set, a sleep will be applied
     * before each attempt (event SYNC_SOURCE_RETRY is fired).
     *
     * @param clientItem  the item to upload
     */
    int retryUpload(UploadSapiSyncItem* clientItem);

    /**
     * Retries to download again the server item, called in case of network error.
     * It will retry the action until the error is not a network error.
     * The max number of retries is defined by the config param 'maxRetriesOnError'.
     * If the config param 'sleepTimeOnRetry' is set, a sleep will be applied
     * before each attempt (event SYNC_SOURCE_RETRY is fired).
     *
     * @param serverItem  the item to download
     */
    ESMRStatus retryDownload(DownloadSapiSyncItem* serverItem);


    /**
     * Returns the media source type for current source.
     * It's used by SapiMediaRequestManager to address sapi calls.
     */
    SapiMediaSourceType getMediaSourceType();

    /**
     * Returns true if the GUID (server item ID) is find in mappings.
     * It means the corresponding item has already be synchronized.
     * Queries the GUID/LUID map owned by the syncsource.
     */
    bool findGuidInMappings(const char* guid);

    /**
     * Returns true if the LUID (client item ID) is find in mappings.
     * It means the corresponding item has already be synchronized.
     * Queries the cache map owned by the syncsource (speed optimization: LUID is the key)
     */
    bool findLuidInMappings(const char* luid);

    /**
     * Returns the number of items in the interested list.
     * @param listType "ALL", "NEW", "MOD", "DEL" to get info from the allItems, newItems...
     * @return 0 if the list is empty, null or unknown, >0 otherwise
     */
    int getClientItemsNumber(const char* listType);

    /**
     * Reads the client filter-by-number value from the config, and returns it.
     * If error, returns 0
     */
    int readClientFilterNumber();

    /**
     * Reads the server filter-by-number value from the config, and returns it.
     * If error, returns 0
     */
    int readServerFilterNumber();

    /**
     * Reads the server last timestamp (download) from the config, and returns it.
     * If error, returns 0
     */
    unsigned long readServerLastTimestamp();


    /**
     * Defines if we are syncing ALL items or CHANGES since a defined date.
     * It is called by beginSync() after the source's filters has been set.
     *
     *   - if is 1st sync on an active direction (download or upload), we always sync ALL items 
     *     (need twin detection at least the first time)
     *   - if filter-by-number is set for an active direction, we sync ALL
     *     (we can filter a number of items only on the ALL lists)
     *   - in none of the above, we can sync CHANGES
     *
     * @return  true if we have to sync item CHANGES since a date
     *          false if we have to sync ALL items
     */
    bool defineSyncBehavior();

    /**
     * Called during resume download of an item.
     * Looks for the SapiSyncItemInfo corresponding to this guid in all server's lists.
     * (the SapiSyncItemInfo contains the metadata of the item to resume)
     * If found, the item is removed from the corresponding list to avoid
     * downloading it twice. If not found, the resume will be then discarded 
     * because it means the item is no more available on the server.
     *
     * @param guid  the server's guid to look for
     * @return      new allocated SapiSyncItemInfo corresponding to the guid
     *              NULL if not found
     */
    SapiSyncItemInfo* copyAndRemoveSapiSyncItemInfo(const StringBuffer& guid);

    /**
     * Called during resume download of an item.
     * Looks for the SapiSyncItemInfo corresponding to this guid in this list.
     * (the SapiSyncItemInfo contains the metadata of the item to resume)
     * If found, the item is removed from the corresponding list to avoid
     * downloading it twice. If not found, the resume will be then discarded 
     * because it means the item is no more available on the server.
     *
     * @param list  the server's list to look into
     * @param guid  the server's guid to look for
     * @return      new allocated SapiSyncItemInfo corresponding to the guid
     *              NULL if not found
     */
    SapiSyncItemInfo* copyAndRemoveSapiSyncItemInfo(ArrayList& list, const StringBuffer& guid);

    /**
     * Creates a  SapiSyncItemInfo, deserializing a key-value pair entry from
     * the resume map, like:
     *    (key)  (value)              
     *  [ GUID ; type,LUID,name,size ]
     *
     * @param kvp  the (input) pair of StringBuffer
     * @param type the (output) type of resume item created
     * @return  new allocated SapiSyncItemInfo
     */
    SapiSyncItemInfo* createResumeItemInfo(const KeyValuePair& kvp, StringBuffer& type);

    /**
     * Adds an entry to the resume map. Called before upload/download of an item.
     * The entry is the serialization of a SapiSyncItemInfo, a key-value pair like:
     *    (key)  (value)              
     *  [ GUID ; type,LUID,name,size ]
     *
     * @param itemInfo  the SapiSyncItemInfo of the current item
     * @param type      the type of resume (RESUME_UPLOAD or RESUME_DOWNLOAD)
     */
    void addToResumeMap(SapiSyncItemInfo& itemInfo, const char* type);

    /**
     * Removes an entry from the resume map. Called when upload/download of an item
     * completed successfully. The entry is identified from the item's guid.
     * @param itemInfo  the SapiSyncItemInfo of the current item
     */
    void removeFromResumeMap(SapiSyncItemInfo& itemInfo);

    /**
     * Checks if the current Server Url and the username are the same as last sync.
     * If not, the cache/mappings/timestamps are no more valid, so they're 
     * cleaned up.
     * @return true if the cache/mappings/tstamps have been cleaned up, false if not
     */
    bool checkLastUserAndServer();

    /**
     * Called in case of sync errors.
     * Updates the report and the ssReport.
     * @param errorCode  one of ESSMError error codes
     * @param data       optionally, additional data
     */
    void setSyncError(const ESSMError errorCode, const int data = 0);



    /// The sapiSyncSource under sync.
    SapiSyncSource& source;

    /// The configuration object, containing all sync settings.
    AbstractSyncConfig& config;

    /// The sync source report, used to return sync results and errors.
    SyncSourceReport& report;
    

    /**
     * The interface for all SAPI calls.
     * It's initialized in the constructor, and it uses platform
     * specific HttpConnection to perform HTTP requests to the Server.
     */
    SapiMediaRequestManager* sapiMediaRequestManager;


    // Server lists of SapiItemInfo elements.
    ArrayList allServerItems;
    ArrayList newServerItems;
    ArrayList modServerItems;
    ArrayList delServerItems;


    /// The name of sapiSyncSource under sync, set in constructor for easy access.
    StringBuffer sourceName;

    /// The remote name of sapiSyncSource under sync, set in constructor for easy access.
    StringBuffer sourceURI;

    /// The syncmode associated to this source. It's set in beginSync().
    SyncMode syncMode;

    /// The filter by number for client side items (uploads). -1 means it's disabled.
    int clientFilterNumber;

    /// The filter by number for server side items (downloads). -1 means it's disabled.
    int serverFilterNumber;

    /**
     * If false, it means we're synchronizing ALL items list (so 1st sync, twin detection,...)
     * If true, it means we're synchronizin NEW/MOD/DEL items changes from a specific date.
     * It's set during beginSync(), and it MUST be the same for upload and download.
     */
    bool isSyncingItemChanges;

    /**
     * This is the exact time when the current sync session started.
     * It is used as the last sync timestamp, for the downloads.
     * It is set in the config at the end of download() method, if all downloads ok.
     *
     * TODO: It should be created and returned by the Server during getAll/getChanges,
     *       now it's created by the client when sync session starts.
     */
    time_t downloadTimestamp;

    /**
     * It is the offset between the current client time (time(NULL)) and the server
     * time (requested in the sapi). It is calculated with clientTime - serverTime
     */
    time_t offsetClientServer;
};


END_FUNAMBOL_NAMESPACE

/** @} */
/** @endcond */
#endif

