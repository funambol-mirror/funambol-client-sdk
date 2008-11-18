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
package com.funambol.syncml.spds;

import java.io.UnsupportedEncodingException;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.funambol.util.Base64;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;

import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.syncml.protocol.SyncMLCommand;
import com.funambol.syncml.protocol.SyncFilter;
import com.funambol.util.XmlUtil;
import com.funambol.util.XmlException;
import com.funambol.util.BasicSyncListener;
import com.funambol.util.SyncListener;
import com.funambol.util.ChunkedString;

/**
 * The SyncManager is the engine of the synchronization process on the
 * client library. It initializes the sync, checks the server responses
 * and communicate with the SyncSource, which is the client-specific
 * source of data.
 * A client developer must prepare a SyncConfig to istantiate a
 * SyncManager, and then can sync its sources calling the sync()
 * method.
 */
public class SyncManager {

    //------------------------------------------------------------- Private data
    /* Fast sync sending add state*/
    private static final int STATE_SENDING_ADD = 1;
    /* Fast sync sending update state*/
    private static final int STATE_SENDING_REPLACE = 2;
    /* Fast sync sending delete state*/
    private static final int STATE_SENDING_DELETE = 3;
    /* Fast sync modification complete state*/
    private static final int STATE_MODIFICATION_COMPLETED = 4;
    /* SyncManager configuration*/
    private SyncConfig config;
    /* SyncSource to sync*/
    private SyncSource source;
    /* Device ID taken from DeviceConfig*/
    private String deviceId;
    /* Max SyncML Message Size taken from DeviceConfig*/
    private int maxMsgSize;
    /**
     * A flag indicating if the client has to prepare the <DevInf> part of the
     * initialization SyncML message containing the device capabilities. It can
     * be set to <code>true</code> in two falls:
     *
     * a) the <code>serverUrl</code> isn't on the list of the already
     * connected servers
     *
     * b) the device configuration is changed
     */
    private boolean sendDevInf = false;
    /**
     * A flag indicating if the client has to add the device capabilities to the
     * modification message as content of a <Results> element. This occurs when
     * the server has sent a <Get> command request, sollicitating items of type
     * './devinf12'
     */
    private boolean addDevInfResults = false;
    /**
     * String containing the last Url of the server the client was connected to
     */
    private String lastServerUrl;
    /**
     * The value of the <CmdID> element of <Get>, to be used building the
     * <Results> command
     */
    private String cmdIDget = null;
    /**
     * The value of the <MsgID> element of the message in which <Get> is, to be
     * used building the <Results> command
     */
    private String msgIDget = null;
    // state used for fast sync
    int state;
    // The alerts sent by server, indexed by source name, instantiated in
    // checkServerAlerts
    private Hashtable serverAlerts;
    // The alert code for the current source (i.e. the actual sync mode
    // eventually modified by ther server
    private int alertCode;
    // Server URL modified with session id.
    private String serverUrl;
    // FIXME: ca be avoided?
    private String login = null;
    private String sessionID = null;
    /**
     * This member stores the LUID/GUID mapping for the items added
     * to the current source during the sync.
     */
    private Hashtable mappings = null;
    /**
     * This member stores the Status commands to send back to the server
     * in the next message. It is modified at each item received,
     * and is cleared after the status are sent.
     */
    private Vector statusList = null;
    /**
     * This member is used to store the current message ID.
     * It is sent to the server in the MsgID tag.
     */
    private int msgID = 0;
    /**
     * This member is used to store the current command ID.
     * It is sent to the server in the CmdID tag.
     */
    private int cmdID = 0;
    /**
     * A single HttpTransportAgent for all the operations
     * performed in this Sync Manager
     */
    private HttpTransportAgent transportAgent;
    private static final int PROTOCOL_OVERHEAD = 3072;
    /**
     * This member is used to indicate if the SyncManager is busy, that is
     * if a sync is on going (SyncManager supports only one synchronization
     * at a time, and requests are queued in the synchronized method sync
     */
    private boolean busy;
    /**
     * Synchronization listener
     */
    private SyncListener listener;
    /**
     * Unique instance of a BasicSyncListener which is used when the user does
     * not set up a listener in the SyncSource. In order to avoid the creation
     * of multiple instances of this class we use this static variable
     */
    private static SyncListener basicListener = null;

    //------------------------------------------------------------- Constructors
    /**
     * SyncManager constructor
     *
     * @param conf is the configuration data filled by the client
     *
     */
    public SyncManager(SyncConfig conf) {
        this.config = conf;
        this.login = conf.userName + ":" + conf.password;
        this.source = null;

        // Cache device info
        this.deviceId = config.deviceConfig.devID;
        this.maxMsgSize = config.deviceConfig.maxMsgSize;

        this.state = 0;
        this.serverAlerts = null;
        this.alertCode = 0;

        // mapping table
        this.mappings = null;

        this.busy = false;

        // status commands
        statusList = null;
        transportAgent =
                new HttpTransportAgent(
                config.syncUrl,
                config.userAgent,
                "UTF-8",
                conf.compress, conf.forceCookies);
    }

    //----------------------------------------------------------- Public methods
    /**
     * Synchronizes synchronization source, using the preferred sync
     * mode defined for that SyncSource.
     *
     * @param source the SyncSource to synchronize
     *
     * @throws SyncException
     *                  If an error occurs during synchronization
     *
     */
    public void sync(SyncSource source) throws SyncException {
        sync(source, source.getSyncMode());
    }

    /**
     * Synchronizes synchronization source
     *
     * @param source the SyncSource to synchronize
     * @param syncMode the sync mode
     * @throws SyncException
     *                  If an error occurs during synchronization
     */
    public synchronized void sync(SyncSource src, int syncMode)
            throws SyncException {


        
        busy = true;

        // Get the SyncListener associated to the source.
        // If it does not exist we use the dummy listener
        listener = src.getListener();
        if (listener == null) {
            if (basicListener == null) {
                listener = new BasicSyncListener();
                basicListener = listener;
            } else {
                listener = basicListener;
            }
        }

        // Notifies the listener that a new sync is about to start
        listener.startSession();

        if (syncMode == SyncML.ALERT_CODE_NONE) {
            Log.info("Source not active.");
            listener.endSession(SyncSource.STATUS_SUCCESS);
            return;
        }

        try {
            String response = null;

            // Set source attribute
            this.source = src;

            // Set initial state
            nextState(STATE_SENDING_ADD);

            //Set NEXT Anchor referring to current timestamp
            this.source.setNextAnchor(System.currentTimeMillis());

            this.sessionID = String.valueOf(System.currentTimeMillis());
            this.serverUrl = config.syncUrl;

            //deciding if the device capabilities have to be sent
            if (isNewServerUrl(serverUrl)) {
                setFlagSendDevInf();
            }

            // ================================================================
            // Initialization phase
            // ================================================================

            Log.info("Sending init message");
            listener.startConnecting();
            //Format request message to be sent to the server
            String initMsg = prepareInizializationMessage(syncMode);
            Log.debug(initMsg);

            response = postRequest(initMsg);
            initMsg = null;

            Log.info("Response received");
            Log.debug("Response: " + response);

            // TODO: today the SyncSource does not need to process this data.
            // When we support large object we may want to change this
            listener.dataReceived(transportAgent.getResponseDate(),
                    response.length());

            ChunkedString chunkedResp = new ChunkedString(response);
            // Check server response (can throws exception and break the sync)
            checkStatus(chunkedResp, SyncML.TAG_SYNCHDR);
            checkStatus(chunkedResp, SyncML.TAG_ALERT);

            // client interpretes server alerts and store them into "serverAlerts"
            checkServerAlerts(chunkedResp);

            // save the alert code for the current source
            String name = source.getName();
            Log.debug(name);
            alertCode = getSourceAlertCode(name);
            Log.info("Alert code: " + alertCode);
            Log.info("Initialization succesfully completed");
            listener.endConnecting(alertCode);

            // if the server has required device capabilities in the response, these
            // are added within the next client request in the <Results> method
            addDevInfResults = isGetCommandFromServer(chunkedResp);

            // Get the server URL with the session ID
            try {
                serverUrl = XmlUtil.getTagValue(chunkedResp, "RespURI").toString();
            } catch (XmlException xe) {
                Log.error("Error parsing RespURI from server " + xe.toString());
                throw new SyncException(
                        SyncException.SERVER_ERROR,
                        "Cannot find the Response URI in server response.");
            }

            chunkedResp = null;

            response = null;


            // ================================================================
            // Sync phase
            // ================================================================
                    
            // init mapping table
            this.mappings = new Hashtable();
            // init status commands list
            this.statusList = new Vector();

            // Notifies that the synchronization is going to begin

            boolean ok = listener.startSyncing(alertCode);

            if (!ok) {
                //User Aborts the slow sync request
                Log.info("[SyncManager] Sync process aborted by the user");
                return;
            }


            source.beginSync(alertCode);
            listener.syncStarted(alertCode);

            boolean done = false;

            // the implementation of the client/server multi-messaging
            // through a do while loop: while </final> tag is reached.
            do {
                listener.startSending(source.getClientAddNumber(),
                        source.getClientReplaceNumber(),
                        source.getClientDeleteNumber());
                String modificationsMsg = prepareModificationMessage();
                Log.info("Sending modification");
                Log.debug(modificationsMsg);

                response = postRequest(modificationsMsg);
                
                modificationsMsg = null;

                Log.info("response received");
                Log.debug(response);
                listener.endSending();

                //listener.dataReceived(transportAgent.getResponseDate(),
                //                      response.length());

                // The startReceiving(n) is notified from within the
                // processModifications because here we do not know the number
                // of messages to be received
                processModifications(new ChunkedString(response));

                done = ((response.indexOf("<Final/>") >= 0) ||
                        (response.indexOf("</Final>") >= 0));

                response = null;

                listener.endReceiving();
                

            } while (!done);

            Log.info("Modification session succesfully completed");
            listener.endSyncing();

            // ================================================================
            // Mapping phase
            // ================================================================

            listener.startMapping();
            
            // Send the map message only if a mapping or a status has to be sent
            if (statusList.size() > 0 || mappings.size() > 0) {
                String mapMsg = prepareMappingMessage();
                
                Log.info("Sending Mappings\n");
                Log.debug(mapMsg);

                try {
                    response = postRequest(mapMsg);
                } catch (ReadResponseException rre) {
                    source.setLastAnchor(source.getNextAnchor());
                    //save last anchors if the mapping message has been sent but
                    //the response has not been received due to network problems
                    Log.info("[SyncManager] Last sync message sent - Error reading the response " + rre);
                }
                
                mapMsg = null;

                if (response!=null) {
                    Log.info("response received");
                    Log.debug(response);

                    //listener.dataReceived(
                    //      transportAgent.getResponseDate(), response.length());

                    // Check server response (can throws exception to the caller)
                    checkStatus(new ChunkedString(response), SyncML.TAG_SYNCHDR);

                    response = null;
                } else {
                    Log.info("Response not received");
                    Log.info("Skipping check for status");
                    
                }

                Log.info("Mapping session succesfully completed");

            } else {
                Log.info("No mapping message to send");

            }
            
            // TODO: following code must be run only for succesfull path or error reading inputstream
            //       the other cases must skip the following code
            Log.debug("Notifying listener end mapping");
            listener.endMapping();

            Log.debug("Changing anchors");
            // Set the last anchor to the next timestamp for the source
            source.setLastAnchor(source.getNextAnchor());
            
            Log.debug("source endSsync method call");
            // Tell the source that the sync is finished
            source.endSync();

        } catch (CompressedSyncException compressedSyncException) {
            Log.error("[SyncManager] CompressedSyncException: " + compressedSyncException);
            //releaseResources();
            throw compressedSyncException;
        } finally {
            // Notifies the listener that the session is over
            Log.debug("Ending session");
            listener.endSession(source.getStatus());
            releaseResources();
        }

    }

    private void releaseResources() {
        // Release resources
        this.mappings = null;
        this.statusList = null;

        this.source = null;
        this.sessionID = null;
        this.serverUrl = null;

        this.busy = false;
    }

    /**
     * To be invoked by every change of the device configuration and if the
     * serverUrl is a new one (i.e., not already on the list
     * <code>lastServerUrl</code>
     */
    public void setFlagSendDevInf() {
        sendDevInf = true;
    }

    public boolean isBusy() {
        return busy;
    }

    //---------------------------------------------------------- Private methods
    /**
     * Checks if the current server URL is the same as by the last connection.
     * If not, the current server URL is persisted in a record store on the
     * device
     *
     * @param url
     *            The server URL coming from the SyncConfig
     * @return true if the client wasn't ever connected to the corresponding
     *         server, false elsewhere
     */
    private boolean isNewServerUrl(String url) {

        //retrieve last server URL from the configuration
        lastServerUrl = config.lastServerUrl;

        if (StringUtil.equalsIgnoreCase(lastServerUrl, url)) {
            // the server url is the same as by the last connection, the client
            // may not send the device capabilities
            return false;
        } else {
            // the server url is new, the value has to be stored (this is let to
            // the SyncmlMPIConfig, while the SyncConfig isn't currently stored)
            return true;//the url is different, client can send the device info
        }
    }

    /**
     * Posts the given message to the url specified by <code>serverUrl</code>.
     *
     * @param request the request msg
     * @return the response of the server as a string
     *
     * @throws SyncException in case of network errors (thrown by sendMessage)
     */
    private String postRequest(String request) throws SyncException {
        transportAgent.setRequestURL(serverUrl);
        return transportAgent.sendMessage(request);
    }

    /**
     * TODO: check CHAL element in the 401 status returned.
     */
    private boolean checkMD5(String msg) {
        return false;
    }

    /**
     * Checks if the given response message is authenticated by the server
     *
     * @param msg the message to be checked
     * @param statusOf the command of which we want to check the status
     *
     * @throws SyncException in case of other errors
     */
    private void checkStatus(ChunkedString msg, String statusOf)
            throws SyncException {

        Vector statusTags = null;
        try {
            statusTags = XmlUtil.getTagValues(
                    XmlUtil.getTagValues(
                    XmlUtil.getTagValues(msg, SyncML.TAG_SYNCML),
                    SyncML.TAG_SYNCBODY),
                    SyncML.TAG_STATUS);
        } catch (XmlException xe) {
            xe.printStackTrace();
            Log.error("checkStatus: error parsing server status " + msg);
            return;
        }


        for (int i = 0,  l = statusTags.size(); i < l; i++) {
            ChunkedString tag = (ChunkedString) statusTags.elementAt(i);
            // Parse the status
            SyncMLStatus status = SyncMLStatus.parse(tag);

            if (status != null) {
                // Consider only that status for the requested command
                if (statusOf.equals(status.getCmd())) {
                    switch (status.getStatus()) {

                        case SyncMLStatus.SUCCESS:                      // 200
                            return;
                        case SyncMLStatus.REFRESH_REQUIRED:             // 508
                            Log.info("Refresh required by server.");
                            return;
                        case SyncMLStatus.AUTHENTICATION_ACCEPTED:      // 212
                            Log.info("Authentication accepted by the server.");
                            return;
                        case SyncMLStatus.INVALID_CREDENTIALS:          // 401
                            // TODO: handle MD5 authentication.
                            if (checkMD5(msg.toString())) {
                                Log.error("MD5 authentication not supported");
                                throw new SyncException(
                                        SyncException.AUTH_ERROR,
                                        "MD5 authentication not supported");
                            } else {
                                Log.error("Invalid credentials: " + config.userName);

                                throw new SyncException(
                                        SyncException.AUTH_ERROR,
                                        "Authentication failed for: " + source.getSourceUri());
                            }
                        case SyncMLStatus.FORBIDDEN:                    // 403
                            throw new SyncException(
                                    //SyncException.AUTH_ERROR,
                                    SyncException.FORBIDDEN_ERROR,
                                    "User not authorized: " + config.userName + " for source: " + source.getSourceUri());
                        case SyncMLStatus.NOT_FOUND:                    // 404
                            Log.error(this, "Source URI not found on server: " + source.getSourceUri());
                            throw new SyncException(
                                    //SyncException.ACCESS_ERROR,
                                    SyncException.NOT_FOUND_URI_ERROR,
                                    "Source URI not found on server: " + source.getSourceUri());
                        case SyncMLStatus.SERVER_BUSY:                  // 503
                            throw new SyncException(
                                    SyncException.SERVER_BUSY,
                                    "Server busy, another sync in progress for " + source.getSourceUri());
                        case SyncMLStatus.PROCESSING_ERROR:             // 506
                            throw new SyncException(
                                    SyncException.BACKEND_ERROR,
                                    "Error processing source: " + source.getSourceUri() + status.getStatusDataMessage());
                        case SyncMLStatus.BACKEND_AUTH_ERROR:             // 506
                            throw new SyncException(
                                    SyncException.BACKEND_AUTH_ERROR,
                                    "Error processing source: " + source.getSourceUri() + status.getStatusDataMessage());
                        default:
                            // Unhandled status code
                            Log.debug("[SyncManger] Unhandled Status Code, throwing exception");
                            throw new SyncException(
                                    SyncException.SERVER_ERROR,
                                    "Error from server: " + status.getStatus());
                    }
                }
            }

        }

        // Should neven happen
        Log.error("checkStatus: can't find Status in " + statusOf + " in server response");
        throw new SyncException(
                SyncException.SERVER_ERROR,
                "Status Tag for " + statusOf + " not found in server response");
    }

    /**
     * <p>Checks response status for the synchronized databases and saves their
     * serverAlerts
     * <p>If this is the first sync for the source, the status code might change
     * according to the value of the PARAM_FIRST_TIME_SYNC_MODE configuration
     * property
     * <p>If firstTimeSyncMode is not set, the alert is left unchanged. If it is
     * set to a value, the specified value is used instead
     *
     * @param msg The message to be checked
     *
     * @throws SyncException In case of errors
     **/
    private void checkServerAlerts(ChunkedString msg) throws SyncException {
        ChunkedString target = null;
        ChunkedString code = null;
        Vector alertTags = null;

        serverAlerts = new Hashtable();

        try {
            alertTags = XmlUtil.getTagValues(
                    XmlUtil.getTagValues(
                    XmlUtil.getTagValues(
                    msg,
                    SyncML.TAG_SYNCML),
                    SyncML.TAG_SYNCBODY),
                    SyncML.TAG_ALERT);

            for (int i = 0,  l = alertTags.size(); i < l; i++) {
                ChunkedString alert = (ChunkedString) alertTags.elementAt(i);
                code = XmlUtil.getTagValue(alert, SyncML.TAG_DATA);
                Vector items = XmlUtil.getTagValues(alert, SyncML.TAG_ITEM);

                for (int j = 0,  m = items.size(); j < m; j++) {
                    ChunkedString targetTag = (ChunkedString) items.elementAt(j);
                    target = XmlUtil.getTagValue(targetTag, SyncML.TAG_TARGET);

                    target = XmlUtil.getTagValue(target, SyncML.TAG_LOC_URI);
                    Log.info("The server alert code for " + target + " is " + code);
                    serverAlerts.put(target.toString(), code.toString());
                }
            }
        } catch (XmlException xe) {
            Log.error("checkServerAlerts: error parsing server alert " + msg);
            xe.printStackTrace();
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Invalid alert from server.");
        }
    }

    /**
     * Prepare a SyncML Message header.
     *
     * @param sessionid the session id to use.
     * @param msgid the message id to use.
     * @param src the source uri
     * @param tgt the target uri
     * @param tags other SyncML tags to insert in the header.
     *             (e.g. &lt;Cred&gt; or &lt;Meta&gt;).
     */
    private String prepareSyncHeader(String sessionid,
            String msgid,
            String src,
            String tgt,
            String tags) {

        StringBuffer ret = new StringBuffer();

        ret.append("<SyncHdr>\n").append("<VerDTD>1.2</VerDTD>\n").append("<VerProto>SyncML/1.2</VerProto>\n").append("<SessionID>").append(sessionid).append("</SessionID>\n").append("<MsgID>").append(msgid).append("</MsgID>\n").append("<Target><LocURI>").append(tgt).append("</LocURI></Target>\n").append("<Source><LocURI>").append(src).append("</LocURI></Source>\n");

        if (tags != null) {
            ret.append(tags);
        }

        ret.append("</SyncHdr>\n");

        return ret.toString();
    }

    /**
     * Prepares inizialization SyncML message
     */
    private String prepareInizializationMessage(int syncMode)
            throws SyncException {


        String b64login = new String(Base64.encode(login.getBytes()));

        StringBuffer ret = new StringBuffer("<SyncML>\n");

        // Add <Cred> and <Meta> to the syncHdr
        StringBuffer tags = new StringBuffer("<Cred>\n");
        tags.append("<Meta>").append("<Type xmlns=\"syncml:metinf\">syncml:auth-basic</Type>\n").append("<Format xmlns=\"syncml:metinf\">b64</Format>\n").append("</Meta>\n").append("<Data>").append(b64login).append("</Data>").append("</Cred>\n");
        // Meta for the maxmsgsize
        tags.append("<Meta><MaxMsgSize>").append(maxMsgSize).append("</MaxMsgSize></Meta>\n");

        // Add SyncHdr
        ret.append(prepareSyncHeader(sessionID, resetMsgID(),
                deviceId, serverUrl,
                tags.toString()));
        // Add SyncBody
        ret.append("<SyncBody>\n").append(createAlerts(source, syncMode));

        // Add DevInf
        if (sendDevInf) {
            ret.append(createPut(config.deviceConfig));
        }

        ret.append("<Final/>").append("</SyncBody>\n");

        ret.append("</SyncML>\n");

        tags = null;

        return ret.toString();
    }

    /**
     * Process the &lt;Format&gt; tag and return the requested modification
     * in a String array.
     */
    private String[] processFormat(ChunkedString xml) {
        String[] ret = null;

        try {
            if (XmlUtil.getTag(xml, "Format") != -1) {
                ChunkedString format = XmlUtil.getTagValue(xml, "Format");

                if (format != null && !format.equals("")) {
                    ret = StringUtil.split(format.toString(), ";");
                }
            }
        } catch (XmlException e) {
            Log.error("[processFormat] Error parsing format from server: " + xml + ". Ignoring it.");
            e.printStackTrace();
        }
        return ret;
    }

    /**
     * Get the format string to add to the outgoing message.
     *
     * @return the Format string, according to the source encoding
     */
    private String getFormat() {
        // Get the Format tag from the SyncSource encoding.
        if (!source.getEncoding().equals(source.ENCODING_NONE)) {
            return "<Format xmlns=\'syncml:metinf\'>" + source.getEncoding() + "</Format>\n";
        } else {
            return "";
        }
    }

    /**
     * Encode the item data according to the format specified by the SyncSource.
     *
     * @param formats the list of requested encodings (des, 3des, b64)
     * @param data the byte array of data to encode
     * @return the encoded byte array, or <code>null</code> in case of error
     */
    private byte[] encodeItemData(String[] formats, byte[] data) {

        if (formats != null && data != null) {
            // If ecryption types are specified, apply them
            for (int count = formats.length - 1; count >= 0; count--) {

                String encoding = formats[count];

                if (encoding.equals("b64")) {
                    data = Base64.encode(data);
                }
            /*
            else if (encoding.equals("des")) {
            // DES not supported now, ignore SyncSource encoding
            }
            else if (currentDecodeType.equals("3des")) {
            // 3DES not supported now, ignore SyncSource encoding
            }
             */
            }
        }
        return data;
    }

    /**
     * Decode the item data according to the format specified by the server.
     *
     * @param formats the list of requested decodings (des, 3des, b64)
     * @param data the byte array of data to decode
     * @return the decode byte array, or <code>null</code> in case of error
     *
     * @throws UnsupportedEncodingException
     */
    private byte[] decodeItemData(String[] formats, byte[] data)
            throws UnsupportedEncodingException {

        if (formats != null && data != null) {
            // If ecryption types are specified, apply them
            for (int count = formats.length - 1; count >= 0; count--) {

                String currentDecodeType = formats[count];

                if (currentDecodeType.equals("b64")) {
                    data = Base64.decode(data);
                } else if (currentDecodeType.equals("des")) {
                    // Error, DES not supported now, send error to the server
                    return null;
                /*
                desCrypto = new Sync4jDesCrypto(Base64.encode(login.getBytes()));
                data = desCrypto.decryptData(data);
                 */
                } else if (currentDecodeType.equals("3des")) {
                    // Error, 3DES not supported now, send error to the server
                    return null;
                /*
                sync3desCrypto = new Sync4j3DesCrypto(Base64.encode(login.getBytes()));
                data = sync3desCrypto.decryptData(data);
                 */
                }
            }
        }
        return data;
    }

    /**
     * Get an item from the SyncML tag.
     *
     * @param command The name command from server
     * @param type the mime type of the item
     * @param xmlItem the SyncML tag for this item
     *
     * @return the SyncItem object
     *
     * @throws SyncException if the command parsing failed
     *
     */
    private SyncItem getItem(char state, String type, ChunkedString xmlItem, String[] formatList) throws SyncException {
        String key = null;
        String data = null;
        String parent = null;
        byte[] content = null;

        // Get item key
        try {
            key = XmlUtil.getTagValue(xmlItem, SyncML.TAG_LOC_URI).toString();
        } catch (XmlException e) {
            Log.error("[getItem] Invalid item key from server: " + xmlItem);
            e.printStackTrace();
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Invalid item key from server.");
        }

        // Get item parent, if present
        if (XmlUtil.getTag(xmlItem, SyncML.TAG_TARGET_PARENT) > 0) {
            try {
                parent = XmlUtil.getTagValue(
                        XmlUtil.getTagValue(
                        xmlItem, SyncML.TAG_TARGET_PARENT),
                        SyncML.TAG_LOC_URI).toString();
            } catch (XmlException e) {
                Log.error("[getItem] Invalid item parent from server: " + e.toString());
                throw new SyncException(
                        SyncException.SERVER_ERROR,
                        "Invalid item parent from server.");
            }
        }

        // Get the item data, if present
        if (XmlUtil.getTag(xmlItem, SyncML.TAG_DATA) != -1) {
            try {
                // Get item data
                data = XmlUtil.getTagValue(xmlItem, SyncML.TAG_DATA).toString();
                if (formatList != null) {
                    // Format tag from server
                    content = decodeItemData(formatList, data.getBytes());
                } else if (!source.getEncoding().equals(source.ENCODING_NONE)) {
                    // If the server does not send a format, apply the one
                    // defined for the sync source
                    formatList = processFormat(new ChunkedString(source.getEncoding()));
                    content = decodeItemData(formatList, data.toString().getBytes());
                } else {
                    // Else, the data is text/plain,
                    // and the XML special chars are escaped.
                    // The encoding must be set to UTF-8
                    // in order to read the symbols like "euro"
                    content = XmlUtil.unescapeXml(data).getBytes("UTF-8");
                }
            } catch (UnsupportedEncodingException uee) {
                uee.printStackTrace();
                Log.error("[getItem] Can't decode content for item: " + key);
                // in case of error, the content is null
                // and this will be reported as an error to the server
                content = null;
            } catch (XmlException xe) {
                xe.printStackTrace();
                Log.error("[getItem] Can't parse data tag for item: " + key);
                // in case of error, the content is null
                // and this will be reported as an error to the server
                content = null;
            }
        }

        // Create the item
        return new SyncItem(
                key, type, state, parent, content);
    }

    /**
     * Processes an item in a modification command received from server.
     *
     * @param cmd the cmmand info
     * @param xmlItem the SyncML tag for this item
     * @return the status code for this item
     *
     * @throws SyncException if the command parsing failed
     *
     */
    private SyncMLStatus processSyncItem(SyncMLCommand cmd, ChunkedString xmlItem, String [] formatList)
            throws SyncException {
        int status = 0;
        SyncItem item = null;
        String guid = null;

        String cmdTag = cmd.getName();

        if (cmdTag.equals(SyncML.TAG_ADD)) {
            item = this.getItem(item.STATE_NEW, cmd.getType(), xmlItem, formatList);
            // Save the key sent by server, it will be replaced by the SyncSource
            // with the local UID.
            guid = new String(item.getKey());   // Duplicate to avoid leaks!!
            // Preliminary check: don't pass a null item to the SyncSource
            // for add
            if (item.getContent() != null) {
                status = source.addItem(item);
                if (SyncMLStatus.isSuccess(status)) {
                    listener.itemReceived(item.getClientRepresentation());
                }
            } else {
                status = SyncMLStatus.GENERIC_ERROR;
            }
            if (SyncMLStatus.isSuccess(status)) {
                mappings.put(new String(item.getKey()), guid); // Avoid leaks!!
            }

        } else if (cmdTag.equals(SyncML.TAG_REPLACE)) {
            item = this.getItem(item.STATE_UPDATED, cmd.getType(), xmlItem, formatList);
            // Preliminary check: don't pass a null item to the SyncSource
            // for update
            if (item.getContent() != null) {
                status = source.updateItem(item);
                if (SyncMLStatus.isSuccess(status)) {
                    listener.itemUpdated(item.getKey(),
                            item.getClientRepresentation());
                }
            } else {
                status = SyncMLStatus.GENERIC_ERROR;
            }
        } else if (cmdTag.equals(SyncML.TAG_DELETE)) {
            item = this.getItem(item.STATE_DELETED, cmd.getType(), xmlItem, formatList);
            status = this.source.deleteItem(item.getKey());
            if (SyncMLStatus.isSuccess(status)) {
                listener.itemDeleted(item.getKey());
            }
        } else {
            Log.error("[processItem] Invalid command: " + cmd.toString());
        }

        // Init the status object
        SyncMLStatus ret = new SyncMLStatus();
        ret.setCmd(cmdTag);
        ret.setCmdRef(cmd.getCmdId());
        // Save the source ref if present (ADD), otherwise the target ref (UPD & DEL)
        if (guid != null) {
            ret.setSrcRef(guid);
        } else {
            ret.setTgtRef(item.getKey());
        }
        ret.setStatus(status);

        return ret;
    }

    /**
     * Processes a modification command received from server,
     * returning the command parts in an Hashtable
     *
     * @param msgRef The messageId tag of the message containing this command
     * @param cmdName the command name
     * @param command the body of the command
     *
     * @return the number of modifications made
     *
     * @throws SyncException if the command parsing failed
     *
     */
    private int processCommand(ChunkedString msgRef, String cmdName, ChunkedString command)
            throws SyncException {

        ChunkedString cmdId = null;
        int i = 0;

        // Get command Id
        try {
            cmdId = XmlUtil.getTagValue(command, SyncML.TAG_CMDID);
        } catch (XmlException e) {
            Log.error("[processCommand] Invalid command Id from server: " + command);
            e.printStackTrace();
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Invalid command from server.");
        }

        SyncMLCommand cmd = new SyncMLCommand(cmdName, cmdId.toString());

        try {
            // Get the type of the items for this command, if present
            // otherwise use the type defined for this source.
            int pos = XmlUtil.getTag(command, SyncML.TAG_TYPE);
            String itemType = null;

            if (pos != -1) {
                try {
                    itemType = XmlUtil.getTagValue(command, SyncML.TAG_TYPE).toString();
                //int begin = command.indexOf(">", pos);
                //int end = command.indexOf("</"++">", begin);
                //if(begin != -1 && end != -1) {
                //    itemType = command.substring(begin+1, end).toString();
                //}
                } catch (XmlException xe) {
                    xe.printStackTrace();
                    Log.error("Error parsing item type, using default for source.");
                }
            }
            // Set the command type or use the source one
            if (itemType != null) {
                cmd.setType(itemType);
            } else {
                cmd.setType(source.getType());
            }

            // Process format tag (encryption and encoding)
            String[] formatList = this.processFormat(command);

            Vector itemTags = XmlUtil.getTagValues(command, SyncML.TAG_ITEM);
            int len = itemTags.size();

            // Process items
            SyncMLStatus status = null;
            for (i = 0; i < len; i++) {
                status = this.processSyncItem(cmd, (ChunkedString) itemTags.elementAt(i), formatList);
                status.setMsgRef(msgRef.toString());
                statusList.addElement(status);
            }

        } catch (XmlException xe) {
            xe.printStackTrace();
            Log.error("[processCommand] Parse error: " + xe.toString());
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Error processing command:" + cmdName + " in message " + msgRef);
        }


        return i;
    }

    /**
     * Processes the modifications from the received response from server
     *
     * @param modifications The modification message from server
     * @return true if a response message is required, false otherwise
     * @throws SyncException
     */
    private boolean processModifications(ChunkedString modifications)
            throws SyncException {
        boolean ret = false;

        ChunkedString msgId = null;
        ChunkedString bodyTag = null;

        try {
            // Check the SyncML tag
            if (XmlUtil.getTag(modifications, SyncML.TAG_SYNCML) == -1) {
                Log.error("Invalid message from server.");
                throw new SyncException(
                        SyncException.SERVER_ERROR,
                        "Invalid message from server.");
            }
            // Process header
            ChunkedString syncHdr = XmlUtil.getTagValue(modifications,
                    SyncML.TAG_SYNCHDR);
            // Get message id
            msgId = XmlUtil.getTagValue(syncHdr, SyncML.TAG_MSGID);

            // Get message body
            bodyTag = XmlUtil.getTagValue(modifications, SyncML.TAG_SYNCBODY);

        } catch (XmlException e) {
            Log.error("[processModification] error parsing message: " + e.toString());
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Error parsing message: " + e.getMessage());
        }

        // Process body
        Vector cmdTags = null;
        Vector xmlBody = new Vector(1);
        xmlBody.addElement(bodyTag);

        // Ignore incoming modifications for one way from client modes
        if (alertCode != SyncML.ALERT_CODE_ONE_WAY_FROM_CLIENT &&
                alertCode != SyncML.ALERT_CODE_REFRESH_FROM_CLIENT) {

            try {

                // If the server sends a <Sync> tag, the message contains
                // modifications, otherwise it contains only status.
                if (XmlUtil.getTag(bodyTag, SyncML.TAG_SYNC) != - 1) {
                    ret = true;
                    String[] cmdNames = {SyncML.TAG_ADD,
                        SyncML.TAG_REPLACE,
                        SyncML.TAG_DELETE
                    };

                    processSyncCommand(msgId, bodyTag);
                    bodyTag = null;

                    // Process commands, one kind at a time, in the order
                    // defined above
                    for (int c = 0; c < cmdNames.length; c++) {
                        int count = 0;
                        Log.debug("[processModification] processing " + cmdNames[c] + " commands");
                        cmdTags = XmlUtil.getTagValues(xmlBody, cmdNames[c]);

                        for (int i = 0,  l = cmdTags.size(); i < l; i++) {
                            ChunkedString command = (ChunkedString) cmdTags.elementAt(i);
                            count += processCommand(msgId, cmdNames[c], command);

                            command = null;

                        }
                        cmdTags = null;

                        Log.info(cmdNames[c] + ": " + count + " items processed");
                    }

                }
            } catch (XmlException e) {
                e.printStackTrace();
                Log.error("[processModification] error parsing command: " + e.toString());
            }
        }

        try {
            // Process status commands
            cmdTags = XmlUtil.getTagValues(xmlBody, SyncML.TAG_STATUS);
            SyncMLStatus status = null;
            for (int i = 0,  l = cmdTags.size(); i < l; i++) {
                status = SyncMLStatus.parse((ChunkedString) cmdTags.elementAt(i));
                if (status != null) {
                    String cmd = status.getCmd();
                    Log.debug("[processModification] processing Status for <" + cmd + "> command.");

                    // Check status to SyncHdr and Sync
                    if (cmd.equals(SyncML.TAG_SYNCHDR) || cmd.equals(SyncML.TAG_SYNC)) {
                        if (status.getStatus() == 506) {
                            String msg = "Server responded " + status.getStatus() + " to command " + cmd + " [" + status.getStatusDataMessage() + "]"; //506
                            Log.error(msg);
                            throw new SyncException(SyncException.BACKEND_ERROR, msg);
                        } else if (status.getStatus() == 511) {
                            String msg = "Server responded " + status.getStatus() + " to command " + cmd + " [" + status.getStatusDataMessage() + "]"; //511
                            Log.error(msg);
                            throw new SyncException(SyncException.BACKEND_AUTH_ERROR, msg);
                        }else if (status.getStatus() < 200 || status.getStatus() >= 300) {
                        
                            String msg = "Server responded " + status.getStatus() + " to command " + cmd + " [" + status.getStatusDataMessage() + "]"; //XXX
                            Log.error(msg);
                            throw new SyncException(SyncException.SERVER_ERROR, msg);
                        }
                    } else {
                        // Otherwise, pass it to the source
                        String[] items = status.getItemKeys();
                        int code = status.getStatus();
                        // Check if it's a multi-item response
                        if (items != null) {
                            for (int j = 0,  n = items.length; j < n; j++) {
                                source.setItemStatus(items[j], code);
                                // Notify the listener
                                listener.itemUpdated(items[j]);
                            }
                        } else {
                            source.setItemStatus(status.getRef(), code);
                            // Notify the listener
                            listener.itemUpdated(status.getRef());
                        }
                    }

                    status = null;

                } else {
                    Log.error("[processModification] error in Status command.");
                }
            }
        } catch (XmlException e) {
            e.printStackTrace();
            Log.error("[processModification] error parsing status: " + e.toString());
        }
        modifications = null;
        return ret;
    }

    /**
     * Prepares the modification message in SyncML.
     *
     * @return the formatted message
     */
    private String prepareModificationMessage() throws SyncException {


        StringBuffer modMsg = new StringBuffer("<SyncML>");

        // Meta
        String meta = "<Meta><MaxMsgSize>" + maxMsgSize + "</MaxMsgSize></Meta>\n";

        // Sync header
        String syncHdr = prepareSyncHeader(sessionID, getNextMsgID(),
                deviceId, serverUrl,
                meta);

        modMsg.append(syncHdr);

        // Sync Body
        modMsg.append("<SyncBody>\n");

        // Status to the alert command
        // This is used to address the correct MsgIdRef on the outgoing message
        int msgIdRef = msgID - 1;
        modMsg.append("<Status>\n").append("<CmdID>").append(resetCmdID()).append("</CmdID>\n").append("<MsgRef>" + msgIdRef + "</MsgRef><CmdRef>0</CmdRef>\n") //fixMe
                .append("<Cmd>SyncHdr</Cmd>\n").append("<TargetRef>").append(deviceId).append("</TargetRef>\n").append("<SourceRef>").append(serverUrl).append("</SourceRef>\n").append("<Data>200</Data>\n</Status>\n");

        // Add the status to the Alert message
        // FIXME: now it checks msgId, but this test is not very smart
        //        should use SyncMLStatus for this too? (anchor must be added)
        if (msgID == 2) {
            modMsg.append("<Status>\n").append("<CmdID>").append(getNextCmdID()).append("</CmdID>\n").append("<MsgRef>1</MsgRef><CmdRef>1</CmdRef><Cmd>Alert</Cmd>\n").append("<TargetRef>").append(source.getSourceUri()).append("</TargetRef>\n").append("<SourceRef>").append(source.getSourceUri()).append("</SourceRef>\n").append("<Data>200</Data>\n").append("<Item>\n").append("<Data>\n").append("<Anchor xmlns=\"syncml:metinf\"><Next>").append(source.getNextAnchor()).append("</Next></Anchor>\n").append("</Data>\n").append("</Item>\n").append("</Status>\n");
        }

        // Add status commands, if any
        appendStatusTags(modMsg);

        // Add mappings if necessary.
        appendMapTag(modMsg);

        if (this.state != STATE_MODIFICATION_COMPLETED) {
            modMsg.append(prepareSyncTag(modMsg.length()));
        }

        //Adding the device capabilities as response to the <Get> command
        //TODO: check if the response from server is the best place to set this
        if (addDevInfResults) {
            modMsg.append(createResults(config.deviceConfig));
            //reset the flag
            addDevInfResults = false;
        }

        if (this.state == STATE_MODIFICATION_COMPLETED) {
            Log.info("Modification done, sending <final> tag.");
            modMsg.append("<Final/>\n");
        }

        modMsg.append("</SyncBody></SyncML>");

        return modMsg.toString();
    }

    /**
     * Prepare mapping message
     *
     **/
    private String prepareMappingMessage() {

        int i = 0;

        StringBuffer ret = new StringBuffer("<SyncML>\n");

        // Add SyncHdr
        ret.append(prepareSyncHeader(sessionID, getNextMsgID(),
                deviceId, serverUrl, null));

        // Add SyncBody
        ret.append("<SyncBody>\n");

        // This is used to address the correct MsgIdRef on the outgoing message
        int msgIdRef = msgID - 1;
        // Add Status to the alert (FIXME)
        ret.append("<Status>\n") // CMD ID were not correctly reset before sending the mapping message
                .append("<CmdID>").append(resetCmdID()).append("</CmdID>\n").append("<MsgRef>" + msgIdRef + "</MsgRef>\n") // FIXME
                .append("<CmdRef>0</CmdRef>\n").append("<Cmd>SyncHdr</Cmd>\n").append("<TargetRef>").append(deviceId).append("</TargetRef>\n").append("<SourceRef>").append(config.syncUrl).append("</SourceRef>\n").append("<Data>200</Data>\n").append("</Status>\n");

        appendStatusTags(ret);

        appendMapTag(ret);

        ret.append("<Final/>\n").append("</SyncBody>\n").append("</SyncML>");

        return ret.toString();
    }

    /**
     * Prepares the Map tag if there is some mappings to send, and append
     * it to the given StringBuffer.
     * It cleans up the mapping table at the end.
     *
     * @param out the StringBuffer to append the Map tag to.
     * @return none.
     */
    private void appendMapTag(StringBuffer out) {
        if (mappings.size() == 0) {
            // No mappings to add
            return;
        }

        String targetRef = null;
        String sourceRef = null;

        Enumeration e = mappings.keys();

        out.append("<Map>\n").append("<CmdID>" + getNextCmdID() + "</CmdID>\n").append("<Target>\n").append("<LocURI>" + source.getSourceUri() + "</LocURI>\n").append("</Target>\n").append("<Source>\n").append("<LocURI>" + source.getName() + "</LocURI>\n").append("</Source>\n");

        while (e.hasMoreElements()) {

            sourceRef = (String) e.nextElement();
            targetRef = (String) mappings.get(sourceRef);

            out.append("<MapItem>\n").append("<Target>\n").append("<LocURI>" + targetRef + "</LocURI>\n").append("</Target>\n").append("<Source>\n").append("<LocURI>" + sourceRef + "</LocURI>\n").append("</Source>\n").append("</MapItem>\n");
        }
        out.append("</Map>\n");

        // Cleanup mappings table before returning.
        mappings.clear();
    }

    /**
     * Prepares the Status tags if there is some status commands to send,
     * and append it to the given StringBuffer.
     * It cleans up the status lost at the end.
     *
     * @param out the StringBuffer to append the Status tags to.
     * @return none.
     */
    private void appendStatusTags(StringBuffer out) {

        int l = statusList.size();
        if (l == 0) {
            // Nothing to send
            return;
        }

        SyncMLStatus status = null;

        // Build status commands...
        for (int idx = 0; idx < l; idx++) {
            status = (SyncMLStatus) statusList.elementAt(idx);
            status.setCmdId(getNextCmdID());    // update the command id
            out.append(status.toString());
        }
        // ...and cleanup the status vector
        statusList.removeAllElements();
    }

    /**
     * Contructs the alerts for the given source.
     * @param src SyncSource
     * @param syncMode
     * @return the XML for the SyncML Alert commands
     */
    private String createAlerts(SyncSource src, int syncMode) {

        StringBuffer sb = new StringBuffer();

        // XXX CHECK IT OUT XXX
        // the Last overwrite the Next?????????????????
        String timestamp = "<Next>" + src.getNextAnchor() + "</Next>\n";

        if (source.getLastAnchor() != 0l) {
            timestamp = "<Last>" + src.getLastAnchor() + "</Last>\n" + timestamp;
        }

        sb.append("<Alert>\n");
        sb.append("<CmdID>1</CmdID>\n");
        sb.append("<Data>");

        // First, use the syncMode passed as argument,
        // if not valid, use the default for the source
        // as last chance, check the anchor.
        if (syncMode != 0) {
            sb.append(syncMode);
        } else if (src.getSyncMode() != 0) {
            sb.append(SyncML.ALERT_CODE_SLOW);
        } else if (src.getLastAnchor() != 0) {
            sb.append(SyncML.ALERT_CODE_FAST);
        } else {
            sb.append(src.getSyncMode());
        }

        SyncFilter f = src.getFilter();
        sb.append("</Data>\n");
        sb.append("<Item>\n");
        sb.append("<Target><LocURI>");
        sb.append(src.getSourceUri());
        sb.append("</LocURI>\n");
        // Apply source filter with a default limit to maxMsgSize.
        // TODO: change it to maxObjSize when the Large Object will be
        // implemented.
        if (f != null) {
            int maxDataSize = maxMsgSize - PROTOCOL_OVERHEAD;
            sb.append(f.toSyncML(maxDataSize));
        }
        sb.append("</Target>\n");
        sb.append("<Source><LocURI>");
        sb.append(src.getName());
        sb.append("</LocURI></Source>\n");
        sb.append("<Meta>\n");
        sb.append("<Anchor xmlns=\"syncml:metinf\">\n");
        sb.append(timestamp);
        sb.append("</Anchor>\n");
        sb.append("</Meta>\n");
        sb.append("</Item>\n");
        sb.append("</Alert>");
        sb.append("\n");

        return sb.toString();
    }

    /**
     * Constructs the <Put> section of a SyncML initialization message used to
     * carry the device capabilities with the <DevInf> element
     *
     * @param devInf
     *            A reference to the current device configuration (<code>DeviceConfig</code>)
     * @return a String to be added to the initialization SyncML message
     */
    private String createPut(DeviceConfig devInf) {
        StringBuffer sb = new StringBuffer();

        //TODO: retrieve most values from the passed DeviceConfig object
        sb.append("<Put>\n").append("<CmdID>2</CmdID>\n")// TODO: this is normally the cmd 2, but...
                .append("<Meta>\n").append("<Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type>\n").append("</Meta>\n").append("<Item>\n").append("<Source><LocURI>./devinf12</LocURI></Source>\n").append("<Data>\n").append(createDevInf(devInf)) //closing all tags
                .append("</Data>\n").append("</Item>\n").append("</Put>\n");

        //reset the flag
        sendDevInf = false;

        return sb.toString();
    }

    /**
     * Used to build the part of the SyncML modification message containing the
     * device sync capabilities (<Results>) when requested by the server with
     * the command <Get>
     *
     * @param devInf
     *            A reference to the current device configuration (<code>DeviceConfig</code>)
     * @return the string containing the device capabilities part of the SyncML
     *         message sent to the server
     */
    private String createResults(DeviceConfig devInf) {
        StringBuffer sb = new StringBuffer();

        sb.append("<Results>\n").append("<CmdID>" + getNextCmdID() + "</CmdID>\n").append("<MsgRef>" + msgIDget + "</MsgRef>\n").append("<CmdRef>" + cmdIDget + "</CmdRef>\n").append("<Meta>\n").append("<Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type>\n").append("</Meta>\n").append("<Item>\n").append("<Source><LocURI>./devinf12</LocURI></Source>\n").append("<Data>\n").append(createDevInf(devInf)) //closing all tags
                .append("</Data>\n").append("</Item>\n").append("</Results>");

        return sb.toString();
    }

    /**
     * Used to build the <DevInf> element as part of a SyncML message's <Put> or
     * <Results> section
     *
     * @param devInf
     *            A reference to the current device configuration (<code>DeviceConfig</code>)
     * @return the string containing the device capabilities part of the SyncML
     *         message sent to the server
     */
    private String createDevInf(DeviceConfig devInf) {
        StringBuffer sb = new StringBuffer();

        if (devInf.man == null) {
            devInf.man = "";
        }

        if (devInf.mod == null) {
            devInf.mod = "";
        }

        if (devInf.oem == null) {
            devInf.oem = "";
        }

        if (devInf.fwv == null) {
            devInf.fwv = "";
        }

        if (devInf.swv == null) {
            devInf.swv = "";
        }

        if (devInf.hwv == null) {
            devInf.hwv = "";
        }

        if (devInf.devID == null) {
            devInf.devID = "";
        }

        if (devInf.devType == null) {
            devInf.devType = "";
        }

        sb.append("<DevInf xmlns='syncml:devinf'>\n").append("<VerDTD>1.2</VerDTD>\n")//mandatory
                .append("<Man>" + devInf.man + "</Man>\n")//mandatory: name of the manufacturer of the device
                .append("<Mod>" + devInf.mod + "</Mod>\n")//mandatory: model name or model number of the device
                .append("<OEM>" + devInf.oem + "</OEM>\n")//optional: Original Equipment Manufacturer
                .append("<FwV>" + devInf.fwv + "</FwV>\n")//mandatory: firmware version of the device or a date
                .append("<SwV>" + devInf.swv + "</SwV>\n")//mandatory: software version of the device or a date
                .append("<HwV>" + devInf.hwv + "</HwV>\n")//mandatory: hardware version of the device or a date
                .append("<DevID>" + devInf.devID + "</DevID>\n")//mandatory: identifier of the source synchronization device
                .append("<DevTyp>" + devInf.devType + "</DevTyp>\n");//mandatory: type of the source synchronization device (see OMA table)

        //optional flag (if present, the server SHOULD send time in UTC form)
        if (devInf.utc) {
            sb.append("<UTC/>\n");
        }
        //optional (if present, it specifies that the device supports receiving
        //large objects)
        if (devInf.loSupport) {
            sb.append("<SupportLargeObjs/>\n");
        }
        //optional: server MUST NOT send <NumberOfChanges> if the client has
        //not specified this flag
        if (devInf.nocSupport) {
            sb.append("<SupportNumberOfChanges/>\n");
        }

        //<DataStore> one for each of the local datastores
        sb.append("<DataStore>\n")//
                .append("<SourceRef>" + source.getName() + "</SourceRef>\n") //required for each specified datastore
                .append("<Rx-Pref>\n").append("<CTType>").append(source.getType()).append("</CTType>\n").append("<VerCT></VerCT>\n").append("</Rx-Pref>\n") //required for each specified datastore
                .append("<Tx-Pref>\n").append("<CTType>").append(source.getType()).append("</CTType>\n").append("<VerCT></VerCT>\n").append("</Tx-Pref>\n") //SyncCap
                .append("<SyncCap>\n")//mandatory
                .append("<SyncType>1</SyncType>\n")//Support of 'two-way sync'
                .append("<SyncType>2</SyncType>\n")//Support of 'slow two-way sync'
                //TODO: add support of one way?
                .append("<SyncType>7</SyncType>\n")//Support of 'server alerted sync'
                .append("</SyncCap>\n").append("</DataStore>\n").append("</DevInf>\n");

        return sb.toString();
    }

    /**
     * Checks if in the response from server a <Get> command is present and that
     * the information required by the server with this command is the device
     * capabilities
     *
     * @param response
     *            The SyncML message received from server
     * @return <code>true</code> if the <Get> tag is present in the message
     *         and the required information is the device capabilities
     */
    private boolean isGetCommandFromServer(ChunkedString response) {
        ChunkedString get = null;
        ChunkedString item = null;
        ChunkedString target = null;
        ChunkedString locUri = null;
        ChunkedString syncHdr = null;

        if (XmlUtil.getTag(response, "Get") == -1) {
            Log.debug("No <Get> command.");
            return false;
        }

        try {
            get = XmlUtil.getTagValue(response, "Get");
            if (get != null) {
                item = XmlUtil.getTagValue(get, "Item");//mandatory if Get present
                target = XmlUtil.getTagValue(item, "Target");
                locUri = XmlUtil.getTagValue(target, "LocURI");
                this.cmdIDget = XmlUtil.getTagValue(get, "CmdID").toString();
                syncHdr = XmlUtil.getTagValue(response, "SyncHdr");
                this.msgIDget = XmlUtil.getTagValue(syncHdr, "MsgID").toString();
            }
        } catch (XmlException e1) {
            e1.printStackTrace();
            Log.error("Invalid get command from server.");
            // TODO: return an error status to the server.
            return false;
        }

        //TODO: check if backward compatibility is required (./devinf11)
        if ("./devinf12".equals(locUri)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * This method returns the Add command tag.
     */
    private String getAddCommand(int size) throws SyncException {

        SyncItem item = source.getNextNewItem();
        StringBuffer cmdTag = new StringBuffer();

        // No item for this source
        if (item == null) {
            // No new items -> go to the next state.
            nextState(STATE_SENDING_REPLACE);
            return null;
        }
        // Notify the listener
        listener.itemAddSent(item);

        // Build Add command
        cmdTag.append("<Add>\n").append("<CmdID>" + getNextCmdID() + "</CmdID>\n").append("<Meta><Type xmlns=\"syncml:metinf\">").append(source.getType()).append("</Type>").append(getFormat()).append("</Meta>\n");

        do {
            cmdTag.append(getItemTag(item));
            // Ask the source for next item
            item = source.getNextNewItem();

            // Last new item found
            if (item == null) {
                // No new items -> go to the next state.
                nextState(STATE_SENDING_REPLACE);
                break;
            }
            // Notify the listener
            listener.itemAddSent(item);
        } while (size + cmdTag.length() < maxMsgSize);

        cmdTag.append("</Add>\n");

        return cmdTag.toString();
    }

    /**
     * This method returns the Replace command tag.
     */
    private String getReplaceCommand(int size) throws SyncException {

        SyncItem item = source.getNextUpdatedItem();
        StringBuffer cmdTag = new StringBuffer();

        // No item for this source
        if (item == null) {
            // No REPLACE -> go to the next state.
            nextState(STATE_SENDING_DELETE);
            return null;
        }
        // Notify the listener
        listener.itemReplaceSent(item);

        // Build
        cmdTag.append("<Replace>\n").append("<CmdID>").append(getNextCmdID()).append("</CmdID>\n").append("<Meta><Type xmlns=\"syncml:metinf\">").append(source.getType()).append("</Type>").append(getFormat()).append("</Meta>\n");

        do {
            cmdTag.append(getItemTag(item));
            // Ask the source for next item
            item = source.getNextUpdatedItem();

            // Last item found
            if (item == null) {
                // No more ADD/REPLACE/DELETE -> go to the next state.
                nextState(STATE_SENDING_DELETE);
                break;
            }
            // Notify the listener
            listener.itemReplaceSent(item);
        } while (size + cmdTag.length() < maxMsgSize);

        cmdTag.append("</Replace>\n");

        return cmdTag.toString();
    }

    /**
     * This method returns the Delete command tag.
     */
    private String getDeleteCommand(int size) throws SyncException {

        SyncItem item = source.getNextDeletedItem();

        // No item for this source
        if (item == null) {
            // No ADD/REPLACE/DELETE -> go to the next state.
            nextState(STATE_MODIFICATION_COMPLETED);
            return null;
        }
        // Notify the listener
        listener.itemDeleteSent(item);

        StringBuffer cmdTag = new StringBuffer();
        // Build Delete command
        cmdTag.append("<Delete>\n").append("<CmdID>").append(getNextCmdID()).append("</CmdID>\n");

        do {
            cmdTag.append(getItemTag(item));
            // Ask the source for next item
            item = source.getNextDeletedItem();

            // Last item found
            if (item == null) {
                // No more ADD/REPLACE/DELETE -> go to the next state.
                nextState(STATE_MODIFICATION_COMPLETED);
                break;
            }
            // Notify the listener
            listener.itemDeleteSent(item);
        } while (size + cmdTag.length() < maxMsgSize);

        cmdTag.append("</Delete>\n");

        return cmdTag.toString();
    }

    /**
     * This method formats the Item tag.
     */
    private String getItemTag(SyncItem item) {

        switch (item.getState()) {

            case SyncItem.STATE_DELETED:
                return "<Item>\n" +
                        "<Source><LocURI>" + item.getKey() + "</LocURI></Source>\n" +
                        "</Item>\n";

            case SyncItem.STATE_UPDATED:
            case SyncItem.STATE_NEW:
                Log.info("The encoding method is [" + source.getEncoding() + "]");
                String encodedData = null;

                if (item.getContent() == null) {
                    Log.error("Empty content from SyncSource for item:" +
                            item.getKey());
                    encodedData = "";
                } else if (!source.getEncoding().equals(source.ENCODING_NONE)) {
                    String[] formatList = StringUtil.split(
                            source.getEncoding(), ";");
                    byte[] data = encodeItemData(formatList, item.getContent());
                    encodedData = new String(data);
                } else {
                    // Else, the data is text/plain,
                    // and the XML special chars are escaped.
                    String content = new String(item.getContent());
                    encodedData = XmlUtil.escapeXml(content);
                }
                StringBuffer ret = new StringBuffer("<Item>\n");
                // FIXME: should distinguish between ADD e REPLACE?
                ret.append(
                        "<Source><LocURI>" + item.getKey() + "</LocURI></Source>\n");
                if (item.getParent() != null) {
                    ret.append("<SourceParent><LocURI>").append(item.getParent()).append("</LocURI></SourceParent>\n");
                }
                ret.append("<Data>").append(encodedData).append("</Data>\n").append("</Item>\n");

                return ret.toString();

            default:
                // Should never happen
                Log.error("[getItemTag] Invalid item state: " + item.getState());
                // Go on without sending this item
                return "";

        }// end switch
    }

    /**
     *  Get the next command tag, with all the items that can be contained
     *  in defined the message size.
     *
     *  @param size
     *
     *  @return the command tag of null if no item to send.
     */
    private String getNextCmdTag(int size) throws SyncException {

        StringBuffer cmdTag = new StringBuffer();
        String uri = source.getSourceUri();

        switch (alertCode) {

            case SyncML.ALERT_CODE_SLOW:
            case SyncML.ALERT_CODE_REFRESH_FROM_CLIENT:

                SyncItem item = source.getNextItem();

                // No item for this source
                if (item == null) {
                    Log.debug("No items for " + uri);
                    nextState(STATE_MODIFICATION_COMPLETED);
                    return null;
                }

                // Build
                cmdTag.append("<Replace>\n").append("<CmdID>").append(getNextCmdID()).append("</CmdID>\n").append("<Meta><Type xmlns=\"syncml:metinf\">").append(source.getType()).append("</Type>").append(getFormat()).append("</Meta>\n");

                // Patch provided from the community to avoid loosing last item
                boolean stopFlag = false;
                do {
                    cmdTag.append(getItemTag(item));
                    if (size + cmdTag.length() < maxMsgSize) {
                        // Ask the source for next item
                        item = source.getNextItem();
                        // Last item found
                        if (item == null) {
                            Log.debug("No more items for " + uri);
                            nextState(STATE_MODIFICATION_COMPLETED);
                            break;
                        }
                    } else {
                        stopFlag = true;
                    }
                } while (!stopFlag);
                /*
                do {
                cmdTag.append(getItemTag(item));
                // Ask the source for next item
                item = source.getNextItem();
                // Last item found
                if(item == null) {
                Log.debug("No more items for " + uri);
                nextState(STATE_MODIFICATION_COMPLETED);
                break;
                }
                }
                while(size + cmdTag.length() < maxMsgSize);
                 */
                cmdTag.append("</Replace>\n");
                break;

            case SyncML.ALERT_CODE_REFRESH_FROM_SERVER:
            case SyncML.ALERT_CODE_ONE_WAY_FROM_SERVER:
                nextState(STATE_MODIFICATION_COMPLETED);
                return null; // no items sent for refresh from server

            case SyncML.ALERT_CODE_FAST:
            case SyncML.ALERT_CODE_ONE_WAY_FROM_CLIENT:
                //
                // Fast Sync or One way from client.
                //
                String command = null;
                switch (state) {
                    case STATE_SENDING_ADD:
                        command = getAddCommand(size);
                        break;
                    case STATE_SENDING_REPLACE:
                        command = getReplaceCommand(size);
                        break;
                    case STATE_SENDING_DELETE:
                        command = getDeleteCommand(size);
                        break;
                    default:
                        return null;
                }
                if (command != null) {
                    cmdTag.append(command);
                }
                break;

            default:
                Log.error("[getNextCmdTag] Invalid alert code: " + alertCode);
                throw new SyncException(
                        SyncException.SERVER_ERROR,
                        "Invalid alert code: " + alertCode);
        }

        return cmdTag.toString();
    }

    /**
     * return Sync tag about sourceUri
     *
     * @param records records to sync
     * @param sourceURI source uri
     * @return sync tag value
     */
    private String prepareSyncTag(int size) throws SyncException {

        StringBuffer syncTag = new StringBuffer();
        String cmdTag = null;

        syncTag.append("<Sync>\n").append("<CmdID>").append(getNextCmdID()).append("</CmdID>\n").append("<Target><LocURI>").append(source.getSourceUri()).append("</LocURI></Target>\n").append("<Source><LocURI>").append(source.getName()).append("</LocURI></Source>\n");

        do {
            cmdTag = getNextCmdTag(size + syncTag.length());
            // Last command?
            if (cmdTag == null) {
                Log.debug("[prepareSyncTag] No more commands to send");
                break;
            }

            // append command tag
            syncTag.append(cmdTag);
        } while (size + syncTag.length() < maxMsgSize);

        syncTag.append("</Sync>\n");

        return syncTag.toString();
    }

    /**
     * Process the Sync command (check the source uri, save the
     * number of changes).
     *
     * @param msgRef message reference
     * @param command xml command to parse
     * @return none
     */
    private void processSyncCommand(ChunkedString msgRef, ChunkedString command)
            throws SyncException {

        String cmdId = null;
        String locuri = null;

        try {
            cmdId = XmlUtil.getTagValue(command, SyncML.TAG_CMDID).toString();
            locuri = XmlUtil.getTagValue(
                    XmlUtil.getTagValue(command, SyncML.TAG_TARGET),
                    SyncML.TAG_LOC_URI).toString();

        } catch (XmlException e) {
            Log.error("[processModification] Invalid Sync command: " + e.toString());
            e.printStackTrace();
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Invalid Sync command from server.");
        }

        // If this sync is not for this source, throw an exception
        if (!locuri.equals(source.getName())) {
            Log.error("[processModification] Invalid uri: '" + locuri + "' for source: '" + source.getName() + "'");
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    "Invalid source to sync: " + locuri);
        }

        int nc = -1;

        // If the Sync contains the number of chages, pass them to
        // the SyncSource, otherwise pass -1
        if (XmlUtil.getTag(command, "NumberOfChanges") > 0) {
            try {
                ChunkedString nc_val = XmlUtil.getTagValue(command, "NumberOfChanges");
                nc = Integer.parseInt(nc_val.toString());
                Log.info("Number of changes from server: " + nc);

            } catch (XmlException xe) {
                xe.printStackTrace();
                Log.error("Error parsing NumberOfChanges, ignoring it.");
            }
        }
        // This is the very first moment we know how many message we're about
        // to receive. This is when we notify the listener about it, even though
        // the receiving phase has already begun.
        listener.startReceiving(nc);
        source.setServerItemsNumber(nc);

        // Build the status to the Sync command
        SyncMLStatus status = new SyncMLStatus();
        status.setMsgRef(msgRef.toString());
        status.setCmdRef(cmdId.toString());
        status.setCmd(SyncML.TAG_SYNC);
        status.setTgtRef(source.getName());
        status.setSrcRef(source.getSourceUri());

        statusList.addElement(status);
    }

    /**
     * Returns the server alert code for the given source
     *
     * @param sourceURI the source
     *
     * @return the server alert code for the given source or -1 if it is not
     *         found/parsable
     */
    private int getSourceAlertCode(String sourceURI) {

        try {
            String alert = (String) serverAlerts.get(sourceURI);
            return Integer.parseInt(alert);
        } catch (Throwable t) {
            t.printStackTrace();
            Log.error("ERROR: unrecognized server alert code (" + serverAlerts.get(sourceURI) + ") for " + sourceURI.toString());
        }

        return -1;
    }

    /**
     * Set variable in XML msg.
     *
     * @param msgXML msg XML
     * @param variable variable name
     * @param variableValue variable value
     * @return msg XML with setting variable value
     */
    private String messageFormat(String msgXML, String variable, String variableValue) {

        String msgXMLBefore = null;
        String msgXMLAfter = null;

        msgXMLBefore = msgXML.substring(0, msgXML.indexOf(variable));
        msgXMLAfter = msgXML.substring(msgXML.indexOf(variable) + variable.length());

        return (msgXMLBefore + variableValue + msgXMLAfter);
    }

    // Reset the message ID counter.
    private String resetMsgID() {
        msgID = 1;
        return "1";
    }

    // Return the next message ID to use.
    private String getNextMsgID() {
        return String.valueOf(++msgID);
    }

    // Reset the command ID counter.
    private String resetCmdID() {
        cmdID = 1;
        return "1";
    }

    // Return the next message ID to use.
    private String getNextCmdID() {
        return String.valueOf(++cmdID);
    }

    private void nextState(int state) {
        this.state = state;
        String msg = null;

        if (Log.getLogLevel() >= Log.DEBUG) {
            switch (state) {
                case STATE_SENDING_ADD:
                    msg = "state=>STATE_SENDING_ADD";
                    break;
                case STATE_SENDING_REPLACE:
                    msg = "state=>STATE_SENDING_REPLACE";
                    break;
                case STATE_SENDING_DELETE:
                    msg = "state=>STATE_SENDING_DELETE";
                    break;
                case STATE_MODIFICATION_COMPLETED:
                    msg = "state=>STATE_MODIFICATION_COMPLETED";
                    break;
                default:
                    msg = "UNKNOWN STATE!";
            }
            Log.debug(msg);
        }
    }
}

