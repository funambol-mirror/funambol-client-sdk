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


BEGIN_NAMESPACE


/**
 * Container for parameters used by this MediaSyncSource class.
 * Server URL, Username and Swv are stored inside the MediaSyncSource cache to check 
 * its validity before every sync.
 */
class MediaSyncSourceParams
{
private:
    
    //StringBuffer dir;           /**< The media directory to sync */
    //bool recursive;             /**< If true, will recurse into subfolders of dir. Default is false. */
    StringBuffer url;           /**< The Sync Server URL. */
    StringBuffer username;      /**< The current username. */ 
    StringBuffer swv;           /**< The current Client software version. */
    
public:
    MediaSyncSourceParams() /*: dir("."), recursive(false)*/ {}
    ~MediaSyncSourceParams() {};
    
    //const StringBuffer& getDir()          { return dir;       }
    //const bool          getRecursive()    { return recursive; }
    const StringBuffer& getUrl()          { return url;       }
    const StringBuffer& getUsername()     { return username;  }
    const StringBuffer& getSwv()          { return swv;       }
    
    //void setDir      (const StringBuffer& v) { dir       = v; }
    //void setRecursive(const bool        & v) { recursive = v; }
    void setUrl      (const StringBuffer& v) { url       = v; }
    void setUsername (const StringBuffer& v) { username  = v; }
    void setSwv      (const StringBuffer& v) { swv       = v; }
};



/**
 * This class extends the FileSyncSource class, to define a special behavior for generic
 * "media files" to be synchronized between the Server and a mobile Client.
 * 
 * Differences from FileSyncSource are:
 * - cache file is stored inside the 'dir' folder (the folder under sync)
 * - in case the URL or username changes, the cache file is resetted
 * - in case of slow sync, the cache file is not cleared and new/mod/del items are sent (as if it was a fast sync). 
 * - in case of slow sync, delete items are sent as empty update items (the Server doesn't expect deletes during slow syncs)
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

    ~MediaSyncSource() {};
    
    /**
     * Overrides FileSyncSource::beginSync().
     * Checks if the pictures cache is still valid, before starting a sync.
     * If not, the cache is cleared.
     */
    int beginSync();

    /**
     * Overrides CacheSyncSource::getFirstItem(), for smart slow-sync.
     * Calls getFirstNewItem(), as it was a fast sync.
     */
    SyncItem* getFirstItem();
    
    /**
     * Overrides CacheSyncSource::getNextItem(), for smart slow-sync.
     * Calls getNew/Updated/DeletedItem() in sequence, as it was
     * a normal fast-sync.
     */
    SyncItem* getNextItem();
    
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
   
    
protected:
    
    /// Contains parameters used by this class.
    MediaSyncSourceParams params;
    
    /**
     * Overrides CacheSyncSource::fillItemModifications().
     * Called for every sync (slow or fast). 
     * Will call the CacheSyncSource::fillItemModifications() and then
     * cleanup the special properties found in the cache (url, username, swv).
     */
    virtual bool fillItemModifications();
    
    /**
     * Used to filter outgoing items (overrides FileSyncSource::filterOutgoingItem)
     * Filters the cache files (*.dat).
     * 
     * @param fullName  the full path + name of the file to check
     * @param st        reference to struct stat for current file
     * @return          true if the item has to be filtered out (skipped)
     *                  false if the item is ok
     */ 
    virtual bool filterOutgoingItem(const StringBuffer& fullName, struct stat& st);
    
    /**
     * Overrides CacheSyncSource::saveCache().
     * Saves the cache and the LUID_Map into the persistent store.
     * Adds the special properties (url, username, swv) to the cache file.
     * Set the files attribute to hidden (TODO).
     */
    virtual int saveCache();
    
    
private:
    
    /// Used during (smart) slow syncs. If true, means the New items are finished.
    bool smartSlowNewItemsDone;
    
    /// Used during (smart) slow syncs. If true, means the Updated items are finished.
    bool smartSlowUpdatedItemsDone;
    
    /// Used during (smart) slow syncs. If true, means we need to retrieve the first item of new/mod/del lists.
    bool smartSlowFirstItem;
    
    
    /**
     * Read the URL, username and client sw version from the cache. 
     * If wrong (different from the passed ones) or missing, the current cache is not valid (true is returned).
     * This method is called by MediaSyncSource::beginSync().
     * 
     * @return  true if the cache file is valid, false if not.
     */
    bool checkCacheValidity();

};

END_NAMESPACE

#endif /*MEDIASOURCESYNC_H_*/
