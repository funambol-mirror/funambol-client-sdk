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
package com.funambol.syncml.protocol;

/**
 * Corresponds to the &lt;SyncHdr&gt; element in the SyncML represent DTD
 *
 * @see    SyncBody
 */
public class SyncHdr {

    // --------------------------------------------------------------- Constants
    public static final String COMMAND_NAME = "SyncHdr";

    // ------------------------------------------------------------ Private data

    private VerDTD    verDTD   ;
    private String    verProto ;
    private String    sessionID;
    private String    msgID    ;
    private Target    target   ;
    private Source    source   ;
    private String    respURI  ;
    private Boolean   noResp   ;
    private Meta      meta     ;
    private Cred      cred     ;

    // ------------------------------------------------------------ Constructors

    public SyncHdr(){}

    /**
     * Creates a nee SyncHdr object
     *
     * @param verDTD SyncML DTD version - NOT NULL
     * @param verProto SyncML protocol version - NOT NULL
     * @param sessionID sync session identifier - NOT NULL
     * @param msgID message ID - NOT NULL
     * @param target target URI - NOT NULL
     * @param source source URI - NOT NULL
     * @param respURI may be null.
     * @param noResp true if no response is required
     * @param cred credentials. May be null.
     * @param meta may be null.
     *
     */
    public SyncHdr(final VerDTD    verDTD   ,
                   final String    verProto ,
                   final String    sessionID,
                   final String    msgID    ,
                   final Target    target   ,
                   final Source    source   ,
                   final String    respURI  ,
                   final boolean   noResp   ,
                   final Meta      meta     ) {

        setMsgID(msgID);
        setVerDTD(verDTD);
        setVerProto(verProto);
        setSessionID(sessionID);
        setTarget(target);
        setSource(source);

        this.noResp  = (noResp) ? new Boolean(noResp) : null;
        this.respURI = respURI;

        this.meta = meta;
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Gets the DTD version
     *
     * @return verDTD the DTD version
     */
    public VerDTD getVerDTD() {
        return verDTD;
    }

    /**
     * Sets the DTD version
     *
     * @param verDTD the DTD version
     *
     */
    public void setVerDTD(VerDTD verDTD) {
        if (verDTD == null) {
            throw new IllegalArgumentException("verDTD cannot be null");
        }
        this.verDTD = verDTD;
    }

    /**
     * Gets the protocol version
     *
     * @return verProto the protocol version
     */
    public String getVerProto() {
        return verProto;
    }

    /**
     * Sets the protocol version
     *
     * @param verProto the protocol version
     */
    public void setVerProto(String verProto) {
        if (verProto == null) {
            throw new IllegalArgumentException("verProto cannot be null");
        }
        this.verProto = verProto;
    }

    /**
     * Gets the session identifier
     *
     * @return sessionID the session identifier
     */
    public String getSessionID() {
        return sessionID;
    }

    /**
     * Sets the session identifier
     *
     * @param sessionID the session identifier
     *
     */
    public void setSessionID(String sessionID) {
        if (sessionID == null) {
            throw new IllegalArgumentException("sessionID cannot be null");
        }
        this.sessionID = sessionID;
    }

    /**
     * Gets the message identifier
     *
     * @return msgID the message identifier
     */
    public String getMsgID() {
        return msgID;
    }

    /**
     * Sets the message identifier
     *
     * @param msgID the message identifier
     */
    public void setMsgID(String msgID) {
        if (msgID == null || msgID.length() == 0) {
            throw new IllegalArgumentException(
                                          "msgID cannot be null or empty");
        }
        this.msgID = msgID;
    }

    /**
     * Gets the Target object
     *
     * @return target the Target object
     */
    public Target getTarget() {
        return target;
    }

    /**
     * Sets the Target object
     *
     * @param target the Target object
     */
    public void setTarget(Target target) {
        if (target == null) {
            throw new IllegalArgumentException("target cannot be null");
        }
        this.target = target;
    }

    /**
     * Gets the Source object
     *
     * @return source the Source object
     */
    public Source getSource() {
        return source;
    }

    /**
     * Sets the Source object
     *
     * @param source the Source object
     */
    public void setSource(Source source) {
        if (source == null) {
            throw new IllegalArgumentException("source cannot be null");
        }
        this.source = source;
    }

    /**
     * Gets the response URI
     *
     * @return respURI the response URI
     */
    public String getRespURI() {
        return respURI;
    }

    /**
     * Sets the response URI.
     *
     * @param uri the new response URI; NOT NULL
     */
    public void setRespURI(String uri) {
        this.respURI = uri;
    }

    /**
     * Gets noResp property
     *
     * @return true if the command doesn't require a response, false otherwise
     */
    public boolean isNoResp() {
        return (noResp != null);
    }

    /**
     * Gets the Boolean value of noResp
     *
     * @return true if the command doesn't require a response, null otherwise
     */
    public Boolean getNoResp() {
        if (noResp == null || !noResp.booleanValue()) {
            return null;
        }
        return noResp;
    }

    /**
     * Sets the noResponse property
     *
     * @param noResp the noResponse property
     */
    public void setNoResp(Boolean noResp) {
        this.noResp = (noResp.booleanValue()) ? noResp : null;
    }

    /**
     * Gets the Meta property
     *
     * @return meta the Meta property
     */
    public Meta getMeta() {
        return meta;
    }

    /**
     * Sets the Meta property
     *
     * @param meta the Meta property
     */
    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    public void setCred(Cred cred) {
        this.cred = cred;
    }

    public Cred getCred() {
        return cred;
    }
}
