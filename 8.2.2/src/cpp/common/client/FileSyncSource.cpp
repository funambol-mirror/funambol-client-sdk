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
#include "spds/SyncItemStatus.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "syncml/core/TagNames.h"
#include "base/util/ArrayListEnumeration.h"
#include "client/FileSyncItem.h"
#include "client/FileSyncSource.h"
#include "inputStream/FileInputStream.h"

BEGIN_NAMESPACE

#define OMA_MIME_TYPE "application/vnd.omads-file+xml"

//------------------------------------------------------------------------------ Static functions

static StringBuffer getCompleteName(const char *dir, const WCHAR *name) {
    
    StringBuffer fileName;
    fileName.convert(name);
    
    if (fileName.find(dir) == 0) {
        // Filename contains the path from the first char -> it's already the complete name
        return fileName;
    }
    else {
        StringBuffer pathName(dir);
        pathName += "/"; 
        pathName += fileName;
        return pathName;
    }
}


//Returns the relative path+name of a file under the 'dir' folder, given its full name.
StringBuffer getRelativeName(const StringBuffer& dir, const StringBuffer& fullName) {
    
    if (dir.empty()) {
        return fullName;
    }
    
    StringBuffer relativeName("");
    unsigned int start = dir.length() + 1;
    
    // Get the relative path (cuts the trailing 'dir')
    if (fullName.length() > start) {
        relativeName = fullName.substr(start, fullName.length() - start);
    }
    return relativeName;
}


/**
 * Adds a suffix like "_00", "_02",... to the file name.
 * It's used to generate a new file name, if another one already exists.
 * @param fileName the file name
 * @param num      the number to add in the suffix (i.e. 1 -> "_01" is appended)
 * @return         the new file name
 */
static StringBuffer addNumericSuffix(const StringBuffer& fileName, const int num) {
   
    StringBuffer suffix;
    suffix.sprintf("_%02d", num);   // "_01", "_02" ...

    StringBuffer ret(fileName);
    unsigned long pos = fileName.rfind(".");
    
    if (pos == StringBuffer::npos) {
        // file with no extension: append at the end of filename
        ret.append(suffix);
    }
    else {
        // file with extension: replace "." with "_0x."
        suffix.append(".");
        ret.replace(".", suffix, pos);
    }

    return ret;
}

/**
 * Compares two files. 
 * Returns true if the files have the same content, false if they are different.
 */
static bool compareFiles(const void* content1, const size_t size1, 
                         const void* content2, const size_t size2) {

    // First, check the size...
    if (size1 != size2) {
        return false;
    }

    StringBuffer crc1, crc2;
    crc1.sprintf("%ld", calculateCRC(content1, size1));
    crc2.sprintf("%ld", calculateCRC(content2, size2));

    return (crc1 == crc2)? true : false;
}


/**
 * Adds a new file (saves it into the filesystem).
 * The destination file cannot already exist, in this case the incoming file
 * will be renamed with a suffix until a valid name is found (append _01, _02, ...).
 * @param name [IN-OUT] the name of file - if changed, the new one is returned.
 */
static int addFile(WString& name, const char* dir, const char *content, size_t size) {

    if (name.empty() || !dir || !size) {
        return STC_COMMAND_FAILED;
    }
    
    int ret = STC_OK;
    StringBuffer originalName = getCompleteName(dir, name.c_str());

    //
    // Change the name of incoming file (append _01, _02, ...) until one available is found.
    //
    StringBuffer fullName(originalName);
    for (int i=1; fileExists(fullName.c_str()); i++) {
        LOG.debug("The file '%s' already exists locally", fullName.c_str());

        // Read the existing file, check if it's the same (CRC)
        char* existingContent = NULL;
        size_t existingSize;
        if (!readFile(fullName.c_str(), &existingContent, &existingSize, true)) {
            LOG.error("cannot read file: %s", fullName.c_str());
        }
        bool isSameFile = compareFiles(content, size, existingContent, existingSize);
        delete [] existingContent;

        if (isSameFile) {
            LOG.info("File not added: it already exists with the same name and content (%s)", fullName.c_str());
            ret = STC_ALREADY_EXISTS;
            break;
        }
        else {
            // append "_0i" and check again if exists...
            fullName = addNumericSuffix(originalName, i);
        }
    }

    // Set the new 'name': it's returned to the caller (it's the key!)
    if (fullName != originalName) {
        StringBuffer newName = getRelativeName(dir, fullName);
        LOG.info("Incoming file renamed into: '%s'", newName.c_str());
        name = newName;
    }

    // Save: only if the same file doesn't already exist.
    if (ret != STC_ALREADY_EXISTS) {
        if (!saveFile(fullName.c_str(), content, size, true)) {
            ret = STC_COMMAND_FAILED;
        }
    }
    return ret;
}

/**
 * Updates a file, replacing the existing one with the same name.
 * NOTE: no check is done to the existance of the file with that name (key),
 *       since we use the file name as the item's key (so the right mapping is kept).
 * @param name  the name of file to update
 */
static int updateFile(const WString& name, const char* dir, const char *content, size_t size) {

    if (name.empty() || !dir || !size) {
        return STC_COMMAND_FAILED;
    }

    // Check file existence
    //StringBuffer fullName = getCompleteName(dir, name.c_str());
    //if ( !fileExists(fullName.c_str()) ) {
    //    LOG.error("Could not update file '%s': file not found locally!", fullName.c_str());
    //    return STC_COMMAND_FAILED;
    //}

    if (!saveFile(getCompleteName(dir, name.c_str()), content, size, true)) {
        return STC_COMMAND_FAILED;
    }
    return STC_OK;
}


/**
 * Saves a file (OMA file data) into the filesystem.
 * The file name is specified in the file data object.
 * NOTE: The new file name is set and returned inside the file::name attribute.

 * @param dir        the destination directory to save the file
 * @param file       the FileData object containing the data & other file params
 * @return isUpdate  true if it's an update, false if it's an add.
 */
static int saveFileData(const char *dir, FileData& file, bool isUpdate) {

    int ret = STC_COMMAND_FAILED;
    WString name(file.getName());

    if (isUpdate) {
        ret = updateFile(name, dir, file.getBody(), file.getSize());
    }
    else {
        ret = addFile(name, dir, file.getBody(), file.getSize());
    }

    // Set the new item key (real file name, can be different)
    if (name != file.getName()) {
        file.setName(name.c_str());
    }
    return ret;
}

/**
 * Saves a file (raw) into the filesystem.
 * The file name will be the Item's key.
 * NOTE: The new file name is set and returned inside the item::key attribute.
 * 
 * @param dir        the destination directory to save the file
 * @param item       the SyncItem containing the data & dataSize
 * @return isUpdate  true if it's an update, false if it's an add.
 */
static int saveFileItem(const char* dir, SyncItem& item, bool isUpdate) {

    int ret = STC_COMMAND_FAILED;
    WString name(item.getKey());

    if (isUpdate) {
        ret = updateFile(name, dir, (const char*)item.getData(), item.getDataSize());
    }
    else {
        ret = addFile(name, dir, (const char*)item.getData(), item.getDataSize());
    }
    
    // Set the new item key (file name).
    if (name != item.getKey()) {
        item.setKey(name.c_str());
    }
    return ret;
}


/**
 * Removes all files from an arraylist of file names (absolute paths expected).
 * @return true if no error
 */
static bool removeAllFiles(ArrayList& files) {
    
    bool ret = true;
    
    for (int i=0; i<files.size(); i++) {
        StringBuffer* fullName = (StringBuffer*)files[i];
        if (fullName) {
            int pos = fullName->rfind("/");
            StringBuffer filePath = fullName->substr(0, pos);
            StringBuffer fileName = fullName->substr(pos+1, fullName->length() - pos);
            //LOG.debug("fullname = %s, path = %s, name = %s", fullName->c_str(), filePath.c_str(), fileName.c_str());
            
            if ( removeFileInDir(filePath.c_str(), fileName.c_str()) == false ) {
                LOG.error("Error removing file: '%s'", fullName->c_str());
                ret = false;
            }
        }
    }
    return ret;
}



//---------------------------------------------------------------------------------- Constructors



FileSyncSource::FileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc, 
                               const StringBuffer& aDir, KeyValueStore* cache)
                              : CacheSyncSource(name, sc, cache), 
                              dir(aDir), 
                              recursive(false) {
    
    // Cut the last "\" or "/"
    if (aDir.endsWith("\\") || aDir.endsWith("/")) {
        dir = aDir.substr(0, aDir.length()-1);
    }            
}

FileSyncSource::~FileSyncSource() { }


Enumeration* FileSyncSource::getAllItemList() {
    
    ArrayList filesFound;
    Enumeration* allKeys = NULL;        
    
    if (scanFolder(dir, filesFound) == false) {
        LOG.error("error reading folder: %s", dir.c_str());
    }    
    
    LOG.info("The Client number of files read is %i", filesFound.size());
    allKeys = new ArrayListEnumeration(filesFound);
    return allKeys;
}

/**
 * Adds an item on the file system in the set directory.
 */
int FileSyncSource::insertItem(SyncItem& item) {
    
    int ret = STC_COMMAND_FAILED;
    FileData file;

    // Try the OMA file data first
    if (file.parse(item.getData(), item.getDataSize()) == 0) {
        if (file.getSize() >= 0) {  
            // Save file and set the LUID with the local name 
            ret = saveFileData(dir, file, false);
            item.setKey(file.getName());
        }        
    } else {
        // Save raw file (item's key is already set with the local name)
        ret = saveFileItem(dir, item, false);
    }

    if (isErrorCode(ret)) {
        report->setLastErrorCode(ERR_ITEM_ERROR);
        report->setLastErrorMsg(ERRMSG_ITEM_ERROR);
        report->setState(SOURCE_ERROR);
        LOG.debug("Error adding item: %" WCHAR_PRINTF, item.getKey());
    }
    return ret;
}

int FileSyncSource::modifyItem(SyncItem& item) {
    
    int ret = STC_COMMAND_FAILED;
    FileData file;
    
    if (file.parse(item.getData(), item.getDataSize()) == 0) {
        // Save OMA File data object.
        // The file name is the item's key, IF ITEM ALREADY MAPPED! (only updates / deletes)
        // So we set the right file name here.
        file.setName(item.getKey());
        if (file.getSize() >= 0) {
            ret = saveFileData(dir, file, true);
        }
    } else {
        // Save raw file
        ret = saveFileItem(dir, item, true);    
    }

    if (isErrorCode(ret)) {
        report->setLastErrorCode(ERR_ITEM_ERROR);
        report->setLastErrorMsg(ERRMSG_ITEM_ERROR);
        report->setState(SOURCE_ERROR);
        LOG.debug("Error updating item: %" WCHAR_PRINTF, item.getKey());
    }
    return ret;
}

int FileSyncSource::removeItem(SyncItem& item) {
    
    const char* filename = toMultibyte(item.getKey());
    removeFileInDir(dir, filename);
    delete [] filename;
    LOG.debug("Item deleted: %" WCHAR_PRINTF, item.getKey());
    
    return STC_OK;
}

int FileSyncSource::removeAllItems() {
    
    if (!recursive) {
        if (removeFileInDir(dir) == false) {
            return 1;
        }
    }
    else {
        ArrayList filesFound;
        if (scanFolder(dir, filesFound, false) == false) {
            return 1;
        }
        if (removeAllFiles(filesFound) == false) {
            return 1;
        }
    }
    
    return 0;
}


SyncItem* FileSyncSource::fillSyncItem(StringBuffer* key, const bool /*fillData*/) {
    
    SyncItem* syncItem = NULL;    
    
    if (!key) {
        return NULL;
    }
    WCHAR* wkey = toWideChar(key->c_str());
    StringBuffer completeName = getCompleteName(dir, wkey);

    //LOG.debug("[%s] Filling item with key %s", getConfig().getName(), key->c_str());
    LOG.debug("complete = %s", completeName.c_str());
    LOG.debug("name = %s", key->c_str());
    
    bool isFileData = false;
    if(!strcmp(config->getType(), OMA_MIME_TYPE)) {
        isFileData = true;
    }
    
    // Note: no data is set here! 
    // We use input stream.
    syncItem = new FileSyncItem(completeName, wkey, isFileData);
    
    delete [] wkey;
    return syncItem;

}


void* FileSyncSource::getItemContent(StringBuffer& key, size_t* size) {

    WString wkey;
    wkey = key;
    StringBuffer fullName = getCompleteName(dir.c_str(), wkey.c_str());

    FileInputStream stream(fullName);
    int fileSize = stream.getTotalSize();

    char* ret = new char[fileSize + 1];
    *size = stream.read(ret, fileSize);

    return ret;
}



// read recursively directory contents
bool FileSyncSource::scanFolder(const StringBuffer& fullPath, ArrayList& filesFound, bool applyFiltering)
{
    int count = 0;
    struct stat st;
    StringBuffer fullName;
    
    // Remove the trailing "/" or "\" if exists
    StringBuffer dirPath(fullPath);
    if (fullPath.endsWith("\\") || fullPath.endsWith("/")) {
        dirPath = fullPath.substr(0, fullPath.length()-1);
    }
    else if (fullPath.empty()) {
        dirPath = dir;
    }
    
    char** fileNames = readDir(dirPath.c_str(), &count, false);
    if (fileNames == NULL) {
        //LOG.debug("Directory '%s' has no items", dirPath.c_str());
        return true;
    }

    for (int i=0; i<count; i++) {
        if (fileNames[i]) {
            memset(&st, 0, sizeof(struct stat));
            
            fullName.sprintf("%s/%s", dirPath.c_str(), fileNames[i]);
            //LOG.debug("fullName = %s", fullName.c_str());
            
            if (stat(fullName, &st) < 0) {
                LOG.error("can't stat file '%s' [%d]", fullName.c_str(), errno);
                continue;
            }
            
            // Filtering on outgoing items
            if (applyFiltering) {
                if (filterOutgoingItem(fullName, st)) {
                    //LOG.debug("Skipping item '%s'", fullName.c_str());
                    continue;
                }
            }

            // Recurse into subfolders.
            if (recursive && S_ISDIR(st.st_mode)) {
                if (scanFolder(fullName, filesFound) == false) {
                    LOG.error("Error reading '%s' folder", fullName.c_str());
                }
            }
            else {
                //
                // It's a file -> add it (key is its full path + name)
                //
                filesFound.add(fullName);
            }
        }
    }
    
    // delete fileNames
    for (int i = 0; i < count; i++) {
        delete [] fileNames[i]; fileNames[i] = NULL; 
    }
    if (fileNames != NULL) {
        delete [] fileNames; fileNames = NULL;
    }
    
    return true;
}


bool FileSyncSource::filterOutgoingItem(const StringBuffer& fullName, struct stat& st) 
{
    // No filtering.
    return false; 
}


bool FileSyncSource::checkFileExtension(const StringBuffer& fileName, const StringBuffer& extension)
{
    unsigned long pos = fileName.rfind(".");
    
    if (pos == StringBuffer::npos) {
        return false;
    }
    if (pos < fileName.length()) {
        pos += 1;
        StringBuffer ext = fileName.substr(pos, fileName.length() - pos);
        if (ext == extension) {
            //LOG.debug("extension is '%s'", ext.c_str());
            return true;
        }
    }
    return false;
}



END_NAMESPACE
