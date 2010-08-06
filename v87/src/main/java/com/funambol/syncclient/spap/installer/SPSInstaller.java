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

import java.io.*;

import java.util.Vector;
import java.util.Hashtable;

import com.funambol.syncclient.common.FileSystemTools;
import com.funambol.syncclient.common.StringTools;

import com.funambol.syncclient.sps.common.DataStoreMetadata;
import com.funambol.syncclient.spap.Asset;
import com.funambol.syncclient.spap.Application;
import com.funambol.syncclient.spap.ApplicationManager;
import com.funambol.syncclient.spap.ApplicationManagementException;
import com.funambol.syncclient.spap.AssetManagementException;
import com.funambol.syncclient.spap.AssetInstallationException;
import com.funambol.syncclient.spap.installer.InstallationContext;
import com.funambol.syncclient.spdm.ManagementNode;
import com.funambol.syncclient.spdm.SimpleDeviceManager;
import com.funambol.syncclient.spdm.DMException;


/**
 * This is the installer class for SPS applications. It creates the required
 * DM structure and moves the datastore classes where appropriate.
 *
 *
 * @version $Id: SPSInstaller.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class SPSInstaller {

    private static final String PALMINSTALL = "palminstall.exe";
    private static final String PPCINSTALL  = "ppcinstall.exe";
    private static final String CONTEXT_APPLICATIONS = "conduit/applications";

    public static int install(InstallationContext ctx)
    throws AssetManagementException {
        try {
            ManagementNode appsNode =
                SimpleDeviceManager.getDeviceManager().getManagementTree(CONTEXT_APPLICATIONS);

            String      adf = null;
            Application app = null;

            try {
                adf = readADF(ctx.getWorkingDirectory());
            } catch (ApplicationManagementException e) {
                //
                // adf not found!
                //
                app = ApplicationManager.applicationFromAsset(ctx.getAsset());
            }

            if (app == null) {
                app = ApplicationManager.applicationFromADF(adf);
                app.setAssetId(ctx.getAsset().getId());
                saveStoreManagerClass(
                    ctx.getWorkingDirectory(),
                    ctx.getLibDirectory()
                );
            }

            registerApplication(appsNode, app);

            installBinaries( ctx.getWorkingDirectory(),
                             ctx.getBinDirectory()    ,
                             PALMINSTALL              ,
                             ".pdb"                   );
            installBinaries( ctx.getWorkingDirectory(),
                             ctx.getBinDirectory()    ,
                             PALMINSTALL              ,
                             ".prc"                   );
            installBinaries( ctx.getWorkingDirectory(),
                             ctx.getBinDirectory()    ,
                             PPCINSTALL               ,
                             ".cab"                   );
        } catch (AssetInstallationException e) {
            throw e;
        } catch (AssetManagementException e) {
            throw e;
        } catch (Exception e) {
            throw new AssetManagementException(e.getMessage(), e);
        }

        return 0;
    }

    public static int uninstall(InstallationContext ctx)
    throws AssetManagementException {
        ManagementNode appsNode =
                SimpleDeviceManager.getDeviceManager().getManagementTree(CONTEXT_APPLICATIONS);

        ManagementNode sourceNode       = null;
        ManagementNode[] sources        = null;

        Hashtable sourceValues = null;

        String storeManagerPackage = null;
        String storeManagerPath    = null;
        String storeManager        = null;
        String dirTmp              = null;

        String      adf = null;
        Application app = null;
        try {
            try {
                adf = readADF(ctx.getWorkingDirectory());
            } catch (ApplicationManagementException e) {
                //
                // adf not found!
                //
                app = ApplicationManager.applicationFromAsset(ctx.getAsset());
            }

            if (app == null) {
                app = ApplicationManager.applicationFromADF(adf);

                deleteStoreManagerClasses(ctx.getWorkingDirectory(), ctx.getLibDirectory());
            }

            appsNode.removeNode(app.getFixedURI());
        } catch (Exception e) {
            throw new AssetManagementException("Error reading configuration information: " + e, e);
        }
        return 0;
    }

    /**
     * Registers the given application into the Device Managet
     *
     * @param appsNode the management node under which applications are
     *        registered
     * @param app application to register
     *
     * @throws DMException in case of error
     */
    private static void registerApplication(ManagementNode appsNode, Application app)
    throws DMException {
        String value;

        //
        // Application info first
        //
        String uri = app.getFixedURI() + "/application";
        appsNode.setValue(
            uri, "applicationDisplayName", app.getDisplayName()
        );
        appsNode.setValue(
            uri, "applicationURI", app.getUri()
        );
        value = app.getSupportUrl();

        if (value != null) {
            appsNode.setValue(uri, "applicationSupportUrl", value);
        }
        value = app.getSupportEmail();
        if (value != null) {
            appsNode.setValue(uri, "applicationSupportMail", value);
        }

        appsNode.setValue(
            uri, "applicationAuthor", app.getAuthor()
        );
        appsNode.setValue(
            uri, "applicationDescription", app.getDescription()
        );
        appsNode.setValue(
            uri, "applicationVersion", app.getVersion()
        );
        appsNode.setValue(
            uri, "assetId", app.getAssetId()
        );
        appsNode.setValue(
            uri, "sync", "true" /* default */
        );

        String name = new File(app.getUri()).getName();

        //
        // Data stores now
        //
        Vector stores = app.getDataStoresMetadata();
        DataStoreMetadata md;
        int l = (stores != null) ? stores.size() : 0;

        // just create the context
        appsNode.setValue(app.getFixedURI() +  "/spds/sources", "", "");

        String node = app.getFixedURI() +  "/spds/sources/source";
        for (int i=0; i<l; ++i) {
            md = (DataStoreMetadata)stores.elementAt(i);
            appsNode.setValue(
                node + String.valueOf(i + 1), "sourceURI", app.getContentId() + '/' + md.getName()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "creatorId", app.getCreatorId()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "dataStoreType", app.getDataStoreType()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "syncModes", StringTools.join(md.getSyncModes())
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "sync", md.getDefaultSync()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "name", md.getDisplayName()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "sourceClass", "com.funambol.syncclient.spds.source.SPSSyncSource"
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "type", "xml/record"
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "storeManager", StringTools.replaceSpecial(name)
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "storeManagerPackage", app.getStoreManagerPkg()
            );
            appsNode.setValue(
                node + String.valueOf(i + 1), "sortDB", String.valueOf(md.isSoftSort())
            );
            if (md.getStoreVolume() != null) {
                appsNode.setValue(
                    node + String.valueOf(i + 1), "storeVolume", md.getStoreVolume()
                );
            }
        }
    }

    /**
     * Copy class files from the working directory where the asset has been
     * extracted to the desrination directory.
     *
     * @param workingDirectory the working directory where the asset has been extracted
     * @param destinationDirectory where to copy the files
     *
     **/
    private static void saveStoreManagerClass(
        String workingDirectory, String destinationDirectory
    ) throws IOException {
        FileOutputStream out = null;
        FileInputStream in = null;

        int l = workingDirectory.length();

        try{
            String[] classes = FileSystemTools.getAllFiles(workingDirectory, ".class");

            File source, dest;
            byte[] buf;
            for (int i=0; i<classes.length; ++i) {
                source = new File(classes[i]);
                dest = new File(destinationDirectory, classes[i].substring(l));

                dest.getParentFile().mkdirs();


                buf = new byte[(int)source.length()];

                in = new FileInputStream(source);
                in.read(buf);
                in.close(); in = null;

                out = new FileOutputStream(dest);
                out.write(buf);
                out.close(); out = null;
            }
        } finally {
            if (in != null) {
                in.close();
            }
            if (out != null) {
                out.close();
            }
        }
    }

    /**
     * Deletes the classes part of the installed package from the binary directory.
     *
     * @param workingDirectory the working directory
     *
     * @throws IOException in case of errors
     */
     private static void deleteStoreManagerClasses(String workingDirectory,
                                                   String destinationDirectory)
     throws IOException {
         int l = workingDirectory.length();

         String[] classes = FileSystemTools.getAllFiles(workingDirectory, ".class");

         File f, p;
         String[] children;
         for (int i=0; i<classes.length; ++i) {
             f = new File(destinationDirectory, classes[i].substring(l));
             p = new File(f.getParent());

             f.delete();

             //
             // If the directory remains empty, remove it and its empty parents
             //
             do {
                 children = p.list();

                 if ((children != null) && (children.length == 0)) {
                     p.delete();
                     p = new File(p.getParent());
                 }
             } while ((children != null) && (children.length == 0));
         }
     }

    /**
     * Reads the Application Description File from the working directory
     *
     * @param workingDirectory the working directory
     *
     * @return the content of the ADF file as a String
     */
    private static String readADF(String workingDirectory)
    throws ApplicationManagementException {
        try {
            String[] files = new File(workingDirectory).list();

            for (int i=0; i<files.length; ++i) {
                if (files[i].toLowerCase().endsWith(".adf")) {
                    return new String(
                        FileSystemTools.getFile(workingDirectory + File.separator + files[i])
                    );
                }
            }
        } catch (IOException e) {
            throw new ApplicationManagementException(
                "Error reading the adf file in " + new File(workingDirectory).getAbsolutePath() + ": " + e, e
            );
        }

        throw new ApplicationManagementException(
            "No Application Definition File (.adf) found in " + new File(workingDirectory).getAbsolutePath()
        );
    }

    /**
     * Executes the binaries installation external program
     *
     * @param workingDirectory the working directory
     * @param binDirectory the directory where the install program resides
     * @param installProgram the install program to use
     * @param fileType file type that the given install program has to process
     *
     * @throws AssetManagementException in case of errors
     */
    private static void installBinaries(String workingDirectory,
                                        String binDirectory    ,
                                        String installProgram  ,
                                        String fileType       )
    throws AssetManagementException {
        StringBuffer output = new StringBuffer();
        int c;

        if (!fileType.startsWith(".")) {
            fileType = "." + fileType;
        }

        try {
            String cmd = binDirectory + File.separator + installProgram + " ";

            String[] files = new File(workingDirectory).list();
            if (files == null) {
                return;
            }

            //
            // Processes all the files in the working directory; the onse that
            // are of te specified types are passed to the external installation
            // program.
            //
            for (int i=0; i<files.length; ++i) {

                if (!(files[i].toLowerCase().endsWith(fileType.toLowerCase()))) {
                    continue;
                }

                if (PALMINSTALL.equals(installProgram)) {
                    cmd += (' ' + files[i]);
                } else if (PPCINSTALL.equals(installProgram)) {
                    cmd += (" \"" + files[i] + "\"");
                }

                Process proc = Runtime.getRuntime().exec(
                    cmd,
                    null,
                    new File(workingDirectory)
                );

                InputStream is = proc.getErrorStream();

                while((c = is.read()) != -1) {
                    output.append((char)c);
                }

                if (PALMINSTALL.equals(installProgram)) {
                    proc.waitFor();
                }

                if (proc.exitValue() != 0) {
                    throw new AssetInstallationException( "Error executing "
                                                        + cmd
                                                        + ": "
                                                        + output.toString()
                                                        );
                }
            }
        } catch (AssetManagementException e) {
            throw e;
        } catch (Exception e) {
            throw new AssetManagementException(
                "Error executing the PRC/PDB installation: " + e, e
            );
        }
    }
}