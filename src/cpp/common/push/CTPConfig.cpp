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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#if 0
#include "base/EncodingUtils.h"
#endif

#include "base/util/StringBuffer.h"
#include "spdm/constants.h"
#include "push/CTPConfig.h"    
#include "base/globalsdef.h"

USE_NAMESPACE

#define CTP_QUEUE_PUSH                  "queuePush"
#define CTP_RETRY                       "ctpRetry"
#define CTP_MAX_RETRY_TIMEOUT           "maxCtpRetry"
#define CTP_CMD_TIMEOUT                 "ctpCmdTimeout"
#define CTP_CONN_TIMEOUT                "ctpConnTimeout"
#define CTP_PORT                        "ctpPort"
#define CTP_READY                       "ctpReady"
#define PROPERTY_CTP_SERVER             "ctpServer"
#define PROPERTY_NOTIFY_TIMEOUT         "notifyTimeout"


CTPConfig::CTPConfig(const char* application_uri)
                    : DMTClientConfig() {
}
        
CTPConfig::~CTPConfig() {}

char* CTPConfig::decodePassword(const char* /* password */) {
    
    char* decripted = NULL;
#if 0
    // Encryption not supported yet!!! TODO FIXME
    if (password && strlen(password) > 0) {
         decripted = decryptData(password); 
    }    

    if (decripted == NULL || getLastErrorCode() == 801) {
        decripted = new char[1];
        decripted[0] = 0;
        setError(ERR_NONE, "");
    }
#endif
    return decripted;       
}

StringBuffer CTPConfig::encodePassword(const char* /* password */) {
    
    StringBuffer buffer("");
#if 0
    // Encryption not supported yet!!! TODO FIXME
    if (password && strlen(password) > 0) {
        char* encoded = encryptData(password);        
        buffer = encoded;        
    }        
#endif
    return buffer;   
}
        
void CTPConfig::readCTPConfig() {

    if (!open()) {
        LOG.error("Impossible read the ctp configuration. exit");
        return;
    }

    readAccessConfig(*syncMLNode);      // nonce is read with this call
    readDeviceConfig(*syncMLNode);      // devID is read with this call

    
#if 0
    // TODO: decode password if encoded!
    passDecoded = decodePassword(accessConfig.getPassword());
    accessConfig.setPassword(passDecoded);
#endif
    
    //
    // Read the single CTP properties
    //
    ManagementNode* node;
    char nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_PUSH_CTP);
    
    node = dmt->readManagementNode(nodeName);
    if (node) 
    {
        char* tmp;        
        tmp = node->readPropertyValue(PROPERTY_PUSH_NOTIFICATION);
        if (tmp) {
            setPush(atoi(tmp));
        } else {
            setPush(0);
        }        
        delete [] tmp;
             
        tmp = node->readPropertyValue(PROPERTY_POLLING_NOTIFICATION);
        if (tmp) {
            setPolling(atoi(tmp));
        } else {
            setPolling(0);
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_QUEUE_PUSH);
        if (tmp) {
            setQueuePush(atoi(tmp) == 0 ? false : true);
        } else {
            setQueuePush(false);
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(PROPERTY_NOTIFY_TIMEOUT);
        if (tmp) {
            setNotifyTimeout(atoi(tmp));
        } else {
            setNotifyTimeout(180);
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_RETRY);
        if (tmp) {
            setCtpRetry(atoi(tmp));
        } else {
            setCtpRetry(5);
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_MAX_RETRY_TIMEOUT);
        if (tmp) {
            setMaxCtpRetry(atoi(tmp));
        } else {
            setMaxCtpRetry(900); // 15 min
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_CMD_TIMEOUT);
        if (tmp) {
            setCtpCmdTimeout(atoi(tmp));
        } else {
            setCtpCmdTimeout(0); 
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_CONN_TIMEOUT);
        if (tmp) {
            setCtpConnTimeout(atoi(tmp));
        } else {
            setCtpConnTimeout(0); 
        }                
        delete [] tmp;

        tmp = node->readPropertyValue(CTP_PORT);
        if (tmp) {
            setCtpPort(atoi(tmp));
        } else {
            setCtpPort(0); 
        }                
        delete [] tmp;
        
        tmp = node->readPropertyValue(CTP_READY);
        if (tmp) {
            setCtpReady(atoi(tmp));
        } else {
            setCtpReady(5);
        }                
        delete [] tmp;

        // server url is loaded from regstry key 'ctpServer'
        tmp = node->readPropertyValue(PROPERTY_CTP_SERVER);
        if (tmp && strlen(tmp)>0) {
            // *** workaround for prefix: remove this when not necessary! ***
            StringBuffer url = checkPrefix(tmp);
            LOG.debug("urlTo = %s", url.c_str());
            setUrlTo(url);
        } else {
            // If 'ctpServer' reg value is empty, extract the url from 'syncUrl'
            setUrlTo(getHostName(accessConfig.getSyncURL()));
        }
        delete [] tmp;

        delete node;
        node = NULL;
    }
    
    close();
}


void CTPConfig::saveCTPConfig() { 
   
    LOG.debug("saveCTPConfig");

    ManagementNode* node = NULL;
    if (!open()) {
        return;
    }

    // TODO: encode password if necessary
    bool passwordEncoded = false;
    if (passwordEncoded) {
        StringBuffer buffer;
        buffer = encodePassword(accessConfig.getPassword());
        accessConfig.setPassword(buffer.c_str());
    }
    
    // Save the nonce: save AuthConfig properties
    StringBuffer nodeName("");
    nodeName.sprintf("%s%s%s", rootContext, CONTEXT_SPDS_SYNCML, CONTEXT_AUTH);
    node = dmt->readManagementNode(nodeName.c_str());
    if (node) {
        saveAuthConfig(*syncMLNode, *node);
        delete node;
    }

    if (passwordEncoded) {
#if 0
        // TODO FIXME
        char* passDecoded = decodePassword(accessConfig.getPassword());
        accessConfig.setPassword(passDecoded);
        delete [] passDecoded;
#endif
    } else {
        // Nothing to do: the password is already in clear text.
    }
    
    
    //
    // Save the single CTP properties
    //
    nodeName.sprintf("%s%s", rootContext, CONTEXT_PUSH_CTP);
    node = dmt->readManagementNode(nodeName.c_str());
    if (node) 
    {
        // NOTE: Leave it empty: so it's generated each time we call readCTPConfig()
        // So when we change the ServerURL, the 'urlTo' value is changed accordingly.
        node->setPropertyValue(PROPERTY_CTP_SERVER, "");
        
        StringBuffer value("");
        value.sprintf("%ld", ctpCmdTimeout);
        node->setPropertyValue(CTP_CMD_TIMEOUT, value.c_str());
        
        value.sprintf("%ld", ctpConnTimeout);
        node->setPropertyValue(CTP_CONN_TIMEOUT, value.c_str());
        
        value.sprintf("%ld", ctpPort);
        node->setPropertyValue(CTP_PORT, value.c_str());
        
        value.sprintf("%ld", ctpReady);
        node->setPropertyValue(CTP_READY, value.c_str());
        
        value.sprintf("%ld", ctpRetry);
        node->setPropertyValue(CTP_RETRY, value.c_str());
        
        value.sprintf("%ld", maxCtpRetry);
        node->setPropertyValue(CTP_MAX_RETRY_TIMEOUT, value.c_str());
        
        value.sprintf("%ld", notifyTimeout);
        node->setPropertyValue(PROPERTY_NOTIFY_TIMEOUT, value.c_str());
        
        value.sprintf("%ld", maxCtpRetry);
        node->setPropertyValue(CTP_MAX_RETRY_TIMEOUT, value.c_str());
        
        /* -- Not used now --
        value.sprintf("%d", push);
        node->setPropertyValue(PROPERTY_PUSH_NOTIFICATION, value.c_str());

        value.sprintf("%d", polling);
        node->setPropertyValue(PROPERTY_POLLING_NOTIFICATION, value.c_str());

        value.sprintf("%d", queuePush);
        node->setPropertyValue(CTP_QUEUE_PUSH, value.c_str());
        */
        
        delete node;
    }

    close();
}
                      

StringBuffer CTPConfig::getHostName(StringBuffer syncUrl) {

    size_t start, end;
    StringBuffer host;
    // Extract the hostName from syncUrl: "http://<hostName>:8080/funambol/ds"    
    start = syncUrl.find("://", 0);
    if (start != StringBuffer::npos) {
        start += 3;
    } else {
        // try to extract hostname from <hostName>:8080/funambol/ds"  
        start = 0;
    }
    
    end = syncUrl.find(":", start);       // stop if ":" or "/" found
    if (end == StringBuffer::npos) {
        end = syncUrl.find("/", start);
        if (end == StringBuffer::npos) {  // so the url is only <hostName>
            end = syncUrl.length();
        }
    }

    if (end > start) {
        host = syncUrl.substr(start, end-start);        
    }      

    return host;
}

int CTPConfig::getHostPort(StringBuffer syncUrl) {

    size_t start, endSlash, endColon, urlLength;
    StringBuffer hostPort = "";
    urlLength = syncUrl.length();
    int port = 0;
    // Extract the port from syncUrl: "http://<hostName>:8080/funambol/ds"    
    start = syncUrl.find("://", 0);
    if (start != StringBuffer::npos) {
        start += 3;
    } else {
        // try to extract the port from <hostName>:8080/funambol/ds"  
        start = 0;
    }
    
    endSlash = syncUrl.find("/", start);       // stop if ":" or "/" found
    endColon = syncUrl.find(":", start);       // stop if ":" or "/" found
    
    if (endSlash == StringBuffer::npos && endColon == StringBuffer::npos) {
        // there is no port
        // hostname
    } else if (endSlash != StringBuffer::npos && endColon == StringBuffer::npos) {
        // there is no port
        // hostname/funambol
    } else if (endSlash == StringBuffer::npos && endColon != StringBuffer::npos) {
        // there is port
        // hostname:8080            
        hostPort = syncUrl.substr(endColon + 1, urlLength - start);
    } else {
        if (endSlash > endColon) {
            // there is port
            // hostname:8080/funambol            
            hostPort = syncUrl.substr(endColon + 1, endSlash - endColon - 1);
        }
    }
    
    if (hostPort != "") {
        port = atoi(hostPort.c_str());
    }

    return port;
}
    

/// If passed 'url' contains "prefix:" the value is appended 
/// next to the host name: <prefix><hostName> is returned
/// (hostName is retrieved from SyncUrl)
/// Otherwise the passed 'url' is returned as StringBuffer.
StringBuffer CTPConfig::checkPrefix(char* url) {

    StringBuffer ctpUrl = url;
    const StringBuffer prefix = "prefix:";

    size_t pos = ctpUrl.find(prefix, 0);
    if (pos == StringBuffer::npos) {
        // Not found: keep the url passed
        return ctpUrl;
    }

    // Go after the ":" 
    pos += prefix.length();
    if (pos != StringBuffer::npos) {
        // Add the prefix
        ctpUrl = ctpUrl.substr(pos, ctpUrl.length());
        // Append the syncUrl
        ctpUrl += getHostName(accessConfig.getSyncURL());
    }

    return ctpUrl;
}

