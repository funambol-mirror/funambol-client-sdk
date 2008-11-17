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
#ifndef INCL_SYNC_CONFIG
#define INCL_SYNC_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "spds/AccessConfig.h"
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"

/**
 * This class groups the configuration information needed by the SyncManager.
 * This implementation is just a transient configuration information
 * repository; persisting configuration settings is delegated to subclasses.
 */
class SyncManagerConfig {
    protected:

        AccessConfig accessConfig;
        DeviceConfig deviceConfig;
        SyncSourceConfig* sourceConfigs;

        unsigned int sourceConfigsCount;

        virtual BOOL addSyncSourceConfig(SyncSourceConfig& sc);

    public:

        SyncManagerConfig();
        virtual ~SyncManagerConfig();

        virtual SyncSourceConfig* getSyncSourceConfigs();
        virtual SyncSourceConfig* getSyncSourceConfig(const char*  name, BOOL refresh = FALSE);
        virtual SyncSourceConfig* getSyncSourceConfig(unsigned int i,    BOOL refresh = FALSE);
		virtual BOOL setSyncSourceConfig(SyncSourceConfig& sc);
        virtual unsigned int getSyncSourceConfigsCount();

		virtual AccessConfig& getAccessConfig();
		virtual void setAccessConfig(AccessConfig& ac);

        virtual DeviceConfig& getDeviceConfig();
		virtual void setDeviceConfig(DeviceConfig& dc);

        BOOL isDirty();

        /**
         * Initializes the access and device config with default values from DefaultConfigFactory.
         */
        void setClientDefaults();

        /**
         * Initializes the given source with default values from DefaultConfigFactory.
         */
        void setSourceDefaults(const char* name);
};

/** @} */
/** @endcond */
#endif
