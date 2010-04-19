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

#include "MappingTestSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"


USE_NAMESPACE

int SyncItemListenerClient::itemcounter = 0;

void SyncItemListenerClient::itemAddedByClient(SyncItemEvent &event) { }

void SyncItemListenerClient::itemAddedByServer(SyncItemEvent &event)
{
    itemcounter++;
    if (itemcounter == 3) {
        throw MappingException();
    }
}

void SyncItemListenerClient::itemDeletedByClient(SyncItemEvent &event) { }
void SyncItemListenerClient::itemDeletedByServer(SyncItemEvent &event) { }
void SyncItemListenerClient::itemUpdatedByClient(SyncItemEvent &event) { }
void SyncItemListenerClient::itemUpdatedByServer(SyncItemEvent &event) { }


MappingTestSyncSource::MappingTestSyncSource(const WCHAR* name, SyncSourceConfig *sc) 
                        : SyncSource(name, sc), count(0) {
                            
}

SyncItem* MappingTestSyncSource::getFirstNewItem() {
   return getNextNewItem();
}

SyncItem* MappingTestSyncSource::getNextNewItem() {

    if (count == 3) {
        return NULL;
    }
    
    WCHAR key[256];
    wsprintf(key, TEXT("%i-%lu"), count, this->getConfig().getLast());
    wcscat(key, getName());    

    SyncItem* item = new SyncItem(key);
    
    StringBuffer data = getNewCard();
    item->setData(data, data.length());
    
    return item;

}

void MappingTestSyncSource::setItemStatus(const WCHAR* key, int status) {
   
}

int MappingTestSyncSource::addItem(SyncItem& item) {

    WCHAR luid[128];
    wsprintf(luid, TEXT("%s-luid"), item.getKey());
    item.setKey(luid);
    return 200;
}


int MappingTestSyncSource::beginSync() {
    LOG.debug("Begin sync MappingTestSyncSource");
    return 0;
}
int MappingTestSyncSource::endSync() {
    LOG.debug("End sync MappingTestSyncSource");
    return 0;
}

StringBuffer MappingTestSyncSource::getNewCard() {
    
    StringBuffer name(toMultibyte(getName()));
    name.append(this->getConfig().getLast());    

    StringBuffer card = "BEGIN:VCARD\n"
                        "VERSION:3.0\n"
                        "TITLE:tester\n";

    card += "FN:John "; card += name; card += "\n";
    card +=             "N:"; card += name;
    card +=             ";John;;;\n"
                        "TEL;TYPE=WORK;TYPE=VOICE:11223344\n"                        
                        "NOTE:\n"
                        "END:VCARD\n";
    
    // for the next time...
    count++;

    return card;

}
