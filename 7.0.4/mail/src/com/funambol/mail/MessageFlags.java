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
import java.io.DataOutputStream;
import java.io.IOException;

import com.funambol.storage.Serializable;

/**
 * An object used to determine the state of a <code>Message</code> within a
 * <code>Folder</code> and the flags related to this <code>Message</code>.
 */
public class MessageFlags implements Serializable {

    // -------------------------------------------------------------- Constants

    //--------------------- Synchronizable flags ------------------------------

    // These flags can be sent to and received by the server, if the protocol
    // supports it.

    /**
     * This message has been answered. This flag is set by clients to indicate
     * that this message has been answered to
     */
    public static final int ANSWERED = 0x00000001;

    /**
     * This message has been forwarded. This flag is set by clients to indicate
     * that this message has been forwarded to
     */
    public static final int FORWARDED = 0x00000002;

    /**
     * This message is seen. This flag is set by clients to indicate that this
     * message has been opened by the user.
     */
    public static final int OPENED = 0x00000004;

    /**
     * This message is a draft. This flag is set by clients
     */
    public static final int DRAFT = 0x00000008;

    /**
     * This message is flagged. No semantic is predefined for this flag.
     * Clients alter this flag
     */
    public static final int FLAGGED = 0x00000010;

    /**
     * This message is marked as deleted. Clients set this flag to mark a
     * message as deleted. The expunge operation on a folder removes all
     * messages in that folder that are marked for deletion
     */

    public static final int DELETED = 0x00000020;

    //--------------------- Local flags -----------------------------------

    // These flags are kept locally on the mail client.

    /**
     * This message has been partially downloaded from the server. It may
     * contain only the headers, or a part of the body. Clients can use this
     * flags to inform the user that more data are available.
     */
    public static final int PARTIAL = 0x00000100;
    
   
    //--------------------- Status flags ----------------------------------

    // These flags are local to the mail client and can be used to track
    // the synchronization status of a message.

    /**
     * The message is queued for sending: the next sync with the server will
     * process this message.
     */
    public static final int TX_SENDING = 0x00010000;

    /**
     * The message has been sent successfully by this client.
     */
    public static final int TX_SENT = 0x00020000;

    /**
     * The message has not been sent due to an error during the transmission.
     */
    public static final int TX_ERROR = 0x00040000;


    // ------------------------------------------------------------- Attributes

    // The flags bit mask
    private int flags;
    

    // ----------------------------------------------------------- Constructors

    /**
     * Default constructor
     */
    public MessageFlags () {
        flags = 0;
    }

    public MessageFlags (MessageFlags mf) {
        this.flags = mf.flags;
    }
    // --------------------------------------------------------- Public Methods

    /**
     * Reset all the flags.
     */
    public void clearFlags() {
        flags = 0;
    }

    /**
     * Reset the flags specified by mask.
     * 
     * @param mask a bit mask with the flags to clear set to 1.
     */
    public void clearFlags(int mask) {
        flags &= ~mask;
    }

    /**
     * Returns the flag mask for this message.
     * 
     * @return the flag mask for this message.
     */
    public int getFlags() {
        return flags;
    }

    /**
     * Set the specified flag
     * 
     * @param flag the flag mask for this message (see com.funambol.mail.Message)
     */
    public void setFlag(int flag, boolean set) {
        if(set){
            flags |= flag;
        }
        else {
            flags &= ~flag;
        }
    }

    /**
     * Set the message flags according to the given mask
     */
    public void setFlags(int mask) {
        flags = mask;
    }

    /**
     * Set the message flags according to the given mask
     * 
     * @return the flag mask for this message.
     */
    public boolean isSet(int flag) {
        return ((flags & flag) != 0);
    }

    // ------------------------------------------- Serializable implementation

    /**
     * Write object fields to the output stream.
     * @param out Output stream
     * @throws IOException
     */
    public void serialize(DataOutputStream out) throws IOException {
        out.writeInt(flags);
    }

    /**
     * Read object field from the input stream.
     * @param in Input stream
     * @throws IOException
     */
    public void deserialize(DataInputStream in) throws IOException {
        flags = in.readInt();
    }

}

