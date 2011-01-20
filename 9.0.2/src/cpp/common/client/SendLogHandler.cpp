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

#include "client/SendLogHandler.h"
#include "base/log.h"
#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "inputStream/InputStream.h"
#include "inputStream/StringOutputStream.h"
#include "http/URL.h"
#include "http/HttpConnection.h"
#include "http/BasicAuthentication.h"


BEGIN_FUNAMBOL_NAMESPACE

SendLogHandler::SendLogHandler(SyncManagerConfig& conf) : config(conf) {
    serverURL = "";

}

StringBuffer SendLogHandler::createLogHeader() {
    
    StringBuffer ret;
    AccessConfig& accessConfig = config.getAccessConfig();
    DeviceConfig& clientConfig = config.getClientConfig();
    
    ret = "******************** LOG HEADER ******************\r\n";
    
    ret += "** AUTH **\r\n";
    ret += "Username:\t"; ret += accessConfig.getUsername(); ret += "\r\n";
    ret += "ServerAuthType:\t"; ret += accessConfig.getServerAuthType(); ret += "\r\n";
    ret += "ClientAuthType:\t"; ret += accessConfig.getClientAuthType(); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** CONN **\r\n";
    ret += "syncURL:\t"; ret += accessConfig.getSyncURL(); ret += "\r\n";
    ret += "userAgent:\t"; ret += accessConfig.getUserAgent(); ret += "\r\n";
    ret += "enableCompress:\t"; ret += convertBoolToStringBuffer(accessConfig.getCompression()); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** DEVDETAIL **\r\n";
    ret += "devType:\t"; ret += clientConfig.getDevType(); ret += "\r\n";
    ret += "oem:\t\t"; ret += clientConfig.getOem(); ret += "\r\n";
    ret += "swv:\t\t"; ret += clientConfig.getSwv(); ret += "\r\n";
    ret += "fwv:\t\t"; ret += clientConfig.getFwv(); ret += "\r\n";
    ret += "hwv:\t\t"; ret += clientConfig.getHwv(); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** DEVINFO **\r\n";
    ret += "devInfo:\t"; ret += clientConfig.getDevID(); ret += "\r\n";
    ret += "man:\t\t";     ret += clientConfig.getMan(); ret += "\r\n";
    ret += "mod:\t\t";     ret += clientConfig.getMod(); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** EXT **\r\n";
    ret += "maxMsgSize:\t"; ret += convertLongToStringBuffer(accessConfig.getMaxMsgSize()); ret += "\r\n";
    ret += "maxObjSize:\t"; ret += convertLongToStringBuffer(clientConfig.getMaxObjSize()); ret += "\r\n";
    ret += "nocSupport:\t"; ret += convertBoolToStringBuffer(clientConfig.getNocSupport()); ret += "\r\n";
    ret += "utc:\t\t";      ret += convertBoolToStringBuffer(clientConfig.getUtc());        ret += "\r\n";
    ret += "devInfHash:\t"; ret += clientConfig.getDevInfHash();                            ret += "\r\n";
    ret += "logLevel:\t";   ret += convertLongToStringBuffer(clientConfig.getLogLevel());   ret += "\r\n";
    ret += "\r\n";
    
    DeviceConfig& serverConfig = config.getServerConfig();
    ret += "********************\r\n";
    ret += "** SERVER INFO **\r\n";
    ret += "devType:\t"; ret += serverConfig.getDevType(); ret += "\r\n";
    ret += "oem:\t\t"; ret += serverConfig.getOem(); ret += "\r\n";
    ret += "swv:\t\t"; ret += serverConfig.getSwv(); ret += "\r\n";
    ret += "fwv:\t\t"; ret += serverConfig.getFwv(); ret += "\r\n";
    ret += "hwv:\t\t"; ret += serverConfig.getHwv(); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** DEVINFO **\r\n";
    ret += "devInfo:\t"; ret += serverConfig.getDevID(); ret += "\r\n";
    ret += "man:\t\t";     ret += serverConfig.getMan(); ret += "\r\n";
    ret += "mod:\t\t";     ret += serverConfig.getMod(); ret += "\r\n";
    ret += "\r\n";
    
    ret += "** EXT **\r\n";
    ret += "smartSlowSync:\t\t";          ret += convertLongToStringBuffer(serverConfig.getSmartSlowSync());         ret += "\r\n";
    ret += "multipleEmailAccount:\t";   ret += convertLongToStringBuffer(serverConfig.getMultipleEmailAccount());  ret += "\r\n";
    ret += "mediaHttpUpload:\t";        ret += convertBoolToStringBuffer(serverConfig.getMediaHttpUpload());       ret += "\r\n";
    ret += "noFieldLevelReplace:\t";  ret += serverConfig.getNoFieldLevelReplace();   ret += "\r\n";
    ret += "nocSupport:\t\t";             ret += convertBoolToStringBuffer(serverConfig.getNocSupport());            ret += "\r\n";
    ret += "verDTD:\t\t\t";                 ret += serverConfig.getVerDTD();                ret += "\r\n";
    ret += "lastSyncURL:\t\t";            ret += serverConfig.getServerLastSyncURL();     ret += "\r\n";
    ret += "\r\n";
    
    ret += "** DATASTORES **\r\n";
    const ArrayList* dataStores = serverConfig.getDataStores();
    
    if (dataStores != NULL) {
        for (int i = 0; i < dataStores->size(); i++) {
            DataStore* dataStore = (DataStore*)dataStores->get(i);
            ret += "DataStore:\t";     ret += "\r\n";
            ret += "\tdisplayName:\t"; ret += dataStore->getDisplayName();    ret += "\r\n";
            SourceRef *sourceRef = dataStore->getSourceRef();
            if (sourceRef) {
                ret += "\tsourceRef:\t";   ret += sourceRef->getValue();      ret += "\r\n";
            }
            if (dataStore->getRxPref() != NULL) {
                ret += "\trx-Pref-Type:\t";        ret += dataStore->getRxPref()->getCTType();  ret += "\r\n";
                ret += "\trx-Pref-Version:\t";     ret += dataStore->getRxPref()->getVerCT();   ret += "\r\n";
            }
            if (dataStore->getTxPref() != NULL) {
                ret += "\ttx-Pref-Type:\t";        ret += dataStore->getTxPref()->getCTType();  ret += "\r\n";
                ret += "\ttx-Pref-Version:\t";     ret += dataStore->getTxPref()->getVerCT();   ret += "\r\n";
            }
            ret += "\r\n";
        }
    }
    
    for (unsigned int i = 0; i < config.getSyncSourceConfigsCount(); i++) {
        
        SyncSourceConfig* ssconfig = config.getSyncSourceConfig(i);
        ret += "** SOURCE: ";       ret += ssconfig->getName();         ret += "**\r\n";
        ret += "Uri:\t\t";          ret += ssconfig->getURI();          ret += "\r\n";
        ret += "SyncModes:\t";      ret += ssconfig->getSyncModes();    ret += "\r\n";
        ret += "Type:\t\t";         ret += ssconfig->getType();         ret += "\r\n";
        ret += "Sync:\t\t";         ret += ssconfig->getSync();         ret += "\r\n";
        ret += "Encoding:\t";       ret += ssconfig->getEncoding();     ret += "\r\n";
        ret += "Version:\t";        ret += ssconfig->getVersion();      ret += "\r\n";
        ret += "SupportedType:\t";  ret += ssconfig->getSupportedTypes(); ret += "\r\n";
        ret += "Last:\t\t";         ret += convertLongToStringBuffer(ssconfig->getLast());       ret += "\r\n";
        ret += "Encryption:\t";     ret += convertBoolToStringBuffer(ssconfig->getEncryption()); ret += "\r\n";
        ret += "Enabled:\t";        ret += convertBoolToStringBuffer(ssconfig->isEnabled());     ret += "\r\n";
        ret += "Last error:\t";     ret += convertLongToStringBuffer(ssconfig->getLastSourceError());                  ret += "\r\n";
        ret += "\r\n";
    }
    ret += "\r\n";
    ret += "***** ENDING LOG HEADER ********";
    ret += "\r\n";

    return ret;
}

StringBuffer SendLogHandler::convertBoolToStringBuffer(bool v) {
    return v ? "true" : "false";
}

StringBuffer SendLogHandler::convertLongToStringBuffer(unsigned long val) {
    StringBuffer ret;
    ret.append(val);
    return ret;
}


int SendLogHandler::sendLog(InputStream& inputStream) {
    
    int status = 0;
    
    // safe checks
    int size = inputStream.getTotalSize();
    if (size == 0) {
        LOG.error("SendLogHandler error: no data to transfer");
        return 1;
    }
    
    const char* syncUrl = NULL;
    if (serverURL.empty() == false) {
        syncUrl = serverURL.c_str();
    } else {
        syncUrl = config.getSyncURL();
    }
    
    StringBuffer fullUrl = composeURL(syncUrl);
    URL url(fullUrl.c_str());
    
    HttpConnection* httpConnection = new HttpConnection(config.getUserAgent());
    httpConnection->setCompression(false);
    
    status = httpConnection->open(url, HttpConnection::MethodPost);
    
    if (status) { 
        delete httpConnection;
        return status;
    }
    
    // Set headers (use basic auth)
    HttpAuthentication* auth = new BasicAuthentication(config.getUsername(), config.getPassword());
    httpConnection->setAuthentication(auth);
    setRequestHeaders(*httpConnection, inputStream);
    
    // Send the HTTP request
    StringOutputStream response;
    status = httpConnection->request(inputStream, response);
    LOG.debug("response returned = %s", response.getString().c_str());
    
    httpConnection->close();
    
    delete auth;
    delete httpConnection;
    return status;
    
    
}

void SendLogHandler::setRequestHeaders(HttpConnection& httpConnection, 
                                       InputStream& inputStream) {
    
    StringBuffer dataSize;
    int totalSize = inputStream.getTotalSize();
    LOG.debug("[%s]: input stream size is %i", __FUNCTION__, totalSize);
    dataSize.sprintf("%d", totalSize);
    
    httpConnection.setRequestHeader(HTTP_HEADER_ACCEPT,         "*/*");
    httpConnection.setRequestHeader(HTTP_HEADER_CONTENT_TYPE,   "text/plain");
    
    // set Funambol mandatory custom headers
    httpConnection.setRequestHeader(HTTP_HEADER_X_FUNAMBOL_FILE_SIZE, dataSize);
    httpConnection.setRequestHeader(HTTP_HEADER_X_FUNAMBOL_DEVICE_ID, config.getDevID());
    
}


StringBuffer SendLogHandler::composeURL(const char* syncUrl) {
    
    // Get the host & port info
    URL url;
    url.setURL(syncUrl);
    StringBuffer port(":80");
    if (url.port != 0) { 
        port = ":"; 
        port.append(url.port); 
    }
    
    // Compose the url
    StringBuffer ret(url.protocol);
    ret += "://"; 
    ret += url.host;
    ret += port;
    ret += "/client-log";
        
    LOG.debug("destination url = %s", ret.c_str());
    
    return ret;
}


END_FUNAMBOL_NAMESPACE