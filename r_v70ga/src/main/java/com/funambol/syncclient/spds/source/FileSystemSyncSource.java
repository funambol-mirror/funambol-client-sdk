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
package com.funambol.syncclient.spds.source;

import java.io.*;

import java.security.Principal;

import java.util.Date;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;

import sync4j.framework.tools.Base64;
import sync4j.framework.core.AlertCode;

import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.SyncException;

/**
 * This class implements a file system <i>SyncSource</i>.
 *
 * IMPORTANT NOTE: on FAT16/32 file system the last modified timestamp of a file
 * is accurate with precision (...) of no more than 2 seconds (all timestamps have
 * even seconds and no milliseconds).
 * For instance, all times between 10:35:02,001 and 10:35:03,999 are rounded to
 * 10:35:04,000. On this filesystem, the risk is to detect a change more times.
 * On EXT2/3 file system the last modified timestamp of a file
 * is accurate with precision (...) of no more than 1 seconds (all timestamps have
 * not milliseconds).
 * For instance, all times between 10:35:03,001 and 10:35:03,999 are truncated to
 * 10:35:03,000. On this filesystem, the risk is to not detect a change because the
 * update time is truncated.
 * To avoid to detect a change more times (on FAT32) and to avoid to not detect
 * a change (on EXT3), we round also the since timestamp according to the
 * underlying FS.
 *
 * @version $Id: FileSystemSyncSource.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class FileSystemSyncSource implements SyncSource {

    // --------------------------------------------------------------- Constants

    public static final String DATABASE_FILE_NAME = "sync.db";
    public static final String DATABASE_HEADER    =
                         "FileSystemSyncSource file database";
    public static final String FORMAT_BASE64      = "b64"    ;

    // -------------------------------------------------------------- Properties

    /**
     * The drive where to read the sourceDirectory
     */
    private String sourceDrive;
    public void setSourceDrive(String sourceDrive) {
        this.sourceDrive = sourceDrive;
        this.sourceDirectory = sourceDrive + getSourceDirectory();
    }
    public String getSourceDrive() {
        return this.sourceDrive;
    }
    
    /**
     * The directory where files are stored (the default is the current
     * directory) - read/write
     */
    private String sourceDirectory = ".";

    public void setSourceDirectory(String sourceDirectory) {
        if (sourceDrive != null) {
            sourceDirectory = sourceDrive + sourceDirectory;
        }
        this.sourceDirectory = sourceDirectory;
    }

    public String getSourceDirectory() {
        return this.sourceDirectory;
    }

    private String name;
    public String getName() {
        return name;
    }
    public void setName(String name) {
        this.name = name;
    }

    private String type;
    public String getType() {
        return this.type;
    }
    public void setType(String type) {
        this.type = type;
    }

    private boolean encode = false;
    public boolean isEncode() {
        return encode;
    }
    public void setEncode(boolean encode) {
        this.encode = encode;
    }
    public void setEncode(String encode)  {
        if ("true".equals(encode)) {
            this.encode = true;
        } else {
            this.encode = false;
        }
    }

    private String sourceURI;

    /**
     * Getter for property uri.
     * @return Value of property uri.
     */
    public String getSourceURI() {
        return sourceURI;
    }

    /**
     * Setter for property uri.
     * @param sourceURI New value of property uri.
     */
    public void setSourceURI(String sourceURI) {
        this.sourceURI = sourceURI;
    }

    /**
     * Counter used to create the name of the file created Adding an item
     */
    protected int counter = 0;

    // ------------------------------------------------------------ Private data
    protected boolean isFAT32 = false;
    protected boolean isEXT3  = false;

    // ------------------------------------------------------------ Constructors

    /** Creates a new instance of FileSystemSyncSource */
    public FileSystemSyncSource() {
    }

    // ---------------------------------------------------------- Public methods
    public void beginSync(int type) throws SyncException {

        //
        // Check the give sourcedirectory:
        // - if it is not a directory, throw an exception
        // - if it does not exist, create it
        //
        File f = new File(sourceDirectory);
        if (!f.exists()) {
            if (!f.mkdirs()) {
                throw new SyncException("Error in creating " + f.getAbsoluteFile());
            }
        } else if (!f.isDirectory()) {
            throw new SyncException(
                "Destination directory (" + sourceDirectory + ") not existing."
            );
        }
        //
        // Why do do this? see class description :)
        //
        //
        // To detect if the file system rounds the last modified timestamp,
        // we create a temporary file and set its last modified timestamp.
        // Then we re-read the timestamp and if it is different from the one
        // set, it means that it was rounded.
        // If the difference is greater than 0, it means we are on EXT3.
        // If the difference is smaller than 0, it means we are on FAT32.
        //
        try {
            //
            // We know that the following timestamp is rounded on FAT32 FS and
            // on EXT2 FS
            //
            final long time = 1114166888033l;
            f = File.createTempFile("s4j", "", new File(getSourceDirectory()));

            f.setLastModified(time);
            long lastModified = f.lastModified();
            long offset = time - lastModified;
            if (offset < 0) {
                isFAT32 = true;
            } else if (offset > 0) {
                isEXT3 = true;
            }

            f.delete();
        } catch (IOException ex) {
        }

        if (type == AlertCode.REFRESH_FROM_SERVER ||
            type == AlertCode.REFRESH_FROM_SERVER_BY_SERVER) {
            // When a REFRESH from server is to be performed, we must remove the
            // local files
            emptyDataStore();
        } else {
            restoreSyncDB();
        }

        counter = 0;
    }

    public void endSync() throws SyncException {
    }

    public SyncItem[] getAllSyncItems(Principal principal) throws SyncException {
        return filterSyncItems(principal, null, SyncItemState.UNKNOWN);
    }

    public SyncItem[] getDeletedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        return filterSyncItems(principal, since, SyncItemState.DELETED);
    }

    public SyncItem[] getNewSyncItems(Principal principal,
                                      Date since    ) throws SyncException {
        return filterSyncItems(principal, since, SyncItemState.NEW);
    }

    public SyncItem[] getUpdatedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        return filterSyncItems(principal, since, SyncItemState.UPDATED);
    }

    public void removeSyncItem(Principal principal, SyncItem syncItem) throws SyncException {
        String fileName = syncItem.getKey().getKeyAsString();

        new File(sourceDirectory, fileName).delete();

        removeState(principal, fileName);
    }

    public SyncItem setSyncItem(Principal principal, SyncItem syncItem)
    throws SyncException {

        char itemState = syncItem.getState();

        try {
            String fileName = null;
            if (itemState == SyncItemState.NEW) {
                fileName = getUniqueFileName();
            } else {
                fileName = syncItem.getKey().getKeyAsString();
            }

            byte[] fileContent =
                (byte[])syncItem.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT);

            File f = new File(sourceDirectory, fileName);
            FileOutputStream fos = new FileOutputStream(f);
            if (fileContent != null) {
               if (encode) {
                   fos.write(Base64.decode(fileContent));
               } else {
                   fos.write(fileContent);
               }
            }
            fos.close();

            Date t = (Date) syncItem.getPropertyValue(SyncItem.PROPERTY_TIMESTAMP);
            f.setLastModified(roundTime(t.getTime()));

            setState(principal,
                     fileName,
                     SyncItemState.SYNCHRONIZED,
                     roundTime(t.getTime()));

            SyncItem newSyncItem =
                new SyncItemImpl(this, fileName, SyncItemState.NEW);

            newSyncItem.setProperties(syncItem.getProperties());

            return newSyncItem;
        } catch (IOException e) {
            throw new SyncException( "Error setting the item "
                                   + syncItem
                                   , e
                                   );
        }
    }

    public void commitSync() {
    }

    // ------------------------------------------------------- Protected Methods
    /**
     * The synchronization database is stored in a file whose name is given
     * by the value of the constant DATABASE_FILE_NAME prefixed by the user name
     * and the device id. For example:
     * <pre>
     *   guest.FunambolTest.sync.db
     * </pre>
     * The database is a property file where each entry has the following
     * format:
     * <pre>
     * [filename]=[state][lastmodified_timestamp]
     * </pre>
     * For example:
     * <blockquote>
     * readme.txt=U98928743098094
     * </blockquote>
     * <p>
     * updateSyncDatabase works as follows:
     * <pre>
     * 1.    Read the existing database (if it exists)
     * 2.    Scan the source directory getting all files in the directory
     * 3.    For each file f in the source directory
     * 3.1.    If f is already in the database
     * 3.1.1.    If f has been modified after the lastmodified_timestamp stored into the database
     * 3.1.1.1.    Set the state of the file to UPDATE and store the new lastmodified_timestamp
     * 3.2.    Else
     * 3.2.1.    Add f to the database setting its state to NEW and store the lastmodified_timestamp
     * 3.    End For each
     * 4.    For each file f in the database
     * 4.1.    If f does not exist in the source directory
     * 4.1.1.    Set the state to DELETED
     * 5.    End For each
     * </pre>
     * <p>
     * At the end of the process, the updated database is saved and than returned.
     *
     * @return the updated databse. In case of error, an error message is traced
     *         and an empty Properties object is returned.
     */
    protected Properties updateSyncDatabase(Principal principal, long since) {

        Properties syncDatabase = new Properties();

        try {
            File fileSyncDatabase = getDatabaseFile(principal);

            //
            // Reads the existing database
            //
            if (fileSyncDatabase.exists()) {
                FileInputStream fis = new FileInputStream(fileSyncDatabase);
                syncDatabase.load(fis);
                fis.close();
            }

            //
            // Get the list of files in the source directory
            //
            Vector existingFiles = getExistingFiles();

            //
            // Get the list of the file in the databsae
            //
            Enumeration databaseFiles = syncDatabase.propertyNames();

            String state    = null,
                   fileName = null;

            long lastModified;

            int n = existingFiles.size();
            for (int i=0; i < n; ++i) {
                fileName = (String)existingFiles.elementAt(i);
                lastModified = new File(sourceDirectory, fileName).lastModified();

                state = syncDatabase.getProperty(fileName);

                if (state != null) {
                    //
                    // The file is already in the database
                    //
                    long lastModifiedFromState = lastModifiedFromStateString(state);
                    char currentState = stateFromStateString(state);
                    if (currentState == SyncItemState.NEW && lastModifiedFromState == (since + 1)) {
                        //
                        // The item has been already detected as new in the current sync 
                        // (when an item is detected as "NEW" its lastmodified is 
                        // set to since + 1
                        //
                        continue;
                    }
                    if (lastModified > lastModifiedFromState) {
                        state = buildStateString(SyncItemState.UPDATED, lastModified);
                        syncDatabase.put(fileName, state);
                    }
                } else {
                    //
                    // The file is not in the database.
                    // We set its state at N with timestamp equal to the "since" 
                    // of the current synchronization plus 1.
                    // In this way, in the current sync is recognize as N, but
                    // in the next sync this item is not recognize as N.
                    //
                    state = buildStateString(SyncItemState.NEW, since + 1);
                    syncDatabase.put(fileName, state);
                }
            }  // next i

            for (; databaseFiles.hasMoreElements();) {
                fileName = (String)databaseFiles.nextElement();

                if (!existingFiles.contains(fileName)) {
                    state = buildStateString(SyncItemState.DELETED,
                                             roundTime(System.currentTimeMillis()));
                    syncDatabase.put(fileName, state);
                }
            }  // next

            //
            // Save & return
            //
            FileOutputStream fos = new FileOutputStream(fileSyncDatabase);
            syncDatabase.save(fos, DATABASE_HEADER);
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return syncDatabase;
    }

    private File getDatabaseFile(Principal principal) {
        return new File(sourceDirectory + '.' + DATABASE_FILE_NAME);
    }

    private String buildStateString(char state, long lastModified) {
        return state + String.valueOf(lastModified);
    }

    protected long lastModifiedFromStateString(String state) {
        return Long.parseLong(state.substring(1));
    }

    protected char stateFromStateString(String state) {
        if ((state == null) || (state.length() == 0)) return SyncItemState.UNKNOWN;

        return state.charAt(0);
    }

    protected Vector getExistingFiles() throws IOException {
        Vector ret = new Vector();

        String[] files = new File(sourceDirectory).list();

        if (files != null) {
            for (int i = 0; i<files.length; ++i) {
                if (!files[i].endsWith('.' + DATABASE_FILE_NAME)) {
                    ret.addElement(files[i]);
                }
            }  // next i
        }

        return ret;
    }

    protected void setState(Principal principal,
                            String    file,
                            char      state,
                            long      timestamp) {
        try {
            Properties syncDatabase = new Properties();

            File fileSyncDatabase = getDatabaseFile(principal);

            //
            // Reads the existing database
            //
            if (fileSyncDatabase.exists()) {
                FileInputStream fis = new FileInputStream(fileSyncDatabase);
                syncDatabase.load(fis);
                fis.close();
            }

            syncDatabase.put(file,
                             buildStateString(state, timestamp));

            FileOutputStream fos = new FileOutputStream(fileSyncDatabase);
            syncDatabase.save(fos, DATABASE_HEADER);
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    protected void removeState(Principal principal, String file) {
        try {
            File fileSyncDatabase = getDatabaseFile(principal);

            if (!fileSyncDatabase.exists()) return;

            Properties syncDatabase = new Properties();

            //
            // Reads the existing database
            //

            FileInputStream fis = new FileInputStream(fileSyncDatabase);
            syncDatabase.load(fis);
            fis.close();

            syncDatabase.remove(file);

            FileOutputStream fos = new FileOutputStream(fileSyncDatabase);
            syncDatabase.save(fos, DATABASE_HEADER);
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Filters the SyncItems in the synchronization database (after a refresh)
     * based on the given principal, last sync timestamp and state (see
     * SyncItemState). If state is equals to UNKNOWN all items are returned.<br>
     * Note that the current implementation ignores the principal: data do not
     * depend on users.
     *
     * @param principal principal. null means any
     * @param since     last sync timestamp. null neans since ever
     * @param state     the state to use as filter
     *
     * @return an array of SyncItem objects whose state is equal to the given
     *         state.
     */
    protected SyncItem[] filterSyncItems(Principal principal,
                                         Date      since    ,
                                         char      state    )
    throws SyncException {

        Vector syncItems = new Vector();

        long fileTimestamp,
             sinceTimestamp = (since == null) ? -1 : roundTime(since.getTime());
        
        Properties syncDatabase = updateSyncDatabase(principal, sinceTimestamp);
        
        SyncItem  syncItem      = null;
        String    fileName      = null;
        String    stateString   = null;
        char      fileState           ;
        for (Enumeration e = syncDatabase.keys(); e.hasMoreElements(); ) {
            fileName  = (String)e.nextElement();
            stateString = (String)syncDatabase.get(fileName);
            fileState = stateFromStateString(stateString);
            if ((state == SyncItemState.UNKNOWN) || (fileState == state)) {
                fileTimestamp = lastModifiedFromStateString(stateString);
                if (fileTimestamp > sinceTimestamp ) {
                    syncItem = new SyncItemImpl(this, fileName, fileState);
                    if (encode){
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_BINARY_CONTENT         ,
                                                 Base64.encode(readFileContent(fileName)))
                        );
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_FORMAT,
                                                 FileSystemSyncSource.FORMAT_BASE64)
                        );
                    } else {
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_BINARY_CONTENT,
                                                 readFileContent(fileName)       )
                        );
                    }
                    syncItem.setProperty(
                        new SyncItemProperty(SyncItem.PROPERTY_TYPE,
                                             this.getType())
                    );
                    syncItems.addElement(syncItem);
                }
            }
        }  // next e

        SyncItem[] ret = new SyncItem[syncItems.size()];
        for (int i=0; i<ret.length; ++i) {
            ret[i] = (SyncItem)syncItems.elementAt(i);
        }

        return ret;
    }

    /**
     * Reads the content of the given file.
     *
     * @param   fileName    the name of the file to read
     *
     * @return the file content as a byte[]
     */
    protected byte[] readFileContent(String fileName) {
        byte buf[] = null;

        try {
            File file = new File(sourceDirectory, fileName);

            if (!file.exists()) return new byte[0];

            buf = new byte[(int)file.length()];

            FileInputStream fis = new FileInputStream(file);
            fis.read(buf);
            fis.close();
        } catch (IOException e) {
            buf = new byte[0];
            e.printStackTrace();
        }

        return buf;
    }

    /**
     * remove old deleted files from syncDB
     */
    protected void restoreSyncDB() throws SyncException {

        Properties syncDatabase = new Properties();

        String state    = null;
        String fileName = null;

        try {

            File fileSyncDatabase = getDatabaseFile(null);

            //
            // Reads the existing database
            //
            if (fileSyncDatabase.exists()) {
                FileInputStream fis = new FileInputStream(fileSyncDatabase);
                syncDatabase.load(fis);
                fis.close();
            }

            //
            // Get the list of the file in the databsae
            //
            Enumeration databaseFiles = syncDatabase.propertyNames();

            for (; databaseFiles.hasMoreElements();) {
                fileName = (String)databaseFiles.nextElement();

                state = syncDatabase.getProperty(fileName);

                if (stateFromStateString(state) == SyncItemState.DELETED) {
                    syncDatabase.remove(fileName);
                }
            }  // next


            //
            // Save & return
            //
            FileOutputStream fos = new FileOutputStream(fileSyncDatabase);
            syncDatabase.save(fos, DATABASE_HEADER);
            fos.close();
        } catch (IOException e) {
            throw new SyncException( "Error reading sync database: "
                                   + e.getMessage());
        }
    }

    /**
     * reset the syncDB removing all items
     */
    protected void emptyDataStore() throws SyncException {

        Properties syncDatabase = new Properties();

        String state    = null;
        String fileName = null;

        try {

            File fileSyncDatabase = getDatabaseFile(null);

            //
            // Reads the existing database
            //
            if (fileSyncDatabase.exists()) {
                FileInputStream fis = new FileInputStream(fileSyncDatabase);
                syncDatabase.load(fis);
                fis.close();
            }

            //
            // Get the list of the file in the databsae
            //
            Enumeration databaseFiles = syncDatabase.propertyNames();

            for (; databaseFiles.hasMoreElements();) {
                fileName = (String)databaseFiles.nextElement();

                state = syncDatabase.getProperty(fileName);

                // Remove from file system and from db
                File file = new File(sourceDirectory, fileName);
                file.delete();

                syncDatabase.remove(fileName);
            }  // next

            //
            // Save & return
            //
            FileOutputStream fos = new FileOutputStream(fileSyncDatabase);
            syncDatabase.save(fos, DATABASE_HEADER);
            fos.close();
        } catch (IOException e) {
            throw new SyncException( "Error reading sync database: "
                                   + e.getMessage());
        }
    }



    /**
     * Rounds the given time according to the underlying FS.
     * If this is <code>EXT3</code> the time is rounded
     * calling <code>roundTimeEXT3</code> otherwise if this is
     * <code>FAT32</code> the time is rounded calling
     * <code>roundTimeFAT32</code>.
     * @param time the time to round
     * @return long the time rounded
     */
    protected long roundTime(long time) {
        if (isEXT3) {
            return roundTimeEXT3(time);
        } else if (isFAT32) {
            return roundTimeFAT32(time);
        }

        return time;
    }

    /**
     * Rounds the given time according to the approximation done by the EXT3
     * filesystem. The milliseconds are simply truncated.
     * @param time the time to round
     * @return long the time rounded
     */
    protected long roundTimeEXT3(long time) {
        return time / 1000 * 1000;
    }

    /**
     * Rounds the given time according to the approximation done by the FAT32
     * filesystem.
     * In the FAT32 all times haven't milliseconds and have only even seconds.
     * So, for instance, all times between 10:35:32,001 and 10:35:33,999 are
     * rounded in 10:35:34,000.
     * @param time the time to round
     * @return long the time rounded
     */
    protected long roundTimeFAT32(long originalTime) {

        long time = originalTime / 1000;
        long offset;
        if ((time % 2) == 0) {
            offset = 2000;
        } else {
            offset = 1000;
        }

        time = time * 1000;

        if (originalTime != time || offset == 1000) {
            time = time + offset;
        }
        return time;
    }

    /**
     * Returns a unique filename created as: "timestamp-counter"
     * @return String
     */
    protected String getUniqueFileName() {
        //
        // Using the timestamp and the counter to create the filename
        //
        StringBuffer tmp = new StringBuffer(String.valueOf(System.currentTimeMillis()));
        tmp.append('-').append(counter++);
        return tmp.toString();

    }
}
