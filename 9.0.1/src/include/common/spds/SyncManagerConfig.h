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
#ifndef INCL_SYNC_CONFIG
#define INCL_SYNC_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "spds/AbstractSyncConfig.h"
#include "spds/AccessConfig.h"
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This implementation is just a transient configuration information
 * repository; persisting configuration settings is delegated to subclasses.
 */
class SyncManagerConfig : public AbstractSyncConfig {
    protected:

        AccessConfig accessConfig;
        //clientConfig stores the client cap
        DeviceConfig clientConfig;
        //serverConfig stores the server cap
        DeviceConfig serverConfig;
        SyncSourceConfig* sourceConfigs;

        unsigned int sourceConfigsCount;

        virtual bool addSyncSourceConfig(SyncSourceConfig& sc);
        
        // should be set by the client to abort the sync process smoothly
        // The SyncManager check this flag periodically (isToAbort method)
        bool abortSyncProcess;

        /**
         * This is the global error code of the last synchronization done
         * (global, not related to a specific source: for sources errors see 
         * SyncSourceConfig::lastSourceError).
         * It is read/stored in the configuration, so it is accessible
         * at any time, even after the sync ended.
         * If using the SyncClient object to trigger the sync, its value is the
         * SyncReport's lastErrorCode, set at the end of each sync session.
         * Code 0 means "no error occurred".
         */
        int lastGlobalError;
    
    public:

        SyncManagerConfig();
        virtual ~SyncManagerConfig();

        // implementation of AbstractSyncConfig (required because of different return type)
        virtual AbstractSyncSourceConfig* getAbstractSyncSourceConfig(const char* name) {
            return getSyncSourceConfig(name);
        }

        virtual AbstractSyncSourceConfig* getAbstractSyncSourceConfig(unsigned int i) {
            return getSyncSourceConfig(i);
        }

        virtual unsigned int getAbstractSyncSourceConfigsCount() const {
            return getSyncSourceConfigsCount();
        }

        // additional calls which return the more specific classes used by SyncManagerConfig
        virtual unsigned int getSyncSourceConfigsCount() const { return sourceConfigsCount; }
        virtual SyncSourceConfig* getSyncSourceConfigs() const { return sourceConfigs; }
        virtual SyncSourceConfig* getSyncSourceConfig(const char*  name, bool refresh = false);
        virtual SyncSourceConfig* getSyncSourceConfig(unsigned int i,    bool refresh = false);
        virtual bool setSyncSourceConfig(SyncSourceConfig& sc);

        virtual const AccessConfig& getAccessConfig() const { return accessConfig; }
        virtual AccessConfig& getAccessConfig() { return accessConfig; }
        virtual void setAccessConfig(AccessConfig& ac) { accessConfig.assign(ac); }


        //deprecated to remove use getClientConfig
        virtual const DeviceConfig& getDeviceConfig() const { return clientConfig; }
        virtual DeviceConfig& getDeviceConfig() { return clientConfig; }
        virtual void setDeviceConfig(DeviceConfig& dc) { clientConfig.assign(dc); }

        /**
         * get the clientConfig that stores the client configurations
         *
         * @return DeviceConfig& ref to the clientConfig object
         */
        virtual const DeviceConfig& getClientConfig() const { return clientConfig; }
        virtual DeviceConfig& getClientConfig() { return clientConfig; }
        virtual void setClientConfig(DeviceConfig& dc) { clientConfig.assign(dc); }

        /*
         * get the serverConfig that stores the server configurations
         * currently it's up to the clients to fill this object with the right data
         * as soon as the server capabilities will be implemented will be in charge
         * to the api
         *
         * @return DeviceConfig& ref to the clientConfig object
         */
        virtual const DeviceConfig& getServerConfig() const {return serverConfig;}
        virtual DeviceConfig& getServerConfig() { return serverConfig; }
        virtual void setServerConfig(DeviceConfig& dc) { serverConfig.assign(dc); }

        /* Is this call obsolete? The DeviceConfig does not have a getDirty() calls. */
        unsigned int isDirty() const { return accessConfig.getDirty() /* || deviceConfig.getDirty() */; }

        /**
         * Initializes the access and device config with default values from DefaultConfigFactory.
         */
        void setClientDefaults();

        /**
         * Initializes the given source with default values from DefaultConfigFactory.
         */
        void setSourceDefaults(const char* name);

        // glue code which implements AbstractSyncConfig via the
        // AccessConfig and DeviceConfig instances
        virtual const char*  getUsername() const { return getAccessConfig().getUsername(); }
        virtual const char*  getPassword() const { return getAccessConfig().getPassword(); }
        virtual bool getUseProxy() const { return getAccessConfig().getUseProxy(); }
        virtual const char*  getProxyHost() const { return getAccessConfig().getProxyHost(); }
        virtual int getProxyPort() const { return getAccessConfig().getProxyPort(); }
        virtual const char* getProxyUsername() const { return getAccessConfig().getProxyUsername(); }
        virtual const char* getProxyPassword() const { return getAccessConfig().getProxyPassword(); }
        virtual const char*  getSyncURL() const { return getAccessConfig().getSyncURL(); }
        virtual void setSyncURL(const char*  v) { getAccessConfig().setSyncURL(v); }
        virtual void setBeginSync(unsigned long timestamp) { getAccessConfig().setBeginSync(timestamp); }
        virtual void setEndSync(unsigned long timestamp) { getAccessConfig().setEndSync(timestamp); }
        virtual bool getServerAuthRequired() const { return getAccessConfig().getServerAuthRequired(); }
        virtual const char*  getClientAuthType() const { return getAccessConfig().getClientAuthType(); }
        virtual const char*  getServerAuthType() const { return getAccessConfig().getServerAuthType(); }
        virtual const char*  getServerPWD() const { return getAccessConfig().getServerPWD(); }
        virtual const char*  getServerID() const { return getAccessConfig().getServerID(); }
        virtual const char*  getServerNonce() const { return getAccessConfig().getServerNonce(); }
        virtual void setServerNonce(const char*  v) { getAccessConfig().setServerNonce(v); }
        virtual const char*  getClientNonce() const { return getAccessConfig().getClientNonce(); }
        virtual void setClientNonce(const char*  v) { getAccessConfig().setClientNonce(v); }
        virtual unsigned long getMaxMsgSize() const { return getAccessConfig().getMaxMsgSize(); }
        virtual unsigned long getReadBufferSize() const { return getAccessConfig().getReadBufferSize(); }
        virtual const char*  getUserAgent() const { return getAccessConfig().getUserAgent(); }
        virtual bool  getCompression() const { return getAccessConfig().getCompression(); }
        virtual unsigned int getResponseTimeout() const { return getAccessConfig().getResponseTimeout(); }

        virtual const char*  getMan() const { return getClientConfig().getMan(); }
        virtual const char*  getMod() const { return getClientConfig().getMod(); }
        virtual const char*  getOem() const { return getClientConfig().getOem(); }
        virtual const char*  getFwv() const { return getClientConfig().getFwv(); }
        virtual const char*  getSwv() const { return getClientConfig().getSwv(); }
        virtual const char*  getHwv() const { return getClientConfig().getHwv(); }
        virtual const char*  getDevID() const { return getClientConfig().getDevID(); }
        virtual const char*  getDevType() const { return getClientConfig().getDevType(); }
        virtual const char*  getDsV() const { return getClientConfig().getDsV(); }
        virtual bool getUtc() const { return getClientConfig().getUtc(); }
        virtual bool getLoSupport() const { return getClientConfig().getLoSupport(); }
        virtual bool getNocSupport() const { return getClientConfig().getNocSupport(); }
        virtual unsigned int getMaxObjSize() const { return getClientConfig().getMaxObjSize(); }
        virtual const char*  getDevInfHash() const { return getClientConfig().getDevInfHash(); }
        virtual void setDevInfHash(const char *hash) { getClientConfig().setDevInfHash(hash); }

        virtual bool getSendDevInfo() const { return getClientConfig().getSendDevInfo(); }
        virtual void setSendDevInfo(bool v) { getClientConfig().setSendDevInfo(v); }

        virtual bool getForceServerDevInfo() const { return getClientConfig().getForceServerDevInfo(); }
        virtual void setForceServerDevInfo(bool v) { getClientConfig().setForceServerDevInfo(v); }


        virtual void setServerSwv(const char* v)        { getServerConfig().setSwv(v); }
        virtual void setServerFwv(const char* v)        { getServerConfig().setFwv(v); }
        virtual void setServerHwv(const char* v)        { getServerConfig().setHwv(v); }
        virtual void setServerMan(const char* v)        { getServerConfig().setMan(v); }
        virtual void setServerMod(const char* v)        { getServerConfig().setMod(v); }
        virtual void setServerOem(const char* v)        { getServerConfig().setOem(v); }
        virtual void setServerDevID     (const char* v) { getServerConfig().setDevID(v);     }
        virtual void setServerDevType   (const char* v) { getServerConfig().setDevType(v);   }
        virtual void setServerUtc       (const bool  v) { getServerConfig().setUtc(v);       }
        virtual void setServerLoSupport (const bool  v) { getServerConfig().setLoSupport(v); }
        virtual void setServerNocSupport(const bool  v) { getServerConfig().setNocSupport(v);}
        virtual void setServerVerDTD    (const char* v) { getServerConfig().setVerDTD(v);    }
        virtual void setServerSmartSlowSync(const int v){ getServerConfig().setSmartSlowSync(v); }
        virtual void setServerMultipleEmailAccount(const int v){ getServerConfig().setMultipleEmailAccount(v); }
        virtual void setServerLastSyncURL(const char* v){ getServerConfig().setServerLastSyncURL(v); }
        virtual void setServerMediaHttpUpload(const bool v) { getServerConfig().setMediaHttpUpload(v);}
        virtual void setServerNoFieldLevelReplace(const char* v) { getServerConfig().setNoFieldLevelReplace(v);}

        virtual const char* getServerSwv() const        { return getServerConfig().getSwv(); }
        virtual const char* getServerFwv() const        { return getServerConfig().getFwv(); }
        virtual const char* getServerHwv() const        { return getServerConfig().getHwv(); }
        virtual const char* getServerMan() const        { return getServerConfig().getMan(); }
        virtual const char* getServerMod() const        { return getServerConfig().getMod(); }
        virtual const char* getServerOem() const        { return getServerConfig().getOem(); }
        virtual const char* getServerDevID() const      { return getServerConfig().getDevID(); }
        virtual const char* getServerDevType() const    { return getServerConfig().getDevType(); }
        virtual bool getServerUtc() const               { return getServerConfig().getUtc(); }
        virtual bool getServerLoSupport() const         { return getServerConfig().getLoSupport(); }
        virtual bool getServerNocSupport() const        { return getServerConfig().getNocSupport(); }
        virtual const char* getServerVerDTD() const     { return getServerConfig().getVerDTD(); }
        virtual int getServerSmartSlowSync() const      { return getServerConfig().getSmartSlowSync(); }
        virtual int getMultipleEmailAccount() const     { return getServerConfig().getMultipleEmailAccount(); }
        virtual const char* getServerLastSyncURL() const{ return getServerConfig().getServerLastSyncURL(); }
        virtual bool getServerMediaHttpUpload() const   { return getServerConfig().getMediaHttpUpload(); }
        virtual const char* getServerNoFieldLevelReplace() const   { return getServerConfig().getNoFieldLevelReplace(); }

        /**
         * Specifies the array of DataStores supported by the Server.
         * Existing datastores will be replaced by these ones.
         */
        virtual void setServerDataStores(const ArrayList* dataStores) { 
            getServerConfig().setDataStores(dataStores); 
        }

        /// Returns the array of DataStores supported by the Server.
        virtual const ArrayList* getServerDataStores() const { 
            return getServerConfig().getDataStores(); 
        }

        /**
         * Returns a specific Server DataStore, given its name (sourceRef).
         * If not found, returns NULL.
         */
        DataStore* getServerDataStore(const char* sourceRef) {
            return getServerConfig().getDataStore(sourceRef); 
        }

        virtual bool isToAbort() {
            return abortSyncProcess;
        }
        
        virtual void setToAbort(bool val = true) {
            abortSyncProcess = val;
        }

        virtual int getLastGlobalError() {
            return lastGlobalError;
        }
        virtual void setLastGlobalError(const int val) {
            lastGlobalError = val;
        }
    
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
