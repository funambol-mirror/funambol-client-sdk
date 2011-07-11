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
 *
 * This class represents the &lt;Put&gt; tag as defined by the SyncML
 * representation specifications.
 */
public class Put extends ItemizedCommand {

    // --------------------------------------------------------------- Constants
    public static String COMMAND_NAME = "Put";

    // ------------------------------------------------------------ Private data
    private String lang;

    // ------------------------------------------------------------ Constructors

    public Put() {}

    /**
     * Creates a new Put object given its elements.
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResp is &lt;NoResponse/&gt; required?
     * @param lang Preferred language
     * @param cred authentication credentials
     * @param meta meta information
     * @param items Item elements - NOT NULL
     *
     * @throws IllegalArgumentException if any NOT NULL parameter is null
     */
    public Put(
        final String  cmdID ,
        final boolean noResp,
        final String  lang  ,
        final Cred    cred  ,
        final Meta    meta  ,
        final Vector  items ) {
        super(cmdID, meta, items);

        setCred(cred);
        this.noResp  = (noResp) ? new Boolean(noResp) : null;
        this.lang   = lang;
    }



   // ----------------------------------------------------------- Public methods

    /**
     * Returns the preferred language
     *
     * @return the preferred language
     *
     */
    public String getLang() {
        return lang;
    }

    /**
     * Sets the preferred language
     *
     * @param lang new preferred language
     */
    public void setLang(String lang) {
        this.lang = lang;
    }

    public void setNoResp(Boolean value) {
        noResp = value;
    }

    /**
     * Returns the command name
     *
     * @return the command name
     */
    public String getName() {
        return Put.COMMAND_NAME;
    }
}
