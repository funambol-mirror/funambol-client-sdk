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

#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "syncml/core/TagNames.h"
#include "base/util/ArrayListEnumeration.h"
#include "client/FileSyncSource.h"
#include "client/MediaSyncSource.h"
#include "base/util/PropertyFile.h"
#include "base/adapter/PlatformAdapter.h"

BEGIN_NAMESPACE

#define MEDIA_CACHE_FILE_NAME   "funambol_cache.dat"
#define MEDIA_LUID_MAP_FILE_NAME "funambol_luid.dat"

#define CACHE_PROPERTY_URL      "_SERVER_URL_"
#define CACHE_PROPERTY_USERNAME "_USERNAME_"
#define CACHE_PROPERTY_SWV      "_CLIENT_SWV_"

#define CONFIG_PROPS_EXT         "_params.ini"      // config props file will be "<sourcename>_params.ini"
#define PROPERTY_NEXT_LUID       "nextLUID"


/**
 * Reads the media cache file.
 * The media cache file is located inside the folder to sync (dir).
 * @param mediaDir  the absolute path where cache file is found
 * @return          new allocated cache, as a KeyValueStore*
 */
static KeyValueStore* getMediaCache(const StringBuffer& dir) 
{
    //LOG.debug("getMediaCache");
    
    StringBuffer cacheFileName(dir);
    if (dir.endsWith("\\") || dir.endsWith("/")) {
        cacheFileName = dir.substr(0, dir.length()-1);
    }
    cacheFileName += "/";
    cacheFileName += MEDIA_CACHE_FILE_NAME;

    LOG.debug("MediaSyncSource: cache file is %s", cacheFileName.c_str());
    
    return new PropertyFile(cacheFileName);
}



MediaSyncSource::MediaSyncSource(const WCHAR* wname, 
                                 AbstractSyncSourceConfig* sc,
                                 const StringBuffer& aDir, 
                                 MediaSyncSourceParams mediaParams) 
                                 : FileSyncSource(wname, sc, aDir, getMediaCache(aDir)), 
                                 params(mediaParams) {
    
    //
    // Load the config_params: "<sourcename>_params.ini"
    //
    StringBuffer configParamsName = PlatformAdapter::getConfigFolder();
    if (!configParamsName.endsWith("\\") && !configParamsName.endsWith("/")) {
        configParamsName += "/";
    }
    configParamsName += getConfig().getName();
    configParamsName += CONFIG_PROPS_EXT;
    LOG.debug("MediaSyncSource: config params file is %s", configParamsName.c_str());
    
    configParams = new PropertyFile(configParamsName);
    
    // Read and set the nextLUID from config_params KeyValueStore
    int next = readNextLUID();
    if (next >= params.getNextLUID()) {
        params.setNextLUID(next);
    }
    
    
    //
    // Load the LUID map (list of path=LUID)
    //
    StringBuffer mapFileName(dir);
    if (dir.endsWith("\\") || dir.endsWith("/")) {
        mapFileName = dir.substr(0, dir.length()-1);
    }
    mapFileName += "/";
    mapFileName += MEDIA_LUID_MAP_FILE_NAME;
    LOG.debug("MediaSyncSource: LUID map file is %s", mapFileName.c_str());
    
    LUIDMap = new PropertyFile(mapFileName);
    
    // Safe check: scan the LUIDMap and check if there's a LUID >= than the passed one.
    if (verifyNextLUIDValue()) {
        LOG.debug("NextLUID updated scanning existing values: next LUID = %d", params.getNextLUID());
        saveNextLUID(params.getNextLUID());
    }
}


MediaSyncSource::~MediaSyncSource() 
{
    if (LUIDMap) { 
        delete LUIDMap;
    }
    if (configParams) {
        delete configParams;
    }
}


int MediaSyncSource::beginSync() {
    
    // If URL or username stored in cache are wrong, reset the cache!
    if (checkCacheValidity() == false) {
        LOG.debug("Resetting cache file");
        clearCache();
    }
    
    // Saves the cache: it's updated with right special props in case
    // something goes wrong during the sync.
    saveCache();
    
    return FileSyncSource::beginSync();
}



bool MediaSyncSource::checkCacheValidity()
{
    const StringBuffer& url      = params.getUrl();
    const StringBuffer& username = params.getUsername();
    //const StringBuffer& swv    = params.getSwv();
    
    StringBuffer cacheUrl        = readCachePropertyValue(CACHE_PROPERTY_URL);
    StringBuffer cacheUsername   = readCachePropertyValue(CACHE_PROPERTY_USERNAME);
    StringBuffer cacheSwv        = readCachePropertyValue(CACHE_PROPERTY_SWV);
    
    //LOG.debug("Current params (%s %s %s)", url.c_str(), username.c_str(), swv.c_str());
    //LOG.debug("Cache params   (%s %s %s)", cacheUrl.c_str(), cacheUsername.c_str(), cacheSwv.c_str());
    
    if (url != cacheUrl || username != cacheUsername) {
        LOG.info("Media cache file is not valid (%s %s %s)", cacheUrl.c_str(), cacheUsername.c_str(), cacheSwv.c_str());
        return false;
    }
    
    //
    // Add here checks about client swv, for backward compatibility.
    //
    return true;
}


int MediaSyncSource::saveCache()
{
    // Update cache with the right values of special props.
    KeyValuePair url, user, swv;
    
    url.setKey (CACHE_PROPERTY_URL);        url.setValue (params.getUrl());
    user.setKey(CACHE_PROPERTY_USERNAME);   user.setValue(params.getUsername());
    swv.setKey (CACHE_PROPERTY_SWV);        swv.setValue (params.getSwv());

    updateInCache(url,  REPLACE);
    updateInCache(user, REPLACE);
    updateInCache(swv,  REPLACE);
    
    //
    // Persist the cache in memory.
    //
    int ret = FileSyncSource::saveCache();
    
    //
    // Refresh and save the LUIDMap file
    //
    refreshLUIDMap();
    LOG.debug("[%s] Saving LUID map", getConfig().getName());
    if (LUIDMap->close()) {
        LOG.error("Error saving LUID map file for source %s", getConfig().getName());
    }
    
    return ret;
}



SyncItem* MediaSyncSource::getFirstItem() 
{
    LOG.info("Smart slow starting for %s", getConfig().getName());
    
    smartSlowNewItemsDone     = false;
    smartSlowUpdatedItemsDone = false;
    smartSlowFirstItem        = true;
    
    SyncItem* item = getFirstNewItem();
    if (item != NULL) {
        return item;
    }
    
    // If here, the new items are finished: go with updated items.
    smartSlowNewItemsDone = true;
    return getNextItem();
}


SyncItem* MediaSyncSource::getNextItem() 
{
    SyncItem* item = NULL;
    
    //
    // New items
    //
    if (smartSlowNewItemsDone == false) {
        item = getNextNewItem();
        if (item != NULL) {
            return item;
        }
        smartSlowNewItemsDone = true;
    }
    
    //
    // Updated items
    //
    if (smartSlowUpdatedItemsDone == false) {
        if (smartSlowFirstItem) {
            item = getFirstUpdatedItem();
            smartSlowFirstItem = false;
        }
        else {
            item = getNextUpdatedItem();
        }
        if (item != NULL) {
            return item;
        }
        smartSlowUpdatedItemsDone = true;
        smartSlowFirstItem = true;
    }
    
    //
    // Deleted items (sent as empty update items)
    //
    if (smartSlowFirstItem) {
        item = getFirstDeletedItem();
        smartSlowFirstItem = false;
    }
    else {
        item = getNextDeletedItem();
    }
    if (item) {
        // *** necessary? ***
        LOG.debug("Sending deleted item as an empty updated item");
        item->setData(NULL, 0);
    }
    return item;

}




int MediaSyncSource::insertItem(SyncItem& item) 
{
    // Items are not added from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::insertItem() for item key = %s", key.c_str());
    
    return STC_COMMAND_NOT_ALLOWED;
}

int MediaSyncSource::modifyItem(SyncItem& item) 
{
    // Items are not modified from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::modifyItem() for item key = %s", key.c_str());
    
    return STC_COMMAND_NOT_ALLOWED;
}

int MediaSyncSource::removeItem(SyncItem& item) 
{
    // Items are not deleted from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::removeItem() for item key = %s", key.c_str());
    
    return STC_COMMAND_NOT_ALLOWED;
}


bool MediaSyncSource::filterOutgoingItem(const StringBuffer& fullName, struct stat& st)
{
    
    if (!S_ISDIR(st.st_mode)) 
    {
        // skip the '.dat' and '.jour' files (cache files)
        if (checkFileExtension(fullName, "dat")) {
            LOG.debug("skipping cache file '%s'", fullName.c_str());
            return true;
        }
        if (checkFileExtension(fullName, "jour")) {
            LOG.debug("skipping cache journal file '%s'", fullName.c_str());
            return true;
        }
        
        // TODO: filter media extensions? (jpg, wav, avi...)
    }

    return FileSyncSource::filterOutgoingItem(fullName, st);
}



StringBuffer MediaSyncSource::getItemSignature(StringBuffer& key) {

    if (key.length() <= 0) {
        return NULL;
    }
    
    LOG.debug("[%s] MediaSyncSource - getting signature for item with key %s", getConfig().getName(), key.c_str());       
    StringBuffer s;
    
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(key, &st) < 0) {
        LOG.error("can't stat file '%s' [%d]", key.c_str(), errno);
        return NULL;
    }
    
    //LOG.debug("signature is %d", st.st_mtime);
    s.sprintf("%d", st.st_mtime);
    return s;
}


bool MediaSyncSource::fillItemModifications()
{
    // Will fill new/mod/del item keys lists.
    bool ret = CacheSyncSource::fillItemModifications();
    
    
    // Remove the special props (url, username, swv) from the deleteKeys
    // (those are fake items, so they can't be found on filesystem)
    ArrayListEnumeration* keys = (ArrayListEnumeration*)deletedKeys;
    
    for (int i=0; i<keys->size(); ) {
        StringBuffer* key = (StringBuffer*)keys->get(i);
        
        if ( *key == CACHE_PROPERTY_URL ||
             *key == CACHE_PROPERTY_USERNAME ||
             *key == CACHE_PROPERTY_SWV ) {
            keys->removeElementAt(i);
            continue;
        }
        i++;
    }
        
    //
    // Fire the total number of items 
    //
    int count = 0; keys = NULL;

    keys = (ArrayListEnumeration*)newKeys;
    if (keys) {
        count += keys->size();
    }
    keys = (ArrayListEnumeration*)updatedKeys;
    if (keys) {
        count += keys->size();
    }
    keys = (ArrayListEnumeration*)deletedKeys;
    if (keys) {
        count += keys->size();
    }

    fireSyncSourceEvent(getConfig().getURI(), 
                        getConfig().getName(), 
                        getSyncMode(), count, 
                        SYNC_SOURCE_TOTAL_CLIENT_ITEMS);

    return ret;
}

void MediaSyncSource::fireClientTotalNumber(int number) {
    // void implementation...
}

SyncItem* MediaSyncSource::fillSyncItem(StringBuffer* key, const bool fillData)
{
    SyncItem* syncItem = FileSyncSource::fillSyncItem(key, fillData);
    
    // FIX the item's key.
    // Outgoing item: the key to send is the item's LUID.
    if (syncItem && key) {
        StringBuffer luid = getLUIDFromPath(*key);
        LOG.debug("MediaSyncSource::fillSyncItem - LUID of item '%s' is %s", key->c_str(), luid.c_str());
        WCHAR* wluid = toWideChar(luid.c_str());
        
        syncItem->setKey(wluid);
        if (wluid) { delete [] wluid; }
    }
    return syncItem;
}

void MediaSyncSource::getKeyAndSignature(SyncItem& item, KeyValuePair& kvp)
{
    // FIX the item's key.
    // Incoming item: the key for the cache is the full path.
    StringBuffer key;
    key.convert(item.getKey());
    
    StringBuffer sign = getItemSignature(key);
    StringBuffer path = getPathFromLUID(key);
    
    if (!path.null()) {
        kvp.setKey(path);
        kvp.setValue(sign);
    }
}
void MediaSyncSource::setItemStatus(const WCHAR* wkey, int status, const char* command) {
    
    StringBuffer key;
    key.convert(wkey);
    
    KeyValuePair vp;
    if (!isErrorCode(status)) {
        LOG.info("[%s], Received success status code from server for %s on item with key %s - code: %d", getConfig().getName(), command, key.c_str(), status);        
        StringBuffer path = getPathFromLUID(key);
        vp.setKey(path);                           // Cache's key is the path.
        if (strcmp(command, DEL)) {
            vp.setValue(getItemSignature(path));
        }
    } else {
        // 500 etc...
        LOG.info("[%s], Received failed status code from server for %s on item with key %s - code: %d", getConfig().getName(), command, key.c_str(), status);
            // error. it doesn't update the cache
    }    
    if (vp.getKey()) {
        updateInCache(vp, command);
    }    
}

StringBuffer MediaSyncSource::getLUIDFromPath(const StringBuffer& path)
{
    StringBuffer luid = LUIDMap->readPropertyValue(path.c_str());
    
    if (luid.null()) {
        // No correspondence found: use a new LUID.
        int newLUID = params.getNextLUID();
        luid.sprintf("%d", newLUID);
        
        // Add the new entry <path,LUID> in the LUIDMap
        LUIDMap->setPropertyValue(path.c_str(), luid.c_str());
        LOG.debug("LUID not found for item '%s' -> using new LUID = %s", path.c_str(), luid.c_str());
        
        // Set and persist the nextLUID for next time
        newLUID ++;
        params.setNextLUID(newLUID);
        saveNextLUID(newLUID);
    }
    
    return luid;
}

StringBuffer MediaSyncSource::getPathFromLUID(const StringBuffer& luid)
{
    StringBuffer path(NULL);
    Enumeration& props = LUIDMap->getProperties();
    
    while (props.hasMoreElement()) {
        KeyValuePair* kvp = (KeyValuePair*)props.getNextElement();
        if (kvp->getValue() == luid) {
            path = kvp->getKey();
            break;
        }
    }
    
    if (path.null()) {
        // Not found... should not happen! 
        LOG.error("MediaSyncSource - path not found in LUIDMap for LUID '%s'!", luid.c_str());
    }
    return path;
}



bool MediaSyncSource::verifyNextLUIDValue()
{
    bool updated = false;
    Enumeration& props = LUIDMap->getProperties();

    while (props.hasMoreElement()) {
        
        KeyValuePair* kvp = (KeyValuePair*)props.getNextElement();
        int currentLUID = strtol(kvp->getValue().c_str(), NULL, 10);
        
        if (currentLUID >= params.getNextLUID()) {
            // NextLUID must be updated!
            params.setNextLUID(currentLUID + 1);
            updated = true;
        }
    }
    return updated;
}

bool MediaSyncSource::refreshLUIDMap()
{
    bool updated = false;
    Enumeration& props = LUIDMap->getProperties();

    while (props.hasMoreElement()) {
        
        KeyValuePair* kvp = (KeyValuePair*)props.getNextElement();
        const StringBuffer& path = kvp->getKey();
        
        if ( readCachePropertyValue(path.c_str()).null() ) {
            // Not found in cache -> remove it from LUIDMap
            LUIDMap->removeProperty(path.c_str());
            updated = true;
        }
        
    }
    return updated;
}


const int MediaSyncSource::readNextLUID()
{
    int ret = 0;
    StringBuffer value = configParams->readPropertyValue(PROPERTY_NEXT_LUID);
    if (!value.null()) {
        ret = strtol(value.c_str(), NULL, 10);
    }
    return ret;
}

void MediaSyncSource::saveNextLUID(const int nextLUID)
{
    StringBuffer value;
    value.sprintf("%d", nextLUID);
    configParams->setPropertyValue(PROPERTY_NEXT_LUID, value);
    configParams->close();
}
END_NAMESPACE
