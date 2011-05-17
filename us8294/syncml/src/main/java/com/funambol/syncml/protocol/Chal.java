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

import com.funambol.util.StringUtil;

/**
 * This class represents an authentication challenge element. It  corresponds
 * to the &lt;Chal&gt; element in SyncML representation DTD
 */
public class Chal {

    private static final String FORMAT_B64 = "b64";

    // ------------------------------------------------------------ Private data
    private Meta meta = null;

    // ------------------------------------------------------------ Constructors
    /** For serialization purposes */
    public Chal() {}

    /**
     * Constructs a new Chal object.
     *
     * @param meta The meta object - NOT NULL
     *
     */
    public Chal (final Meta meta) {
        this.meta = meta;
        String type         = meta.getType();
        String format       = meta.getFormat();

        if (type == null) {
            throw new IllegalArgumentException(
                                     "The authentication type cannot be null");
        }
        if (format == null) {
            if (StringUtil.equalsIgnoreCase(type,Cred.AUTH_TYPE_BASIC)) {
                meta.setFormat(FORMAT_B64);
            } else if (StringUtil.equalsIgnoreCase(type,Cred.AUTH_TYPE_MD5)) {
                meta.setFormat(FORMAT_B64);
            } else {
                throw new IllegalArgumentException(
                                   "The authentication format cannot be null");
            }
        }
    }

    // ---------------------------------------------------------- Public methods
    /**
     * Gets the Meta property
     *
     * @return meta the Meta property
     */
    public Meta getMeta() {
        return this.meta;
    }

    /**
     * Sets the Meta property
     *
     * @param meta the Meta property
     *
     */
    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    /**
     * Returns the nextNonce property or null
     *
     *  @return the nextNonce property or null
     */
    public NextNonce getNextNonce() {
        return meta.getNextNonce();
    }

    public void setNextNonce(NextNonce nextNonce) {
        if (meta == null) {
            meta = new Meta();
        }
        meta.setNextNonce(nextNonce);
    }
    /**
     * Returns the authentication type
     *
     * @return authentication type.
     */
    public String getType() {
        return meta.getType();
    }

    /**
     * Returns the authentication format
     *
     * @return format the authentication format
     */
    public String getFormat() {
        return meta.getFormat();
    }

    /**
     * Creates a basic authentication challange.
     * This will have type = Cred.AUTH_TYPE_BASIC and
     * format = Constants.FORMAT_B64
     *
     * @return the newly created AuthenticationChallange
     */
    public static Chal getBasicChal() {
        Meta m = new Meta();
        m.setType(Cred.AUTH_TYPE_BASIC);
        m.setFormat(FORMAT_B64);
        m.setNextNonce(null);
        return new Chal(m);
    }

    /**
     * Creates a MD5 authentication challange.
     * This will have type = Cred.AUTH_TYPE_MD5 and
     * format = Constants.FORMAT_B64
     *
     * @return the newly created AuthenticationChallange
     */
    public static Chal getMD5Chal() {
        Meta m = new Meta();
        m.setType(Cred.AUTH_TYPE_MD5);
        m.setFormat(FORMAT_B64);
        m.setNextNonce(null);
        return new Chal(m);
    }
}
