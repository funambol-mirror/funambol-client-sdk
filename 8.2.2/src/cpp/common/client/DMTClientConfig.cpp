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

#include "base/fscapi.h"
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "base/globalsdef.h"
#include "base/util/utils.h"
#include "base/adapter/PlatformAdapter.h"

#include "client/DMTClientConfig.h"
#include "spdm/constants.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "spdm/ManagementNode.h"
#include "spds/DefaultConfigFactory.h"
#include "client/MailSourceManagementNode.h"

USE_NAMESPACE


void DMTClientConfig::initialize() {
    dmt = NULL;
    syncMLNode = NULL;
    serverNode = NULL;
    sourcesNode = NULL;
    // TODO: use StringBuffer instead.
    rootContext = new char[PlatformAdapter::getAppContext().length()+1];
    strcpy(rootContext, PlatformAdapter::getAppContext().c_str());
}

DMTClientConfig::DMTClientConfig() : SyncManagerConfig() {
    initialize();
}


DMTClientConfig::DMTClientConfig(const char* root): SyncManagerConfig() {
    LOG.info("DMTClientConfig(root): deprecated method, use PlatformAdapter::init(root) instead.");
    PlatformAdapter::init(root);
    initialize();
}


DMTClientConfig::~DMTClientConfig() {
    if (rootContext) {
        delete [] rootContext;
    }
    close();
}

SyncSourceConfig* DMTClientConfig::getSyncSourceConfig(const char* name, bool refresh) {
    if ((name == NULL) || (strlen(name) == 0)) {
        return NULL;
    }

    //
    // If refresh is true, we need to re-read the syncsource settings
    // from the DM tree.
    //
    // PS: for now we use a brute force approach so that we refresh the
    // entire configuration. A better implementation would be to just
    // refresh the single source.
    //
    if (refresh) {
      read();
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (strcmp(sourceConfigs[i].getName(), name) == 0) {
            return &sourceConfigs[i];
        }
    }

    return NULL;
}


SyncSourceConfig* DMTClientConfig::getSyncSourceConfig(unsigned int i, bool refresh) {
    if (i >= sourceConfigsCount) {
        return NULL;
    }

    //
    // If refresh is true, we need to re-read the syncsource settings
    // from the DM tree.
    //
    // PS: for now we use a brute force approach so that we refresh the
    // entire configuration. A better implementation would be to just
    // refresh the single source.
    //
    if (refresh) {
      read();
    }

    return &sourceConfigs[i];
}


bool DMTClientConfig::read() {
    int n = 0, i = 0; // number of sync sources
    bool ret = false;

    LOG.debug("%s", DBG_READING_CONFIG_FROM_DM);

    //
    // Reading syncml node
    //
    if (!open()) {
        return false;
    }

    //
    // Check if the server device config exists by analyzing the last error code
    // and create it otherwise.
    //
    resetError();
    // reading Server Device Config (the true is to switch to the serverConfig object)
    readDeviceConfig(*serverNode, true);
    if(getLastErrorCode() != 0) {
        LOG.debug("Server DeviceConfig not found, create a default one.");
        
        DeviceConfig* sdc = DefaultConfigFactory::getServerDeviceConfig();
        setServerConfig(*sdc);
        delete sdc; sdc = NULL;

        saveDeviceConfig(*serverNode, true);
        resetError();
        readDeviceConfig(*serverNode, true);
    }

    // Reading AccessConfig
    readAccessConfig(*syncMLNode);

    // Reading DeviceConfig
    readDeviceConfig(*syncMLNode);

    n = sourcesNode->getChildrenMaxCount();

    //
    // Let's remove previously created config objects and reinitialize
    // sourceConfigs
    //
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }
    if (n>0) {
        sourceConfigs = new SyncSourceConfig[n];
    }
    sourceConfigsCount = n;

    for (i=0; i<n; ++i) {
        // node owns children, we must not delete them
        readSourceConfig(i, *(sourcesNode) );
    }

    ret = (getLastErrorCode() == 0);

    close();
    return ret;
}


bool DMTClientConfig::save() {
    bool ret = false;
    unsigned int i = 0;

    LOG.debug("%s", DBG_WRITING_CONFIG_TO_DM);
    if (!open()) {
        return false;
    }

    if (accessConfig.getDirty()) {
        resetError();

        //
        // SyncML management node
        //
        saveAccessConfig(*syncMLNode);
    }

    saveDeviceConfig(*syncMLNode);
    saveDeviceConfig(*serverNode, true);

    //
    // Sources management node
    //
    resetError();
    for(i=0; i<sourceConfigsCount; ++i) {
        saveSourceConfig(i, *(sourcesNode) );
    }
    
    resetError(); // FIXME 

    ret = (getLastErrorCode() == 0);

    close();
    return ret;
}

bool DMTClientConfig::open() {
    char nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;

    dmt = DMTreeFactory::getDMTree(rootContext);

    delete syncMLNode;
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SYNCML);
    syncMLNode = dmt->readManagementNode(nodeName);
    if (!syncMLNode ) {
        goto failed;
    }

    delete serverNode;
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SERVER);
    serverNode = dmt->readManagementNode(nodeName);
    if (!serverNode ) {
        goto failed;
    }

    delete sourcesNode;
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SOURCES);
    sourcesNode = dmt->readManagementNode(nodeName);
    if (!sourcesNode) {
        goto failed;
    }

    return true;

failed:
    setErrorF(ERR_INVALID_CONTEXT, ERRMSG_INVALID_CONTEXT, nodeName);
    close();
    return false;
}

ManagementNode* DMTClientConfig::getSyncMLNode() {
    return syncMLNode;
}

int DMTClientConfig::getNumSources() {
    return sourcesNode ?
        sourcesNode->getChildrenMaxCount() :
        -1;
}

ManagementNode* DMTClientConfig::getSyncSourceNode(int index) {
    return sourcesNode ?
        sourcesNode->getChild(index) :
        NULL;
}

ManagementNode* DMTClientConfig::getSyncSourceNode(const char* name) {
    return sourcesNode ?
        sourcesNode->getChild(name) :
        NULL;
}

void DMTClientConfig::close() {
    
    delete syncMLNode;
    syncMLNode = NULL;
    
    delete sourcesNode;
    sourcesNode = NULL;

    delete serverNode;
    serverNode = NULL;

    delete dmt;
    dmt = NULL;
}


/*
 * Read Access Config properties stored in DMTree.
 * Access properties are placed in 3 nodes under syncML node
 * (Auth - Conn - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 * @return : true if config is correctly read
 */
bool DMTClientConfig::readAccessConfig(ManagementNode& n) {

    bool ret = true;
    char nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;
    ManagementNode* node;

    char syncMLContext[DIM_MANAGEMENT_PATH];
    char* fn = n.createFullName();
    sprintf(syncMLContext, "%s", fn);
    delete [] fn;

    //
    // Auth properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_AUTH);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readAuthConfig(n, *node)) {
            ret = false;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    //
    // Conn properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_CONN);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readConnConfig(n, *node)) {
            ret = false;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readExtAccessConfig(n, *node)) {
            ret = false;
        }

        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    return true;
}

/*
 * Save Access Config properties in DMTree.
 * Access properties are placed in 3 nodes under syncML node
 * (Auth - Conn - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 */
void DMTClientConfig::saveAccessConfig(ManagementNode& n) {

    ManagementNode* node;
    char nodeName[DIM_MANAGEMENT_PATH];

    char syncMLContext[DIM_MANAGEMENT_PATH];
    char* fn = n.createFullName();
    sprintf(syncMLContext, "%s", fn);
    delete [] fn;

    //
    // Auth properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_AUTH);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveAuthConfig(n, *node);
        delete node;
        node = NULL;
    }

    //
    // Conn properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_CONN);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveConnConfig(n, *node);
        delete node;
        node = NULL;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveExtAccessConfig(n, *node);
        delete node;
        node = NULL;
    }
}


/*
 * Read Device Config properties stored in DMTree.
 * Device properties are placed in 3 nodes under syncML node
 * (DevInfo - DevDetail - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 * @return : true if config is correctly read
 */
bool DMTClientConfig::readDeviceConfig(ManagementNode& n, bool server) {

    bool ret = true;
    char nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;
    ManagementNode* node;

    char syncMLContext[DIM_MANAGEMENT_PATH];
    char* fn = n.createFullName();
    sprintf(syncMLContext, "%s", fn);
    delete [] fn;
    //
    // DevInfo properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_INFO);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readDevInfoConfig(n, *node, server)) {
            ret = false;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    //
    // DevDetail properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_DETAIL);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readDevDetailConfig(n, *node, server)) {
            ret = false;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readExtDevConfig(n, *node, server)) {
            ret = false;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = false;
    }

    return ret;
}


/*
 * Save Device Config properties in DMTree.
 * Device properties are placed in 3 nodes under syncML node
 * (DevInfo - DevDetail - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 */
void DMTClientConfig::saveDeviceConfig(ManagementNode& n, bool server) {

    ManagementNode* node;
    char nodeName[DIM_MANAGEMENT_PATH];

    char syncMLContext[DIM_MANAGEMENT_PATH];
    char* fn = n.createFullName();
    sprintf(syncMLContext, "%s", fn);
    delete [] fn;

    //
    // DevInfo properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_INFO);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveDevInfoConfig(n, *node, server);
        delete node;
        node = NULL;
    }

    //
    // DevDetail properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_DETAIL);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveDevDetailConfig(n, *node, server);
        delete node;
        node = NULL;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveExtDevConfig(n, *node, server);
        delete node;
        node = NULL;
    }
}



/*
 * Read Source Config properties in DMTree for the desired Source.
 * Source properties are placed in specific node under sources node.
 *
 * @param i   : the index of SyncSource
 * @param n   : the sourceNode (parent node)
 */
bool DMTClientConfig::readSourceConfig(int i, ManagementNode& n) {

    ManagementNode* node;

    node = n.getChild(i);

    if (node) {
        if (!readSourceConfig(i, n, *node) ||
            !readSourceVars(i, n, *node)) {
            return false;
        }
        // *** TBD ***
        // CTCap c = getCtCap that is stored somewhere...
        //sourceConfigs[i].setCtCap(c);
    }
    return true;
}


/*
 * Save Source Config properties in DMTree for the desired Source.
 * Source properties are placed in specific node under sources node.
 * Note:
 * if the node for the current source is not found, it is created.
 *
 * @param i   : the index of SyncSource
 * @param n   : the sourceNode (parent node)
 */
void DMTClientConfig::saveSourceConfig(int i, ManagementNode& n) {

        char nodeName[DIM_MANAGEMENT_PATH];

    if( strcmp( sourceConfigs[i].getName(), "mail" ) == 0 ){

        MailSourceManagementNode* msmn = new MailSourceManagementNode(n.createFullName(),sourceConfigs[i].getName());
        MailSyncSourceConfig& mssc = ((MailSyncSourceConfig&)((sourceConfigs[i])));
        msmn->setMailSourceConfig(mssc);
        delete msmn;

    }else{
        ManagementNode* node;

        if (n.getChild(i) == NULL) {
            // Create node from Source name.
            char* fn = n.createFullName();
            sprintf(nodeName, "%s/%s", fn, sourceConfigs[i].getName());
            delete [] fn;
            node = dmt->readManagementNode(nodeName);
        }
        else {
            node = (ManagementNode*)n.getChild(i)->clone();
        }

        if (node) {
            saveSourceConfig(i, n, *node);
            saveSourceVars(i, n, *node);

            // *** TBD ***
            // CTCap c = sourceConfigs[i].getCtCap();
            // saveCtCap() somewhere...

            delete node;
        }
    }
}

bool DMTClientConfig::readAuthConfig(ManagementNode& /* syncMLNode */,
                                     ManagementNode& authNode) {
    const char *tmp;

    tmp = authNode.readPropertyValue(PROPERTY_USERNAME);
    accessConfig.setUsername(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_PASSWORD);
    accessConfig.setPassword(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_SERVER_ID);
    accessConfig.setServerID(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_SERVER_PWD);
    accessConfig.setServerPWD(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_SERVER_NONCE);
    accessConfig.setServerNonce(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_CLIENT_NONCE);
    accessConfig.setClientNonce(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_CLIENT_AUTH_TYPE);
    accessConfig.setClientAuthType(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_SERVER_AUTH_TYPE);
    accessConfig.setServerAuthType(tmp);
    delete [] tmp;

    tmp = authNode.readPropertyValue(PROPERTY_IS_SERVER_REQUIRED);
    accessConfig.setServerAuthRequired((*tmp == '1') ? true : false);
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveAuthConfig(ManagementNode& /* syncMLNode */,
                                     ManagementNode& authNode) {
    authNode.setPropertyValue(PROPERTY_USERNAME, accessConfig.getUsername());
    authNode.setPropertyValue(PROPERTY_PASSWORD, accessConfig.getPassword());
    authNode.setPropertyValue(PROPERTY_SERVER_ID, accessConfig.getServerID());
    authNode.setPropertyValue(PROPERTY_SERVER_PWD, accessConfig.getServerPWD());
    authNode.setPropertyValue(PROPERTY_SERVER_NONCE, accessConfig.getServerNonce());
    authNode.setPropertyValue(PROPERTY_CLIENT_NONCE, accessConfig.getClientNonce());
    authNode.setPropertyValue(PROPERTY_CLIENT_AUTH_TYPE, accessConfig.getClientAuthType());
    authNode.setPropertyValue(PROPERTY_SERVER_AUTH_TYPE, accessConfig.getServerAuthType());
    authNode.setPropertyValue(PROPERTY_IS_SERVER_REQUIRED,
                              (accessConfig.getServerAuthRequired() ? "1" : "0" ) );
}

bool DMTClientConfig::readConnConfig(ManagementNode& /* syncMLNode */,
                                     ManagementNode& connNode) {
    char* tmp;

    tmp = connNode.readPropertyValue(PROPERTY_SYNC_URL);
    accessConfig.setSyncURL(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_USE_PROXY);
    accessConfig.setUseProxy((*tmp == '1') ? true : false);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_PROXY_HOST);
    accessConfig.setProxyHost(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_PROXY_PORT);
    accessConfig.setProxyPort(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_PROXY_USERNAME);
    accessConfig.setProxyUsername(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_PROXY_PASSWORD);
    accessConfig.setProxyPassword(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_CHECK_CONN);
    accessConfig.setCheckConn((*tmp == '1') ? true : false);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_RESPONSE_TIMEOUT);
    accessConfig.setResponseTimeout(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_READ_BUFFER_SIZE);
    accessConfig.setReadBufferSize(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_USER_AGENT);
    accessConfig.setUserAgent(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_ENABLE_COMPRESSION);
    accessConfig.setCompression((strcmp(tmp,  "1")==0) ? true : false);
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveConnConfig(ManagementNode& /* syncMLNode */,
                                     ManagementNode& connNode) {
    char buf[512];

    connNode.setPropertyValue(PROPERTY_SYNC_URL, accessConfig.getSyncURL());
    connNode.setPropertyValue(PROPERTY_USE_PROXY,
                              (accessConfig.getUseProxy() ? "1": "0") );
    connNode.setPropertyValue(PROPERTY_PROXY_HOST, accessConfig.getProxyHost());
    sprintf(buf, "%d", accessConfig.getProxyPort());
    connNode.setPropertyValue(PROPERTY_PROXY_PORT, buf);
    connNode.setPropertyValue(PROPERTY_PROXY_USERNAME, accessConfig.getProxyUsername());
    connNode.setPropertyValue(PROPERTY_PROXY_PASSWORD, accessConfig.getProxyPassword());
    connNode.setPropertyValue(PROPERTY_CHECK_CONN,
                              (accessConfig.getCheckConn() ? "1": "0") );
    sprintf(buf, "%u", accessConfig.getResponseTimeout());
    connNode.setPropertyValue(PROPERTY_RESPONSE_TIMEOUT, buf);
    sprintf(buf, "%lu", accessConfig.getReadBufferSize());
    connNode.setPropertyValue(PROPERTY_READ_BUFFER_SIZE, buf);
    connNode.setPropertyValue(PROPERTY_USER_AGENT, accessConfig.getUserAgent());
    connNode.setPropertyValue(PROPERTY_ENABLE_COMPRESSION, accessConfig.getCompression() ? "1": "0");
}

bool DMTClientConfig::readExtAccessConfig(ManagementNode& /* syncMLNode */,
                                          ManagementNode& extNode) {
    char* tmp;

    tmp = extNode.readPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE);
    SyncMode i = (SyncMode)(*tmp ? strtol(tmp, NULL, 10) : 0);
    accessConfig.setFirstTimeSyncMode(i);
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_MAX_MSG_SIZE);
    accessConfig.setMaxMsgSize(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_SYNC_BEGIN);
    accessConfig.setBeginSync(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_SYNC_END);
    accessConfig.setEndSync(strtol(tmp, NULL, 10));
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveExtAccessConfig(ManagementNode& /* syncMLNode */,
                                          ManagementNode& extNode) {
    char buf[512];

    sprintf(buf, "%u", (unsigned)accessConfig.getFirstTimeSyncMode());
    extNode.setPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE, buf);

    sprintf(buf, "%lu", accessConfig.getMaxMsgSize());
    extNode.setPropertyValue(PROPERTY_MAX_MSG_SIZE, buf);

    timestampToAnchor(accessConfig.getBeginSync(), buf);
    extNode.setPropertyValue(PROPERTY_SYNC_BEGIN, buf);

    timestampToAnchor(accessConfig.getEndSync(), buf);
    extNode.setPropertyValue(PROPERTY_SYNC_END, buf);

}

bool DMTClientConfig::readDevInfoConfig(ManagementNode& /* syncMLNode */,
                                        ManagementNode& devInfoNode, bool server) {
    char* tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_DEVICE_ID);
    if(server){
        serverConfig.setDevID(tmp);
    }else{
        clientConfig.setDevID(tmp);
    }
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_MANUFACTURER);
    if(server){
        serverConfig.setMan(tmp);
    }else{
        clientConfig.setMan(tmp);
    }
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_MODEL);
    if(server){
        serverConfig.setMod(tmp);
    }else{
        clientConfig.setMod(tmp);
    }
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_DS_VERSION);
    clientConfig.setDsV(tmp);
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveDevInfoConfig(ManagementNode& /* syncMLNode */,
                                        ManagementNode& devInfoNode, bool server) {
    if (!server){
	devInfoNode.setPropertyValue(PROPERTY_DEVICE_ID, clientConfig.getDevID());
        devInfoNode.setPropertyValue(PROPERTY_MANUFACTURER, clientConfig.getMan());
        devInfoNode.setPropertyValue(PROPERTY_MODEL, clientConfig.getMod());
        devInfoNode.setPropertyValue(PROPERTY_DS_VERSION, clientConfig.getDsV());
    } else{
        devInfoNode.setPropertyValue(PROPERTY_DEVICE_ID, serverConfig.getDevID());
        devInfoNode.setPropertyValue(PROPERTY_MANUFACTURER, serverConfig.getMan());
        devInfoNode.setPropertyValue(PROPERTY_MODEL, serverConfig.getMod());
        devInfoNode.setPropertyValue(PROPERTY_DS_VERSION, serverConfig.getDsV());
    }
}

bool DMTClientConfig::readDevDetailConfig(ManagementNode& /* syncMLNode */,
                                          ManagementNode& devDetailNode, bool server) {
    char* tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_DEVICE_TYPE);
	if(server){
	    serverConfig.setDevType(tmp);
	}else{
		clientConfig.setDevType(tmp);
	}
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_OEM);
	if(server){
	    serverConfig.setOem(tmp);
	}else{
		clientConfig.setOem(tmp);
	}
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_FIRMWARE_VERSION);
    if(server){
	    serverConfig.setFwv(tmp);
	}else{
		clientConfig.setFwv(tmp);
	}
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_SOFTWARE_VERSION);
    if(server){
	    serverConfig.setSwv(tmp);
	}else{
		clientConfig.setSwv(tmp);
	}
	delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_HARDWARE_VERSION);
    if(server){
	    serverConfig.setHwv(tmp);
	}else{
		clientConfig.setHwv(tmp);
	}    
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT);
	if(server){
		serverConfig.setLoSupport((*tmp == '1') ? true : false);
	}else{
		clientConfig.setLoSupport((*tmp == '1') ? true : false);
	}
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveDevDetailConfig(ManagementNode& /* syncMLNode */,
                                          ManagementNode& devDetailNode, bool server) {
                                              
    if(server){

        devDetailNode.setPropertyValue(PROPERTY_DEVICE_TYPE, serverConfig.getDevType());
        devDetailNode.setPropertyValue(PROPERTY_OEM, serverConfig.getOem());
        devDetailNode.setPropertyValue(PROPERTY_FIRMWARE_VERSION, serverConfig.getFwv());
        devDetailNode.setPropertyValue(PROPERTY_SOFTWARE_VERSION, serverConfig.getSwv());
        devDetailNode.setPropertyValue(PROPERTY_HARDWARE_VERSION, serverConfig.getHwv());
        devDetailNode.setPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT,
                                   (serverConfig.getLoSupport() ? "1": "0") );

    }else{
    
        devDetailNode.setPropertyValue(PROPERTY_DEVICE_TYPE, clientConfig.getDevType());
        devDetailNode.setPropertyValue(PROPERTY_OEM, clientConfig.getOem());
        devDetailNode.setPropertyValue(PROPERTY_FIRMWARE_VERSION, clientConfig.getFwv());
        devDetailNode.setPropertyValue(PROPERTY_SOFTWARE_VERSION, clientConfig.getSwv());
        devDetailNode.setPropertyValue(PROPERTY_HARDWARE_VERSION, clientConfig.getHwv());
        devDetailNode.setPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT,
                                   (clientConfig.getLoSupport() ? "1": "0") );
    }
}

bool DMTClientConfig::readExtDevConfig(ManagementNode& /* syncMLNode */,
                                       ManagementNode& extNode, bool server) {
    char* tmp;

    if(server){

        tmp = extNode.readPropertyValue(PROPERTY_SMART_SLOW_SYNC);
		if(strcmp(tmp,"")==0){
			serverConfig.setSmartSlowSync(2);
		}else if(strcmp(tmp,"0")==0){
			serverConfig.setSmartSlowSync(0);
		}else if(strcmp(tmp,"1")==0){
			serverConfig.setSmartSlowSync(1);
		}else if(strcmp(tmp,"2")==0){
			serverConfig.setSmartSlowSync(2);
		}
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_MULTIPLE_EMAIL_ACCOUNT);
		if(strcmp(tmp,"")==0){
			serverConfig.setMultipleEmailAccount(2);
		}else if(strcmp(tmp,"0")==0){
			serverConfig.setMultipleEmailAccount(0);
		}else if(strcmp(tmp,"1")==0){
			serverConfig.setMultipleEmailAccount(1);
		}else if(strcmp(tmp,"2")==0){
			serverConfig.setMultipleEmailAccount(2);
		}
        delete [] tmp;

		tmp = extNode.readPropertyValue(PROPERTY_UTC);
        serverConfig.setUtc((*tmp == '1') ? true : false);
        delete [] tmp;
	    tmp = extNode.readPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT);
        serverConfig.setNocSupport((*tmp == '1') ? true : false);
        delete [] tmp;
		tmp = extNode.readPropertyValue(PROPERTY_VER_DTD);
        serverConfig.setVerDTD(tmp);
        delete [] tmp;
        tmp = extNode.readPropertyValue(PROPERTY_SERVER_LAST_SYNC_URL);
        serverConfig.setServerLastSyncURL(tmp);
        delete [] tmp;
     
    }else{
        tmp = extNode.readPropertyValue(PROPERTY_UTC);
        clientConfig.setUtc((*tmp == '1') ? true : false);
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT);
        clientConfig.setNocSupport((*tmp == '1') ? true : false);
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_LOG_LEVEL);
        LogLevel l = (LogLevel)strtol(tmp, NULL, 10);
        clientConfig.setLogLevel(l);
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_MAX_OBJ_SIZE);
        clientConfig.setMaxObjSize(strtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_DEVINF_HASH);
        clientConfig.setDevInfHash(tmp);
        delete [] tmp;

        tmp = extNode.readPropertyValue(PROPERTY_SEND_CLIENT_DEVINF);
        clientConfig.setSendDevInfo((*tmp == '0') ? false : true);          // So if different the default is true (send client devinf)
        delete [] tmp;

        // forceServerDevInfo is not stored: default is false and it's intended to
        // be set to true by the Client everytime we need to force it.
    }

    return true;
}

void DMTClientConfig::saveExtDevConfig(ManagementNode& /* syncMLNode */,
                                       ManagementNode& extNode, bool server) {
    char buf[512];
    if(server){
		switch (serverConfig.getSmartSlowSync()){
			case 0:
			extNode.setPropertyValue(PROPERTY_SMART_SLOW_SYNC, "0");
			break;
			case 1:
			extNode.setPropertyValue(PROPERTY_SMART_SLOW_SYNC, "1");
			break;
			case 2:
			extNode.setPropertyValue(PROPERTY_SMART_SLOW_SYNC, "2");
			break;
		}

        switch (serverConfig.getMultipleEmailAccount()){
			case 0:
			extNode.setPropertyValue(PROPERTY_MULTIPLE_EMAIL_ACCOUNT, "0");
			break;
			case 1:
			extNode.setPropertyValue(PROPERTY_MULTIPLE_EMAIL_ACCOUNT, "1");
			break;
			case 2:
			extNode.setPropertyValue(PROPERTY_MULTIPLE_EMAIL_ACCOUNT, "2");
			break;
		}

		//extNode.setPropertyValue(PROPERTY_SMART_SLOW_SYNC, itow(  ));
		extNode.setPropertyValue(PROPERTY_UTC,
                             (serverConfig.getUtc() ? "1": "0") );
        extNode.setPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT,
                             (serverConfig.getNocSupport() ? "1": "0") );
		extNode.setPropertyValue(PROPERTY_VER_DTD, serverConfig.getVerDTD());
        extNode.setPropertyValue(PROPERTY_SERVER_LAST_SYNC_URL, 
                              serverConfig.getServerLastSyncURL());
		
    }else{
    
        extNode.setPropertyValue(PROPERTY_DEVINF_HASH, clientConfig.getDevInfHash());
        extNode.setPropertyValue(PROPERTY_UTC,
                             (clientConfig.getUtc() ? "1": "0") );
        extNode.setPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT,
                             (clientConfig.getNocSupport() ? "1": "0") );

        sprintf(buf, "%d", clientConfig.getLogLevel());
        extNode.setPropertyValue(PROPERTY_LOG_LEVEL, buf);

        sprintf(buf, "%u", clientConfig.getMaxObjSize());
        extNode.setPropertyValue(PROPERTY_MAX_OBJ_SIZE, buf);

        extNode.setPropertyValue(PROPERTY_SEND_CLIENT_DEVINF,      (clientConfig.getSendDevInfo() ? "1": "0") );

        // forceServerDevInfo is not stored: default is false and it's intended to
        // be set to true by the Client everytime we need to force it.
    }
}

bool DMTClientConfig::readSourceVars(int i,
                                     ManagementNode& /* sourcesNode */,
                                     ManagementNode& sourceNode) {
    char* tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_LAST_SYNC);
    sourceConfigs[i].setLast( ((*tmp) ? strtoul(tmp, NULL, 10) : 0) );
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveSourceVars(int i,
                                     ManagementNode& /* sourcesNode */,
                                     ManagementNode& sourceNode) {
    char buf[512];

    timestampToAnchor(sourceConfigs[i].getLast(), buf);
    sourceNode.setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, buf);
}

bool DMTClientConfig::readSourceConfig(int i,
                                       ManagementNode& /* sourcesNode */,
                                       ManagementNode& sourceNode) {
    char* tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_NAME);
    sourceConfigs[i].setName(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_URI);
    sourceConfigs[i].setURI(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_SYNC_MODES);
    sourceConfigs[i].setSyncModes(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_SYNC);
    sourceConfigs[i].setSync(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_TYPE);
    sourceConfigs[i].setType(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_VERSION);
    sourceConfigs[i].setVersion(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_ENCODING);
    sourceConfigs[i].setEncoding(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_ENCRYPTION);
    sourceConfigs[i].setEncryption(tmp);
    delete [] tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_SUPP_TYPES);
    sourceConfigs[i].setSupportedTypes(tmp);
    delete [] tmp;
    
    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_ENABLED);
    sourceConfigs[i].setIsEnabled(strcmp(tmp, "0")? true:false);    // Set true if any value different from "0" (also if empty)
    delete [] tmp;

    return true;
}

void DMTClientConfig::saveSourceConfig(int i,
                                       ManagementNode& /* sourcesNode */,
                                       ManagementNode& sourceNode) {
    sourceNode.setPropertyValue(PROPERTY_SOURCE_NAME,       sourceConfigs[i].getName());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_URI,        sourceConfigs[i].getURI());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_TYPE,       sourceConfigs[i].getType());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_VERSION,    sourceConfigs[i].getVersion());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, sourceConfigs[i].getSyncModes());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SYNC,       sourceConfigs[i].getSync());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_ENCODING,   sourceConfigs[i].getEncoding());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SUPP_TYPES, sourceConfigs[i].getSupportedTypes());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_ENABLED,    sourceConfigs[i].isEnabled()? "1":"0");
    sourceNode.setPropertyValue(PROPERTY_SOURCE_ENCRYPTION, sourceConfigs[i].getEncryption());

}
