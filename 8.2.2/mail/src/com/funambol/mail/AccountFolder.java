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

import java.util.Date;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

/**
 * AccountFolder represents a particular <code>Folder</code> with additional
 * attributes: <code>displayName</code> and <code>emailAddress</code>.
 *
 * It has the role attribute fixed to "account".
 */
public class AccountFolder extends Folder {

    public static final char ACCOUNT_ITEM_PREFIX = 'A';
    
    public static final String ACCOUNT_ROLE  = "account";
    
    public static final String VISIBLE_NAME  = "VisibleName";
    public static final String EMAIL_ADDRESS = "EmailAddress";

    /**
     * The VisibleName of the <code>Account</code>
     */
    private String visibleName = null;

    /**
     * The EmailAddress of the <code>Account</code>
     */
    private String emailAddress = null;

    /**
     * Constructs a new <code>AccountFolder</code> providing a reference to the
     * <code>Store</code> in which it has to be created. It is used when
     * creating a AccountFolder from a DataInputStream.
     *
     * @param store The <code>Store</code> in which the folder has to be created
     */
    public AccountFolder(Store store) {
        super(store);
        this.visibleName = null;
        this.emailAddress = null;
    }

    /**
     * Constructs a new <code>AccountFolder</code> providing a name for it,
     * the creation date and a reference to the <code>Store</code> in which it
     * has to be created.
     *
     * @param fullname The fullname for this <code>AccountFolder</code>
     * @param created The creation date for this <code>AccountFolder</code>
     * @param store The <code>Store</code> in which the folder has to be created
     * @param visibleName The VisibleName of the <code>AccountFolder</code>
     * @param emailAddress The EmailAddress of the <code>AccountFolder</code>
     */
    public AccountFolder(String fullname, Date created, Store store,
            String visibleName, String emailAddress) {
        super(fullname, ACCOUNT_ROLE, created, store);
        this.visibleName = visibleName;
        this.emailAddress = emailAddress;
    }

    /**
     * Return the <code>VisibleName</code> attribute
     * @return The <code>VisibleName</code>
     */
    public String getVisibleName() {
        return visibleName;
    }

    /**
     * Return the <code>EmailAddress</code> attribute
     * @return The <code>EmailAddress</code>
     */
    public String getEmailAddress() {
        return emailAddress;
    }

    /**
     * Set the <code>VisibleName</code> attribute
     * @param name The new visible name
     */
    public void setVisibleName(String name) {
        visibleName = name;
    }

    /**
     * Set the <code>EmailAddress</code> attribute
     * @param address The new email address
     */
    public void setEmailAddress(String address) {
        emailAddress = address;
    }

    /**
     * The the Account role
     * @return ACCOUNT_ROLE
     */
    public String getRole() {
        return ACCOUNT_ROLE;
    }

    /**
     * Check whether the provided prefix char represents a
     * <code>AccountFolder</code> <code>DataInputStream</code>
     *
     * @param prefix The item prefix.
     * @return true If supported.
     */
    public static boolean isSupportedStream(char prefix) {
        return (prefix == AccountFolder.ACCOUNT_ITEM_PREFIX);
    }

    // Append the prefix for a AccountFolder record item
    protected void writeRecordPrefix(DataOutputStream dout) throws IOException {
        dout.writeChar(AccountFolder.ACCOUNT_ITEM_PREFIX);
    }

    // ---------------------------- Serializable implementation

    public void serialize(DataOutputStream dout) throws IOException {
        super.serialize(dout);
        dout.writeUTF(visibleName);           // "Name Surname"
        dout.writeUTF(emailAddress);          // "email@address.com"
    }

    public void deserialize(DataInputStream din) throws IOException {
        super.deserialize(din);
        visibleName = din.readUTF();
        emailAddress = din.readUTF();
    }

    public boolean equals(AccountFolder account) {
        return this.emailAddress.equals(account.getEmailAddress()) &&
               this.visibleName.equals(account.getVisibleName());
    }
}

