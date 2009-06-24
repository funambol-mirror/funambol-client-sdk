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

import java.io.*;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Vector;
import java.util.Hashtable;
import java.util.Properties;
import java.util.Enumeration;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

import com.funambol.syncclient.common.StringTools;
import com.funambol.syncclient.spdm.*;

import com.funambol.syncclient.sps.common.DataStoreMetadata;
import com.funambol.syncclient.spap.*;

/**
 * This class is responsible for the management of the SyncClient Conduit
 * applications. It performs the following tasks:
 * <ul>
 *  <li>install new applications
 *  <li>remove (uninstall) applications
 *  <li>configure applications
 * </ul>
 *
 * <b>Installation packages</b>
 * <p>
 * An installation package is a jar file containing a SyncClient Conduit
 * application. It is structured as follows:
 * <pre>
 *   - {application-description-file}.adf
 *   - {other packages and classes}
 *   - bin
 *     - {other binary files}
 * </pre>
 * For example:
 * <pre>
 *  - soccerleagues.adf
 *  - com
 *    - funambol
 *      - soccerleagues
 *        - SoccerleaguesStoreManager.class
 *  - bin
 *    - Leagues.prc
 *    - Italy.pdb
 *    - England.pdb
 * </pre>
 *
 * The package is first save as an asset, so that it can be handled by the
 * application provisioning subsystem.
 *
 * @version $Id: ApplicationManager.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class ApplicationManager {

    // --------------------------------------------------------------- Constants

    public static String[] ALLOWED_SYNC_MODES = new String[] {
        "none", "slow", "two-way", "one-way", "refresh"
    };

    // ------------------------------------------------------------ Private Data

    /**
     * The application configuration management tree
     */
    private ManagementNode applicationConfigNode;


    // ------------------------------------------------------------ Constructors

    /**
     * Creates a new instance of ApplicationManager. It stores the application
     * configuration management tree and the executables directory for further
     * use.
     *
     * @param applicationConfigNode application configuration management tree
     *
     */
    public ApplicationManager(ManagementNode applicationConfigNode) {
        this.applicationConfigNode = applicationConfigNode;

    }

    // ---------------------------------------------------------- Public methods

    /**
     * Install an application packaged in a jar/zip file as described in the class
     * description.
     * <p>
     * The file is read and copied in the installation directory; then the DM
     * structure required by the application provisioning subsystem is created
     * so that the application looks like an installable asset.
     *
     * @param f the package file
     *
     * @throws ApplicationManagementException if an error occurs during installation
     */
    public Application install(File f)
    throws ApplicationManagementException {
        String xmlADF    = null;
        String applicationURI = null;
        String adfFileName = null;

        int l = 0;

        Application app = null;
        Asset asset = null;
        try {
            adfFileName = getADFNameFromJar(f);
            xmlADF = readADFFromJar (f);
            app = applicationFromADF(xmlADF);
            asset = makeAsset(app, f);

            AssetManager assetManager = AssetManager.getAssetManager();

            assetManager.addAsset(asset);

            assetManager.installAsset(asset.getId());
            return app;
        } catch (FileNotFoundException e) {
            throw new ApplicationManagementException("Package " + f + " not found!", e);
        } catch (IOException e) {
            throw new ApplicationManagementException("Error reading package " + f, e);
        } catch (AssetManagementException e) {
            throw new ApplicationManagementException("Error storing the asset " + e, e);
        }
    }

    /**
     * Uninstall the application ideintified by the given application URI.
     * It removes both the application classes and configuration files.
     *
     * @param app the application to unistall
     *
     * @throws ApplicationManagementException in case of errors
     */
    public void uninstall(Application app)
    throws ApplicationManagementException {
        Asset asset = null;
        try {
            asset = makeAsset(app, null);

            AssetManager assetManager = AssetManager.getAssetManager();
            assetManager.setAssetState(asset, "D");
            assetManager.removeAsset(asset.getId());
        } catch (AssetManagementException e) {
            throw new ApplicationManagementException("Error storing the asset " + e, e);
        }
    }

    /**
     * Returns the installed applications.
     *
     * @return an array containing the installed applications
     */
    public Application[] getApplications()
    throws ApplicationManagementException {
        String applicationsSyncTmp = null;
        String applicationKeyTmp   = null;

        ManagementNode[] applicationNodes  = null;

        ManagementNode[] sources  = null;

        try {

            applicationNodes = applicationConfigNode.getChildren();

            DataStoreMetadata dsmd;

            Application[] applications = new Application[applicationNodes.length];
            for (int i = 0; i < applicationNodes.length; i++) {
                applications[i] = new Application((String) applicationNodes[i].getNodeValue("/application", "applicationURI"));
                applications[i].setDisplayName((String) applicationNodes[i].getNodeValue("/application", "applicationDisplayName"));
                applications[i].setAuthor((String) applicationNodes[i].getNodeValue("/application", "applicationAuthor"));
                applications[i].setDescription((String) applicationNodes[i].getNodeValue("/application", "applicationDescription"));
                applications[i].setVersion((String) applicationNodes[i].getNodeValue("/application", "applicationVersion"));
                applications[i].setAssetId((String) applicationNodes[i].getNodeValue("/application", "assetId"));
                applications[i].setSync(
                    Boolean.valueOf((String) applicationNodes[i].getNodeValue("/application", "sync")).booleanValue()
                );

                sources = applicationNodes[i].getChildNode("spds/sources").getChildren();
                for (int j = 0; j < sources.length; j++) {
                    dsmd = new DataStoreMetadata((String)sources[j].getValue("sourceURI"));
                    dsmd.setDisplayName((String)sources[j].getValue("name"));
                    dsmd.setDefaultSync((String)sources[j].getValue("sync"));
                    dsmd.setSyncModes(StringTools.split((String)sources[j].getValue("syncModes")));

                    applications[i].addDataStoreMetadata(dsmd);
                }
            }

            return applications;
        } catch (Exception e) {
            throw new ApplicationManagementException("Error loading applications parameters: " + e, e);
        }
    }


    /**
     * Creates an application object from the ADF file. An exception is thrown
     * if any error is found and the application object cannot be created.
     *
     * @param adf the adf content
     *
     * @throws ApplicationManagementException if there is something wrong with
     *         the ADF
     */
    public static Application applicationFromADF(String adf)
    throws ApplicationManagementException {
        try{
            //
            // First the header
            //
            String header = getXMLTagValue(adf, "header");

            if (StringTools.isEmpty(header)) {
                throw new ApplicationManagementException("Missing <header>...</header> in ADF");
            }

            //
            // Check thet all mandatory fields are specified
            //
            String[] mandatoryFields = new String[] {
                "application-name", "application-creator-id", "application-datastore-type",
                "application-display-name", "application-description", "application-support-url",
                "application-support-email", "store-manager-package", "content-id"
            };

            String value;
            for (int i=0; i<mandatoryFields.length; ++i) {
                value = getXMLTagValue(header, mandatoryFields[i]);

                if (StringTools.isEmpty(value)) {
                    throw new ApplicationManagementException(
                        "Missing <" + mandatoryFields[i] + ">...</" + mandatoryFields[i] + "> in ADF"
                    );
                }
            }

            //
            // Here header is ok, set Application properties
            //
            Application app = new Application(getXMLTagValue(header, "application-name"));

            app.setDisplayName     (getXMLTagValue(header, "application-display-name"  ));
            app.setCreatorId       (getXMLTagValue(header, "application-creator-id"    ));
            app.setDataStoreType   (getXMLTagValue(header, "application-datastore-type"));
            app.setContentId       (getXMLTagValue(header, "content-id"                ));
            app.setDescription     (getXMLTagValue(header, "application-description"   ));
            app.setSupportUrl      (getXMLTagValue(header, "application-support-url"   ));
            app.setSupportEmail    (getXMLTagValue(header, "application-support-email" ));
            app.setStoreManagerPkg (getXMLTagValue(header, "store-manager-package"     ));
            app.setAuthor          (getXMLTagValue(header, "application-author"        ));
            app.setVersion         (getXMLTagValue(header, "application-version"       ));

            //
            // Now datastores
            //
            Vector datastores;
            Vector xmlADFVector = new Vector();

            DataStoreMetadata md;

            xmlADFVector.addElement(adf);

            datastores = getXMLTag(xmlADFVector, "datastore");

            String[] syncModes;
            String defaultSync;
            int l = datastores.size();
            for (int i=0; i < l; ++i) {
                md = new DataStoreMetadata(getXMLTagValue((String) datastores.elementAt(i) , "name"));

                md.setDisplayName(getXMLTagValue((String) datastores.elementAt(i), "display-name"));
                syncModes = getSyncModes(getXMLTagValue((String) datastores.elementAt(i), "sync-modes"), ALLOWED_SYNC_MODES);
                md.setSyncModes(syncModes);
                defaultSync = getXMLTagValue((String) datastores.elementAt(i), "default-sync");
                checkSyncMode(defaultSync, syncModes);
                md.setDefaultSync(defaultSync);

                //
                // For backward compatibility (with conduit 1.2), if soft-sort
                // is not specified, it is set to true
                //
                try {
                    value = getXMLTagValue((String) datastores.elementAt(i), "soft-sort");
                } catch (Exception e) {
                    value = "true";
                }
                md.setSoftSort(new Boolean(value).booleanValue());

                //
                // For backward compatibility (with conduit 1.2), if store-volume
                // is not specified, it is set to the empty string
                //
                try {
                    value = getXMLTagValue((String) datastores.elementAt(i), "store-volume");
                } catch (Exception e) {
                    value = "";
                }
                md.setStoreVolume(value);

                app.addDataStoreMetadata(md);
            }

            return app;

        } catch (Throwable t) {
            throw new ApplicationManagementException("Unexpected error: " + t, t);
        }
    }

    /**
     * Creates an application object from an asset. An exception is thrown
     * if any error is found and the application object cannot be created.
     *
     * @param asset the asset from which create the application
     *
     * @throws ApplicationManagementException if there is something wrong with
     *         the asset
     */
    public static Application applicationFromAsset(Asset asset)
    throws ApplicationManagementException {
        Application a = new Application(asset.getName());

        a.setUri               (asset.getName()                   );
        a.setAuthor            (asset.getManufacturer()           );
        a.setAssetId           (asset.getId()                     );
        a.setVersion           (asset.getNewVersion().getVersion());
        a.setDescription       (asset.getDescription()            );
        a.setDisplayName       (asset.getName()                   );
        a.setDataStoresMetadata(      new Vector()                );

        return a;
    }

    /**
     * Creates an Asset object from an Application and its package.
     *
     * @param app the application object
     * @param packageFile the package file - NULL
     *
     * @throws ApplicationManagementException if something gets wrong
     */
    public static Asset makeAsset(Application app, File packageFile)
    throws ApplicationManagementException {
        try{
            Properties p = new Properties();

            //
            // If no asset id is provided (for instance during installation),
            // the current time millis is taken, otherwise (for instance during
            // uninstall) the one in the Application object is taken
            //
            if (StringTools.isEmpty(app.getAssetId())) {
                p.put(Asset.PROPERTY_ID, String.valueOf(System.currentTimeMillis()));
            } else {
                p.put(Asset.PROPERTY_ID, app.getAssetId());
            }
            p.put(Asset.PROPERTY_NAME, app.getUri());
            p.put(Asset.PROPERTY_MANUFACTURER, app.getAuthor());
            p.put(Asset.PROPERTY_DESCRIPTION, app.getDescription());
            p.put(Asset.PROPERTY_STATE, "U");

            p.put(
                AssetVersion.PROPERTY_VERSION,
                app.getVersion()
            );
            p.put(
                AssetVersion.PROPERTY_RELEASE_DATE,
                new Date(System.currentTimeMillis()).toString()
            );
            p.put(AssetVersion.PROPERTY_RELEASE_NOTES, "");
            if (packageFile != null) {
                p.put(
                    AssetVersion.PROPERTY_URL,
                    packageFile.toURL().toString()
                );
                p.put(
                    AssetVersion.PROPERTY_SIZE_ASSET_FILE,
                    String.valueOf(packageFile.length())
                );
            }
            p.put(
                AssetVersion.PROPERTY_INSTALL_PROGRAM,
                "com.funambol.syncclient.spap.installer.SPSInstaller.class"
            );
            p.put(
                AssetVersion.PROPERTY_UNINSTALL_PROGRAM,
                "com.funambol.syncclient.spap.installer.SPSInstaller.class"
            );
            p.put(
                AssetVersion.PROPERTY_NEED_UNINSTALL_PREV,
                "true"
            );

            return new Asset(p, true);
        } catch (Throwable t) {
            throw new ApplicationManagementException("Unexpected error: " + t, t);
        }
    }


    // --------------------------------------------------------- Private methods

    /**
     * Make a String[] by tags find with search.
     *
     * @param xmlInput tags about search
     * @param tag to find
     * @return find tags
     **/
    private static Vector getXMLTag(Vector xmlInput, String tag)
        throws ApplicationManagementException {

        Vector xmlReturn = new Vector();

        String xmlInputTag = null;

        String startTag = null;
        String endTag   = null;

        int i = 0;

        startTag = "<" + tag + ">";
        endTag   = "</" + tag + ">";

        for (int j=0; j < xmlInput.size(); j++) {

            xmlInputTag = (String) xmlInput.elementAt(j);

            try {

                while (xmlInputTag.indexOf(startTag) != -1) {
                    xmlReturn.addElement(xmlInputTag.substring(xmlInputTag.indexOf(startTag) + startTag.length(), xmlInputTag.indexOf(endTag)));
                    xmlInputTag = xmlInputTag.substring(xmlInputTag.indexOf(endTag) + endTag.length());
                    i++;
                }

            } catch (StringIndexOutOfBoundsException e) {
                throw new ApplicationManagementException(
                    "Error getting the value of <" + tag +">"
                );
            }

        }

        return xmlReturn;

    }

    /**
     * Make a String by value of <i>tag</i>.
     *
     * @param xml xml msg
     * @param tag tag to find
     * @return tag value
     **/
    private static String getXMLTagValue(String xml, String tag)
        throws ApplicationManagementException {

        String startTag  = null;
        String endTag    = null;
        String value     = null;

        startTag = "<" + tag + ">";
        endTag   = "</" + tag + ">";

        try {
            value = xml.substring(xml.indexOf(startTag) + startTag.length(), xml.indexOf(endTag));
        } catch (StringIndexOutOfBoundsException e) {
            throw new ApplicationManagementException(
                "Error getting the value of <" + tag +">"
            );
        }

        value = value.trim();

        while (value.indexOf("\n") != -1) {
            value = value.substring(0, value.indexOf("\n")) + value.substring(value.indexOf("\n") + 1);
        }

        return value.trim();

    }

    /**
     * Finds the Adpplication Descriptor File in the given jar file. If no
     * ADF is found, an ApplicationManagementException is thrown.
     *
     * @param f the jar file
     *
     * @return the ADF file name
     *
     * @throws ApplicationMAnagementException if no adf is found.
     */
    private String getADFNameFromJar(File f)
    throws ApplicationManagementException {
        try {
            JarFile jarFile = null;

            Enumeration zipEntries = null;

            ZipEntry zipEntry = null;

            boolean findADF = false;

            jarFile = new JarFile(f);

            zipEntries = (new JarFile (f)).entries();

            String adfFileName;
            while (zipEntries.hasMoreElements()) {
                zipEntry = (ZipEntry) zipEntries.nextElement();

                adfFileName = zipEntry.getName();

                if(adfFileName.endsWith(".adf")) {
                    return adfFileName;
                }
            }
        } catch (IOException e) {
            throw new ApplicationManagementException(
                "Error reading the jar file " + f + ": " + e, e
            );
        }

        throw new ApplicationManagementException(
            "No Application Definition File (.adf) found in " + f
        );
    }

    /**
     * Reads ADF file from JAR
     *
     * @param f jar file
     * @return ADF file
     **/
    private String readADFFromJar(File f)
    throws ApplicationManagementException, IOException {

        JarFile jarFile = null;

        Enumeration zipEntries = null;

        ZipEntry zipEntry = null;

        boolean findADF = false;

        jarFile = new JarFile(f);

        zipEntries = (new JarFile (f)).entries();

        String adfFileName;
        while (zipEntries.hasMoreElements()) {

            zipEntry = (ZipEntry) zipEntries.nextElement();

            adfFileName = zipEntry.getName();

            if(adfFileName.endsWith(".adf")) {
                findADF = true;
                break;
            }

        }

        if (!findADF) {
            throw new ApplicationManagementException (
                "No Application Description File (.adf) found in package " + f
            );
        }

        return read (jarFile.getInputStream(zipEntry));

    }

   /**
     * Reads the content of the given input stream.
     *
     * @param is the input stream
     **/
    private String read(InputStream is) throws IOException {
        StringBuffer sb = new StringBuffer();

        try {
            byte[] buf = new byte[1024];

            int nbyte = -1;
            while ((nbyte = is.read(buf)) >= 0) {
                sb.append(new String(buf, 0, nbyte));
            }
        } finally {
            is.close();
        }

        return sb.toString();
    }

    /**
     * Splits the comma separated sync modes given into a correspondig string
     * array. Each element is checked with the given allowed elements and if one of
     * the element to check is not in the superset, an IllegalArgumentException
     * is thrown.
     *
     * @param the comma separated list of sync modes
     * @param allowed the allowed superset
     *
     * @return the sync modes as a string arrey
     *
     * @throws IllegalArgumentException in case of one of the given value is not
     *         listed in <i>ALLOWED_SYNC_MODES</i>
     */
    private static String[] getSyncModes(String s, String[] allowed)
    throws IllegalArgumentException {
        String[] modes = StringTools.split(s);

        for (int i=0; i<modes.length; ++i) {
            checkSyncMode(modes[i], allowed);
        }

        return modes;
    }

    /**
     * Checks if the given mode is one of the allowed ones.
     *
     * @param mode the mode to check
     * @param allowed the allowed superset
     *
     * @throws IllegalArgumentException if mode is not in allowed[]
     */
    private static void checkSyncMode(String mode, String[] allowed) {
        for(int i=0; ((allowed != null) && (i<allowed.length)); ++i) {
            if (allowed[i].equals(mode)) {
                return;
            }
        }

        throw new IllegalArgumentException(
            "Invalid sync mode '"     +
            mode                      +
            "'; it must be one of ("  +
            StringTools.join(allowed) +
            ")"
        );
    }

}