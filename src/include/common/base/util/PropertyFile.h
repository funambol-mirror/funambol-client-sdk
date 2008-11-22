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

#ifndef INCL_PROPERTY_FILE
#define INCL_PROPERTY_FILE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSourceReport.h"
#include "base/util/MemoryKeyValueStore.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This is the implementation of the keyValueStore on filesystem.
 * It provides methods to read and write in the filesystem the arraylist
 * of KeyValuePair
 */
class PropertyFile : public MemoryKeyValueStore {

private:
    
    /**
    * The name of the property file
    */
    StringBuffer node;

    /**
    * The name of the property file that is used as a journal. 
    * It contains all the entries that are set with setPropertyValue (they are appended into the file). 
    * They can be more than twice of the same key (two calls to setPropertyValue(prop1,val1); setPropertyValue(prop1,val111);)
    * The idea is that in memory there is the ArrayList of KeyValuePair with all the properties that
    * are with a unique key and this journal contains all the entries that can have more that one time
    * the same key. (in the case above, the arrayList contains only (prop1,val111)).
    * With the close() method the right property/value is written in the final storage and the journal is deleted.
    * If there is something wrong and the memory is erased before the close() is called, the next read() 
    * will find the journal file and populates properly the ArrayList in memory with the right property/value. 
    * Note that the ArrayList in memory is filled in the same order the properties are read from the journal file.
    */
    StringBuffer nodeJour;
    
     /**
     * Extract all currently properties in the node looking also at the journal file if exists.
     * It populates the data ArrayList to hold the key/values in the filesystem
     */
    int read();
        
public:
    
    /**      
     * The name of the general node 
     */
    PropertyFile(const char* n) : node(n) {
        nodeJour = node + ".jour";
        read();
    }

    // Destructor
    ~PropertyFile() {}               

    /**
     * Store the current properties that are
     * in the data arraylist in the filesystem. It deletes the journal too
     */
    int close();
    
    /**
    * The setPropertyValue of the PropertyFile calls the super
    * implementation and then write in the journal list with
    * a simple append.
    *
    * @param prop - the name of the prop
    * @param value - the value
    */
    int setPropertyValue(const char *prop, const char *value);
    
    /**
    * To remove the property, also the journal has to be updated. In the case, 
    * it is always updated with an append so at the next read it can be fixed
    */
    int removeProperty(const char* prop);

    /**
    * It remove all the properties in memory and in the storage
    */
    int removeAllProperties();
   
    /**
    * It sepatares from the line read from the property file the key and value.
    * It takes care 
    * 
    */
    bool separateKeyValue(StringBuffer& s, StringBuffer& key, StringBuffer& value);
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
