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

package com.funambol.client.test.contact;

import java.util.Vector;

import com.funambol.client.test.CommandRunner;


public class ContactsCommandRunner extends CommandRunner implements ContactsUserCommands {

    private static final String TAG_LOG = "ContactsCommandRunner";

    // Contact fields used by SetContactField and EmptyContactField commands
    public static final String CONTACT_FIELD_DISPLAY_NAME  = "DisplayName";
    public static final String CONTACT_FIELD_FIRST_NAME    = "FirstName";
    public static final String CONTACT_FIELD_LAST_NAME     = "LastName";
    public static final String CONTACT_FIELD_MIDDLE_NAME   = "MiddleName";
    public static final String CONTACT_FIELD_PREFIX_NAME   = "PrefixName";
    public static final String CONTACT_FIELD_SUFFIX_NAME   = "SuffixName";
    public static final String CONTACT_FIELD_NICK_NAME     = "NickName";
    public static final String CONTACT_FIELD_TEL_HOME      = "TelHome";
    public static final String CONTACT_FIELD_TEL_WORK      = "TelWork";
    public static final String CONTACT_FIELD_TEL_OTHER     = "TelOther";
    public static final String CONTACT_FIELD_TEL_OTHER2    = "TelOther2";
    public static final String CONTACT_FIELD_TEL_CELL      = "TelCell";
    public static final String CONTACT_FIELD_TEL_PAGER     = "TelPager";
    public static final String CONTACT_FIELD_TEL_FAX_HOME  = "TelFaxHome";
    public static final String CONTACT_FIELD_TEL_FAX_WORK  = "TelFaxWork";
    public static final String CONTACT_FIELD_TEL_COMPANY   = "TelCompany";
    public static final String CONTACT_FIELD_TEL_OTHER_FAX = "TelOtherFax";
    public static final String CONTACT_FIELD_EMAIL_HOME    = "EmailHome";
    public static final String CONTACT_FIELD_EMAIL_WORK    = "EmailWork";
    public static final String CONTACT_FIELD_EMAIL_OTHER   = "EmailOther";
    public static final String CONTACT_FIELD_EMAIL_IM      = "Im";
    public static final String CONTACT_FIELD_ADR_OTHER     = "AddressOther";
    public static final String CONTACT_FIELD_ADR_HOME      = "AddressHome";
    public static final String CONTACT_FIELD_ADR_WORK      = "AddressWork";
    public static final String CONTACT_FIELD_WEB           = "Website";
    public static final String CONTACT_FIELD_WEB_HOME      = "WebsiteHome";
    public static final String CONTACT_FIELD_WEB_WORK      = "WebsiteWork";
    public static final String CONTACT_FIELD_BDAY          = "Birthday";
    public static final String CONTACT_FIELD_ANNIVERSARY   = "Anniversary";
    public static final String CONTACT_FIELD_CHILDREN      = "Children";
    public static final String CONTACT_FIELD_SPOUSE        = "Spouse";
    public static final String CONTACT_FIELD_TITLE         = "Title";
    public static final String CONTACT_FIELD_ORGANIZATION  = "Organization";
    public static final String CONTACT_FIELD_NOTE          = "Note";
    public static final String CONTACT_FIELD_PHOTO         = "Photo";
    
    public ContactsCommandRunner(ContactsRobot robot) {
        super(robot);
    }

    public boolean runCommand(String command, Vector pars) throws Throwable {
        if (CREATE_EMPTY_CONTACT_COMMAND.equals(command)) {
            createEmptyContact(command, pars);
        } else if (LOAD_CONTACT_COMMAND.equals(command)) {
            loadContact(command, pars);
        } else if (SET_CONTACT_FIELD_COMMAND.equals(command)) {
            setContactField(command, pars);
        } else if (EMPTY_CONTACT_FIELD_COMMAND.equals(command)) {
            emptyContactField(command, pars);
        } else if (SAVE_CONTACT_COMMAND.equals(command)) {
            saveContact(command, pars);
        } else if (DELETE_CONTACT_COMMAND.equals(command)) {
            deleteContact(command, pars);
        } else if (DELETE_ALL_CONTACTS_COMMAND.equals(command)) {
            deleteAllContacts(command, pars);
        } else if (CREATE_EMPTY_CONTACT_ON_SERVER_COMMAND.equals(command)) {
            createEmptyContactOnServer(command, pars);
        } else if (SAVE_CONTACT_ON_SERVER_COMMAND.equals(command)) {
            saveContactOnServer(command, pars);
        } else if (DELETE_CONTACT_ON_SERVER_COMMAND.equals(command)) {
            deleteContactOnServer(command, pars);
        } else if (DELETE_ALL_CONTACTS_ON_SERVER_COMMAND.equals(command)) {
            deleteAllContactsOnServer(command, pars);
        } else if (SET_CONTACT_AS_VCARD_COMMAND.equals(command)){
            setContactAsVCard(command, pars);
        } else if (CHECK_CONTACT_AS_VCARD.equals(command)) {
            checkContactAsVCard(command, pars);
        } else if (SET_CONTACT_FROM_SERVER.equals(command)) {
            setContactFromServer(command, pars);
        } else if (CREATE_EMPTY_RAW_CONTACT.equals(command)) {
            createEmptyRawContact(command, pars);
        } else if (SET_RAW_CONTACT_DATA.equals(command)) {
            setRawContactData(command, pars);
        } else if (SAVE_RAW_CONTACT.equals(command)) {
            saveRawContact(command, pars);
        } else if (CHECK_RAW_CONTACT_AS_VCARD.equals(command)) {
            checkRawContactAsVCard(command, pars);
        } else if (CHECK_RAW_CONTACT_DATA.equals(command)) {
            checkRawContactData(command, pars);
        } else if (CHECK_CONTACTS_COUNT_ON_SERVER_COMMAND.equals(command)) {
            checkContactsCountOnServer(command, pars);
        } else if (CHECK_CONTACTS_COUNT_COMMAND.equals(command)) {
            checkContactsCount(command, pars);
        } else {
            return false;
        }
        return true;
    }

    private ContactsRobot getContactsRobot() {
        return (ContactsRobot)robot;
    }

    private void createEmptyContact(String command, Vector args) throws Throwable {
        getContactsRobot().createEmptyContact();
    }

    private void createEmptyContactOnServer(String command, Vector args) throws Throwable {
        getContactsRobot().createEmptyContact();
    }

    private void setContactField(String command, Vector args) throws Throwable {
        String field = getParameter(args, 0);
        String value = getParameter(args, 1);
        checkArgument(field, "Missing field name in " + command);
        checkArgument(value, "Missing value in " + command);
        getContactsRobot().setContactField(field, value);
    }

    private void setContactAsVCard (String command, Vector args) throws Throwable {
        String VCard = getParameter(args, 0);
        checkArgument(VCard, "Missing field name in " + command);
        getContactsRobot().setContactAsVCard(VCard);
    }

    private void emptyContactField(String command, Vector args) throws Throwable {
        String field = getParameter(args, 0);
        checkArgument(field, "Missing field in " + command);
        String empty = "";
        if(field.startsWith("Address")) {
            empty = ";;;;;;";
        } else if(field.equals(CONTACT_FIELD_ORGANIZATION)) {
            empty = ";";
        }
        getContactsRobot().setContactField(field, empty);
    }

    private void saveContact(String command, Vector args) throws Throwable {
        getContactsRobot().saveContact();
    }

    private void deleteContact(String command, Vector args) throws Throwable {
        String firstname = getParameter(args, 0);
        String lastname  = getParameter(args, 1);
        checkArgument(firstname, "Missing firstname in " + command);
        checkArgument(lastname, "Missing lastname in " + command);
        getContactsRobot().deleteContact(firstname, lastname);
    }

    private void deleteAllContacts(String command, Vector args) throws Throwable {
        getContactsRobot().deleteAllContacts();
    }

    private void checkContactAsVCard(String command, Vector args) throws Throwable {
        String vcard     = getParameter(args, 0);
        checkArgument(vcard, "Missing vcard in " + command);
        getContactsRobot().checkContactAsVCard(vcard);
    }

    private void setContactFromServer(String command, Vector args) throws Throwable {
        String vcard     = getParameter(args, 0);
        checkArgument(vcard, "Missing vcard in " + command);
        getContactsRobot().setContactFromServer(vcard);
    }

    private void loadContact(String command, Vector args) throws Throwable {
        String firstname = getParameter(args, 0);
        String lastname  = getParameter(args, 1);
        checkArgument(lastname, "Missing lastname in " + command);
        checkArgument(lastname, "Missing lastname in " + command);
        getContactsRobot().loadContact(firstname, lastname);
    }

    private void saveContactOnServer(String command, Vector args) throws Throwable {
        // If there is only one par, this is the JSON object directly
        // otherwise it is first and last name
        String firstname = getParameter(args, 0);
        String lastname  = getParameter(args, 1);
        checkArgument(firstname, "Missing firstname or JSON object in " + command);

        if (lastname == null) {
            getContactsRobot().saveContactOnServer(firstname);
        } else {
            getContactsRobot().saveContactOnServer(firstname, lastname);
        }
    }

    private void deleteContactOnServer(String command, Vector args) throws Throwable {
        String firstname = getParameter(args, 0);
        String lastname  = getParameter(args, 1);
        checkArgument(firstname, "Missing firstname in " + command);
        checkArgument(lastname, "Missing lastname in " + command);
        getContactsRobot().deleteContactOnServer(firstname, lastname);
    }

    private void deleteAllContactsOnServer(String command, Vector args) throws Throwable {
        getContactsRobot().deleteAllContactsOnServer();
    }

    private void createEmptyRawContact(String command, Vector args) throws Throwable {
        getContactsRobot().createEmptyRawContact();
    }

   private void setRawContactData(String command, Vector args) throws Throwable {
        String mimeType  = getParameter(args, 0);
        Vector dataFields = new Vector();
        String data = null;
        int count = 1;
        while((data = getParameter(args, count++)) != null) {
            dataFields.addElement(data);
        }
        checkArgument(mimeType, "Missing field mimeType in " + command);
        getContactsRobot().setRawContactData(mimeType, dataFields);
    }

    private void saveRawContact(String command, Vector args) throws Throwable {
        getContactsRobot().saveRawContact();
    }

    private void checkRawContactAsVCard(String command, Vector args) throws Throwable {
        String vcard= getParameter(args, 0);
        checkArgument(vcard, "Missing vcard in " + command);
        getContactsRobot().checkRawContactAsVCard(vcard);
    }

    private void checkRawContactData(String command, Vector args) throws Throwable {
        String mimeType  = getParameter(args, 0);
        Vector dataFields = new Vector();
        String data = null;
        int count = 1;
        while((data = getParameter(args, count++)) != null) {
            dataFields.addElement(data);
        }
        checkArgument(mimeType, "Missing field mimeType in " + command);
        getContactsRobot().checkRawContactData(mimeType, dataFields);
    }

    /**
     * Command to check the items count on server
     * @param command the String formatted command to check the server's items
     * count
     * @param args the command's String formatted arguments
     * @throws Throwable if anything went wrong
     */
    private void checkContactsCountOnServer(String command, Vector args) throws Throwable {

        String count =  getParameter(args, 0);
        checkArgument(count, "Missing count in " + command);
        getContactsRobot().checkItemsCountOnServer(Integer.parseInt(count));
    }

    /**
     * Command to check the items count on the device
     * @param command the String formatted command to check the client's items
     * count
     * @param args the command's String formatted arguments
     * @throws Throwable if anything went wrong
     */
    public void checkContactsCount(String command, Vector args) throws Throwable {

        String count =  getParameter(args, 0);
        checkArgument(count, "Missing count in " + command);
        getContactsRobot().checkContactsCount(Integer.parseInt(count));
    }


}
