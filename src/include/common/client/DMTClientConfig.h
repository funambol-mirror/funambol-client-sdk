/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
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

class DMTree;

/**
 * This class is an extension of SyncManagerConfig that is DM tree aware; this
 * means that configuration properties are read/stored from/to the DM tree.
 *
 * @todo describe the properties of the DM tree and how they are grouped into device info,
 * additional device info, extended device info, etc.
 */
class DMTClientConfig : public SyncManagerConfig {

    protected:

        char*  rootContext;

        DMTree* dmt;
        ManagementNode* syncMLNode;
        ManagementNode* sourcesNode;

        void initialize();
        DMTClientConfig();

        /* top level functions */
        virtual BOOL readAccessConfig(ManagementNode& n);
        virtual void saveAccessConfig(ManagementNode& n);
        virtual BOOL readDeviceConfig(ManagementNode& n);
        virtual void saveDeviceConfig(ManagementNode& n);
        virtual BOOL readSourceConfig(int i, ManagementNode& n);
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
        virtual BOOL readAuthConfig(ManagementNode& syncMLNode,
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
        virtual BOOL readConnConfig(ManagementNode& syncMLNode,
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
        virtual BOOL readExtAccessConfig(ManagementNode& syncMLNode,
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
        virtual BOOL readDevInfoConfig(ManagementNode& syncMLNode,
                                       ManagementNode& devInfoNode);
        /**
         * Same as readAccessConfig() for saving device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devInfoNode    the "spds/syncml/devinfo" node
         */
        virtual void saveDevInfoConfig(ManagementNode& syncMLNode,
                                       ManagementNode& devInfoNode);

        /**
         * Same as readAccessConfig() for reading additional device
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devDetailNode  the "spds/syncml/devdetail" node
         */
        virtual BOOL readDevDetailConfig(ManagementNode& syncMLNode,
                                         ManagementNode& devDetailNode);
        /**
         * Same as readAccessConfig() for saving additional device
         * information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param devDetailNode  the "spds/syncml/devdetail" node
         */
        virtual void saveDevDetailConfig(ManagementNode& syncMLNode,
                                         ManagementNode& devDetailNode);

        /**
         * Same as readAccessConfig() for reading some more additional
         * device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual BOOL readExtDevConfig(ManagementNode& syncMLNode,
                                      ManagementNode& extNode);
        /**
         * Same as readAccessConfig() for saving some more additional
         * device information.
         *
         * @param syncMLNode     the "spds/syncml" node
         * @param extNode        the "spds/syncml/ext" node
         */
        virtual void saveExtDevConfig(ManagementNode& syncMLNode,
                                      ManagementNode& extNode);

        /**
         * Same as readAccessConfig() for reading variables that the
         * library uses internally, like anchors.
         *
         * @param i              index of the source
         * @param sourcesNode    the "spds/sources" node
         * @param sourceNode     the "spds/sources/<source name>" node
         */
        virtual BOOL readSourceVars(int i,
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
        virtual BOOL readSourceConfig(int i,
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

    public:

        DMTClientConfig(const char*  root);

        ~DMTClientConfig();

        SyncSourceConfig* getSyncSourceConfig(const char* name, BOOL refresh = FALSE);
        SyncSourceConfig* getSyncSourceConfig(unsigned int i,   BOOL refresh = FALSE);

        virtual BOOL read();
        virtual BOOL save();

        /**
         * Opens the configuration backend associated with the root context.
         * Calling on an open config does nothing.
         *
         * @return TRUE for success
         */
        virtual BOOL open();

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

/** @} */
/** @endcond */
#endif
