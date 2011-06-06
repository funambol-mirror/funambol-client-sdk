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
 * Corresponds to the &lt;Data&gt; tag in the SyncML represent DTD
 */
public class Data {

    // ---------------------------------------------------------- Protected data
    protected String data;
    protected Anchor anchor;
    protected DevInf devInf;
    protected byte[] binData;

    // ------------------------------------------------------------ Constructors
    /**
     * Creates a new Data object with the given data value
     *
     * @param data the data value
     *
     */
    protected Data(final String data) {
        this.data = data;
    }

    protected Data(final Anchor anchor) {
        this.anchor = anchor;
    }

    protected Data(final DevInf devInf) {
        this.devInf = devInf;
    }

    protected Data(final byte[] binData) {
        this.binData = binData;
    }

    // ---------------------------------------------------------- Public methods
    public static Data newInstance(final String data) {
        return ObjectsPool.createData(data, null, null, null);
    }

    public static Data newInstance(final Anchor anchor) {
        return ObjectsPool.createData(null, anchor, null, null);
    }

    public static Data newInstance(final DevInf devInf) {
        return ObjectsPool.createData(null, null, devInf, null);
    }

    public static Data newInstance(final byte[] binData) {
        return ObjectsPool.createData(null, null, null, binData);
    }

    public void init() {
        data =null;
        anchor = null;
        devInf = null;
        binData = null;
    }

    /**
     * Sets the data property
     *
     * @param data the data property
     */
    public void setData(String data) {
        this.data = data;
    }

    /**
     * Gets the data properties
     *
     * @return the data properties
     */
    public String getData() {
        return data;
    }

    public void setDevInf(DevInf devInf) {
        this.devInf = devInf;
    }

    public DevInf getDevInf() {
        return devInf;
    }

    public void setBinData(byte[] binData) {
        this.binData = binData;
    }

    public byte[] getBinData() {
        return binData;
    }

    public void setAnchor(Anchor anchor) {
        this.anchor = anchor;
    }

    public Anchor getAnchor() {
        return anchor;
    }

    /**
     * Returns the size of the item (once formatted). In case Data contains
     * DevInf or hancor the size is just an approximation and not the real value
     * which is unknown until we format the value.
     */ 
    public int getSize() {
        if (data != null) {
            return data.length();
        } else if (binData != null) {
            return binData.length;
        } else if (devInf != null) {
            // This is just heuristic...
            return 1024;
        } else if (anchor != null) {
            // This is just heuristic...
            return 128;
        } else {
            return 0;
        }
    }
}
