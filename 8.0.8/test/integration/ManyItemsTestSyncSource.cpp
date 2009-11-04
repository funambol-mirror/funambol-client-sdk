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

#include "ManyItemsTestSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"


USE_NAMESPACE




ManyItemsTestSyncSource::ManyItemsTestSyncSource(const WCHAR* name, SyncSourceConfig *sc, int numItems) 
                                                      : SyncSource(name, sc), count(0), numNewItems(numItems) {

}


SyncItem* ManyItemsTestSyncSource::getNextNewItem() {

    if (numNewItems == 0) {
        return NULL;
    }
    if (count == numNewItems) {
        return NULL;
    }

    // Set the new item's key.
    WCHAR key[256];
    wsprintf(key, TEXT("%i"), count);
    wcscat(key, getName());
    SyncItem* item = new SyncItem(key);

    // Generate item's data.
    StringBuffer data;
    if (!strcmp(config->getName(), "contact")) {
        data = getNewCard();
    }
    else if (!strcmp(config->getName(), "calendar")) {
        data = getNewCal();
    }

    item->setData(data.c_str(), data.length());
    
    count ++;
    return item;
}

void ManyItemsTestSyncSource::setItemStatus(const WCHAR* key, int status) {
    LOG.debug("ManyItemsTestSyncSource - key: %ls, status: %i", key, status);
}

int ManyItemsTestSyncSource::addItem(SyncItem& item) {

    WCHAR luid[128];
    wsprintf(luid, TEXT("%s-luid"), item.getKey());
    item.setKey(luid);
    return STC_ITEM_ADDED;
}


int ManyItemsTestSyncSource::beginSync() {
    LOG.debug("Begin sync ManyItemsTestSyncSource");
    return 0;
}
int ManyItemsTestSyncSource::endSync() {
    LOG.debug("End sync ManyItemsTestSyncSource");
    return 0;
}



StringBuffer ManyItemsTestSyncSource::getNewCard() {
    
    StringBuffer name(toMultibyte(getName()));
    name.append(count);    

    StringBuffer card = "BEGIN:VCARD\n"
                        "VERSION:2.1\n"
                        "TITLE:tester\n";
    card +=             "FN:John "; card += name; card += "\n";
    card +=             "N:"; card += name;
    card +=             ";John;;;\n"
                        "TEL;TYPE=WORK;TYPE=VOICE:11223344\n"                        
                        "NOTE:\n"
                        "END:VCARD\n";

    return card;
}

StringBuffer ManyItemsTestSyncSource::getNewCal() {
    
    StringBuffer name(toMultibyte(getName()));
    name.append(count);

    StringBuffer day;
    day.sprintf("%02d", (count%30) + 1);

    StringBuffer cal = "BEGIN:VCALENDAR\n"
                       "VERSION:1.0\n"
                       "BEGIN:VEVENT\n";
    cal +=             "SUMMARY:"; cal += name; cal += "\n";
    cal +=             "DTSTART:200907"; cal+= day; cal+="T170000Z\n";
    cal +=             "DTEND:200907";   cal+= day; cal+="T180000Z\n";
    cal +=             "DESCRIPTION:body-1234567890\n";
    cal +=             "END:VEVENT\n"
                       "END:VCALENDAR\n";

    return cal;
}
