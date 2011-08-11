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

#ifndef INCL_MAPPINGS_MANAGER
#define INCL_MAPPINGS_MANAGER
/** @cond DEV */

#include "spds/MappingStoreBuilder.h"
#include "base/util/PropertyFile.h"
#include "base/util/utils.h"

BEGIN_NAMESPACE

class MappingsManager {
   
    private:
        
        /**
        * Pointer to a KeyValueStore implementation that can be set by the client
        * to use its own KeyValueStore 
        */
        KeyValueStore* store;
        
        /**
        * The MappingStoreBuilder used to create the KeyValueStore
        */
        static MappingStoreBuilder* builder; 

    public:
        
        /**
        * The constructor initialize the proper MappingManager with a name of the
        * source. This can be used as a filename for a KeyValueStore that is on 
        * file system, or as a table name for a KeyValueStore on a db...
        *
        * @param sourceName - the name of the source the MappingsManager is created for
        */
        MappingsManager(const char* sourceName) {
            store = MappingsManager::getMappingStore(sourceName);
        }
        
        /**
        * Delete the instantce of the KeyValueStore. The client can create but the 
        * MappingsManager is responsible to delete it.
        */
        ~MappingsManager() {
            delete store;
        }

        /**
        * Stores the pair LUID/GUID in the storage immediately. These are the
        * values that will be used at the next sync if something
        * goes wrong. 
        *
        * @param LUID - the Local UID of the item (client side)
        * @param GUID - the Global UID of the item (server side)
        *
        * @return true if all is OK, false otherwise
        */
        bool addMapping(const char* LUID, const char* GUID) {
            return store->setPropertyValue(LUID, GUID) == 0 ? true : false;
            
        }

        /**
        * Return the enumeration of the mappings 
        *
        * @return a reference to the stored mappings
        */
        Enumeration& getMappings() {
            return store->getProperties();
        }
        
        /**
        * It erases the mappings data from the storage
        *
        * @returns true if ok, false otherwise
        */
        bool resetMappings() {  
            return store->removeAllProperties() == 0 ? true : false;                        
        }

        /**
        * It persists properly the mappings element. They could be stored in such a 
        * temporary location and the method could be implemented to persistently in
        * a definitive storage. Some store implementation could have this empty
        *
        * @returns true if ok, false otherwise
        */
        bool closeMappings() {            
            return store->close() == 0 ? true : false;
        }
        
        /**
        * Method to allow the client to set its own MappingStoreBuilder
        *
        * @param b - the client builder implementation
        */
        static void setBuilder(MappingStoreBuilder* b) {
            delete builder;
            builder = b;
        }

        /**
        * Used to retrieve the KeyValueStore to be used to store the mappings
        * for the source.
        *
        * @param sourceName - the name of the source
        */
        static KeyValueStore* getMappingStore(const char* sourceName) {
            if (builder == NULL) {
                builder = new MappingStoreBuilder();
            }
            return builder->createNewInstance(sourceName);
        }
} ;

END_NAMESPACE

/** @endcond */
#endif

