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
import java.io.File;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;
import java.net.URL;
import java.net.MalformedURLException;

import com.funambol.syncclient.common.FileSystemTools;
import com.funambol.syncclient.common.HttpTools;
import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.common.VectorTools;
import com.funambol.syncclient.common.ZipTools;
import com.funambol.syncclient.spap.launcher.Launcher;
import com.funambol.syncclient.spap.launcher.LauncherFactory;
import com.funambol.syncclient.spap.installer.InstallationContext;
import com.funambol.syncclient.spdm.DeviceManager;
import com.funambol.syncclient.spdm.ManagementNode;
import com.funambol.syncclient.spdm.SimpleDeviceManager;

/**
 * This class supplies the methods for the management of the <code>Asset</code>.
 *
 * <p>Manages the synchronization process, the installation process and the
 * uninstallation process of the assets.</p>
 *
 * <p>Uses <code>AssetDAO</code> for retrieves the information of the asset.</p>
 *
 * <p>It's a singleton class and for to get an instance you need to use the
 * <code>getAssetManager()</code> static method.</p>
 *
 * AssetManager is configured with a set of directories where it
 * stores installation packages, configuration files and so on; they are:
 * <ul>
 *  <li>installationDirectory - the directory where the assets must be installed</li>
 *  <li>binDirectory - the directory where installation executables are stored</li>
 *  <li>libDirectory - the directory where libraries and classes are stored</li>
 * <ul>
 * If these directories are specified as absolute paths, they are used as they
 * are, otherwise, they are prepended with the base dir returned by the
 * device manager (calling DeviceManager.getBaseDir).
 *
 * @version $Id: AssetManager.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class AssetManager {

    // ---------------------------------------------------------------Constants

    /** Name of the DM node used for read the installationDirectory property  */
    private static final String PROPERTY_INSTALLATION_DIRECTORY = "installationDirectory";

    /** Name of the DM node used for read the binDirectory property  */
    private static final String PROPERTY_BIN_DIRECTORY = "binDirectory";

    /** Name of the DM node used for read the libDirectory property  */
    private static final String PROPERTY_LIB_DIRECTORY = "libDirectory";

    /** Property that defines if the package size should be checked **/
    private static final String PROPERTY_CHECK_PACKAGE_SIZE = "checkSize";

    /** Directory in which to install asset  */
    private static final String ASSET_CONTENT_DIRECTORY = "assetContent";

    /** Directory in which to save the content file  */
    private static final String ASSET_FILE_DIRECTORY = "assetFile";

    /** Name of the DM node used for initialize the AssetManager   */
    private static final String CONFIG_CONTEXT_NODE = "spap/assetManager";


    // -------------------------------------------------------------Properties

    /**
     * Directory in the which installs the assets
     */
    private String installationDirectory = null;

    /**
     * Sets property
     * @param installationDirectory
     *     description: directory in the which installs the assets
     *     displayName: installationDirectory
     */
    public void setInstallationDirectory(String installationDirectory) {
        this.installationDirectory = installationDirectory;
    }

    /**
     * Returns property installationDirectory
     */
    public String getInstallationDirectory() {
        return installationDirectory;
    }

    /**
     * Directory for executables
     */
    private String binDirectory = null;

    /**
     * Sets property
     * @param binDirectory
     *     description: directory for executables
     *     displayName: binDirectory
     */
    public void setBinDirectory(String binDirectory) {
        this.binDirectory = binDirectory;
    }

    /**
     * Returns property installationDirectory
     */
    public String getBinDirectory() {
        return binDirectory;
    }

    /**
     * Directory for classes and libraries
     */
    private String libDirectory = null;

    /**
     * Sets property
     * @param libDirectory
     *     description: directory for classes and libraries
     *     displayName: libDirectory
     */
    public void setLibDirectory(String libDirectory) {
        this.libDirectory = libDirectory;
    }

    /**
     * Returns property installationDirectory
     */
    public String getLibDirectory() {
        return libDirectory;
    }

    /**
     * Should the package size be checked after downloading?
     **/
    private boolean checkPackageSize = false;

    public void setCheckPackageSize(boolean checkPackageSize){
        this.checkPackageSize = checkPackageSize;
    }

    public boolean isCheckPackageSize() {
        return this.checkPackageSize;
    }


    // ------------------------------------------------------------Private data

    /** instance for singleton pattern */
    private static AssetManager instance = null;

    /** AssetDAO used for get asset's information */
    private AssetDAO assetDAO = null;

    private Logger logger = new Logger();

    // ------------------------------------------------------------ Constructor

    /**
     * Constructs a new AssetManager.
     * Using DM for the initialization
     *
     * @throws IllegalStateException if an error occurs during the
     *          initialization process
     */
    private AssetManager() throws IllegalStateException {
        DeviceManager dm = null;
        ManagementNode managementNode = null;
        dm = SimpleDeviceManager.getDeviceManager();
        Hashtable values = null;

        try {
            values = dm.getManagementTree("").getNodeValues(CONFIG_CONTEXT_NODE);

            installationDirectory = (String)values.get(PROPERTY_INSTALLATION_DIRECTORY);
            binDirectory          = (String)values.get(PROPERTY_BIN_DIRECTORY         );
            libDirectory          = (String)values.get(PROPERTY_LIB_DIRECTORY         );

            //
            // If the directories are relative paths, a base directory taken as
            // described in the class description is prepended to them.
            //
            if (!new File(installationDirectory).isAbsolute()) {
                installationDirectory =
                    new File(
                        dm.getDevice().getBaseDirectory(),
                        installationDirectory
                    ).getAbsolutePath();
            }
            if (!new File(binDirectory).isAbsolute()) {
                binDirectory = new File(
                                   dm.getDevice().getBaseDirectory(),
                                   binDirectory
                               ).getAbsolutePath();
            }
            if (!new File(libDirectory).isAbsolute()) {
                libDirectory = new File(
                                   dm.getDevice().getBaseDirectory(),
                                   libDirectory
                               ).getAbsolutePath();
            }
            // ---

            checkPackageSize = Boolean.getBoolean((String)values.get(PROPERTY_CHECK_PACKAGE_SIZE));
        }
        catch (Exception ex) {
            throw new IllegalStateException(
                    "Error in AssetManager config (" + ex.getMessage() + ")"
                    );
        }
        assetDAO = new AssetDAO();
    }

    // ---------------------------------------------------------- Public Methods

    /**
     * Method used for obtain a instance of a <code>AssetManager</code>
     *
     * @return a instance of a <code>AssetManager</code>
     * @throws IllegalStateException if an error occurs during the initialization
     *         process
     */
    public static AssetManager getAssetManager() throws IllegalStateException {
        instance = new AssetManager();
        return instance;
    }



    /**
     * Starts the installation process of a asset.
     * <p>This method is used for to install a new asset and for to update a
     * asset.</p>
     *
     * <p>A asset requires a installation if his state is:
     * <ui>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_NEW
     *      STATE_NEW}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_UPDATE
     *      STATE_UPDATE}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_FILE_DOWNLOAD
     *      STATE_CONTENT_DOWNLOAD}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_FILE_EXTRACTED
     *      STATE_CONTENT_EXTRACTED}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_PREVIOUS_VERSION_UNINSTALLED
     *      STATE_PREVIOUS_VERSION_UNINSTALLED}</li>
     * </ui>
     * </p>
     * <p>The installation process is composed by the following task:</p>
     * <ui>
     * <li>uninstallation, if required, of the previous version</li>
     * <li>download the content file for the version to install</li>
     * <li>extract the content file</li>
     * <li>launch the installation program</li>
     * <li>save the dm information</li>
     * </ui>
     *
     * @param idAsset identifier of the asset to install
     * @throws AssetManagementException if not exists a <i>newVersion</i> or if an error
     *                        occurs during installation processs
     */
    public void installAsset(String idAsset) throws AssetManagementException {

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("InstallAsset: " + idAsset);
        }

        Asset asset = assetDAO.getAsset(idAsset);

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Asset's state: " + asset.getState());
        }

        AssetVersion assetNewVersion = asset.getNewVersion();

        if (assetNewVersion == null) {
            throw new AssetManagementException(asset, "New version not found");
        }

        String name = asset.getName().toLowerCase();
        String manufacturer = asset.getManufacturer().toLowerCase();
        String state = asset.getState();

        String workingDirectory = this.installationDirectory + "/" + manufacturer + "/" + name;

        AssetVersion assetCurrentVersion = asset.getCurrentVersion();

        byte[] content = null;

        boolean stateRecognized = false;

        if (state.equals(Asset.STATE_UPDATE)) {
            stateRecognized = true;

            // verify if musts uninstall previous version
            try {
                verifyAndUninstallPreviousVersion(asset);
            }
            catch (Exception ex) {
                throw new AssetManagementException(asset, "Error during uninstall previous version" , ex);
            }
            state = Asset.STATE_PREVIOUS_VERSION_UNINSTALLED;
            assetDAO.setAssetState(asset, state);
        }

        if (state.equals(Asset.STATE_NEW) ||
            state.equals(Asset.STATE_PREVIOUS_VERSION_UNINSTALLED)) {

            stateRecognized = true;

            // download zip file for new version
            try {
                content = downloadContentFileForNewVersion(asset);
            }
            catch (IOException ex) {
                throw new AssetManagementException(asset, "Error downloading asset file '" +
                        assetNewVersion.getUrl() + "'", ex);
            }

            String logMsg = "Size content file: "                 +
                            assetNewVersion.getSizeContentFile()  +
                            ", size downloaded file: "            +
                            content.length                        ;

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(logMsg);
            }

            // verify the dimension of the asset's file
            if (checkPackageSize &&
                (content.length != assetNewVersion.getSizeContentFile())) {

                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Error! Wrong size");
                }

                throw new AssetManagementException(asset,
                        "Error downloading content file. Wrong size ("
                        + content.length + ")");


            }

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Dimension is correct (or ignored)");
            }

            // verify the content
            try {
                ZipTools.verifyZip(content);
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Zip file is valid");
                }
            }
            catch (IOException ex) {
                // content is not a valid zip file
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Zip file is not valid");
                }

                throw new AssetManagementException(asset, "Zip file '" +
                        assetNewVersion.getUrl().toString() +
                        "' downloaded but not appared be a valid file", ex);
            }

            // save content file
            try {
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Save the content file");
                }
                FileSystemTools.createFile(workingDirectory +
                        File.separator + ASSET_FILE_DIRECTORY,
                        asset.getName() + "_" + assetNewVersion.getVersion() + ".zip", content);

            }
            catch (Exception ex) {

                throw new AssetManagementException(asset,
                        "Zip file '" + assetNewVersion.getUrl() +
                        "' verified but fault saving process", ex);
            }

            state = Asset.STATE_FILE_DOWNLOAD;
            assetDAO.setAssetState(asset, state);

        }

        if (state.equals(Asset.STATE_FILE_DOWNLOAD)) {
            stateRecognized = true;
            if (content == null) {
                try {
                    if (logger.isLoggable(Logger.DEBUG)) {
                        logger.debug("Load the content file from local system");
                    }
                    content = getAssetPackage(asset);
                }
                catch (Exception ex) {
                    throw new AssetManagementException(asset,
                            "Error loading content file '" +
                            asset.getName() + "_" + assetNewVersion.getVersion() + ".zip" +
                            "' from local system", ex);
                }

            }


            // delete asset_content_directory before to extract the new version
            String msgLog = "Delete directory '"          +
                                 workingDirectory         +
                                 File.separator           +
                                 ASSET_CONTENT_DIRECTORY  +
                                 "'"                      ;

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(msgLog);
            }

            try {
              FileSystemTools.removeDirectoryTree(workingDirectory + File.separator +
                                   ASSET_CONTENT_DIRECTORY);
            }
            catch (Exception ex) {
              throw new AssetManagementException(asset,
                      "Error during the cancellation of the directory '" +
                      workingDirectory + File.separator +
                      ASSET_CONTENT_DIRECTORY + "'", ex);
            }


            // extract new version
            msgLog =  "Extract new version in '" +
                      workingDirectory           +
                      File.separator             +
                      ASSET_CONTENT_DIRECTORY    +
                      "'"                        ;
            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(msgLog);
            }

            try {
                ZipTools.extract(workingDirectory + File.separator +
                                 ASSET_CONTENT_DIRECTORY, content);
            }
            catch (Exception ex) {

                throw new AssetManagementException(asset,
                        "Error during the extraction of the zip file '" +
                        assetNewVersion.getUrl() + "'", ex);

            }


            // after extraction, delete the zip file
            new File(workingDirectory +
                     File.separator + ASSET_FILE_DIRECTORY,
                     asset.getName() + "_" + assetNewVersion.getVersion() + ".zip").delete();


            state = Asset.STATE_FILE_EXTRACTED;
            assetDAO.setAssetState(asset, state);
        }


        if (state.equals(Asset.STATE_FILE_EXTRACTED)) {
            stateRecognized = true;

            // launch the installation program of the new version
            String installProgram = assetNewVersion.getInstallProgram();
            if (installProgram != null && !installProgram.equalsIgnoreCase("")) {

                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Launch the installation program (" + installProgram + ")");
                }
                int exitState = -1;

                try {
                    Launcher launcher = LauncherFactory.getLauncher(installProgram);

                    InstallationContext ctx = new InstallationContext();
                    ctx.setWorkingDirectory(
                        workingDirectory +
                        File.separator   +
                        ASSET_CONTENT_DIRECTORY
                    );
                    ctx.setBinDirectory(binDirectory);
                    ctx.setLibDirectory(libDirectory);
                    ctx.setAsset(asset);

                    exitState = launcher.execute(installProgram, true, ctx);

                } catch (AssetInstallationException e) {
                    throw e;
                } catch (AssetManagementException e) {
                    assetDAO.setAssetAsNotValid(asset, e);
                    throw e;
                } catch (Throwable t) {
                    assetDAO.setAssetAsNotValid(asset, t);

                    throw new AssetManagementException(asset,
                            "Error during the execution of the installation program", t);
                }

                if (exitState != 0) {
                    throw new AssetManagementException("Installation program exit with state: " +exitState);
                }

            }

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Save dm information");
            }

            AssetVersion newVersion = asset.getNewVersion();

            // set the currentVersion with the newVersion
            asset.setCurrentVersion(newVersion);
            newVersion = null;
            asset.setNewVersion(newVersion);

            asset.setState(Asset.STATE_ASSET_INSTALLED);
            assetDAO.setAsset(asset, asset.getLastUpdate() );
        }


        if (!stateRecognized) {

            throw new AssetManagementException(asset,
                                     "Asset not required installation process. State is: '"
                                     + state + "'");
        }

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Installation process finished");
        }
    }


    /**
     * Remove a asset.
     * <p>The state of the asset must be
     * {@link com.funambol.syncclient.spap.Asset#STATE_DELETE STATE_DELETE}
     *
     * <p>The remove process is composed by the following task:</p>
     * <ui>
     * <li>launch the uninstallation program</li>
     * <li>delete content directory</li>
     * <li>delete DM information</li>
     * </ui>
     *
     * @param idAsset identifier of the asset to remove
     * @throws AssetManagementException if an error occurs or if asset not exists
     */
    public void removeAsset(String idAsset) throws AssetManagementException {

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("removeAsset: " + idAsset);
        }

        Asset asset = assetDAO.getAsset(idAsset);
        AssetVersion currentVersion = null;

        String state = asset.getState();
        currentVersion = asset.getCurrentVersion();

        if (!state.equals(Asset.STATE_DELETE)) {
            throw new AssetManagementException(asset, "Asset cannot be deleted, state is not '" +
                                     Asset.STATE_DELETE + "'");
        }
        String name = asset.getName().toLowerCase();
        String manufacturer = asset.getManufacturer().toLowerCase();

        String workingDirectory = this.installationDirectory + "/" + manufacturer + "/" + name;

        if (currentVersion != null) {
            // launch the uninstall program
            String uninstallProgram = currentVersion.getUninstallProgram();
            if (uninstallProgram != null && !uninstallProgram.equalsIgnoreCase("")) {

                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Launch the uninstall program (" + uninstallProgram + ")");
                }

                try {
                    Launcher launcher = LauncherFactory.getLauncher(uninstallProgram);

                    InstallationContext ctx = new InstallationContext();
                    ctx.setWorkingDirectory(
                        workingDirectory +
                        File.separator   +
                        ASSET_CONTENT_DIRECTORY
                    );
                    ctx.setBinDirectory(binDirectory);
                    ctx.setLibDirectory(libDirectory);
                    ctx.setAsset(asset);

                    launcher.execute(uninstallProgram, false, ctx);
                }
                catch (Exception ex) {
                    throw new AssetManagementException(asset, "Error during uninstall process", ex);
                }
            } else {
                if (logger.isLoggable(Logger.DEBUG)) {
                    logger.debug("Uninstall program is null or empty");
                }
            }
        } else {
            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Current version not found");
            }
        }


        // remove asset's information from DM
        assetDAO.removeAsset(asset);


        // remove asset's directory
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Remove the asset directory (" + workingDirectory + ")");
        }

        try {
            FileSystemTools.removeDirectoryTree(workingDirectory);
        }
        catch (Exception ex) {
            // ignore this error because the asset is already removed
        }

        // if manufacturer's directory is empty will must removed
        File file = new File(this.installationDirectory + "/" + manufacturer);
        file.delete();

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Asset removed");
        }

    }


    /**
     * Return list of asset that require the installation process
     *
     * <p>A asset requires a installation if his state is:
     * <ui>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_NEW
     *      STATE_NEW}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_UPDATE
     *      STATE_UPDATE}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_FILE_DOWNLOAD
     *      STATE_FILE_DOWNLOAD}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_FILE_EXTRACTED
     *      STATE_FILE_EXTRACTED}</li>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_PREVIOUS_VERSION_UNINSTALLED
     *      STATE_PREVIOUS_VERSION_UNINSTALLED}</li>
     * </ui>
     *
     * @return list of asset that require the installation process
     */
    public Vector listAssetsForInstallation() {

        String[] states = {
            Asset.STATE_NEW,
            Asset.STATE_UPDATE,
            Asset.STATE_FILE_DOWNLOAD,
            Asset.STATE_FILE_EXTRACTED,
            Asset.STATE_PREVIOUS_VERSION_UNINSTALLED,
        };

        return listAssets(states);
    }

    /**
     * Return list of asset that require the uninstallation process
     *
     * <p>A asset requires a uninstallation if its state is:
     * <ui>
     * <li>{@link com.funambol.syncclient.spap.Asset#STATE_DELETE
     *      STATE_DELETE}</li>
     *
     * @return list of asset that require the uninstallation process
     */
    public Vector listAssetsForRemoving() {
        return listAssets(Asset.STATE_DELETE);
    }

    /**
     * Returns the list of the asset with the given states
     *
     * @param states array of the state
     * @return list of the asset with the given states
     */
    public Vector listAssets(String[] states) {
        Vector assetsList = new Vector();
        Vector temp = null;
        if (states==null || states.length==0) {
            assetsList = assetDAO.getAllAsset();
        } else {
            int numState = states.length;
            for (int i=0; i<numState; i++) {
                temp = listAssets(states[i]);
                VectorTools.add(assetsList,temp);
            }
        }
        return assetsList;
    }


    /**
     * Returns the list of the asset with the given state.
     * <p>If the given state is <code>null</code> returns the all assets
     *
     * @param state array of the state. If <code>null</code> returns the all assets
     * @return list of the asset with the given states
     */
    public Vector listAssets(String state) {
        Vector assetsList = null;
        if (state==null || state.equals("")) {
            assetsList = assetDAO.getAllAsset();
        } else {
            assetsList = assetDAO.listAsset(state);
        }

        return assetsList;
    }


    /**
     * Returns the state of the asset identified from the given id
     * @param idAsset identifier of the asset
     *
     * @return the state of the asset
     * @throws AssetManagementException if asset not exists
     */
    public String getAssetState(String idAsset) throws AssetManagementException {
        return assetDAO.getAssetState(idAsset);
    }

    /**
     * Set the state of the given asset.
     *
     * @param asset the asset to be set
     * @param state the new state
     *
     * @throws AssetManagementException in case of errors
     */
    public void setAssetState(Asset asset, String state)
    throws AssetManagementException {
        assetDAO.setAssetState(asset, state);
    }


    /**
     * Returns the asset with the given identifier
     *
     * @param idAsset the identifier of the asset
     * @return the Asset with the given identifier
     * @throws AssetManagementException if Asset not exist
     */
    public Asset getAsset(String idAsset) throws AssetManagementException {
        return assetDAO.getAsset(idAsset);
    }

    /**
     * Sets asset's state to <i>Asset.STATE_NEW_VERSION_NOT_WISHED</i>
     *
     * @param idAsset identifier of the asset
     * @throws AssetManagementException if an error occurs
     */
    public void setAssetAsNotWanted(String idAsset) throws AssetManagementException {
        Asset asset = assetDAO.getAsset(idAsset);
        String state = asset.getState();

        if (!state.equals(Asset.STATE_NEW) &&
            !state.equals(Asset.STATE_UPDATE) &&
            !state.equals(Asset.STATE_FILE_DOWNLOAD) &&
            !state.equals(Asset.STATE_FILE_EXTRACTED) &&
            !state.equals(Asset.STATE_PREVIOUS_VERSION_UNINSTALLED)) {

            throw new AssetManagementException(asset, "Impossible setting like NOT WISHED" +
                                     " (state: " + state + ")");
        }


        assetDAO.setAssetState(idAsset, Asset.STATE_NEW_VERSION_NOT_WANTED);
    }


    /**
     * Adds the given asset.
     *
     * @param asset the asset to add
     *
     * @throws AssetManagementException in case of errors
     */
    public void addAsset(Asset asset) throws AssetManagementException {
        assetDAO.setAsset(asset, new Timestamp(System.currentTimeMillis()));
    }

    // --------------------------------------------------------- Private Methods

    /**
     * Verification if it is necessary launch the uninstall program of the
     * previous version.
     * <p>It's necessary launch the uninstallation program if:
     * <ui>
     * <li>the current version exists</li>
     * <li>the current version has a uninstall program</li>
     * <li>the new version has <i>needUninstallPrev</i>='Y'</li>
     *
     * @param asset
     *
     * @throws Exception if an error occurs
     */
    private void verifyAndUninstallPreviousVersion(Asset asset)
            throws AssetInstallationException {


        String name = asset.getName().toLowerCase();
        String manufacturer = asset.getManufacturer().toLowerCase();

        String workingDirectory = this.installationDirectory + "/" + manufacturer + "/" + name;

        AssetVersion assetCurrentVersion = asset.getCurrentVersion();
        AssetVersion assetNewVersion = asset.getNewVersion();

        if (assetCurrentVersion != null) {
            String uninstallProgram = assetCurrentVersion.getUninstallProgram();
            if (uninstallProgram != null && !uninstallProgram.equals("")) {
                if (assetNewVersion.getNeedUninstallPrev().equalsIgnoreCase("Y")) {

                    String msgLog = "Uninstall previous version (Uninstall program: " +
                                    uninstallProgram                                  +
                                    ")"                                               ;

                    if (logger.isLoggable(Logger.DEBUG)) {
                        logger.debug(msgLog);
                    }

                    // devo lanciare il programma di disinstallazione

                    Launcher launcher = LauncherFactory.getLauncher(uninstallProgram);

                    InstallationContext ctx = new InstallationContext();
                    ctx.setWorkingDirectory(
                        workingDirectory +
                        File.separator   +
                        ASSET_CONTENT_DIRECTORY
                    );
                    ctx.setBinDirectory(binDirectory);
                    ctx.setLibDirectory(libDirectory);
                    ctx.setAsset(asset);

                    int exitState = launcher.execute(uninstallProgram, false, ctx);
                    if (exitState != 0) {
                        throw new AssetInstallationException("Uninstallation program exit with state: " +exitState);
                    }
                }
            }
        }
    }


    /**
     * Download content file for the new version of the given Asset.
     *
     * <p>The file is downloaded from the url:<br>
     * <i>http://hostServer:portServer/baseUrl/&#60;manufacturer&#62;/
     * &#60;name&#62;/&#60;version&#62;/&#60;contentFile&#62;</i>
     *
     * @param asset asset of the which download the content file
     * @return the byte array downloaded
     * @throws IOException if an error occurs
     */
    private byte[] downloadContentFileForNewVersion(Asset asset) throws IOException {
        byte[] content = null;
        AssetVersion newVersion = asset.getNewVersion();

        String name = asset.getName();
        String manufacturer = asset.getManufacturer();

        String version = newVersion.getVersion();
        String assetUrl = newVersion.getUrl();

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Download asset file: " + assetUrl.toString());
        }

        if (HttpTools.isHTTPURL(assetUrl)) {
            content =  HttpTools.downloadPackage(assetUrl);
        } else {
            if (assetUrl.startsWith("file:/")) {
                try {
                    assetUrl = new URL(assetUrl).getFile();
                } catch (MalformedURLException e) {
                    // we ignore it so that we treat it as a normal file
                }
            }
            content = FileSystemTools.getFile(assetUrl);
        }

        return content;
    }

    /**
     * Read the content file prevoiusly saved for the given asset
     *
     * @param asset
     * @return the byte array of the content file of the asset
     * @throws Exception if an error occurs
     */
    private byte[] getAssetPackage(Asset asset) throws Exception {

        AssetVersion assetNewVersion = asset.getNewVersion();

        if (assetNewVersion == null) {
            throw new AssetManagementException(asset, "New version not found");
        }

        String name = asset.getName().toLowerCase();
        String manufacturer = asset.getManufacturer().toLowerCase();

        String workingDirectory = this.installationDirectory + File.separator +
                                  manufacturer + File.separator + name +
                                  File.separator + ASSET_FILE_DIRECTORY;


        String contentFile = workingDirectory + File.separator +
                             asset.getName() + "_" + assetNewVersion.getVersion() + ".zip";

        return FileSystemTools.getFile(contentFile);

    }


}
