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

#include "sapi/FileSapiSyncSource.h"

BEGIN_FUNAMBOL_NAMESPACE

//Returns the file name, given its full (absolute path) name.
static StringBuffer getFileNameFromThisPath(const StringBuffer& fullName) {
    
    StringBuffer fileName("");
    
    unsigned long pos = fullName.rfind("/");
    if (pos == StringBuffer::npos) {
        pos = fullName.rfind("\\");
        if (pos == StringBuffer::npos) {
            // fullName is already the file name
            return fullName;
        }
    }
    // Move to the first char of the filename
    pos += 1;
    
    fileName = fullName.substr(pos, fullName.length() - pos);
    return fileName;
}

static bool removeFile(const char* fullname) {
    char* p;
	int len;
    bool ret = false;
    
    // The path separator could be '/' or '\'
    p = strrchr((char*)fullname, '/');
    if (!p) {
        // try with '\'
        p = strrchr((char*)fullname, '\\');
    }

    if (!p) {
        // the file is in the current directory
        ret = removeFileInDir(".", fullname);                
    } else {
	    len = p-fullname;        
        StringBuffer dir(fullname, len);	 
	    p++; len=strlen(fullname)-len;
        StringBuffer filename(p, len);
        ret = removeFileInDir(dir, filename);
    }    	
    return ret;
}


FileSapiSyncSource::FileSapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& rep, size_t incomingFilterDate, size_t outgoingFilterDate, const char* storageLocation) 
                                    : SapiSyncSource(sc, rep, incomingFilterDate, outgoingFilterDate, storageLocation), 
                                      folderSize(0) {
}


FileSapiSyncSource::~FileSapiSyncSource() {
}


bool FileSapiSyncSource::populateAllItemInfoList(AbstractSyncConfig& mainConfig) {
    
    StringBuffer path = getFolderPath();
    if (path.null()) {
        return false;
    }
    
    if (createFolder(path)) {
        LOG.error("[%s] Error creating folder %s", __FUNCTION__, path.c_str());
        return false;
    }    
    
    bool ret = true;
    folderSize = 0;
    LOG.debug("Read all files from dir: %s", path.c_str());
    ArrayList files = readFilesInDirRecursive(path.c_str(), false);
    
    LOG.debug("Creating local allItemsInfo list...");
    for (int i=0; i<files.size(); i++) {

        // check user aborted
        if (i%50 == 0) {
            if (mainConfig.isToAbort()) {
                return false;
            }
        }

        StringBuffer* file = (StringBuffer*)files.get(i);
        StringBuffer fileName(getFileNameFromThisPath(*file));
        if (fileName == "." || fileName == "..") {
            continue;
        }
        SapiSyncItemInfo info;
        
        info.setLuid(file->c_str());
        struct stat st;        
        memset(&st, 0, sizeof(struct stat));            
        
        if (statFile(file->c_str(), &st) < 0) {
            LOG.error("[%s] can't stat file '%s' [%d]", __FUNCTION__, file->c_str(), errno);
            //ret = false;
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            continue;   // skip subfolders: TODO fix here for recursive reading
        }

        FILE* f = fileOpen(file->c_str(), "rb");
        if (!f) {
            LOG.error("[%s] can't open the file'%s'", __FUNCTION__, file->c_str());
            //ret = false;
            continue;
        }
                   
        // set size (it's mandatory)
        int fileSize = fgetsize(f);         
        fseek(f, 0, SEEK_SET); 
        info.setSize(fileSize); 
        fclose(f); 
        f = NULL;

        if (fileSize == 0) {
            LOG.debug("[%s] the fileSize is 0. Removed from the list to be sent '%s'", __FUNCTION__, file->c_str());            
            continue;
        }

        // update the amount of disk space used
        folderSize += fileSize;

        info.setName(fileName.c_str());
        
        unsigned long tstamp = getFileModTime(file->c_str());
        info.setModificationDate(tstamp);
    
        tstamp = (unsigned long)st.st_ctime;
        info.setCreationDate(tstamp);
        
        // set the mime/type based on the extension        
        StringBuffer mime("");        
        int pos = fileName.rfind(".");
        if (pos != StringBuffer::npos) {            
            StringBuffer extension = fileName.substr(pos);    
            mime = SapiContentType::getContentTypeByExtension(extension);            
        }
        info.setContentType(mime);

        if (!allItemInfo) {
            allItemInfo = new ArrayListEnumeration();
        }
        allItemInfo->add(info);
    }

    // sort by modification date
    sortByModificationTime(allItemInfo);

    LOG.debug("Total dir size = %li (dir: %s)", folderSize, path.c_str());
    return ret;
}


InputStream* FileSapiSyncSource::createInputStream(const char* luid) {

    FileInputStream* istream = NULL;
    
    if (fileExists(luid)) {
        istream = new FileInputStream(luid);
    }

    return istream;    
}

OutputStream* FileSapiSyncSource::createOutputStream(SapiSyncItemInfo& itemInfo) {

    StringBuffer guid = itemInfo.getGuid();
    
    if (guid.empty()) {
        LOG.error("[%s] the guid from the server is empty", __FUNCTION__);
        return NULL;
    }
    
    guid.append(".tmp");

    StringBuffer path = getFolderPath();
    if (path.null()) {
        return NULL;
    }

    StringBuffer tmpFile(path);
    tmpFile.append("/");
    tmpFile.append(guid);

    FileOutputStream* ostream = new FileOutputStream(tmpFile, true); // move the offset if the file already exists

    return ostream;

}

StringBuffer FileSapiSyncSource::insertItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) {

    if (syncItem == NULL) {
        LOG.error("[%s] Error storing the file of the NULL ostream", __FUNCTION__);
        *modificationDate = 0;
        *errCode = ESSSInvalidItem;
        return "";
    }

    StringBuffer path = getFolderPath();
    if (path.null()) {
        *modificationDate = 0;
        *errCode = ESSSInternalError;
        return NULL;
    }

    FileOutputStream* stream = (FileOutputStream*)syncItem->getStream();
    StringBuffer p = stream->getFilePath();
    stream->close();

    StringBuffer realFileName(path);
    realFileName.append("/");
    realFileName.append(syncItem->getSapiSyncItemInfo()->getName());

    //check if the file already exists with the same name. Otherwise append _i        
    if (fileExists(realFileName.c_str())) {
        LOG.info("The file already exists (%s). Create a new one", realFileName.c_str());
        for (int i = 0; i < 20; i++) {            
            StringBuffer tmp;
            StringBuffer extension;
            int pos = realFileName.rfind(".");
            if (pos != StringBuffer::npos) {
                tmp = realFileName.substr(0, pos);
                extension = realFileName.substr(pos);
                tmp.append(StringBuffer().sprintf("_%02i", i));
                tmp.append(extension);
                if (fileExists(tmp.c_str())) {
                    continue;
                } else {
                    realFileName = tmp;
                    LOG.info("New file name is: %s", realFileName.c_str());        
                    break;
                }
            }        
        }
    }    

    if (renameFile(p.c_str(), realFileName.c_str()) != 0) {
        LOG.error("[%s] cannot rename the file %s... keep the old one %s.", __FUNCTION__, p.c_str(), realFileName.c_str());
        realFileName = p;
    }

    // returned to caller: it's used to update the cache
    *modificationDate = getFileModTime(realFileName.c_str());

    // update the total storage size
    folderSize += syncItem->getSapiSyncItemInfo()->getSize();

    *errCode = ESSSNoErr;
    
    return realFileName;

}
   
StringBuffer FileSapiSyncSource::changeItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) {
    
    if (syncItem == NULL) {
        LOG.error("[%s] Error storing the file of the NULL ostream", __FUNCTION__);
        *modificationDate = 0;
        *errCode = ESSSInvalidItem;
        return "";
    }

    StringBuffer path = getFolderPath();
    if (path.null()) {
        LOG.error("[%s] the MediaHub path is null", __FUNCTION__);
        *modificationDate = 0;
        *errCode = ESSSInternalError;
        return "";
    }
    
    *errCode = ESSSNoErr;
    bool isRename               = syncItem->getSapiSyncItemInfo()->isRename();
    StringBuffer serverFileName = syncItem->getSapiSyncItemInfo()->getName();
    StringBuffer luid           = syncItem->getSapiSyncItemInfo()->getLuid();    

    StringBuffer realFileName(path);
    realFileName.append("/");
    realFileName.append(serverFileName);

    if (isRename) {
        if (renameFile(luid.c_str(), realFileName.c_str()) != 0) {
            LOG.error("[%s] cannot rename the file %s... keep the old one %s.", __FUNCTION__, luid.c_str(), realFileName.c_str());
            realFileName = luid;
            *errCode = ESSSPermissionDenied;
        } else {
            LOG.debug("[%s] renamed file from %s into %s", __FUNCTION__, luid.c_str(), realFileName.c_str());
        }
        *modificationDate = getFileModTime(realFileName.c_str());
        return realFileName;
    } 
    
    // not renaming: it is an update in which there is the new file that is a .tmp and the old one.

    FileOutputStream* stream = (FileOutputStream*)syncItem->getStream();
    StringBuffer p = stream->getFilePath();
    stream->close();
    
    if (luid.endsWith(serverFileName.c_str()) == false) {
        LOG.debug("%s the luid %s and the name from server %s are different", __FUNCTION__,
                                                        luid.c_str(), serverFileName.c_str());
    }       
    
    if (removeFile(luid.c_str())) {
        LOG.debug("[%s] file %s removed", __FUNCTION__, luid.c_str());
        
        if (renameFile(p.c_str(), realFileName.c_str()) != 0) {
            LOG.error("[%s] cannot rename the file %s... keep the old one %s.", __FUNCTION__, p.c_str(), realFileName.c_str());
            realFileName = p;
            *errCode = ESSSPermissionDenied;
        }
    } else {
        LOG.error("[%s] cannot delete the file %s. Fatal error", __FUNCTION__, luid.c_str());
        *modificationDate = 0;
        *errCode = ESSSPermissionDenied;
        return "";
    }

    // returned to caller: it's used to update the cache
    *modificationDate = getFileModTime(realFileName.c_str());

    // update the total storage size
    folderSize += syncItem->getSapiSyncItemInfo()->getSize();

    return realFileName;
}

int FileSapiSyncSource::removeItem(const StringBuffer& identifier) {
    
    int ret = -1;
    StringBuffer fullName(identifier);
    
    normalizeSlash(fullName);

    StringBuffer path = getFolderPath();
    if (path.null()) {
        return 2;
    }
    if (fullName.find(path) == StringBuffer::npos) {
        // id is a relative path
        fullName = path;
        fullName.append("/");
        fullName.append(identifier);
    }

    if (fileExists(fullName.c_str()) == false) {
        LOG.info("[%s] file %s not existing. Return as success", __FUNCTION__, identifier.c_str());  
        return 1;
    }

    int size = fgetsize(fullName.c_str());
    if (removeFile(fullName.c_str())) {
        LOG.debug("[%s] file %s removed successfully", __FUNCTION__, identifier.c_str());
        // update the total storage size
        folderSize -= size;
        ret = 0;
    } else {
        LOG.error("[%s] file %s NOT removed successfully", __FUNCTION__, identifier.c_str());
        ret = -2;
    }
    
    return ret;
}

StringBuffer FileSapiSyncSource::getFolderPath() {

    const char* pp = config.getProperty(PROPERTY_MEDIAHUB_PATH);
    if (pp == NULL) {
        LOG.error("[%s] folder path not defined in configuration", __FUNCTION__);
        return NULL;
    }

    StringBuffer path(pp);
    normalizeSlash(path);
    return path;
}

int FileSapiSyncSource::cleanTemporaryDownloadedItem(const StringBuffer& item) {
    StringBuffer tmpFileName(item);
    tmpFileName.append(".tmp");
    return removeItem(tmpFileName);
}

bool FileSapiSyncSource::isLocalStorageAvailable(unsigned long long size, int* errorCode) { 
    
    StringBuffer val = config.getProperty(PROPERTY_LOCAL_QUOTA_STORAGE);
    if (val.null() || val.length() == 0) {
        // storage limit disabled
        return true;
    }

    // get the local storage available
    StringBuffer path = getFolderPath();
    
    if (val.endsWith("%")) {
        // It's a percentage value [0-100]
        unsigned long long totalBytes = 0, freeBytes = 0;
        if (int res = getFreeDiskSpace(path.c_str(), &totalBytes, &freeBytes)) {
            LOG.info("[%s] can't get the free space on disk, items are rejected. Code %i", __FUNCTION__, res);
            *errorCode = res;
            return false;
        }

        int perc = 0;
        sscanf(val.c_str(), "%d%%", &perc);
        if (perc > 100) { perc = 100; }
        if (perc < 0)   { perc = 0;   }

        unsigned long long used = totalBytes - freeBytes + size;
        unsigned long long limit = totalBytes * perc/100;
        if (used >= limit) {
            LOG.error("Local storage full: used = %llu, limit = %llu [bytes] (%d%%)", used, limit, perc);
            return false;
        }
    }
    else {
        // It's an absolute value [MBytes]
        long limit = atol(val.c_str());
        unsigned long long used = folderSize + size;
        if (used >= limit*1000*1000) {
            LOG.error("Local storage full: used = %llu, limit = %ld [bytes]", used, limit*1000*1000);
            return false;
        }
    }

    return true;
}

SapiSyncItemInfo* FileSapiSyncSource::twinDetection(SapiSyncItemInfo& serverItemInfo, const char* array) {
    
    ArrayListEnumeration* list = allItemInfo;
    
    if (array && strcmp(array,"NEW") == 0) {
        list = newItemInfo;
    } 
    
    SapiSyncItemInfo* ret = NULL;
    if (list && list->size() > 0) {

        SapiSyncItemInfo* itemInfo;
        for (int i = 0; i < list->size(); i++) {
            itemInfo = (SapiSyncItemInfo*)list->get(i);
            if ((itemInfo->getSize() == serverItemInfo.getSize()) && 
                (itemInfo->getName() == serverItemInfo.getName()) ) {

                ret = (SapiSyncItemInfo*)itemInfo->clone();
                list->removeElementAt(i);
                break;
            }
        } 
    }
    return ret;
}

UploadSapiSyncItem* FileSapiSyncSource::getNextModItem(int* err) {
    
    LOG.debug("[%s] - get item number %i from updatedItemInfo", __FUNCTION__, updatedItemInfo ? updatedItemInfo->size() : 0);    
    UploadSapiSyncItem* item = getUploadItem(updatedItemInfo);
    *err = 0;
    return item;        
}

int FileSapiSyncSource::resolveConflicts(ArrayList* modServerItems, ArrayList* delServerItems, AbstractSyncConfig& config, time_t offsetTime)
{
    int modListItemsUpdated = 0,
        delListItemsUpdated = 0;
 
    if (modServerItems) {
        if ((modListItemsUpdated = updateItemsList(updatedItemInfo, modServerItems, config)) < 0) {
            return 1;
        }
    }
 
    if (delServerItems) {
        if ((delListItemsUpdated = updateItemsList(deletedItemInfo, delServerItems, config)) < 0) {
            return 1;
        }
    }
    
    if (modListItemsUpdated) {
        if (resolveConflictOnItemsList(updatedItemInfo, modServerItems, EKeepLastModified, config, offsetTime)) {
            return 1;
        }
 
        if (delListItemsUpdated) {
            if (resolveConflictOnItemsList(updatedItemInfo, delServerItems, EKeepFromFirstList, config, 0)) {
                return 1;
            }
        }
    }

    if (delListItemsUpdated) {
        if (resolveConflictOnItemsList(deletedItemInfo, delServerItems, ERemoveFromFirstFromCache, config, 0)) {
            return 1;
        }
        
        if (modListItemsUpdated) {
            if (resolveConflictOnItemsList(deletedItemInfo, modServerItems, EKeepFromSecondList, config, 0)) {
                return 1;
            }
        }
    }
    
    return 0;
/*
    int res = resolveConflictOnItemsList(clientItemsList, &modServerItems, EKeepLastModified);
    if (res) { goto finally; }
    res = resolveConflictOnItemsList(clientItemsList, &delServerItems, EKeepFromFirstList);
    if (res) { goto finally; }

    clientItemsList = source.getItemsList("DEL");
    LOG.debug("%s: checking conflicts for deleted items", __FUNCTION__);
    res = resolveConflictOnItemsList(clientItemsList, &modServerItems, EKeepFromSecondList);
    if (res) { goto finally; }
    res = resolveConflictOnItemsList(clientItemsList, &delServerItems, ERemoveFromFirstFromCache);
*/
}

int FileSapiSyncSource::resolveConflictOnItemsList(ArrayList* clientItemsList, ArrayList* serverItemsList, 
                        ESSSConflictResType conflictResType, AbstractSyncConfig& config, time_t offsetTime) {
    int i = 0;
    int clientItemsNum = 0, serverItemsNum = 0;

    if (!clientItemsList || !serverItemsList) {
        return 0;
    }

    // cycle over client items list  
    if ((clientItemsNum = clientItemsList->size()) > 0) {
        if ((serverItemsNum = serverItemsList->size()) > 0) {
            for (; i < clientItemsNum; i++) {
                SapiSyncItemInfo* clientItemInfo = dynamic_cast<SapiSyncItemInfo *>(clientItemsList->get(i));
                const char* clientItemGuid = NULL;
                int j = 0;
                
                if (clientItemInfo == NULL) {
                    LOG.error("%s: can't get client item info", __FUNCTION__);
                    continue;
                }
                
                if (((clientItemGuid = clientItemInfo->getGuid()) == NULL)) {
                    const char* clientItemLuid = clientItemInfo->getLuid();
                    
                    if ((clientItemLuid != NULL) && (strlen(clientItemLuid) != 0)) {
                        clientItemGuid = getGuidFromLuid(clientItemLuid);
                        
                        if ((clientItemGuid == NULL) || (strlen(clientItemGuid) == 0)) {
                            LOG.error("%s: can't get client item guid", __FUNCTION__);
                            continue;
                        }
                    } else {
                        LOG.error("%s: can't get client item luid", __FUNCTION__);
                        continue;
                    }
                }
                
                // cycle over server items list
                for (; j < serverItemsNum; j++) {
                    if (config.isToAbort()) { 
                        return 1; 
                    }
                    SapiSyncItemInfo* serverItemInfo = dynamic_cast<SapiSyncItemInfo *>(serverItemsList->get(j));
                    const char* serverItemGuid = NULL;
                
                    if (serverItemInfo == NULL) {
                        LOG.error("%s: can't get server item info", __FUNCTION__);
                        break;
                    }
                
                    if (((serverItemGuid = serverItemInfo->getGuid()) == NULL) ||
                            (strlen(serverItemGuid) == 0)) {
                        LOG.error("%s: can't get server item guid", __FUNCTION__);
                        break;
                    }
                
                    // check for conflicts: guid is the key for 
                    if (strcmp(serverItemGuid, clientItemGuid) == 0) {
                        if (conflictResType == EKeepLastModified) {
                            time_t clientItemModTime = clientItemInfo->getModificationDate();
                            time_t serverItemModTime = serverItemInfo->getModificationDate() + offsetTime;
                            
                            LOG.info("%s: found conflict on update for item (guid '%s') - checking for most recent update:", 
                                __FUNCTION__, clientItemGuid);
                            
                            // resolve conflict keeping the item most recently modified: 
                            // if the modification time is the same (really a corner case), 
                            // server wins
                            if (clientItemModTime < serverItemModTime) {
                                LOG.info("%s: conflict for item resolved keeping server update", __FUNCTION__);
                                clientItemsList->removeElementAt(i);
                                i--;
                                clientItemsNum--;
                                
                                break;
                            } else {
                                LOG.info("%s: conflict for item resolved keeping client update", __FUNCTION__);
                                serverItemsList->removeElementAt(j);
                                serverItemsNum--;
                            
                                break;
                            }
                        } else if (conflictResType == EKeepFromFirstList) {
                            LOG.info("%s: conflict for item resolved keeping client update", __FUNCTION__);
                            serverItemsList->removeElementAt(j);
                            serverItemsNum--;
                            
                            break;
                        } else if (conflictResType == EKeepFromSecondList) {
                            LOG.info("%s: conflict for item resolved keeping server update", __FUNCTION__);
                            clientItemsList->removeElementAt(i);
                            i--;
                            clientItemsNum--;
                                
                            break;
                        } else if (conflictResType == ERemoveFromFirstFromCache) {
                            StringBuffer clientItemLuid = clientItemInfo->getLuid();
                            
                            LOG.info("%s: conflict for item resolved keeping server update", __FUNCTION__);
                            if (clientItemLuid.empty() == false) {
                                getCache().removeProperty(clientItemLuid);
                                getMappings().removeProperty(clientItemGuid);
                                getServerDateMappings().removeProperty(clientItemGuid);
                            }
                            
                            clientItemsList->removeElementAt(i);
                            i--;
                            clientItemsNum--;
                            
                            serverItemsList->removeElementAt(j);
                            serverItemsNum--;
                            
                            break;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int FileSapiSyncSource::pruneModifiedItemsList(ArrayList* modServerItems) {

    if (!modServerItems || modServerItems->size() == 0) {
        return 0;
    }

    for (int i=0; i<modServerItems->size(); i++) 
    {
        SapiSyncItemInfo* serverItemInfo = dynamic_cast<SapiSyncItemInfo*>(modServerItems->get(i));
        if (serverItemInfo == NULL) {
            LOG.error("%s: can't get server item info from updated items list", __FUNCTION__);
            continue;
        }

        StringBuffer& guid = serverItemInfo->getGuid();
        time_t remoteDate  = serverItemInfo->getModificationDate();

        // Lookup into the serverDate Mappings!
        StringBuffer val = serverDateMappings->readPropertyValue(guid.c_str());
        if (val.empty()) {
            LOG.error("%s: internal error: date not found in serverDate mapping for guid = %s", __FUNCTION__, guid.c_str());
            continue;
        }
        time_t localDate = atoll(val.c_str());
        
        if (remoteDate == localDate) {
            // the item has the same las mod date
            LOG.debug("Skip server updated item '%s': it was not updated remotely", serverItemInfo->getName().c_str()); 
            modServerItems->removeElementAt(i);
            i--;
        }
    }

    return 0;
}

void FileSapiSyncSource::validateLocalLists() {
    //
    // Only NEW and MOD items are supported, client to server.
    //
    if (deletedItemInfo && deletedItemInfo->size() > 0) {
        LOG.info("Deletes from client to server are NOT supported (%d items skipped)", deletedItemInfo->size());
        deletedItemInfo->clear();
    }
}

void FileSapiSyncSource::validateRemoteLists(ArrayList* newServerItems, 
                                             ArrayList* modServerItems, 
                                             ArrayList* delServerItems) {
    //
    // nothing to do: NEW/MOD/DEL items are supported, server to client.
    //
}

END_FUNAMBOL_NAMESPACE

