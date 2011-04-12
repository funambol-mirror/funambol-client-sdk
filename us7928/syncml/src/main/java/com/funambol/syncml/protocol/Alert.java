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

import java.util.Vector;

/**
 * Corresponds to &lt;Alert&gt; element in SyncML represent DTD
 *
 * @see AlertCode
 *
 * @version $Id: Alert.java,v 1.6 2007/06/15 14:43:35 luigiafassina Exp $
 */
public class Alert extends ItemizedCommand {

    // --------------------------------------------------------------- Constants
    public static String COMMAND_NAME = "Alert";

    // ------------------------------------------------------------ Private data
    private int data;
    private String correlator;
    // ------------------------------------------------------------ Constructors

    public Alert() {}

    /**
     * Creates a new Alert object with the given command identifier,
     * noResponse, authentication credential, alert code, the link correlator
     * and array of item
     *
     * @param cmdID  the command identifier - NOT NULL
     * @param noResp is true if no response is required
     * @param cred   the authentication credential
     * @param data   the code of Alert
     * @param correlator the link between the command and an asynchronous response
     * @param items the array of item - NOT NULL
     *
     */
    public Alert(final String  cmdID     ,
                 final boolean noResp    ,
                 final Cred    cred      ,
                 final int     data      ,
                 final String  correlator,
                 final Vector  items     ) {

        super(cmdID, items);

        this.noResp  = (noResp) ? new Boolean(noResp) : null;
        setCred(cred);
        this.data       = data      ;
        this.correlator = correlator;
    }

    /**
     * Creates a new Alert object with the given command identifier,
     * noResponse, authentication credential, alert code and array of item
     *
     * @param cmdID command identifier - NOT NULL
     * @param noResp is true if no response is required
     * @param cred the authentication credential
     * @param data the code of Alert
     * @param items the array of item - NOT NULL
     *
     */
    public Alert(final String  cmdID ,
                 final boolean noResp,
                 final Cred    cred  ,
                 final int     data  ,
                 final Vector  items ) {
        this(cmdID, noResp, cred, data, null, items);
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Gets the alert code
     *
     * @return the alert code
     */
    public int getData() {
        return data;
    }

    /**
     * Sets the alert code
     *
     * @param data the alert code
     */
    public void setData(int data) {
        this.data = data;
    }

    /**
     * Gets the correlator property
     *
     * @return the correlator property
     */
    public String getCorrelator() {
        return correlator;
    }

    /**
     * Sets the link between the command and an asynchronous response
     *
     * @param correlator the link
     */
    public void setCorrelator(String correlator) {
        this.correlator = correlator;
    }

    /**
     * Gets the command name property
     *
     * @return the command name property
     */
    public String getName() {
        return Alert.COMMAND_NAME;
    }
}
