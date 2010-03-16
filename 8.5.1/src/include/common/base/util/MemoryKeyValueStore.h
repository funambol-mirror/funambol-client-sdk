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

#ifndef INCL_ARRAYLIST_KEY_VALUE_STORE
#define INCL_ARRAYLIST_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/util/KeyValuePair.h"
#include "base/util/KeyValueStore.h"
#include "base/util/ArrayListEnumeration.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This is the interface for the handling of the key/value that
 * has to be written in the storage. It provides methods that has to be 
 * specialized by implementation on filesystem, registry, db...
 */
class MemoryKeyValueStore : public KeyValueStore {

protected:

    /**
    * The list contains the pair key/value that are what is in the file that is 
    * in the form key:value
    */
    ArrayListEnumeration data; 

public:
    // Destructor
    virtual ~MemoryKeyValueStore() {}      
          
    /**
    *  Read a property value from the data ArrayList
    */
    virtual StringBuffer readPropertyValue(const char *prop) const;

    /*
     * Sets a property value in the data ArrayList
     *
     * @param prop      - the property name
     * @param value     - the property value (zero terminated string)
     */
    virtual int setPropertyValue(const char *prop, const char *value);
    
     /**
     * Remove a certain property from the data ArrayList
     *
     * @param prop    the name of the property which is to be removed
     * @return int 0 on success, an error code otherwise
     */
    virtual int removeProperty(const char *prop);
    
    virtual int removeAllProperties();

    /**
     * Read all the properties that are in the store. This is
     * an enumeration of KeyValuePairs. 
     * 
     */
    virtual Enumeration& getProperties()  {         
        data.resetIterator();
        return (Enumeration&)data; 
    }

    /**
     * Save the current properties that are
     * in the data arraylist 
     * @return 0 - success, failure otherwise
     */
    virtual int close() = 0;
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
