/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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

#include "base/fscapi.h"
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "client/DMTClientConfig.h"
#include "spdm/constants.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "spdm/ManagementNode.h"


void DMTClientConfig::initialize() {
    dmt = NULL;
    syncMLNode = NULL;
    sourcesNode = NULL;
}

DMTClientConfig::DMTClientConfig() : SyncManagerConfig() {
    initialize();
    rootContext = 0;
}


DMTClientConfig::DMTClientConfig(const char* root): SyncManagerConfig() {
    initialize();
    rootContext = new char[strlen(root)+1];
    strcpy(rootContext, root);
}


DMTClientConfig::~DMTClientConfig() {
    if (rootContext) {
        delete [] rootContext;
    }
    close();
}

SyncSourceConfig* DMTClientConfig::getSyncSourceConfig(const char* name, BOOL refresh) {
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


SyncSourceConfig* DMTClientConfig::getSyncSourceConfig(unsigned int i, BOOL refresh) {
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


BOOL DMTClientConfig::read() {
    int n = 0, i = 0; // number of sync sources

    BOOL ret = FALSE;

    LOG.debug(DBG_READING_CONFIG_FROM_DM);

    //
    // Reading syncml node
    //
    //char nodeName[DIM_MANAGEMENT_PATH];

    if (!open()) {
        return FALSE;
    }

    readAccessConfig(*syncMLNode);
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

    ret = TRUE;

//finally:

    close();
    return ret;
}


BOOL DMTClientConfig::save() {
    BOOL ret = FALSE;
    unsigned int i = 0;

    LOG.debug(DBG_WRITING_CONFIG_TO_DM);
    if (!open()) {
        return FALSE;
    }

    if (accessConfig.getDirty()) {
        resetError();

        //
        // SyncML management node
        //
        saveAccessConfig(*syncMLNode);
    }
    //
    // TBD: handle the dirty flag
    //

    saveDeviceConfig(*syncMLNode);

    //
    // Sources management node
    //
    lastErrorCode = ERR_NONE;
    for(i=0; i<sourceConfigsCount; ++i) {
        saveSourceConfig(i, *(sourcesNode) );
    }

    ret = (lastErrorCode == ERR_NONE);

    close();
    return ret;
}

BOOL DMTClientConfig::open() {
    char nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;

    dmt = DMTreeFactory::getDMTree(rootContext);

    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SYNCML);
    syncMLNode = dmt->readManagementNode(nodeName);
    if (!syncMLNode ) {
        goto failed;
    }

    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SOURCES);
    sourcesNode = dmt->readManagementNode(nodeName);
    if (!sourcesNode) {
        goto failed;
    }

    return TRUE;

failed:
    lastErrorCode = ERR_INVALID_CONTEXT;
    sprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
    close();
    return FALSE;
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
    if (syncMLNode) {
        delete syncMLNode;
        syncMLNode = NULL;
    }
    if (sourcesNode) {
        delete sourcesNode;
        sourcesNode = NULL;
    }
    if (dmt) {
        delete dmt;
        dmt = NULL;
    }
}


/*
 * Read Access Config properties stored in DMTree.
 * Access properties are placed in 3 nodes under syncML node
 * (Auth - Conn - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 * @return : TRUE if config is correctly read
 */
BOOL DMTClientConfig::readAccessConfig(ManagementNode& n) {

    BOOL ret = TRUE;
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
            ret = FALSE;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Conn properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_CONN);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readConnConfig(n, *node)) {
            ret = FALSE;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readExtAccessConfig(n, *node)) {
            ret = FALSE;
        }

        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    return TRUE;
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
 * @return : TRUE if config is correctly read
 */
BOOL DMTClientConfig::readDeviceConfig(ManagementNode& n) {

    BOOL ret = TRUE;
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
        if (!readDevInfoConfig(n, *node)) {
            ret = FALSE;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // DevDetail properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_DETAIL);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readDevDetailConfig(n, *node)) {
            ret = FALSE;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        if (!readExtDevConfig(n, *node)) {
            ret = FALSE;
        }
        delete node;
        node = NULL;
    }
    else {
        ret = FALSE;
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
void DMTClientConfig::saveDeviceConfig(ManagementNode& n) {

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
        saveDevInfoConfig(n, *node);
        delete node;
        node = NULL;
    }

    //
    // DevDetail properties
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_DEV_DETAIL);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveDevDetailConfig(n, *node);
        delete node;
        node = NULL;
    }

    //
    // Ext properties (other misc props)
    //
    sprintf(nodeName, "%s%s", syncMLContext, CONTEXT_EXT);
    node = dmt->readManagementNode(nodeName);
    if (node) {
        saveExtDevConfig(n, *node);
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
BOOL DMTClientConfig::readSourceConfig(int i, ManagementNode& n) {

    ManagementNode* node;

    node = n.getChild(i);

    if (node) {
        if (!readSourceConfig(i, n, *node) ||
            !readSourceVars(i, n, *node)) {
            return FALSE;
        }
        // *** TBD ***
        // CTCap c = getCtCap that is stored somewhere...
        //sourceConfigs[i].setCtCap(c);
    }
    return TRUE;
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

    ManagementNode* node;
    char nodeName[DIM_MANAGEMENT_PATH];

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

BOOL DMTClientConfig::readAuthConfig(ManagementNode& syncMLNode,
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
    accessConfig.setServerAuthRequired((*tmp == '1') ? TRUE : FALSE);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveAuthConfig(ManagementNode& syncMLNode,
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

BOOL DMTClientConfig::readConnConfig(ManagementNode& syncMLNode,
                                     ManagementNode& connNode) {
    char* tmp;

    tmp = connNode.readPropertyValue(PROPERTY_SYNC_URL);
    accessConfig.setSyncURL(tmp);
    delete [] tmp;

    tmp = connNode.readPropertyValue(PROPERTY_USE_PROXY);
    accessConfig.setUseProxy((*tmp == '1') ? TRUE : FALSE);
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
    accessConfig.setCheckConn((*tmp == '1') ? TRUE : FALSE);
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
    accessConfig.setCompression((strcmp(tmp,  "1")==0) ? TRUE : FALSE);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveConnConfig(ManagementNode& syncMLNode,
                                     ManagementNode& connNode) {
    char buf[512];

    connNode.setPropertyValue(PROPERTY_SYNC_URL, accessConfig.getSyncURL());
    connNode.setPropertyValue(PROPERTY_USE_PROXY,
                              (accessConfig.getUseProxy() ? "1": "0") );
    connNode.setPropertyValue(PROPERTY_PROXY_HOST, accessConfig.getProxyHost());
    sprintf(buf, "%lu", accessConfig.getProxyPort());
    connNode.setPropertyValue(PROPERTY_PROXY_PORT, buf);
    connNode.setPropertyValue(PROPERTY_PROXY_USERNAME, accessConfig.getProxyUsername());
    connNode.setPropertyValue(PROPERTY_PROXY_PASSWORD, accessConfig.getProxyPassword());
    connNode.setPropertyValue(PROPERTY_CHECK_CONN,
                              (accessConfig.getCheckConn() ? "1": "0") );
    sprintf(buf, "%lu", accessConfig.getResponseTimeout());
    connNode.setPropertyValue(PROPERTY_RESPONSE_TIMEOUT, buf);
    sprintf(buf, "%lu", accessConfig.getReadBufferSize());
    connNode.setPropertyValue(PROPERTY_READ_BUFFER_SIZE, buf);
    connNode.setPropertyValue(PROPERTY_USER_AGENT, accessConfig.getUserAgent());
    connNode.setPropertyValue(PROPERTY_ENABLE_COMPRESSION, accessConfig.getCompression() ? "1": "0");
}

BOOL DMTClientConfig::readExtAccessConfig(ManagementNode& syncMLNode,
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

    return TRUE;
}

void DMTClientConfig::saveExtAccessConfig(ManagementNode& syncMLNode,
                                          ManagementNode& extNode) {
    char buf[512];

    sprintf(buf, "%lu", accessConfig.getFirstTimeSyncMode());
    extNode.setPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE, buf);

    sprintf(buf, "%lu", accessConfig.getMaxMsgSize());
    extNode.setPropertyValue(PROPERTY_MAX_MSG_SIZE, buf);

    timestampToAnchor(accessConfig.getBeginSync(), buf);
    extNode.setPropertyValue(PROPERTY_SYNC_BEGIN, buf);

    timestampToAnchor(accessConfig.getEndSync(), buf);
    extNode.setPropertyValue(PROPERTY_SYNC_END, buf);

}

BOOL DMTClientConfig::readDevInfoConfig(ManagementNode& syncMLNode,
                                        ManagementNode& devInfoNode) {
    char* tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_DEVICE_ID);
    deviceConfig.setDevID(tmp);
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_MANUFACTURER);
    deviceConfig.setMan(tmp);
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_MODEL);
    deviceConfig.setMod(tmp);
    delete [] tmp;

    tmp = devInfoNode.readPropertyValue(PROPERTY_DS_VERSION);
    deviceConfig.setDsV(tmp);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveDevInfoConfig(ManagementNode& syncMLNode,
                                        ManagementNode& devInfoNode) {
    devInfoNode.setPropertyValue(PROPERTY_DEVICE_ID, deviceConfig.getDevID());
    devInfoNode.setPropertyValue(PROPERTY_MANUFACTURER, deviceConfig.getMan());
    devInfoNode.setPropertyValue(PROPERTY_MODEL, deviceConfig.getMod());
    devInfoNode.setPropertyValue(PROPERTY_DS_VERSION, deviceConfig.getDsV());
}

BOOL DMTClientConfig::readDevDetailConfig(ManagementNode& syncMLNode,
                                          ManagementNode& devDetailNode) {
    char* tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_DEVICE_TYPE);
    deviceConfig.setDevType(tmp);
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_OEM);
    deviceConfig.setOem(tmp);
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_FIRMWARE_VERSION);
    deviceConfig.setFwv(tmp);
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_SOFTWARE_VERSION);
    deviceConfig.setSwv(tmp);
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_HARDWARE_VERSION);
    deviceConfig.setHwv(tmp);
    delete [] tmp;

    tmp = devDetailNode.readPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT);
    deviceConfig.setLoSupport((*tmp == '1') ? TRUE : FALSE);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveDevDetailConfig(ManagementNode& syncMLNode,
                                          ManagementNode& devDetailNode) {
    devDetailNode.setPropertyValue(PROPERTY_DEVICE_TYPE, deviceConfig.getDevType());
    devDetailNode.setPropertyValue(PROPERTY_OEM, deviceConfig.getOem());
    devDetailNode.setPropertyValue(PROPERTY_FIRMWARE_VERSION, deviceConfig.getFwv());
    devDetailNode.setPropertyValue(PROPERTY_SOFTWARE_VERSION, deviceConfig.getSwv());
    devDetailNode.setPropertyValue(PROPERTY_HARDWARE_VERSION, deviceConfig.getHwv());
    devDetailNode.setPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT,
                                   (deviceConfig.getLoSupport() ? "1": "0") );
}

BOOL DMTClientConfig::readExtDevConfig(ManagementNode& syncMLNode,
                                       ManagementNode& extNode) {
    char* tmp;

    tmp = extNode.readPropertyValue(PROPERTY_VER_DTD);
    deviceConfig.setVerDTD(tmp);
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_UTC);
    deviceConfig.setUtc((*tmp == '1') ? TRUE : FALSE);
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT);
    deviceConfig.setNocSupport((*tmp == '1') ? TRUE : FALSE);
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_LOG_LEVEL);
    LogLevel l = (LogLevel)strtol(tmp, NULL, 10);
    deviceConfig.setLogLevel(l);
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_MAX_OBJ_SIZE);
    deviceConfig.setMaxObjSize(strtol(tmp, NULL, 10));
    delete [] tmp;

    tmp = extNode.readPropertyValue(PROPERTY_DEVINF_HASH);
    deviceConfig.setDevInfHash(tmp);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveExtDevConfig(ManagementNode& syncMLNode,
                                       ManagementNode& extNode) {
    char buf[512];

    extNode.setPropertyValue(PROPERTY_VER_DTD, deviceConfig.getVerDTD());
    extNode.setPropertyValue(PROPERTY_DEVINF_HASH, deviceConfig.getDevInfHash());
    extNode.setPropertyValue(PROPERTY_UTC,
                             (deviceConfig.getUtc() ? "1": "0") );
    extNode.setPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT,
                             (deviceConfig.getNocSupport() ? "1": "0") );

    sprintf(buf, "%lu", deviceConfig.getLogLevel());
    extNode.setPropertyValue(PROPERTY_LOG_LEVEL, buf);

    sprintf(buf, "%lu", deviceConfig.getMaxObjSize());
    extNode.setPropertyValue(PROPERTY_MAX_OBJ_SIZE, buf);
}

BOOL DMTClientConfig::readSourceVars(int i,
                                     ManagementNode& sourcesNode,
                                     ManagementNode& sourceNode) {
    char* tmp;

    tmp = sourceNode.readPropertyValue(PROPERTY_SOURCE_LAST_SYNC);
    sourceConfigs[i].setLast( ((*tmp) ? strtol(tmp, NULL, 10) : 0) );
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveSourceVars(int i,
                                     ManagementNode& sourcesNode,
                                     ManagementNode& sourceNode) {
    char buf[512];

    timestampToAnchor(sourceConfigs[i].getLast(), buf);
    sourceNode.setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, buf);
}

BOOL DMTClientConfig::readSourceConfig(int i,
                                       ManagementNode& sourcesNode,
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

    return TRUE;
}

void DMTClientConfig::saveSourceConfig(int i,
                                       ManagementNode& sourcesNode,
                                       ManagementNode& sourceNode) {
    sourceNode.setPropertyValue(PROPERTY_SOURCE_NAME, sourceConfigs[i].getName());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_URI, sourceConfigs[i].getURI());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_TYPE, sourceConfigs[i].getType());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_VERSION, sourceConfigs[i].getVersion());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, sourceConfigs[i].getSyncModes());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SYNC, sourceConfigs[i].getSync());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_ENCODING, sourceConfigs[i].getEncoding());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_SUPP_TYPES, sourceConfigs[i].getSupportedTypes());
    sourceNode.setPropertyValue(PROPERTY_SOURCE_ENCRYPTION, sourceConfigs[i].getEncryption());

}
