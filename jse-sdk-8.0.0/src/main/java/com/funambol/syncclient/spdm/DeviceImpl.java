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
package com.funambol.syncclient.spdm;



/**
 * This is a simple implementation of the Device interface. It is still quite
 * generic and will be replaced in the future with more interesting
 * implementations. For now it is a good trade-off between quick and dirty.
 *
 *
 * @version $Id: DeviceImpl.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class DeviceImpl
implements Device {

    // --------------------------------------------------------------- Constants

    /**
     * The management tree where the device information is stored
     */
    public static final String CONTEXT_DEVICE = "spdm";

    /**
     * The base directory configuration property
     */
    public static final String PROP_BASE_DIR = "baseDir";

    // ------------------------------------------------------------ Private data

    private DeviceManager dm;

    // ------------------------------------------------------------ Constructors

    public DeviceImpl(DeviceManager dm) {
        this.dm = dm;
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Returns the base directory.
     *
     * @see Device
     *
     * @return the base directory as a string.
     *
     * @throws DMException if there is an error in determining the current
     *         working directory.
     */
    public String getBaseDirectory() throws DMException {
        return (String)dm.getManagementTree(CONTEXT_DEVICE).getValue(PROP_BASE_DIR);
    }

    /**
     * Sets the device base directory
     *
     * @param baseDir the new base directory.
     *
     * @throws DMException in case of error setting the base direcotry
     */
    public void setBaseDirectory(String baseDir) throws DMException {
        dm.getManagementTree(CONTEXT_DEVICE).setValue(PROP_BASE_DIR, baseDir);
    }

}
