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
import com.funambol.storage.Serializable;

import com.funambol.util.Log;

import java.io.IOException;
import java.io.DataInputStream;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import java.util.Vector;

/**
 * Implements the Store using J2ME RecordStore. <p> A folder is mapped to a
 * RecordStore with name equal to the full path of the folder, and the messages
 * inside the folder are the records inside that RecordStore.
 */
public class RMSStore implements Store {
    
    // -------------------------------------------------------------- Attributes

    /** Named ObjectStore related to this RMSStore instance*/
    private ObjectStore objs;
    
    /** Folder which this RMSStore is related to*/
    private Folder folder;
    
    /** Message **/
    private Message message;

    /** Store version (defines data layout) */
    private int version;
    
    /**
     * Max number of messages per store: set as default into the interface
     * This can be changed using related accessor methods
     */
    //private int defaultMessageNumber = -1;
    
    // ------------------------------------------------------------ Constructors
    /**
     * Default Constructor
     */
    RMSStore() {
        objs = new ObjectStore();
        folder = null;
        message = null;
        version = LATEST_VERSION;
    }
    
    
    // ---------------------------------------------------------- Public methods
    /**
     * Initialize the message store, creating the main folders.
     *
     * @param reset if true, erase and re-create all the main folders.
     */
    public void init(boolean reset) throws MailException {
        Log.info("init store: " + reset);

        if (reset) {
            removeFolder(INBOX);
            removeFolder(OUTBOX);
            removeFolder(DRAFTS);
            removeFolder(SENT);
        }
        
        createFolder(INBOX);
        createFolder(OUTBOX);
        createFolder(DRAFTS);
        createFolder(SENT);
        
    }
    
    /**
     * This method returns the list of the top level folders in this store.
     *
     * @return an array of folders, or <code>null</code> if the store is
     *         empty.
     */
    public Folder[] list() {
        try {
            return list("//");
        } catch (MailException e) {
            Log.error(this, "Cant list root folders");
            e.printStackTrace(); // Cannot happen
        }
        return null;
    }
    
    
    /**
     * This method returns the list of the folders whose path starts with 'path'
     * and are direct subfolders of it.
     *
     * @param path
     *            the path of the parent folder
     * @return an array of folders, or <code>null</code> if <code>path</code>
     *         has no subfolders.
     *
     * @throws MailException
     *             If the path is not valid
     */
    public Folder[] list(String path) throws MailException {
        path = checkPath(path);
        String[] folders = AbstractRecordStore.listRecordStores();
        Vector ret = new Vector();
        
        for (int i = 0; i < folders.length; i++) {
            if (folders[i].startsWith(path)
            && folders[i].indexOf('/', path.length()) == -1) {
                ret.addElement(new Folder(folders[i], this));
            }
        }
        return toFolderArray(ret);
    }
    
    
    /**
     * This method creates a folder in the record store. The names of the
     * folder must start with a "/" to distinguish them (they are record stores)
     * from other record stores. The method add the leading slash if not
     * present.
     *
     * @param path
     *             the full path of the folder in the Store.
     * @throws MailException
     *             If an error occurs on the store (e.g. no space left)
     */
    public Folder createFolder(String path) throws MailException {
        path = checkPath(path);
        try {
            objs.create(path);
            return new Folder(path, this);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "createFolder(): recordstore exception," +
                    " throwing mailException");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[createFolder] Can't create folder: " + path);
        }
    }
    
    /**
     * This method removes a folder from the record store.
     *
     * @param path
     *             The full pathname in the Store
     * @return <code>true</code> if the folder has been actually deleted,
     *         <code>false</code> if the folder did not exist.
     * @throws MailException
     *             if an error occurs on the store
     */
    public boolean removeFolder(String path) throws MailException {
        path = checkPath(path);
        try {
            // Close the open path, in case was the one to remove
            objs.close();
            AbstractRecordStore.deleteRecordStore(path);
        } catch (RecordStoreNotFoundException rsnfe) {
            Log.error("removeFolder: folder "+path+" not found.");
            rsnfe.printStackTrace();
            return false;
        } catch (RecordStoreException rse) {
            Log.error("removeFolder: recordstore exception on  "+path + "," +
                    " throwing mail Exception");
            rse.printStackTrace();
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[removeFolder] Can't remove folder: "
                    + path);
        }
        return true;
    }
    
    
    /**
     * This method returns a new reference to the folder whose name is exactly 
     * path. IMPORTANT: note that the return statement of this method generates 
     * a new reference to the selected folder every time this method is invoked.
     *
     * @param path
     *            the path of the folder
     * @return a Folder, or <code>null</code> if not found
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public Folder getFolder(String path) throws MailException {
        
        path = checkPath(path);
        try {
            objs.open(path);
        } catch (Exception e) {
            e.printStackTrace();
            Log.error(this,"unable to open path " + path +
                    "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[getFolder] Can't open folder: " + path);
        }
        return new Folder(path, this);
    }
    
    
    /**
     * This method returns the list of the folders whose path matches
     * <code>subst</code>.
     *
     * @param subst
     *            the part of name to search for
     * @return A list of Folder objects whose path matches the provided
     *         substring, or <code>null</code> if there are no matches
     */
    public Folder[] findFolders(String subst) {
        String[] folders = AbstractRecordStore.listRecordStores();
        Vector ret = new Vector();
        
        for (int i = 0; i < folders.length; i++) {
            if (folders[i].startsWith("/") && folders[i].indexOf(subst) != -1)
                ret.addElement(new Folder(folders[i], this));
        }
        
        return toFolderArray(ret);
    }
    
    
    /**
     * This method returns the headers of all the messages in the folder
     * <code>path</code>. IMPORTANT: a folder MUST be set as parent of 
     * this message before calling this method, otherwise the headers of this 
     * message will be returned as an empty hastable
     *
     * @param path
     *            the path of the folder
     * @return An array of <code>Message</code> with all the headers
     *         set but without content
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public Message[] getMsgHeaders(String path) throws MailException {
        try {
            objs.open(path);
            
            int count = objs.size();

            Message[] ret = new Message[count];
            if(count ==0) {
                return ret;
            }

            int firstIdx = objs.getFirstIndex();
            Message msg = new Message();
            Serializable s = objs.retrieve(firstIdx, msg);
            msg.setRecordId(firstIdx);
            // Load the message headers.
            ret[0] = (Message)s;
            for (int i = 1; i<count; i++) {
                int nextIdx = objs.getNextIndex();
                msg = new Message();

                s = objs.retrieve(nextIdx, msg);
                msg.setRecordId(nextIdx);

                ret[i] = (Message)s;
            }
            return ret;
        } catch (RecordStoreNotFoundException rsnfe) {
            rsnfe.printStackTrace();
            Log.error("getMsgHeaders: folder "+path+" not found, " +
                    "throwing MailException");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[getMsgHeaders] Can't open folder: " + path);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error("getMsgHeaders(): folder "+path+" not found, " +
                    "throwing mail exception");
            
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[getMsgHeaders] Error accessing folder: " + path);
        } catch (IOException ioe) {
            ioe.printStackTrace();
            Log.error(this, "[getMsgHeaders] Error getting message headers, " +
                    "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[getMsgHeaders] Error getting message headers"
                    + ioe);
        }
    }
    
    
    /**
     * This method returns the messageIDs of all the messages in the folder
     * <code>path</code>.
     *
     * @param path
     *            the complete path of the folder
     * @return an array of String, or <code>null</code> if the folder is
     *         empty.
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public String[] getMessageIDs(String path) throws MailException {
        try {
            objs.open(path);
            int count = objs.size();

            String[] ret = new String[count];
            if(count ==0) {
                return ret;
            }

            int firstIdx = objs.getFirstIndex();

            ret[0] = ""+firstIdx;
            for (int i = 1; i<count; i++) {
                int nextIdx = objs.getNextIndex();
                ret[i] = ""+nextIdx;
            }
            return ret;
        } catch (Exception e) {
            Log.error(this, "getMessageIDs() can't open folder, " +
                    "throwing mailException");
            e.printStackTrace();
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[getMessageIDs] Can't open folder: " + path);
        }
    }
    
    
    /**
     * This method reads a message from the Store, using the record id.
     *
     * IMPORTANT: a folder MUST be set as parent of 
     * this message before calling this method, otherwise the headers of this 
     * message will be returned as an empty hastable
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param recordId
     *            A string representing the record ID of this message (must be a
     *            number)
     *
     * @return The <code>Message</code> corresponding to the passed path and
     *         message ID from this <code>Store</code>
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public Message readMessage(String path, String recordId)
    throws MailException {
        try {
            Message m = new Message();
            objs.open(path);
            int rid = Integer.parseInt(recordId);
            objs.retrieve(rid, m);
            m.setRecordId(rid);
            return m;
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readMessage] Can't read message from the store: " +
                               path + " at record id " + recordId;
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * Return the first message in the store.
     *
     * @return the first Message if it exists, null otherwise
     * @throws MailException if the store cannot be accessed
     */
    public Message readFirstMessage(String path) throws MailException {
        return readMessage(path, true);
    }

    /**
     * Return the next message in the store.
     *
     * @return the next Message if it exists, null otherwise
     * @throws MailException if the store cannot be accessed
     */
    public Message readNextMessage(String path) throws MailException {
        return readMessage(path, false);
    }


    /**
     * This method reads a record from the Store, using its record id. The
     * message must be in a folder for this method to succeed. If the record is
     * found the method returns a DataInputStream to read its bytes.
     *
     * @param msg
     *           The message whose record must be re-read
     * @throws MailException
     *           If an error occurs accessing the Store
     */
    public DataInputStream readMessage(Message msg) throws MailException {

        try {
            int rid = msg.getRecordId();
            String path = msg.getParent().getFullName();
            objs.open(path);
            return objs.retrieveBytes(rid);
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readMessage] Can't read record bytestream " +
                              "throwing mail exception";
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

    /**
     * This method saves (NOT ADD) a message in the Store. This is basically and
     * update of an existing message that gets overwritten.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param msg
     *            The <code>Message</code> to save.
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public void saveMessage(String path, Message msg) throws MailException {
        try {
            objs.open(path);
            int recordId = msg.getRecordId();
            objs.store(recordId, msg);
        } catch (RecordStoreNotFoundException rse) {
            rse.printStackTrace();
            Log.error(this, "saveMessage Can't open folder: " + path
                    + "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Can't open folder: " + path);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "saveMessage Error accessing folder:: " + path
                    + "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Error accessing folder: " + path);
        } catch (IOException ioe) {
            Log.error(this, "saveMessage() IOException, throwing mail exception");
            ioe.printStackTrace();
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Error saving message " + msg.getRecordId());
        }
    }

    /**
     * This method add a message to the Store. The newly created message is
     * given its record id.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param msg
     *            The <code>Message</code> to save.
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public void addMessage(String path, Message msg) throws MailException {
         try {
            objs.open(path);
            int index = objs.store(msg);
            msg.setRecordId(index);
        } catch (RecordStoreNotFoundException rse) {
            rse.printStackTrace();
            Log.error(this, "saveMessage Can't open folder: " + path
                    + "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Can't open folder: " + path);
        } catch (RecordStoreException rse) {
            rse.printStackTrace();
            Log.error(this, "saveMessage Error accessing folder:: " + path
                    + "throwing mail exception");
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Error accessing folder: " + path);
        } catch (IOException ioe) {
            Log.error(this, "saveMessage() IOException, throwing mail exception");
            ioe.printStackTrace();
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[saveMessage] Error saving message " + msg.getRecordId());
        }
    }
    
    /**
     * This method removes a Message from the Store, using message ID as index.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param messageId
     *            A string representing the unique message ID
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public void removeMessage(String path, String recordId)
    throws MailException {
        
        try {
            int rid = Integer.parseInt(recordId);
            objs.open(path);
            objs.remove(rid);
        } catch (RecordStoreException rse) {
            throw new MailException(
                    MailException.MESSAGE_DELETE_ERROR,
                    "[removeMessage] Can't remove message in " + path + " "  + rse.toString());
        }
    }
    
    
    /**
     * This method returns the message count in this Store.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public int countMessages(String path) throws MailException {
        try {
            objs.open(path);
            int size = objs.size();
            return size;
        } catch (Exception e) {
            e.printStackTrace();
            Log.error(this, "countMessage Error opening path: " + path
                    + "throwing mail exception");
            
            throw new MailException(
                    MailException.STORAGE_ERROR,
                    "[countMessages] Can't open folder: " + path);
        }
        
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
        this.version = version;
    }


    // --------------------------------------------------------- Private methods
    /**
     * Remove the trailing blanks and '/' from a given path. Add the leading '/'
     * if needed.
     * @param path is the given path
     * @return trimmed string related to the given path
     */
    private String checkPath(String path) {
        String ret = path.trim();
        
        if (!ret.startsWith("/")) {
            ret = "/" + ret;
        }
        
        //the normalized path has a "/" at beginning and no "/" at the end
        if (ret.endsWith("/")) {
            return ret.substring(0, ret.length() - 1);
        }
        return ret;
    }
    
    
    /**
     * Converts a Vector of folders into an array
     * @param v is the vector to be converted
     * @return Folder[] representation of vector v
     */
    private Folder[] toFolderArray(Vector v) {
        int size = v.size();
        
        if (size == 0) {
            return null;
        }
        
        Folder[] ret = new Folder[size];
        
        v.copyInto(ret);
        
        return ret;
    }

    /**
     * Utility method to read a message in the RecordStore order. The method can
     * read the first or the next Message.
     *
     * @param path is the store path
     * @param first indicates if the first record must be read. If not the next
     * one is returned
     * @return the read message if available, null otherwise
     * @throws MailException if the path is not accessibile, or the message
     * cannot be read
     */
    private Message readMessage(String path, boolean first) throws MailException {

        int rid = -1;
        try {

            if (first) {
                rid = objs.getFirstIndex();
            } else {
                rid = objs.getNextIndex();
            }
            objs.open(path);
            if (rid != -1) {
                Message m = new Message();
                objs.retrieve(rid, m);
                m.setRecordId(rid);
                return m;
            } else {
                return null;
            }
        } catch (Exception ex) {
            ex.printStackTrace();
            String errorMsg = "[readMessage] Can't read message from the store: " +
                               path + " at record id " + rid;
            Log.error(errorMsg) ;
            throw new MailException(MailException.STORAGE_ERROR, errorMsg);
        }
    }

}
