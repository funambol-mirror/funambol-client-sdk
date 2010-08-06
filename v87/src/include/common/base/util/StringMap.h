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


#ifndef INCL_STRING_MAP
#define INCL_STRING_MAP

/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/util/KeyValuePair.h"

BEGIN_NAMESPACE

/**
* This container is an associative array of StringBuffers. It is accessed using 
* the key string, and canot contains duplicate keys.
*/
class StringMap : public ArrayElement {

private:

    ArrayList c;

    int findElement(const char *key);

public:

    /**
     * Add a new element to the StringMap, or modify an existent one
     * If either the key or the value is NULL, the method does nothing 
     * and return false.
     *
     * @param key the unique key of the map
     * @param val the value associated to key
     *
     * @return true if the element has been added,
     *         false if has been modified, or if a parameter was NULL.
     */
    bool put(const char *key, const char *val) ;

    /**
     * Remove an element from the StringMap, if exists
     *
     * @return true if the element has been succesfully removed.
     */
    bool remove(const char *key);

    /**
     * Retrieve an element from the StringMap, if exists.
     *
     * @return the value of the element with the given key,
     *         or the null() StringBuffer if not found.
     */
    const StringBuffer& get(const char *key);

    /// Same as method get.
    const StringBuffer& operator[](const char *key) {return get(key);} 

    /// Return the first pair, or KeyValuePair.null() if the StringMap is empty.
    const KeyValuePair& front();

    /// Return the next pair in the StringMap, or KeyValuePair.null() if the end is reached.
    const KeyValuePair& next();

    /// Return true if the StringMap is empty
    bool empty() const { return c.isEmpty(); }

    /// Clear the map.
    void clear() { c.clear(); }

    /// Implementation of ArrayElement.
    ArrayElement *clone() { return new StringMap(*this); }
};


END_NAMESPACE

/** @endcond */
#endif

