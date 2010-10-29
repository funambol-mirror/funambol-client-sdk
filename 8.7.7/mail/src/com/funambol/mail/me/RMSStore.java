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

import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.InvalidRecordIDException;

import java.io.IOException;
import java.io.DataInputStream;
import java.util.Vector;
import java.util.Date;
import java.util.NoSuchElementException;

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

    private static final String TAG_LOG = "RMSStore";
    
    /** Store version (defines data layout). */
    private int currentVersion;

    /** Max number of messages that can be stored */
    public static final int DEFAULT_MAX_MESSAGE_NUMBER = 100;

    /**
     * Default constructor
     */
    RMSStore() {
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
        Log.trace(TAG_LOG, "list");
        try {
            return list(Folder.ROOT_FOLDER_PATH);
        } catch (MailException e) {
            Log.error(TAG_LOG, "Cannot list root folders.");
            e.printStackTrace();
        }
        return null;
    }
    
    /**
     * @see com.funambol.mail.Store#list(java.lang.String) 
     */
    public Folder[] list(String path) throws MailException {
        Log.trace(TAG_LOG, "list(" + path + ")");
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
                Log.error(TAG_LOG, "Error reading child content: ", e);
            }
        }
        return toFolderArray(ret);
    }

    /**
     * @see com.funambol.mail.Store#addFolder(com.funambol.mail.Folder) 
     */
    public Folder addFolder(Folder folder) throws MailException {
        Log.trace(TAG_LOG, "addFolder(Folder)");
        if(folder == null) {
            Log.error(TAG_LOG, "Cannot add null folder");
            return null;
        }
        String folderPath = checkPath(folder.getFullName());
        String parentPath = null;
        if(folder.getParent() == null) {
            if(!folderPath.equals(Folder.ROOT_FOLDER_PATH)) {
                Log.error(TAG_LOG, "Cannot add folder with invalid parent");
                return null;
            }
        } else {
            parentPath = checkPath(folder.getParent().getFullName());
        }
        if(folderExists(folderPath)) {
            // If the specified folder alreay exists, return the existing one
            return getFolder(folderPath);
        }
        ObjectStore objs = createObjectStore();
        try {
            // Create a new record store for the new folder
            objs.create(folderPath);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(TAG_LOG, "RecordStoreException: ", rse);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addFolder] Can't add folder: " + folderPath);
        } finally {
            closeObjectStore(objs);
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
        Log.trace(TAG_LOG, "removeFolder(Folder, " + recursive + ")");
        if(folder == null) {
            Log.debug(TAG_LOG, "Cannot remove null folder");
            return false;
        }
        String folderPath = checkPath(folder.getFullName());
        String parentPath = null;
        if(!folderExists(folderPath)) {
            Log.debug(TAG_LOG, "Cannot remove non existing folder: " +
                    folderPath);
            return false;
        }
        if(folder.getParent() == null) {
            if(!folderPath.equals(Folder.ROOT_FOLDER_PATH)) {
                Log.debug(TAG_LOG, "Cannot remove folder with invalid path");
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
                Log.error(TAG_LOG, "Error while removing folder" +
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
            // Delete the Folder RecordStore
            ObjectStore objs = createObjectStore(folderPath);
            objs.remove();
        } catch (RecordStoreNotFoundException rsnfe) {
            Log.debug(TAG_LOG, "Folder " + folderPath + " not found.");
            return false;
        } catch (RecordStoreException rse) {
            Log.error(TAG_LOG, "RecordStoreException: ", rse);
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
        Log.trace(TAG_LOG, "removeFolder(" + folderPath + ")");
        removeFolder(getFolder(folderPath), true);
    }

    /**
     * @see com.funambol.mail.Store#getFolder(java.lang.String)
     */
    public Folder getFolder(String path) throws MailException {
        Log.trace(TAG_LOG, "getFolder(" + path + ")");
        return getFolder(path, false);
    }

    /**
     * @see com.funambol.mail.Store#getFolder(java.lang.String, boolean) 
     */
    public Folder getFolder(String path, boolean lightFolder) throws MailException {
        Log.trace(TAG_LOG, "getFolder(" + path + ", " + lightFolder + ")");
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
                Log.debug(TAG_LOG, "Folder not found: " + path);
                throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Folder not found: " + path);
            }
        } else {
            Log.debug(TAG_LOG, "[getFolder] Invalid Folder path format" +
                    ", parent path not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Folder not found: " + path);
        }
        // Avoid to return a null folder
        if(result == null) {
            Log.debug(TAG_LOG, "Can't open folder, folder not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "[getFolder] Can't open folder: " + path);
        }
        return result;
    }

    /**
     * @see com.funambol.mail.Store#findFolders(java.lang.String) 
     */
    public Folder[] findFolders(String path) {
        Log.trace(TAG_LOG, "findFolders(" + path + ")");
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
        Log.trace(TAG_LOG, "retrieveSubfolderID(" + subfolderPath + ", " + parentPath + ")");
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
                Log.error(TAG_LOG, "Error while retrieving child content.");
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
        Log.trace(TAG_LOG, "getChildIDs(" + path + ")");
        ObjectStore objs = createObjectStore(path);
        try {
            objs.open();
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
            Log.debug(TAG_LOG, "Record store not found: " + path);
            // Return an emty array
            return new String[0];
        } catch (Exception e) {
            Log.error(TAG_LOG, "Can't open folder: " + path, e);
            throw new MailException( MailException.STORAGE_ERROR, "Can't open folder: " + path);
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#readChild(java.lang.String, java.lang.String)
     */
    public RmsRecordItem readChild(String path, String childId) throws MailException {
        Log.trace(TAG_LOG, "readChild(" + path + ", " + childId + ")");
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
                Log.error(TAG_LOG, msg);
                throw new MailException(MailException.STORAGE_ERROR, msg);
            }
        } catch (Exception ex) {
            String errorMsg = "[readChild] Exception: " + ex.toString() +
                    " Can't read item from the store: " + path +
                    " at record id: " + childId;
            Log.error(TAG_LOG, errorMsg, ex);
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * @see com.funambol.mail.Store#readChildBytes(java.lang.String, java.lang.String) 
     */
    public DataInputStream readChildBytes(String path, String childId) throws MailException { 
        Log.trace(TAG_LOG, "readChildBytes(" + path + ", " + childId + ")");
        try {
            DataInputStream din = readRawChildBytes(path, childId);
            // read the child prefix, it's not used here
            din.readChar();
            return din;
        } catch (Exception ex) {
            String errorMsg = "[readChildBytes] Exception: " + ex.toString() +
                    " Can't read record prefix for id: "
                    + childId + " in folder " + path;
            Log.error(TAG_LOG, errorMsg, ex) ;
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
        Log.trace(TAG_LOG, "readRawChildBytes(" + path + ", " + childId + ")");
        ObjectStore objs = createObjectStore(path);
        try {
            int rid = Integer.parseInt(childId);
            objs.open();
            return objs.retrieveBytes(rid);
        } catch (RecordStoreNotFoundException rse) {
            Log.debug(TAG_LOG, "Record store not found: " + path);
            throw new MailException(MailException.FOLDER_NOT_FOUND_ERROR,
                    "Record store not found: " + path);
        }  catch (InvalidRecordIDException ex) {
            String errorMsg = "Can't read record content for id: "
                    + childId + " in folder " + path +
                    ". The child could have been previously deleted";
            Log.error(TAG_LOG, errorMsg, ex) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        } catch (Exception ex) {
            String errorMsg = "[readRawChildBytes] Exception: " + ex.toString() + 
                    ". Can't read record content for id: " + childId + " in folder " + path;
            Log.error(TAG_LOG, errorMsg, ex);
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#getChildrenEnumeration(java.lang.String)
     */
    public ChildrenEnumeration getChildren(String path) throws MailException {
        try {
            return new RMSChildrenEnumeration(path);
        } catch (RecordStoreException ex) {
            String errorMsg = "[RMSStore.getChildren]Exception: " + ex.toString();
            Log.error(TAG_LOG, errorMsg, ex);
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * @see com.funambol.mail.Store#addChild(java.lang.String, com.funambol.storage.RmsRecordItem)
     */
    public int addChild(String path, RmsRecordItem child) throws MailException {
        Log.trace(TAG_LOG, "addChild(" + path + ", RmsRecordItem)");
        ObjectStore objs = createObjectStore(path);
        try {
            objs.open();
            int index = objs.store(child);
            child.setRecordId(index);
            return index;
        } catch (RecordStoreNotFoundException rse) {
            Log.error(TAG_LOG, "Can't open folder: " + path, rse);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Can't open folder: " + path);
        } catch (RecordStoreException rse) {
            Log.error(TAG_LOG, "Error accessing folder:: " + path, rse);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Error accessing folder: " + path);
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "IOException: ", ioe);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[addChild] Error saving child " + child.getRecordId());
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#updateChild(java.lang.String, com.funambol.storage.RmsRecordItem)
     */
    public void updateChild(String path, RmsRecordItem child) throws MailException {
        Log.trace(TAG_LOG, "updateChild(" + path + ", RmsRecordItem)");
        ObjectStore objs = createObjectStore(path);
        try {
            objs.open();
            int recordId = child.getRecordId();
            objs.store(recordId, child);
        } catch (RecordStoreNotFoundException rse) {
            Log.error(TAG_LOG, "Can't open folder: " + path, rse);
            throw new MailException(MailException.STORAGE_ERROR,
                    "Can't open folder: " + path);
        } catch (InvalidRecordIDException rse) {
            Log.error(TAG_LOG, "Cannot find child: " + path +
                    ", id: " + child.getRecordId() + ". It could have been deleted.", rse);
        } catch (RecordStoreException rse) {
            Log.error(TAG_LOG, "Error accessing folder: " + path, rse);
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[updateChild] Error accessing folder: " + path);
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "[updateChild] IOException: ", ioe);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[updateChild] Error updating child " + child.getRecordId());
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#removeChild(java.lang.String, java.lang.String)
     */
    public void removeChild(String path, String childId) throws MailException {
        Log.trace(TAG_LOG, "removeChild(" + path + ", " + childId + ")");
        ObjectStore objs = createObjectStore(path);
        try {
            int rid = Integer.parseInt(childId);
            objs.open();
            objs.remove(rid);
        } catch (NumberFormatException e) {
            Log.error(TAG_LOG, "Invalid child id: " + childId, e);
        } catch (InvalidRecordIDException e) {
            Log.debug(TAG_LOG, "Can't remove child: " + childId +
                    " in folder: " + path);
        } catch (RecordStoreException e) {
            throw new MailException(MailException.ITEM_DELETE_ERROR,
                    "[removeChild] Can't remove child in folder: " + path +
                    ". RecordStoreException: "  + e.toString());
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#countChilds(java.lang.String)
     */
    public int countChilds(String path) throws MailException {
        Log.trace(TAG_LOG, "countChilds(" + path + ")");
        ObjectStore objs = createObjectStore(path);
        try {
            objs.open();
            return objs.size();
        } catch (Exception e) {
            Log.error(TAG_LOG, "Error opening path: " + path, e);
            throw new MailException(MailException.STORAGE_ERROR,
                    "[countChilds] Can't open folder: " + path);
        } finally {
            closeObjectStore(objs);
        }
    }

    /**
     * @see com.funambol.mail.Store#countChilds(java.lang.String, char) 
     */
    public int countChilds(String path, char prefix) throws MailException {
        Log.trace(TAG_LOG, "countChilds(" + path + ", " + prefix + ")");
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
                Log.error(TAG_LOG, "Error while retrieving child content.");
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
            Log.error(TAG_LOG, "The folder path is not complete: " + path);
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

    private void closeObjectStore(ObjectStore os) throws MailException {
        try {
            os.close();
        } catch(RecordStoreException ex) {
            String errorMsg = "Cannot close store " + os.getName();
            Log.error(TAG_LOG, errorMsg, ex);
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    protected ObjectStore createObjectStore() {
        return new ObjectStore();
    }

    protected ObjectStore createObjectStore(String name) {
        return new ObjectStore(name);
    }


    /**
     * Implements the <code>ChildrenEnumeration</code> interface. The returned
     * elements are <code>RmsRecordItem</code> objects.
     */
    private class RMSChildrenEnumeration implements ChildrenEnumeration {

        private ObjectStore store = null;

        private int size         = 0;
        private int currentIndex = -1;

        private boolean closed;
        
        /**
         * Default constructor
         */
        public RMSChildrenEnumeration(String path) throws RecordStoreException {
            store = new ObjectStore(path);
            store.open();
            size   = store.size();
            closed = false;
        }

        /**
         * Check wheter there are more childs available.
         * @return True if there are more elements.
         */
        public boolean hasMoreElements() {

            boolean moreElements = ((currentIndex+1) < size) ? true : false;
            if(!moreElements && !closed) {
                try {
                    close();
                } catch(Exception ex) {
                    Log.error(TAG_LOG, "Error in hasMoreElements", ex);
                }
            }
            return (moreElements && !closed) ? true : false;
        }

        /**
         * Get the next child.
         * 
         * @return The child in <code>RmsRecordItem</code> format.
         * @throws NoSuchElementException
         */
        public Object nextElement() throws NoSuchElementException {

            if(!hasMoreElements() || closed) {
                // No more elements available
                throw new NoSuchElementException();
            }

            // Increments the current element index
            currentIndex++;

            try {
                // Get the current record id
                int recordId = currentIndex == 0 ?
                    store.getFirstIndex() : store.getNextIndex();

                RmsRecordItem item = readChild(store.getName(), Integer.toString(recordId));

                // Close the record store handler if there are no more elements
                if(!hasMoreElements()) {
                    close();
                }
                return item;
            } catch(Exception ex) {
                Log.error(TAG_LOG, "Error getting nextElement ", ex);
            }
            // No such element to return
            throw new NoSuchElementException();
        }

        /**
         * Close the Record Store handler
         */
        public void close() throws RecordStoreException {
            if(!closed) {
                store.close();
                closed = true;
            }
        }
    }
}
