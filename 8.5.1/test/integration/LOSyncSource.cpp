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

#include "LOSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"
#include "testUtils.h"

USE_NAMESPACE

LOSyncSource::LOSyncSource(const WCHAR* name, SyncSourceConfig *sc) 
                        : SyncSource(name, sc), count(0), useSif(false), 
                        useSlowSync(false), useAdd(false), useUpdate(false),
                        useDataEncoding(false) {
                            
}

SyncItem* LOSyncSource::getFirstItem() {     
    return getNextItem();
}

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync
     * and retrieve the entire data source content.
     */
SyncItem* LOSyncSource::getNextItem() { 
    if (getUseSlowSync()) {
        setUseAdd(true);
        return getNextNewItem();
    } else {
        return NULL;
    }
}


SyncItem* LOSyncSource::getFirstNewItem() {
   return getNextNewItem();
}

SyncItem* LOSyncSource::getNextNewItem() {
    
    if (getUseAdd() == false) {
        return NULL;
    }
    if (count == 2) {
        return NULL;
    }
    
    StringBuffer name = getSyncItemName();
    WCHAR key[256];
    wsprintf(key, TEXT("%S"), name.c_str());
    wcscat(key, getName());    

    SyncItem* item = new SyncItem(key);    
    if (getUseDataEncoding()) {
        // just to test that the api works properly with custom encoding
        item->setDataEncoding("bin");
    }


    char* data = getNewCard(true);
    item->setData(data, (long)strlen(data));
    
    delete [] data;
    return item;

}


SyncItem* LOSyncSource::getFirstUpdatedItem() {
    return getNextUpdatedItem();
}

SyncItem* LOSyncSource::getNextUpdatedItem() { 
    
    if (getUseUpdate() == false) {
        return NULL;
    }
    if (count == 2) {
        return NULL;
    }
    
    StringBuffer name = getSyncItemName();
    WCHAR key[256];
    wsprintf(key, TEXT("%S"), name.c_str());
    wcscat(key, getName());    

    SyncItem* item = new SyncItem(key);
    
    char* data = getNewCard(false);
    item->setData(data, (long)strlen(data));
    
    delete [] data;
    return item;

}


void LOSyncSource::setItemStatus(const WCHAR* key, int status) {
   
}

int LOSyncSource::addItem(SyncItem& item) {

    WCHAR luid[128];
    wsprintf(luid, TEXT("%s-luid"), item.getKey());
    item.setKey(luid);
    return 200;
}


int LOSyncSource::beginSync() {
    LOG.debug("Begin sync MappingTestSyncSource");
    return 0;
}
int LOSyncSource::endSync() {
    LOG.debug("End sync MappingTestSyncSource");
    return 0;
}

StringBuffer LOSyncSource::getSyncItemName() {
    StringBuffer name;
    if (useSif) {        
        name.sprintf("sif%i.txt", count);
    } else {
        name.sprintf("vcard%i.txt", count);
    }
    return name;

}
char* LOSyncSource::getNewCard(bool isAdd) {
    
    int suffix = (isAdd ? count : count+2);
    StringBuffer filetest;
    if (useSif) {
        filetest.sprintf("sif%i.txt", suffix);
    } else {
        filetest.sprintf("vcard%i.txt", suffix);
    }
    char* card = loadTestFile("LOItemTest", filetest.c_str(), true);
    count++;    
    return card;

}
