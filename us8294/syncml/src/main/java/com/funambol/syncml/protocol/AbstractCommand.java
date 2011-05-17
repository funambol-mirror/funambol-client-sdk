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
 * This is a base class for "command" classes
 */
public abstract class AbstractCommand {

    // ---------------------------------------------------------- Protected data
    protected String  cmdID     ;
    protected Boolean noResp    ;
    protected Meta    meta      ;
    protected Cred    credential;

    // ------------------------------------------------------------ Constructors

    /** For serialization purposes */
    protected AbstractCommand() {}

    /**
     * Create a new AbstractCommand object with the given commandIdentifier
     * and noResponse
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResp true if the command doesn't require a response
     *
     */
    public AbstractCommand(final String cmdID, final boolean noResp) {
        setCmdID(cmdID);
        this.noResp  = (noResp) ? new Boolean(noResp) : null;
    }

    /**
     * Create a new AbstractCommand object with the given commandIdentifier
     *
     * @param cmdID the command identifier - NOT NULL
     *
     */
    public AbstractCommand(final String cmdID) {
        this(cmdID, false);
    }

    /**
     * Create a new AbstractCommand object with the given commandIdentifier
     * and noResponse
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResponse true if the command doesn't require a response
     * @param meta the Meta object
     */
    public AbstractCommand(final String cmdID,
                           final boolean noResp,
                           final Meta meta) {
        setCmdID(cmdID);
        this.noResp  = (noResp) ? new Boolean(noResp) : null;
        setMeta(meta);
    }

    // ---------------------------------------------------------- Public methods
    public void init() {
        cmdID      = null;
        noResp     = null;
        meta       = null;
        credential = null;
    }

    /**
     * Get CommandIdentifier property
     *
     * @return the command identifier - NOT NULL
     */
    public String getCmdID() {
        return this.cmdID;
    }

    /**
     * Sets the CommandIdentifier property
     *
     * @param cmdID the command identifier
     *
     */
    public void setCmdID(String cmdID) {
        if (cmdID == null) {
            throw new IllegalArgumentException("cmdID cannot be null");
        }
        this.cmdID = cmdID;
    }

    /**
     * Gets noResp property
     *
     * @return true if the command doesn't require a response, false otherwise
     */
    public boolean isNoResp() {
        return (noResp != null);
    }

    public Boolean getNoResp() {
        if (noResp == null || !noResp.booleanValue()) {
            return null;
        }
        return noResp;
    }

    /**
     * Sets noResp true if no response is required
     *
     * @param noResp is true if no response is required
     *
     */
    public void setNoResp(Boolean noResp) {
        this.noResp = (noResp.booleanValue()) ? noResp : null;
    }

    /**
     * Gets Credential object
     *
     * @return the Credential object
     */
    public Cred getCred() {
        return credential;
    }

    /**
     * Sets authentication credential
     *
     * @param cred the authentication credential
     *
     */
    public void setCred(Cred cred) {
        this.credential = cred;
    }

    /**
     * Gets an Meta object
     *
     * @return an Meta object
     */
    public Meta getMeta() {
        return meta;
    }

    /**
     * Sets Meta object
     *
     * @param meta the meta object
     *
     */
    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    /**
     * Get name property
     *
     * @return the name of the command
     */
    public abstract String getName();
}
