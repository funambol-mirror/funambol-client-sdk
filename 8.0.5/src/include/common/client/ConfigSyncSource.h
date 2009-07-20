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
#ifndef INCL_CONFIG_SYNC_SOURCE
#define INCL_CONFIG_SYNC_SOURCE

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/util/utils.h"
#include "client/CacheSyncSource.h"

BEGIN_NAMESPACE

class ConfigSyncSource : public CacheSyncSource {

private:

    /// the mime type of the item
    StringBuffer mimeType;    
    StringBuffer applicationUri;
    
    /**
     * The arraylist where we store the values of the keys
     */
    ArrayList    itemsList;
    
    /**
     * The arraylist that contains all the properties to sync
     */
    ArrayList    keysList;
    
protected:

    ConfigSyncSource(SyncSource& s);

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     */
    ConfigSyncSource(const WCHAR* name, const StringBuffer& applicationUri,
                     AbstractSyncSourceConfig* sc, KeyValueStore* cache = NULL);

    virtual ~ConfigSyncSource();

    /**
     * setter for the list of properties to sync
     *
     * @param properties - the list of the properties
     *
     */
    void setConfigProperties(ArrayList properties){ keysList = properties ;};

    /**
    * set the mimetype
    */
    void setMimeType(StringBuffer& t);
    /**
    * get the mimetype
    */
    const StringBuffer& getMimeType() const;


    virtual void* getItemContent(StringBuffer& key, size_t* size);
    
            
    /**
    * Get an array list containing all the StringBuffer keys of all items. 
    * Used for the sync requiring and exchange of all items and
    * for the sync that need to calculate the modification.
    * It has to return a new allocated Enumeration that is
    * freed by the ConfigSyncSource
    */
    virtual Enumeration* getAllItemList();      
    
    /**
     * Called by the sync engine to add an item that the server has sent.
     * The sync source is expected to add it to its database, then set the
     * key to the local key assigned to the new item. Alternatively
     * the sync source can match the new item against one of the existing
     * items and return that key.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    virtual int insertItem(SyncItem& item);

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    virtual int modifyItem(SyncItem& item);

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     * sets to "" the value of the property
     *
     * @param item    the item as sent by the server
     */
    virtual int removeItem(SyncItem& item);
    /**
     * cleans all the items in the DMTree. 
     * sets to "" the value of the property
     */
    int removeAllItems();

private:

    /**
     * this method read the value in the properties in the config
     * @param key - the key of the property
     * @param size - the size of the property
     * @return char* the value of the property
     */
    char* readItemContent(const char* key, size_t* size);

    /**
     * this method cleans the value in the properties in the config
     * @param key - the key of the property
     */
    void cleanItem(const char* key);

};

END_NAMESPACE

#endif
