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
#include "base/Log.h"
#include "base/debug.h"
#include "base/util/utils.h"
#include "base/base64.h"
#include "base/messages.h"
#include "http/TransportAgentFactory.h"
#include "spds/constants.h"
#include "spds/DataTransformer.h"
#include "spds/DataTransformerFactory.h"
#include "spds/AbstractSyncConfig.h"
#include "spds/SyncManager.h"
#include "spds/SyncMLProcessor.h"
#include "spds/spdsutils.h"
#include "syncml/core/TagNames.h"
#include "syncml/core/ObjectDel.h"


#include "event/FireEvent.h"

#include <limits.h>
#include "base/globalsdef.h"
#include "spds/MappingsManager.h"

USE_NAMESPACE

MappingStoreBuilder* MappingsManager::builder = NULL;

static void fillContentTypeInfoList(ArrayList &l, const char*  types);

const char SyncManager::encodedKeyPrefix[] = "funambol-b64-";

// Module variables ----------------------------------------------------------

static char prevSourceName[64];
static char prevSourceUri[64];
static SyncMode prevSyncMode;

static bool isFiredSyncEventBEGIN;
//static bool isFiredSyncEventEND;

// Static functions ------------------------------------------------------------

/**
 * Is the given status code an error status code? Error codes are the ones
 * outside the range 200-299.
 *
 * @param status the status code to check
 */
inline static bool isErrorStatus(int status) {
    return (status) && ((status < 200) || (status > 299));
}

/**
 * Return true if the status code is authentication failed
 *
 * @param status the status code to check
 */
inline static bool isAuthFailed(int status) {
    return (status) && ((status == 401) || (status == 407));
}

/**
 * Return true if there's no more work to do
 * (if no source has a correct status)
 */
bool SyncManager::isToExit() {
    for (int i = 0; i < sourcesNumber; i++) {
        if (sources[i]->getReport()->checkState() == true) {
            return false; 
        }
    }
    return true;
}

//--------------------------------------------------------------------------

void SyncManager::decodeItemKey(SyncItem *syncItem)
{
    char *key;

    if (syncItem &&
        (key = toMultibyte(syncItem->getKey())) != NULL &&
        !strncmp(key, encodedKeyPrefix, strlen(encodedKeyPrefix))) {
        int len;
        char *decoded = (char *)b64_decode(len, key + strlen(encodedKeyPrefix));
        LOG.debug("replacing encoded key '%s' with unsafe key '%s'", key, decoded);
        WCHAR* t = toWideChar(decoded);
        syncItem->setKey(t);
        delete [] decoded;
        delete [] key;
        delete [] t;
    }
}

void SyncManager::encodeItemKey(SyncItem *syncItem)
{
    if (!syncItem) { 
        LOG.error("The syncItem is NULL: invalid encoding?");
        return; 
    }

    if (wcschr(syncItem->getKey(), '<') || wcschr(syncItem->getKey(), '&')) {
        char *key= toMultibyte(syncItem->getKey());

        if (!key) {
            LOG.error("encodeItemKey: cannot convert key %" WCHAR_PRINTF, syncItem->getKey());
            return;
        }
        
        StringBuffer encoded;
        b64_encode(encoded, key, strlen(key));
        StringBuffer newkey(encodedKeyPrefix);
        newkey += encoded;
        LOG.debug("replacing unsafe key '%s' with encoded key '%s'", key, newkey.c_str());
        WCHAR* t = toWideChar(newkey.c_str());
        syncItem->setKey(t);
        
        delete [] t;
        delete [] key;
    }

}

/*
 * Utility to set a SyncSource state + errorCode + errorMsg.
 */
void SyncManager::setSourceStateAndError(unsigned int index, SourceState  state,
                                         unsigned int code,  const char*  msg) {

    SyncSourceReport* report = sources[index]->getReport();

    report->setState(state);
    report->setLastErrorCode(code);
    report->setLastErrorMsg(msg);
}


// Used to reserve some more space (DATA_SIZE_TOLERANCE) for incoming items.
// This is used to add a little tolerance to the data size of items sent by
// server in case of large object (item splitted in multiple chunks).
long SyncManager::getToleranceDataSize(long size) {
    return (long)(size*DATA_SIZE_TOLERANCE + 0.5);
}


// Used to verify if data size of incoming item is different from the one declared.
bool SyncManager::testIfDataSizeMismatch(long allocatedSize, long receivedSize) {
    long declaredSize = (long) (allocatedSize/DATA_SIZE_TOLERANCE + 0.5);
    if (declaredSize != receivedSize) {
        LOG.info("WARNING! Item size mismatch: real size = %d, declared size = %d", receivedSize, declaredSize);
        return true;
    }
    return false;
}


SyncManager::SyncManager(AbstractSyncConfig& c, SyncReport& report) : config(c), syncReport(report) {
    initialize();
}

void SyncManager::initialize() {
    // set all values which are checked by the destructor;
    // previously some pointers were only set later, leading to
    // uninitialized memory reads and potential crashes when
    // constructing a SyncManager, but not using it
    transportAgent = NULL;
    
    sources        = NULL;
    currentState   = STATE_START;
    sourcesNumber  = 0;
    count          = 0;
    devInf         = NULL;
    incomingItem   = NULL;

    syncURL = config.getSyncURL();
    deviceId = config.getDevID();

    credentialHandler.setUsername           (config.getUsername());
    credentialHandler.setPassword           (config.getPassword());
    credentialHandler.setClientNonce        (config.getClientNonce());
    credentialHandler.setClientAuthType     (config.getClientAuthType());

    credentialHandler.setServerID           (config.getServerID());
    credentialHandler.setServerPWD          (config.getServerPWD());
    credentialHandler.setServerNonce        (config.getServerNonce());
    credentialHandler.setServerAuthType     (config.getServerAuthType());
    credentialHandler.setServerAuthRequired (config.getServerAuthRequired());

    responseTimeout = config.getResponseTimeout();
    if (responseTimeout <= 0) {
        responseTimeout = DEFAULT_MAX_TIMEOUT;
    }
    maxMsgSize   = config.getMaxMsgSize();
    if (maxMsgSize <= 0) {
        maxMsgSize = DEFAULT_MAX_MSG_SIZE;
    }
    maxObjSize   = config.getMaxObjSize();
    loSupport    = config.getLoSupport();
    readBufferSize = 5000; // default value

    if (config.getReadBufferSize() > 0)
        readBufferSize = config.getReadBufferSize();

    syncMLBuilder.set(syncURL.c_str(), deviceId.c_str());
    memset(credentialInfo, 0, 1024*sizeof(char));
    sortedSourcesFromServer = NULL;
    prevSourceName[0] = 0;
    prevSourceUri[0] = 0;
    prevSyncMode = SYNC_NONE;
    isFiredSyncEventBEGIN = false;

    mmanager = NULL;

}

SyncManager::~SyncManager() {
    if (transportAgent) {
        delete transportAgent;
    }
    
    if (sources) {
        // This deletes only SyncSource array
        // We DON'T want to release SyncSource objects here!
        delete [] sources;
    }
    if (devInf) {
        delete devInf;
    }
    if (incomingItem) {
        delete incomingItem;
    }
    if (sortedSourcesFromServer) {
        int i=0;
        while (sortedSourcesFromServer[i]) {
            delete [] sortedSourcesFromServer[i];
            i++;
        }
        delete [] sortedSourcesFromServer;
    }

    if (mmanager) {
        int i=0;
        while (mmanager[i]) {
            delete mmanager[i];
            i++;
        }
        delete [] mmanager;
    }

}

/*
 * Modification to perform the sync of an array of sync sources.
 */

int SyncManager::prepareSync(SyncSource** s) {

    char* initMsg               = NULL;
    const char* respURI         = NULL;
    char* responseMsg           = NULL;
    SyncML*  syncml             = NULL;
    int ret                     = 0;
    int count                   = 0;
    const char* requestedAuthType  = NULL;
    ArrayList* list             = NULL; //new ArrayList();
    ArrayList alerts;

    // for authentication improvments
    bool isServerAuthRequired   = credentialHandler.getServerAuthRequired();
    int clientAuthRetries       = 1;
    int serverAuthRetries       = 1;
    int authStatusCode          = 200;

    bool isClientAuthenticated  = false;
    bool isServerAuthenticated  = false;
    Chal*   clientChal          = NULL; // The chal of the server to the client
    Chal*   serverChal          = NULL; // The chal of the client to the server
    Status* status              = NULL; // The status from the client to the server
    Cred*   cred                = NULL;
    Alert*  alert               = NULL;
    StringBuffer* devInfStr     = NULL;
    bool putDevInf              = false;
    char devInfHash[16 * 4 +1]; // worst case factor base64 is four
    unsigned long timestamp = (unsigned long)time(NULL);

    //lastErrorCode = 0;
    resetError();

    // Fire Sync Begin Event
    fireSyncEvent(NULL, SYNC_BEGIN);

    URL url(config.getSyncURL());
    Proxy proxy;
    LOG.developer(MSG_SYNC_URL, syncURL.c_str());

    // Copy and validate given
    sourcesNumber = assignSources(s);
    if (getLastErrorCode()) {
        ret = getLastErrorCode();    // set when a source has an invalid config
    }

    if (isToExit()) {
        if (!ret) {
            // error: no source to sync
            setError(ERR_NO_SOURCE_TO_SYNC, ERRMSG_NO_SOURCE_TO_SYNC);
            ret = getLastErrorCode();
        }

        goto finally;
    }

    // Set proxy username/password if proxy is used.
    if (config.getUseProxy()) {
        //char* proxyHost = config.getProxyHost();
        //int    proxyPort = config.getProxyPort();
        const char* proxyUser = config.getProxyUsername();
        const char* proxyPwd  = config.getProxyPassword();
        proxy.setProxy(NULL, 0, proxyUser, proxyPwd);
    }
        
    mmanager = new MappingsManager*[sourcesNumber + 1];
    
    for (count = 0; count < sourcesNumber; count++) {
        LOG.info(MSG_PREPARING_SYNC, sources[count]->getConfig().getName());
        mmanager[count] = new MappingsManager(sources[count]->getConfig().getName());
    }
    mmanager[count] = 0;

    syncMLBuilder.resetCommandID();
    syncMLBuilder.resetMessageID();
    config.setBeginSync(timestamp);

    // Create the device informations.
    devInf = createDeviceInfo();

    // check device information for changes
    if (devInf) {
        char md5[16];
        devInfStr = Formatter::getDevInf(devInf);
        LOG.debug("Checking devinfo...");
        // Add syncUrl to devInfHash, so hash changes if syncUrl has changed
        devInfStr->append("<SyncURL>");
        devInfStr->append(config.getSyncURL());
        devInfStr->append("</SyncURL>");
        calculateMD5(devInfStr->c_str(), devInfStr->length(), md5);
        devInfHash[b64_encode(devInfHash, md5, sizeof(md5))] = 0;
        LOG.debug("devinfo hash: %s", devInfHash);

        // compare against previous device info hash:
        // if different, then the local config has changed and
        // infos should be sent again
        if (strcmp(devInfHash, config.getDevInfHash())) {
            putDevInf = true;
        }
        LOG.debug("devinfo %s", putDevInf ? "changed, retransmit" : "unchanged, no need to send");
    } else {
        LOG.debug("no devinfo available");
    }

    if (isServerAuthRequired == false) {
        isServerAuthenticated = true;
    }

    // Authentication
    do {
        deleteCred(&cred);
        deleteAlert(&alert);
        deleteSyncML(&syncml);
        alerts.clear();
        

        bool addressChange = false;

        // credential of the client
        if (isClientAuthenticated == false) {
            char anc[DIM_ANCHOR];
            timestamp = (unsigned long)time(NULL);
            for (count = 0; count < sourcesNumber; count ++) {
                if (!sources[count]->getReport()->checkState())
                    continue;
                sources[count]->setNextSync(timestamp);
                timestampToAnchor(sources[count]->getNextSync(), anc);
                sources[count]->setNextAnchor(anc);
                // Test if this source is for AddressChangeNotification
                int prefmode = sources[count]->getPreferredSyncMode();
                if( prefmode == SYNC_ADDR_CHANGE_NOTIFICATION ) {
                    alert = syncMLBuilder.prepareAddrChangeAlert(*sources[count]);
                    if(!alert) {
                        LOG.error("Error preparing Address Change Sync");
                        goto finally;
                    }
                    addressChange = true;   // remember that this sync is for
                                            // address change notification
                }
                else {
                    alert = syncMLBuilder.prepareInitAlert(*sources[count], maxObjSize);
                }
                alerts.add(*alert);
                deleteAlert(&alert);
            }
            cred = credentialHandler.getClientCredential();
            strcpy(credentialInfo, cred->getAuthentication()->getPassword());

        }

        // actively send out device infos?
        if (putDevInf) {
            AbstractCommand* put = syncMLBuilder.prepareDevInf(NULL, *devInf);
            if (put) {
                commands.add(*put);
                delete put;
            }
            putDevInf = false;
        }

        // "cred" only contains an encoded strings as username, also
        // need the original username for LocName
        syncml = syncMLBuilder.prepareInitObject(cred, &alerts, &commands, maxMsgSize, maxObjSize);
        if (syncml == NULL) {
            ret = getLastErrorCode();
            goto finally;
        }

        initMsg = syncMLBuilder.prepareMsg(syncml);
        if (initMsg == NULL) {
            ret = getLastErrorCode();
            goto finally;
        }

        LOG.debug("%s", MSG_INITIALIZATATION_MESSAGE);

        currentState = STATE_PKG1_SENDING;

        if (transportAgent == NULL) {
            transportAgent = TransportAgentFactory::getTransportAgent(url, proxy, responseTimeout, maxMsgSize);
            transportAgent->setReadBufferSize(readBufferSize);
            transportAgent->setSSLServerCertificates(config.getSSLServerCertificates());
            transportAgent->setSSLVerifyServer(config.getSSLVerifyServer());
            transportAgent->setSSLVerifyHost(config.getSSLVerifyHost());
            // Here we also ensure that the user agent string is valid
            const char* ua = getUserAgent(config);
            LOG.debug("User Agent = %s", ua);
            transportAgent->setUserAgent(ua);
            transportAgent->setCompression(config.getCompression());
            delete [] ua; ua = NULL;
        }
        else {
            transportAgent->setURL(url);
        }
        if (getLastErrorCode() != 0) { // connection: lastErrorCode = 2005: Impossible to establish internet connection
            ret = getLastErrorCode();
            goto finally;
        }

        deleteSyncML(&syncml);
        deleteChal(&serverChal);
        deleteCred(&cred);
        commands.clear();

        //Fire Initialization Event
        fireSyncEvent(NULL, SEND_INITIALIZATION);

        responseMsg = transportAgent->sendMessage(initMsg);
        // Non-existant or empty reply?
        // Synthesis server replies with empty message to
        // a message that it cannot parse.
        if (responseMsg == NULL || !responseMsg[0]) {
            if (responseMsg) {
                delete [] responseMsg;
                responseMsg = NULL;
            }

            if ( addressChange && getLastErrorCode() == ERR_READING_CONTENT ) {
                // This is not an error if it's an AddressChange
                ret = 0;
            }
            else {
                // use last error code if one has been set (might not be the case)
                ret = getLastErrorCode();
                /*
                if (!ret) {
                    ret = ERR_READING_CONTENT;
                }
                */
            }
            goto finally;
        }

        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        syncml = syncMLProcessor.processMsg(responseMsg);
        safeDelete(&responseMsg);
        safeDelete(&initMsg);

        if (syncml == NULL) {
            ret = getLastErrorCode();
            LOG.error("Error processing alert response.");
            goto finally;
        }

        // ret = syncMLProcessor.processInitResponse(*sources[0], syncml, &alerts);

        ret = syncMLProcessor.processSyncHdrStatus(syncml);

        if (ret == -1) {
            ret = getLastErrorCode();
            LOG.error("Error processing SyncHdr Status");
            goto finally;

        } else if (isErrorStatus(ret) && ! isAuthFailed(ret)) {
            setErrorF(ret, "Error from server: status = %d", ret);
            goto finally;
        }

        for (count = 0; count < sourcesNumber; count ++) {
            if (!sources[count]->getReport()->checkState())
                continue;

            int sourceRet = syncMLProcessor.processAlertStatus(*sources[count], syncml, &alerts);
            if (isAuthFailed(ret) && sourceRet == -1) {
                // Synthesis server does not include SourceRefs if
                // authentication failed. Remember the authentication
                // failure in that case, otherwise we'll never get to the getChal() below.
            } else {
                ret = sourceRet;
            }

            if (ret == -1 || ret == 404 || ret == 415) {
                setErrorF(ret, "AlertStatus from server %d", ret);
                LOG.error("%s", getLastErrorMsg());
                setSourceStateAndError(count, SOURCE_ERROR, ret, getLastErrorMsg());
            }
        }
        if (isToExit()) {
            // error. no source to sync
            ret = getLastErrorCode();
            goto finally;
        }

        //
        // Set the uri with session
        //
        respURI = syncMLProcessor.getRespURI(syncml->getSyncHdr());
        if (respURI) {
            url = respURI;
            transportAgent->setURL(url);
        }
        //
        // Server Authentication
        //
        if (isServerAuthenticated == false) {

            cred = syncml->getSyncHdr()->getCred();
            if (cred == NULL) {
                if (serverAuthRetries == 1) {
                    // create the serverNonce if needed and set into the CredentialHendler, serverNonce property
                    serverChal = credentialHandler.getServerChal(isServerAuthenticated);
                    authStatusCode = 407;
                    serverAuthRetries++;
                } else {
                     ret = -1;
                     goto finally;
                }

            } else {
                isServerAuthenticated = credentialHandler.performServerAuth(cred);
                if (isServerAuthenticated) {
                    serverChal   = credentialHandler.getServerChal(isServerAuthenticated);
                    authStatusCode = 212;
                }
                else {
                    if (strcmp(credentialHandler.getServerAuthType(), AUTH_TYPE_MD5) == 0 ||
                        serverAuthRetries == 1)
                    {
                        serverChal   = credentialHandler.getServerChal(isServerAuthenticated);
                        authStatusCode = 401;

                    } else {
                        ret = -1;   //XXX
                        LOG.error("Server not authenticated");
                        goto finally;
                    }
                    serverAuthRetries++;
                }
            }
            cred = NULL; // this cred is only a reference
        } else  {
            authStatusCode = 200;
        }
        status = syncMLBuilder.prepareSyncHdrStatus(serverChal, authStatusCode);
        commands.add(*status);
        deleteStatus(&status);
        list = syncMLProcessor.getCommands(syncml->getSyncBody(), ALERT);
        for (count = 0; count < sourcesNumber; count ++) {
            if (!sources[count]->getReport()->checkState())
                continue;

            status = syncMLBuilder.prepareAlertStatus(*sources[count], list, authStatusCode);
            if (status) {
                commands.add(*status);
                deleteStatus(&status);
            }
        }
        if (list) {
            delete list;
            list = NULL;
        }

        //
        // Process Put/Get commands
        //
        list = syncml->getSyncBody()->getCommands();
        int cmdindex;
        for (cmdindex = 0; cmdindex < list->size(); cmdindex++) {
            AbstractCommand* cmd = (AbstractCommand*)list->get(cmdindex);
            const char* name = cmd->getName();
            if (name) {
                bool isPut = !strcmp(name, PUT);
                bool isGet = !strcmp(name, GET);

                if (isGet || isPut) {
                    int statusCode = 200; // if set, then send it (on by default)

                    if (isGet) {
                        Get *get = (Get *)cmd;
                        ArrayList *items = get->getItems();
                        bool sendDevInf = false;

                        Results results;
                        for (int i = 0; i < items->size(); i++) {
                            Item *item = (Item *)items->get(i);

                            // we are not very picky: as long as the Item is
                            // called "./devinf11" as required by the standard
                            // we return our device infos
                            Target *target = item->getTarget();
                            if (target && target->getLocURI() &&
                                !strcmp(target->getLocURI(),
                                         DEVINF_URI)) {
                                sendDevInf = true;
                            } else {
                                LOG.debug("ignoring request to Get item #%d", i);
                            }
                        }

                        // cannot send if we have nothing, then simply acknowledge the request,
                        // but ignore it
                        if (sendDevInf && devInf) {
                            AbstractCommand *result = syncMLBuilder.prepareDevInf(cmd, *devInf);
                            if (result) {
                                commands.add(*result);
                                delete result;
                            }
                        }
                    } else {
                        // simply acknowledge Put
                    }

                    if (statusCode) {
                        status = syncMLBuilder.prepareCmdStatus(*cmd, statusCode);
                        if (status) {
                            // Fire Sync Status Event: status from client
                            fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), NULL, NULL, NULL , CLIENT_STATUS);

                            commands.add(*status);
                            deleteStatus(&status);
                        }
                    }
                }
            }
        }

        //
        // Client Authentication. The auth of the client on the server
        //
        clientChal = syncMLProcessor.getChal(syncml->getSyncBody());

        if (isAuthFailed(ret)) {
            if (clientChal == NULL) {
                requestedAuthType = credentialHandler.getClientAuthType();
            } else {
                requestedAuthType = clientChal->getType();
            }
            if (strcmp(credentialHandler.getClientAuthType(),requestedAuthType) != 0 ) {
                if (clientChal && strcmp(requestedAuthType, AUTH_TYPE_MD5) == 0) {
                    if (clientChal->getNextNonce()) {
                        credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                    }
                }
            } else {
                if (strcmp(requestedAuthType, AUTH_TYPE_MD5) == 0 && clientAuthRetries == 1)  {
                    if (clientChal->getNextNonce()) {
                        credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                    }

                } else {
                    //lastErrorCode = 401;
                    //sprintf(lastErrorMsg, "Client not authenticated");
                    setError(401, "Client not authenticated");
                    ret = getLastErrorCode();
                    goto finally;
                }
            }
            credentialHandler.setClientAuthType(requestedAuthType);
            clientAuthRetries++;

       } else {
            if (clientChal && strcmp(clientChal->getType(), AUTH_TYPE_MD5) == 0) {
                if (clientChal->getNextNonce()) {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                }
            }
            isClientAuthenticated = true;

            // Get sorted source list from Alert commands sent by server.
            if (sortedSourcesFromServer) {
                delete [] sortedSourcesFromServer;
                sortedSourcesFromServer = NULL;
            }
            sortedSourcesFromServer = syncMLProcessor.getSortedSourcesFromServer(syncml, sourcesNumber);

            for (count = 0; count < sourcesNumber; count ++) {
                if (!sources[count]->getReport()->checkState())
                    continue;
                ret = syncMLProcessor.processServerAlert(*sources[count], syncml);
                if (isErrorStatus(ret)) {
                    setErrorF(ret, "AlertStatus from server %d", ret);
                    LOG.error("%s", getLastErrorMsg());
                    setSourceStateAndError(count, SOURCE_ERROR, ret, getLastErrorMsg());
                }
                fireSyncSourceEvent(sources[count]->getConfig().getURI(),
                                    sources[count]->getConfig().getName(),
                                    sources[count]->getSyncMode(),
                                    0, SYNC_SOURCE_SYNCMODE_REQUESTED);
            }
       }

    } while(isClientAuthenticated == false || isServerAuthenticated == false);

    config.setClientNonce(credentialHandler.getClientNonce());
    config.setServerNonce(credentialHandler.getServerNonce());
    config.setDevInfHash(devInfHash);

    if (isToExit()) {
        // error. no source to sync
        if (!ret) {
            // error: no source to sync
            //ret = lastErrorCode = ERR_NO_SOURCE_TO_SYNC;
            //sprintf(lastErrorMsg, ERRMSG_NO_SOURCE_TO_SYNC);
            setError(ERR_NO_SOURCE_TO_SYNC, ERRMSG_NO_SOURCE_TO_SYNC);
            ret = getLastErrorCode();
        }

        goto finally;
    }

    currentState = STATE_PKG1_SENT;


// ---------------------------------------------------------------------------------------
finally:

    if(ret) {
        //Fire Sync Error Event
        fireSyncEvent(getLastErrorMsg(), SYNC_ERROR);
    }

    if (respURI) {
        delete [] respURI;
    }
    if (responseMsg) {
        safeDelete(&responseMsg);
    }
    if (initMsg) {
        safeDelete(&initMsg);
    }
    if (devInfStr) {
        delete devInfStr;
    }

    deleteSyncML(&syncml);
    deleteCred(&cred);
    deleteAlert(&alert);
    deleteStatus(&status);
    deleteChal(&serverChal);
    return ret;
}

//
// utility function to process any <Sync> command that the server might
// have included in its <SyncBody>
//
// @param syncml       the server response
// @param statusList   list to which statuses for changes are to be added
// @return true if a fatal error occurred
//
bool SyncManager::checkForServerChanges(SyncML* syncml, ArrayList &statusList)
{
    bool result = false;

    // Danger, danger: count is a member variable!
    // It has to be because that's the context for some of
    // the other methods. Modifying it has to be careful to
    // restore the initial value before returning because
    // our caller might use it, too.
    int oldCount = this->count;


    //
    // Get the server modifications for each syncsource.
    // We need to work on syncsources in the same order as the server sends them.
    // (use 'sortedSourcesFromServer' list of source names)
    //
    char* sourceUri = NULL;
    int i=0;
    while (sortedSourcesFromServer[i]) {

        sourceUri = sortedSourcesFromServer[i];

        // Retrieve the correspondent index for this syncsource.
        for (count = 0; count < sourcesNumber; count ++) {
            if ( !strcmp(sourceUri, sources[count]->getConfig().getName()) ) {
                break;
            }
        }
        if (count >= sourcesNumber) {
            LOG.error("Source uri not recognized: %s", sourceUri);
            goto finally;
        }

        Sync* sync = syncMLProcessor.getSyncResponse(syncml, i);

        if (sync) {
            const char *locuri = ((Target*)(sync->getTarget()))->getLocURI();

            for (int k = 0; k < sourcesNumber; k++) {
                if (strcmp(locuri, sources[k]->getConfig().getName()) == 0) {
                    count = k;
                    break;
                }
            }
            if (count >= sourcesNumber) {
                LOG.error("Source uri not recognized: %s", sourceUri);
                goto finally;
            }

            if (!sources[count]->getReport()->checkState()) {
                i++;
                continue;
            }

            if (strcmp(prevSourceName, "") == 0) {
                strcpy(prevSourceName, locuri);
            }
            if (strcmp(prevSourceName, locuri) != 0) {
                isFiredSyncEventBEGIN = false;
                fireSyncSourceEvent(prevSourceUri, prevSourceName, prevSyncMode, 0, SYNC_SOURCE_END);
                strcpy(prevSourceName, locuri);
            }
        }

        if (sync) {
            // Fire SyncSource event: BEGIN sync of a syncsource (server modifications)
            // (fire only if <sync> tag exist)
            if (isFiredSyncEventBEGIN == false) {
                fireSyncSourceEvent(sources[count]->getConfig().getURI(),
                        sources[count]->getConfig().getName(),
                        sources[count]->getSyncMode(), 0, SYNC_SOURCE_BEGIN);

                strcpy(prevSourceUri,  sources[count]->getConfig().getURI());
                prevSyncMode = sources[count]->getSyncMode();

                long noc = sync->getNumberOfChanges();
                fireSyncSourceEvent(sources[count]->getConfig().getURI(),
                        sources[count]->getConfig().getName(),
                        sources[count]->getSyncMode(), noc, SYNC_SOURCE_TOTAL_SERVER_ITEMS);

                isFiredSyncEventBEGIN = true;
            }

            ArrayList* items = sync->getCommands();
            Status* status = syncMLBuilder.prepareSyncStatus(*sources[count], sync);
            statusList.add(*status);
            deleteStatus(&status);

            ArrayList previousStatus;
            for (int i = 0; i < items->size(); i++) {
                CommandInfo cmdInfo;
                ModificationCommand* modificationCommand = (ModificationCommand*)(items->get(i));
                Meta* meta = modificationCommand->getMeta();
                ArrayList* list = modificationCommand->getItems();

                cmdInfo.commandName = modificationCommand->getName();
                cmdInfo.cmdRef = modificationCommand->getCmdID()->getCmdID();

                if (meta) {
                    cmdInfo.dataType = meta->getType();
                    cmdInfo.format = meta->getFormat();
                    cmdInfo.size = meta->getSize();
                }
                else {
                    cmdInfo.dataType = 0;
                    cmdInfo.format = 0;
                    cmdInfo.size = 0;
                }

                for (int j = 0; j < list->size(); j++) {
                    Item *item = (Item*)list->get(j);
                    if (item == NULL) {
                        LOG.error("SyncManager::checkForServerChanges() - unexpected NULL item.");
                        result = true;
                        goto finally;
                    }
                    // Size might have been included in either the command or the item meta information.
                    // The check for size > 0 is necessary because the function returns 0 even if no
                    // size information was sent by the server - that's okay, for items that really have
                    // size 0 the value doesn't matter as they shouldn't be split into chunks.
                    Meta *itemMeta = item->getMeta();
                    if (itemMeta && itemMeta->getSize() > 0) {
                        cmdInfo.size = itemMeta->getSize();
                    }

                    //
                    // set the syncItem element
                    //
                    status = processSyncItem(item, cmdInfo, syncMLBuilder);

                    if (status) {
                        syncMLBuilder.addItemStatus(&previousStatus, status);
                        deleteStatus(&status);
                    }
                }

                statusList.add(&previousStatus);
                previousStatus.clear();
            }
        // Fire SyncSourceEvent: END sync of a syncsource (server modifications)
        //fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_END);
            previousStatus.clear();
        }
        i++;
    } // End: while (sortedSourcesFromServer[i])


  finally:
    this->count = oldCount;
    return result;
}


/*
 * Starts the synchronization phase
 */
int SyncManager::sync() {

    char* msg            = NULL;
    char* responseMsg    = NULL;
    Status* status       = NULL;
    SyncML* syncml       = NULL;
    /** Current item to be transmitted. Might be split across multiple messages if LargeObjectSupport is on. */
    SyncItem* syncItem   = NULL;
    /** number of bytes already transmitted from syncItem */
    long syncItemOffset  = 0;
    Alert* alert         = NULL;
    ModificationCommand* modificationCommand = NULL;
    unsigned int tot     = 0;
    unsigned int step    = 0;
    unsigned int toSync  = 0;
    unsigned int iterator= 0;
    int ret              = 0;
    bool last            = false;
    //ArrayList* list      = new ArrayList();
    bool isFinalfromServer = false;
    bool isAtLeastOneSourceCorrect = false;

    //
    // If this is the first message, currentState is STATE_PKG1_SENT,
    // otherwise it is already in STATE_PKG3_SENDING.
    //
    if (currentState == STATE_PKG1_SENT) {
        currentState = STATE_PKG3_SENDING;
    }

    // The real number of source to sync (XXX REMOVE ME)
    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport()->checkState())
            continue;
        toSync++;
    }

    for (count = 0; count < sourcesNumber; count ++) {
        
        if (!sources[count]->getReport()->checkState())
            continue;

        // note: tot == 0 is used to detect when to start iterating over
        // items from the beginning
        tot  = 0;
        step = 0;
        last = false;
        iterator++;

        // Fire SyncSource event: BEGIN sync of a syncsource (client modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_BEGIN);

        if ( sources[count]->beginSync() ) {
            // Error from SyncSource
            //lastErrorCode = ERR_UNSPECIFIED;
            setError(ERR_UNSPECIFIED, "");
            ret = getLastErrorCode();
            // syncsource should have set its own errors. If not, set default error.
            if (sources[count]->getReport()->checkState()) {
                setSourceStateAndError(count, SOURCE_ERROR, ERR_UNSPECIFIED, "Error in begin sync");
            }
            continue;
        }
        else {
            isAtLeastOneSourceCorrect = true;
        }

        // keep sending changes for current source until done with it
        do {
            if (modificationCommand) {
                delete modificationCommand;
                modificationCommand = NULL;
            }

            if (commands.isEmpty()) {

                status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
                commands.add(*status);
                deleteStatus(&status);

            }
            
            /*
            * Check if there is some mappings from the previous 
            * sync of the source. This is valid only for two-way sync
            * and one-way. Otherwise the old mappings are removed
            */
            switch (sources[count]->getSyncMode()) {
                case SYNC_TWO_WAY:
                case SYNC_ONE_WAY_FROM_SERVER: {
                    Enumeration& en = mmanager[count]->getMappings();
                    ArrayList tmpMapItems;            
                    while(en.hasMoreElement()) {
                        KeyValuePair* kvp = (KeyValuePair*)en.getNextElement();
                        SyncMap sMap(kvp->getValue(), kvp->getKey());
                        MapItem* mapItem = syncMLBuilder.prepareMapItem(&sMap);
                        tmpMapItems.add(*mapItem);                
                        delete mapItem;
                    }
                    if (tmpMapItems.size() > 0) {
                        Map* tmpMap = syncMLBuilder.prepareMapCommand(*sources[count]);
                        tmpMap->getMapItems()->add(&tmpMapItems); 
                        commands.add(*tmpMap);
                        delete tmpMap;
                    }
                }
                break;
                default: {
                    mmanager[count]->resetMappings();
                }
            }

            // Accumulate changes for the current sync source until the maxMsgSize
            // is reached.
            //
            // In each loop iteration at least one change must be sent to ensure 
            // progress.
            // Keeping track of the current message size is a heuristic which 
            // assumes a constant overhead for each message and change item 
            // and then adds the actual item data sent.
            deleteSyncML(&syncml);
            static long changeOverhead = 150;
            long msgSize = 0;
            Sync* sync = syncMLBuilder.prepareSyncCommand(*sources[count]);
            ArrayList* list = new ArrayList();

            switch (sources[count]->getSyncMode()) {
                case SYNC_SLOW:
                    {
                        if (syncItem == NULL) {
                            if (tot == 0) {
                                syncItem = getItem(*sources[count], &SyncSource::getFirstItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = getItem(*sources[count], &SyncSource::getNextItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }

                            if (tot &&
                                maxMsgSize &&
                                syncItem &&
                                msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                // avoid adding another item that exceeds the message size
                                break;
                            }

                            msgSize += changeOverhead;
                            msgSize +=
                                syncMLBuilder.addItem(modificationCommand,
                                                      syncItemOffset,
                                                      (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                      REPLACE_COMMAND_NAME,
                                                      syncItem,
                                                      sources[count]->getConfig().getType());

                            if (syncItem) {
                                if (syncItemOffset == syncItem->getDataSize()) {
                                    // the item is only the pointer not another instance. to save mem
                                    delete syncItem; syncItem = NULL;
                                } else {
                                    assert(msgSize >= maxMsgSize);
                                    break;
                                }
                            }
                            else {
                                last = true;
                                break;
                            }
                            tot++;
                        } while(msgSize < maxMsgSize);
                    }
                    break;

                case SYNC_REFRESH_FROM_SERVER:
                    {
                        last = true;
                        char *name = toMultibyte(sources[count]->getName());
                        if (sources[count]->removeAllItems() == 0) {
                            LOG.debug("Removed all items for source %s", name);
                        } else {
                            LOG.error("Error removing all items for source %s", name);
                        }
                        delete [] name;
                    }
                    break;
                    
                case SYNC_ONE_WAY_FROM_SERVER:
                    last = true;
                    break;

                case SYNC_REFRESH_FROM_CLIENT:
                    {
                        if (syncItem == NULL) {
                            if (tot == 0) {
                                syncItem = getItem(*sources[count], &SyncSource::getFirstItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(),
                                                      sources[count]->getConfig().getName(),
                                                      syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = getItem(*sources[count], &SyncSource::getNextItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(),
                                                      sources[count]->getConfig().getName(),
                                                      syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }

                            if (tot &&
                                maxMsgSize &&
                                syncItem &&
                                msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                // avoid adding another item that exceeds the message size
                                break;
                            }

                            msgSize += changeOverhead;
                            msgSize +=
                                syncMLBuilder.addItem(modificationCommand,
                                                      syncItemOffset,
                                                      (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                      REPLACE_COMMAND_NAME, syncItem,
                                                      sources[count]->getConfig().getType());

                            if (syncItem) {
                                if (syncItemOffset == syncItem->getDataSize()) {
                                    delete syncItem; syncItem = NULL;// the item is only the pointer not another instance. to save mem
                                } else {
                                    assert(msgSize >= maxMsgSize);
                                    break;
                                }
                            }
                            else {
                                last = true;
                                break;
                            }
                            tot++;
                        } while(msgSize < maxMsgSize);
                    }
                    break;

                default:
                    {
                        tot = 0;
                        //
                        // New Item
                        //
                        if (step == 0) {
                            assert(syncItem == NULL);
                            syncItem = getItem(*sources[count], &SyncSource::getFirstNewItem);
                            syncItemOffset = 0;
                            step++;
                            if (syncItem == NULL)
                                step++;
                        }
                        if (step == 1) {
                            do {
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextNewItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }

                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          ADD_COMMAND_NAME,
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - New Item Detected
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), syncItem->getKey(), ITEM_ADDED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        assert(msgSize >= maxMsgSize);
                                        break;
                                    }
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                            } while(msgSize < maxMsgSize);
                        }

                        //
                        // Updated Item
                        //
                        if (step == 2) {

                            if (modificationCommand) {
                                list->add(*modificationCommand);
                                delete modificationCommand;
                                modificationCommand = NULL;
                            }

                            assert(syncItem == NULL);
                            syncItem = getItem(*sources[count], &SyncSource::getFirstUpdatedItem);
                            syncItemOffset = 0;

                            step++;
                            if (syncItem == NULL)
                                step++;

                        }
                        if (step == 3) {
                            do {
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextUpdatedItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }


                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          REPLACE_COMMAND_NAME,
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - Item Updated
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        assert(msgSize >= maxMsgSize);
                                        break;
                                    }
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                            } while( msgSize < maxMsgSize);
                        }

                        //
                        // Deleted Item
                        //
                        if (step == 4) {

                            if (modificationCommand) {
                                list->add(*modificationCommand);
                                delete modificationCommand;
                                modificationCommand = NULL;
                            }

                            syncItem = getItem(*sources[count], &SyncSource::getFirstDeletedItem);
                            syncItemOffset = 0;

                            step++;
                            if (syncItem == NULL)
                                step++;
                        }
                        if (step == 5) {
                            do {
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextDeletedItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }

                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          DELETE_COMMAND_NAME,
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - Item Deleted
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), syncItem->getKey(), ITEM_DELETED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        assert(msgSize >= maxMsgSize);
                                        break;
                                    }
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                            } while(msgSize < maxMsgSize);
                        }
                        if (step == 6 && syncItem == NULL)
                            last = true;

                        break;
                    }
            }

            if (modificationCommand) {
                list->add(*modificationCommand);
                delete modificationCommand;
                modificationCommand = NULL;
            }
            sync->setCommands(list);
            delete list;
            commands.add(*sync);
            delete sync;

            //
            // Check if all the sources were synced.
            // If not the prepareSync doesn't use the <final/> tag
            //
            syncml = syncMLBuilder.prepareSyncML(&commands, (iterator != toSync ? false : last));
            msg    = syncMLBuilder.prepareMsg(syncml);

            deleteSyncML(&syncml);
            commands.clear();

            if (msg == NULL) {
                ret = getLastErrorCode();
                goto finally;
            }

            // Synchronization message:
            long realMsgSize = strlen(msg);
            LOG.debug("%s estimated size %ld, allowed size %ld, real size %ld / estimated size %ld = %ld%%",
                      MSG_MODIFICATION_MESSAGE,
                      msgSize, maxMsgSize, realMsgSize, msgSize,
                      msgSize ? (100 * realMsgSize / msgSize) : 100);

            //Fire Modifications Event
            fireSyncEvent(NULL, SEND_MODIFICATION);

            responseMsg = transportAgent->sendMessage(msg);
            if (responseMsg == NULL) {
                ret=getLastErrorCode();
                goto finally;
            }

            // reset the mappings if any. This action is done everytime...
            mmanager[count]->resetMappings();

            // increment the msgRef after every send message
            syncMLBuilder.increaseMsgRef();
            syncMLBuilder.resetCommandID();

            syncml = syncMLProcessor.processMsg(responseMsg);
            safeDelete(&responseMsg);
            safeDelete(&msg);

            if (syncml == NULL) {
                ret = getLastErrorCode();
                goto finally;
            }

            isFinalfromServer = syncml->isLastMessage();
            ret = syncMLProcessor.processSyncHdrStatus(syncml);
            if (isErrorStatus(ret)) {
                //lastErrorCode = ret;
                //sprintf(lastErrorMsg, "Server Failure: server returned error code %i", ret);
                setErrorF(ret, "Server Failure: server returned error code %i", ret);
                LOG.error("%s", getLastErrorMsg());
                goto finally;

            }
            ret = 0;
            //
            // Process the status of the item sent by client. It invokes the
            // source method
            //
            int itemret = syncMLProcessor.processItemStatus(*sources[count], syncml->getSyncBody());
            if(itemret){
                char *name = toMultibyte(sources[count]->getName());
                LOG.error("Error #%d in source %s", itemret, name);
                delete [] name;
                // skip the source, and set an error
                setSourceStateAndError(count, SOURCE_ERROR, itemret, getLastErrorMsg());
                //lastErrorCode = itemret;
                setError(itemret, "");
                break;
            }

            // Fire SyncSourceEvent: END sync of a syncsource (client modifications)
            if (last)
                fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_END);

            // The server might have included a <Sync> command without waiting
            // for a 222 alert. If it hasn't, then nothing is done here.
            ArrayList statusList;
            if (checkForServerChanges(syncml, statusList)) {
                goto finally;
            }
            if (statusList.size()) {
                Status* status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
                commands.add(*status);
                deleteStatus(&status);
                commands.add(&statusList);
            }

            // deleteSyncML(&syncml);

        } while (last == false);

        // Fire SyncSourceEvent: END sync of a syncsource (client modifications)
        // fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_END);

    } // end for (count = 0; count < sourcesNumber; count ++)

    if (isToExit()) {
        // error. no source to sync
        ret = getLastErrorCode();
        goto finally;
    }

    deleteSyncML(&syncml);

    //
    // If this was the last chunk, we move the state to STATE_PKG3_SENT
    // At this time "last" is always true. The client is going to send
    // the 222 package for to get the server modification if at least a source is correct
    //
    last = true;
    currentState = STATE_PKG3_SENT;

    //
    // send 222 alert code to retrieve the item from server
    //
    if ( !isFinalfromServer && isAtLeastOneSourceCorrect ) {
        status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
        commands.add(*status);
        deleteStatus(&status);
        for (count = 0; count < sourcesNumber; count ++) {
            if(!sources[count]->getReport()->checkState()) {
                continue;
            }
            if ((sources[count]->getSyncMode() != SYNC_ONE_WAY_FROM_CLIENT) &&
                (sources[count]->getSyncMode() != SYNC_REFRESH_FROM_CLIENT))
            {
                alert = syncMLBuilder.prepareAlert(*sources[count]);
                commands.add(*alert);
                deleteAlert(&alert);
            }
        }

        syncml = syncMLBuilder.prepareSyncML(&commands, false);
        msg    = syncMLBuilder.prepareMsg(syncml);

        LOG.debug("Alert to request server changes");

        responseMsg = transportAgent->sendMessage(msg);
        if (responseMsg == NULL) {
            LOG.debug("SyncManager::sync(): null responseMsg");
            ret=getLastErrorCode();
            goto finally;
        }

        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        deleteSyncML(&syncml);
        safeDelete(&msg);

        syncml = syncMLProcessor.processMsg(responseMsg);
        safeDelete(&responseMsg);
        commands.clear();

        if (syncml == NULL) {
            LOG.debug("SyncManager::sync(): null syncml");
            ret = getLastErrorCode();
            goto finally;
        }
        ret = syncMLProcessor.processSyncHdrStatus(syncml);
        if (isErrorStatus(ret)) {
            //lastErrorCode = ret;
            //sprintf(lastErrorMsg, "Server Failure: server returned error code %i", ret);
            setErrorF(ret, "Server Failure: server returned error code %i", ret);
            LOG.error("%s", getLastErrorMsg());
            goto finally;
        }
        ret = 0;

        //
        // Process the items returned from server
        //

        do {
            last = syncml->getSyncBody()->getFinalMsg();
            ArrayList statusList;

            status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
            commands.add(*status);
            deleteStatus(&status);

            if (checkForServerChanges(syncml, statusList)) {
                goto finally;
            }

            commands.add(&statusList);

            // Add any map command pending for the active sources
            for(int i=0; i<sourcesNumber; i++) {
                addMapCommand(i);
                // finalize the mapping in the storage
                mmanager[i]->closeMappings();
            }                       

            if (!last) {
                deleteSyncML(&syncml);
                syncml = syncMLBuilder.prepareSyncML(&commands, last);
                msg    = syncMLBuilder.prepareMsg(syncml);

                LOG.debug("Status to the server");

                responseMsg = transportAgent->sendMessage(msg);
                if (responseMsg == NULL) {
                    ret=getLastErrorCode();
                    goto finally;
                }
                
                // reset the mappings in the storage
                for(int i=0; i<sourcesNumber; i++) {
                    mmanager[i]->resetMappings();
                } 

                // increment the msgRef after every send message
                syncMLBuilder.increaseMsgRef();
                syncMLBuilder.resetCommandID();

                deleteSyncML(&syncml);
                safeDelete(&msg);

                syncml = syncMLProcessor.processMsg(responseMsg);
                safeDelete(&responseMsg);
                commands.clear();
                if (syncml == NULL) {
                    ret = getLastErrorCode();
                    goto finally;
                }
                ret = syncMLProcessor.processSyncHdrStatus(syncml);

                if (isErrorStatus(ret)) {
                    //lastErrorCode = ret;
                    //sprintf(lastErrorMsg, "Server Failure: server returned error code %i", ret);
                    setErrorF(ret, "Server Failure: server returned error code %i", ret);
                    LOG.error("%s", getLastErrorMsg());
                    goto finally;
                }
                ret = 0;
            }
        } while (last == false);
    }


finally:

    if (isAtLeastOneSourceCorrect == true)
    {
        fireSyncSourceEvent(prevSourceUri, prevSourceName, prevSyncMode, 0, SYNC_SOURCE_END);
        safeDelete(&responseMsg);
        safeDelete(&msg);
        deleteSyncML(&syncml);
    }
    else
    {
        ret = -1;
        LOG.debug("sources not available");
    }

    if (ret) {
        //Fire Sync Error Event
        fireSyncEvent(getLastErrorMsg(), SYNC_ERROR);
    }
    return ret;
}

/* 
 * Adds the Map command to the commands list if needed.
 * Updates the class member 'commands'.
 */
void SyncManager::addMapCommand(int sourceIndex) {    
    Map* map = NULL;
    Enumeration& en = mmanager[sourceIndex]->getMappings();
    while (en.hasMoreElement()) {
        if (map == NULL) {
            map = syncMLBuilder.prepareMapCommand(*sources[sourceIndex]);
        } 
        KeyValuePair* kvp = (KeyValuePair*)en.getNextElement();                
        SyncMap sMap(kvp->getValue(), kvp->getKey());
        MapItem* mapItem = syncMLBuilder.prepareMapItem(&sMap);
        syncMLBuilder.addMapItem(map, mapItem);
        delete mapItem;
    }
    if (map) {
        // Add it to the list
        commands.add(*map);
        delete map;
    }
    
}


int SyncManager::endSync() {

    char* mapMsg            = NULL;
    char* responseMsg       = NULL;
    SyncML*  syncml         = NULL;
    int ret                 = 0;
    Status* status          = NULL;
    ArrayList* list         = new ArrayList();
    bool msgToSend          = false;


    //
    // Add map commands for all active sources to syncml object.
    //
    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport()->checkState()) {
            continue;
        }
        
        if (  (sources[count]->getSyncMode() == SYNC_ONE_WAY_FROM_CLIENT &&
                commands.isEmpty() && (mmanager[count]->getMappings().hasMoreElement() == false)) ||
                (sources[count]->getSyncMode() == SYNC_REFRESH_FROM_CLIENT &&
                commands.isEmpty() && (mmanager[count]->getMappings().hasMoreElement() == false))
                ) {
            // No need to send the final msg (no mapping).
        }
        else {
            // At least one source, so we'll send the final msg.
            msgToSend = true;

            if (commands.isEmpty()) {
                status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
                commands.add(*status);
                deleteStatus(&status);
            }
            // @@ No more used when inserting the new mapping...
            // Add any map command pending for this source 
            // addMapCommand(count);            

            // finalize the mapping in the storage
            mmanager[count]->closeMappings();
        }
    }


    //
    // Send the final message with mappings.
    //
    if (msgToSend) {
        syncml = syncMLBuilder.prepareSyncML(&commands, true);
        mapMsg = syncMLBuilder.prepareMsg(syncml);

        LOG.debug("Mapping");

        //Fire Finalization Event
        fireSyncEvent(NULL, SEND_FINALIZATION);

        responseMsg = transportAgent->sendMessage(mapMsg);        
        if (responseMsg == NULL) {
            ret=getLastErrorCode();
            goto finally;
        }
        for (count = 0; count < sourcesNumber; count ++) {
            if (!sources[count]->getReport()->checkState()) {
                continue;
            }
            mmanager[count]->resetMappings();
        }
            
        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        deleteSyncML(&syncml);
        safeDelete(&mapMsg);

        syncml = syncMLProcessor.processMsg(responseMsg);
        delete [] responseMsg; responseMsg = NULL;
        commands.clear();

        if (syncml == NULL) {
            ret = getLastErrorCode();
            goto finally;
        }
        ret = syncMLProcessor.processSyncHdrStatus(syncml);

        if (isErrorStatus(ret)) {
            //lastErrorCode = ret;
            //sprintf(lastErrorMsg, "Server Failure: server returned error code %i", ret);
            setErrorF(ret,  "Server Failure: server returned error code %i", ret);
            LOG.error("%s", getLastErrorMsg());
            goto finally;
        }
        ret = 0;

        //
        // Process the status of mapping
        //
        ret = syncMLProcessor.processMapResponse(*sources[count], syncml->getSyncBody());
        deleteSyncML(&syncml);
        if (ret == -1) {
            ret = getLastErrorCode();
            goto finally;
        }
    }


    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport()->checkState()) {
            continue;
        }

        int sret = sources[count]->endSync();
        if (sret) {
            setErrorF(sret, "Error in endSync of source '%ls'", sources[count]->getName());
        }
    }

 finally:

    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport()->checkState()) {
            LOG.debug("The source %s got and error %i: %s", 
                            _wcc(sources[count]->getName()), 
                            sources[count]->getReport()->getLastErrorCode(),
                            sources[count]->getReport()->getLastErrorMsg());  
            LOG.debug("The old last value is committed (no changed)");
            continue;

        }

        commitChanges(*sources[count]);
    }

    config.setEndSync((unsigned long)time(NULL));
    safeDelete(&responseMsg);
    safeDelete(&mapMsg);
    if (list){
        delete list;
        list = NULL;
    }
    LOG.debug("ret: %i, lastErrorCode: %i, lastErrorMessage: %s",
              ret, getLastErrorCode(), getLastErrorMsg());

    // Fire Sync End Event
    fireSyncEvent(NULL, SYNC_END);
    
    if (ret){
        //Fire Sync Error Event
        fireSyncEvent(getLastErrorMsg(), SYNC_ERROR);

        return ret;
    }
    else if (getLastErrorCode()){
        return getLastErrorCode();
    }
    else {
        LOG.info("Sync successfully completed.");
        return 0;
    }
}

bool SyncManager::readSyncSourceDefinition(SyncSource& source) {
    char anchor[DIM_ANCHOR];

    
    if (config.getAbstractSyncSourceConfig(_wcc(source.getName())) == NULL) {
        return false;
    }

    AbstractSyncSourceConfig& ssc(source.getConfig());

    // only copy properties which either have a different format
    // or are expected to change during the synchronization
    timestampToAnchor(ssc.getLast(), anchor);
    source.setLastAnchor(anchor);
    timestampToAnchor(source.getNextSync(), anchor);
    source.setNextAnchor(anchor);

    return true;
}


bool SyncManager::commitChanges(SyncSource& source) {
    const char* name = _wcc(source.getName());
    AbstractSyncSourceConfig* ssconfig = config.getAbstractSyncSourceConfig(name);

    if (ssconfig) {
        char anchor[DIM_ANCHOR];
        unsigned long next = source.getNextSync();
        timestampToAnchor(next, anchor);
        LOG.debug(DBG_COMMITTING_SOURCE, name, anchor);
        ssconfig->setLast(next);
        return true;
    } else {
        return false;
    }
}

/**
 * This method copies the valid sources into the member <code>sources</code>.
 * The check done before the source is put in the list are:
 * - must have a SyncSourceReport
 * - must have a AbstractSyncSourceConfig
 * - the preferred syncmode must be different from SYNC_NONE
 *
 * Sets lastErrorCode.

 * @return the number of active sources
 */
int SyncManager::assignSources(SyncSource** srclist) {

    int n = 0, i = 0, activeSources = 0;

    if (!srclist) {
        return 0;
    }

    // Count sources
    while (srclist[n]) n++;

    // Allocate the array (use max value anyway)
    this->sources = new SyncSource*[n+1];

    // Copy source pointers (only valid ones)
    for (i = 0; i < n; i++) {
        const char* name = srclist[i]->getConfig().getName();
        // Check valid report
        if ( !srclist[i]->getReport() ) {
            LOG.error("No SyncSourceReport for source: %s", name);
            continue;
        }
        // Check valid config
        if ( !readSyncSourceDefinition(*srclist[i]) ) {

            //lastErrorCode = ERR_SOURCE_DEFINITION_NOT_FOUND;
            //sprintf(lastErrorMsg, ERRMSG_SOURCE_DEFINITION_NOT_FOUND, name);
            setErrorF(ERR_SOURCE_DEFINITION_NOT_FOUND, ERRMSG_SOURCE_DEFINITION_NOT_FOUND, name);
            LOG.debug("%s", getLastErrorMsg());

            setSourceStateAndError(i, SOURCE_ERROR,
                                   ERR_SOURCE_DEFINITION_NOT_FOUND, getLastErrorMsg());
            continue;
        }
        // Check source active
        if (srclist[i]->getPreferredSyncMode() != SYNC_NONE) {
            srclist[i]->getReport()->setState(SOURCE_ACTIVE);
            // Ok, store the source in the active list
            this->sources[activeSources++] = srclist[i];
        }
    }
    this->sources[activeSources] = 0;   // Terminate the array

    return activeSources;
}

SyncItem* SyncManager::getItem(SyncSource& source, SyncItem* (SyncSource::* getItemFunction)()) {
    SyncItem *syncItem = (source.*getItemFunction)();

    if (!syncItem) {
        return NULL;
    }

    // change encoding automatically?
    const char* encoding   = source.getConfig().getEncoding();
    const char* encryption = source.getConfig().getEncryption();
    if (!syncItem->getDataEncoding()) {
        if ( (encoding && encoding[0]) || (encryption && encryption[0]) ) {
            if (syncItem->changeDataEncoding(encoding, encryption, credentialInfo)) {
                LOG.error("Error: invalid encoding for item: %" WCHAR_PRINTF ,
                    syncItem->getKey());
                delete syncItem;
                syncItem = NULL;
            }
        }
    }

    // the client might have used a key which is not safe for SyncML, encode it if necessary
    encodeItemKey(syncItem);

    return syncItem;
}

Status *SyncManager::processSyncItem(Item* item, const CommandInfo &cmdInfo, SyncMLBuilder &syncMLBuilder)
{
    int code = 0;
    const char* itemName;
    Status *status = 0;

    // During an "Add" we want the source URI, otherwise
    // ("Delete", "Update") the target URI.
    Target* t = item->getTarget();
    if (t && strcmp(cmdInfo.commandName, ADD)) {
        itemName = t->getLocURI();
    }
    else {
        Source* s = item->getSource();
        itemName = s->getLocURI();
    }

    if (!itemName) {
        // invalid command: no URI
        status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, "", cmdInfo.cmdRef, 412);
        return status;
    }

    // Fill item -------------------------------------------------
    WCHAR *iname = toWideChar(itemName);
    bool append = true;
    if (incomingItem) {
        bool newItem = false;

        if (iname) {
            if (incomingItem->getKey()) {
                if(wcscmp(incomingItem->getKey(), iname)) {
                    // another item before old one is complete
                    newItem = true;
                }
            } else {
                incomingItem->setKey(iname);
            }
        }

        // if no error yet, also check for the same command and same source
        if (incomingItem->cmdName.c_str() &&
            strcmp(incomingItem->cmdName.c_str(), cmdInfo.commandName) ||
            count != incomingItem->sourceIndex) {

            newItem = true;
        }
        if (newItem) {
            // send 223 alert:
            //
            // "The Alert should contain the source and/or target information from
            // the original command to enable the sender to identify the failed command."
            //
            // The target information is the one from the item's source.
            Alert *alert = syncMLBuilder.prepareAlert(*sources[incomingItem->sourceIndex], 223);
            commands.add(*alert);
            delete alert;

            delete incomingItem;
            incomingItem = NULL;
        }
    } else {
        incomingItem = new IncomingSyncItem(iname, cmdInfo, count);

        // incomplete item?
        if (item->getMoreData()) {
            // reserve buffer in advance, append below
            long size = cmdInfo.size;
            if (size < 0 || maxObjSize && size > maxObjSize) {
                // invalid size, "Request entity too large"
                status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 416);
                delete incomingItem;
                incomingItem = NULL;
            } else {
                incomingItem->setData(NULL, getToleranceDataSize(size));
            }
        } else {
            // simply copy all data below
            append = false;
        }
    }
    delete [] iname;

    if (incomingItem) {
        ComplexData *cdata = item->getData();
        if (cdata) {
            const char* data = cdata->getData();
            const char* format = 0;

            //
            // Retrieving how the content has been encoded.
            // Remember that in the item and convert it once it is
            // complete.
            //
            if (cmdInfo.format) {
                format = cmdInfo.format;
            }
            else {
                Meta* m = item->getMeta();
                if (m) {
                    format = m->getFormat();
                }
            }
            if (format && !incomingItem->getDataEncoding()) {
                incomingItem->setDataEncoding(format);
            }

            // append or set new data
            long size = strlen(data);
            if (append) {
                if (size + incomingItem->offset > incomingItem->getDataSize()) {
                    // overflow, signal error: "Size mismatch"
                    status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 424);
                    //sprintf(lastErrorMsg, "Item size mismatch: real size = %d, declared size = %d", size + incomingItem->offset, incomingItem->getDataSize());
                    //lastErrorCode = OBJECT_SIZE_MISMATCH;
                    setErrorF(OBJECT_SIZE_MISMATCH , "Item size mismatch: real size = %d, declared size = %d", (size + incomingItem->offset), (incomingItem->getDataSize()));
                    delete incomingItem;
                    incomingItem = NULL;
                } else {
                    memcpy((char *)incomingItem->getData() + incomingItem->offset, data, size);
                }
            } else {
                incomingItem->setData(data, size);
            }
            if (incomingItem) {
                incomingItem->offset += size;
            }
        }
    }

    if (incomingItem) {
        if (cmdInfo.dataType) {
            WCHAR *dtype = toWideChar(cmdInfo.dataType);
            incomingItem->setDataType(dtype);
            delete [] dtype;
        }
        WCHAR *sparent = toWideChar(item->getSourceParent());
        incomingItem->setSourceParent(sparent);
        delete [] sparent;
        WCHAR *tparent = toWideChar(item->getTargetParent());
        incomingItem->setTargetParent(tparent);
        delete [] tparent;

        incomingItem->setModificationTime(sources[count]->getNextSync());

        if (!item->getMoreData()) {


            if (append) {
                // Warning if data mismatch (only log).
                testIfDataSizeMismatch(incomingItem->getDataSize(), incomingItem->offset);
            }

            // Set the item size to the real size received.
            // (more space was allocated for the item data, to be tolerant to small
            // error in size declared by server)
            incomingItem->setDataSize(incomingItem->offset);

            // attempt to transform into plain format, if that fails let the client deal with
            // the encoded content
            incomingItem->changeDataEncoding(SyncItem::encodings::plain, NULL, credentialInfo);

            // Process item ------------------------------------------------------------
            if ( strcmp(cmdInfo.commandName, ADD) == 0) {
                // Fire Sync Item Event - New Item Added by Server
                fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), incomingItem->getKey(), ITEM_ADDED_BY_SERVER);

                incomingItem->setState(SYNC_STATE_NEW);
                code = sources[count]->addItem(*incomingItem);
                status = syncMLBuilder.prepareItemStatus(ADD, itemName, cmdInfo.cmdRef, code);

                // new client key might be unsafe, encode it in that case
                encodeItemKey(incomingItem);

                // Fire Sync Status Event: item status from client
                fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getName(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                // Update SyncReport
                sources[count]->getReport()->addItem(CLIENT, COMMAND_ADD, incomingItem->getKey(), status->getStatusCode(), NULL);

                // If the add was successful, set the id mapping
                if (code >= 200 && code <= 299) {
                    char *key = toMultibyte(incomingItem->getKey());
                    //SyncMap syncMap(item->getSource()->getLocURI(), key);
                    //mappings[count].add(syncMap);
                    mmanager[count]->addMapping(key, item->getSource()->getLocURI()); // LUID, GUID
                    delete [] key;
                }
            }
            else if (strcmp(cmdInfo.commandName, REPLACE) == 0) {
                // item key as stored on the server might have been encoded by library,
                // check that before passing to client
                decodeItemKey(incomingItem);

                // Fire Sync Item Event - Item Updated by Server
                fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), incomingItem->getKey(), ITEM_UPDATED_BY_SERVER);

                incomingItem->setState(SYNC_STATE_UPDATED);
                code = sources[count]->updateItem(*incomingItem);
                status = syncMLBuilder.prepareItemStatus(REPLACE, itemName, cmdInfo.cmdRef, code);

                // Fire Sync Status Event: item status from client
                fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getName(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                // Update SyncReport
                sources[count]->getReport()->addItem(CLIENT, COMMAND_REPLACE, incomingItem->getKey(), status->getStatusCode(), NULL);
            }
            else if (strcmp(cmdInfo.commandName, DEL) == 0) {
                // item key as stored on the server might have been encoded by library,
                // check that before passing to client
                decodeItemKey(incomingItem);

                // Fire Sync Item Event - Item Deleted by Server
                fireSyncItemEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), incomingItem->getKey(), ITEM_DELETED_BY_SERVER);

                incomingItem->setState(SYNC_STATE_DELETED);
                code = sources[count]->deleteItem(*incomingItem);
                status = syncMLBuilder.prepareItemStatus(DEL, itemName, cmdInfo.cmdRef, code);

                // Fire Sync Status Event: item status from client
                fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getName(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                // Update SyncReport
                sources[count]->getReport()->addItem(CLIENT, COMMAND_DELETE, incomingItem->getKey(), status->getStatusCode(), NULL);
            }

            delete incomingItem;
            incomingItem = NULL;
        }
        else {
            // keep the item, tell server "Chunked item accepted and buffered"
            status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 213);
        }
    }

    if (incomingItem) {
        // Make sure that more data comes by asking for it.
        // The standard says that if there are other commands, the 222 alert
        // may be omitted, but because that's hard to determine here we always
        // send it, just to be on the safe side.
        Alert *alert = syncMLBuilder.prepareAlert(*sources[count], 222);
        commands.add(*alert);
        delete alert;
    }

    return status;
}

/*
 * Creates the device info for this client and sources.
 */
DevInf *SyncManager::createDeviceInfo()
{
    const char *rxType, *rxVer, *txType, *txVer;

    // check that essential information is available
    // for each source
    for (SyncSource **source = sources;  *source;  source++) {

        rxType = (*source)->getConfig().getType();
        txType = (*source)->getConfig().getType();
        rxVer  = (*source)->getConfig().getVersion();
        txVer  = (*source)->getConfig().getVersion();

        if (!rxType || !rxVer || !txType || !txVer) {
            return NULL;
        }
    }

    DevInf *devinfo = new DevInf();
    //
    // Copy devInf params from current Config.
    //
    VerDTD v("1.2");
    devinfo->setVerDTD(&v);
    devinfo->setMan(config.getMan());
    devinfo->setMod(config.getMod());
    devinfo->setOEM(config.getOem());
    devinfo->setFwV(config.getFwv());
    devinfo->setSwV(config.getSwv());
    devinfo->setHwV(config.getHwv());
    devinfo->setDevID(config.getDevID());
    devinfo->setDevTyp(config.getDevType());
    devinfo->setUTC(config.getUtc());
    devinfo->setSupportLargeObjs(loSupport);
    devinfo->setSupportNumberOfChanges(config.getNocSupport());

    static const struct {
        SyncMode mode;
        int type;
    } mapping[] = {
        { SYNC_TWO_WAY, 1 },             // Support of 'two-way sync'
        { SYNC_SLOW, 2 },                // Support of 'slow two-way sync'
        { SYNC_ONE_WAY_FROM_CLIENT, 3 }, // Support of 'one-way sync from client only'
        { SYNC_REFRESH_FROM_CLIENT, 4 }, // Support of 'refresh sync from client only'
        { SYNC_ONE_WAY_FROM_SERVER, 5 }, // Support of 'one-way sync from server only'
        { SYNC_REFRESH_FROM_SERVER, 6 }, // Support of 'refresh sync from server only'
        // 7, // Support of 'server alerted sync'
        { SYNC_NONE, -1 }
    };

    ArrayList dataStores;

    for (unsigned int k=0; k < config.getAbstractSyncSourceConfigsCount(); k++) {
        AbstractSyncSourceConfig* ssconfig = config.getAbstractSyncSourceConfig(k);
        
        ArrayList syncModeList;
        const char *syncModes = ssconfig->getSyncModes();
        if (syncModes) {
            char buffer[80];
            const char *mode = syncModes;

            while (*mode) {
                // skip leading spaces and commas
                while (isspace(*mode) || *mode == ',') {
                    mode++;
                }
                // fast-forward to comma
                const char *eostr = mode;
                while (*eostr && *eostr != ',') {
                    eostr++;
                }
                // strip spaces directly before comma
                while (eostr > mode && isspace(eostr[-1])) {
                    eostr--;
                }
                // make temporary copy (mode is read-only)
                size_t len = eostr - mode;
                if (len > sizeof(buffer) - 1) {
                    len = sizeof(buffer) - 1;
                }
                memcpy(buffer, mode, sizeof(char) * len);
                buffer[len] = 0;
                SyncMode sm = syncModeCode(buffer);
                for (int i = 0; mapping[i].type >= 0; i++) {
                    if (mapping[i].mode == sm) {
                        SyncType syncType(mapping[i].type);
                        syncModeList.add(syncType);
                        break;
                    }
                }

                // next item
                mode = eostr;
            }
        }

        const char* name = ssconfig->getName();
        SourceRef sourceRef(name);

        rxType = ssconfig->getType();
        txType = ssconfig->getType();
        rxVer  = ssconfig->getVersion();
        txVer  = ssconfig->getVersion();

        ContentTypeInfo rxPref(rxType, rxVer);
        ArrayList rx;
        fillContentTypeInfoList(rx, ssconfig->getSupportedTypes());
        ContentTypeInfo txPref(txType, txVer);
        ArrayList tx;
        fillContentTypeInfoList(tx, ssconfig->getSupportedTypes());
        SyncCap syncCap(&syncModeList);
        DataStore dataStore(&sourceRef,
                            NULL,
                            -1,
                            &rxPref,
                            &rx,
                            &txPref,
                            &tx,
                            /* hack: DataStore is not const-clean, have to cast away the const here */
                            (ArrayList *)&(ssconfig->getCtCaps()),
                            NULL,
                            &syncCap);
        dataStores.add(dataStore);
    }
    devinfo->setDataStore(&dataStores);

    return devinfo;
}


/* Copy from AbstractSyncSourceConfig::getSupportedTypes() format into array list
 * of ContentTypeInfos.
 * @param types: formatted string of 'type:version' for each supported type
 *               i.e. "text/x-s4j-sifc:1.0,text/x-vcard:2.1"
 * @param l    : arraylist of ContentTypeInfo, to fill
 */
static void fillContentTypeInfoList(ArrayList &l, const char* types) {

    l.clear();
    if (!types) {
        return;
    }

    char typeName[80];
    char typeVersion[20];
    const char* curr = types;
    const char* eostr = NULL;
    size_t len = 0;

    while (*curr) {
        // -------- Type Name ----------
        // skip leading spaces and commas
        while (isspace(*curr) || *curr == ',') {
            curr++;
        }
        // fast-forward to colon (next separator)
        eostr = curr;
        while (*eostr && *eostr != ':') {
            eostr++;
        }
        // copy type name
        len = eostr - curr;
        strncpy(typeName, curr, len);
        typeName[len] = 0;

        // ------- Type Version ---------
        curr = eostr;

        // skip leading spaces and colon
        while (isspace(*curr) || *curr == ':') {
            curr++;
        }
        // fast-forward to comma (next separator)
        eostr = curr;
        while (*eostr && *eostr != ',') {
            eostr++;
        }
        // copy type version
        len = eostr - curr;
        strncpy(typeVersion, curr, len);
        typeVersion[len] = 0;

        //
        // Add contentTypeInfo
        //
        ContentTypeInfo cti(typeName, typeVersion);
        l.add(cti);

        curr = eostr;
    }
}

/*
 * Ensure that the user agent string is valid.
 * If property 'user agent' is empty, it is replaced by 'mod' and 'SwV'
 * properties from AbstractDeviceConfig.
 * If also 'mod' property is empty, return a default user agent.
 *
 * @param config: reference to the current AbstractSyncConfig
 * @return      : user agent property as a new char*
 *                (need to be freed by the caller)
 */
const char* SyncManager::getUserAgent(AbstractSyncConfig& config) {

    char* ret;
    StringBuffer userAgent(config.getUserAgent());
    StringBuffer buffer;

    if (userAgent.length()) {
        ret = stringdup(userAgent.c_str());
    }
    // Use 'mod + SwV' parameters for user agent
    else {
        const char* mod = config.getMod();
        const char* swV = config.getSwv();
        if (mod && strcmp(mod, "")) {
            buffer.append(mod);
            if (swV && strcmp(swV, "")) {
                buffer.append(" ");
                buffer.append(swV);
            }
            ret = stringdup(buffer.c_str());
        } else {
            // Default user agent value
            ret = stringdup(CHAR_USER_AGENT);
        }
    }

    return ret;
}

