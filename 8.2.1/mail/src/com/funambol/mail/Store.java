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

package com.funambol.mail;

import com.funambol.storage.RmsRecordItem;

import java.io.DataInputStream;

/**
 * <p>This interface defines a hierarchical object store and its access protocol.
 * It can be used to store and retrieve <code>Folder</code> items and the
 * contained child items.</p>
 *
 * <p>A <code>Folder</code> child is represented as a generic Serializable object
 * in order to store any kind of data, including:
 * <li><code>AccountFolder</code> items;</li>
 * <li><code>Folder</code> items;</li>
 * <li><code>Message</code> items.</li>
 * </p>
 */
public interface Store {
   
    /** Store available versions */
    public static final int VERSION_101 = 101;
    public static final int VERSION_102 = 102;
    public static final int VERSION_103 = 103;
    public static final int VERSION_104 = 104;

    // First Store version supporting Account information
    public static final int VERSION_105 = 105;
    
    /** Latest version */
    public static final int LATEST_VERSION = VERSION_105;

    /**
     * Initialize the store.
     *
     * @param reset If true, erase and re-create all the store.
     */
    public void init(boolean reset) throws MailException ;

    /**
     * Retrieves the list of all the folders directly under this Store.
     * 
     * @return An array of folders, or <code>null</code> if the store is empty.
     */
    public Folder[] list();

    /**
     * Returns the list of the subfolders contained under the specified
     * <code>Folder</code>.
     *
     * @param path The path of the parent <code>Folder</code>.
     * @return An array of folders, or <code>null</code> if <code>path</code>
     *         has no subfolders.
     *
     * @throws MailException If the path is not valid.
     */
    public Folder[] list(String path) throws MailException;

    /**
     * Add a new <code>Folder</code> to the <code>Store</code>.
     * 
     * @param folder The <code>Folder</code> item to be stored.
     * @return The added <code>Folder</code> reference
     * @throws MailException If an error occurs on the store (e.g. no space 
     *                       left).
     */
    public Folder addFolder(Folder folder) throws MailException;

    /**
     * Removes a <code>Folder</code> from the record store. The removal can be
     * recursive, if the <code>Folder</code> contains subfolders and the
     * removal is not recursive, the method will fail.
     * 
     * @param folder The <code>Folder</code> to be removed. Use
     *               <code>getFolder</code> to retrieve the <code>Folder</code>
     *               from its path.
     * @param recursive Perform a recursive removal.
     * @return <code>true</code> if the <code>Folder</code> has been actually
     *         deleted, <code>false</code> if the <code>Folder</code> did not
     *         exist.
     * @throws MailException If an error occurs on the store.
     */
    public boolean removeFolder(Folder folder, boolean recursive) throws MailException;
    
    /**
    * Recursively removes a <code>Folder</code> given the <code>Folder</code>
    * name
    * @param folderPath the path of the <code>Folder</code> to be removed
    * @throws com.funambol.mail.MailException if a storage error occurs
    */
    public void removeFolder(String folderPath) throws MailException;

    /**
     * Retrieves a <code>Folder</code> object by path.
     *
     * @param path The path to the Folder in the device's file system
     * @return The <code>Folder</code> object.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public Folder getFolder(String path) throws MailException;

    /**
     * Retrieves a <code>Folder</code> object by path.
     * 
     * @param path The path to the Folder in the device's file system
     * @param lightFolder If true return the <code>Folder</code> item without
     * reading its properties (e.g. role, creation date). It doesn't check
     * if it really exists in the Store.
     * @return The <code>Folder</code> object.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public Folder getFolder(String path, boolean lightFolder) throws MailException;

    /**
     * Retrieves a <code>Folder</code> in this Store that matches the provided
     * substring.
     * 
     * @param path The partial path in the device's file system to the searched
     *             <code>Folder</code>.
     * @return A list of <code>Folder</code> objects whose path matches the
     *         provided substring, or <code>null</code> if there are no matches.
     */
    public Folder[] findFolders(String path);

    /**
     * Retrieve a subfolder from a folder record store and return its record id.
     *
     * @param subfolderPath The path to the subfolder
     * @param parentPath The path to the parent folder
     * @return The folder ID, <code>null</code> if it's not found
     */
    public String retrieveSubfolderID(String subfolderPath, String parentPath);

    /**
     * This method returns the array of the childs IDs contained in a
     * <code>Folder</code>.
     *
     * @param path The complete path of the <code>Folder</code>.
     * @return An array containing all the childs Ids, or <code>null</code> if
     *         the <code>Folder</code> is empty.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public String[] getChildIDs(String path) throws MailException;

    /**
     * This method reads a <code>Folder</code> from the Store, using the child
     * id.
     * 
     * @param path The complete path of the <code>Folder</code> containing the
     *             child.
     * @param childId The child unique ID.
     *         
     * @return The <code>RmsRecordItem</code> object corresponding to the
     *         childId.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public RmsRecordItem readChild(String path, String childId) throws MailException;

    /**
     * This method returns the InputStream of a <code>Folder</code> child.
     *
     * @param path The complete path of the <code>Folder</code> containing the
     *             child.
     * @param childId The child unique ID.
     *
     * @return The <code>DataInputStream</code> corresponding to the childId.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public DataInputStream readChildBytes(String path, String childId) throws MailException;

    /**
     * Return the first child from the specified <code>Folder</code>.
     *
     * @return The first child if it exists, null otherwise.
     * @throws MailException if the store cannot be accessed.
     */
    public RmsRecordItem readFirstChild(String path) throws MailException;

    /**
     * Return the next child from the specified <code>Folder</code>.
     *
     * @return The next child if it exists, null otherwise.
     * @throws MailException if the store cannot be accessed.
     */
    public RmsRecordItem readNextChild(String path) throws MailException;

    /**
     * This method add a new child to a <code>Folder</code>.
     *
     * @param path The complete path of the <code>Folder</code>.
     * @param child The <code>RmsRecordItem</code> child to save.
     * @return The new child id
     * @throws MailException If an error occurs accessing the Store.
     */
    public int addChild(String path, RmsRecordItem child) throws MailException;

    /**
     * This method updates an existing <code>Folder</code> child in the Store.
     *
     * @param path The complete path of the <code>Folder</code> containing the
     *             child.
     * @param child The <code>RmsRecordItem</code> child to save.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public void updateChild(String path, RmsRecordItem child) throws MailException;

    /**
     * This method removes a <code>Folder</code> child from the Store.
     *
     * @param path The complete path of the <code>Folder</code> containing the
     *             child.
     * @param childId A string representing the unique child ID.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public void removeChild(String path, String childId) throws MailException;

    /**
     * This method returns the count of childs in a <code>Folder</code>.
     *
     * @param path The complete path of the <code>Folder</code>.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public int countChilds(String path) throws MailException;

    /**
     * This method returns the count of childs in a <code>Folder</code>. Only
     * childs with the specified prefix will be counted.
     *
     * @param path The complete path of the <code>Folder</code>.
     * @param prefix The childs type to be counted.
     *
     * @throws MailException If an error occurs accessing the Store.
     */
    public int countChilds(String path, char prefix) throws MailException;

    /**
     * This method sets the version of the Store which is currently being used
     * on the device. If no version is specified, the Store defaults to the
     * latest one.
     *
     *@param version The store version.
     *
     */
    public void setVersion(int version);
    
    /**
     * This method gets the version of the Store which is currently being used
     * on the device. 
     */
    public int getVersion();
}

