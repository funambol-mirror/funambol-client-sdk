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

package com.funambol.syncclient.spap;

import java.util.HashMap;
import java.util.Map;

/**
 * Represent a version of a <code>Asset</code>
 *
 * <p>A <code>AssetVersion</code> is constituted from:
 * <ul>
 *   <li>a version
 *   <li>a release date
 *   <li>a release notes
 *   <li>a content file: the file to download during intallation process
 *       of this <code>AssetVersion</code>
 *   <li>a size of the content file: the dimension in bytes of the content file
 *   <li>an installation program: the name of the program to execute during
 *       the installation process
 *   <li>an uninstallation program: the name of the program to execute during
 *       the uninstallation process
 *   <li>a flag for indicate if the installation process of this version require
 *       the uninstallation process of the previous version
 * </ul>
 *
 * @version $Id: AssetVersion.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */

public class AssetVersion {

    // --------------------------------------------------------------- Constants

    // The following contants are used for create a Map from a AssetVersion
    // and for to represent a AssetVersion in Map
    public static final String PROPERTY_VERSION             = "version";
    public static final String PROPERTY_RELEASE_DATE        = "release_date";
    public static final String PROPERTY_RELEASE_NOTES       = "release_notes";
    public static final String PROPERTY_URL                 = "url";
    public static final String PROPERTY_SIZE_ASSET_FILE     = "size_asset_file";
    public static final String PROPERTY_INSTALL_PROGRAM     = "install_program";
    public static final String PROPERTY_UNINSTALL_PROGRAM   = "uninstall_program";
    public static final String PROPERTY_NEED_UNINSTALL_PREV = "need_uninstall_prev";

    /**
     * version of the AssetVersion
     */
    private String version;

    /**
     * Sets property
     * @param version
     *     description: version of the AssetVersion
     *     displayName: version
     */
    public void setVersion(String version) {
        this.version = version;
    }

    /**
     * Returns property version
     */
    public String getVersion() {
        return version;
    }

    /**
     * release date of the AssetVersion
     */
    private String releaseDate;

    /**
     * Sets property
     * @param releaseDate
     *     description: release date of the AssetVersion
     *     displayName: releaseDate
     */
    public void setReleaseDate(String releaseDate) {
        this.releaseDate = releaseDate;
    }

    /**
     * Returns property releaseDate
     */
    public String getReleaseDate() {
        return releaseDate;
    }

    /**
     * release notes of the AssetVersion
     */
    private String releaseNotes;

    /**
     * Sets property
     * @param releaseNotes
     *     description: release notes of the AssetVersion
     *     displayName: releaseNotes
     */
    public void setReleaseNotes(String releaseNotes) {
        this.releaseNotes = releaseNotes;
    }

    /**
     * Returns property releaseNotes
     */
    public String getReleaseNotes() {
        return releaseNotes;
    }

    /**
     * Url of the file of the AssetVersion.
     * <p>It's the url from which to download the asset file during the installation process
     */
    private String url;

    /**
     * Sets property
     * @param url
     *     description: Url of the file of the AssetVersion
     *     displayName: url
     */
    public void setUrl(String url) {
          this.url = url;
    }

    /**
     * Returns property url
     */
    public String getUrl() {
        return url;
    }

    /**
     * Installation program of the AssetVersion.
     * <p>Name of the program to execute during the installation process
     */
    private String installProgram;

    /**
     *  Sets property BooleanProperty
     *
     * @param installProgram
     *     description: installation program of the AssetVersion.
     *     displayName:installProgram **/
    public void setInstallProgram(String installProgram) {
        this.installProgram = installProgram;
    }

    /**
     * Returns property installProgram
     */
    public String getInstallProgram() {
        return installProgram;
    }

    /**
     * Uninstallation program of the AssetVersion.
     * <p>Name of the program to execute during the uninstallation process
     */
    private String uninstallProgram;

    /**
     *  Sets property BooleanProperty
     *
     * @param uninstallProgram
     *     description: installation program of the AssetVersion.
     *     displayName:iunnstallProgram **/
    public void setUninstallProgram(String uninstallProgram) {
        this.uninstallProgram = uninstallProgram;
    }

    /**
     * Returns property uninstallProgram
     */
    public String getUninstallProgram() {
        return uninstallProgram;
    }

    /**
     * Flag for indicate if the installation process of this version
     * require the uninstallation of the previous version
     */
    private String needUninstallPrev;

    /**
     * Sets property
     * @param needUninstallPrev
     *     description: Flag for indicate if the installation process of this
     *                  version require the uninstallation process of the previous version
     *     displayName: needUninstallPrev
     */
    public void setNeedUninstallPrev(String needUninstallPrev) {
        this.needUninstallPrev = needUninstallPrev;
    }

    /**
     * Returns property needUninstallPrev
     */
    public String getNeedUninstallPrev() {
        return needUninstallPrev;
    }

    /**
     * Dimension of the content file
     */
    private int sizeContentFile;

    /**
     * Sets property
     * @param sizeContentFile
     *     description: dimension of the content file
     *     displayName: sizeContentFile
     */
    public void setSizeContentFile(int sizeContentFile) {
        this.sizeContentFile = sizeContentFile;
    }

    /**
     * Returns property sizeContentFile
     */
    public int getSizeContentFile() {
        return sizeContentFile;
    }


    // ------------------------------------------------------------- Constructor

    /**
     * Constructs a AssetVersion with the values contained in the given Map
     */
    public AssetVersion(Map values) {

        setVersion((String)values.get(PROPERTY_VERSION));
        setReleaseDate((String)values.get(PROPERTY_RELEASE_DATE));
        setReleaseNotes((String)values.get(PROPERTY_RELEASE_NOTES));
        setUrl((String)values.get(PROPERTY_URL));

        Object obj = values.get(PROPERTY_SIZE_ASSET_FILE);
        if (obj!=null) {
            try {
                this.sizeContentFile = Integer.parseInt(obj.toString());
            } catch (NumberFormatException e) {
                this.sizeContentFile = 0;
            }
        }

        this.installProgram = (String)values.get(PROPERTY_INSTALL_PROGRAM);
        this.uninstallProgram = (String)values.get(PROPERTY_INSTALL_PROGRAM);
        this.needUninstallPrev = (String)values.get(PROPERTY_NEED_UNINSTALL_PREV);
    }


    /**
     * Returns a representation of a AssetVersion in Map
     * @return the Map representation
     */
    public Map toMap() {
        Map map = new HashMap();

        map.put(PROPERTY_VERSION,getVersion());
        map.put(PROPERTY_RELEASE_DATE,getReleaseDate());
        map.put(PROPERTY_RELEASE_NOTES,getReleaseNotes());
        map.put(PROPERTY_URL,getUrl());
        map.put(PROPERTY_SIZE_ASSET_FILE,""+getSizeContentFile());
        map.put(PROPERTY_INSTALL_PROGRAM,getInstallProgram());
        map.put(PROPERTY_NEED_UNINSTALL_PREV,getNeedUninstallPrev());
        return map;
    }

    //---------------------------------------------------
    //  Overrides java.lang.Object method
    //---------------------------------------------------

    public String toString() {
        StringBuffer sb = new StringBuffer("\tAsset version: \n");

        sb.append("\t\tVersion: ");
        sb.append(getVersion());
        sb.append("\n\t\tRelease date: ");
        sb.append( new java.util.Date(Long.parseLong(getReleaseDate())));
        sb.append(" (");
        sb.append(getReleaseDate());
        sb.append(")");
        sb.append("\n\t\tRelease notes: ");
        sb.append(getReleaseNotes());
        sb.append("\n\t\tAsset file: ");
        sb.append(getUrl());
        sb.append("\n\t\tSize Asset file: ");
        sb.append(getSizeContentFile() );
        sb.append("\n\t\tInstall program: ");
        sb.append(getInstallProgram());
        sb.append("\n\t\tNeed uninstall prev: ");
        sb.append(getNeedUninstallPrev());

        return sb.toString();
    }


}
