/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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

#include "client/ConfigSyncSource.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTree.h"
#include "base/util/ArrayListEnumeration.h"


USE_NAMESPACE

ConfigSyncSource::ConfigSyncSource(const WCHAR* name,
                                   const StringBuffer& applicationUri,
                                   AbstractSyncSourceConfig* sc,
                                   KeyValueStore* cache) :
                  CacheSyncSource(name, sc, cache),
                  applicationUri(applicationUri) {
}

ConfigSyncSource::~ConfigSyncSource() {  
}

void ConfigSyncSource::setMimeType(StringBuffer& t) {
    mimeType = t;
}

const StringBuffer& ConfigSyncSource::getMimeType() const {
    return mimeType;
}

void getPropertyVal(StringBuffer& keyString, StringBuffer& prop){
   if (keyString.find("./") == 0) {
        keyString = keyString.substr(2);
    }
    int lastslash = keyString.rfind("/");    
    if(lastslash >0 ){
        prop = keyString.substr(lastslash+1, (keyString.length() - lastslash) );
        keyString = keyString.substr(0, lastslash);
    }else{
        prop = keyString;
        keyString = "";
    }

}

void* ConfigSyncSource::getItemContent(StringBuffer& key, size_t* size) {

    StringBuffer* keysIter = (StringBuffer*)keysList.front();
    StringBuffer* itemsIter = (StringBuffer*)itemsList.front();
    while(keysIter) {
        if (*keysIter == key) {
            if(!itemsIter){
                *size = strlen("");
                return stringdup("");
            }
            *size = strlen(itemsIter->c_str());
            return stringdup(itemsIter->c_str());
        }
        keysIter  = (StringBuffer*)keysList.next();
        itemsIter = (StringBuffer*)itemsList.next();
    }
    *size = 0;
    return NULL;
}

char* ConfigSyncSource::readItemContent(const char* key, size_t* size) {
    // The key is the node name
    StringBuffer keyString(key);
    StringBuffer prop;
    getPropertyVal(keyString, prop);
    DMTree tree(applicationUri.c_str());
    ManagementNode* node = tree.getNode(keyString.c_str());
    if (getLastErrorCode() == ERR_DM_TREE_NOT_AVAILABLE) {
        // First time the node is not found, it's generated empty.
        resetError();
    }
    if (!node) {
        // There is something really wrong here...
        return NULL;
    }
    char* value = node->readPropertyValue(prop.c_str());
    LOG.debug("ConfigSyncSource: %s=%s", keyString.c_str(), value);
    delete node;
    *size = strlen(value);
    return value;
}

int ConfigSyncSource::removeAllItems() {
    
    for(int i = 0; i< keysList.size(); i++){	
        cleanItem(((StringBuffer*)keysList.get(i))->c_str());
    }
    return 0;
}

void ConfigSyncSource::cleanItem(const char* key) {

    StringBuffer keyString(key);
    StringBuffer prop;
    getPropertyVal(keyString, prop);

    DMTree tree(applicationUri.c_str());
    ManagementNode* node = tree.getNode(keyString.c_str());
    if (!node) {
        // There is something really wrong here...
        return;
    }
    node->setPropertyValue(prop, "");
    delete node;
}
    
Enumeration* ConfigSyncSource::getAllItemList() {

    itemsList.clear();
    size_t size;

    // Read all items and discard the empty ones. We should use a map here
    StringBuffer value;
	int keysListSize = keysList.size();
    for(int i = 0; i< keysListSize; i++){
		value = readItemContent(((StringBuffer*)keysList.get(i))->c_str(), &size);
        if (!value.empty()) {
            itemsList.add(value);
        }
	}

    // Collect all local items (their keys)
    ArrayListEnumeration* enumeration = new ArrayListEnumeration(keysList);
    return enumeration;
}
    
int ConfigSyncSource::insertItem(SyncItem& item) {
    // The key is the node name
    DMTree tree(applicationUri.c_str());
    StringBuffer key;
    key.convert(item.getKey());
    if (key.find("./") == 0) {
        key = key.substr(2);
    }
    
    int lastslash = key.rfind("/");
    StringBuffer prop;
    if(lastslash >0 ){
        prop = key.substr(lastslash+1, (key.length() - lastslash) );
        key = key.substr(0, lastslash);
    }else{
        prop = key;
        key = "";
    }

    ManagementNode* node = tree.getNode(key.c_str());
    if (getLastErrorCode() == ERR_DM_TREE_NOT_AVAILABLE) {
        // First time the node is not found, it's generated empty.
        resetError();
    }
    if (!node) {
        return STC_COMMAND_FAILED;
    }
    LOG.debug("ConfigSyncSource::insertItem: %s", (char*)item.getData());
    node->setPropertyValue(prop, (char*)item.getData());
    delete node;
    return STC_OK;
}

int ConfigSyncSource::modifyItem(SyncItem& item) {
    // Same behavior as for insertItem
    return insertItem(item);
}

int ConfigSyncSource::removeItem(SyncItem& item) {
    // This should never happen
    return STC_COMMAND_FAILED;
}


