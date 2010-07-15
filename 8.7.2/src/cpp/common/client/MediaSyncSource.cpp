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
#include "inputStream/FileInputStream.h"
#include "spds/SyncReport.h"

BEGIN_NAMESPACE

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
    
    // Check the filters
    if (params.getFilterBySize()) {
        LOG.debug("MediaSyncSource: the size filtering is ON (max file size = %u KB)", params.getFilterBySize());
    } else {
        LOG.debug("MediaSyncSource: the size filtering is OFF");
    }
    if (params.getFilterByDate()) {
        filterDateString = unixTimeToString(params.getFilterByDate(), true);
        LOG.debug("MediaSyncSource: the date filtering is ON");
        LOG.info("Files modified before %s will not be sent", filterDateString.c_str());
    } else {
        filterDateString = "";
        LOG.debug("MediaSyncSource: the date filtering is OFF");
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
    
    // The list of LUIDs is created new for each sync session.
    LUIDsToSend.clear();
    
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
    // Save the LUIDMap file.
    // note: don't refresh the map! orphan LUIDs can be cleared only in endSync, they
    //       are used to send the same LUID in case of http upload errors.
    //
    if (LUIDMap->close()) {
        LOG.error("Error saving LUID map file for source %s", getConfig().getName());
    }
    
    return ret;
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
        LOG.error("can't stat file '%s' [%d]", key.c_str(), (int)errno);
        return NULL;
    }
    
    //LOG.debug("signature is %d", st.st_mtime);
    s.sprintf("%d", (int)st.st_mtime);
    return s;
}


SyncItem* MediaSyncSource::getFirstItem() {
    
    // A slow sync is started -> clear the cache
    clearCache();
    
    //
    // fill the ALL keys array
    //
    allKeys = getAllItemList();
    
    //
    // Dynamic filtering on ALL items keys (filter by size/date)
    //
    dynamicFilterItems(allKeys);

    
    // Fire the total number of client items
    ArrayListEnumeration* keys = (ArrayListEnumeration*)allKeys;
    if (keys) {
        fireSyncSourceEvent(getConfig().getURI(), getConfig().getName(), 
                            getSyncMode(), keys->size(), SYNC_SOURCE_TOTAL_CLIENT_ITEMS);
    }

    return getNextItem();
}


SyncItem* MediaSyncSource::getFirstNewItem() {    
    
    //
    // fill the NEW/MOD/DEL key arrays
    //
    fillItemModifications();   
    
    
    // Remove the special props (url, username, swv) from the deleteKeys
    // (those are fake items, so they can't be found on filesystem)
    ArrayListEnumeration* keys = (ArrayListEnumeration*)deletedKeys;
    for (int i=0; i<keys->size(); /*don't increment if item removed*/) {
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
    // Dynamic filtering on NEW/MOD/DEL items keys (filter by date)
    // (last time filtered items are always new, but items previously unfiltered
    // could be modified or deleted, so they need to be checked as well)
    //
    dynamicFilterItems(newKeys);
    dynamicFilterItems(updatedKeys);
    dynamicFilterItems(deletedKeys);


    // Fire the total number of items 
    int count = 0;
    keys = (ArrayListEnumeration*)newKeys;
    if (keys) count += keys->size();
    keys = (ArrayListEnumeration*)updatedKeys;
    if (keys) count += keys->size();
    keys = (ArrayListEnumeration*)deletedKeys;
    if (keys) count += keys->size();

    fireSyncSourceEvent(getConfig().getURI(), getConfig().getName(), 
                        getSyncMode(), count, SYNC_SOURCE_TOTAL_CLIENT_ITEMS);
    
    
    return getNextNewItem();    
}


void MediaSyncSource::dynamicFilterItems(Enumeration* itemKeys) {

    ArrayListEnumeration* keys = (ArrayListEnumeration*)itemKeys;
    if (!keys) return;
    
    //LOG.debug("before filtering: files to send = %d", keys->size());
    for (int i=0; i<keys->size(); /*don't increment if item removed*/) {
        StringBuffer* key = (StringBuffer*)keys->get(i);
        StringBuffer fullName = getCompleteName(dir, *key);     // key is already the full path... but to be sure :)
        
        if (dynamicFilterItem(fullName)) {
            keys->removeElementAt(i);
            continue;
        }
        i++;
    }
    //LOG.debug("after filtering: files to send = %d", keys->size());
}
    
bool MediaSyncSource::dynamicFilterItem(const StringBuffer& fileName) {
    
    //
    // Filter by size: exclude files with size > filterBySize (in KB)
    // If filterSize = 0, it means the filtering is off.
    //
    unsigned int filterSize = params.getFilterBySize();
    if (filterSize) {
        if (fileExists(fileName.c_str())) {
            size_t fileSize = fgetsize(fileName.c_str()) / 1024;
            if (fileSize > filterSize) {
                LOG.debug("item '%s' is not sent (exceeding maximum size: %u KB)", fileName.c_str(), fileSize);
                return true;
            }
        }
    }
    
    //
    // Filter by date: exclude files with modification date < filterDate.
    // If filterDate = 0, it means the filtering is off.
    //
    unsigned long filterDate = params.getFilterByDate();
    if (filterDate) {
        LOG.debug("MediaSyncSource: filtering date = %s", filterDateString.c_str());
        
        // Get the file modification time.
        unsigned long fileModTime = 0;
        if (fileExists(fileName.c_str())) {
            fileModTime = getFileModTime(fileName);
        }
        else {
            // This is a delete (the file does not exist)
            // Let's check the last modification time from the cache (it's our item's signature!)
            StringBuffer fileSignature = readCachePropertyValue(fileName.c_str());
            LOG.debug("File not found: get file's last modification time from cache: %s", fileSignature.c_str());
            if (!fileSignature.empty()) {
                fileModTime = strtol(fileSignature.c_str(), NULL, 10);
            }
        }
        
        // both are in UTC
        if (fileModTime && (fileModTime < filterDate)) {
            LOG.debug("item '%s' is not sent (modification time = %s)", fileName.c_str(), unixTimeToString(fileModTime, true).c_str());
            return true;
        }
    }
    
    return false;
}

void MediaSyncSource::fireClientTotalNumber(int number) {
    // void implementation (to avoid firing event from CacheSyncSource)
}

SyncItem* MediaSyncSource::fillSyncItem(StringBuffer* key, const bool fillData)
{
    if (report->getLastErrorCode() == ERR_SERVER_QUOTA_EXCEEDED) {
        LOG.debug("Stop sending new items: quota exceeded Server side");
        return NULL;
    }
    
    if (!key) { return NULL; }
    WCHAR* wkey = toWideChar(key->c_str());
    
    //
    // Create a new SyncItem with ONLY the METADATA of the fileDataObject (no body is included).
    //
    SyncItem* syncItem = new SyncItem(wkey);
    StringBuffer metadata = formatMetadata(wkey);
    syncItem->setData(metadata.c_str(), metadata.length());
    delete [] wkey;
    
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
    
    if (!isErrorCode(status)) {
        LOG.debug("[%s], Received success status code from server for %s on item with key %s - code: %d", getConfig().getName(), command, key.c_str(), status);        
        
        if (strcmp(command, DEL)) {
            // ADD, REPLACE: add this LUID to the array of LUIDs to upload via http.
            // Note: the cache is no longer updated here (it's done in endSync)
            LUIDsToSend.add(key);
        } 
        else {
            // DELETE: update directly the cache (no file content to send)
            KeyValuePair vp;
            StringBuffer path = getPathFromLUID(key);
            vp.setKey(path); 
            updateInCache(vp, DEL);
        }
    } 
    else if (status == STC_CHUNKED_ITEM_ACCEPTED) {
        // code 213 is not an error
        LOG.debug("[%s], Chunk accepted for %s on item with key %s - code: %d", getConfig().getName(), command, key.c_str(), status);  
    }
    else {
        // error status 500 etc...
        LOG.debug("[%s], Received failed status code from server for %s on item with key %s - code: %d", getConfig().getName(), command, key.c_str(), status);
        
        if (status == STC_DEVICE_FULL) {
            // status 420: Server quota exceeded -> avoid sending other items.
            if (report->getLastErrorCode() != ERR_SERVER_QUOTA_EXCEEDED) {
                setSourceError(ERR_SERVER_QUOTA_EXCEEDED);
                LOG.error("%s", report->getLastErrorMsg());
            }
        }
    }
}


void MediaSyncSource::setSourceError(const int errorCode) {

    StringBuffer msg;

    switch (errorCode) {

        case ERR_SERVER_QUOTA_EXCEEDED:
        {
            msg.sprintf("Cannot sync more %s: quota exceeded on the Server", getConfig().getName());

            // Add extended info about the quota on Server (OPTIONAL)
            int syncedItemsCount = report->getItemReportCount("Server", COMMAND_ADD);
            ItemReport* iReport = report->getItemReport("Server", COMMAND_ADD, syncedItemsCount - 1);
            if (iReport && iReport->getStatus() == STC_DEVICE_FULL) {
                StringBuffer quotaInfo;
                quotaInfo.convert(iReport->getStatusMessage());
                msg.append(" (");
                msg.append(quotaInfo);
                msg.append(" bytes)");
            }
            // NOTE: the source error is not set immediately (otherwise the sync would stop)
            // the source state will be set to ERROR in endSync().
            report->setLastErrorCode(errorCode);
            report->setLastErrorMsg(msg.c_str());
            return;
        }
        default:
            msg.sprintf("Error in %s source, code %d", getConfig().getName(), errorCode);
            break;
    }

    report->setState(SOURCE_ERROR);
    report->setLastErrorCode(errorCode);
    report->setLastErrorMsg(msg.c_str());
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


int MediaSyncSource::endSync() {

    LOG.info("[%s] %d items to upload via HTTP", config->getName(), LUIDsToSend.size());

    // Initialize the Http uploader module
    HttpUploader* httpUploader = getHttpUploader();
    httpUploader->setUsername (params.getUsername());
    httpUploader->setPassword (params.getPassword());
    httpUploader->setSyncUrl  (params.getUrl());
    httpUploader->setDeviceID (params.getDeviceID());
    httpUploader->setSourceURI(config->getURI());
    httpUploader->setUserAgent(params.getUserAgent());
    httpUploader->setUseSessionID(true);
    httpUploader->setKeepAlive(true);       // not applicable on some platforms
    
    //
    // Cycle all files to send via http
    //
    for (int i=0; i<LUIDsToSend.size(); i++) {
        
        StringBuffer* luid = (StringBuffer*)LUIDsToSend[i];
        StringBuffer path = getPathFromLUID(*luid);
        LOG.info("Uploading file via HTTP: \"%s\" (luid = %s)", path.c_str(), luid->c_str());
        
        // Fire the event: before uploading a file
        WString key;  
        key = *luid;
        fireSyncItemEvent(config->getURI(), config->getName(), key.c_str(), ITEM_UPLOADED_BY_CLIENT);
        
        // ---------------------
        // upload file via http
        // ---------------------
        FileInputStream* inputStream = new FileInputStream(path);
        int status = httpUploader->upload(*luid, inputStream);
        inputStream->close();


        // Update the report (after the upload)
        report->addItem(SERVER, HTTP_UPLOAD, key.c_str(), status, NULL);
        
        
        // Manage HTTP status returned
        if (status == HTTP_OK) {
            //
            // OK: update the cache and continue.
            //
            LOG.info("Media item uploaded successfully (LUID = %s)", luid->c_str());
            KeyValuePair vp;
            vp.setKey(path);
            vp.setValue(getItemSignature(path));
            updateInCache(vp);
        }
        else {
            //
            // Error: set the source error and decide if we need to continue.
            //
            setSourceError(status);
            LOG.error("Error uploading media item: %s", report->getLastErrorMsg());
            
            if (status == HttpConnection::StatusCancelledByUser) {      // user cancelled
                LOG.info("Sync cancelled by user");
                setSourceError(STC_OPERATION_CANCELLED_OK);
                break;
            }
            else if (status == ERR_CREDENTIAL ||                // HTTP basic auth failed
                     status == ERR_SERVER_QUOTA_EXCEEDED ||     // Quota exceeded: BTW should happen only during sync of metadata
                     status < 0) {                              // internal processing errors
                break;
            }
            else {
                continue;     // Other error status (like status 500) -> continue with next file
            }
        }

    }
    
    // Optimization to clear orphan LUIDs from LUIDMap: 
    // ONLY if ALL uploads were successfull.
    if (report->getLastErrorCode() == ERR_NONE) {
        refreshLUIDMap();
    }
    
    // All uploads done: save cache and LUIDMap.
    if (saveCache()) {
        LOG.error("Error saving cache file for source %s", getConfig().getName());
        setSourceError(ERR_FILE_WRITE);
    }
    
    // In case of quota exceeded, the source error is not set immediately (otherwise the sync would stop).
    // We can safely set the source error now.
    if (report->getLastErrorCode() == ERR_SERVER_QUOTA_EXCEEDED) {
        report->setState(SOURCE_ERROR);
    }

    delete httpUploader;
    return report->getLastErrorCode();
}


bool MediaSyncSource::isErrorCode(int code) {
    
    if ( code >= STC_OK && code < STC_MULTIPLE_CHOICES && 
         code != STC_CHUNKED_ITEM_ACCEPTED &&
         code != STC_PARTIAL_CONTENT       && 
         code != STC_RESET_CONTENT         && 
         code != STC_NO_CONTENT ) {
        return false;
    } else {
        return true;
    }
}


END_NAMESPACE
