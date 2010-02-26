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

import com.funambol.storage.AbstractRecordStore;
import com.funambol.storage.ObjectStore;
import com.funambol.storage.RmsRecordItem;

import com.funambol.util.Log;

import java.io.IOException;
import java.io.DataInputStream;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import java.util.Vector;
import java.util.Date;
import javax.microedition.rms.InvalidRecordIDException;

/**
 * Implements the <code>Store</code> interface using J2ME RecordStore. Each
 * <code>Folder</code> is mapped to a RecordStore with name equal to the full
 * path of the <code>Folder</code>. The records of each RecordStore can contain
 * any kind of data, including:
 * <li><code>AccountFolder</code> items;</li>
 * <li><code>Folder</code> items;</li>
 * <li><code>Message</code> items.</li>
 */
public class RMSStore implements Store {
    
    /** Named ObjectStore related to this RMSStore instance. */
    private ObjectStore objs;
    
    /** Store version (defines data layout). */
    private int currentVersion;

    /** Max number of messages that can be stored */
    public static final int DEFAULT_MAX_MESSAGE_NUMBER = 100;

    /**
     * Default constructor
     */
    RMSStore() {
        objs = new ObjectStore();
        currentVersion = LATEST_VERSION;
    }
    
    /**
     * @see com.funambol.mail.Store#init(boolean) 
     */
    public void init(boolean reset) throws MailException {
        if (reset) {
            // Reset the store. The following instruction will remove all the
            // folders hierarchy.
            removeFolder(getFolder(Folder.ROOT_FOLDER_PATH, true), true);
        }
        addFolder(new Folder(Folder.ROOT_FOLDER_PATH, null, new Date(), this));
    }
    
    /**
     * @see com.funambol.mail.Store#list() 
     */
    public Folder[] list() {
        Log.trace("[RMSStore] list()");
        try {
            return list(Folder.ROOT_FOLDER_PATH);
        } catch (MailException e) {
            Log.error(this, "[list] Cannot list root folders.");
            e.printStackTrace();
        }
        return null;
    }
    
    /**
     * @see com.funambol.mail.Store#list(java.lang.String) 
     */
    public Folder[] list(String path) throws MailException {
        Log.trace("[RMSStore] list(" + path + ")");
        String[] childIDs = getChildIDs(path);
        Vector ret = new Vector();

        for (int i = 0; i < childIDs.length; i++) {
            DataInputStream din = readRawChildBytes(path, childIDs[i]);
            Folder child = null;
            try {
                char prefix = din.readChar();
                if(Folder.isSupportedStream(prefix)) {
                    child = new Folder(this);
                } else if(AccountFolder.isSupportedStream(prefix)) {
                    child = new AccountFolder(this);
                }
                if(child != null) {
                    child.deserialize(din);
                    child.setRecordId(Integer.parseInt(childIDs[i]));
                    ret.addElement(child);
                }
                din.close();
            } catch(IOException e) {
                Log.debug(this, "[list] Error reading child content: " + e.toString());
            }
        }
        return toFolderArray(ret);
    }

    /**
     * @see com.funambol.mail.Store#addFolder(com.funambol.mail.Folder) 
     */
    public Folder addFolder(Folder folder) throws MailException {
        Log.trace("[RMSStore] addFolder(Folder)");
        if(folder == null) {
            Log.error(this, "[addFolder] Invalid null folder");
            return null;
        }
        String folderPath = checkPath(folder.getFullName());
        String parentPath = null;
        if(folder.getParent() == null) {
            if(!folderPath.equals(Folder.ROOT_FOLDER_PATH)) {
                Log.error(this, "[addFolder] Invalid folder Parent");
                return null;
            }
        } else {
            parentPath = checkPath(folder.getParent().getFullName());
        }
        if(folderExists(folderPath)) {
            // If the specified folder alreay exists, return the existing one
            return getFolder(folderPath);
        }
        try {
            // Create a new record store for the new folder
            objs.create(folderPath);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "[addFolder] RecordStoreException: " +
                    rse.toString());
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addFolder] Can't add folder: " + folderPath);
        }
        if(parentPath != null) {
            // Add the related subfolder to the parent
            addChild(parentPath, folder);
        }
        // Set the Store reference
        folder.setStore(this);
        return folder;
    }

    /**
     * Removes a <code>Folder</code> RecordStore, its parent reference and the
     * subfolders.
     * 
     * @see com.funambol.mail.Store#removeFolder(com.funambol.mail.Folder, boolean) 
     */
    public boolean removeFolder(Folder folder, boolean recursive) throws MailException {
        Log.trace("[RMSStore] removeFolder(Folder, " + recursive + ")");
        if(folder == null) {
            Log.debug(this, "[removeFolder] Invalid null folder");
            return false;
        }
        String folderPath = checkPath(folder.getFullName());
        String parentPath = null;
        if(!folderExists(folderPath)) {
            Log.debug(this, "[removeFolder] The folder doesn't exist: " +
                    folderPath);
            return false;
        }
        if(folder.getParent() == null) {
            if(!folderPath.equals(Folder.ROOT_FOLDER_PATH)) {
                Log.debug(this, "[removeFolder] Invalid folder Path");
                return false;
            }
        } else {
            parentPath = checkPath(folder.getParent().getFullName());
        }
        //
        // Remove subfolders if recursive
        //
        Folder[] subfolders = list(folderPath);
        if(subfolders.length > 0) {
            if(!recursive) {
                Log.error(this, "[removeFolder] Error while removing folder" +
                        ", the specified folder contains subfolders.");
                return false;
            }
            for(int i=0; i<subfolders.length; i++) {
                if(!removeFolder(subfolders[i], true)) {
                    return false;
                }
            }
        }

        //
        // Remove the folder record store
        //
        try {
            // Close the open path, in case was the one to remove
            objs.close();
            // Delete the Folder RecordStore
            AbstractRecordStore.deleteRecordStore(folderPath);
        } catch (RecordStoreNotFoundException rsnfe) {
            Log.debug(this, "[removeFolder] Folder " + folderPath + " not found.");
            return false;
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.debug(this, "[removeFolder] RecordStoreException: " +
                    rse.toString());
            throw new MailException(MailException.ITEM_DELETE_ERROR,
                    "[removeFolder] Can't remove folder: " + folderPath);
        }

        //
        // Remove the related subfolder from the parent childs
        //
        if(parentPath != null) {
            String subfolderId = retrieveSubfolderID(folderPath, parentPath);
            if(subfolderId != null) {
                removeChild(parentPath, subfolderId);
            }
        }
        return true;
    }
    
    /**
    * @see com.funambol.mail.Store#removeFolder(String)
    */
    public void removeFolder(String folderPath) throws MailException {
        Log.trace("[RMSStore] removeFolder(" + folderPath + ")");
        removeFolder(getFolder(folderPath), true);
    }

    /**
     * @see com.funambol.mail.Store#getFolder(java.lang.String)
     */
    public Folder getFolder(String path) throws MailException {
        Log.trace("[RMSStore] getFolder(" + path + ")");
        return getFolder(path, false);
    }

    /**
     * @see com.funambol.mail.Store#getFolder(java.lang.String, boolean) 
     */
    public Folder getFolder(String path, boolean lightFolder) throws MailException {
        Log.trace("[RMSStore] getFolder(" + path + ", " + lightFolder + ")");
        if(path.equals(Folder.ROOT_FOLDER_PATH)) {
            // This is the root folder
            return new Folder(Folder.ROOT_FOLDER_PATH, null, new Date(), this);
        }
        String folderPath = checkPath(path);
        String parentPath = getParentPath(folderPath);
        if(lightFolder) {
            return new Folder(folderPath, null, new Date(), this);
        }
        Folder result = null;
        if(parentPath != null) {
            Folder[] subfolders = list(parentPath);
            for(int i=0; i<subfolders.length; i++) {
                if(subfolders[i].getFullName().equals(path)) {
                    result = subfolders[i];
                }
            }
            if(result == null) {
                Log.debug(this, "[getFolder] Folder not found: " + path);
                throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Folder not found: " + path);
            }
        } else {
            Log.debug(this, "[getFolder] Invalid Folder path format" +
                    ", parent path not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Folder not found: " + path);
        }
        // Avoid to return a null folder
        if(result == null) {
            Log.debug(this, "[getFolder] Can't open folder, folder not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Can't open folder: " + path);
        }
        return result;
    }

    /**
     * @see com.funambol.mail.Store#findFolders(java.lang.String) 
     */
    public Folder[] findFolders(String path) {
        Log.trace("[RMSStore] findFolders(" + path + ")");
        String[] folders = AbstractRecordStore.listRecordStores();
        Vector ret = new Vector();
        for (int i = 0; i < folders.length; i++) {
            if (folders[i].startsWith("/") && folders[i].indexOf(path) != -1)
                ret.addElement(getFolder(folders[i]));
        }
        return toFolderArray(ret);
    }

    /**
     * @see com.funambol.mail.Store#retrieveSubfolderID(java.lang.String, java.lang.String) 
     */
    public String retrieveSubfolderID(String subfolderPath, String parentPath) {
        Log.trace("[RMSStore] retrieveSubfolderID(" + subfolderPath + ", " + parentPath + ")");
        String[] parentChilds = getChildIDs(parentPath);
        for(int i=0; i<parentChilds.length; i++) {
            DataInputStream din = readRawChildBytes(parentPath, parentChilds[i]);
            Folder child = null;
            try {
                char prefix = din.readChar();
                if(Folder.isSupportedStream(prefix)) {
                    child = new Folder(this);
                } else if(AccountFolder.isSupportedStream(prefix)) {
                    child = new AccountFolder(this);
                } else {
                    // This is not a Folder item
                    continue;
                }
                child.deserialize(din);
                din.close();
            } catch(IOException e) {
                Log.error(this, "[retrieveSubfolderID] Error while retrieving" +
                        " child content.");
                continue;
            }
            if(child.getFullName().equals(subfolderPath)) {
                // The subfolder has been found
                return parentChilds[i];
            }
        }
        return null;
    }

    /**
     * @see com.funambol.mail.Store#getChildIDs(java.lang.String)
     */
    public String[] getChildIDs(String path) throws MailException {
        Log.trace("[RMSStore] getChildIDs(" + path + ")");
        try {
            objs.open(path);
            int count = objs.size();
            count = count < 0 ? 0 : count;
            String[] ret = new String[count];
            if(count == 0) {
                return ret;
            }
            ret[0] = Integer.toString(objs.getFirstIndex());
            for (int i = 1; i<count; i++) {
                ret[i] = Integer.toString(objs.getNextIndex());
            }
            return ret;
        } catch (RecordStoreNotFoundException rse) {
            Log.debug(this, "[getChildIDs] Record store not found: " + path);
            // Return an emty array
            return new String[0];
        } catch (Exception e) {
            e.printStackTrace();
            Log.debug(this, "[getChildIDs] Can't open folder: " + path + ": " + e);
            throw new MailException( MailException.STORAGE_ERROR,
                    "[getChildIDs] Can't open folder: " + path);
        }
    }

    /**
     * @see com.funambol.mail.Store#readChild(java.lang.String, java.lang.String)
     */
    public RmsRecordItem readChild(String path, String childId) throws MailException {
        Log.trace("[RMSStore] readChild(" + path + ", " + childId + ")");
        try {
            DataInputStream din = readRawChildBytes(path, childId);
            RmsRecordItem item = null;
            char prefix = din.readChar();
            if(Message.isSupportedStream(prefix)) {
                item = new Message();
            } else if(Folder.isSupportedStream(prefix)) {
                item = new Folder(this);
            } else if(AccountFolder.isSupportedStream(prefix)) {
                item = new AccountFolder(this);
            }
            int rid = Integer.parseInt(childId);
            if(item != null) {
                item.deserialize(din);
                item.setRecordId(rid);
                return item;
            } else {
                // The record content is not supported
                String msg = "[readChild] Record content not supported";
                Log.error(msg);
                throw new MailException(MailException.STORAGE_ERROR, msg);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readChild] " + ex.toString() + 
                    " Can't read item from the store: " + path +
                    " at record id: " + childId;
            Log.error(errorMsg);
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * @see com.funambol.mail.Store#readChildBytes(java.lang.String, java.lang.String) 
     */
    public DataInputStream readChildBytes(String path, String childId) throws MailException { 
        Log.trace("[RMSStore] readChildBytes(" + path + ", " + childId + ")");
        try {
            DataInputStream din = readRawChildBytes(path, childId);
            // read the child prefix, it's not used here
            din.readChar();
            return din;
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readChildBytes] Can't read record prefix for id: "
                    + childId + " in folder " + path;
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * Retrieve the raw child content, including the record prefix.
     * 
     * @param path
     * @param childId
     * @return
     * @throws com.funambol.mail.MailException
     */
    private DataInputStream readRawChildBytes(String path, String childId) throws MailException {
        Log.trace("[RMSStore] readRawChildBytes(" + path + ", " + childId + ")");
        try {
            int rid = Integer.parseInt(childId);
            objs.open(path);
            return objs.retrieveBytes(rid);
        } catch (RecordStoreNotFoundException rse) {
            Log.debug(this, "[readRawChildBytes] Record store not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "Record store not found: " + path);
        }  catch (InvalidRecordIDException ex) {
            String errorMsg = "[readRawChildBytes] Can't read record content for id: "
                    + childId + " in folder " + path +
                    ". The child could have been previously deleted";
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readRawChildBytes] Can't read record content for id: "
                    + childId + " in folder " + path;
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * @see com.funambol.mail.Store#readFirstChild(java.lang.String)
     */
    public RmsRecordItem readFirstChild(String path) throws MailException {
        Log.trace("[RMSStore] readFirstChild(" + path + ")");
        return readChild(path, true);
    }

    /**
     * @see com.funambol.mail.Store#readNextChild(java.lang.String)
     */
    public RmsRecordItem readNextChild(String path) throws MailException {
        Log.trace("[RMSStore] readNextChild(" + path + ")");
        return readChild(path, false);
    }

    /**
     * @see com.funambol.mail.Store#addChild(java.lang.String, com.funambol.storage.RmsRecordItem)
     */
    public int addChild(String path, RmsRecordItem child) throws MailException {
        Log.trace("[RMSStore] addChild(" + path + ", RmsRecordItem)");
        try {
            objs.open(path);
            int index = objs.store(child);
            child.setRecordId(index);
            return index;
        } catch (RecordStoreNotFoundException rse) {
            rse.printStackTrace();
            Log.error(this, "[addChild] Can't open folder: " + path);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Can't open folder: " + path);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "[addChild] Error accessing folder:: " + path);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Error accessing folder: " + path);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            Log.error(this, "[addChild] IOException: " + ioe.toString());
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Error saving child " + child.getRecordId());
        }
    }

    /**
     * @see com.funambol.mail.Store#updateChild(java.lang.String, com.funambol.storage.RmsRecordItem)
     */
    public void updateChild(String path, RmsRecordItem child) throws MailException {
        Log.trace("[RMSStore] updateChild(" + path + ", RmsRecordItem)");
        try {
            objs.open(path);
            int recordId = child.getRecordId();
            objs.store(recordId, child);
        } catch (RecordStoreNotFoundException rse) {
            rse.printStackTrace();
            Log.error(this, "[updateChild] Can't open folder: " + path);
            throw new MailException(MailException.STORAGE_ERROR,
                    "Can't open folder: " + path);
        } catch (InvalidRecordIDException rse) {
            rse.printStackTrace();
            Log.error(this, "[updateChild] Cannot find child: " + path +
                    ", id: " + child.getRecordId() + ". It could have been deleted.");
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "[updateChild] Error accessing folder: " + path);
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[updateChild] Error accessing folder: " + path);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            Log.error(this, "[updateChild] IOException: " + ioe.toString());
            throw new MailException(MailException.STORAGE_ERROR,
                    "[updateChild] Error updating child " + child.getRecordId());
        }
    }

    /**
     * @see com.funambol.mail.Store#removeChild(java.lang.String, java.lang.String)
     */
    public void removeChild(String path, String childId) throws MailException {
        Log.trace("[RMSStore] removeChild(" + path + ", " + childId + ")");
        try {
            int rid = Integer.parseInt(childId);
            objs.open(path);
            objs.remove(rid);
        } catch (NumberFormatException e) {
            Log.error("[removeChild] Invalid child id: " + childId);
        } catch (InvalidRecordIDException e) {
            Log.debug("[removeChild] Can't remove child: " + childId +
                    " in folder: " + path);
        } catch (RecordStoreException e) {
            throw new MailException(MailException.ITEM_DELETE_ERROR,
                    "[removeChild] Can't remove child in folder: " + path +
                    ". RecordStoreException: "  + e.toString());
        }
    }

    /**
     * @see com.funambol.mail.Store#countChilds(java.lang.String)
     */
    public int countChilds(String path) throws MailException {
        Log.trace("[RMSStore] countChilds(" + path + ")");
        try {
            objs.open(path);
            return objs.size();
        } catch (Exception e) {
            e.printStackTrace();
            Log.error(this, "[countChilds] Error opening path: " + path);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[countChilds] Can't open folder: " + path);
        }
    }

    /**
     * @see com.funambol.mail.Store#countChilds(java.lang.String, char) 
     */
    public int countChilds(String path, char prefix) throws MailException {
        Log.trace("[RMSStore] countChilds(" + path + ", " + prefix + ")");
        int count = 0;
        String[] childs = getChildIDs(path);
        for(int i=0; i<childs.length; i++) {
            DataInputStream din = readRawChildBytes(path, childs[i]);
            try {
                char childPrefix = din.readChar();
                if(childPrefix == prefix) {
                    count++;
                } 
                din.close();
            } catch(IOException e) {
                Log.error(this, "[countChilds] Error while retrieving" +
                        " child content.");
                continue;
            }
        }
        return count;
    }

    /**
     * This method sets the version of the Store which is currently being used
     * on the device. If no version is specified, the Store defaults to the
     * latest one.
     *
     *@param version is the store version
     *
     */
    public void setVersion(int version) {
        currentVersion = version;
    }
    
    /**
     * This method gets the version of the Store which is currently being used
     * on the device. 
     *
     */
    public int getVersion(){
        return currentVersion;
    }

    // --------------------------------------------------------- Private methods
    
    /**
     * Remove the trailing blanks and '/' from a given path. Check if it starts
     * with the <code>FOLDER_SEPARATOR</code> char in order to verify the
     * completeness of the path.
     * @param path is the given path
     * @return trimmed string related to the given path
     * @throws MailException If the path is not complete
     */
    private String checkPath(String path) throws MailException {
        String ret = path.trim();
        if(!path.startsWith(String.valueOf(Folder.FOLDER_SEPARATOR))) {
            Log.error(this, "[checkPath] The folder path is not complete: " + path);
            throw new MailException(MailException.FOLDER_ERROR,
                    "[checkPath] The folder path is not complete: " + path);
        }
        if(path.endsWith(String.valueOf(Folder.FOLDER_SEPARATOR)) &&
                path.length() > 1) {
            ret = path.substring(0, path.length()-1);
        }
        return ret;
    }

    /**
     * Get the parent folder path related to the specified folder path
     * @param folderPath
     * @return null if the given folder is root
     */
    private String getParentPath(String folderPath) {
        int folderNameIndex = folderPath.lastIndexOf(Folder.FOLDER_SEPARATOR);
        if(folderNameIndex > 0) {
            return folderPath.substring(0, folderNameIndex);
        } else if (folderNameIndex == 0 && folderPath.length()>1) {
            // The parent is the root folder
            return Folder.ROOT_FOLDER_PATH;
        }
        return null;
    }
    
    /**
     * Converts a <code>Vector</code> of <code>Folder</code> items into an
     * array.
     * 
     * @param v The vector to be converted
     * @return Folder[] representation of <code>Vector</code> v
     */
    private Folder[] toFolderArray(Vector v) {
        int size = v.size();
        Folder[] ret = new Folder[size];
        v.copyInto(ret);
        return ret;
    }

    /**
     * Utility method to read a <code>RmsRecordItem</code> in the RecordStore
     * order. The method can read the first or the next
     * <code>RmsRecordItem</code>.
     *
     * @param path The store path
     * @param first Indicates if the first record must be read. If not the next
     * one is returned
     * @return The read <code>RmsRecordItem</code> if available, null otherwise.
     * @throws MailException if the path is not accessibile.
     */
    private RmsRecordItem readChild(String path, boolean first) throws MailException {
        int rid = -1;
        try {
            objs.open(path);
            if (first) {
                rid = objs.getFirstIndex();
            } else {
                rid = objs.getNextIndex();
            }
            if(rid == -1) {
                // There are no more childs
                return null;
            }
            DataInputStream din = readRawChildBytes(path, Integer.toString(rid));
            RmsRecordItem item = null;
            char prefix = din.readChar();
            if(Message.isSupportedStream(prefix)) {
                item = new Message();
            } else if(Folder.isSupportedStream(prefix)) {
                item = new Folder(this);
            } else if(AccountFolder.isSupportedStream(prefix)) {
                item = new AccountFolder(this);
            }
            if(item != null) {
                item.deserialize(din);
                item.setRecordId(rid);
                return item;
            } else {
                return null;
            }
        } catch (InvalidRecordIDException ex) {
            String errorMsg = "[readChild] Can't read item from the store: " +
                    path + " at record id " + rid +
                    ". The item could have been previously deleted.";
            Log.debug(errorMsg);
            return null;
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readChild] Can't read item from the store: " + 
                    path + " at record id " + rid;
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    // Check whether a folder path already exists
    private boolean folderExists(String path) {
        String[] folders = AbstractRecordStore.listRecordStores();
        if(folders == null) {
            return false;
        }
        for (int i = 0; i < folders.length; i++) {
            if (folders[i].equals(path)) {
                return true;
            }
        }
        return false;
    }
}
