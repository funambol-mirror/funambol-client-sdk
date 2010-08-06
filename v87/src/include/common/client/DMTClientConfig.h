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
#ifndef INCL_DM_CONFIG
#define INCL_DM_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "http/constants.h"
#include "spdm/constants.h"
#include "spds/SyncManagerConfig.h"
#include "spds/AccessConfig.h"
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"
#include "spdm/ManagementNode.h"
#include "base/globalsdef.h"
#include "syncml/core/DataStore.h"

BEGIN_NAMESPACE

class DMTree;

/**
 * This class is an extension of SyncManagerConfig that is DM tree aware; this
 * means that configuration properties are read/stored from/to the DM tree.
 */
class DMTClientConfig : public SyncManagerConfig {

    protected:

        char*  rootContext;

        DMTree* dmt;
        ManagementNode* syncMLNode;
        ManagementNode* sourcesNode;
        ManagementNode* serverNode;

        void initialize();

        /* top level functions */
        virtual bool readRootConfig  ();
        virtual void saveRootConfig  ();
        virtual bool readAccessConfig(ManagementNode& n);
        virtual void saveAccessConfig(ManagementNode& n);
        virtual bool readDeviceConfig(ManagementNode& n, bool server = false);
        virtual void saveDeviceConfig(ManagementNode& n, bool server = false);
        virtual bool readSourceConfig(int i, ManagementNode& n);
        virtual void saveSourceConfig(int i, ManagementNode& n);

        /**
         * Called by readAccessConfig() to save authentication
         * settings.  The purpose of making this function virtual is
         * that a derived class can override it and then to read the
         * settings from a different than the default
         * "spds/syncml/auth" node by calling the base function with a
         * different \a authNode parameter or generate the settings in
         * some other way.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param authNode       the "spds/syncml/auth" node
         */
        virtual bool readAuthConfig(ManagementNode& syncMLNode,
                                    ManagementNode& authNode);
        /**
         * Same as readAccessConfig() for saving the settings.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param authNode       the "spds/syncml/auth" node
         */
        virtual void saveAuthConfig(ManagementNode& syncMLNode,
                                    ManagementNode& authNode);

        /**
         * Same as readAccessConfig() for reading connection
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param connNode       the "spds/syncml/conn" node
         */
        virtual bool readConnConfig(ManagementNode& syncMLNode,
                                    ManagementNode& connNode);
        /**
         * Same as readAccessConfig() for saving connection
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param connNode       the "spds/syncml/conn" node
         */
        virtual void saveConnConfig(ManagementNode& syncMLNode,
                                    ManagementNode& connNode);

        /**
         * Same as readAccessConfig() for reading additional access
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual bool readExtAccessConfig(ManagementNode& syncMLNode,
                                         ManagementNode& extNode);
        /**
         * Same as readAccessConfig() for saving additional access
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual void saveExtAccessConfig(ManagementNode& syncMLNode,
                                         ManagementNode& extNode);

        /**
         * Same as readAccessConfig() for reading device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devInfoNode    the "spds/syncml/devinfo" node
         */
        virtual bool readDevInfoConfig(ManagementNode& syncMLNode,
                                       ManagementNode& devInfoNode, bool server = false);
        /**
         * Same as readAccessConfig() for saving device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devInfoNode    the "spds/syncml/devinfo" node
         */
        virtual void saveDevInfoConfig(ManagementNode& syncMLNode,
                                       ManagementNode& devInfoNode, bool server = false);

        /**
         * Same as readAccessConfig() for reading additional device
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devDetailNode  the "spds/syncml/devdetail" node
         */
        virtual bool readDevDetailConfig(ManagementNode& syncMLNode,
                                         ManagementNode& devDetailNode, bool server = false);
        /**
         * Same as readAccessConfig() for saving additional device
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devDetailNode  the "spds/syncml/devdetail" node
         */
        virtual void saveDevDetailConfig(ManagementNode& syncMLNode,
                                         ManagementNode& devDetailNode, bool server = false);

        /**
         * Same as readAccessConfig() for reading some more additional
         * device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual bool readExtDevConfig(ManagementNode& syncMLNode,
                                      ManagementNode& extNode, bool server = false);
        /**
         * Same as readAccessConfig() for saving some more additional
         * device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual void saveExtDevConfig(ManagementNode& syncMLNode,
                                      ManagementNode& extNode, bool server = false);

        /**
         * Same as readAccessConfig() for reading variables that the
         * library uses internally, like anchors.
         *
         * @param i              index of the source
         * @param sourcesNode    the "spds/sources" node
         * @param sourceNode     the "spds/sources/<source name>" node
         */
        virtual bool readSourceVars(int i,
                                    ManagementNode& sourcesNode,
                                    ManagementNode& sourceNode);

        /**
         * Same as readAccessConfig() for saveing variables that the
         * library uses internally, like anchors.
         *
         * @param i              index of the source
         * @param sourcesNode    the "spds/sources" node
         * @param sourceNode     the "spds/sources/<source name>" node
         */
        virtual void saveSourceVars(int i,
                                    ManagementNode& sourcesNode,
                                    ManagementNode& sourceNode);

        /**
         * Same as readAccessConfig() for reading the normal
         * properties of a sync source, i.e. excluding variables like
         * anchors.
         *
         * @param i              index of the source
         * @param sourcesNode    the "spds/sources" node
         * @param sourceNode     the "spds/sources/<source name>" node
         */
        virtual bool readSourceConfig(int i,
                                      ManagementNode& sourcesNode,
                                      ManagementNode& sourceNode);

        /**
         * Same as readAccessConfig() for reading the normal
         * properties of a sync source, i.e. excluding variables like
         * anchors.
         *
         * @param i              index of the source
         * @param sourcesNode    the "spds/sources" node
         * @param sourceNode     the "spds/sources/<source name>" node
         */
        virtual void saveSourceConfig(int i,
                                      ManagementNode& sourcesNode,
                                      ManagementNode& sourceNode);

        /**
         * Reads the dataStores tree and sets values in serverConfig::dataStores
         * arrayList of DataStores. Only for Server config.
         *
         * @param dataStoresNode  the "server/DataStores" node to read from
         * @return                true if operation succeded
         */
        bool readDataStoresConfig(ManagementNode& dataStoresNode);

        /**
         * Reads a single dataStore and returns a DataStore object populated.
         * Only for Server config.
         *
         * @param dataStoreNode  the "server/DataStores/<datastore-name>" node to read from
         * @return               a new allocated DataStore
         */
        DataStore* readDataStoreConfig(ManagementNode& dataStoreNode);

        /**
         * Saves the dataStores tree from serverConfig::dataStores arrayList 
         * into the passed config node. Only for Server config.
         * Cleans up the existing nodes, then calls saveDataStoreConfig() for
         * each datastore in the array.
         *
         * @param dataStoresNode  the "server/DataStores" node to write into
         */
        void saveDataStoresConfig(ManagementNode& dataStoresNode);

        /**
         * Saves a single dataStore into the passed config node. 
         * Only for Server config.
         *
         * @param dataStoreNode   the "server/DataStores/<datastore-name>" node to write into
         * @param dataStore       the DataStore object with params to read
         */
        void saveDataStoreConfig(ManagementNode& dataStoreNode, DataStore* dataStore);

    public:
        
        /**
         * Initialize a DMTClientConfig with the given application context.
         * It's deprecated, use PlatformAdapter::init(appContext) and 
         * DMTClientConfig() instead.
         *
         * @param root the application context
         */
        DMTClientConfig(const char* root);
        
        /**
         * Initialize a DMTClientConfig. 
         * You must call PlatformAdapter::init(appContext) before.
         */
        DMTClientConfig();
    
        ~DMTClientConfig();

        SyncSourceConfig* getSyncSourceConfig(const char* name, bool refresh = false);
        SyncSourceConfig* getSyncSourceConfig(unsigned int i,   bool refresh = false);

        virtual bool read();
        virtual bool save();

        /**
         * Opens the configuration backend associated with the root context.
         * Calling on an open config does nothing.
         *
         * @return true for success
         */
        virtual bool open();

        /**
         * Provides access to the "syncml" configuration node,
         * can be used to read/write custom configuration options.
         * Config must have been opened before.
         *
         * @return node pointer owned by config and valid while the config is open
         */
        virtual ManagementNode* getSyncMLNode();

        /**
         * Gets number of sync source configurations, -1 if not open.
         */
        virtual int getNumSources();

        /**
         * Get the specified sync source configuration.
         *
         * @param index    number of the requested sync source configuration
         * @return node pointer owned by config and valid while the config is open
         */
        virtual ManagementNode* getSyncSourceNode(int index);

        /**
         * Get the specified sync source configuration by name.
         */
        virtual ManagementNode* getSyncSourceNode(const char* name);

        /**
         * Closes the configuration backend. Frees all resources associated
         * with and invalidates all ManagementNode pointers returned by this
         * config.
         */
        virtual void close();
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
