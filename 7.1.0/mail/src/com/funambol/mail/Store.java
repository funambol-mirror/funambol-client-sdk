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

import java.io.DataInputStream;

/**
 * Represents a message store and its access protocol, for storing and
 * retrieving messages on the device
 */
public interface Store {
   
    /** Store available versions */
    public static final int VERSION_101 = 101;
    public static final int VERSION_102 = 102;
    public static final int VERSION_103 = 103;
    
    /** Latest version */
    public static final int LATEST_VERSION = VERSION_103;
    
    

    /* Default Name for the Inbox folders*/
    public static final String INBOX  = "Inbox";
    /* Default Name for the Outbox folders*/
    public static final String OUTBOX = "Outbox";
    /* Default Name for the Draft folders*/
    public static final String DRAFTS = "Drafts";
    /* Default Name for the sent folders*/
    public static final String SENT   = "Sent";
    // Max number of messages that can be stored
    //public static int DEFAULT_MAX_MESSAGE_NUMBER = 100;

    
    
    /**
     * Initialize the message store, creating the main folders.
     *
     * @param reset if true, erase and re-create all the main folders.
     */
    public void init(boolean reset) throws MailException ;

    /**
     * Retrieves a list of folders directly under this Store
     * 
     * @return an array of folders, or <code>null</code> if the store is empty.
     */
    public Folder[] list();

    /**
     * This method returns the list of the folders whose path
     * starts with 'path' and are direct subfolders of it.
     *
     * @param path the path of the parent folder
     * @return an array of folders, or <code>null</code> if
     *          <code>path</code> has no subfolders.
     *
     * @throws StoreException
     *             If the path is not valid
     */
    public Folder[] list(String path) throws MailException;

    /**
     * Creates a Folder in this Store with the provided path
     * 
     * @param path
     *             the full path of the folder in the Store.     
     * @throws StoreException
     *             If an error occurs on the store (e.g. no space left)
     */
    public Folder createFolder(String path) throws MailException;

    /**
     * Removes a folder from the record store. 
     * 
     * @param path
     *             The full pathname in the Store
     * @return <code>true</code> if the folder has been actually deleted,
     *         <code>false</code> if the folder did not exist.
     * @throws StoreException
     *             if an error occurs on the store 
     */
    public boolean removeFolder(String path) throws MailException ;
     
    /**
     * Retrieves a Folder by name
     * 
     * @param path
     *            The path to the Folder in the device's file system
     * @return The Folder object
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    public Folder getFolder(String path) throws MailException;

    /**
     * Retrieves a Folder in this Store that matches the provided substring
     * 
     * @param path
     *            The partial path in the device's file system to the searched
     *            Folder
     * @return A list of Folder objects whose path matches the provided
     *         substring, or <code>null</code> if there are no matches
     */
    public Folder[] findFolders(String path);

    /**
     * This method reads the headers of all the message from a folder.
     * 
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            messages.
     * @return An array of <code>Message</code> with all the headers
     *          set but without content
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    public Message[] getMsgHeaders(String path) throws MailException;

    /**
     * This method returns the messageIDs of all the messages in the
     * folder <code>path</code>.
     *
     * @param path the complete path of the folder
     * @return an array of String, or <code>null</code> if the folder is
     *         empty.
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    public String[] getMessageIDs(String path) throws MailException;
    

    /**
     * This method reads a message from the Store, using the message Id.
     * 
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param msgid
     *            A string representing the unique message ID 
     * @return The <code>Message</code> corresponding to the passed path and
     *         message ID from this <code>Store</code>
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    Message readMessage(String path, String msgid) throws MailException;

    /**
     * Return the first message in the store.
     *
     * @return the first Message if it exists, null otherwise
     * @throws MailException if the store cannot be accessed
     */
    Message readFirstMessage(String path) throws MailException;

    /**
     * Return the next message in the store.
     *
     * @return the next Message if it exists, null otherwise
     * @throws MailException if the store cannot be accessed
     */
    Message readNextMessage(String path) throws MailException;

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
    DataInputStream readMessage(Message msg) throws MailException;

    /**
     * This method reloads a message from the Store. The message has already
     * been loaded and initialized, but this method forces a reload.
     * Which fields get re-written depends on the Message state. In other words
     * this method does not guarantee anything on what parts of the message are
     * kept or modified. The message state determines this behavior.
     * The method is intented for the API only, and as of today it is used for
     * the lazy Method.getContent implementation.
     *
     * @param msg
     *            The Message to be reloaded.
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */

    /**
     * This method saves a new message in the Store
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param msg
     *            The <code>Message</code> to save.
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    void addMessage(String path, Message msg) throws MailException;

    /**
     * This method updates an existing message in the Store
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param msg
     *            The <code>Message</code> to save.
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    void saveMessage(String path, Message msg) throws MailException;

    /**
     * This method removes a Message from the Store, using message ID
     * as index.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     * @param messageID
     *            A string representing the unique message ID 
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    void removeMessage(String path, String messageID) throws MailException;

    /**
     * This method returns the message count in this Store.
     *
     * @param path
     *            The complete path of the <code>Folder</code> containing the
     *            <code>Message</code>.
     *
     * @throws StoreException
     *             If an error occurs accessing the Store
     */
    int countMessages(String path) throws MailException;

    /**
     * This method sets the version of the Store which is currently being used
     * on the device. If no version is specified, the Store defaults to the
     * latest one.
     *
     *@param version is the store version
     *
     */
    void setVersion(int version);
    
    /**
     * This method gets the version of the Store which is currently being used
     * on the device. 
     *
     */
    int getVersion();
    
    
}

