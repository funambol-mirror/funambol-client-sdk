/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

package com.funambol.client.test;

import java.util.Vector;

import com.funambol.syncml.spds.SyncItem;
import com.funambol.util.StringUtil;

public abstract class ContactsRobot extends Robot {
   
    private static final String TAG_LOG = "ContactsRobot";

    protected static final char FOLDING_INDENT_CHAR = ' ';

    protected long currentContactId = -1;

    protected long incrementalServerItemkey = 10000000;

    public void saveContactOnServer(CheckSyncClient client) throws Throwable {

        CheckSyncSource source = client.getSyncSource(
                CheckSyncClient.SOURCE_NAME_CONTACTS);
        SyncItem item = new SyncItem(Long.toString(incrementalServerItemkey++));
        item.setContent(getCurrentContactVCard().getBytes());
        
        if(currentContactId != -1) {
            item.setKey(Long.toString(currentContactId));
            source.updateItemFromOutside(item);
        } else {
            source.addItemFromOutside(item);
        }

        // Reset current contact
        currentContactId = -1;
    }

    public void deleteContactOnServer(String firstname, String lastname,
            CheckSyncClient client) throws Throwable {
        CheckSyncSource source = client.getSyncSource(
                CheckSyncClient.SOURCE_NAME_CONTACTS);
        String itemKey = findContactKeyOnServer(firstname, lastname, client);
        source.deleteItemFromOutside(itemKey);
    }

    public void deleteAllContactsOnServer(CheckSyncClient client) throws Throwable {
        CheckSyncSource source = client.getSyncSource(
                CheckSyncClient.SOURCE_NAME_CONTACTS);
        source.deleteAllFromOutside();
    }

    /**
     * Order the vCard item fields alphabetically.
     * @param vcard
     * @return 
     */
    protected String orderVCard(String vcard) {

        Vector fields_al = getFieldsVector(vcard);

        // order the fields array list
        String result = "";
        String[] fields = StringUtil.getStringArray(fields_al);
        for(int i=0; i<fields.length; i++) {
            for(int j=fields.length-1; j>i; j--) {
                if(fields[j].compareTo(fields[j-1])<0) {
                    String temp = fields[j];
                    fields[j] = fields[j-1];
                    fields[j-1] = temp;
                }
            }
            // Exclude empty fields
            if(!fields[i].endsWith(":") && !fields[i].endsWith(":;")
            && !fields[i].endsWith(":;;;;") && !fields[i].endsWith(":;;;;;;")) {
                result += fields[i] + "\r\n";
            }
        }
        return result;
    }

    private Vector getFieldsVector(String vcard) {

        String sep[] = {"\r\n"};
        String lines[] = StringUtil.split(new String(vcard), sep);

        Vector fieldsAl = new Vector();
        String field = "";
        for(int i=0;i<lines.length;++i) {
            String line = lines[i];
            if(line.length() > 0 && line.charAt(0) == FOLDING_INDENT_CHAR) {
                // this is a multi line field
                field += line.substring(1); // cut the indent char
            } else {
                if(!field.equals("")) {
                    fieldsAl.addElement(field);
                }
                field = line;
            }
        }
        // add the latest field
        fieldsAl.addElement(field);

        return fieldsAl;
    }


    public abstract void createEmptyContact() throws Throwable;
    public abstract void setContactField(String field, String value) throws Throwable;

    public abstract void loadContact(String firstName, String lastName) throws Throwable;
    public abstract void saveContact() throws Throwable;
    public abstract void deleteContact(String firstname, String lastname) throws Throwable;
    public abstract void deleteAllContacts() throws Throwable;

    public abstract void checkNewContact(String firstname, String lastname,
            CheckSyncClient client, boolean checkContent) throws Throwable;

    public abstract void checkUpdatedContact(String firstname, String lastname,
            CheckSyncClient client, boolean checkContent) throws Throwable;

    public abstract void checkDeletedContact(String firstname, String lastname, 
            CheckSyncClient client) throws Throwable;

    public abstract void checkNewContactOnServer(String firstname, String lastname,
            CheckSyncClient client, boolean checkContent) throws Throwable;

    public abstract void checkUpdatedContactOnServer(String firstname, String lastname,
            CheckSyncClient client, boolean checkContent) throws Throwable;

    public abstract void checkDeletedContactOnServer(String firstname, String lastname,
            CheckSyncClient client) throws Throwable;

    public abstract void loadContactOnServer(String firstName, String lastName,
            CheckSyncClient client) throws Throwable;

    protected abstract String getCurrentContactVCard() throws Throwable;

    protected abstract String findContactKeyOnServer(String firstName, String lastName,
            CheckSyncClient client) throws Throwable;
}
