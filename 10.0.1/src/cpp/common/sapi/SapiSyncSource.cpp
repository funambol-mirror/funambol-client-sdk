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

#include "sapi/SapiSyncSource.h"

BEGIN_FUNAMBOL_NAMESPACE

// the name of the repository
#define SAPI_STORAGE_FOLDER    "sapi_media_storage"
#define SAPI_CACHE             "sapi_cache"
#define SAPI_MAPPINGS          "sapi_mappings"
#define SAPI_RESUME            "sapi_resume"
#define SAPI_BLACKLIST         "sapi_blacklist"
#define SAPI_STORAGE_FILE_EXT  ".dat"
#define NOT_IMPLEMENTED        "__NOT_IMPLEMENTED__"
#define SAPI_SERVER_DATE_MAPPINGS "server_date_mappings"

// Compose the cache folder taking the config folder from
// the platform adapter and tries to create it if not present.
static bool initCacheDir(StringBuffer& dir) {
    
    dir = PlatformAdapter::getConfigFolder();
    SapiSyncSource::normalizeSlash(dir);
    int len = dir.length();
    
    if (len == 0) {
        LOG.debug("[%s] - missing dir info to create the cache.", __FUNCTION__);
        return false;
    }
    
    char pathEnd = dir[len - 1];
    // check if cache dir path terminates with slash or backslash
    if ((pathEnd != '/') && (pathEnd != '\\')) {
        dir += "/";
    }
    
    dir += SAPI_STORAGE_FOLDER;

    if (createFolder(dir)){
        LOG.error("initCacheDir: error creating cache directory '%s'", dir.c_str());
        dir = NULL;
        return false;
    }
    return true;
}

/// Returns true if passed syncMode is upload enabled
static bool isUploadEnabled(const SyncMode syncMode) {
    
    if (syncMode == SYNC_TWO_WAY || 
        syncMode == SYNC_ONE_WAY_FROM_CLIENT ||
        syncMode == SYNC_REFRESH_FROM_CLIENT) {
        return true;
    }
    return false;
}


bool SapiSyncSource::init(SyncSourceConfig& sc,  
                            KeyValueStore* mappings, 
                            KeyValueStore* cache,
                            KeyValueStore* blacklist, 
                            const char* storageLocation) {

    allItemInfo     = NULL;     
    newItemInfo     = NULL;     
    updatedItemInfo = NULL;     
    deletedItemInfo = NULL;
    isSyncingItemChanges = false;

    // get the default directory of the 
    StringBuffer localStorage("");
    StringBuffer name = sc.getName();
    StringBuffer fileName;

    if (storageLocation) {
        if (createFolder(storageLocation) == 0) {
            localStorage = storageLocation;
            normalizeSlash(localStorage);
        } else {
            LOG.info("[%s] - The storageLocation passed by the client is not valid. Cannot create it and use default one");
        }
    }

    if (localStorage.empty() == true) {
        // init the storage with default values 
        if(initCacheDir(localStorage) == false) {   
            setErrorF(ERR_FILE_SYSTEM, "Cannot create cache file, sync not done.");
            this->resume   = NULL;
            this->cache    = NULL;
            this->mappings = NULL;
            this->serverDateMappings = NULL;
            this->blackList = NULL;
            return false;
        }
    }
    
    // init the resume   
    fileName = localStorage;
    fileName += "/";
    fileName += name;
    fileName += "_";
    fileName += SAPI_RESUME;
    fileName += SAPI_STORAGE_FILE_EXT;

    LOG.debug("[%s]: resume pathname is %s", __FUNCTION__, fileName.c_str());
    this->resume = new PropertyFile(fileName);
    
    if (cache) {
        this->cache = cache;
    } else {
         // init the cache
        fileName = localStorage;
        fileName += "/";
        fileName += name;
        fileName += "_";
        fileName += SAPI_CACHE;
        fileName += SAPI_STORAGE_FILE_EXT;

        LOG.debug("[%s]: cache pathname is %s", __FUNCTION__, fileName.c_str());
        this->cache = new PropertyFile(fileName);
    }

    if (mappings) {
        this->mappings = mappings;
    } else {
        // init the mappings   
        fileName = localStorage;
        fileName += "/";
        fileName += name;
        fileName += "_";
        fileName += SAPI_MAPPINGS;
        fileName += SAPI_STORAGE_FILE_EXT;

        LOG.debug("[%s]: mapping pathname is %s", __FUNCTION__, fileName.c_str());
        this->mappings = new PropertyFile(fileName);
    }

    // init the serverDate mappings (GUID - lastmod time)  
    fileName = localStorage;
    fileName += "/";
    fileName += name;
    fileName += "_";
    fileName += SAPI_SERVER_DATE_MAPPINGS;
    fileName += SAPI_STORAGE_FILE_EXT;
    LOG.debug("[%s]: serverdate mapping pathname is %s", __FUNCTION__, fileName.c_str());
    this->serverDateMappings = new PropertyFile(fileName);
    
    if (blacklist) {
        this->blackList = blacklist;
    } else {
        // init the mappings   
        fileName = localStorage;
        fileName += "/";
        fileName += name;
        fileName += "_";
        fileName += SAPI_BLACKLIST;
        fileName += SAPI_STORAGE_FILE_EXT;

        LOG.debug("[%s]: blacklist pathname is %s", __FUNCTION__, fileName.c_str());
        this->blackList = new PropertyFile(fileName);
    }
    
    const char* ext = sc.getProperty(PROPERTY_EXTENSION);
    if (ext) {
        StringBuffer vals(ext);
        vals.split(extension, ",");
    }
    if (extension.size() == 0) {
        LOG.debug("[%s] No filter on extension: all allowed outgoing", __FUNCTION__);
    }
    
    
    return true;
}


SapiSyncSource::SapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& rep, size_t incomingFilterD, size_t outgoingFilterD, const char* storageLocation) 
                : config(sc), report(rep), incomingFilterDate(incomingFilterD), outgoingFilterDate(outgoingFilterD) {
    
    if (init(sc, NULL, NULL, NULL, storageLocation) == false) {
        LOG.debug("[%s] - initialization failed (sc, storageLocation)", __FUNCTION__);
    }
    
}

SapiSyncSource::SapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& rep, KeyValueStore* mappings, 
               KeyValueStore* cache, KeyValueStore* blacklist, size_t incomingFilterD, size_t outgoingFilterD, const char* storageLocation) 
                : config(sc), report(rep),  incomingFilterDate(incomingFilterD), outgoingFilterDate(outgoingFilterD)   {

    if (init(sc, mappings, cache, blacklist, storageLocation) == false) {
        LOG.debug("[%s] - initialization failed (sc,  mappings, cache, storageLocation)", __FUNCTION__);
    }
}

void SapiSyncSource::sortByModificationTime(ArrayListEnumeration* list, int maxItemsToSort) {

    if (!list) { 
        return; 
    }

    if (list->size() > maxItemsToSort) {
        LOG.debug("No sorting by modification time: too many items (%d)", list->size());
        return;
    }

    LOG.debug("Sorting by modification date (%d items)", list->size());
    for(int i = 0; i < list->size(); i++)
    {
        for(int j = 0; j < list->size()-1; j++)
        {
            if(((SapiSyncItemInfo*)list->get(j))->getModificationDate() > ((SapiSyncItemInfo*)list->get(j+1))->getModificationDate() ){
                SapiSyncItemInfo* jItem = (SapiSyncItemInfo*)((SapiSyncItemInfo*)(list->get(j)))->clone();
                SapiSyncItemInfo* j1Item = (SapiSyncItemInfo*)((SapiSyncItemInfo*)(list->get(j+1)))->clone();

                list->removeElementAt(j+1);
                list->removeElementAt(j);
                list->add(j , *j1Item);
                list->add(j+1, *jItem);
                delete jItem;
                delete j1Item;
            }
        }
    } 
}

SapiSyncSource::~SapiSyncSource() {

    if (newItemInfo)     { delete newItemInfo;     } 
    if (updatedItemInfo) { delete updatedItemInfo; } 
    if (deletedItemInfo) { delete deletedItemInfo; } 
    if (allItemInfo)     { delete allItemInfo;     }
    if (cache)           { delete cache;       }
    if (resume)          { delete resume;      }
    if (mappings)        { delete mappings;    }
    if (serverDateMappings) { delete serverDateMappings; }
    if (blackList)       { delete blackList;   }
}
/**
 * Begins the synchronization for this source.
 * Check last sync time and initialize local items lists.
 *
 * The allItems list is always populated, and each item (SyncItemInfo) in
 * this list is filtered with the filterOutgoingItem() method.
 * The new/mod/delItems lists are populated only if client
 * and server are already in sync (comparing the allItems list with the cache).
 */
int SapiSyncSource::beginSync(bool changes, AbstractSyncConfig& mainConfig) {
       
    LOG.debug("[%s]", __FUNCTION__);
    int ret = 0;
    isSyncingItemChanges = changes;

    //
    // Always populate ALL items list
    //
    if (populateAllItemInfoList(mainConfig) == false) {
        return 1;
    }

    // NEW/MOD/DEL lists are populated only if:
    //  - we're syncing item changes (next syncs)
    //  - upload is enabled (optimization, local changes would not be used)
    SyncMode syncMode = syncModeCode(config.getSync());
    if (changes && isUploadEnabled(syncMode)) {

        populateModifiedItemInfoList(mainConfig); 
    
        if (newItemInfo == NULL || newItemInfo->size() == 0) {
            LOG.debug("[%s] - populateModifiedItemInfoList returns there are no new items on the client", __FUNCTION__);
        } 
        if (updatedItemInfo == NULL || updatedItemInfo->size() == 0) {
            LOG.debug("[%s] - populateModifiedItemInfoList returns there are no updated items on the client", __FUNCTION__);
        } 
        if (deletedItemInfo == NULL || deletedItemInfo->size() == 0) {
            LOG.debug("[%s] - populateModifiedItemInfoList returns there are no deleted items on the client", __FUNCTION__);
        } 
    }
    return ret;

}


bool SapiSyncSource::filterByExtension(SapiSyncItemInfo& itemInfo) {
 
    if (extension.size() == 0) {        
        return false; // wee keep it
    }
    
    StringBuffer s = itemInfo.getName();
    s.lowerCase();
    
    bool allowed = true;    
    
    if (extension.size() > 0) {
        StringBuffer* val = (StringBuffer*)extension.get(0);
        if (val && strcmp(val->c_str(), "!") == 0) {
            allowed = false;
        }
    }    
    
    bool ret;
    int i;
    
    if (allowed) { ret = true;   i = 0; } 
    else         { ret = false;  i = 1; }
    
    for (i; i < extension.size(); i++) {
        if (s.endsWith(((StringBuffer*)extension.get(i))->c_str())) {
            if (allowed) { ret = false; } 
            else         { ret = true;  }            
            break;
        }
    }

    if (ret) {
        LOG.debug("[%s] item %s filtered (extension not supported)", __FUNCTION__, itemInfo.getName().c_str());
    }
    return ret;
    
    
}

bool SapiSyncSource::filterFromBlackList(SapiSyncItemInfo& itemInfo) {

    bool ret = false;
    StringBuffer& guid = itemInfo.getGuid();
   
    if (guid.empty()) {
        return ret;
    }
    
    Enumeration& blackListEnum = blackList->getProperties();
   
    while (blackListEnum.hasMoreElement()) {
        KeyValuePair* kvp = (KeyValuePair*)(blackListEnum.getNextElement());
        StringBuffer blackListedItemGuid = kvp->getKey();
        
        if (blackListedItemGuid.empty()) {
            continue;
        }
        
        if (strcmp(blackListedItemGuid.c_str(), guid.c_str()) == 0) {
            LOG.debug("%s: item found in blacklist [guid '%s']", __FUNCTION__,
                guid.c_str());
            ret = true;
            break;
        }
    }
    
    return ret;
}

bool SapiSyncSource::filterOutgoingItem(SapiSyncItemInfo& clientItemInfo) {
        
    bool err;
    bool ret = false;
    
    //
    // 1. Check for item size  (0 means disabled)
    //
    int maxItemSize = config.getIntProperty(PROPERTY_SYNC_ITEM_MAX_SIZE, &err);
    
    if (!err && maxItemSize>0) {
        size_t itemSize = clientItemInfo.getSize();
        
        if (itemSize > (unsigned int)maxItemSize) {
            LOG.info("%s: discarding local item of size %lu (maximum item size allowed exceeded)",
                __FUNCTION__, itemSize);
            return true;
        }
    }
    
    //
    // 2. Filter by the number of item to send out. If there is a number in the config it uses it 
    // without any check on the date
    //
    int number = config.getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
    if (err) {
        number = -1;  // default = disabled
    }
    if (number > 0) {
        return false; // don't consider the date
    }
    
    //
    // 3. Check if the modification date is over the filter date
    //
    if (clientItemInfo.getModificationDate() >= outgoingFilterDate) {
        return false;  // we keep it
    } else {
        return true;   // we discard it
    }
}

bool SapiSyncSource::populateModifiedItemInfoList(AbstractSyncConfig& mainConfig) {

    if (allItemInfo == NULL || allItemInfo->isEmpty()) {
        LOG.debug("AllItemsInfo is empty");
        return false;
    }
    
    newItemInfo = new ArrayListEnumeration();
    updatedItemInfo = new ArrayListEnumeration();
    deletedItemInfo = new ArrayListEnumeration();

    // all the action are done on the copy so we can delete
    // the element found. The remained are the deleted by the user.        
    Enumeration& e = cache->getProperties();
    ArrayList cacheCopy;
    while(e.hasMoreElement()) {
        cacheCopy.add(*e.getNextElement());
    }

    SapiSyncItemInfo* itemInfo;
    KeyValuePair* kvp;
   
    if (allItemInfo) {
        while(allItemInfo->hasMoreElement()) {

            // check user aborted
            if (mainConfig.isToAbort()) {
                return false;
            }

            itemInfo = (SapiSyncItemInfo*)allItemInfo->getNextElement();
            StringBuffer& luid = itemInfo->getLuid();
            int size = cacheCopy.size();
            bool foundnew = true;

            for (int i = 0; i < size; i++) {
                kvp = (KeyValuePair*)(cacheCopy[i]);
                if (strcmp(kvp->getKey(), luid.c_str()) == 0) {
                    foundnew = false;               
                    // see if it is updated.  
                    long val = 0;
                    if (kvp->getValue()) {
                        val = atol(kvp->getValue());
                    }
                    if (itemInfo->getModificationDate() != val) {
                        // there is an update. if equal nothing to do...
                        updatedItemInfo->add(*itemInfo);                      
                    }
                    cacheCopy.removeElementAt(i);
                    break;
                }
            }
            if (foundnew) {
                newItemInfo->add(*itemInfo);
            }
        }
    }
    
    // deleted
    for (kvp = (KeyValuePair*)cacheCopy.front(); kvp; kvp = (KeyValuePair* )cacheCopy.next()) {
        long lastModDate = atol(kvp->getValue().c_str());
        SapiSyncItemInfo s;
        s.setLuid(kvp->getKey().c_str());
        s.setModificationDate(lastModDate);
        deletedItemInfo->add(s);    // deletedItemInfo contains only LUID and lastModDate!
    }    
    
    return true;
}

SapiSyncItemInfo* SapiSyncSource::twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array) {
    
    ArrayListEnumeration* list = allItemInfo;
    
    if (array && strcmp(array,"NEW") == 0) {
        list = newItemInfo;
    } 
    
    SapiSyncItemInfo* ret = NULL;
    if (list && list->size() > 0) {

        SapiSyncItemInfo* itemInfo;
        for (int i = 0; i < list->size(); i++) {
            itemInfo = (SapiSyncItemInfo*)list->get(i);
            if (itemInfo->getSize() == serverItemInfo.getSize()) {
                if (itemInfo->getName() != serverItemInfo.getName()) {
                    LOG.info("[%s] - Item size is the same but name is different. Server: %s - Client: %s", 
                                                    __FUNCTION__, serverItemInfo.getName().c_str(), itemInfo->getName().c_str());
                }

                ret = (SapiSyncItemInfo*)itemInfo->clone();
                list->removeElementAt(i);
                break;
            }
        } 
    }
    return ret;
}

bool SapiSyncSource::filterIncomingItem(SapiSyncItemInfo& serverItemInfo, time_t offsetTime) {

    bool ret = false;  // we want to keep it from the server
    bool err;
   
    //
    // 1. (blacklisted item) if the item is not supported by sync source don't to download it.
    //
    ret = filterFromBlackList(serverItemInfo);
    if (ret == true) {
        return ret;
    }
    
    //
    // 2. if the extension is not supported we don't want to download it.
    //
    ret = filterByExtension(serverItemInfo);
    if (ret == true) {
        return ret;
    }
    
    //
    // 3. Check for item size (0 means disabled)
    //
    int maxItemSize = config.getIntProperty(PROPERTY_SYNC_ITEM_MAX_SIZE, &err);
    
    if (!err  && maxItemSize>0) {
        size_t itemSize = serverItemInfo.getSize();
        
        if (itemSize > (unsigned int)maxItemSize) {
            LOG.info("%s: discarding server item of size %lu (maximum item size allowed exceeded)",
                __FUNCTION__, itemSize); 
            return true;
        }
    }
    
    //
    // 4. Filter by the number of item to get in. If there is a number in the config it uses it 
    // without any check on the date
    //
    int number = config.getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
    if (err) {
        number = -1;  // default = disabled
    }
    if (number > 0) {
        return false; // don't consider the date
    }
    
    //
    // 5. if the date is less then the filter one, we don't want it
    //
    if (serverItemInfo.getModificationDate() < (incomingFilterDate - offsetTime)) {
        ret = true;  // we discard it from the server
    }
    return ret;
    
}

SapiSyncItemInfo* SapiSyncSource::copyAndRemoveSapiSyncItemInfo(const StringBuffer& luid) {
    
    SapiSyncItemInfo* itemInfo = NULL;   

    if (isSyncingItemChanges) {
        itemInfo = copyAndRemoveSapiSyncItemInfo(luid, newItemInfo);
        if (itemInfo) {
            return itemInfo;
        }
        itemInfo = copyAndRemoveSapiSyncItemInfo(luid, updatedItemInfo);
        // DEL items list is excluded
    }
    else {
        itemInfo = copyAndRemoveSapiSyncItemInfo(luid, allItemInfo);
    }

    return itemInfo;
}

SapiSyncItemInfo* SapiSyncSource::copyAndRemoveSapiSyncItemInfo(const StringBuffer& luid, ArrayListEnumeration* list) {

    if (list) {
        for (int i=0; i<list->size(); i++) {
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)list->get(i);
            if (!itemInfo) continue;

            if (itemInfo->getLuid() == luid) {
                SapiSyncItemInfo* ret = (SapiSyncItemInfo*)list->get(i)->clone();
                list->removeElementAt(i);                    
                return ret;
            }                
        } 
    }
    return NULL;
}

SapiSyncItemInfo* SapiSyncSource::getItemInfo(const StringBuffer& luid) {

    SapiSyncItemInfo* itemInfo = NULL;   

    // scan all + new + mod items lists
    itemInfo = getItemInfo(luid, allItemInfo);
    if (itemInfo) return itemInfo;

    itemInfo = getItemInfo(luid, newItemInfo);
    if (itemInfo) return itemInfo;

    itemInfo = getItemInfo(luid, updatedItemInfo);

    return itemInfo;   
}

SapiSyncItemInfo* SapiSyncSource::getItemInfo(const StringBuffer& luid, ArrayListEnumeration* list) {

    if (list) {
        for (int i=0; i<list->size(); i++) {
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)list->get(i);
            if (!itemInfo) continue;

            if (itemInfo->getLuid() == luid) {                 
                return itemInfo;
            }                
        } 
    }
    return NULL;
}


UploadSapiSyncItem* SapiSyncSource::getItem(const StringBuffer& luid) {
    
    InputStream* istream = createInputStream(luid.c_str());
    
    if (istream == NULL) {
        // Not an error, luid may not exist.
        LOG.debug("[%s] - cannot create input stream for item %s", __FUNCTION__, luid.c_str());
        return NULL;
    }
   
    SapiSyncItemInfo* info = copyAndRemoveSapiSyncItemInfo(luid);
    if (info == NULL) {
        LOG.error("[%s] - error removing itemInfo from all the lists", __FUNCTION__);
        return NULL;
    }
    
    UploadSapiSyncItem* item = new UploadSapiSyncItem(info, istream);

    return item;
}


UploadSapiSyncItem* SapiSyncSource::getUploadItem(ArrayListEnumeration* list, int pos) {
    
    if (list == NULL) {
        LOG.debug("[%s] - the list is NULL", __FUNCTION__);
        return NULL;
    }

    SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)list->get(pos);
        
    if (itemInfo == NULL) {
        LOG.debug("[%s] - no iteminfo retrieved by the list", __FUNCTION__);
        return NULL;
    }
    
    InputStream* istream = createInputStream(itemInfo->getLuid().c_str());
    
    if (istream == NULL) {
        LOG.error("[%s] - no InputStream retrieved with the current luid %s", __FUNCTION__, itemInfo->getLuid().c_str());
        return NULL;
    }

    SapiSyncItemInfo* info = copyAndRemoveSapiSyncItemInfo(itemInfo->getLuid(), list);
    if (info == NULL) {
        LOG.error("[%s] - error removing itemInfo from all the lists", __FUNCTION__);
        return NULL;
    }
    /*
    bool ret = removeItemInfoFromList(list, itemInfo->getLuid());
    if (ret == false) {
        LOG.info("[%s] - error removing itemInfo from the allItemInfo list", __FUNCTION__);
    }
    */
    UploadSapiSyncItem* item = new UploadSapiSyncItem(info, istream);
    return item;

}


/**
 * Called by SapiSyncManager before uploading an item.
 * Returns the next SapiSyncItem of allItems list.
 * The SapiSyncItem contains the inputStream attached to 
 * the stream of item's data to upload.
 * @return new allocated SapiSyncItem / NULL if no more items
 */
UploadSapiSyncItem* SapiSyncSource::getNextItem(int* err) {
        
    LOG.debug("[%s] - get item number %i from allItemList", __FUNCTION__, allItemInfo ? allItemInfo->size() : 0);

    //UploadSapiSyncItem* item = getUploadItem(allItemInfo, allItemInfoPos);
    //allItemInfoPos++;
    
    UploadSapiSyncItem* item = getUploadItem(allItemInfo);
    *err = 0;
    return item;    
}

UploadSapiSyncItem* SapiSyncSource::getNextNewItem(int* err) {
    
    LOG.debug("[%s] - get item number %i from newItemInfo", __FUNCTION__, newItemInfo ? newItemInfo->size() : 0);

    UploadSapiSyncItem* item = getUploadItem(newItemInfo);
    *err = 0;
    return item;
}

UploadSapiSyncItem* SapiSyncSource::getNextModItem(int* err) {
    
    LOG.debug("[%s] - get item number %i from updatedItemInfo", __FUNCTION__, updatedItemInfo ? updatedItemInfo->size() : 0);
    LOG.info("[%s] - UPDATE FROM CLIENT NOT YET IMPLEMENTED", __FUNCTION__);

    // UploadSapiSyncItem* item = getUploadItem(updatedItemInfo);
    // return item;
    *err = 0;
    return NULL;
}

SapiSyncItemInfo* SapiSyncSource::getNextDelItem(int* err) {
    
    LOG.debug("[%s] - get item number %i from deletedItemInfo", __FUNCTION__, deletedItemInfo ? deletedItemInfo->size() : 0);
    LOG.debug("[%s] - DELETE FROM CLIENT NOT YET IMPLEMENTED", __FUNCTION__);
    /*
    SapiSyncItemInfo* item = NULL;
    if (deletedItemInfo && deletedItemInfo->isEmpty() == false) {
        SapiSyncItemInfo* info = (SapiSyncItemInfo*)deletedItemInfo->get(0);
        if (info) {
            item = copyAndRemoveSapiSyncItemInfo(info->getLuid(), deletedItemInfo);        
            if (item == NULL) {
                LOG.error("[%s] - error removing itemInfo from all the lists", __FUNCTION__);
                return NULL;
            }
        }
    }
    *err = 0;
    return item;
    */
    *err = 0;
    return NULL;
}

bool SapiSyncSource::isErrorCode(int code) {    
    if (code == 0) {
        return false;
    } else {
        return true;
    }
}

void SapiSyncSource::setItemStatus(SapiSyncItemInfo& itemInfo, int status, const char* command) {
     
    KeyValuePair vp;
    if (!isErrorCode(status)) {
        LOG.debug("Received success status code from server for command %s on source %s on item with key %s - code: %d", 
                         command, config.getName(), itemInfo.getLuid().c_str(), status);    

        vp.setKey(itemInfo.getLuid().c_str());
        if (strcmp(command, COMMAND_DELETE)) {
            vp.setValue(StringBuffer().sprintf("%li", (long)itemInfo.getModificationDate()));
        }
    } else {                    
            LOG.info("Received failed status code from server for command %s on source %s on item with key %s - code: %d", 
                            command, config.getName(), itemInfo.getLuid().c_str(), status);                    
    }    
    if (vp.getKey()) {
        updateInCache(vp, command);
    }
}

int SapiSyncSource::getItemsNumber(const char* value) {
    
    int ret = -1;
    ArrayListEnumeration* list = NULL;

    if (value == NULL) {
        return ret;
    }    
    
    if (strcmp(value, "ALL") == 0) {
        list = allItemInfo;
        ret = 0;
    } else if (strcmp(value, "NEW") == 0) {
        list = newItemInfo;
        ret = 0;
    } else if (strcmp(value, "MOD") == 0) {
        list = updatedItemInfo;
        ret = 0;
    } else if (strcmp(value, "DEL") == 0) {
        list = deletedItemInfo;
        ret = 0;
    }
    
    if (list) {
        ret = list->size();
    } 

    return ret;
}

ArrayListEnumeration* SapiSyncSource::getItemsList(const char* value) {
        
    ArrayListEnumeration* list = NULL;

    if (value == NULL) {
        return list;
    }    
    
    if (strcmp(value, "ALL") == 0) {
       list = allItemInfo;
    } else if (strcmp(value, "NEW") == 0) {
        list = newItemInfo;
    } else if (strcmp(value, "MOD") == 0) {
        list = updatedItemInfo;
    } else if (strcmp(value, "DEL") == 0) {
        list = deletedItemInfo;
    }
        
    return list;
}


/**
 * Called by SapiSyncManager, before downloading a new item. *
 * @param  itemInfo the item's metadata info
 * @return a new allocated SapiSyncItem
 */
DownloadSapiSyncItem* SapiSyncSource::createItem(SapiSyncItemInfo& itemInfo) {
    
    OutputStream* ostream = createOutputStream(itemInfo);
    
    if (ostream == NULL) {
        LOG.error("[%s] - Error creating a new DownloadSapiSyncItem", __FUNCTION__);
        return NULL;
    }        
    SapiSyncItemInfo* info = (SapiSyncItemInfo*)itemInfo.clone();
    DownloadSapiSyncItem* item = new DownloadSapiSyncItem(info, ostream);
        
    return item;
}

/**
 * Called by SapiSyncManager after downloading an item to add (after call to createItem)
 * Stores the passed DownloadSapiSyncItem to the local storage.
 * Updates the cache file.
 * @param syncItem the item to add
 * @return the LUID of the new item added. Empty string if there was an error
 */
StringBuffer SapiSyncSource::addItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode) {
    
    if (syncItem == NULL) {
        LOG.error("[%s] - The syncItem is NULL", __FUNCTION__);
        *errCode = ESSSInvalidItem;
        return "";
    }

    long modTime = 0;
    StringBuffer res = insertItem(syncItem, errCode, &modTime);
    if (res == "") {
        LOG.error("[%s] - error in adding item. The luid of the item to insert is empty", __FUNCTION__);

        // source status code has been updated by insertItem
        if (*errCode == ESSSItemNotSupported) {
            blackListItem(syncItem->getSapiSyncItemInfo());
        }
        return "";
    }
    
    // Update cache: <LUID - lastModTime>
    KeyValuePair kp;    
    kp.setKey(res.c_str());
    kp.setValue(StringBuffer().sprintf("%li", modTime).c_str());
    updateInCache(kp, COMMAND_ADD);

    return res;

}

StringBuffer SapiSyncSource::changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) {
    LOG.info("The %s is not implemented...", __FUNCTION__);
    *modificationDate = 0;
    *errCode = ESSSNoErr;
    return NOT_IMPLEMENTED;
}

StringBuffer SapiSyncSource::updateItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode) {    
    
    if (syncItem == NULL) {
        LOG.error("[%s] - The syncItem is NULL", __FUNCTION__);
        *errCode = ESSSInternalError;
        return "";
    }
    StringBuffer res("");

    long lastModTime = 0;
    if (syncItem->getSapiSyncItemInfo()->getLuid().empty()) {
        LOG.debug("[%s] - calling add item", __FUNCTION__);
        return addItem(syncItem, errCode);
    } else {
        res = changeItem(syncItem, errCode, &lastModTime);                
    }    
    
    if (res == NOT_IMPLEMENTED) {
        return "";
    } else if (res.empty()) {
        LOG.error("%s error update item with key %s", __FUNCTION__, syncItem->getSapiSyncItemInfo()->getLuid().c_str());
        return res;
    }
    
    // Just to log a warning: realSize in not really used.
    int realSize = 0;
    if (syncItem->getSapiSyncItemInfo()->isRename() == false) {
        realSize = syncItem->getStream()->size();
        LOG.debug("[%s] - luid: %s, size from server: %i", __FUNCTION__, res.c_str(), realSize);

        if (realSize != syncItem->getSapiSyncItemInfo()->getSize()) {
            LOG.debug("Warning: item size declared (%d) does not match the real size!", 
                syncItem->getSapiSyncItemInfo()->getSize());
        }
    }

    // Update cache: <LUID - lastModTime>
    KeyValuePair kp;    
    kp.setKey(res.c_str());
    kp.setValue(StringBuffer().sprintf("%li", lastModTime).c_str());
    updateInCache(kp,COMMAND_REPLACE);

    return res;


}

int SapiSyncSource::removeItem(const StringBuffer& identifier) {
    LOG.info("The %s is not implemented...", __FUNCTION__);
    return -1;
}

int SapiSyncSource::deleteItem(SapiSyncItemInfo& itemInfo) {
    
    int ret = removeItem(itemInfo.getLuid());
    if (ret >= 0) { // removeItem successfully done
        KeyValuePair kp;    
        kp.setKey(itemInfo.getLuid());
        updateInCache(kp, COMMAND_DELETE);
        LOG.debug("[%s] - removed item and cache updated properly: id %s", __FUNCTION__, itemInfo.getLuid().c_str());        
    } 
    else if (ret == -1) {
        LOG.debug("[%s] - NOT IMPLEMENTED", __FUNCTION__);
    } 
    else {    
        LOG.error("[%s] - error in removing item. The luid is %s", __FUNCTION__, itemInfo.getLuid().c_str());        
    }
    return ret;    
}

int SapiSyncSource::endUpload() {
    saveCache();
    return 0;
}

int SapiSyncSource::endDownload() {
    saveCache();
    return 0;
}

int SapiSyncSource::endSync() {

    saveCache();    
    closingSync();    
    return 0;
}


int SapiSyncSource::saveCache() {
    
    LOG.debug("[%s] Saving cache of %s", __FUNCTION__, config.getName());
    int ret = cache->close();
    return ret;
}

int SapiSyncSource::updateInCache(KeyValuePair& k, const char* action) {

    if (strcmp(action, COMMAND_ADD    ) == 0 ||
        strcmp(action, COMMAND_REPLACE) == 0) {        
        cache->setPropertyValue(k.getKey(), k.getValue());
    } else if (strcmp(action, COMMAND_DELETE) == 0) {
        cache->removeProperty(k.getKey());
    }
    return 0;
}

void SapiSyncSource::normalizeSlash(StringBuffer& s) {
    s.replaceAll("\\", "/");
}

int SapiSyncSource::cleanTemporaryDownloadedItem(const StringBuffer& item) {
    
    LOG.info("The %s is not implemented...", __FUNCTION__);
    return -2;
}

void SapiSyncSource::validateLocalLists() {
    //
    // Only NEW items are supported, client to server.
    //
    if (updatedItemInfo && updatedItemInfo->size() > 0) {
        LOG.info("Updates from client to server are NOT supported (%d items skipped)", updatedItemInfo->size());
        updatedItemInfo->clear();
    }
    if (deletedItemInfo && deletedItemInfo->size() > 0) {
        LOG.info("Deletes from client to server are NOT supported (%d items skipped)", deletedItemInfo->size());
        deletedItemInfo->clear();
    }
}

void SapiSyncSource::validateRemoteLists(ArrayList* newServerItems, 
                                         ArrayList* modServerItems, 
                                         ArrayList* delServerItems) {
    //
    // Only NEW items are supported, server to client.
    //
    if (modServerItems && modServerItems->size() > 0) {
        LOG.info("Updates from server to client are NOT supported (%d items skipped)", modServerItems->size());
        modServerItems->clear();
    }
    if (delServerItems && delServerItems->size() > 0) {
        LOG.info("Deletes from server to client are NOT supported (%d items skipped)", delServerItems->size());
        delServerItems->clear();
    }
}


int SapiSyncSource::blackListItem(SapiSyncItemInfo* itemInfo)
{
    KeyValuePair kp;    
    
    if (itemInfo == NULL) {
        LOG.error("%s: invalid item info", __FUNCTION__);
        
        return 1;
    }
    
    StringBuffer itemGuid = itemInfo->getGuid();
    
    if (itemGuid.empty()) {
        LOG.error("%s: can't get item guid from item info data", __FUNCTION__);
        
        return 1;
    }
    
    StringBuffer itemName = itemInfo->getName();
    
    if (itemName.empty()) {
        LOG.error("%s: can't get item name from item info data", __FUNCTION__);
        
        return 1;
    }
    
    kp.setKey(itemGuid);
    kp.setValue(itemName);
   
    LOG.info("%s: adding item [guid %s - name '%s'] to blacklist", __FUNCTION__,
        itemGuid.c_str(), itemName.c_str());
        
    blackList->setPropertyValue(kp.getKey(), kp.getValue()); 
    
    return 0;
}

void SapiSyncSource::localRenameChecks() {

    if (!newItemInfo || !updatedItemInfo || !deletedItemInfo) {
        // nothing to do
        return;
    }

    for (int i=0; i<newItemInfo->size(); i++) {

        SapiSyncItemInfo* newItem = (SapiSyncItemInfo*)newItemInfo->get(i);
        if (!newItem) continue;

        for (int j=0; j<deletedItemInfo->size(); j++) {
            SapiSyncItemInfo* delItem = (SapiSyncItemInfo*)deletedItemInfo->get(j);
            if (!delItem) continue;
            // NOTE: delItemInfo just has the basic info retrieved from the cache (LUID and lastModDate)

            if (newItem->getModificationDate() == delItem->getModificationDate()) {
                // Same last modification date: this is a rename!
                // Also flag this updated item as a rename, to avoid syncing all the item's data!
                LOG.debug("local item %s is a rename (old luid: %s): moving to the MOD list!", 
                    newItem->getName().c_str(), delItem->getLuid().c_str());
                
                // Don't flag it: SAPI rename not yet implemented!
                // (the item's data will be uploaded)
                //newItem->setRename(true);

                // fill the missing GUID in the updated item, otherwise it would not be treated as an update
                newItem->setGuid(delItem->getGuid().c_str());
                updatedItemInfo->add(*newItem);

                // update the cache: the LUID (which is the key) has changed!
                // (set a fake lastModTime value here, so the item will be recognized as updated in case
                // the sync fails - the cache will be correctly updated at the end of the upload)
                cache->removeProperty(delItem->getLuid().c_str());
                cache->setPropertyValue(newItem->getLuid().c_str(), "0");

                newItemInfo->removeElementAt(i);
                deletedItemInfo->removeElementAt(j);
                i--;
                j--;
                break;
            }
        }
    }
}

void SapiSyncSource::remoteRenameChecks(ArrayList& modServerItems) {

    for (int i=0; i<modServerItems.size(); i++) {

        SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)modServerItems.get(i);
        if (!itemInfo) continue;

        StringBuffer& luid = itemInfo->getLuid();
        SapiSyncItemInfo* localItemInfo = getItemInfo(luid);
        if (!localItemInfo) continue;

        if ((itemInfo->getName() != localItemInfo->getName()) &&
            (itemInfo->getSize() == localItemInfo->getSize()) ) {
            LOG.debug("remote item '%s' has the same size but different name '%s' -> it's a rename", 
                itemInfo->getName().c_str(), localItemInfo->getName().c_str());
            itemInfo->setRename(true);
        }
    }
}


int SapiSyncSource::resolveConflicts(ArrayList* modServerItems, ArrayList* delServerItems, AbstractSyncConfig& config, time_t offsetTime)
{
    return 0;
}

int SapiSyncSource::resolveConflictOnItemsList(ArrayList* clientItemList, ArrayList* serverItemsList, 
                ESSSConflictResType conflictResType, AbstractSyncConfig& config, time_t offsetTime)
{
    return 0;
}

int SapiSyncSource::pruneModifiedItemsList(ArrayList* modServerItems)
{
    return 0;
}

int SapiSyncSource::updateItemsList(ArrayList* clientItems, ArrayList* serverItems, AbstractSyncConfig& config)
{
    int clientItemsListSize = 0,
        serverItemsListSize = 0; 
    int itemsUpdated = 0;

    LOG.debug("%s: checking for updates on items list", __FUNCTION__);
    
    if ((clientItems) && ((clientItemsListSize = clientItems->size()) > 0)) {
        for (int i=0; i < clientItemsListSize; i++) {
            if (config.isToAbort()) { 
                return -1;
            }
            
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)clientItems->get(i);
            if (!itemInfo) continue;

            StringBuffer luid = itemInfo->getLuid();
            if (luid.empty()) {
                continue;
            }

            StringBuffer guid = getGuidFromLuid(luid.c_str());
            if (!guid.empty()) {
                itemInfo->setGuid(guid.c_str());
                itemsUpdated++;
            }
        }
    }
    
    if ((serverItems) && ((serverItemsListSize = serverItems->size()) > 0)) {
        // Add the missing LUID for every updated item to download (mappings lookup)
        for (int i=0; i < serverItemsListSize; i++) {
            if (config.isToAbort()) { 
                return -1;
            }
            
            SapiSyncItemInfo* itemInfo = (SapiSyncItemInfo*)serverItems->get(i);
            if (!itemInfo) continue;

            StringBuffer guid = itemInfo->getGuid();

            if (guid.empty()) {
                continue;
            }       

            StringBuffer luid = mappings->readPropertyValue(guid.c_str());
            if (!luid.empty()) {
                itemInfo->setLuid(luid.c_str());
                itemsUpdated++;
            }
        }
    }

    return itemsUpdated;
}

StringBuffer SapiSyncSource::getGuidFromLuid(const char* luid) 
{
    if (!luid) {
        return "";
    }

    Enumeration& map = mappings->getProperties();
    while (map.hasMoreElement()) {
        KeyValuePair* kvp = (KeyValuePair*)map.getNextElement();
        if (!kvp) continue;

        StringBuffer val = kvp->getValue();
        if (val == luid) {
            return kvp->getKey();
        }
    }
    return "";
}


END_FUNAMBOL_NAMESPACE

