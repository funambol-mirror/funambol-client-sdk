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

/**
 * This component lists all the commands available in the Android automatic test
 * scripting language.
 */
public interface UserCommands {


    /**
     * This instruction must be the first one at the beginning of a test.
     * For a single test script there is only one of such instruction, while a 
     * multi test script may contain several instances.
     *
     * @param name is the name of the test as it will be reported in the final
     *             summary.
     */
    public void BeginTest(String name);

    /**
     * Terminates the commands of a test.
     */
    public void EndTest();

    /**
     * This command includes another script file.
     *
     * @param name is the script name. If this is an URL, then the script is
     *             fetched from that location, otherwise the base url if the main script is
     *             added as prefix to this name
     */
    public void Include(String name);
    
    /**
     * This command starts the main application. When the test starts the main
     * application is not started until the script triggers this command.
     * This instruction starts the FunambolClient Activity, so it is recommended
     * to run the waitForActivity command in order to verify that it is started
     * correctly.
     */
    public void StartMainApp();
    
    /**
     * This command suspends the test execution for the given amount of time.
     *
     * @param delay is an integer value expressed in seconds. If the delay
     *              is &lt= 0 then the command wait forever and the script gets
     *              interrupted. This is useful if the script shall give the
     *              user the possibility to use the application.
     */
    public void Wait(int delay);

    /**
     * This command wait for a sync to start and finish within a maximum amount
     * of time. As soon as the sync terminates, the script continues its
     * execution.
     *
     * @param minStart is the time the script waits for the sync to start. If
     *                 the sync does not start withing this time, the test fails
     * @param maxTime  is the maximum time the script is willing for the sync to
     *                 terminate. If it does not terminate within this limit,
     *                 the test fails.
     */
    public void WaitForSyncToComplete(String minStart, String maxTime);

    /**
     * This command wait for a authentication process to start and finish within
     * a maximum amount of time. As soon as the authentication terminates, the
     * script continues its execution.
     *
     * @param minStart is the time the script waits for the authentication to
     *                 start. If the authentication does not start withing this
     *                 time, the test fails
     * @param maxTime  is the maximum time the script is willing for the 
     *                 authentication to terminate. If it does not terminate
     *                 within this limit, the test fails.
     */
    public void WaitForAuthToComplete(String minStart, String maxTime);

    /**
     * This command simulates a user action via the device keypad.
     * @param command the command to simulate. Possible values are:
     *                <ul>
     *                  <li> KeyDown to move down </li>
     *                  <li> KeyUp to move up </li>
     *                  <li> KeyLeft to move left </li>
     *                  <li> KeyRight to move right </li>
     *                  <li> KeyFire to click </li>
     *                  <li> KeyMenu to open menu </li>
     *                  <li> KeyBack to return back </li>
     *                  <li> KeyDelete to delete </li>
     *                </ul>
     * @param count the number of commands to send (not mandatory)
     */
    public void KeyPress(String command, int count);

    /**
     * This command simulates a user writing a text through the device keyboard.
     * @param text the text to write (e.g. into an input field)
     */
    public void WriteString(String text);

    /**
     * This command forces the next sync to be a slow sync for the given source
     *
     * @param sourceName is the name of the source. The value is what is displayed
     * on the main screen for that source.
     */
    public void ForceSlowSync(String sourceName);

    /**
     * This command checks the amount of items exchanged between the server and
     * the client during the last synchronization (for a given source).
     *
     * @param source the source name. The value is what is displayed
     * on the main screen for that source.
     * @param sentAdd the expected number of new items sent
     * @param sentReplace the expected number of replace items sent
     * @param sentDelete the expected number of delete items sent
     * @param receivedAdd the expected number of new received items
     * @param receivedReplace the expected number of replace received items
     * @param receivedDelete the expected number of delete received items
     */
    public void CheckExchangedData(String source, int sentAdd, int sentReplace, int sentDelete,
                                   int receivedAdd, int receivedReplace, int receivedDelete);

    /**
     * This command checks the requested sync mode of the last sync.
     *
     * @param source is the source name
     * @param mode an integer representing the expected sync mode
     */
    public void CheckRequestedSyncMode(String source, int mode);

    /**
     * This command checks the alerted sync mode of the last sync.
     *
     * @param source is the source name
     * @param mode an integer representing the expected sync mode
     */
    public void CheckAlertedSyncMode(String source, int mode);


    /**
     * This command can used to simulate a contact addition. It creates an empty
     * contact in memory which will be saved as soon as the SaveContact command 
     * is called.
     * Once this command is called you shall set the contact's FirstName and
     * LastName fields via the SetContactField command before saving it.
     */
    public void CreateEmptyContact();

    /**
     * This command can used to simulate a contact update. It loads an existing
     * contact identified by the given FirstName and LastName fields.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void LoadContact(String firstName, String lastName);

    /**
     * This command can used while simulating a contact additon or update. It
     * sets the given field to the given value.
     *
     * @param fieldName is the contact's field name to edit. It can take one of
     * the following values:
     * <ul>
     *  <li>DisplayName</li>
     *  <li>FirstName</li>
     *  <li>LastName</li>
     *  <li>MiddleName</li>
     *  <li>PrefixName</li>
     *  <li>SuffixName</li>
     *  <li>NickName</li>
     *  <li>TelHome</li>
     *  <li>TelWork</li>
     *  <li>TelOther</li>
     *  <li>TelOther2</li>
     *  <li>TelCell</li>
     *  <li>TelPager</li>
     *  <li>TelFaxHome</li>
     *  <li>TelFaxWork</li>
     *  <li>TelCompany</li>
     *  <li>TelOtherFax</li>
     *  <li>EmailHome</li>
     *  <li>EmailWork</li>
     *  <li>EmailOther</li>
     *  <li>Im</li>
     *  <li>AddressOther: formatted as post-office;ext-address;street;city;
     *  state;cap;country</li>
     *  <li>AddressHome: formatted as post-office;ext-address;street;city;
     *  state;cap;country</li>
     *  <li>AddressWork: formatted as post-office;ext-address;street;city;
     *  state;cap;country</li>
     *  <li>Website</li>
     *  <li>WebsiteHome</li>
     *  <li>WebsiteWork</li>
     *  <li>Birthday: formatted as yyyymmdd yyyy-mm-dd or yyyy/mm/dd</li>
     *  <li>Anniversary: formatted as yyyymmdd yyyy-mm-dd or yyyy/mm/dd</li>
     *  <li>Children</li>
     *  <li>Spouse</li>
     *  <li>Title</li>
     *  <li>Organization: formatted as company;department</li>
     *  <li>Note</li>
     *  <li>Photo</li>
     * </ul>
     * @param value is field value to set
     */
    public void SetContactField(String fieldName, String value);

    /**
     * This command can used to simulate a contact update. It empties the value
     * of the specified field.
     *
     * @param fieldName is the contact's field name to empty.
     * @see SetContactField to see the available fields.
     */
    public void EmptyContactField(String fieldName);

    /**
     * This command can used to simulate a contact addition or update. It saves
     * the contact actually created or loaded through the CreateEmptyContact
     * and LoadContact respectively.
     */
    public void SaveContact();

    /**
     * This command can used to simulate a contact deletion. It removes from the
     * device store the contact identified by the given firstname and lastname.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void DeleteContact(String firstName, String lastName);

    /**
     * This command can used to simulate the deletion of all the contacts stored
     * in the device.
     */
    public void DeleteAllContacts();

    /**
     * This command can used to check that a new contact created on the server
     * has been correctly received by the client and has the same content of the
     * server's contact as expected.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     * @param checkContent set as true if you want to check the item content.
     */
    public void CheckNewContact(String firstName, String lastName, boolean checkContent);

    /**
     * This command can used to check that an updated contact on the server has
     * been correctly received by the client and has the same content of the
     * server's contact as expected.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     * @param checkContent set as true if you want to check the item content.
     */
    public void CheckUpdatedContact(String firstName, String lastName, boolean checkContent);

    /**
     * This command can used to check that a deleted contact on the server has
     * been correctly deleted in the client.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void CheckDeletedContact(String firstName, String lastName);

    /**
     * This command can used to check that a new contact sent to the server has
     * been correctly received and has the same content of the device's contact
     * as expected.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     * @param checkContent set as true if you want to check the item content.
     */
    public void CheckNewContactOnServer(String firstName, String lastName, boolean checkContent);

    /**
     * This command can used to check that an updated contact sent to the server
     * has been correctly received and has the same content of the device's
     * contact as expected.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     * @param checkContent set as true if you want to check the item content.
     */
    public void CheckUpdatedContactOnServer(String firstName, String lastName, boolean checkContent);

    /**
     * This command can used to check that a deleted contact sent to the server
     * has been correctly deleted by the server.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void CheckDeletedContactsOnServer(String firstName, String lastName);

    /**
     * This command can used to check that a deleted contact sent to the server
     * has been correctly deleted by the server.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param sourceName is the name of the source. The value is what is displayed
     * on the main screen for that source.
     * @param count the items count.
     */
    public void CheckItemsCount(String sourceName, int count);

    /**
     * This command can used to check the total source items count on server side.
     *
     * Remember to run RefreshServer before running check commands.
     *
     * @param sourceName is the name of the source. The value is what is displayed
     * on the main screen for that source.
     * @param count the items count.
     */
    public void CheckItemsCountOnServer(String sourceName, int count);

    /**
     * This command shall be used everytime the server shall be updated of both
     * client side or server side changes, and before any check command. It
     * refresh the server content for the specified source. If the source is not
     * specified then it will refresh all the sources.
     *
     * @param sourceName is the name of the source. The value is what is displayed
     * on the main screen for that source.
     */
    public void RefreshServer(String sourceName);

    /**
     * This command can used to simulate a contact addition on the server.
     * It creates an empty contact in memory which will be saved as soon as the
     * SaveContactOnServer command is called.
     * Once this command is called you shall set the contact's FirstName and
     * LastName fields via the SetContactField command before saving it.
     */
    public void CreateEmptyContactOnServer();

    /**
     * This command can used to simulate a contact update on the server. It
     * loads an existing contact identified by the given FirstName and LastName
     * fields.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void LoadContactOnServer(String firstName, String lastName);

    /**
     * This command can used to simulate a contact addition or update on the 
     * server. It saves the contact actually created or loaded through the
     * CreateEmptyContactOnServer and LoadContactOnServer respectively.
     */
    public void SaveContactOnServer();

    /**
     * This command can used to simulate a contact deletion on the server. It
     * removes from the server the contact identified by the given firstname and
     * lastname.
     *
     * @param firstName is the contact firstname
     * @param lastName is the contact lastname
     */
    public void DeleteContactOnServer(String firstName, String lastName);

    /**
     * This command can used to simulate the deletion of all the contacts stored
     * in the server.
     */
    public void DeleteAllContactsOnServer();
}
    

