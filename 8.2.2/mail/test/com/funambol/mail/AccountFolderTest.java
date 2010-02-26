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

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;
import junit.framework.*;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

import java.util.Date;

public class AccountFolderTest extends TestCase {

    public AccountFolderTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
    }

    public void setUp() {
    }

    public void tearDown() {
    }

    /**
     * Test of serialize method, of class com.funambol.mail.AccountFolder.
     */
    public void testserialize() throws Exception {

        Log.info("AccountFolderTest: testserialize");

        AccountFolder account = new AccountFolder("/Funambol", new Date(1240907915),
                StoreFactory.getStore(), "Name Surname", "email@address.com");

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        account.serialize(dout);

        //Creates expected result
        ByteArrayOutputStream expectedBaos = new ByteArrayOutputStream();
        DataOutputStream expectedDout = new DataOutputStream(expectedBaos);

        expectedDout.writeChar(AccountFolder.ACCOUNT_ITEM_PREFIX);
        expectedDout.writeUTF(account.getFullName());
        expectedDout.writeUTF(account.getRole());
        expectedDout.writeLong(account.getCreated().getTime());
        expectedDout.writeUTF(account.getVisibleName());
        expectedDout.writeUTF(account.getEmailAddress());

        assertTrue(isStreamEqual(baos, expectedBaos));
        Log.info("AccountFolderTest: testserialize successful");
    }

    /**
     * Test of deserialize method, of class com.funambol.mail.AccountFolder.
     */
    public void testdeserialize() throws Exception {

        Log.info("AccountFolderTest: testdeserialize");

        AccountFolder account = new AccountFolder("/Funambol", new Date(1240907915),
                StoreFactory.getStore(), "Name Surname", "email@address.com");

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dout = new DataOutputStream(baos);
        account.serialize(dout);

        ByteArrayInputStream bais = new ByteArrayInputStream(baos.toByteArray());
        DataInputStream dis = new DataInputStream(bais);
        AccountFolder expAccount = new AccountFolder(null, null, null, null, null);

        // The first byte is not deserialized by the AccountFolder itself
        dis.readChar();
        expAccount.deserialize(dis);

        boolean isAccountOk = account.getName().equals(expAccount.getName());
        isAccountOk |= account.getRole().equals(expAccount.getRole());
        isAccountOk |= account.getCreated().equals(expAccount.getCreated());
        isAccountOk |= account.getFullName().equals(expAccount.getFullName());
        isAccountOk |= account.getVisibleName().equals(expAccount.getVisibleName());
        isAccountOk |= account.getEmailAddress().equals(expAccount.getEmailAddress());
        
        assertTrue(isAccountOk);
         
        Log.info("AccountFolderTest: testdeserialize successful");
    }

    /**
     * Compare actual and expected Byte array byte by byte.
     */
    private boolean isStreamEqual(ByteArrayOutputStream baos,
            ByteArrayOutputStream expectedBaos)
            throws Exception {
        boolean ret = false;
        byte[] actual = expectedBaos.toByteArray();
        byte[] expected = baos.toByteArray();
        for (int j = 0; j < actual.length; j++) {
            ret = actual[j] == expected[j];
            if (!ret) {
                break;
            }
        }
        return ret;
    }
}
