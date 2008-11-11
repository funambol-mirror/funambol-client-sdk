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

import java.sql.Timestamp;
import java.util.HashMap;
import java.util.Map;
import java.text.SimpleDateFormat;


/**
 * This class represents a asset.
 * <p>A <code>Asset</code> is constituted from:
 * <ul>
 *   <li>a state: the JNDI name of the datasource to be useed
 *   <li>an identifier: the name of the table containg the items
 *   <li>a name: the column name of the primary key
 *   <li>a manufacturer: the column name of the timestamp
 *   <li>a description: the column name of the change column
 *   <li>a a time of the last update: the column name of the primary key
 *   <li>a current version: is the <code>AssetVersion</code>
 *       for the version installed
 *   <li>a new version: is the <code>AssetVersion</code>
 *       for the version to install
 * </ul>
 *
 * @version  $Id: Asset.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class Asset {

    // --------------------------------------------------------------- Constants

    // These are the possible state of the Asset
    public static final String STATE_NEW = "N";
    public static final String STATE_UPDATE = "U";
    public static final String STATE_DELETE = "D";
    public static final String STATE_ASSET_INSTALLED = "AI";
    public static final String STATE_FILE_DOWNLOAD = "FD";
    public static final String STATE_FILE_EXTRACTED = "FE";
    public static final String STATE_PREVIOUS_VERSION_UNINSTALLED = "PVU";
    public static final String STATE_NEW_VERSION_NOT_WANTED = "NVNW";
    public static final String STATE_NOT_VALID = "NV";


    // The following constants are used for create a Map from a Asset
    // and for to represent a Asset in Map
    public static final String PROPERTY_ID = "id";
    public static final String PROPERTY_NAME = "name";
    public static final String PROPERTY_MANUFACTURER = "manufacturer";
    public static final String PROPERTY_DESCRIPTION = "description";
    public static final String PROPERTY_STATE = "state";
    public static final String PROPERTY_LAST_UPDATE = "last_update";


    // -------------------------------------------------------------- Properties

    /**
     * Identifier of the asset
     */
    private String id;

    /**
     * Sets property id
     * @param id
     *     description: identifier of the asset
     *     displayName: id
     */
    public void setId(String id) {
        this.id = id;
    }

    /**
     * Returns property id
     */
    public String getId() {
        return id;
    }

    /**
     * Manufacturer or developer of the asset
     */
    private String manufacturer;

    /**
     * Sets property
     * @param manufacturer
     *     description: manufacturer or developer of the asset
     *     displayName: manufacturer
     */
    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    /**
     * Returns property manufacturer
     */
    public String getManufacturer() {
        return manufacturer;
    }


    /**
     * Name of the asset
     */
    private String name;

    /**
     * Sets property
     * @param name
     *     description: name of the asset
     *     displayName: name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Returns property name
     */
    public String getName() {
        return name;
    }


    /**
     * Description of the asset
     */
    private String description;

    /**
     * Sets property
     * @param description
     *     description: description of the asset
     *     displayName: description
     */
    public void setDescription(String description) {
        this.description = description;
    }

    /**
     * Returns property description
     */
    public String getDescription() {
        return description;
    }

    /**
     * State of the asset
     */
    private String state;

    /**
     * Sets property
     * @param state
     *     description: state of the asset
     *     displayName: state
     */
    public void setState(String state) {
        this.state = state;
    }

    /**
     * Returns property state
     */
    public String getState() {
        return state;
    }

    /**
     * Time of the last update of the asset
     */
    private Timestamp lastUpdate;

    /**
     * Sets property
     * @param lastUpdate
     *     description: time of the last update of the asset
     *     displayName: lastUpdate
     */
    public void setLastUpdate(Timestamp lastUpdate) {
        this.lastUpdate = lastUpdate;
    }

    /**
     * Returns property lastUpdate
     */
    public Timestamp getLastUpdate() {
        return lastUpdate;
    }


    /**
     * Current version of the asset
     */
    private AssetVersion currentVersion;

    /**
     * Sets property
     * @param currentVersion
     *     description: current version of the asset
     *     displayName: currentVersion
     */
    public void setCurrentVersion(AssetVersion currentVersion) {
        this.currentVersion = currentVersion;
    }

    /**
     * Returns property currentVersion
     */
    public AssetVersion getCurrentVersion() {
        return currentVersion;
    }


    /**
     * New version of the asset
     */
    private AssetVersion newVersion;

    /**
     * Sets property
     * @param newVersion
     *     description: new version of the asset
     *     displayName: newVersion
     */
    public void setNewVersion(AssetVersion newVersion) {
        this.newVersion = newVersion;
    }

    /**
     * Returns property newVersion
     */
    public AssetVersion getNewVersion() {
        return newVersion;
    }


    // ------------------------------------------------------------ Constructors

    /**
     * Constructs a Asset with the values contained in the given Map
     */
    public Asset(Map values) {
        this.id = (String)values.get(PROPERTY_ID);
        this.manufacturer = (String)values.get(PROPERTY_MANUFACTURER);
        this.name = (String)values.get(PROPERTY_NAME);
        this.description = (String)values.get(PROPERTY_DESCRIPTION);
        this.state = (String)values.get(PROPERTY_STATE);

        //
        // lastUpdate can be specified as a date in the format YYYY-MM-DD HH:MM:SS.S
        // or  millisecond since Jan the 1st 1970
        //
        String lastUpdate = (String)values.get(PROPERTY_LAST_UPDATE);
        if (lastUpdate != null) {
            try {
                SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.S");
                this.lastUpdate = new Timestamp(format.parse(lastUpdate).getTime());
            } catch (Exception e1) {
                try {
                    this.lastUpdate = new Timestamp(Long.parseLong(lastUpdate));
                } catch (Exception e2) {
                    throw new IllegalArgumentException(
                        "'"                                            +
                        lastUpdate                                     +
                        "' is neither a date (yyyy-MM-dd HH:mm:ss.S) " +
                        "nor a timestamp in milliseconds."
                     );
                 }

            }
        }
    }

    /**
     * Constructs a Asset with the values contained in the given Map.
     * <p>If the param <code>buildNewVersion</code> is true it comes
     * built also the new version
     * using the same given Map
     */
    public Asset(Map values, boolean buildNewVersion) {
        this(values);

        if (buildNewVersion) {
            newVersion = new AssetVersion(values);
        }
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Sets the current version using the given Map
     * @param values the Map used for build a <code>AssetVersion</code>
     */
    public void setCurrentVersion(Map values) {
        currentVersion = new AssetVersion(values);
    }

    /**
     * Sets the new version using the given Map
     * @param values the Map used for build a <code>AssetVersion</code>
     */
    public void setNewVersion(Map values) {
        newVersion = new AssetVersion(values);
    }

    /**
     * Returns a representation of a Asset in Map
     * @return the Map representation
     */
    public Map toMap() {
        Map map = new HashMap();

        map.put(PROPERTY_ID, getId());
        map.put(PROPERTY_NAME,getName());
        map.put(PROPERTY_DESCRIPTION,getDescription());
        map.put(PROPERTY_MANUFACTURER,getManufacturer());
        map.put(PROPERTY_STATE, getState());
        map.put(PROPERTY_LAST_UPDATE, getLastUpdate());

        return map;
    }


    //---------------------------------------------------
    //  Overrides java.lang.Object method
    //---------------------------------------------------

    public String toString() {
        StringBuffer sb = new StringBuffer("Asset: \n");

        sb.append("\tId: ");
        sb.append(getId());
        sb.append("\n\tName: ");
         sb.append(getName());
        sb.append("\n\tManufacturer: ");
         sb.append(getManufacturer());
        sb.append("\n\tDescription: ");
         sb.append(getDescription());
        sb.append("\n\tState: ");
         sb.append(getState());
        sb.append("\n\tLast update: ");
         sb.append(getLastUpdate());

        sb.append("\n\t----");
        sb.append("\n\tCurrentVersion: \n");
        if (currentVersion!=null) {
            sb.append(currentVersion.toString());
        } else {
            sb.append("\t\tNot found\n");
        };
        sb.append("\n\t----\n");
        sb.append("\tNewVersion: \n");
        if (newVersion!=null) {
            sb.append(newVersion.toString());
        } else {
            sb.append("\t\tNot found");
        }

        return sb.toString();
    }


}
