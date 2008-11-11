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

import java.io.File;
import java.io.IOException;

import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.spap.AssetInstallationException;
import com.funambol.syncclient.spap.installer.InstallationContext;

/**
 * This class represents a <code>Laucher</code> for the execution of
 * the win program.
 *
 * @version  $Id: Win32Launcher.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 *
 */

public class Win32Launcher implements Launcher {


    // -----------------------------------------------------------Private data

    private Logger logger = new Logger();

    // -----------------------------------------------------------Public methods

    /**
     * Executes the win program with given name.
     *
     * @param programName the program to execute.
     * @param install <code>true</code> if the program is the installation program,
     *  <code>false</code> if the program is the uninstallation program
     * @param ctx installation context information
     *
     * @throws AssetInstallationException if an error occurs during the execution
     */
    public int execute(String programName, boolean install, InstallationContext ctx)
            throws AssetInstallationException {

        String completeProgramPath = ctx.getWorkingDirectory() + File.separator + programName;
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Win32Launcher - Execute " +
                         completeProgramPath        );
        }

        int exitState = -1;
        try {
            Process proc = Runtime.getRuntime().exec(
                    completeProgramPath, null, new File(ctx.getWorkingDirectory())
                    );

            proc.waitFor();

            exitState = proc.exitValue();

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Process executed. Return state: " + exitState);
            }

        } catch (Exception e) {
            throw new AssetInstallationException("Error launching '" + completeProgramPath + "'", e);
        }

        return exitState;
    }


}
