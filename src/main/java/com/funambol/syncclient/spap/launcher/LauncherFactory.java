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

package com.funambol.syncclient.spap.launcher;

import java.util.Hashtable;


import com.funambol.syncclient.spap.AssetInstallationException;
import com.funambol.syncclient.spdm.DMException;
import com.funambol.syncclient.spdm.DeviceManager;
import com.funambol.syncclient.spdm.SimpleDeviceManager;

/**
 * Manages <code>Laucher</code>
 * <p>Supplies the method for obtain the appropriate <code>Launcher</code>
 *  for a given program
 *
 * @version  $Id: LauncherFactory.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 *
 */

public class LauncherFactory {

    // Contain the mapping from program's extension to appropriate launcher.
    // It's inizilized through DM
    private static Hashtable fileExtensionMapping = null;

    static {
        // init LauncherFactory with DM
        DeviceManager dm  = SimpleDeviceManager.getDeviceManager();

        try {
            fileExtensionMapping =
                    dm.getManagementTree("").getNodeValues("spap/launcherFactoryConfig");
        }
        catch (DMException ex) {
            throw new IllegalStateException("Error in LauncherFactory config");
        }
    }


    // ---------------------------------------------------------- Public methods

    /**
     * Returns an instance of the appropriate <code>Launcher</code>
     * for the given <i>programName</i>
     * @param programName name of the program of which it wants
     * the <code>Launcher</code>
     * @return the appropriate <code>Launcher</code>
     * @throws Exception if the appropriate <code>Launcher</code> is not found
     *                   or if an error occurs during the inizialization
     */
    public static Launcher getLauncher(String programName) throws AssetInstallationException {
        Launcher launcher = null;

        int indexPoint = programName.lastIndexOf(".");
        String extension = programName.substring(indexPoint + 1, programName.length());

        String classLauncher = (String)fileExtensionMapping.get(extension);

        if (classLauncher != null) {

            try {
                launcher =  (Launcher)(Class.forName(classLauncher).newInstance());
            } catch (ClassNotFoundException ex) {
                throw new AssetInstallationException(
                        "Error in launcher inizialization for class '" +
                        classLauncher + "' (Class not found)"
                                );
            } catch (Exception ex) {
                throw new AssetInstallationException(
                        "Error in launcher inizialization for class '" +
                        classLauncher + "' (" + ex.getMessage() + ")");
            }

        } else {
            throw new AssetInstallationException("Launcher not defined for " +
                                extension + " program");
        }
        return launcher;
    }


}