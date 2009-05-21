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


/**
 * Represents a mailbox folder on the device. Folders can contain
 * <code>Message</code> objects, other <code>Folder</code> objects or both.
 * This nesting capability enables a folder hierarchy for stored messages. The
 * different levels of hierarchy in a folder's full name are separated by the
 * hierarchy separator character, which you can retrieve using
 * <code>getSeparator()</code>. You can retrieve a contained folder by name
 * using <code>getFolder()</code>, or a list of contained folders by using
 * <code>list()</code>
 */
public class Folder {

    // -------------------------------------------------------------- Attributes

    /**
     * The name of the folder
     */
    private String fullname = null;

    /**
     * A store contains the list of main Folders, and each <code>Folder</code>
     * a reference to its <code>Store</code>
     */
    private Store store = null;

    /**
     * A cache of the the subfolders contained by this <code>Folder</code>
     * 
     * TODO: evaluate the weight and benefits of this cache.
     */
    private Folder[] children = null;

    /**
     * The parent Folder of this Folder
     */
    private Folder parent = null;

    /**
     * The list of the contained message Ids.
     */
    private String[] messageIDs;

    // ------------------------------------------------------------ Constructors

    /*
     * The standard constructor is private, because this class must be
     * istantiated with the name and store.
     */
    private Folder() {
    }

    /**
     * Constructs a new <code>Folder</code> providing a name for it and a
     * reference to the <code>Store</code> in which it has to be created
     * 
     * @param fullname
     *            The name for this <code>Folder</code>
     * @param store
     *            The <code>Store</code> in which the folder has to be created
     */
    public Folder(String fullname, Store store) {
        this.fullname = fullname;
        this.store = store;
    }

    /**
     * Appends the given <code>Message</code> to this <code>Folder</code>
     */
    public void appendMessage(Message msg) throws MailException {
        msg.setParent(this);
        store.addMessage(fullname, msg);
    }

    /**
     * Updates the given <code>Message</code>
     */
    public void updateMessage(Message msg) throws MailException {
        store.saveMessage(fullname, msg);
    }

    /**
     * Gets the <code>Message</code> object corresponding to the given record
     * ID
     */
    public Message getMessage(String recordId) throws MailException {
        Message ret = store.readMessage(fullname, recordId);
        //if the message has been previously deleted this generates NPE
        if (ret!=null) {
            ret.setParent(this);
        }
        return ret;
    }

    /**
     * This method returns the headers of all the messages in the
     * folder <code>path</code>.
     *
     * @return An array of <code>Message</code> with all the headers
     *          set but without content
     *
     * @throws MailException
     *             If an error occurs accessing the Store
     */
    public Message[] getMsgHeaders() throws MailException {
        Message[] ret = store.getMsgHeaders(fullname);
        for(int i=0, l=ret.length; i<l; i++) {
            ret[i].setParent(this);
        }
        return ret;
    }

    /**
     * Get the first message in the folder.
     *
     * @return the first message if it exists, null otherwise
     * @throws MailException if an error occurs while reading a message, or if
     * the folder is not found in the store
     */
    public Message getFirstMessage() throws MailException {
        Message msg = store.readFirstMessage(fullname);
        //if the message has been previously deleted this generates NPE
        if (msg != null) {
            msg.setParent(this);
        }
        return msg;
    }

    /**
     * Get the next message in the folder.
     *
     * @return the next message if it exists, null otherwise
     * @throws MailException if an error occurs while reading a message, or if
     * the folder is not found in the store
     */
    public Message getNextMessage() throws MailException {
        Message msg = store.readNextMessage(fullname);
        //if the message has been previously deleted this generates NPE
        if (msg != null) {
            msg.setParent(this);
        }
        return msg;
    }
    
    /**
     * Deletes a <code>Message</code> from this <code>Folder</code>
     * The message is properly updated so that it is both complete (completely
     * loaded in memory) and its parent folder is cleared. After this call the
     * Message object is valid and can be used in any operation.
     * 
     * @param msg A reference to the message to be deleted
     */
    public void deleteMessage(Message msg) throws MailException {
        // Before removing the message we load it completely, so that we can
        // handle it without the need to read the store
        msg.getContent();
        // Now remove it from the store
        store.removeMessage(fullname, ""+msg.getRecordId());
        // The message no longer belongs to this folder
        // TODO: check this. It is preferrable to enable it after more investigation
        // msg.setParent(null);
    }

    /**
     * Deletes a <code>Message</code> from this <code>Folder</code>, using
     * the record id as key. If the client has one or more Message objects that
     * hold the message at this record id position, there is no guarantee these
     * objects are still valid after this call. For such cases the client should
     * use @see deleteMessage(Message)
     *
     * @param key A reference to the message to be deleted
     */
    public void deleteMessage(String recordId) throws MailException {
        store.removeMessage(fullname, recordId);
    }

    /**
     * Deletes a <code>Message</code> from this <code>Folder</code>, using
     * a positional index as key. If the client has one or more Message objects
     * that hold the message at this record id position, there is no guarantee
     * these objects are still valid after this call. For such cases the client
     * should use @see deleteMessage(Message)
     * 
     * The position starts from 1 and it is always referred to the messages
     * currently present in the folder. Deleting a message causes the index
     * to be shifted. This must be taken into account by calling methods.
     * The order in which the messages are ordered is not defined, and the
     * caller must not rely on it.
     * 
     * @param index The index of the message to delete
     */
    public void deleteMessage(int index) throws MailException {
        if(index < 1){
            throw new MailException(
                        MailException.MESSAGE_DELETE_ERROR,
                        "[getMessage] Invalid index: " + index);
        }
        // Index starts from 1, adapt it to array index
        index--;
        // Get the array of message IDs
        String[] messageIDs = store.getMessageIDs(fullname);
        // Check index
        if (index>messageIDs.length){
            throw new MailException(
                        MailException.MESSAGE_DELETE_ERROR,
                        "[getMessage] Invalid index: " + index);
        }
        store.removeMessage(fullname, messageIDs[index]);
    }

    /**
     * Get the total number of messages in this <code>Folder</code>
     */
    public int getMessageCount() throws MailException {
        return store.countMessages(fullname);
    }

    /**
     * Returns the total number of messages in this <code>Folder</code>
     * with the specified flag enabled or disabled
     *
     * @param flag the message flag (@see MessageFlags)
     * @param set  if this parameter is true, then a message is counted iff
     *             the corresponding flag is set to true.
     *             If this parameter is false, then a message is counted iff
     *             the corresponding flag is set to false.
     * @throws MailException if getFirstMessage or getNextMessage throw this
     *                        exception, then this method propagates it
     */
    public int getMessageWithFlagCount(int flag, boolean set)
    throws MailException {
        int res = 0;

        Message msg = getFirstMessage();
        while (msg != null) {
            MessageFlags flags = msg.getFlags();
            if (set && flags.isSet(flag)) {
                ++res;
            } else if (!set && !flags.isSet(flag)) {
                ++res;
            }
            msg = getNextMessage();
        }
        return res;
    }


    /**
     * Returns the parent Folder of this Folder
     * 
     * @return The Folder object this Folder is child of
     */
    public Folder getParent() throws MailException {
        if (parent == null){
            int idx = fullname.lastIndexOf('/');
            // To be a subfolder, the name must be at least "/a/b"
            if(idx > 1) {
                String parentName = fullname.substring(0, idx);
                parent = store.getFolder(parentName);
            }
        }
        return parent;
    }

    /**
     * Returns the delimiter character that separates this <code>Folder</code>'s
     * pathname from the names of immediate subfolders
     * 
     * TODO: Perhaps it is better to define in a field the character used as
     * separator and not directly here (this adopted here is the solution by
     * Sun)
     * 
     * @return The hierarchy separator character
     */
    public char getSeparator() {
        return '/';
    }

    /**
     * This method returns the list of folders under this Folder
     */
    public Folder[] list() throws MailException{
        if(children == null){
            children= store.list(fullname);
        }
        return children;
    }

    /**
     * Retrieves the <code>Store</code> containing this <code>Folder</code>
     * 
     * @return A reference to the Store containing this Folder
     */
    public Store getStore() {
        return store;
    }

    /**
     * Returns the full name of this <code>Folder</code> as passed in the
     * constructor
     * 
     * @return The string containing the full name of this <code>Folder</code>
     */
    public String getFullName() {
        return fullname;
    }

    /**
     * Returns the name of this <code>Folder</code>, taking the last part of
     * the fullname
     * 
     * @return The string containing the name of this <code>Folder</code>
     */
    public String getName() {
        return fullname.substring(
                    fullname.lastIndexOf('/') + 1, fullname.length()
                );
    }

    /**
     * Returns the requested subfolder <p>
     * 
     * @param name
     *            The name of the subfolder
     * @return The <code>Folder</code> object representing the requested
     *         subfolder
     */
    public Folder getFolder(String name) throws MailException {
        String path = fullname+'/'+name;
        Folder ret = null;
        if (children != null){
            ret = searchChild(path);
        }
        else{
            ret = store.getFolder(path);
        }
        return ret;
    }

    //---------------------------------------------------------- Private methods

    // Search a child inside the Folder cache.
    private Folder searchChild(String path){
        for(int i=0; i<children.length; i++){
            if(children[i].getName().equals(path)){
                return children[i];
            }
        }
        return null;
    }

}

