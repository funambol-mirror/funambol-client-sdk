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
#include "base/util/utils.h"
#include "spds/AccessConfig.h"
#include "spdm/constants.h"
#include "base/globalsdef.h"

USE_NAMESPACE


AccessConfig::AccessConfig() {

    useProxy = false;
    proxyPort = 8080;
    firstTimeSyncMode = SYNC_SLOW;
    dirty = false;

    username   = NULL;
    password   = NULL;
    proxyHost  = NULL;
    syncURL    = NULL;

    endTimestamp = beginTimestamp = 0;

    serverNonce           = NULL;
    clientNonce           = NULL;
    serverID              = NULL;
    serverPWD             = NULL;
    clientAuthType        = NULL;
    serverAuthType        = NULL;
    isServerAuthRequired  = false;
    maxMsgSize            = 0;
    readBufferSize        = 0;
    userAgent             = NULL;
    proxyUsername         = NULL;
    proxyPassword         = NULL;
    checkConn             = false;
    responseTimeout       = 0;
    compression           = false;
}

AccessConfig::AccessConfig(AccessConfig& s) {
    assign(s);
}

AccessConfig::~AccessConfig() {
    safeDelete(&username );
    safeDelete(&password );
    safeDelete(&proxyHost);
    safeDelete(&syncURL  );

    safeDelete(&serverNonce         );
    safeDelete(&clientNonce         );
    safeDelete(&serverID            );
    safeDelete(&serverPWD           );
    safeDelete(&clientAuthType      );
    safeDelete(&serverAuthType      );
    safeDelete(&userAgent           );
    safeDelete(&proxyUsername       );
    safeDelete(&proxyPassword       );
}

bool AccessConfig::getServerAuthRequired() const {
    return isServerAuthRequired;
}

void AccessConfig::setServerAuthRequired(bool v) {
    isServerAuthRequired = v;

    dirty |= DIRTY_SERVERAUTH_REQUIRED;
}

const char* AccessConfig::getServerAuthType() const {
    return serverAuthType;
}


void AccessConfig::setServerAuthType(const char* v){
    set(&serverAuthType, v);
}


const char* AccessConfig::getClientAuthType() const {
    return clientAuthType;
}


void AccessConfig::setClientAuthType(const char* v){
    set(&clientAuthType, v);

    dirty |= DIRTY_CLIENTAUTHTYPE;
}

const char* AccessConfig::getServerPWD() const {
    return serverPWD;
}


void AccessConfig::setServerPWD(const char* v){
    set(&serverPWD, v);

    dirty |= DIRTY_SERVERPWD;
}

const char* AccessConfig::getServerID() const {
    return serverID;
}


void AccessConfig::setServerID(const char* v){
    set(&serverID, v);

    dirty |= DIRTY_SERVERID;
}

const char* AccessConfig::getServerNonce() const {
    return serverNonce;
}


void AccessConfig::setServerNonce(const char* v){
    set(&serverNonce, v);

    dirty |= DIRTY_SERVER_NONCE;
}

const char* AccessConfig::getClientNonce() const {
    return clientNonce;
}


void AccessConfig::setClientNonce(const char* v){
    set(&clientNonce, v);

    dirty |= DIRTY_CLIENT_NONCE;
}

const char* AccessConfig::getUsername() const {
    return username;
}


void AccessConfig::setUsername(const char* v){
    set(&username, v);

    dirty |= DIRTY_USERNAME;
}


const char* AccessConfig::getPassword() const {
    return password;
}

void AccessConfig::setPassword(const char* v) {
    set(&password, v);

    dirty |= DIRTY_PASSWORD;
}

SyncMode AccessConfig::getFirstTimeSyncMode() const {
    return firstTimeSyncMode;
}

void AccessConfig::setFirstTimeSyncMode(SyncMode v) {
    firstTimeSyncMode = v;

    dirty |= DIRTY_FIRST_TIME_SYNC_MODE;
}

bool AccessConfig::getUseProxy() const {
    return useProxy;
}

void AccessConfig::setUseProxy(bool v) {
    useProxy = v;

    dirty |= DIRTY_USE_PROXY;
}

const char* AccessConfig::getProxyHost() const {
    return proxyHost;
}

void AccessConfig::setProxyHost(const char* v) {
    set(&proxyHost, v);

    dirty |= DIRTY_PROXY_HOST;
}

int AccessConfig::getProxyPort() const {
    return proxyPort;
}

void AccessConfig::setProxyPort(int v) {
    proxyPort = v;

    dirty |= DIRTY_PROXY_PORT;
}

const char* AccessConfig::getProxyUsername() const {
    return proxyUsername;
}

void AccessConfig::setProxyUsername(const char* v) {
    set(&proxyUsername, v);
}

const char* AccessConfig::getProxyPassword() const {
    return proxyPassword;
}

void AccessConfig::setProxyPassword(const char* v) {
    set(&proxyPassword, v);
}

const char* AccessConfig::getUserAgent() const {
    return userAgent;
}

void AccessConfig::setUserAgent(const char* v) {
    set(&userAgent, v);
}


unsigned int AccessConfig::getResponseTimeout() const {
    return responseTimeout;
}
void AccessConfig::setResponseTimeout(unsigned int v) {
    responseTimeout = v;
}

bool AccessConfig::getCheckConn() const {
    return checkConn;
}
void AccessConfig::setCheckConn(bool v) {
    checkConn = v;
}


const char* AccessConfig::getSyncURL() const {
    return syncURL;
}

void AccessConfig::setSyncURL(const char* v) {
    //
    // Checks if the url starts with http(s)://; if not, http:// is prepended
    //
    set(&syncURL, v);

    // Adds default protocol if not set AND the string is not empty
    if (*syncURL							&&
        strncmp(syncURL, "http://", 7)  &&
        strncmp(syncURL, "HTTP://", 7)  &&
        strncmp(syncURL, "https://", 8) &&
        strncmp(syncURL, "HTTPS://", 8) ) {

            char* dest = new char[strlen(syncURL)+8];
            sprintf(dest, "http://%s", syncURL );

            set(&syncURL, dest);

            delete dest;
    }

    dirty |= DIRTY_SYNC_URL;
}

void AccessConfig::setBeginSync(unsigned long timestamp) {
    beginTimestamp = timestamp;
    dirty |= DIRTY_SYNC_BEGIN;
}

unsigned long AccessConfig::getBeginSync() const {
    return beginTimestamp;
}

void AccessConfig::setMaxMsgSize(unsigned long msgSize) {
    maxMsgSize = msgSize;
}

unsigned long AccessConfig::getMaxMsgSize() const {
    return maxMsgSize;
}

void AccessConfig::setReadBufferSize(unsigned long bufferSize) {
    readBufferSize = bufferSize;
}

unsigned long AccessConfig::getReadBufferSize() const {
    return readBufferSize;
}

void AccessConfig::setEndSync(unsigned long timestamp) {
    endTimestamp = timestamp;
    dirty |= DIRTY_SYNC_END;
}

unsigned long AccessConfig::getEndSync() const {
    return endTimestamp;
}


unsigned int AccessConfig::getDirty() const {
    return dirty;
}

void AccessConfig::set(char** buf, const char* v) {
    safeDelete(buf);

    if (v == NULL) {
        v = "";
    }
    int len = strlen(v);
    *buf = new char[len+2];

    strcpy(*buf, v);
}

void AccessConfig::assign(const AccessConfig& s) {
    setUsername (s.getUsername() );
    setPassword (s.getPassword() );
    setSyncURL  (s.getSyncURL()  );
    setProxyHost(s.getProxyHost());
    setProxyPort(s.getProxyPort());
    setUserAgent(s.getUserAgent());

    setProxyUsername(s.getProxyUsername());
    setProxyPassword(s.getProxyPassword());
    setBeginSync(s.getBeginSync());
    setEndSync(s.getEndSync());
    setFirstTimeSyncMode(s.getFirstTimeSyncMode());

    setServerAuthRequired(s.getServerAuthRequired());
    setClientAuthType(s.getClientAuthType());
    setServerAuthType(s.getServerAuthType());
    setServerPWD(s.getServerPWD());
    setServerID(s.getServerID());
    setServerNonce(s.getServerNonce());
    setClientNonce(s.getClientNonce());
    setMaxMsgSize(s.getMaxMsgSize());
    setReadBufferSize(s.getReadBufferSize());
    setCheckConn(s.getCheckConn());
    setResponseTimeout(s.getResponseTimeout());
    setCompression(s.getCompression());

    dirty = s.getDirty();
}

void AccessConfig::setCompression(bool v){
        compression= v;
}


/*void AccessConfig::setCompression(const char *v){
    if(strcmp(v,"1") == 0){
        compression = true;
    }else{
        compression = false;
    }
}*/



bool AccessConfig::getCompression() const{
    return compression;
}

