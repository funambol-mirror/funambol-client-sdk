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
package com.funambol.syncclient.spds;

import java.io.*;

import java.text.*;

import java.util.*;

import sync4j.framework.core.*;
import sync4j.framework.tools.*;

import com.funambol.syncclient.common.*;
import com.funambol.syncclient.common.logging.*;
import com.funambol.syncclient.spdm.*;
import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.event.*;

/**
 * The <i>SyncManager</i> is the contact point between a host application and the
 * synchronization engine. It is designed to hidden as much as possible the
 * details of the synchronization logic, protocol, communication and so on;
 * the simplest way to use it is to get an instance of the <i>SyncManager</i>
 * and call its <i>sync()</i> method, as in the examples below:
 * <pre>
 *    SyncManager syncManager = SyncManager.getSyncManager("test");
 *
 *    syncManager.sync();
 * </pre>
 * <pre>
 *    SyncManager syncManager = SyncManager.getSyncManager("test", runtimeProperties);
 *
 *    syncManager.sync();
 * </pre>
 * <i>getSyncManager()</i> is a factory method that creates a new <i>SyncManager</i>
 * bound to the given application URI. The application URI is an application
 * identifier that must be unique amongst all the SyncPlatform-enabled
 * applications running on the device. It is intended for future use.<br>
 * <b>runtimeProperties</b> are properties set in runtime and not load
 * by Device Management, may be:
 * <pre>
 *  username
 *  password
 *  device-id
 *  </pre>
 * The information required by the synchronization engine for initialization and
 * to kick off a data synchronization session is stored in the device management
 * configuration tree and can be manipulated by the means of the SyncPlatform
 * Device Management API.  See <i>Funambol SyncClient API 2.0 Programmer Guide</i>
 * for more information.
 *
 * @version  $Id: SyncManager.java,v 1.4 2008-05-15 20:42:20 stefano_fornari Exp $
 */
public class SyncManager implements SyncTransportListener {

    // --------------------------------------------------------------- Constants

    /**
     * Configuration context for sync sources
     */
    public static final String CONTEXT_SOURCES   = "spds/sources";

    /**
     * Configuration context for the SyncML agent
     */
    public static final String CONTEXT_SYNCAGENT = "spds/syncml" ;

    /**
     * Synchronization modes
     */
    public static final String SYNC_NONE          = "none"          ;
    public static final String SYNC_SLOW          = "slow"          ;
    public static final String SYNC_TWOWAY        = "two-way"       ;
    public static final String SYNC_ONEWAY        = "one-way"       ;
    public static final String SYNC_REFRESH       = "refresh"       ;
    public static final String SYNC_REFRESHCLIENT = "refresh-client";
    public static final String SYNC_ONEWAYCLIENT  = "one-way-client";

    public static final String TAG_ALERT    =  "Alert"   ;
    public static final String TAG_CMD      =  "Cmd"     ;
    public static final String TAG_DATA     =  "Data"    ;
    public static final String TAG_ITEM     =  "Item"    ;
    public static final String TAG_LOCALURI =  "LocURI"  ;
    public static final String TAG_STATUS   =  "Status"  ;
    public static final String TAG_SYNCBODY =  "SyncBody";
    public static final String TAG_SYNCHDR  =  "SyncHdr" ;
    public static final String TAG_SYNCML   =  "SyncML"  ;
    public static final String TAG_TARGET   =  "Target"  ;

    public static final String PROP_APPLICATION_DISPLAY_NAME =
        "applicationDisplayName";
    public static final String PROP_APPLICATION_SUPPORT_URL
            = "applicationSupportUrl";
    public static final String PROP_APPLICATION_SUPPORT_MAIL
            = "applicationSupportMail";

    public static final String AUTHENTICATION_BASIC = "basic";
    public static final String AUTHENTICATION_CLEAR = "clear";

    public static final String SESSION_ID = "12345678";

    public static final String MIMETYPE_SYNCMLDS_XML
            = "application/vnd.syncml+xml";
    public static final String MIMETYPE_SYNCMLDS_WBXML
            = "application/vnd.syncml+wbxml";

    public static final String XML_SYNCML_CLIENT
            = "com.funambol.syncclient.spds.XMLSyncMLClient";

    public static final String WBXML_SYNCML_CLIENT
            = "com.funambol.syncclient.spds.WBXMLSyncMLClient";

    public static final String DEFAULT_MESSAGETYPE = MIMETYPE_SYNCMLDS_XML;

    public static final String STATUS_COMMAND_NEW     =
                    "<Status>\n"                      +
                    "<CmdID>{0}</CmdID>\n"            +
                    "<MsgRef>{1}</MsgRef>\n"          +
                    "<CmdRef>{2}</CmdRef>\n"          +
                    "<Cmd>{3}</Cmd>\n"                +
                    "<SourceRef>{4}</SourceRef>\n"    +
                    "<Data>{5}</Data>\n"              +
                    "</Status>"                       ;

    public static final String STATUS_COMMAND_CHANGE  =
                    "<Status>\n"                      +
                    "<CmdID>{0}</CmdID>\n"            +
                    "<MsgRef>{1}</MsgRef>\n"          +
                    "<CmdRef>{2}</CmdRef>\n"          +
                    "<Cmd>{3}</Cmd>\n"                +
                    "<TargetRef>{4}</TargetRef>\n"    +
                    "<Data>{5}</Data>\n"              +
                    "</Status>"                       ;

    public static final String DEFAULT_MESSAGE_ENC = "UTF-8";

    // ------------------------------------------------------------ Private data

    private Hashtable          syncSourceDefinitions  = null  ;
    private Hashtable          serverAlerts           = null  ;
    private String[]           sourceURIs             = null  ;
    private String[]           sourceNames            = null  ;
    private String[]           sourceTypes            = null  ;

    private SyncMessages       msgs                   = null  ;
    private int                msgId                  = 2     ;
    private String             url                    = null  ;

    private SyncConfiguration  mgrConfig              = null  ;

    private String             applicationDisplayName = null  ;
    private String             applicationSupportUrl  = null  ;
    private String             applicationSupportMail = null  ;

    private ManagementNode     rootNode               = null  ;
    private ManagementNode[]   sources                = null  ;

    private String[]           statusCommands         = null  ;
    private Hashtable          mappings               = null  ;
    private long               nextTimestamp                  ;

    private Hashtable          syncSourceStatus       = null  ;

    private SyncSourceFactory  syncSourceFactory      = null  ;

    private Logger             logger                 = null  ;

    private int                detectedNewItems       = 0     ;
    private int                detectedUpdateItems    = 0     ;
    private int                detectedDeleteItems    = 0     ;
    private int                receivedNewItems       = 0     ;
    private int                receivedUpdateItems    = 0     ;
    private int                receivedDeleteItems    = 0     ;

    private Vector             syncItemListeners      = null  ;
    private Vector             syncListeners          = null  ;
    private Vector             syncSourceListeners    = null  ;
    private Vector             syncStatusListeners    = null  ;
    private Vector             syncTransportListeners = null  ;

    private SyncItemEvent      syncItemEvent          = null  ;
    private SyncSourceEvent    syncSourceEvent        = null  ;
    private SyncStatusEvent    syncStatusEvent        = null  ;

    // ------------------------------------------------------------ Constructors

    /**
     * Creates a SyncManager bound to a specific application. The application is
     * identified by the application URI parameter, which must be unique between
     * applications running in the same JVM.
     *
     * @param appURI The unique application URI
     * @param runtimeProperties are properties set
     *        in runtime and not load by Device Management, may be:
     * <pre>
     *  username
     *  password
     *  device-id
     * </pre>
     *
     * @throws SyncException in case an error occurs during initialization
     */
    protected SyncManager(String appURI, Properties runtimeProperties)
    throws SyncException {

        syncItemListeners      = new Vector();
        syncListeners          = new Vector();
        syncSourceListeners    = new Vector();
        syncStatusListeners    = new Vector();
        syncTransportListeners = new Vector();

        try {

            loadResources(appURI);

            //
            // Sync agent configuration
            //
            rootNode =
                SimpleDeviceManager.getDeviceManager().getManagementTree("/");

            Hashtable syncParams = rootNode.getNodeValues(CONTEXT_SYNCAGENT);

            mgrConfig = new SyncConfiguration(syncParams, runtimeProperties);
            url = mgrConfig.getUrl();

            if (Logger.isLoggable(Logger.DEBUG)) {
                Logger.debug("System properties: " +
                             System.getProperties().toString());
            }

            //
            // Sources configuration - sync source definitions are read and stored
            // in a hash table for quicker reference. In addition, source uris are
            // in sourceURIs in order to quickly loop over them.
            //
            rootNode = SimpleDeviceManager.
                            getDeviceManager().getManagementTree(appURI);

            ManagementNode sourcesNode = rootNode.getChildNode(CONTEXT_SOURCES);

            sources = sourcesNode.getChildren();

            applicationDisplayName
                = (String)rootNode.
                    getNodeValue("/application", PROP_APPLICATION_DISPLAY_NAME);
            applicationSupportUrl
                = (String)rootNode.
                    getNodeValue("/application", PROP_APPLICATION_SUPPORT_URL );
            applicationSupportMail
                = (String)rootNode.
                    getNodeValue("/application", PROP_APPLICATION_SUPPORT_MAIL);

        } catch (IOException e) {
            throw new SyncException("Error loading resources: " +
                                    e.getMessage(), e);
        } catch (DMException e) {
            throw new SyncException("Configuration error: "     +
                                    e.getMessage(), e);
        } catch (NumberFormatException e) {
            throw new SyncException("Invalid proxy port: "      +
                                    e.getMessage(), e);
        }
    }

    /**
     * Creates a SyncManager bound to a specific application. The application is
     * identified by the application URI parameter, which must be unique between
     * applications running in the same JVM.
     *
     * @param appURI The unique application URI
     * @throws SyncException
     *
     */
    public SyncManager (String appURI) throws SyncException {
        this(appURI, new Properties());
    }

    // ---------------------------------------------------------- Public methods

    /**
     * This is a factory method for SyncManager instances.
     * It creates a SyncManager bound to a specific application. The application is
     * identified by the application URI parameter, which must be unique between
     * applications running in the same JVM.
     * @param appURI The unique application URI
     * @throws SyncException in case of a synchronization engine related error
     * @throws DMException in case of a configuration error
     * @return a new SyncManager instance for the givven application
     */
    public static SyncManager getSyncManager(String appURI)
    throws SyncException, DMException {
        return new SyncManager(appURI);
    }

    /**
     * This is a factory method for SyncManager instances.
     * It creates a SyncManager bound to a specific application.
     * The application is identified by the application URI parameter,
     * which must be unique between applications running in the same JVM.
     * @param appURI The unique application URI
     * @param runtimeProperties are properties set in runtime
     *        and not load by Device Management, may be:
     * <pre>
     *  username
     *  password
     *  device-id
     * </pre>
     * @throws SyncException in case of a synchronization engine related error
     * @throws DMException in case of a configuration error
     * @return a new SyncManager instance for the givven application
     */
    public static SyncManager getSyncManager(String     appURI            ,
                                             Properties runtimeProperties )
    throws SyncException, DMException {
        return new SyncManager(appURI, runtimeProperties);
    }

    /**
     * Synchronize all registered synchronization sources.
     *
     * @throws SyncException if an error occurs during synchronization
     * @throws UpdateException if an error occurs during an update on server
     * @throws AuthenticationException if the server responded with
     *         "non authorized" return code
     */
    public void sync()
    throws SyncException, AuthenticationException, UpdateException {

        try {

            initSyncSources();

            nextTimestamp = System.currentTimeMillis();
            url = mgrConfig.getUrl();

            fireSyncEvent(new SyncEvent(SyncEvent.SYNC_BEGIN, nextTimestamp));

            String databaseList = "";

            //
            // If sourceURIs is null or zero-length, there is nothing to do...
            //
            if ((sourceURIs == null) || (sourceURIs.length == 0)) {
                if (logger.isLoggable(Logger.ERROR)) {
                    logger.error("No database to synchronize");
                }
                return;
            }

            String response = "";

            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Initializing");
            }

            String clientInitXML = prepareInizializationMessage();

            fireSyncEvent(new SyncEvent(SyncEvent.SEND_INITIALIZATION,
                                      System.currentTimeMillis()   ));

            response = syncInitialization(clientInitXML);

            checkServerAlerts(response);

            Enumeration keys = serverAlerts.keys();
            while (keys.hasMoreElements()) {
                String keyStr = (String) keys.nextElement();
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("source - Name: "        +
                                 keyStr                   +
                                 "| Status: "             +
                                 serverAlerts.get(keyStr));
                }
                if (databaseList.length() == 0) {
                    databaseList = keyStr;
                } else {
                    databaseList = databaseList + ", " + keyStr;
                }
            }

            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Synchronizing " + databaseList);
            }

            url = response.substring(
                response.indexOf("<RespURI>") + 9, response.indexOf("</RespURI>"));

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("url from inizialization-response:" + url);
            }

            //
            // Notifies the sources that the synchronization is going to begin
            //
            int alertCode = -1;
            int l = sourceURIs.length;
            for (int i=0; i < l; i++) {
                try {
                    alertCode = getSourceAlertCode(sourceURIs[i]);

                    syncSourceEvent =
                        new SyncSourceEvent(SyncSourceEvent.SYNC_BEGIN,
                                            sourceURIs[i]             ,
                                            alertCode                 ,
                                            System.currentTimeMillis());

                    fireSyncSourceEvent(syncSourceEvent);
                    syncSourceFactory.getSyncSource(sourceURIs[i]).beginSync(alertCode);
                } catch(SyncException e) {
                    fireSyncEvent(new SyncEvent(SyncEvent.SYNC_ERROR       ,
                                              System.currentTimeMillis() ,
                                              e.getMessage()             ,
                                              e                          ));
                    throw e;
                }
            }

            exchangeModifications();

            exchangeMappings();

            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Synchronization done");
            }

            //
            // Set the last anchor to the next timestamp for all the sources that
            // has been synchronized
            //
            l = sourceURIs.length;
            for (int i=0; i < l; i++) {
                try {
                    //
                    // Notifies the source that the synchronization is successfully
                    // give it a chance to stop the committing.
                    //
                    syncSourceFactory.getSyncSource(sourceURIs[i]).commitSync();
                    //
                    // Here everything was ok
                    //
                    updateLastAnchor(sourceURIs[i], nextTimestamp);

                    alertCode = getSourceAlertCode(sourceURIs[i]);

                    syncSourceEvent = new
                        SyncSourceEvent(SyncSourceEvent.SYNC_END  ,
                                        sourceURIs[i]             ,
                                        alertCode                 ,
                                        System.currentTimeMillis());

                    fireSyncSourceEvent(syncSourceEvent);

                } catch (DMException e) {
                    if (logger.isLoggable(Logger.ERROR)) {
                        String msg = "Error saving timestamp in context " +
                                      CONTEXT_SOURCES                      +
                                      '/'                                  +
                                      sources[i].getContext()              ;
                        logger.error(msg);
                    }

                    fireSyncEvent(new
                        SyncEvent(SyncEvent.SYNC_ERROR      ,
                                  System.currentTimeMillis(),
                                  e.getMessage()            ,
                                  e                         ));
                } catch (SyncException e) {
                    if (logger.isLoggable(Logger.ERROR)) {
                        String msg = "Error committing synchronization of source " +
                                      sourceURIs[i]                                +
                                      ": "                                         +
                                      e.getMessage()                               ;
                        logger.error(msg);
                    }

                    fireSyncEvent(new
                        SyncEvent(SyncEvent.SYNC_ERROR       ,
                                  System.currentTimeMillis() ,
                                  e.getMessage()             ,
                                  e                          ));
                }
            }

            fireSyncEvent(new SyncEvent(SyncEvent.SYNC_END        ,
                                      System.currentTimeMillis()));
        } catch (AuthenticationException e) {

            if (logger.isLoggable(Logger.ERROR)) {
                logger.error(e.getMessage());
            }
            throw e;

        } catch (UpdateException e) {

            if (logger.isLoggable(Logger.ERROR)) {
                logger.error(e.getMessage());
            }
            throw e;

        } catch (SyncException e) {

            if (logger.isLoggable(Logger.ERROR)) {
                logger.error(e.getMessage());
            }
            throw e;
        }
    }

    /**
     * Register a new SyncListener.
     *
     * @param listener
     */
    public void addSyncListener(SyncListener listener) {
        syncListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncListener.
     *
     * @param listener
     */
    public void removeSyncListener(SyncListener listener) {
        syncListeners.removeElement(listener);
    }

    /**
     * Register a new SyncTransportListener.
     *
     * @param listener
     */
    public void addSyncTransportListener(SyncTransportListener listener) {
        syncTransportListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncTransportListener.
     *
     * @param listener
     */
    public void removeSyncTransportListener(SyncTransportListener listener) {
        syncTransportListeners.removeElement(listener);
    }

    /**
     * Register a new SyncSourceListener.
     *
     * @param listener
     */
    public void addSyncSourceListener(SyncSourceListener listener) {
        syncSourceListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncSourceListener.
     *
     * @param listener
     */
    public void removeSyncSourceListener(SyncSourceListener listener) {
        syncSourceListeners.removeElement(listener);
    }

    /**
     * Register a new SyncItemListener.
     *
     * @param listener
     */
    public void addSyncItemListener(SyncItemListener listener) {
        syncItemListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncItemListener.
     *
     * @param listener
     */
    public void removeSyncItemListener(SyncItemListener listener) {
        syncItemListeners.removeElement(listener);
    }

    /**
     * Register a new SyncStatusListener.
     *
     * @param listener
     */
    public void addSyncStatusListener(SyncStatusListener listener) {
        syncStatusListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncStatusListener.
     *
     * @param listener
     */
    public void removeSyncStatusListener(SyncStatusListener listener) {
        syncStatusListeners.removeElement(listener);
    }

    /**
     * Send syncTransportEvent to syncTransportListeners
     * on sendDataBegin method.
     *
     * @param syncTransportEvent
     */
    public void sendDataBegin(SyncTransportEvent syncTransportEvent) {
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            ((SyncTransportListener)
                syncTransportListeners.elementAt(i)).
                    sendDataBegin(syncTransportEvent);
        }
    }

    /**
     * Send syncTransportEvent to syncTransportListeners
     * on sendDataEnd method.
     *
     * @param syncTransportEvent
     */
    public void sendDataEnd(SyncTransportEvent syncTransportEvent) {
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            ((SyncTransportListener)
                syncTransportListeners.elementAt(i)).
                    sendDataEnd(syncTransportEvent);
        }
    }

    /**
     * Send syncTransportEvent to syncTransportListeners
     * on receiveDataBegin method.
     *
     * @param syncTransportEvent
     */
    public void receiveDataBegin(SyncTransportEvent syncTransportEvent) {
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            ((SyncTransportListener)
                syncTransportListeners.elementAt(i)).
                    receiveDataBegin(syncTransportEvent);
        }
    }

    /**
     * Send syncTransportEvent to syncTransportListeners
     * on dataReceived method.
     *
     * @param syncTransportEvent
     */
    public void dataReceived(SyncTransportEvent syncTransportEvent) {
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            ((SyncTransportListener)
                syncTransportListeners.elementAt(i)).
                    dataReceived(syncTransportEvent);
        }
    }

    /**
     * Send syncTransportEvent to syncTransportListeners
     * on receiveDataEnd method.
     *
     * @param syncTransportEvent
     */
    public void receiveDataEnd(SyncTransportEvent syncTransportEvent) {
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            ((SyncTransportListener)
                syncTransportListeners.elementAt(i)).
                    receiveDataEnd(syncTransportEvent);
        }
    }

    // --------------------------------------------------------- Private methods

    /**
     * Synch initialization.
     *
     * @throws SyncException if an error occurs during synchronization
     * @throws UpdateException if an error occurs during an update on server
     *
     * @return sync initialization response
     **/
    private String syncInitialization(String clientInitXML)
    throws SyncException, AuthenticationException, UpdateException {
        try {
            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Sending initialization commands");
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("inizialization-request:" + clientInitXML);
                }
            }

            String response = postRequest(clientInitXML);
            checkStatus(response, TAG_SYNCHDR ) ;
            checkStatus(response, TAG_ALERT   ) ;

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("inizialization-response:" + response);
            }

            return response;
        } catch (Exception e) {
            String msg = "Error sync initializing: " + e.getMessage();
            throw new SyncException(msg);
        }
    }

    /**
     * Sync Modification.
     *
     * @throws SyncException if an error occurs during synchronization
     *
     * @return sync modification response
     **/
    private String syncModifications(String modificationsXML)
    throws SyncException {
        try {
            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Sending modifications");
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("modification-request:" + modificationsXML);
                }
            }

            String response = postRequest(modificationsXML);

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("modification-response: " + response);
            }

            return response;
        } catch (Exception e) {
            String msg = "Error sync modification: " + e.getMessage();
            throw new SyncException(msg);
        }
    }

    /**
     * Send Alert message.
     *
     * @throws SyncException if an error occurs during synchronization
     *
     * @return response
     **/
    private String sendAlertMessage(String alertXML) throws SyncException {
        try {
            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Sending alert message");
                    if (logger.isLoggable(Logger.DEBUG)) {
                        logger.debug("alert-request:" + alertXML);
                }
            }

            String response = postRequest(alertXML);

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("alert-response:" + response);
            }
            return response;
        } catch (Exception e) {
            String msg = "Error send Alert command: " + e.getMessage();
            throw new SyncException(msg);
        }
    }

    /**
     * Sync LUID-GUID mapping.
     *
     * @throws SyncException if an error occurs during synchronization
     *
     * @return sync modification response
     **/
    private String syncMapping(String mappingXML) throws SyncException {
        try {
            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Sending mapping");
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("mapping-request: " + mappingXML);
                }
            }

            String response = postRequest(mappingXML);

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("mapping-response: " + response);
            }

            return response;
        } catch (Exception e) {
            String msg = "Error sync mapping: " + e.getMessage();
            throw new SyncException(msg);
        }
    }

    /**
     * Posts the given message to the url specified by <i>url</i>.
     *
     * @param request the request msg
     * @return the url content as a byte array
     *
     * @throws SyncException in case of network errors
     **/
    private String postRequest(String request) throws SyncException {
        BaseSyncMLClient syncMLClient          = null;
        String           syncMLClientClassName = null ;

        try {

            if (MIMETYPE_SYNCMLDS_XML.equals(mgrConfig.getMessageType())) {
                syncMLClientClassName = XML_SYNCML_CLIENT   ;
            } else {
                syncMLClientClassName = WBXML_SYNCML_CLIENT ;
            }

            try {

                syncMLClient = (BaseSyncMLClient)
                    (Class.forName(syncMLClientClassName)).newInstance();

                syncMLClient.addSyncTransportListener(this);

            } catch (Exception e) {
                throw new SyncException ("Error loading class " +
                                         syncMLClientClassName  +
                                         ": "                   +
                                         e.getMessage()         );
            }

            syncMLClient.setUrl         (url         ) ;
            syncMLClient.setContentType (mgrConfig.getContentType());

            if(mgrConfig.getUseProxy()) {
                syncMLClient.setProxyHost(mgrConfig.getProxyHost());
                syncMLClient.setProxyPort(mgrConfig.getProxyPort());
                syncMLClient.setUseProxy (mgrConfig.getUseProxy() );
            }
            if (mgrConfig.useMsisdn()) {
                syncMLClient.setMsisdnKey(mgrConfig.getMsisdnKey());
                syncMLClient.setMsisdnVal(mgrConfig.getMsisdnVal());
                syncMLClient.setUseMsisdn(mgrConfig.useMsisdn());
            }

            if (mgrConfig.getUserAgent() != null) {
                syncMLClient.setUserAgent(mgrConfig.getUserAgent());
            }

            return syncMLClient.sendMessage(request, mgrConfig.getMessageEnc());

        } catch (Exception e) {
            String msg = "Error sending the request: " + e.getMessage();
            throw new SyncException(msg);
        }
    }

    /**
     * Checks if the given response message is authenticated from the server.
     *
     * @param msg the message to be checked
     * @param statusOf the command of which we want to check the status
     *
     * @throws AuthenticationException in case the request has not been
     *                                 authenticated by the server
     * @throws UpdateException if an error occurs during an update on server
     * @throws SyncException in case of other errors
     **/
    private void checkStatus(String msg, String statusOf)
    throws SyncException, AuthenticationException, UpdateException {

        Vector xmlMsg = new Vector();

        xmlMsg.addElement(msg);

        Vector statusTag =
            getXMLTag(
                getXMLTag(getXMLTag(xmlMsg, TAG_SYNCML),TAG_SYNCBODY),
                TAG_STATUS
            );

        int l = statusTag.size();

        for (int i=0; i < l; i++) {

            if (getXMLTagValue((String) statusTag.elementAt(i), TAG_CMD).equals(statusOf)) {

                String statusCode = getXMLTagValue((String) statusTag.elementAt(i), TAG_DATA);

                if (String.valueOf(StatusCode.OK).equals(statusCode)) {
                    //
                    // 200
                    //
                    return;
                } else if (String.valueOf(StatusCode.AUTHENTICATION_ACCEPTED).equals(statusCode)) {
                    //
                    // 212
                    //
                    return;
                } else if (String.valueOf(StatusCode.REFRESH_REQUIRED).equals(statusCode)) {
                    //
                    // 508
                    //
                    return;
                } else if (String.valueOf(StatusCode.INVALID_CREDENTIALS).equals(statusCode)   ||
                           String.valueOf(StatusCode.FORBIDDEN).equals          (statusCode)   ||
                           String.valueOf(StatusCode.MISSING_CREDENTIALS).equals(statusCode))      {
                    //
                    // 401, 403, 407
                    //
                    throw new AuthenticationException(
                        "Sorry, you are not authorized to synchronize."
                    );
                } else {
                    //
                    // Unhandled status code
                    //
                    throw new UpdateException(
                        "The server returned the error code " + statusCode
                    );
                }
            }
        }  // next i
        throw new SyncException("No status received");
    }

    /**
     * Checks response status for the synchronized databases and saves their
     * serverAlerts. <br>
     * If this is the first sync for the source, the status code might change
     * according to the value of the PARAM_FIRST_TIME_SYNC_MODE configuration
     * property.<br>
     * If firstTimeSyncMode is not set, the alert is left unchanged. If it is
     * set to a value, the specified value is used instead.
     *
     * @param msg the message to be checked
     *
     * @throws AuthenticationException in case the request has not been
     *                                 authenticated by the server
     * @throws SyncException in case of other errors
     **/
    private void checkServerAlerts(String msg)
    throws SyncException, AuthenticationException {

        Vector xmlMsg    = null;
        Vector itemMsg   = new Vector();
        Vector targetTag = null;

        String alert     = null;
        String item      = null;
        String dataStore = null;

        xmlMsg = new Vector();
        xmlMsg.addElement(msg);

        serverAlerts = new Hashtable();


        Vector alertTag
            = getXMLTag(getXMLTag(
                    getXMLTag(xmlMsg, TAG_SYNCML),TAG_SYNCBODY), TAG_ALERT);

        int l = alertTag.size();

        for (int i=0; i < l; i++) {
            itemMsg.clear();

            alert = getXMLTagValue((String) alertTag.elementAt(i), TAG_DATA);
            item = getXMLTagValue((String) alertTag.elementAt(i),  TAG_ITEM);
            itemMsg.addElement(item);

            targetTag = getXMLTag(itemMsg, TAG_TARGET);

            for (int j=0, m = targetTag.size(); j < m; j++) {
                dataStore = getXMLTagValue((String)targetTag.elementAt(j),
                                           TAG_LOCALURI);
            }

            if (logger.isLoggable(Logger.INFO)) {
                logger.info("The server alert code for " + dataStore + " is " + alert);
            }
            serverAlerts.put(dataStore, alert);
        }
    }

    /**
     * Prepare inizialization message
     *
     **/
    private String prepareInizializationMessage() throws SyncException {

        String username           = null;
        String password           = null;
        String userKey            = null;
        String deviceId           = null;

        int maxMsgSize            = 0;
        int maxObjSize            = 0;

        String authenticationType = null;
        String authentication     = null ;

        StringBuffer dbAlertsXML   = new StringBuffer();

        authenticationType = mgrConfig.getAuthenticationType();
        username = mgrConfig.getUsername();
        password = mgrConfig.getPassword();

        maxMsgSize = mgrConfig.getMaxMsgSize();
        maxObjSize = mgrConfig.getMaxObjSize();

        //
        // authentication default, clear
        //
        if (authenticationType == null || !(authenticationType.length() > 0) ||
            AUTHENTICATION_CLEAR.equals(authenticationType)) {

            userKey         = username + ':' + password;
            authentication  = "auth-clear";

        } else if (AUTHENTICATION_BASIC.equals(authenticationType)) {

            userKey         = new String(Base64.encode(
                                        (username + ':' + password).getBytes()));

            authentication  = "auth-basic";

        } else  {
            throw new SyncException("Configuration error, authentication type");
        }

        deviceId           = mgrConfig.getDeviceId();

        int l = sourceURIs.length;

        for (int i=0; i < l; i++) {
            dbAlertsXML.append(createAlerts(sourceURIs[i]));
        }

        return msgs.getClientInitXML(
             new Object[] {
                SESSION_ID            ,
                url                   ,
                deviceId              ,
                authentication        ,
                userKey               ,
                String.valueOf(maxMsgSize),
                String.valueOf(maxObjSize),
                dbAlertsXML.toString()
            }
        );
    }

    /**
     * Process the modifications from the received from server.
     *
     * @param modifications the modification message
     *
     * @return true if a response message is required, false otherwise
     *
     * @throws SyncException
     */
    private boolean processModifications(String modifications)
    throws SyncException {
        Message msg = null;

        try {
            msg = new Message(modifications);
        } catch (RepresentationException e) {
            throw new SyncException("Not a SyncML message: " + e.getMessage());
        }

        AbstractCommand[]  cmds   = msg.getBody().getCommands();
        String             msgRef = msg.getHeader().getMessageID();

        //
        // Process only Sync commands
        //
        boolean ret = false;

        //
        // about status command
        //
        Vector       sourceUris     = null ;
        String       sourceUri      = null ;
        String       status         = null ;
        String       cmdName        = null ;
        SyncItemKey  key            = null ;
        Item      [] items          = null ;
        SourceRef [] sr             = null ;
        TargetRef [] tr             = null ;
        int          sourceUriCount = 0    ;

        sourceUris = new Vector();

        for (int i=0; (cmds != null) && (i<cmds.length); ++i) {

            //
            // process sync command
            //
            if (SyncCommand.COMMAND_NAME.equals(cmds[i].getName())) {

                processSyncCommand((SyncCommand)cmds[i], msgRef);
                ret = true;

                if (logger.isLoggable(Logger.INFO)) {

                    String source =((SyncCommand) cmds[i]).getTarget().getURI();

                    logger.info("Returned "           +
                                receivedNewItems      +
                                " new items, "        +
                                receivedUpdateItems   +
                                " updated items, "    +
                                receivedDeleteItems   +
                                " deleted items for " +
                                source                );
                }

                receivedNewItems    = 0 ;
                receivedUpdateItems = 0 ;
                receivedDeleteItems = 0 ;

            //
            // process status command
            //
            } else if (StatusCommand.COMMAND_NAME.equals(cmds[i].getName())) {

                cmdName = ((StatusCommand) cmds[i]).getCommandName();

                if ("Sync".equals(cmdName)) {
                    tr = ((StatusCommand)cmds[i]).getTargetRefs();
                    sourceUris.addElement(tr[0].toString());
                }

                if ("Add".equals    (cmdName) ||
                    "Replace".equals(cmdName) ||
                    "Delete".equals (cmdName)   )   {

                    if ("Add".equals(cmdName)) {
                        sourceUriCount++;
                    } else if ("Replace".equals(cmdName)) {
                        sourceUriCount++;
                    } else if ("Delete".equals(cmdName)) {
                        sourceUriCount++;
                    }

                    sourceUri = (String)sourceUris.elementAt(sourceUriCount) ;
                    sr        = ((StatusCommand)cmds[i]).getSourceRefs()     ;
                    status    = ((StatusCommand)cmds[i]).getData().toString();

                    //
                    // 1 status item
                    //
                    if (sr != null && sr.length > 0) {

                        key = new SyncItemKey(sr[0].toString());

                        syncStatusEvent = new
                            SyncStatusEvent (SyncStatusEvent.STATUS_RECEIVED,
                                             cmdName                        ,
                                             Integer.parseInt(status)       ,
                                             key                            ,
                                             sourceUri                      );

                        fireSyncStatusEvent(syncStatusEvent);

                    //
                    // more than 1 status items
                    //
                    } else {

                        items = ((StatusCommand) cmds[i]).getItems();

                        for (int k = 0, m = items.length; k < m; k++) {
                            key = new SyncItemKey(items[k].getSource().getURI());

                            syncStatusEvent = new
                            SyncStatusEvent (SyncStatusEvent.STATUS_RECEIVED,
                                             cmdName                        ,
                                             Integer.parseInt(status)       ,
                                             key                            ,
                                             sourceUri                      );

                            fireSyncStatusEvent(syncStatusEvent);
                        }
                    }
                }
            }
        }
        return ret;
    }

    /**
     * Process a single SyncCommand
     *
     * @param cmd the SyncCommand to be processed - NOT NULL
     *
     * @throws SyncException in case of error in the processing
     */
    private void processSyncCommand(SyncCommand cmd, String msgRef)
    throws SyncException {
        AbstractCommand[] cmds = cmd.getCommands();
        String source = cmd.getTarget().getURI();

        //
        // Get the existing mappings for the source. Create a new one if needed
        //
        Hashtable storeMappings = (Hashtable)mappings.get(source);

        if (storeMappings == null) {
            storeMappings = new Hashtable();
            mappings.put(source, storeMappings);
        }

        String luid;
        for (int i=0; (i<cmds.length); ++i) {
            Hashtable newMappings = null;
            newMappings = processModificationCommand(source                 ,
                                                    (ItemizedCommand)cmds[i],
                                                     msgRef                 );

            for(Enumeration e=newMappings.keys(); e.hasMoreElements(); ) {
                luid = (String)e.nextElement();
                storeMappings.put(luid, newMappings.get(luid));
            }
        }
    }

    /**
     * Performs a modification command on the given store calling modifyRecords().
     * This implementation handles only Add, Delete and Replace commands; all other
     * commands are ignored.
     *
     * @param source the name of the datastore - NOT NULL
     * @param cmd the modification command to be processed - NOT NULL
     *
     * @return an Hashtable containing the LUID-GUID mapping for the new items
     *
     * @throws SyncException in case of errors
     */
    private Hashtable processModificationCommand(String           source,
                                                 ItemizedCommand  cmd   ,
                                                 String           msgRef)
    throws SyncException {

        char        state      = ' '  ;
        SyncSource  syncSource = null ;
        String      cmdRef     = null ;
        String      cmdName    = null ;
        String      cmdFormat  = null ;

        cmdRef  = cmd.getCommandIdentifier().getValue();
        cmdName = cmd.getName()                        ;

        Meta m = cmd.getMeta();
        if (m != null) {
            MetaContent mc = m.getValue();
            if ((mc != null) && ("Format".equals(mc.getContentTag()))) {
                cmdFormat = ((StringMetaContent)mc).getContent();
            }
        }

        if (AddCommand.COMMAND_NAME.equals            (cmdName)) {
            state = 'N';
        } else if (ReplaceCommand.COMMAND_NAME.equals (cmdName)) {
            state = 'U';
        } else if (DeleteCommand.COMMAND_NAME.equals  (cmdName)) {
            state = 'D';
        }

        if (state == ' ') {
            return new Hashtable();
        }

        Item[] items = cmd.getItems();

        SyncItem syncItem = null;
        int l = items.length;
        SyncOperation[] syncOperations = new SyncOperation[l];

        syncSource = syncSourceFactory.getSyncSource(source);
        //
        // Note: in the case of a refresh or slow sync, the client database is
        // cleared and the received items must be inserted as new (they must
        // produce a mapping). Therefore, we check which sync is occurring and
        // set the state flag accordingly.
        //
        for (int i=0; i < l; ++i) {
            switch (state) {
                case SyncItemState.NEW:
                    syncItem = new SyncItemImpl(syncSource,
                                                items[i].getSource().getURI(),
                                                state);
                    fillSyncItem(syncItem, items[i].getData().getValue());
                    if (cmdFormat != null) {
                        //
                        // Sets or adds the Format property to the SyncItem
                        //
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_FORMAT,
                                                 cmdFormat               ));
                    }

                    break;
                case SyncItemState.UPDATED:
                    if (isRefreshingSync(source)) {
                        state = SyncItemState.NEW;
                    }
                    syncItem = new SyncItemImpl(syncSource,
                                                items[i].getTarget().getURI(),
                                                state);
                    fillSyncItem(syncItem, items[i].getData().getValue());
                    if (cmdFormat != null) {
                        //
                        // Sets or adds the Format property to the SyncItem
                        //
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_FORMAT,
                                                 cmdFormat               ));
                    }

                    break;
                case SyncItemState.DELETED:
                    syncItem = new SyncItemImpl(syncSource,
                                                items[i].getTarget().getURI(),
                                                state);
                    fillSyncItem(syncItem, null);
                    break;
                default:
                    throw new SyncException("State " + state +
                                            " not valid for this operation");
            }

            syncOperations[i] = new SyncOperation(syncItem, state);
        }

        //
        // Persist the modifications to the source
        //
        return modifySyncItem(syncSource      ,
                              syncOperations  ,
                              msgRef          ,
                              cmdRef          ,
                              cmdName         );
    }

    /**
     * Fill a SyncItem object with the given data as XML string.
     *
     * @param record the Record to be filled - NOT NULL
     * @param data the data - NULL
     */
    private void fillSyncItem(SyncItem syncItem, String data) {

        data = StringTools.unescapeXml(data);

        if (data != null) {
            syncItem.setProperty(
                new SyncItemProperty(
                    SyncItem.PROPERTY_BINARY_CONTENT, data.getBytes()
                )
            );
        }
        syncItem.setProperty(
            new SyncItemProperty(
                SyncItem.PROPERTY_TIMESTAMP,new Date(nextTimestamp)
            )
        );
    }

    /**
     * Parse the XML content in the form
     * <p>
     * &lt;record&gt;
     *   &lt;field&gt;<i>value</i>&lt;/field&gt;
     *   ...
     * &lt;/record&gt;
     * <p>
     * and return an array of values
     *
     * @param xml the xml string to parse
     *
     * @return the array of values
     */
    private String[] xmlToValues(String xml) {
        int offset = 0, end;
        int len    = xml.length();

        Vector values = new Vector();
        while (offset<len) {
            offset = xml.indexOf("<field>", offset);
            if (offset < 0) {
                break;
            }
            offset += 7;
            end = xml.indexOf("</field>", offset);

            if (end < 0) {
                break;
            }

            values.addElement(xml.substring(offset, end));
            offset = end+8;
        }

        String[] ret = new String[values.size()];
        int l = ret.length;
        for (int i=0; i < l; ++i) {
            ret[i] = (String)values.elementAt(i);
        }

        return ret;
    }

    /**
     * executing the SyncML modification phase
     * @throws SyncException
     */
    private void exchangeModifications() throws SyncException {

        String targetLocUri = mgrConfig.getTargetLocalUri();
        String deviceId     = mgrConfig.getDeviceId();

        Vector modifications = new Vector();
        StringBuffer commands;
        String modificationsXML;
        String response;

        if (logger.isLoggable(Logger.INFO)) {
            logger.info("exchange modifications started");
        }

        // Collect the modifications we have for synchronization
        commands = collectModifications(modifications);
        do {
            // adding the modifications in the message
            commands.append(prepareModificationCommands(modifications, mgrConfig.getMaxItemsPerMsg()));
            // only if we have no modifications left in the list we add the 'Final'
            if (modifications.isEmpty()) {
                commands.append("<Final/>");
            }
            modificationsXML = msgs.getModificationsXML(
                                   new Object[] {SESSION_ID   ,
                                                 Integer.toString(msgId++),
                                                 targetLocUri ,
                                                 deviceId     ,
                                                 deviceId     ,
                                                 targetLocUri ,
                                                 commands});

            fireSyncEvent(new SyncEvent(SyncEvent.SEND_MODIFICATION,
                                            System.currentTimeMillis()));
            response = syncModifications(modificationsXML);

            checkStatus(response, TAG_SYNCHDR);
        } while (modifications.size() > 0);
        mappings = new Hashtable();
        boolean alert = true;
        // start collecting the modifications from the server side
        // and keep looping until the server tells us 'Final'
        do {
           if (alert) {
                // Send alert we are done.
                String alertXML = prepareAlertMessage();
                alert = false;
                fireSyncEvent(new SyncEvent(SyncEvent.SEND_MODIFICATION,
                                            System.currentTimeMillis()));
                response = sendAlertMessage(alertXML);
            } else {
                String statusXML = prepareStatusMessage();
                fireSyncEvent(new SyncEvent(SyncEvent.SEND_MODIFICATION,
                                            System.currentTimeMillis()));
                response = sendAlertMessage(statusXML);
            }
            processModifications(response);
        } while (response.indexOf("<Final/>") == -1 &&
                response.indexOf("</Final>") == -1);
    }

    /**
     * Collect the required modifications.
     * @param items The vector of all collected items.
     * @return The string components for the data sources that did not change.
     *
     **/
    private StringBuffer collectModifications(Vector list) throws SyncException {
        Vector items;
        StringBuffer syncTag = new StringBuffer();
        String alert;
        int alertCode;
        int l = sourceURIs.length;
        for (int i=0; i < l; i++) {
            //
            // Note we must sync only the sources that are aknoledged by the
            // server with a server Alert command.
            //
            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("serverAlerts: " + serverAlerts);
            }

            alert = (String)serverAlerts.get(sourceURIs[i]);
            if (alert == null) {
                if (logger.isLoggable(Logger.INFO)) {
                    String msg = "The server did not sent an Alert for " +
                                 sourceURIs[i]                           +
                                 ". The source will not be synchronized.";
                    logger.info(msg);
                }
                continue;
            }

            alertCode = getSourceAlertCode(sourceURIs[i]);
            if (alertCode == AlertCode.SLOW) {
                //
                // Slow Sync!
                //
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Preparing slow sync for " + sourceURIs[i]);
                }
                items = filterRecordsForSlowSync(sourceURIs[i]);
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Detected " + items.size() + " items");
                }
            } else if (alertCode == AlertCode.REFRESH_FROM_CLIENT) {
                //
                // Refresh from client!
                //
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Preparing refresh from client for " + sourceURIs[i]);
                }
                items = filterRecordsForSlowSync(sourceURIs[i]);
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Detected " + items.size() + " items");
                }
            } else if (alertCode == AlertCode.REFRESH_FROM_SERVER) {
                //
                // Refresh from server
                //
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Preparing refresh for " + sourceURIs[i]);
                }
                items = new Vector(); // no items sent for refresh
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("No items sent for refresh");
                }
            } else if (alertCode == AlertCode.ONE_WAY_FROM_SERVER) {
                //
                // One-way sync from server: no needs of client modifications
                //
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Preparing one-way sync for " + sourceURIs[i]);
                }
                items = new Vector(); // no items sent for one-way
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("No items sent for one-way");
                }
            } else {
                //
                // Fast Sync!
                //
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Preparing fast sync for " + sourceURIs[i]);
                }
                //
                // NOTE: filterRecordsForFastSync returns items in updated state
                //
                items = filterRecordsForFastSync(sourceURIs[i]);
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info("Detected "           +
                                 detectedNewItems     +
                                 " new items, "       +
                                 detectedUpdateItems  +
                                 " updated items, "   +
                                 detectedDeleteItems  +
                                 " deleted items"     );
                }

                detectedNewItems    = 0 ;
                detectedUpdateItems = 0 ;
                detectedDeleteItems = 0 ;
            }
            if (items.size() == 0) {
                SyncSource syncSource = syncSourceFactory.getSyncSource(
                                                        sourceURIs[i]);
                syncTag.append(prepareSyncTag(items, sourceURIs[i],
                                                        syncSource.getType()));
            } else {
                list.addAll(items);
            }
        }
        return syncTag;
    }

    private String prepareModificationCommands(Vector modifications, int limit)
    throws SyncException {

        SyncItem item = null;
        SyncSource previous_syncsource = null;
        SyncSource current_syncsource = null;
        String sourceURI = "";
        String sourceType = "";
        Vector items = new Vector();
        StringBuffer syncTag = new StringBuffer();

        int l = modifications.size();
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("total amount of items " + l);
        }

        for(int i=0; i < l && limit > 0; i++, limit--) {
            item = (SyncItem) modifications.remove(0);

            current_syncsource = item.getSyncSource();
            if (previous_syncsource == null) {
                previous_syncsource = current_syncsource;
                sourceURI = previous_syncsource.getSourceURI();
                sourceType = previous_syncsource.getType();
                items.add(item);
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("previous syncsource == null, current URI: "
                         + sourceURI + ", type: " + sourceType
                         + ", items: " + items.size());
                }
            } else if (sourceURI.equals(current_syncsource.getSourceURI())) {
                items.add(item);
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("previous equals current, current URI: "
                         + sourceURI + ", type: " + sourceType
                         + ", items: " + items.size());
                }
            } else {
                syncTag.append(prepareSyncTag(items, sourceURI, sourceType));
                previous_syncsource = current_syncsource;
                sourceURI = previous_syncsource.getSourceURI();
                sourceType = previous_syncsource.getType();
                items = new Vector(); // reset
                items.add(item);
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("previous not equals current, current URI: "
                        + sourceURI + ", type: " + sourceType);
                }
            }
        }
        if (previous_syncsource != null) {
            // We still need to add the last one
            syncTag.append(prepareSyncTag(items, sourceURI, sourceType));
            previous_syncsource = current_syncsource;
            sourceURI = previous_syncsource.getSourceURI();
            sourceType = previous_syncsource.getType();
            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("closing the list -> current URI "
                    + sourceURI + " type " + sourceType);
            }
        }
        return syncTag.toString();
    }

    /**
     * Prepare the status messages for server modifications.
     * @return Message with modifications.
     **/
    private String prepareStatusMessage() {

        String targetLocUri = mgrConfig.getTargetLocalUri();
        String deviceId     = mgrConfig.getDeviceId();

        StringBuffer commands = new StringBuffer();

        if (statusCommands != null) {
            for (int i = 0, l = statusCommands.length; i < l; i++) {
               commands.append(statusCommands[i]).append('\n');
            }
        }
        // Clean up the status commands.
        statusCommands = new String[0];

        return msgs.getMappingXML(new Object[] { SESSION_ID,
                                                 Integer.toString(msgId++),
                                                 targetLocUri,
                                                 deviceId,
                                                 deviceId,
                                                 targetLocUri,
                                                 commands.toString() });
    }

    /**
     * Prepare alert message
     *
     **/
    private String prepareAlertMessage() {
        String deviceId = mgrConfig.getDeviceId();

        return msgs.getAlertXML(new Object[] {SESSION_ID,
                                              Integer.toString(msgId++),
                                              url       ,
                                              deviceId  ,
                                              url       ,
                                              deviceId  ,
                                              url       ,
                                              deviceId  });
    }

    /**
     * executing the SyncML mapping phase
     * @throws SyncException
     */
    private void exchangeMappings() throws SyncException {

        String targetLocUri = mgrConfig.getTargetLocalUri();
        String deviceId     = mgrConfig.getDeviceId();

        String response;

        if (logger.isLoggable(Logger.INFO)) {
            logger.info("Mapping started");
        }

        boolean status = false;
        StringBuffer commands;
        String message;

        do {
            if (status == false) {
                message = prepareStatusMessage();
                status = true;
            } else {
                commands = new StringBuffer();
                status = prepareMappingCommands(commands, mgrConfig.getMaxItemsPerMsg());
                if (status == false) {
                    commands.append("<Final></Final>");
                }
                message = msgs.getMappingXML(
                            new Object[] { SESSION_ID,
                                           Integer.toString(msgId++),
                                           targetLocUri,
                                           deviceId,
                                           deviceId,
                                           targetLocUri,
                                           commands.toString() });
            }

            fireSyncEvent(new SyncEvent(SyncEvent.SEND_FINALIZATION,
                                        System.currentTimeMillis()));
            response = syncMapping(message);
        } while (status);

        if (logger.isLoggable(Logger.INFO)) {
            logger.info("Mapping done");
        }
    }

    /**
     * Prepare mapping message
     * @return number of items still left.
     **/
    private boolean prepareMappingCommands(StringBuffer mapTag, int limit) {

        Hashtable  sourceMapping = null;

        int beginIndex = 0;

        boolean added = false;
        String source = null;
        Hashtable storeMapping = null;
        for (Enumeration e = mappings.keys(); e.hasMoreElements(); ) {
            source = (String)e.nextElement();
            sourceMapping = (Hashtable)mappings.get(source);

            if (sourceMapping == null) continue;

            ArrayList mapItems = new ArrayList(sourceMapping.size());

            int i = 0;
            for (Enumeration f = sourceMapping.keys();
                f.hasMoreElements() && (limit > 0); ++i, limit--) {
                String luid = (String)f.nextElement();
                String guid = (String)sourceMapping.remove(luid);
                mapItems.add(new MapItem(new Target(guid), new Source(luid)));
                added = true;
            }

            if (mapItems.size() > 0) {
                //
                // <CmdID>1</CmdID> is always SyncBody
                //
                beginIndex = statusCommands.length + 2;
                MapCommand mapCmd = new MapCommand(
                            new CommandIdentifier(beginIndex),
                                new Target(source),
                                new Source(source),
                                null /* Credential */,
                                null /* Meta       */,
                                (MapItem[])mapItems.toArray(new MapItem[mapItems.size()]));

                mapTag.append(mapCmd.toXML()).append('\n');
            }
        }
        return added;
    }


    /**
     * Load the resources from the classpath.
     *
     * @throws IOException
     **/
    private void loadResources(String appURI) throws IOException {
        Class c = this.getClass();
        ManagementNode rootNode = null;
        String basePath = null;

        rootNode = SimpleDeviceManager.getDeviceManager().getManagementTree("/");
        basePath = rootNode.getContext();

        msgs = new SyncMessages();
    }

    /**
     * Reads the content of the given input stream.
     *
     * @param is the input stream
     **/
    private String read(InputStream is) throws IOException {
        StringBuffer sb = new StringBuffer();

        try {
            byte[] buf = new byte[1024];

            int nbyte = -1;
            while ((nbyte = is.read(buf)) >= 0) {
                sb.append(new String(buf, 0, nbyte));
            }
        } finally {
            is.close();
        }
        return sb.toString();
    }

    /**
     * Contructs the alerts for the given databses.
     *
     * @return the XML for the SyncML Alert commands
     **/
    private String createAlerts(String sourceURI) {
        StringBuffer sb = new StringBuffer();

        SyncSourceDefinition ssd =
            (SyncSourceDefinition)syncSourceDefinitions.get(sourceURI);

        long lastTimestamp = ssd.getLastTimestamp();

        String choosenSyncMode =
            chooseSyncMode(lastTimestamp, ssd.getDefaultSync());

        sb.append("<Alert>\n");
        sb.append("<CmdID>1</CmdID>\n");
        sb.append("<Data>");
        sb.append(syncMode2AlertCode(choosenSyncMode));
        sb.append("</Data>\n");
        sb.append("<Item>\n");
        sb.append("<Target><LocURI>");
        sb.append(sourceURI);
        sb.append("</LocURI></Target>\n");
        sb.append("<Source><LocURI>");
        sb.append(sourceURI);
        sb.append("</LocURI></Source>\n");
        sb.append("<Meta>\n");
        sb.append("<Anchor xmlns=\"syncml:metinf\">\n");
        sb.append("<Last>" + lastTimestamp + "</Last>\n");
        sb.append("<Next>" + nextTimestamp + "</Next>\n");
        sb.append("</Anchor>\n");
        sb.append("</Meta>\n");
        sb.append("</Item>\n");
        sb.append("</Alert>\n");

        return sb.toString();
    }

    /**
     * Make a String[] by tags find with search.
     *
     * @param xmlInput tags about search
     * @param tag to find
     * @return find tags
     **/
    private Vector getXMLTag(Vector xmlInput, String tag) throws SyncException {

        Vector xmlReturn   = null;
        String xmlInputTag = null;
        String endTag      = null;

        endTag   = "</" + tag + ">";

        xmlReturn = new Vector();

        try {

            for (int j=0, l = xmlInput.size(); j < l; j++) {
                xmlInputTag = (String) xmlInput.elementAt(j);
                //
                // tag without namespace
                // or tag with namespace
                //
                while (xmlInputTag.indexOf("<" + tag + ">") != -1 ||
                       xmlInputTag.indexOf("<" + tag + " ") != -1) {

                    xmlReturn.addElement(getXMLTagValue(xmlInputTag, tag));
                    xmlInputTag =
                        xmlInputTag.substring(
                                        xmlInputTag.
                                            indexOf(endTag) + endTag.length());
                }
            }
        } catch (Exception e) {
            throw new SyncException("Parsing XML error, TAG " +
                                    tag                       +
                                    " - : "                   +
                                    e.getMessage()            );
        }
        return xmlReturn;
    }

    /**
     * Make a String by value of <i>tag</i>.
     *
     * @param xml xml msg
     * @param tag tag to find
     * @return tag value
     **/
    private String getXMLTagValue(String xml, String tag) {
        String endTag = null;
        int    begin  = 0   ;
        int    init   = 0   ;

        begin = xml.indexOf("<" + tag + ">");
        if (begin == -1) {
            //tag with namespace
            begin = xml.indexOf("<" + tag + " ");
        }

        init   = xml.indexOf(">", begin) + 1;
        endTag = "</" + tag + ">" ;

        return xml.substring(init, xml.indexOf(endTag));
    }

    /**
     * Retrieves the modified items since the last synchronization.
     *
     * @param sourceURI the source URI
     *
     * @return the modified records since the last synchronization
     **/
    private Vector filterRecordsForFastSync(String sourceURI)
    throws SyncException {

        Vector     ret        = new Vector();
        SyncItem[] items      = null;
        SyncSource syncSource = null;

        SyncSourceDefinition ssd =
            (SyncSourceDefinition)syncSourceDefinitions.get(sourceURI);

        Date lastTimestamp = new Date(ssd.getLastTimestamp());

        syncSource = syncSourceFactory.getSyncSource(sourceURI);
        items = syncSource.getNewSyncItems(null, lastTimestamp);
        if (items != null) {
            detectedNewItems = items.length;
            for (int i=0; i < detectedNewItems; i++) {
                ret.addElement(items[i]);

                syncItemEvent = new
                    SyncItemEvent(SyncItemEvent.ITEM_ADDED_BY_CLIENT,
                                  sourceURI                         ,
                                  items[i].getKey()                 );

                fireSyncItemEvent(syncItemEvent);
            }
        }

        items = syncSource.getUpdatedSyncItems(null, lastTimestamp);
        if (items != null) {
            detectedUpdateItems = items.length;
            for (int i=0; i < detectedUpdateItems; i++) {
                ret.addElement(items[i]);

                syncItemEvent = new
                    SyncItemEvent(SyncItemEvent.ITEM_UPDATED_BY_CLIENT,
                                  sourceURI                           ,
                                  items[i].getKey()                   );

                fireSyncItemEvent(syncItemEvent);
            }
        }

        items = syncSource.getDeletedSyncItems(null, lastTimestamp);
        if (items != null) {
            detectedDeleteItems = items.length;
            for (int i=0; i < detectedDeleteItems; i++) {
                ret.addElement(items[i]);

                syncItemEvent = new
                    SyncItemEvent(SyncItemEvent.ITEM_DELETED_BY_CLIENT,
                                  sourceURI                           ,
                                  items[i].getKey()                   );

                fireSyncItemEvent(syncItemEvent);
            }
        }
        return ret;
    }

    /**
     * Retrieves all items belonging to the given datastore, regardless their
     * modification status. It is used for slow sync.
     *
     * @param sourceURI the source we want the items
     *
     * @return all items in the source
     **/
    private Vector filterRecordsForSlowSync(String sourceURI)
    throws SyncException {

        Vector     ret   = new Vector();
        SyncItem[] items = null;

        SyncSource syncSource = syncSourceFactory.getSyncSource(sourceURI);
        items = syncSource.getAllSyncItems(null);
        for(int i=0, l = items.length; i < l; i++) {
            //
            // The source should not include deleted items, but for more
            // reliability....
            //
            if (items[i].getState() != SyncItemState.DELETED) {
                items[i].setState(SyncItemState.UPDATED);
                ret.addElement(items[i]);
            }
        }
        return ret;
    }

    /**
     * Apply the given operations to the given sync source.
     *
     * @param syncSource the source the changes must be applied to
     * @param syncOperations the operations to apply
     *
     * @throws SyncException if something goes wrong
     **/
    private Hashtable modifySyncItem(SyncSource         syncSource    ,
                                     SyncOperation []   syncOperations,
                                     String             msgRef        ,
                                     String             cmdRef        ,
                                     String             cmdName       )
    throws SyncException {

        Hashtable     mapping                = new Hashtable();
        SyncItem      item                   = null           ;
        SyncItem      newItem                = null           ;
        int           itemStatus             = StatusCode.OK  ;
        String[]      statusCommands1        = null           ;
        //
        // temporaney array to save status
        //
        String[]      statusCommands2        = null           ;
        String        statusCommandsTemplate = null           ;
        //
        // <CmdID>1</CmdID> is always SyncBody
        //;
        int           beginStatusIndex       = 2             ;

        if (statusCommands != null) {
            beginStatusIndex = statusCommands.length + 2;
        }

        int l = syncOperations.length;

        statusCommands1 = new String[l];

        for (int i=0; i < l; i++) {
            item = syncOperations[i].getSyncItem();

            try {
                switch (syncOperations[i].getOperation()) {
                    case SyncItemState.NEW:
                        newItem = syncSource.setSyncItem(null, item);
                        mapping.put(
                            newItem.getKey().getKeyAsString(),
                            item.getKey().getKeyAsString()
                        );
                        receivedNewItems ++;
                        syncItemEvent = new
                            SyncItemEvent(SyncItemEvent.ITEM_ADDED_BY_SERVER,
                                          syncSource.getSourceURI()         ,
                                          item.getKey()                     );

                        fireSyncItemEvent(syncItemEvent);
                        break;
                    case SyncItemState.UPDATED:
                        SyncItem syncItem = syncOperations[i].getSyncItem();

                        syncSource.setSyncItem(null, syncItem);
                        receivedUpdateItems ++;
                        syncItemEvent = new
                            SyncItemEvent(SyncItemEvent.ITEM_UPDATED_BY_SERVER,
                                          syncSource.getSourceURI()           ,
                                          syncItem.getKey()                   );

                        fireSyncItemEvent(syncItemEvent);
                        break;
                    case SyncItemState.DELETED:
                        syncItem = syncOperations[i].getSyncItem();

                        syncSource.removeSyncItem(null, syncItem);
                        receivedDeleteItems ++;
                        syncItemEvent = new
                            SyncItemEvent(SyncItemEvent.ITEM_DELETED_BY_SERVER,
                                          syncSource.getSourceURI()           ,
                                          syncItem.getKey()                   );

                        fireSyncItemEvent(syncItemEvent);
                        break;
                }

                itemStatus = StatusCode.OK;

            } catch (SyncException e) {
                if (logger.isLoggable(Logger.INFO)) {
                    logger.info( "Error setting the item "
                               + item.getKey().getKeyAsString()
                               + ": "
                               + e.getCause().getMessage()
                               );
                }

                itemStatus = StatusCode.COMMAND_FAILED;

                fireSyncEvent(new
                    SyncEvent(SyncEvent.SYNC_ERROR      ,
                              System.currentTimeMillis(),
                              e.getMessage()            ,
                              e.getCause()              ));
            }

            if (syncOperations[i].getOperation() == SyncItemState.NEW) {
                statusCommandsTemplate = STATUS_COMMAND_NEW   ;
            } else  {
                statusCommandsTemplate = STATUS_COMMAND_CHANGE;
            }

            statusCommands1[i] = MessageFormat.format(
                                    statusCommandsTemplate,
                                    new Object[] {
                                        String.valueOf(i + beginStatusIndex),
                                        msgRef                              ,
                                        cmdRef                              ,
                                        cmdName                             ,
                                        item.getKey().getKeyAsString()      ,
                                        String.valueOf(itemStatus)
                                    }
                                 );
            syncStatusEvent = new
                SyncStatusEvent(SyncStatusEvent.STATUS_TO_SEND,
                                cmdName                       ,
                                itemStatus                    ,
                                item.getKey()                 ,
                                syncSource.getSourceURI()     );

                fireSyncStatusEvent(syncStatusEvent);
        }

        if (statusCommands == null) {
            statusCommands = statusCommands1;
        } else {
            statusCommands2 = new String [statusCommands.length  +
                                          statusCommands1.length ];
            System.arraycopy(statusCommands       ,
                             0                    ,
                             statusCommands2      ,
                             0                    ,
                             statusCommands.length);

            System.arraycopy(statusCommands1       ,
                             0                     ,
                             statusCommands2       ,
                             statusCommands.length ,
                             statusCommands1.length);

            statusCommands = statusCommands2;
        }
        return mapping;
    }

    /**
     * return Sync tag about dataStoreName
     *
     * @param items rcords to sync
     * @param sourceURI  source uri
     * @param sourceType source type
     * @return sync tag value
     **/
    private String prepareSyncTag(Vector items     ,
                                  String sourceURI ,
                                  String sourceType) {

        SyncItem item = null;

        StringBuffer addItems     = new StringBuffer();
        StringBuffer replaceItems = new StringBuffer();
        StringBuffer deleteItems  = new StringBuffer();
        StringBuffer add          = new StringBuffer();
        StringBuffer replace      = new StringBuffer();
        StringBuffer delete       = new StringBuffer();
        StringBuffer syncTag      = new StringBuffer();

        boolean updatedItemsAreSameTypeAndFormat = checkItemsSourceTypeAndFormat(items, SyncItemState.UPDATED);
        boolean newItemsAreSameTypeAndFormat = checkItemsSourceTypeAndFormat(items, SyncItemState.NEW);

        int l = items.size();
        for(int i=0; i < l; i++) {
            item = (SyncItem) items.elementAt(i);

            switch (item.getState()) {
                case SyncItemState.DELETED:
                    deleteItems.append ("<Item>\n"                    )
                               .append ("<Source><LocURI>"            )
                               .append (item.getKey().getKeyAsString())
                               .append ("</LocURI></Source>\n"        )
                               .append ("</Item>\n"                   );
                    break;

                case SyncItemState.UPDATED:
                    replaceItems.append ("<Item>\n"                    )
                                .append ("<Source><LocURI>"            )
                                .append (item.getKey().getKeyAsString())
                            .append("</LocURI></Source>\n");

                    //if the format and the type is NOT the same for all updated
                    // items them the meta tag is added for each item
                    if (!updatedItemsAreSameTypeAndFormat) {
                        replaceItems.append("<Meta>\n");

                        String itemFormat = null;
                        try {
                            //try to get the format from the item
                            itemFormat = item.getProperty(SyncItem.PROPERTY_FORMAT)
                                    .getValue().toString();
                        } catch (NullPointerException ex) {
                        }
                        ;

                        //if the format is not null append it to message
                        if (itemFormat != null && !itemFormat.equals("")) {
                            replaceItems.append("<Format xmlns=\"syncml:metinf\">")
                                    .append(itemFormat)
                                    .append("</Format>\n");
                        }

                        String itemSourceType = null;
                        try {
                            //try to get the format from the item
                            itemSourceType = item.getProperty(SyncItem.PROPERTY_TYPE)
                                    .getValue().toString();
                        } catch (NullPointerException ex) {
                        }
                        ;
                        if (itemSourceType == null || itemSourceType.equals("")) {
                            itemSourceType = sourceType;
                        }
                        replaceItems.append("<Type xmlns=\"syncml:metinf\">")
                                .append(itemSourceType)
                                .append("</Type>\n")
                                .append("</Meta>\n");
                    }

                    replaceItems.append("<Data>")
                                .append (getContent(item)              )
                                .append ("</Data>\n"                   )
                                .append ("</Item>\n"                   );
                    break;

                case SyncItemState.NEW:
                    addItems.append ("<Item>\n"                    )
                            .append ("<Source><LocURI>"            )
                            .append (item.getKey().getKeyAsString())
                            .append("</LocURI></Source>\n");

                    //if the format and the type is NOT the same for all new
                    // items them the meta tag is added for each item
                    if (!newItemsAreSameTypeAndFormat) {
                        addItems.append("<Meta>\n");

                        String itemFormat = null;
                        try {
                            //try to get the format from the item
                            itemFormat = item.getProperty(SyncItem.PROPERTY_FORMAT)
                                    .getValue().toString();
                        } catch (NullPointerException ex) {
                        }
                        ;

                        //if the format is not null append it to message
                        if (itemFormat != null && !itemFormat.equals("")) {
                            addItems.append("<Format xmlns=\"syncml:metinf\">")
                                    .append(itemFormat)
                                    .append("</Format>\n");
                        }

                        String itemSourceType = null;
                        try {
                            //try to get the format from the item
                            itemSourceType = item.getProperty(SyncItem.PROPERTY_TYPE)
                                    .getValue().toString();
                        } catch (NullPointerException ex) {
                        }
                        ;
                        if (itemSourceType == null || itemSourceType.equals("")) {
                            itemSourceType = sourceType;
                        }
                        addItems.append("<Type xmlns=\"syncml:metinf\">")
                                .append(itemSourceType)
                                .append("</Type>\n")
                                .append("</Meta>\n");
                    }

                    addItems.append("<Data>")
                            .append (getContent(item)              )
                            .append ("</Data>\n"                   )
                            .append ("</Item>\n"                   );
                    break;
            } // end switch
        }  // next i
        //
        // NOTE: for JDK 1.1.8 compatibility we cannot use StringBuffer.append(StringBuffer)
        //
        if (addItems.length()>0) {
            add.append ("<Add>\n"                             )
                    .append("<CmdID>4</CmdID>\n");
            if (newItemsAreSameTypeAndFormat) {
                //all the new items have the same type and format
                //so we add the meta tag only once for the command

                String newItemsFormat = getItemsFormat(items, SyncItemState.NEW);
                String newItemsSourceType = getItemsType(items, SyncItemState.NEW);

                add.append("<Meta>\n");
                if ((newItemsFormat != null) && (!newItemsFormat.equals(""))) {
                    add.append("<Format xmlns=\"syncml:metinf\">")
                            .append(newItemsFormat)
                            .append("</Format>\n");
                }
                if ((newItemsSourceType == null) || (newItemsSourceType.equals(""))) {
                    newItemsSourceType = sourceType;
                }
                add.append("<Type xmlns=\"syncml:metinf\">")
                        .append(newItemsSourceType)
                        .append("</Type>\n")
                        .append("</Meta>\n");
            }
            add.append(addItems.toString())
               .append ("\n</Add>\n"                          );
       }
       if (replaceItems.length()>0) {
          replace.append ("<Replace>\n"                         )
                    .append("<CmdID>4</CmdID>\n");
            if (updatedItemsAreSameTypeAndFormat) {

                String updatedItemsFormat = getItemsFormat(items, SyncItemState.UPDATED);
                String updatedItemsSourceType = getItemsType(items, SyncItemState.UPDATED);

                replace.append("<Meta>\n");
                if ((updatedItemsFormat != null) && (!updatedItemsFormat.equals(""))) {
                    replace.append("<Format xmlns=\"syncml:metinf\">")
                            .append(updatedItemsFormat)
                            .append("</Format>\n");
                }
                if ((updatedItemsSourceType == null) || (updatedItemsSourceType.equals(""))) {
                    updatedItemsSourceType = sourceType;
                }
                replace.append("<Type xmlns=\"syncml:metinf\">")
                        .append(updatedItemsSourceType)
                        .append("</Type>\n")
                        .append("</Meta>\n");
            }
            replace.append(replaceItems.toString())
                 .append ("\n</Replace>\n"                      );
       }
       if (deleteItems.length()>0) {
           delete.append ("<Delete>\n"          )
                 .append ("<CmdID>4</CmdID>\n"  )
                 .append (deleteItems.toString())
                 .append ("\n</Delete>\n"       );
       }
       syncTag.append ("<Status>\n"                                            )
              .append ("<CmdID>2</CmdID>\n"                                    )
              .append ("<MsgRef>1</MsgRef><CmdRef>1</CmdRef><Cmd>Alert</Cmd>\n")
              .append ("<TargetRef>"                                           )
              .append (sourceURI                                               )
              .append ("</TargetRef>\n"                                        )
              .append ("<SourceRef>"                                           )
              .append (sourceURI                                               )
              .append ("</SourceRef>\n"                                        )
              .append ("<Data>200</Data>\n"                                    )
              .append ("<Item>\n"                                              )
              .append ("<Data>" + "\n"                                         )
              .append ("<Anchor xmlns=\"syncml:metinf\"><Next>"                )
              .append (nextTimestamp                                           )
              .append ("</Next></Anchor>\n"                                    )
              .append ("</Data>\n"                                             )
              .append ("</Item>\n"                                             )
              .append ("</Status>\n"                                           )
              .append ("<Sync>\n"                                              )
              .append ("<CmdID>3</CmdID>\n"                                    )
              .append ("<Target><LocURI>"                                      )
              .append (sourceURI                                               )
              .append ("</LocURI></Target>\n"                                  )
              .append ("<Source><LocURI>"                                      )
              .append (sourceURI                                               )
              .append ("</LocURI></Source>\n"                                  )
              .append (add.toString     ()                                     )
              .append (replace.toString ()                                     )
              .append (delete.toString  ()                                     )
              .append ("</Sync>"                                               );

       return syncTag.toString();
    }

    /**
     * Tests if items of same state have the same source type and format. If all items
     * of same state (add or update) have the same format and source type it return true.
     *
     * @param items vector of items to test
     * @param state the type of items to test (add, update)
     * @return true if all items of same type have same source type and same format
     */
     private boolean checkItemsSourceTypeAndFormat(Vector items, char state) {
        if (items == null) {
            return false;
        }
        //we put in itemTest the first item of type 'type'
        SyncItem itemTest = null;
        SyncItem item = null;
        Object testFormatProp=null;
        Object formatProp=null;
        Object testSourceTypeProp=null;
        Object sourceTypeProp=null;
        for (int i = 0; i < items.size(); i++) {
            item = (SyncItem) items.get(i);
            if (item.getState() == state) {
                if (itemTest == null) {
                    itemTest = item;
                } else {
                    //test if the format is the same
                    testFormatProp=itemTest.getPropertyValue(SyncItem.PROPERTY_FORMAT);
                    formatProp=item.getPropertyValue(SyncItem.PROPERTY_FORMAT);
                    if((testFormatProp!=null) &&
                       (formatProp!=null)     &&
                       (!formatProp.toString().equals(testFormatProp.toString())))
                    {
                        return false;
                    }
                    if((testFormatProp!=null && formatProp==null)||
                       (testFormatProp==null && formatProp!=null))
                    {
                        return false;
                    }
                    //test if the source type is the same
                    testSourceTypeProp=itemTest.getPropertyValue(SyncItem.PROPERTY_TYPE);
                    sourceTypeProp=item.getPropertyValue(SyncItem.PROPERTY_TYPE);
                    if((testSourceTypeProp!=null) &&
                       (sourceTypeProp!=null)     &&
                       (!sourceTypeProp.toString().equals(testSourceTypeProp.toString())))
                    {
                        return false;
                    }
                    if((testSourceTypeProp!=null && sourceTypeProp==null)||
                       (testSourceTypeProp==null && sourceTypeProp!=null))
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * Return the format of the first item in the vector that has the specified type
     *
     * @param items vector of items
     * @param state the type for wich the format will be returned (update, add)
     * @return the format of the first item of the specified type
     */
    private static String getItemsFormat(Vector items, char state) {
        SyncItem item=null;
        for(int i=0;i<items.size();i++) {
            item=(SyncItem)items.get(i);
            if(item.getState()==state) {
                Object format=item.getPropertyValue(SyncItem.PROPERTY_FORMAT);
                if(format!=null)
                    return format.toString();
                else
                    return null;
            }
        }
        return null;
    }

    /**
     * Return the source type of the first item in the vector that has the specified state
     *
     * @param items vector of items
     * @param state the state for wich the source type will be returned (update, add)
     * @return the source type of the first item of the specified type
     */
    private String getItemsType(Vector items, char state) {
        SyncItem item=null;
        for(int i=0;i<items.size();i++) {
            item=(SyncItem)items.get(i);
            if(item.getState()==state) {
                Object type=item.getPropertyValue(SyncItem.PROPERTY_TYPE);
                if(type!=null)
                    return type.toString();
                else
                    return null;
            }
        }
        return null;
    }

    /**
     * Converts one of the SyncMode none, slow, one-way, two-way, refres in the
     * corresponding SyncML alert code.
     *
     * @param sync the sync mode
     *
     * @return the SyncML alert code corresponding to the given sync mode
     */
    private int syncMode2AlertCode(String sync) {
        if (SYNC_TWOWAY.equals(sync)) {
            return AlertCode.TWO_WAY;
        } else if (SYNC_SLOW.equals(sync)) {
            return AlertCode.SLOW;
        } else if (SYNC_ONEWAY.equals(sync)) {
            return AlertCode.ONE_WAY_FROM_SERVER;
        } else if (SYNC_REFRESH.equals(sync)) {
            return AlertCode.REFRESH_FROM_SERVER;
        } else if (SYNC_ONEWAYCLIENT.equals(sync)) {
            return AlertCode.ONE_WAY_FROM_CLIENT;
        } else if (SYNC_REFRESHCLIENT.equals(sync)) {
            return AlertCode.REFRESH_FROM_CLIENT;
        }

        if (logger.isLoggable(Logger.INFO)) {
            logger.info("Sync mode " + sync + " not recognized. Using two-way");
        }
        return AlertCode.TWO_WAY;
    }

    /**
     * A refreshing sync is a sync that replace completely the client database.
     * Refreshing syncs are:
     * <ul>
     *   <li>SLOW
     *   <li>REFRESH
     * </ul>
     *
     * @param sourceURI the source to check
     *
     * @return <i>true</i> if the sync perfomed on the given source is a refreshing
     *              sync, <i>false</i> otherwise.
     */
    private boolean isRefreshingSync(String sourceURI) {
        int alertCode;
        try {
            alertCode = Integer.parseInt((String)serverAlerts.get(sourceURI));
        } catch (NumberFormatException e) {

            if (logger.isLoggable(Logger.ERROR)) {
                String msg = "Unrecognized alert code from server: " +
                             serverAlerts.get(sourceURI)             +
                             ". Source "                             +
                             sourceURI                               +
                             " not synchronized"                     ;
                logger.error(msg);
            }

            fireSyncEvent(new
                SyncEvent(SyncEvent.SYNC_ERROR      ,
                          System.currentTimeMillis(),
                          e.getMessage()            ,
                          e                         ));
            return false;
        }

        return ((alertCode == AlertCode.REFRESH_FROM_SERVER           ) ||
                (alertCode == AlertCode.REFRESH_FROM_SERVER_BY_SERVER )
               );
    }

    /**
     * Updates the last anchor for the given source.
     *
     * @param sourceURI the source to be updated - NOT NULL
     * @param last the last timestamp
     *
     * @throws DMException in case of erros due to the DM
     */
    private void updateLastAnchor(String sourceURI, long last)
    throws DMException {
        int l = sources.length;
        for (int i=0; i < l; ++i) {
            if (sources[i].getValues() == null) {
                // this node is not a valid syncSource
                continue;
            }

            String sURI = (String)sources[i].getValue(SyncSourceDefinition.CONFIG_URI);

            if (sourceURI.equals(sURI)) {
                sources[i].setValue(SyncSourceDefinition.CONFIG_LAST,
                                    String.valueOf(nextTimestamp));
                break;
            }
        }
    }

    /**
     * Returns the server alert code for the given source.
     *
     * @param sourceURI the source
     *
     * @return the server alert code for the given source or -1 if it is not
     *         found/parsable
     */
    private int getSourceAlertCode(String sourceURI) {
        try {
            return Integer.parseInt((String)serverAlerts.get(sourceURI));
        } catch (Throwable t) {
             if (logger.isLoggable(Logger.ERROR)) {
                String msg = "Unrecognized server alert code (" +
                             serverAlerts.get(sourceURI)        +
                             ") for "                           +
                             sourceURI                          ;
                logger.error(msg);
             }

             fireSyncEvent(new
                 SyncEvent(SyncEvent.SYNC_ERROR      ,
                           System.currentTimeMillis(),
                           t.getMessage()            ,
                           t                         ));
        }
        return -1;
    }

    /**
     * Chooses the sync mode based on a last update timestamp and a preferred
     * sync mode.
     * <p>
     * The sync mode is choosen as follows:
     * <ul>
     *   <li>if the source has been already synced (lastUpdate <> 0),
     *       defaultSyncMode is returned
     *   <li>otherwise, the value stored in FIRST_TIME_SYNC_MODE configuration
     *       parameter is returned
     * </ul>
     *
     * @param lastUpdate the last update timestamp
     * @param defaultSyncMode the default syncmode specified by the source
     *
     * @return the choosen sync mode
     */
    private String chooseSyncMode(long lastUpdate, String defaultSyncMode) {
        if (lastUpdate == 0) {
            String firstTimeSyncMode =
                (String)mgrConfig.getFirstSyncMode();

            if (firstTimeSyncMode == null) {
                return SYNC_TWOWAY;
            }

            return firstTimeSyncMode;
        }

        return defaultSyncMode;
    }

    /**
     * Returns the complete classpath for classes in the lib directory (such as
     * the data store manager classes). If the given classpath is not an absolute
     * pathm the complete classpath is created starting from the first element
     * in the system classpath (which should be in the form <i>{palmpath}/Funambol</i>
     * with the given classpath directory.
     *
     * @param classpath the absolute/relative classpath - NOT NULL
     *
     * @return the given classpath if it is an absolute path or a completed classpath
     * as described in the class description.
     */
    private String getLibClasspath(String classpath) {
        if (classpath == null) {
            classpath = "";
        }
        if (new File(classpath).isAbsolute()) {
            return classpath;
        }

        String systemClasspath = System.getProperty(SyncConfiguration.PARAM_SYSTEM_CLASSPATH);

        String funambolClasspath =
            new StringTokenizer(systemClasspath, File.pathSeparator).nextToken();

        return funambolClasspath + File.separator + classpath;
    }

    /**
     * Returns contents from SyncItem, replace non-XML char by encode
     * @param item
     *
     * @return content
     */
    private String getContent(SyncItem item) {
        return StringTools.escapeXml(
            new String(
                (byte[])item.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT))
            );
    }

    /**
     * Fire SyncEvent to syncListeners.
     *
     * @param syncEvent
     */
    private void fireSyncEvent(SyncEvent syncEvent) {
        for (int i = 0, l = syncListeners.size(); i < l; i++) {
            if (syncEvent.getType() == SyncEvent.SYNC_BEGIN) {
                ((SyncListener)syncListeners.elementAt(i)).syncBegin(syncEvent);
            } else if (syncEvent.getType() == SyncEvent.SYNC_END) {
                ((SyncListener)syncListeners.elementAt(i)).syncEnd(syncEvent);
            } else if (syncEvent.getType() == SyncEvent.SYNC_ERROR) {
                ((SyncListener)syncListeners.elementAt(i)).syncError(syncEvent);
            } else if (syncEvent.getType() == SyncEvent.SEND_INITIALIZATION) {
                ((SyncListener)
                    syncListeners.elementAt(i)).sendInitialization(syncEvent);
            } else if (syncEvent.getType() == SyncEvent.SEND_MODIFICATION) {
                ((SyncListener)
                    syncListeners.elementAt(i)).sendModification(syncEvent);
            } else if (syncEvent.getType() == SyncEvent.SEND_FINALIZATION) {
                ((SyncListener)
                    syncListeners.elementAt(i)).sendFinalization(syncEvent);
            }
        }
    }

    /**
     * Fire SyncSourceEvent to syncSourceListeners.
     *
     * @param syncSourceEvent
     */
    private void fireSyncSourceEvent(SyncSourceEvent syncSourceEvent) {
        for (int i = 0, l = syncSourceListeners.size(); i < l; i++)  {
            if (syncSourceEvent.getType() == syncSourceEvent.SYNC_BEGIN) {
                ((SyncSourceListener)
                    syncSourceListeners.elementAt(i)).syncBegin(syncSourceEvent);
            } else if (syncSourceEvent.getType() == syncSourceEvent.SYNC_END) {
                ((SyncSourceListener)
                    syncSourceListeners.elementAt(i)).syncEnd(syncSourceEvent);
            }
        }
    }

    /**
     * Fire SyncItemEvent to syncItemListeners.
     *
     * @param syncItemEvent
     */
    private void fireSyncItemEvent(SyncItemEvent syncItemEvent) {
        int type = syncItemEvent.getType();
        for (int i = 0, l = syncItemListeners.size(); i < l; i++) {
            if (type == SyncItemEvent.ITEM_ADDED_BY_SERVER) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemAddedByServer(syncItemEvent);
            } else if (type == SyncItemEvent.ITEM_DELETED_BY_SERVER) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemDeletedByServer(syncItemEvent);
            } else if (type == SyncItemEvent.ITEM_UPDATED_BY_SERVER) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemUpdatedByServer(syncItemEvent);
            } else if (type == SyncItemEvent.ITEM_ADDED_BY_CLIENT) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemAddedByClient(syncItemEvent);
            } else if (type == SyncItemEvent.ITEM_DELETED_BY_CLIENT) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemDeletedByClient(syncItemEvent);
            } else if (type  == SyncItemEvent.ITEM_UPDATED_BY_CLIENT) {
                ((SyncItemListener)
                    syncItemListeners.elementAt(i)).
                        itemUpdatedByClient(syncItemEvent);
            }
        }
    }

    /**
     * Fire SyncStatusEvent to syncStatusListeners.
     *
     * @param syncStatusEvent
     */
    private void fireSyncStatusEvent(SyncStatusEvent syncStatusEvent) {
        int type = syncItemEvent.getType();
        for (int i = 0, l = syncStatusListeners.size(); i < l; i++) {
            if (type == syncStatusEvent.STATUS_TO_SEND) {
                ((SyncStatusListener)
                    syncStatusListeners.elementAt(i)).
                        statusToSend(syncStatusEvent);
            } else if (type == syncStatusEvent.STATUS_RECEIVED) {
                ((SyncStatusListener)
                    syncStatusListeners.elementAt(i)).
                        statusReceived(syncStatusEvent);
            }
        }
    }

    /**
     * Initializes the Hashtable of SyncSourceDefinition and the SyncSourceFactory
     *
     */
    private void initSyncSources() throws SyncException {
        Vector sourceURIVector  = new Vector();
        Vector sourceNameVector = new Vector();
        Vector sourceTypeVector = new Vector();

        SyncSourceDefinition ssd = null;

        syncSourceDefinitions   = new Hashtable();
        Hashtable sourceConfig  = null;

        //
        // Read sync sources configuration and records their uri and name in the
        // instance variable sourceURIs and sourceNames.
        // Note that only the sources whose default-sync is not NONE are
        // stored.
        //
        String uri;

        int l = sources.length;
        for (int i=0; i < l; ++i) {
            try {
                sourceConfig  = sources[i].getValues();

                if (sourceConfig == null) {
                  // this node is not a valid syncSource
                  continue;
                }

                ssd = new SyncSourceDefinition(sourceConfig);
                if (SYNC_NONE.equals(ssd.getDefaultSync())) {
                    continue;
                }

                uri = (String)sourceConfig.get(SyncSourceDefinition.CONFIG_URI);
                sourceURIVector.addElement(uri);

                sourceNameVector.addElement(
                    sourceConfig.get(SyncSourceDefinition.CONFIG_NAME));

                sourceTypeVector.addElement(
                    sourceConfig.get(SyncSourceDefinition.CONFIG_TYPE));

            } catch (DMException e) {
                throw new SyncException( "Configuration error for node "
                                       + sources[i].getFullContext()
                                       + ": "
                                       + e.getMessage()
                                       , e);
            }
            syncSourceDefinitions.put(uri, ssd);
        }

        //
        // Store sync source uri, name and type for later use
        //
        l = sourceURIVector.size();

        sourceURIs  = new String[l];
        sourceNames = new String[l];
        sourceTypes = new String[l];

        for (int i=0; i < l; ++i) {
            sourceURIs [i] = (String)sourceURIVector.elementAt (i);
            sourceNames[i] = (String)sourceNameVector.elementAt(i);
            sourceTypes[i] = (String)sourceTypeVector.elementAt(i);
        }

        syncSourceFactory = new SyncSourceFactory(
            getLibClasspath(mgrConfig.getClasspath()),
            syncSourceDefinitions
        );
    }
}
