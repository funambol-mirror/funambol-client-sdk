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

#ifndef INCL_SYNC_SOURCE
#define INCL_SYNC_SOURCE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "filter/SourceFilter.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"
#include "spds/AbstractSyncSourceConfig.h"
#include "spds/SyncSourceReport.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This is the main API that a SyncML client developer needs to implement
 * to let the sync engine access the client's data. Each client may provide
 * access to one or more sources.
 */
class SyncSource {

private:
    SyncMode      syncMode;
    unsigned long lastSync;
    unsigned long nextSync;
    WCHAR*      name;

    char next[DIM_ANCHOR];
    char last[DIM_ANCHOR];

    SourceFilter* filter;

protected:
    AbstractSyncSourceConfig* config;
    SyncSourceReport* report;

    /**
     * copies all elements, to be used by derived class' clone() implementation
     */
    void assign(SyncSource& s);

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name   the name of the SyncSource
     * @param sc     configuration for the sync source: the instance
     *               must remain valid throughout the lifetime of the
     *               sync source because it keeps a reference to it
     *               and uses it as its own. A NULL pointer is allowed
     *               for unit testing outside of the sync framework;
     *               the sync source then references a global config
     *               instance to avoid crashes, but modifying that config
     *               will not make much sense. The pointer may also be
     *               set directly after creating the SyncSource, which
     *               is useful when a derived class creates the config
     *               in its own constructor.
     */
    SyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc);

    // Destructor
    virtual ~SyncSource();

    /**
     * Get the source name.
     *
     * @return - the source name (a pointer to the object buffer,
     *           will be released at object destruction)
     *
     */
    const WCHAR *getName();

    /**********************************************************
     * Most of the configurable properties are read
     * by the client library from the config (in
     * SyncClient::setDMConfig()) and then copied into the
     * sync source.
     *
     * These properties are stored in a local copy which will not be
     * written back into the permanent config, with a few exceptions:
     * properties related to mananging sync sessions like lastAnchor
     * are written back into the config by the library afer a
     * successful synchronization.
     *
     * A client developer is not required to modify these calls,
     * but he can use and/or update the properties before the
     * synchronization starts.
     *********************************************************/

    /**
     * use this directly after constructing the source when passing
     * the configuration to the constructor directly is not possible
     */
    void setConfig(AbstractSyncSourceConfig* sc);

    /** read-only access to configuration */
    const AbstractSyncSourceConfig& getConfig() const;
    /** read-write access to configuration */
    AbstractSyncSourceConfig& getConfig();

    

    /**
     * Return pointer to report object.
     */
    SyncSourceReport* getReport();

    /**
     * Set the report pointer with the given one
     * (no copy, only assign the pointer to the external one)
     *
     * @param sr   the report for this sync source
     */
    void setReport(SyncSourceReport* sr);

    /**
     * Get & Set the preferred synchronization mode for the SyncSource.
     *
     * Taken initially from the configuration by setConfig(), it can then
     * be modified by the client. The code synchronization code itself
     * reads this value, but it doesn't modify it.
     */
    SyncMode getPreferredSyncMode();
    void setPreferredSyncMode(SyncMode syncMode);

    /**
     * Get & Sets the server imposed synchronization mode for the SyncSource.
     *
     * Agreed upon with the server during the initial exchange with the server.
     * The SyncSource can react to it in beginSync(), in particular it must wipe
     * its local data during a refresh from server.
     */
    SyncMode getSyncMode();
    void setSyncMode(SyncMode syncMode);

    /**
     * Get & Set the timestamp in milliseconds of the last synchronization.
     * The reference time of the timestamp is platform specific.
     */
    unsigned long getLastSync();
    void setLastSync(unsigned long timestamp);

    /**
     * Gets & Sets the timestamp in milliseconds of the next synchronization.
     * The reference time of the timestamp is platform specific.
     */
    unsigned long getNextSync();
    void setNextSync(unsigned long timestamp);

    /**
     * Gets & Sets the last anchor associated to the source
     */
    void setLastAnchor(const char*  last);
    const char*  getLastAnchor();

    /**
     * Gets & Sets the next anchor associated to the source
     */
    const char*  getNextAnchor();
    void setNextAnchor(const char*  next);

    /**
     * Gets filter
     */
    SourceFilter* getFilter();

    /**
     * Sets filter
     *
     * @param f the new filter
     *
     */
    void setFilter(SourceFilter* f);

    /******************************************************
     * The following methods are virtual because a
     * derived SyncSource is expected to override or
     * implement them. Only the pure virtual methods
     * really have to be implemented, the others have
     * reasonable defaults.
     *****************************************************/

    /**
     * Called by the engine from inside SyncClient::sync()
     * at the begin of the sync.
     *
     * The SyncSource can do every initialization it needs.
     * The server has been contacted, so in particular
     * getSyncMode() can now be used to find out what
     * the sync mode for this synchronization run will be.
     * After this call the iterators for SyncItems must return
     * valid results for the current sync mode.
     *
     * The synchronization stops if this function return a non-zero value.
     *
     * @return - 0 on success, an error otherwise
     */
    virtual int beginSync();

    /**
     * Called by the engine from inside SyncClient::sync()
     * at the end of the sync.
     *
     * The SyncSource can do any needed commit action to save
     * the state of the items. The engine commits to the server
     * the changes applied in the transaction only if this function
     * return 0.
     *
     * FIXME: the return code is currently ignored by the sync engine
     *
     * @return - 0 on success, an error otherwise
     */
    virtual int endSync();

    /**
     * called by the sync engine with the status returned by the
     * server for a certain item that the client sent to the server
     *
     * @param key      the local key of the item
     * @param status   the SyncML status returned by the server
     * @deprecated
     * @since          SyncML API v7
     */
    virtual void setItemStatus(const WCHAR* /* key */, int /* status */) {}

    /**
     * called by the sync engine with the status returned by the
     * server for a certain item that the client sent to the server.
     * It contains also the proper command associated to the item.
     *
     * @param key      the local key of the item
     * @param status   the SyncML status returned by the server
     * @param command  the SyncML command associated to the item
     
     */
    virtual void setItemStatus(const WCHAR* key, int status,
                               const char* /* command */) {
        setItemStatus(key, status);
    }
    
    /**
    * Indicates that all the server status of the current package 
    * of the client items has been processed by the engine.
    * This signal can be useful to update the modification arrays
    */
    virtual void serverStatusPackageEnded();    
    
    /**
    * Indicates that all the client status of the current package 
    * of the server items that has been processed by the client and 
    * are going to be sent to the server.
    * This signal can be useful to update the modification arrays
    */
    virtual void clientStatusPackageEnded(); 
    
    /**
     * Removes all the item of the sync source. It is called 
     * by the engine in the case of a refresh from server to clean      
     * all the client items before receiving the server ones.
     * It is called after the beginSync() method.
     *
     *@return 0 if the remote succeded.
     */
    virtual int removeAllItems() = 0;

    /**
     * Return the first SyncItem of all.
     * It is used in case of slow sync
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getFirstItem() = 0;

    /**
     * Return the next SyncItem of all.
     * It is used in case of slow sync
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getNextItem() = 0;

    /**
     * Return the first SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstNewItem() = 0;

    /**
     * Return the next SyncItem of new one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getNextNewItem() = 0;

    /**
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstUpdatedItem() = 0;

    /**
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getNextUpdatedItem() = 0;

    /**
     * Return the first SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstDeletedItem() = 0;

    /**
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */
    virtual SyncItem* getNextDeletedItem() = 0;

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
    virtual int addItem(SyncItem& item) = 0;

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item.
     *
     * @param item    the item as sent by the server
     * @return SyncML status code
     */
    virtual int updateItem(SyncItem& item) = 0;

    /**
     * Called by the sync engine to update an item that the source already
     * should have. The item's key is the local key of that item, no data is
     * provided.
     *
     * @param item    the item as sent by the server
     */
    virtual int deleteItem(SyncItem& item) = 0;
 
               
    
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
