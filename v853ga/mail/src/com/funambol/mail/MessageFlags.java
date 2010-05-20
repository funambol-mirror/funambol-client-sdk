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
import com.funambol.util.Log;

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
    public static final int DELETED = 0x00000020;    //--------------------- Local flags -----------------------------------

    // These flags are kept locally on the mail client.
    /**
     * This message has been partially downloaded from the server. It may
     * contain only the headers, or a part of the body. Clients can use this
     * flags to inform the user that more data are available.
     */
    public static final int PARTIAL = 0x00000100;    //--------------------- Status flags ----------------------------------

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
    public static final int TX_ERROR = 0x00040000;    // ------------------------------------------------------------- Attributes

    // The flags bit mask
    private int flags;
    // ----------------------------------------------------------- Constructors
    /**
     * Default constructor
     */
    public MessageFlags() {
        flags = 0;
    }

    public MessageFlags(MessageFlags mf) {
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
        if (set) {
            flags |= flag;
        } else {
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

    /**
     * Merge the current object with the given mask
     * @param f1 is the MessageFlags with the values to be merged
     */
    public void merge(MessageFlags f1) {
        merge(f1, true);
    }

    /**
     * Merge the current object with the given mask. The flags of the current 
     * object will be entirely overridden with the given mask.
     * @param f1 is the MessageFlags with the values to be merged
     * @param enableDelayedFlags is used to enable the update of answer and forward flag
     */
    public void merge(MessageFlags f1, boolean enableDelayedFlags) {
        if (enableDelayedFlags) {
            if (this.isSet(ANSWERED) != f1.isSet(ANSWERED)) {
                this.setFlag(ANSWERED, f1.isSet(ANSWERED));
                Log.debug("Answered flag not equals");
            }
            if (this.isSet(FORWARDED) != f1.isSet(FORWARDED)) {
                this.setFlag(FORWARDED, f1.isSet(FORWARDED));
                Log.debug("Forwarded flag not equals");
            }
        }
        if (this.isSet(DELETED) != f1.isSet(DELETED)) {
            this.setFlag(DELETED, f1.isSet(DELETED));
            Log.debug("Deleted flag not equals");
        }
        if (this.isSet(DRAFT) != f1.isSet(DRAFT)) {
            this.setFlag(DRAFT, f1.isSet(DRAFT));
            Log.debug("Draft flag not equals");
        }
        if (this.isSet(FLAGGED) != f1.isSet(FLAGGED)) {
            this.setFlag(FLAGGED, f1.isSet(FLAGGED));
            Log.debug("Flagged flag not equals");
        }
        if (this.isSet(OPENED) != f1.isSet(OPENED)) {
            this.setFlag(OPENED, f1.isSet(OPENED));
            Log.debug("Opened flag not equals");
        }
        if (this.isSet(PARTIAL) != f1.isSet(PARTIAL)) {
            this.setFlag(PARTIAL, f1.isSet(PARTIAL));
            Log.debug("Partial flag not equals");
        }
        if (this.isSet(TX_ERROR) != f1.isSet(TX_ERROR)) {
            this.setFlag(TX_ERROR, f1.isSet(TX_ERROR));
            Log.debug("Tx_Error flag not equals");
        }
        if (this.isSet(TX_SENDING) != f1.isSet(TX_SENDING)) {
            this.setFlag(TX_SENDING, f1.isSet(TX_SENDING));
            Log.debug("Tx_Sending flag not equals");
        }
        if (this.isSet(TX_SENT) != f1.isSet(TX_SENT)) {
            this.setFlag(TX_SENT, f1.isSet(TX_SENT));
            Log.debug("Tx_Sent flag not equals");
        }
    }

    /**
     * Compare the current object with the given mask
     * @param newMask is the mask to be compared
     * @return int[] with the comparison result
     */
    public int[] compareFlags(MessageFlags newMask) {
        int[] ret = new int[10];

        for (int i = 0; i < ret.length; i++) {
            ret[i] = 0;
        }

        if (this.isSet(ANSWERED) != newMask.isSet(ANSWERED)) {
            ret[0] = 1;
            Log.debug("Answered flag not equals");
        }
        if (this.isSet(DELETED) != newMask.isSet(DELETED)) {
            ret[1] = 1;
            Log.debug("Deleted flag not equals");
        }
        if (this.isSet(DRAFT) != newMask.isSet(DRAFT)) {
            ret[2] = 1;
            Log.debug("Draft flag not equals");
        }
        if (this.isSet(FLAGGED) != newMask.isSet(FLAGGED)) {
            ret[3] = 1;
            Log.debug("Flagged flag not equals");
        }
        if (this.isSet(FORWARDED) != newMask.isSet(FORWARDED)) {
            ret[4] = 1;
            Log.debug("Forwarded flag not equals");
        }
        if (this.isSet(OPENED) != newMask.isSet(OPENED)) {
            ret[5] = 1;
            Log.debug("Opened flag not equals");
        }
        if (this.isSet(PARTIAL) != newMask.isSet(PARTIAL)) {
            ret[6] = 1;
            Log.debug("Partial flag not equals");
        }
        if (this.isSet(TX_ERROR) != newMask.isSet(TX_ERROR)) {
            ret[7] = 1;
            Log.debug("Tx_Error flag not equals");
        }
        if (this.isSet(TX_SENDING) != newMask.isSet(TX_SENDING)) {
            ret[8] = 1;
            Log.debug("Tx_Sending flag not equals");
        }
        if (this.isSet(TX_SENT) != newMask.isSet(TX_SENT)) {
            ret[9] = 1;
            Log.debug("Tx_Sent flag not equals");
        }

        return ret;

    }

    public String toString() {
        return ("[MessageFlags.toString]" + "\n" +
                "Open flag: " + this.isSet(OPENED) + "\n" +
                "Reply flag: " + this.isSet(ANSWERED) + "\n" +
                "Flag flag: " + this.isSet(FLAGGED) + "\n" +
                "Forward flag: " + this.isSet(FORWARDED) + "\n" +
                "Delete flag: " + this.isSet(DELETED) + "\n" +
                "TxSending flag: " + this.isSet(TX_SENDING) + "\n" +
                "TxSent flag: " + this.isSet(TX_SENT) + "\n" +
                "TxError flag: " + this.isSet(TX_ERROR) + "\n" +
                "Partial flag: " + this.isSet(PARTIAL) + "\n" +
                "Draft flag: " + this.isSet(DRAFT));
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

