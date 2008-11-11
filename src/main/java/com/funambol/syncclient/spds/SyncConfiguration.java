/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2006 - 2007 Funambol, Inc.
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
package com.funambol.syncclient.spds;

import java.io.UnsupportedEncodingException;

import java.util.Hashtable;
import java.util.Properties;

import com.funambol.syncclient.common.*;
import com.funambol.syncclient.common.logging.*;
import com.funambol.syncclient.spdm.*;

/**
 * @version  $Id: SyncConfiguration.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class SyncConfiguration {

    // --------------------------------------------------------------- Constants
    /**
     * Synchronization modes
     */
    public static final String SYNC_NONE              = "none"                ;
    public static final String SYNC_SLOW              = "slow"                ;
    public static final String SYNC_TWOWAY            = "two-way"             ;
    public static final String SYNC_ONEWAY            = "one-way"             ;
    public static final String SYNC_REFRESH           = "refresh"             ;

    public static final String PARAM_CLASSPATH        = "classpath"           ;
    public static final String PARAM_SYSTEM_CLASSPATH = "java.class.path"     ;
    public static final String PARAM_FIRST_SYNC_MODE  = "first-time-sync-mode";
    public static final String PARAM_MAX_ITEMS_MSG    = "max-items-msg"       ;
    public static final String PARAM_MAX_MSG_SIZE     = "max-msg-size"        ;
    public static final String PARAM_MAX_OBJ_SIZE     = "max-obj-size"        ;
    public static final String PARAM_AUTHENTICATION   = "authentication"      ;
    public static final String PARAM_DEVICEID         = "device-id"           ;
    public static final String PARAM_LOG_CONSOLE      = "log-console"         ;
    public static final String PARAM_LOG_FILE         = "log-file"            ;
    public static final String PARAM_LOG_LEVEL        = "log-level"           ;
    public static final String PARAM_MESSAGE_TYPE     = "message-type"        ;
    public static final String PARAM_MESSAGE_ENC      = "message-enc"         ;
    public static final String PARAM_PASSWORD         = "password"            ;
    public static final String PARAM_MSISDN_HDR       = "msisdn-hdr"          ;
    public static final String PARAM_MSISDN_VAL       = "msisdn-val"          ;
    public static final String PARAM_PROXYHOST        = "proxy-host"          ;
    public static final String PARAM_PROXYPORT        = "proxy-port"          ;
    public static final String PARAM_SYNCMLURL        = "syncml-url"          ;
    public static final String PARAM_TARGETLOCALURI   = "target-local-uri"    ;
    public static final String PARAM_USERNAME         = "username"            ;
    public static final String PARAM_USERPROXY        = "use-proxy"           ;
    public static final String PARAM_USERAGENT        = "user-agent"          ;

    public static final String PROP_APPLICATION_DISPLAY_NAME =
        "applicationDisplayName";
    public static final String PROP_APPLICATION_SUPPORT_URL
        = "applicationSupportUrl";
    public static final String PROP_APPLICATION_SUPPORT_MAIL
        = "applicationSupportMail";

    public static final String DEFAULT_PROXYHOST = "localhost";
    public static final String DEFAULT_PROXYPORT = "8080"     ;

    public static final String AUTHENTICATION_BASIC = "basic";
    public static final String AUTHENTICATION_CLEAR = "clear";

    public static final String MIMETYPE_SYNCMLDS_XML
            = "application/vnd.syncml+xml";
    public static final String MIMETYPE_SYNCMLDS_WBXML
            = "application/vnd.syncml+wbxml";

    public static final String XML_SYNCML_CLIENT
            = "com.funambol.syncclient.spds.XMLSyncMLClient";

    public static final String WBXML_SYNCML_CLIENT
            = "com.funambol.syncclient.spds.WBXMLSyncMLClient";

    public static final String DEFAULT_MESSAGETYPE = MIMETYPE_SYNCMLDS_XML;

    public static final String DEFAULT_MESSAGE_ENC = "UTF-8";

    public static final int DEFAULT_MAX_ITEMS_PER_MSG = 100;
    public static final int DEFAULT_MAX_MSG_SIZE      = 250000;
    public static final int DEFAULT_MAX_OBJ_SIZE      = 4000000;

    // ------------------------------------------------------------ Private data
    private Hashtable    params                 = null ;

    private String       url                    = null ;

    private String       username               = null ;
    private String       password               = null ;
    private String       deviceId               = null ;

    private String       messageType            = null ;
    private String       messageEnc             = null ;
    // contentType == combination of messageType and messageEnc
    private StringBuffer contentType            = null ;

    private String       msisdnKey              = null ;
    private String       msisdnVal              = null ;
    private boolean      useMsisdn              = false;

    private String       proxyHost              = null ;
    private int          proxyPort              = 8080 ;
    private boolean      useProxy               = false;

    private String       applicationDisplayName = null ;
    private String       applicationSupportUrl  = null ;
    private String       applicationSupportMail = null ;

    private int          maxItemsPerMsg         = DEFAULT_MAX_ITEMS_PER_MSG;
    private int          maxMsgSize             = DEFAULT_MAX_MSG_SIZE;
    private int          maxObjSize             = DEFAULT_MAX_OBJ_SIZE;

    private String       userAgent              = null ;

    // ------------------------------------------------------------ Constructors

    /**
     * SyncConfiguration constructor
     *
     * @param syncParams
     * @param runtimeProperties
     *
     */
    public SyncConfiguration(Hashtable syncParams,
                             Properties runtimeProperties)
    throws SyncException {
        params = syncParams;

        Object value        = null;

        username = runtimeProperties.getProperty(PARAM_USERNAME);
        if (username == null || username.length() == 0) {
            username = (String)syncParams.get(PARAM_USERNAME);
        }

        password = runtimeProperties.getProperty(PARAM_PASSWORD);
        if (password == null || password.length() == 0) {
            password = (String)syncParams.get(PARAM_PASSWORD);
        }

        deviceId = runtimeProperties.getProperty(PARAM_DEVICEID);
        if (deviceId == null || deviceId.length() == 0) {
            deviceId = (String)syncParams.get(PARAM_DEVICEID);
        }

        value = syncParams.get(PARAM_SYNCMLURL);
        if (value != null) {
            url = ((String)value).trim();
        }

        value = syncParams.get(PARAM_MESSAGE_TYPE);
        if (value != null) {
            messageType = ((String)value).trim();
        }

        value = syncParams.get(PARAM_MESSAGE_ENC);
        if (value != null) {
            messageEnc = ((String)value).trim();
        } else {
            messageEnc = DEFAULT_MESSAGE_ENC;
        }

        // Logging
        String logLevel     = null;
        String logFile      = null;
        String logConsole   = null;

        value = syncParams.get(PARAM_LOG_LEVEL);
        if (value != null) {
            logLevel= ((String)value).trim();
        }

        value = syncParams.get(PARAM_LOG_FILE);
        if (value != null) {
            logFile = ((String)value).trim();
        }

        value = syncParams.get(PARAM_LOG_CONSOLE );
        if (value != null) {
            logConsole = ((String)value).trim();
        }

        if (logLevel == null || logLevel.length() == 0) {
            Logger.setDefaultLevel();
        } else if (!logLevel.equals(Logger.PROP_NONE) &&
                   !logLevel.equals(Logger.PROP_ERROR) &&
                   !logLevel.equals(Logger.PROP_INFO) &&
                   !logLevel.equals(Logger.PROP_DEBUG)) {

            throw new SyncException("Incorrect log-level: " + logLevel);
        } else  {
            Logger.setLevel(logLevel);
        }

        if (logFile == null || logFile.length() == 0) {
            Logger.setDefaultLogFile();
        } else {
            Logger.setLogFile(logFile);
        }

        if (logConsole == null || logConsole.length() == 0) {
            Logger.setDefaultEnableConsole();
        } else if (!"true".equals(logConsole) &&
                   !"false".equals(logConsole)) {

            throw new SyncException("Incorrect log-console: " + logConsole);
        } else  {
            Logger.setEnableConsole(Boolean.valueOf(logConsole).booleanValue());
        }

        // MSISDN
        value = syncParams.get(PARAM_MSISDN_HDR);
        if (value != null) {
            msisdnKey = ((String)value).trim();
            if (msisdnKey.length() == 0) {
                msisdnKey = null;
            }
        }
        value = syncParams.get(PARAM_MSISDN_VAL);
        if (value != null) {
            msisdnVal = ((String)value).trim();
            if (msisdnVal.length() == 0) {
                msisdnVal = null;
            }
        }
        if ((msisdnVal != null) && (msisdnKey != null)) {
            useMsisdn = true;
        }

        // Proxy
        String useProxyStr  = null;
        String proxyPortStr = null;

        value = syncParams.get(PARAM_USERPROXY);
        if (value != null) {
            useProxyStr = ((String)value).trim();
        }

        value = syncParams.get(PARAM_PROXYHOST);
        if (value != null) {
            proxyHost = ((String)value).trim();
        }

        value = syncParams.get(PARAM_PROXYPORT);
        if (value != null) {
            proxyPortStr = ((String)value).trim();
        }

        // Message Encoding
        if (messageType == null || messageType.length() == 0) {
            messageType = DEFAULT_MESSAGETYPE;
        } else if (!messageType.equals(MIMETYPE_SYNCMLDS_XML) &&
                   !messageType.equals(MIMETYPE_SYNCMLDS_WBXML)) {

            throw new SyncException("Incorrect message-type: " +
                                    messageType                );
        }
        contentType = new StringBuffer(messageType);

        if ((messageEnc != null) && (messageEnc.length() != 0)) {
            if (isValidEncoding(messageEnc)) {
                contentType.append("; charset=");
                contentType.append(messageEnc);
            } else {
                throw new SyncException("Incorrect message-enc: " +
                                        messageEnc                );
            }
        }

        if (useProxyStr == null || useProxyStr.length() == 0) {
            useProxy = false;
        } else if (!"true".equals(useProxyStr) &&
                   !"false".equals(useProxyStr)) {

            throw new SyncException("Incorrect proxy-port: " + useProxyStr);
        } else  {
            useProxy = Boolean.valueOf(useProxyStr).booleanValue();
        }

        if (proxyHost == null || proxyHost.length() == 0) {
            proxyHost = DEFAULT_PROXYHOST;
        }

        if (proxyPortStr == null || proxyPortStr.length() == 0) {
            proxyPortStr = DEFAULT_PROXYPORT;
        }

        try {
            proxyPort = Integer.parseInt(proxyPortStr);
        } catch (NumberFormatException e) {
            throw new SyncException("Wrong proxy-port: " + proxyPortStr);
        }

        value = syncParams.get(PARAM_MAX_ITEMS_MSG);
        if (value != null) {
            String str = ((String)value).trim();
            try {
                maxItemsPerMsg = Integer.parseInt(str);
            } catch (NumberFormatException e) {
                throw new SyncException("Wrong max-items-msg: " + value);
            }
        }

        value = syncParams.get(PARAM_MAX_MSG_SIZE);
        if (value != null) {
            String str = ((String)value).trim();
            try {
                maxMsgSize = Integer.parseInt(str);
            } catch (NumberFormatException e) {
                throw new SyncException("Wrong max-msg-size: " + value);
            }
        }

        value = syncParams.get(PARAM_MAX_OBJ_SIZE);
        if (value != null) {
            String str = ((String)value).trim();
            try {
                maxObjSize = Integer.parseInt(str);
            } catch (NumberFormatException e) {
                throw new SyncException("Wrong max-obj-size: " + value);
            }
        }

        value = syncParams.get(PARAM_USERAGENT);
        if (value != null) {
            userAgent = (String)value;
        }
    }

    /**
     * Returns true if the given encoding is supported. False otherwise
     * @param enc String
     * @return boolean
     */
    private boolean isValidEncoding(String enc) {
        String tmp = "";
        try {
            byte[] b = tmp.getBytes(enc);
        } catch (UnsupportedEncodingException ex) {
            return false;
        }
        return true;
    }

    // --------------------------------------------------------------- Accessors

    public String getUsername() {
        return username;
    }
    
    public String getPassword() {
        return password;
    }
    
    public String getDeviceId() {
        return deviceId;
    }
    
    public String getAuthenticationType() {
        return (String) params.get(PARAM_AUTHENTICATION);
    }

    public boolean useMsisdn() {
        return useMsisdn;
    }
    
    public String getMsisdnKey() {
        return msisdnKey;
    }
    
    public String getMsisdnVal() {
        return msisdnVal;
    }

    public boolean getUseProxy() {
        return useProxy;
    }
    
    public String getProxyHost() {
        return proxyHost;
    }
    
    public int getProxyPort() {
        return proxyPort;
    }

    public String getMessageType() {
        return messageType;
    }
    
    public String getMessageEnc() {
        return messageEnc;
    }
    
    public String getContentType() {
        return contentType.toString();
    }

    public String getUrl() {
        return url;
    }
    
    public String getTargetLocalUri() {
        return (String) params.get(PARAM_TARGETLOCALURI);
    }

    public String getFirstSyncMode() {
        return (String) params.get(PARAM_FIRST_SYNC_MODE);
    }
    
    public int getMaxItemsPerMsg() {
        if (maxItemsPerMsg == -1) {
            //
            // -1 means no multimessages
            //
            return Integer.MAX_VALUE;
        }
        return maxItemsPerMsg;
    }
    
    public int getMaxMsgSize() {
        return maxMsgSize;
    }
    
    public int getMaxObjSize() {
        return maxObjSize;
    }    

    public String getClasspath() {
        return (String) params.get(PARAM_CLASSPATH);
    }

    public String getUserAgent() {
        return userAgent;
    }
}
