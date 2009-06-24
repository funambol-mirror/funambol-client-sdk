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
package com.funambol.syncclient.spap.installer;

import com.funambol.syncclient.spap.Asset;
import com.funambol.syncclient.spap.AssetDAO;

/**
 * This class keeps information about the installation process and provides a
 * way for installation program to interact with the installation engine,
 * without the need of the AssetManager.
 *
 *
 * @version $Id: InstallationContext.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class InstallationContext {

    // ------------------------------------------------------------ Private Date

    // ------------------------------------------------------------ Constructors

    /** Creates a new instance of InstallationContext */
    public InstallationContext() {
    }

    // -------------------------------------------------------------- Properties

    /**
     * The installation program working directory
     */
    private String workingDirectory;

    public void setWorkingDirectory(String workingDirectory) {
        this.workingDirectory = workingDirectory;
    }

    public String getWorkingDirectory() {
        return this.workingDirectory;
    }

    /**
     * The executables directory
     */
    private String binDirectory;

    public void setBinDirectory(String binDirectory) {
        this.binDirectory = binDirectory;
    }

    public String getBinDirectory() {
        return this.binDirectory;
    }

    /**
     * Classes and other libraries directory
     */
    private String libDirectory;

    public void setLibDirectory(String libDirectory) {
        this.libDirectory = libDirectory;
    }

    public String getLibDirectory() {
        return this.libDirectory;
    }

    /**
     * The asset being installed/uninstalled
     */
    private Asset asset;

    public void setAsset(Asset asset) {
        this.asset = asset;
    }

    public Asset getAsset() {
        return this.asset;
    }
}
