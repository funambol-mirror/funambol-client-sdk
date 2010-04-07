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

import java.io.ByteArrayOutputStream;
import java.io.PrintWriter;
import java.sql.Timestamp;
import java.util.Hashtable;
import java.util.Vector;



import com.funambol.syncclient.common.HashtableTools;
import com.funambol.syncclient.spdm.DMException;
import com.funambol.syncclient.spdm.DeviceManager;
import com.funambol.syncclient.spdm.ManagementNode;
import com.funambol.syncclient.spdm.SimpleDeviceManager;


/**
 * This class supplies the methods for use the DM like repository of the
 * information on the present assets on the client.
 *
 * The DM is used with the following node structure:
 *
 * <PRE>
 * conduit
 *        applications
 *                &#60;manufacturer&#62;
 *                        &#60;asset's name&#62;
 *                                asset           (contains the information
 *                                                 of the asset)
 *                                currentVersion  (contains the information
 *                                                 of the version installed)
 *                                newVersion      (contains the information
 *                                                 of the new version to install)
 *</PRE>
 *
 * @version  $Id: AssetDAO.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class AssetDAO {

    // ---------------------------------------------------------------Constants

    /** Name of the DM node used for read/write the asset's information  */
    private static final String CONFIG_ASSET_NODE_NAME = "asset";

    /** Name of the DM node used for read/write the current version's information  */
    private static final String CONFIG_CURRENT_VERSION_NODE_NAME = "currentVersion";

    /** Name of the DM node used for read/write the new version's information  */
    private static final String CONFIG_NEW_VERSION_NODE_NAME = "newVersion";

    /** Name of the DM node used for read/write the applications' information  */
    private static final String CONFIG_MANAGEMENT_NODE_NAME = "spap/applications";




    // ------------------------------------------------------------Private data

    /** Management node for access to the DM */
    private ManagementNode managementNode = null;



    // -------------------------------------------------------------Constructor
    /**
     * Constructs a new AssetDAO
     */
    public AssetDAO() {
        DeviceManager dm = SimpleDeviceManager.getDeviceManager();
        managementNode = dm.getManagementTree(CONFIG_MANAGEMENT_NODE_NAME);
    }

    /**
     * Save the asset.
     * The asset is saved in DM.
     *
     * @param asset asset to save
     * @param time time of the saving
     * @throws AssetManagementException if an error occurs during the saving
     */
    public void setAsset(Asset asset, Timestamp time) throws AssetManagementException {

        String manufacturer = asset.getManufacturer().toLowerCase();
        String name = asset.getName().toLowerCase();

        String state = asset.getState();
        String id = asset.getId();
        String description = null2empty(asset.getDescription());
        String lastUpdate = "" + time.getTime();

        AssetVersion currentVersion = asset.getCurrentVersion();
        AssetVersion newVersion = asset.getNewVersion();


        String assetNodePath = manufacturer + "/" + name + "/" + CONFIG_ASSET_NODE_NAME;
        try {

            // save the asset
            managementNode.setValue(assetNodePath, Asset.PROPERTY_ID ,id);
            managementNode.setValue(assetNodePath, Asset.PROPERTY_STATE ,state);
            managementNode.setValue(assetNodePath, Asset.PROPERTY_NAME ,asset.getName());
            managementNode.setValue(assetNodePath, Asset.PROPERTY_DESCRIPTION , description);
            managementNode.setValue(assetNodePath, Asset.PROPERTY_MANUFACTURER , asset.getManufacturer());
            managementNode.setValue(assetNodePath, Asset.PROPERTY_LAST_UPDATE, lastUpdate);

            // save the current version
            if (currentVersion != null) {
                setAssetVersion(manufacturer, name, currentVersion, false);
            } else {
                managementNode.removeNode(manufacturer + "/" + name + "/" + CONFIG_CURRENT_VERSION_NODE_NAME);
            }

            // save the new version
            if (newVersion != null) {
                setAssetVersion(manufacturer, name, newVersion, true);
            } else {
                managementNode.removeNode(manufacturer + "/" + name + "/" + CONFIG_NEW_VERSION_NODE_NAME);
            }

        }
        catch (DMException ex) {
            throw new AssetManagementException(asset, "Error setting asset", ex);
        }

    }

    /**
     * Returns the asset with the given identifier
     *
     * @param idAsset the identifier of the asset
     * @return the Asset with the given identifier
     * @throws AssetManagementException if Asset not exist
     */
    public Asset getAsset(String idAsset) throws AssetManagementException {
        Asset asset = null;

        ManagementNode[] manufacturesNode = null;

        try {
            manufacturesNode = managementNode.getChildren();
        }
        catch (DMException ex) {
            throw new AssetManagementException("Asset with id: '" +
                                     idAsset + "' not found", ex);
        }

        int numManufacturer = manufacturesNode.length;

        ManagementNode manufacturerNode = null;
        ManagementNode[] assetsNode = null;
        ManagementNode assetNode = null;

        boolean assetFound = false;
        Hashtable currentVersion = null;
        Hashtable newVersion = null;

        int numAsset = 0;
        for (int i=0; i<numManufacturer; i++) {
            manufacturerNode = manufacturesNode[i];
            try {
                assetsNode = manufacturerNode.getChildren();
            }
            catch (DMException ex) {
                continue;
            }

            numAsset = assetsNode.length;
            for (int j=0; j<numAsset; j++) {
                assetNode = assetsNode[j];

                String id = null;

                try {

                    id = (String)assetNode.getNodeValue(
                            CONFIG_ASSET_NODE_NAME, Asset.PROPERTY_ID
                            );

                    if (idAsset.equalsIgnoreCase(id)) {

                        // load asset
                        asset = new Asset(
                                HashtableTools.hashtable2hashMap(
                                 assetNode.getNodeValues(CONFIG_ASSET_NODE_NAME)
                                )
                                );

                        assetFound = true;

                        // load currentVersion
                        try {
                            currentVersion = assetNode.getNodeValues(CONFIG_CURRENT_VERSION_NODE_NAME);
                            asset.setCurrentVersion(HashtableTools.hashtable2hashMap(currentVersion));
                        }
                        catch (DMException ex) {
                            // ignore this exception because the asset could not have the currentVersion
                        }

                        // load newVersion
                        try {
                            newVersion = assetNode.getNodeValues(CONFIG_NEW_VERSION_NODE_NAME);
                            asset.setNewVersion(HashtableTools.hashtable2hashMap(newVersion));
                        }
                        catch (DMException ex) {
                            // ignore this exception because the asset could not have the  newVersion
                        }

                        // interrupted search in the list of this manufacturer
                        break;

                    }

                }
                catch (DMException ex) {
                    // ignore this exception
                    continue;
                }

            }

            if (assetFound) {
                // interrupted search
                break;
            }
        }

        if (!assetFound) {
            throw new AssetManagementException("Asset with id: '" +
                                     idAsset + "' not found");
        }

        return asset;
    }


    /**
     * Sets the state of the asset identified from the given idAsset
     *
     * @param idAsset identifier of the asset of which wants to set the state
     * @param state state of the asset
     * @return asset with the changed state
     * @throws AssetManagementException if an error occurs or if asset not exists
     */
    public Asset setAssetState(String idAsset, String state)
            throws AssetManagementException {
        Asset asset = getAsset(idAsset);
        return setAssetState(asset,state);
    }

    /**
     * Sets the state of the asset.
     *
     * @param asset  asset of which wants to set the state
     * @param state state of the asset
     * @return asset with the changed state
     * @throws AssetManagementException if a error occurs during setting
     */
    public Asset setAssetState(Asset asset, String state) throws AssetManagementException {
        String manufacturer = asset.getManufacturer().toLowerCase();
        String assetName = asset.getName().toLowerCase();

        ManagementNode assetNode = null;

        java.util.Date time = new java.util.Date();

        try {
            assetNode = managementNode.getChildNode(manufacturer + "/"
                    + assetName);

            assetNode.setValue(CONFIG_ASSET_NODE_NAME, Asset.PROPERTY_STATE , state);
        }
        catch (DMException e) {
            e.printStackTrace();
            return null;
        }

        asset.setState(state);

        return asset;
    }

    /**
     * Returns the list of all asset
     *
     * @return the list of all asset
     */
    public Vector getAllAsset() {
        return listAsset(null);
    }

    /**
     * Returns the state of the asset identified from the given id
     * @param idAsset identifier of the asset
     *
     * @return the state of the asset
     * @throws AssetManagementException if asset not exists
     */
    public String getAssetState(String idAsset) throws AssetManagementException {

        Asset asset = getAsset(idAsset);

        return asset.getState();
    }



    /**
     * Returns the list of the asset with the given state.
     * If the given <code>state</code> is null, returns
     * the list of the all asset.
     *
     * @param state the state of the wanted assets.
     * @return the list of the asset
     */
    public Vector listAsset(String state) {
        Vector assets = new Vector();

        ManagementNode[] manufacturesNode = null;

        try {
            manufacturesNode = managementNode.getChildren();
        }
        catch (DMException ex) {
            // if an error occurs returns an empty list
            return assets;
        }

        int numManufacturer = manufacturesNode.length;

        ManagementNode manufacturerNode = null;
        ManagementNode[] assetsNode = null;
        ManagementNode assetNode = null;
        String assetName = null;
        Hashtable currentVersion = null;
        Hashtable newVersion = null;
        AssetVersion currentVersionAsset = null;
        AssetVersion newVersionAsset = null;
        Asset asset = null;
        String idAsset = null;
        boolean foundNewVersion = false;
        boolean foundCurrentVersion = false;
        String assetState = null;
        boolean assetFound = false;

        int numAsset = 0;

        // for all manufacturer

        for (int i=0; i<numManufacturer; i++) {
            manufacturerNode = manufacturesNode[i];
            try {
                assetsNode = manufacturerNode.getChildren();
            }
            catch (DMException ex) {
                continue;
            }
            numAsset = assetsNode.length;


            // for all asset of this manifacturer

            for (int j=0; j<numAsset; j++) {
                assetFound = false;
                asset = null;

                assetNode = assetsNode[j];

                try {
                    asset = new Asset(
                              HashtableTools.hashtable2hashMap(
                                assetNode.getNodeValues(CONFIG_ASSET_NODE_NAME)
                              )
                            );
                    currentVersion = null;
                    newVersion = null;
                }
                catch (DMException ex) {
                    // ignore this exception
                }

                // if asset node not exists then asset==null
                if (asset == null) {
                    continue;
                }

                assetState = asset.getState();

                if (state != null)  {
                    if (state.equals(assetState)) {
                        assetFound = true;
                    }
                } else {
                    // if stateWanted==null, returns all assets
                    assetFound = true;
                }


                if (assetFound) {
                    try {
                        currentVersion = assetNode.getNodeValues(
                                CONFIG_CURRENT_VERSION_NODE_NAME
                                );
                    }
                    catch (DMException ex) {
                        // ignore this exception because the asset could not have the currentVersion
                    }

                    if (currentVersion != null) {
                        asset.setCurrentVersion(
                                new AssetVersion(
                                  HashtableTools.hashtable2hashMap(currentVersion)
                                )
                                );
                    }

                    try {
                        newVersion = assetNode.getNodeValues(CONFIG_NEW_VERSION_NODE_NAME);
                    }
                    catch (DMException ex) {
                        // ignore this exception because the asset could not have the newVersion
                    }

                    if (newVersion != null) {
                        asset.setNewVersion(
                                new AssetVersion(HashtableTools.hashtable2hashMap(newVersion))
                                );
                    }

                    assets.addElement(asset);
                }
            }
        }
        return assets;
    }


    /**
     * Remove the asset from DM
     *
     * @param asset asset to remove
     * @throws AssetManagementException if an error occurs
     */
    public void removeAsset(Asset asset) throws AssetManagementException {
        String manufacturer = asset.getManufacturer().toLowerCase();
        String name = asset.getName().toLowerCase();

        try {
            managementNode.removeNode(manufacturer + "/" + name);

            // if there are not other asset for this manufacturer,
           // remove the manufacturer node
            ManagementNode manufacturerNode =
                    managementNode.getChildNode(manufacturer);

            if (manufacturerNode.getChildren().length == 0) {
                managementNode.removeNode(manufacturer);
            }
        }
        catch (DMException ex) {
            throw new AssetManagementException(
                    asset, "Error removing asset information from DM", ex
                    );
        }
    }

    /**
     * Sets the asset as NOT VALID
     *
     * @param asset asset to set as NOT VALID
     * @param cause the cause for which the asset is not valid
     * @return asset asset with the state changed
     * @throws AssetManagementException if a error occurs
     */
    public Asset setAssetAsNotValid(Asset asset, Throwable cause)
            throws AssetManagementException {

        String manufacturer = asset.getManufacturer().toLowerCase();
        String assetName = asset.getName().toLowerCase();

        ManagementNode assetNode = null;

        String causeMessage = stackTraceToString(cause);

        try {
            assetNode = managementNode.getChildNode(manufacturer + "/" + assetName);
        }
        catch (DMException ex) {
            throw new AssetManagementException(
                    asset, "Error during the setting state as NOT VALID", ex
                    );
        }

        try {
            assetNode.setValue(CONFIG_ASSET_NODE_NAME, Asset.PROPERTY_STATE ,
                               Asset.STATE_NOT_VALID);

            assetNode.setValue(CONFIG_ASSET_NODE_NAME, "CAUSE" , causeMessage);
        }
        catch (DMException ex) {
            throw new AssetManagementException(
                    asset, "Error during the setting state as NOT VALID", ex
                    );
        }

        asset.setState(Asset.STATE_NOT_VALID);

        return asset;
    }


    /**
     * Transforms a null object in an empty string.
     * If the object is not null, returns a string representation of the object.
     *
     * @param temp object to transform
     * @return a string representation of the object
     */
    private String null2empty(Object temp) {
        if (temp==null) {
            return "";
        } else {
            return temp.toString();
        }
    }


    /**
     * Saving the given AssetVersion for the asset identified from the specified
     * manufacturer and name.
     * <p>If <i>isNewVersion</i> is <code>true</code>, the given version is
     * saved as <i>newVersion</i> otherwise is saved as <i>currentVersion</i>
     *
     * @param manufacturer manufacturer of the asset to modify
     * @param name name of the asset to modify
     * @param assetVersion version to saving
     * @param isNewVersion if <code>true</code>, save
     *        the version as <i>newVersion</i>,
     *        otherwise save the version as <i>currentVersion</i>
     * @throws DMException if an error occurs
     */
    protected void setAssetVersion(String manufacturer, String name,
                                   AssetVersion assetVersion, boolean isNewVersion)
            throws DMException {

        String nodePath = null;

        if (isNewVersion) {
            nodePath = manufacturer + "/" + name + "/" +
                       CONFIG_NEW_VERSION_NODE_NAME;
        } else {
            nodePath = manufacturer + "/" + name + "/" +
                       CONFIG_CURRENT_VERSION_NODE_NAME;
        }

        managementNode.setValue(nodePath, AssetVersion.PROPERTY_VERSION, assetVersion.getVersion());
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_RELEASE_DATE , assetVersion.getReleaseDate());
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_RELEASE_NOTES , null2empty(assetVersion.getReleaseNotes()));
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_URL, assetVersion.getUrl().toString());
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_SIZE_ASSET_FILE, ""+assetVersion.getSizeContentFile());
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_INSTALL_PROGRAM , null2empty(assetVersion.getInstallProgram()));
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_UNINSTALL_PROGRAM , null2empty(assetVersion.getUninstallProgram()));
        managementNode.setValue(nodePath, AssetVersion.PROPERTY_NEED_UNINSTALL_PREV , null2empty(assetVersion.getNeedUninstallPrev()));

    }

    /**
     * Print the exception's stackTrace into a String
     *
     * @param e the exception
     * @return stackTrace
     */
    private static String stackTraceToString(Throwable e){
        String stackTraceString = "";
        ByteArrayOutputStream sBuf = new ByteArrayOutputStream( 1024 );
        PrintWriter           s    = new PrintWriter          ( sBuf );
        e.printStackTrace( s );
        s.flush();
        stackTraceString = sBuf.toString();
        s.close();
        return stackTraceString;
    }

}
