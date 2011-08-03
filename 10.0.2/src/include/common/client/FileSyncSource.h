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
#include "base/util/ItemContainer.h"
#include "spds/FileData.h"
#include "client/CacheSyncSource.h"

// for stat 
//#include <errno.h>
//#include "sys/types.h"
//#include "sys/stat.h"



//#ifndef S_ISDIR
//#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
//#endif

BEGIN_NAMESPACE


#define OMA_MIME_TYPE "application/vnd.omads-file+xml"

#define DEFAULT_SYNC_DIR   "."

/**
 * This class extends the CacheSyncSource abstract class, implementing a plain
 * file datastore. All the files in a folder are synchronized with the server.
 * Depending on the MIME type defined for the source (see
 * SyncSourceConfig::getType()), this class can work in two ways:
 *
 * - if the type is "application/vnd.omads-file+xml", the files are wrapped into
 *   the OMA File Object representation, to preserve the file name and attributes.
 * - otherwise, the file is sent as it to the server.
 *
 * for incoming items, the format of the file is detected by the content.
 *
 */
class FileSyncSource : public CacheSyncSource {
   
protected:
    StringBuffer dir;
    
    /**
     * Used to filter outgoing items. Current implementation 
     * doesn't filter any item: override this method for specific filtering.
     * NOTE: this is only STATIC filtering.
     *       Meaning that these filters CANNOT be changed/disabled anytime by the Client,
     *       otherwise a change in the filter may result in the Client to send 
     *       deleted items for files fitered out.
     *       See dynamicFilterItem() for dynamic filtering.
     * 
     * @param fullName  the full path + name of the file to check
     * @param st        reference to struct stat for current file
     * @return          true if the item has to be filtered out (skipped)
     *                  false if the item is ok
     */ 
    virtual bool filterOutgoingItem(const StringBuffer& fullName, struct stat& st);

    /**
     * Filtering on incoming items from Server.
     * Derived classes may override this method to define specific filtering.
     * @param file  [IN-OUT] the FileData object received from Server
     * @return      true if the item has to be filtered out (rejected)
     *              false if the item is ok (accepted)
     */
    virtual bool filterIncomingItem(FileData& file);

    /**
     * Creates and returns the metadata (OMA file data object) for this file.
     * Note: the file's content is not included: the "body" tag is not added.
     * @param key  the file's name (the item key)
     * @return     the metadata, as a formatted XML string
     */
    StringBuffer formatMetadata(const WCHAR* key);

private:

    /// If true, will recurse into subfolders of 'dir'. Default is false.
    bool recursive;
    
    // Copy is not allowed
    FileSyncSource(const FileSyncSource& s) : CacheSyncSource(s){};
    FileSyncSource& operator=(const FileSyncSource& s) { return *this; };
 
    
    /**
     * Reads the directory 'fullPath' and get all file names.
     * If recursive is true, reads recursively all subfolders too.
     * Populates the filesFound Arraylist with the names of files.
     * Will call filterOutgoingItem() for each item found if applyFiltering is true.
     * 
     * @param fullPath       the absolute path of desired folder to scan
     *                       if empty, will scan the 'dir' folder
     * @param filesFound     [OUT] the arraylist of file names
     * @param applyFiltering if true will call filterOutgoingItem() for each item found. Default = true.
     * @return               true if no error
     */
    bool scanFolder(const StringBuffer& fullPath, ArrayList& filesFound, bool applyFiltering = true);
    

public:
    
    FileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc, 
                   const StringBuffer& aDir = DEFAULT_SYNC_DIR, 
                   KeyValueStore* cache = NULL);

    virtual ~FileSyncSource();

    void assign(FileSyncSource& s);
    
    /**
    * set/get the directory where to sync the files
    */
    void setDir(const char* p) { dir = p; }
    const StringBuffer& getDir() { return dir; };
    
    /**
    * set/get the recursive flag (if true, will recurse subfolders)
    */
    void setRecursive(const bool value) { recursive = value; }
    const bool getRecursive() { return recursive;  };
    
    
    /**
    * Get the list of all the keys stored in a StringBuffer. It reads all the 
    * files name in the directory. The directory is set in the sync source.
    */
    virtual Enumeration* getAllItemList();
    
    /**
     * Removes all the item of the sync source. It is called 
     * by the engine in the case of a refresh from server to clean      
     * all the client items before receiving the server ones.
     */
    virtual int removeAllItems();

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
    virtual int insertItem(SyncItem& item);
    
    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    virtual int modifyItem(SyncItem& item);

     /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     *
     * @param item    the item as sent by the server
     */
    virtual int removeItem(SyncItem& item);

    /**
     * Overrides CacheSyncSource::fillSyncItem() in order to create
     * new FileSyncItem instead of a SyncItem.
     * NOTE: it doesn't really fill the item's data: it just creates the
     * FileSyncItem specifying the file's path, then the file content 
     * will be read chunk by chunk from the input stream when needed.
     * 
     * @param key      the item's key = the file name
     * @param fillData [OPTIONAL] ignored param
     * @return         a new allocated FileSyncItem
     */
    SyncItem* fillSyncItem(StringBuffer* key, const bool fillData = true);
    
    /**
     * Returns the whole file content and size, for the file with name 'key'.
     * Note: this method is no more called by the sync engine, since we now
     * use input streams to retrieve the file's data chunk by chunk.
     */
    virtual void* getItemContent(StringBuffer& key, size_t* size);
        
};

END_NAMESPACE

/** @} */
/** @endcond */
#endif

