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
 * This is a simple implementation of <i>DeviceManager</i> that uses the file
 * system to store and read nodes.
 * The root of the configuration tree is specified with the system property
 * <code>spdm.dir.config</code>; if null, the current directory is taken.
 * <p>
 * The tree structure is orgnaized in contexts, represented by directories, and
 * leaf nodes, represented by properties files. Leaf nodes contain the
 * configuration values.
 *
 *
 *
 * @version $Id: SimpleDeviceManager.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */

public class SimpleDeviceManager extends DeviceManager {

    // --------------------------------------------------------------- Constants

    /**
     * The system property name for the base directory
     */
    public static final String PROP_DM_DIR_BASE = "spdm.dir.base";


    //@todo trovare un altro modo per settare la appURI in relazione al JDBC
    /**
     * The system property name for the application base uri
     */
    //public static final String PROP_DM_APP_BASE = "sp.application.uri";

    // ------------------------------------------------------------ private dara

    private String baseDir = null;

    // ------------------------------------------------------------ Constructors

    /**
     * Creates a new SimpleDeviceManager
     */
    public SimpleDeviceManager() {
        baseDir = System.getProperty(PROP_DM_DIR_BASE, "./");

        //
        // Config dir must end with '/'
        //
        if (!baseDir.endsWith("/")) {
            baseDir += '/';
        }
    }
    //----------------------------------------------------------- Public methods

    /**
     * Factory for SimpleDeviceManager.
     *
     * @return the newly created instance
     */
    public static DeviceManager getDeviceManager(){
        return (DeviceManager) new SimpleDeviceManager();
    }

    /**
     * Return the management tree given its context
     *
     * @param context the node context
     *
     * @return the root management node starting at the specified context
     *
     */
    public ManagementNode getManagementTree(String context){
        return new NodeImpl(null, baseDir + context);
    }

    /**
     * The same as <i>getManagementTree("")</i>
     *
     * @return <i>getManagementTree("")</i>
     */
    public ManagementNode getManagementTree(){
        return getManagementTree("");
    }

    public Device getDevice() {
        return new DeviceImpl(this);
    }

}
