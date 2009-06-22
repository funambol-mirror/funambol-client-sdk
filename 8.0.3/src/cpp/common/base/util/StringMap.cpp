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

/** @cond DEV */

#include "base/util/StringMap.h"

BEGIN_NAMESPACE

const static StringBuffer nullVal(NULL);

// Find the element in the StringMap with the given key.
int StringMap::findElement(const char *key) {

    KeyValuePair *e;
    int i=0;

    for (e=(KeyValuePair*)c.front(); e; e=(KeyValuePair*)c.next()) {
        if ( e->getKey() == key ) {
            return i;                   // item found
        }
        i++;
    }
    return -1;

}

// Add a new element to the StringMap, or modify an existent one
bool StringMap::put(const char *key, const char *val) {
    int index = findElement(key);
    if(index != -1) {
        ((KeyValuePair*)c[index])->setValue(val);
        return false;
    }
    else {
        KeyValuePair kv(key, val);
        c.add(kv);
        return true;
    }
}

// Remove an element from the StringMap, if exists
bool StringMap::remove(const char *key) {
    int index = findElement(key);
    if(index != -1) {
        if(c.removeElementAt(index) != -1){
            return true;        // Success
        }
    }
    return false;               // Not found or failure
}

// Retrieve an element from the StringMap, if exists.
const StringBuffer& StringMap::get(const char *key) {
    int index = findElement(key);
    if(index != -1) {
        return ((KeyValuePair*)c[index])->getValue();
    }
    else {
        return nullVal;
    }
}


END_NAMESPACE

/** @endcond */
