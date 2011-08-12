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

#ifndef FILE_SAPI_SYNC_SOURCE
#define FILE_SAPI_SYNC_SOURCE

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncSourceConfig.h"
#include "sapi/SapiSyncItemInfo.h"
#include "sapi/SapiSyncSource.h"
#include "sapi/SapiContentTypes.h"
#include "base/util/KeyValueStore.h"
#include "spds/SyncSourceReport.h"
#include "base/util/PropertyFile.h"
#include "spds/spdsutils.h"

BEGIN_FUNAMBOL_NAMESPACE

/**
 * The super class that represents the SyncSource used for the SapiSyncManager
 */
class FileSapiSyncSource : public SapiSyncSource
{
  
protected:
    
    /**
     * The total amount of disk space used in the current folder, in bytes.
     * It's calculated during the initial scan (populateAllItemInfo)
     * and it's update at the end of each download.
     * It's used to check the local storage available (see isLocalStorageAvailable)
     * It includes all the files in the sync folder, also filtered ones.
     */
    long folderSize;    

public:

    /**
     * Constructs the SapiSyncSource.
     * 
     * @param sc    the syncSource's configuration
     * @param report the syncSource's report
     * @param incomingFilterDate  the filter by date for incoming items (downloads)
     *                            timestamp value (unix time), if 0 means the filter is disabled
     * @param outgoingFilterDate  the filter by date for outcoming items (uploads)
     *                            timestamp value (unix time), if 0 means the filter is disabled
     * @param cacheLocation  the path to store cache/resume/mappings tables. By default this is
     *                       null and uses the one from the PlatformAdapter. If not null, it tries
     *                       to create it and if no success, it uses the one from PlatformAdapter
     */
    FileSapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& report, size_t incomingFilterDate, size_t outgoingFilterDate, const char* storageLocation = NULL, bool recursive_=false);
                       
    virtual ~FileSapiSyncSource();
   
    /**
     * This must populate the list with all the SyncItemInfo of the client.
     * It is used both to handle the first sync or to handle the modification items.
     * PhaseI is just for new items
     */
    virtual bool populateAllItemInfoList(AbstractSyncConfig& mainConfig);         

    // ------ upload -------

    /**
     * It return a new allocated InputStream (basically a FileInputStream) with the 
     * luid choosen by the client. It is used in the getItem method 
     */
    virtual InputStream* createInputStream(const char* luid);  

    // ------ download phase ---------
    
    /**
     * It returns a new allocated OutputStream (basically a FileOutputStream).
     * The output is a stream where the SyncManager will be write the info from the server
     * The output stream could refer to something already existing or not. Based on this
     * the caller or this method may choose to set the offset to allow the syncmanager
     * to download from there on
     * 
     */
    virtual OutputStream* createOutputStream(SapiSyncItemInfo& itemInfo);

    /**
     * It must store the outputstream in the proper way depending on the device.     
     * @param syncItem - DownloadSapiSyncItem that the client must store in the proper way
     * @param errCode  - pointer to a ESapiSyncSourceError status code
     * @return StringBuffer - The key of the item stored in the device, empty string in case of errors
     */    
    virtual StringBuffer insertItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate);   
    
    virtual StringBuffer changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate);

    /**
     * Default implementation to remove items (not implemented by default)
     * @param id the local id used to identify the item to be removed
     *           can be either a local file name or the full path
     * @return   => 0 means success. < 0 means error or not implemented
     *           -2: item not removed successfully 
     *           -1: default implementation (basically not implemented)
     *            0: item removed successfully
     *            1: item locally not found. It is considered as success by the way
     *            2: folder path not found in config
     */
    virtual int removeItem(const StringBuffer& identifier);    

    /**
     * Called by the endSync method. Allows to do extra actions to the client if needed.
     * @return 0 if success.
     */
    virtual int closingSync() { return 0; }

    /**
     * Returns the folder path where media files are synchronized to/from.
     * This value is stored in the configuration (PROPERTY_FOLDER_PATH)
     * @return  the folder path, NULL if not found / error
     */
    StringBuffer getFolderPath();
    
    int cleanTemporaryDownloadedItem(const StringBuffer& item);

    /**
     * It should return if there is enough room to store the size passed as argument.
     * Checks the 'localQuotaStorage' configuration property.
     * @param the size to check if is is possible to be stored
     * @return true if there is room, false otherwise
     */
    virtual bool isLocalStorageAvailable(unsigned long long size, int* errorCode);

    /// Returns the current folder size (where files are synchronized to/from)
    virtual long getFolderSize() { return folderSize; }

    virtual SapiSyncItemInfo* twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array = NULL);

    virtual UploadSapiSyncItem* getNextModItem(int* err);

    virtual int resolveConflicts(ArrayList* modServerItems, ArrayList* delServerItems, AbstractSyncConfig& config, time_t offsetTime);

    virtual int resolveConflictOnItemsList(ArrayList* clientItemsList, ArrayList* serverItemsList, 
                ESSSConflictResType conflictResType, AbstractSyncConfig& config, time_t offsetTime);


    virtual int pruneModifiedItemsList(ArrayList* modServerItems);

    /**
     * Validates the lists of NEW/MOD/DEL items from client to server.
     * Called by SapiSyncManager before upload starts, to meet the behavior of this source.
     * Thus implementation clears the DEL lists, since only NEW and MOD items are
     * supported (derived classes may redefine this behavior).
     */
    virtual void validateLocalLists();

    /**
     * Validates the lists of NEW/MOD/DEL items from server to client.
     * Called by SapiSyncManager before download starts, to meet the behavior of this source.
     * This implementation doesn't do anything, since all the changes are
     * supported (derived classes may redefine this behavior).
     * @param newServerItems the array of new server items
     * @param modServerItems the array of mod server items
     * @param delServerItems the array of del server items
     */
    virtual void validateRemoteLists(ArrayList* newServerItems, ArrayList* modServerItems, ArrayList* delServerItems);

    /**
     * Overrides SapiSyncSource::remoteRenameChecks().
     * No check is done here, since having the same size is not enough
     * to decide the item is a rename, for generic files.
     */
    virtual void remoteRenameChecks(ArrayList& modServerItems) {}
};

END_FUNAMBOL_NAMESPACE

#endif 

