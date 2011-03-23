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

package com.funambol.client.test.basic;

import java.util.Vector;

import com.funambol.client.test.ClientTestException;
import com.funambol.client.test.CommandRunner;
import com.funambol.client.test.IgnoreScriptException;
import com.funambol.client.test.Robot;

import com.funambol.util.Log;

/**
 * Implementation of the CommandRunner class that define the full commands set
 * available to the tester in order to create automatic test script.
 */
public abstract class BasicCommandRunner extends CommandRunner implements BasicUserCommands {

    private static final String TAG_LOG = "BasicCommandRunner";
    
    // Key events used by KeyPress command
    public static final String DOWN_KEY_NAME  = "KeyDown";
    public static final String UP_KEY_NAME    = "KeyUp";
    public static final String LEFT_KEY_NAME  = "KeyLeft";
    public static final String RIGHT_KEY_NAME = "KeyRight";
    public static final String FIRE_KEY_NAME  = "KeyFire";
    public static final String MENU_KEY_NAME  = "KeyMenu";
    public static final String BACK_KEY_NAME  = "KeyBack";
    public static final String DEL_KEY_NAME   = "KeyDelete";

    public String currentTestName;

    private String globalFilterByName = null;
    private String globalFilterBySourceType = null;
    private String globalFilterByDirection = null;
    private String globalFilterByLocality = null;

    /**
     * Constructor
     * @param robot the BasicRobot object that runs the commands on the given
     * client implementation. This robot should be defined into the high level
     * as it is architecture specific.
     */
    public BasicCommandRunner(BasicRobot robot, String filterByName, String filterBySourceType,
                              String filterByDirection, String filterByLocality)
    {
        super(robot);
        globalFilterByName = filterByName;
        globalFilterBySourceType = filterBySourceType;
        globalFilterByDirection = filterByDirection;
        globalFilterByLocality = filterByLocality;

        if (globalFilterByName == null) {
            globalFilterByName = "*";
        }
        if (globalFilterBySourceType == null) {
            globalFilterBySourceType = "*";
        }
        if (globalFilterByDirection == null) {
            globalFilterByDirection = "*";
        }
        if (globalFilterByLocality == null) {
            globalFilterByLocality = "*";
        }
    }

    /**
     * Core method of this class. It parses the command line command and
     * arguments to realize the actions defined in the high level provided
     * script
     * @param command the String formatted command to be parsed and given to the
     * robot that is defined to execute it
     * @param pars the command string formatted arguments
     * @return boolean true if the command is valid, false otherwise
     * @throws Throwable if anything goes wrong when the command is run
     */
    public boolean runCommand(String command, Vector pars) throws Throwable {

        if (WAIT_COMMAND.equals(command)) {
            wait(command, pars);
        } else if (BEGIN_TEST_COMMAND.equals(command)) {
            beginTest(command, pars);
        } else if (END_TEST_COMMAND.equals(command)) {
            endTest(command, pars);
        } else if (KEY_PRESS_COMMAND.equals(command)) {
            keyPress(command, pars);
        } else if (WRITE_STRING_COMMAND.equals(command)) {
            writeString(command, pars);
        } else if (WAIT_FOR_SYNC_TO_COMPLETE_COMMAND.equals(command)) {
            waitForSyncToComplete(command, pars);
        } else if (WAIT_FOR_AUTH_TO_COMPLETE_COMMAND.equals(command)) {
            waitForAuthToComplete(command, pars);
        } else if (CHECK_EXCHANGED_DATA_COMMAND.equals(command)) {
            checkExchangedData(command, pars);
        } else if (CHECK_SYNC_ERRORS_COMMAND.equals(command)) {
            checkSyncErrors(command, pars);
        } else if (CHECK_SYNC_STATUS_CODE_COMMAND.equals(command)) {
            checkSyncStatusCode(command, pars);
        } else if (CHECK_RESUMED_DATA_COMMAND.equals(command)) {
            checkResumedData(command, pars);
        } else if (CHECK_REQUESTED_SYNC_MODE_COMMAND.equals(command)) {
            checkLastSyncRequestedSyncMode(command, pars);
        } else if (CHECK_ALERTED_SYNC_MODE_COMMAND.equals(command)) {
            checkLastSyncAlertedSyncMode(command, pars);
        } else if (CHECK_REMOTE_URI_COMMAND.equals(command)) {
            checkLastSyncRemoteUri(command, pars);
        }  else if (CHECK_LAST_NOTIFICATION.equals(command)) {
            checkLastNotification(command, pars);
        } else if (FORCE_SLOW_SYNC_COMMAND.equals(command)) {
            resetSourceAnchor(command, pars);
        } else if (START_MAIN_APP_COMMAND.equals(command)) {
            startMainApp(command, pars);
        } else if (CLOSE_MAIN_APP_COMMAND.equals(command)) {
            closeMainApp(command, pars);
        } else if (INTERRUPT_SYNC_AFTER_PHASE_COMMAND.equals(command)) {
            interruptSyncAfterPhase(command, pars);
        } else if (SET_DEVICE_DATE.equals(command)) {
            //setDeviceTime(command, pars);
        } else if (RESET_FIRST_RUN_TIMESTAMP.equals(command)) {
            resetFirstRunTimestamp(command, pars);
        } else {
            return false;
        }
        return true;
    }

    /**
     * Return the name of the test that is executing.
     * @return String the String formatted test name
     */
    protected String getCurrentTestName() {
        return currentTestName;
    }

    /**
     * Accessor method
     * @return BasicRobot the BasicRobot instance reference.
     */
    protected BasicRobot getBasicRobot() {
        return (BasicRobot)robot;
    }

    /**
     * The automatic test common method to start the main application.
     * See implementation for further details.
     * @param command the String formatted command to be executed
     * @param args the command related and String formatted arguments
     * @throws Throwable if anything goes wrong when the application starts.
     */
    protected void startMainApp(String command, Vector args) throws Throwable {
        getBasicRobot().startMainApp();
    }

    /**
     * The automatic test common method to close the main application.
     * See implementation for further details.
     * @param command the String formatted command to be executed
     * @param args the command related and String formatted arguments
     * @throws Throwable if anything goes wrong when the application starts.
     */
    protected void closeMainApp(String command, Vector args) throws Throwable {
        getBasicRobot().closeMainApp();
    }
    
    /**
     * Uses the SyncMonitor object to wait that for specific sync action and
     * validate it as completed after a given amount of time. Use the BasicRobot
     * to perform the sync action.
     * @param command the String representation of the command
     * @param args the command's related and String formatted arguments.
     * In particular the script commad must contain the sync startup time and
     * the maximum time for the sync to be completed.
     * @throws Throwable if anything went wrong during the sync
     */
    protected void waitForSyncToComplete(String command, Vector args) throws Throwable {

        String minStart = getParameter(args, 0);
        String maxWait  = getParameter(args, 1);

        checkArgument(minStart, "Missing min start in " + command);
        checkArgument(maxWait, "Missing max wait in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        int min = Integer.parseInt(minStart)*1000;
        int max = Integer.parseInt(maxWait)*1000;

        getBasicRobot().waitForSyncToComplete(min, max, syncMonitor);
    }

    /**
     * a command that interrupts the current sync action
     * @param command the command related to the cancel sync action
     * @param args the command related and String formatted arguments
     * @throws Throwable if an error occurred
     */
    private void interruptSyncAfterPhase(String command, Vector args) throws Throwable {

        String phase  = getParameter(args, 0);
        String num    = getParameter(args, 1);
        String reason = getParameter(args, 2);

        checkArgument(phase, "Missing phase name in " + command);
        checkArgument(num, "Missing num in " + command);
        checkArgument(reason, "Missing reason in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        int n = Integer.parseInt(num);

        getBasicRobot().interruptSyncAfterPhase(phase, n, reason, syncMonitor);
    }

    /**
     * This command is referred to the authentication action to be performed
     * from the client's login screen. Wait that the login operation is
     * correctly performed after a given amount of time. (whatever it means:
     * config sync or simple authentication)
     * @param command the command related to the authentication action
     * @param args the command related and String formatted arguments. In
     * particular the startup action time and the maximum allowed time to
     * complete the operation must be given as parameters to this command by
     * the tester in the input script.
     * @throws Throwable if an error occurred
     */
    private void waitForAuthToComplete(String command, Vector args) throws Throwable {

        String minStart = getParameter(args, 0);
        String maxWait  = getParameter(args, 1);

        checkArgument(minStart, "Missing min start in " + command);
        checkArgument(maxWait, "Missing max wait in " + command);

        checkObject(authSyncMonitor, "Run StartMainApp before command: " + command);

        int min = Integer.parseInt(minStart)*1000;
        int max = Integer.parseInt(maxWait)*1000;

        getBasicRobot().waitForAuthToComplete(min, max, authSyncMonitor);
    }

    /**
     * Begin test declaration command. Useful to address the current test name
     * @param command the String fomatted command to declare that a test was
     * begun
     * @param args the command String formatted parameter. In particular the
     * tester must provide the test name when declaring the begin of a test
     * @throws Throwable if an error occurred
     */
    private void beginTest(String command, Vector args) throws Throwable {

        currentTestName = getParameter(args, 0);
        if (currentTestName == null) {
            Log.error(TAG_LOG, "Syntax error in script, missing test name in begin "
                               + BEGIN_TEST_COMMAND);
        } else {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Starting test " + currentTestName);
            }
        }

        // Grab the test filters if they are set
        Vector testFilters = new Vector();
        String filter;
        int i = 1;
        do {
            filter = getParameter(args, i++);
            if (filter != null) {
                testFilters.addElement(filter);
            }
        } while(filter != null);

        // Now check if this test shall be executed
        // The filters are defined this way:
        // 1) filter by name
        // 2) filter by source type
        // 3) filter by direction
        // 4) filter by locality type
        if (testFilters.size() == 4) {
            String byName = ((String)testFilters.elementAt(0)).trim();
            String bySourceType = ((String)testFilters.elementAt(1)).trim();
            String byDirection = ((String)testFilters.elementAt(2)).trim();
            String byLocality = ((String)testFilters.elementAt(3)).trim();

            if (!globalFilterByName.equals("*") && !byName.equals("*")) {
                if (!globalFilterByName.equals(byName)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Ignoring script " + currentTestName);
                        Log.info(TAG_LOG, "because byName is " + byName + " and globalFilterByName is " + globalFilterByName);
                    }
                    throw new IgnoreScriptException();
                }
            }

            if (!globalFilterBySourceType.equals("*") && !bySourceType.equals("*")) {
                if (!globalFilterBySourceType.equals(bySourceType)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Ignoring script " + currentTestName);
                        Log.info(TAG_LOG, "because bySourceType is " + bySourceType + " and globalFilterBySourceType is " + globalFilterBySourceType);
                    }
                    throw new IgnoreScriptException();
                }
            }

            if (!globalFilterByDirection.equals("*") && !byDirection.equals("*")) {
                if (!globalFilterByDirection.equals(byDirection)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Ignoring script " + currentTestName);
                        Log.info(TAG_LOG, "because byDirection is " + byDirection + " and globalFilterByDirection is " + globalFilterByDirection);
                    }
                    throw new IgnoreScriptException();
                }
            }

            if (!globalFilterByLocality.equals("*") && !byLocality.equals("*")) {
                if (!globalFilterByLocality.equals(byLocality)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Ignoring script " + currentTestName);
                        Log.info(TAG_LOG, "because byLocality is " + byLocality + " and globalFilterByLocality is " + globalFilterByLocality);
                    }
                    throw new IgnoreScriptException();
                }
            }
        } else {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Test has invalid filters, execute it " + currentTestName);
            }
        }


    }

    /**
     * End test declaration command.
     * @param command the end test related Stirng formatted representation
     * @param args the command's related String arguments. Not required for this
     * command
     * @throws Throwable if an error occurred
     */
    private void endTest(String command, Vector args) throws Throwable {
        currentTestName = null;
    }

    /**
     * checks the final data exchanged after a sync
     * @param command the check data command related Stirng formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkExchangedData(String command, Vector args) throws Throwable {

        String source          = getParameter(args, 0);
        String sentAdd         = getParameter(args, 1);
        String sentReplace     = getParameter(args, 2);
        String sentDelete      = getParameter(args, 3);
        String receivedAdd     = getParameter(args, 4);
        String receivedReplace = getParameter(args, 5);
        String receivedDelete  = getParameter(args, 6);

        checkArgument(source, "Missing source name in " + command);
        checkArgument(sentAdd, "Missing sentAdd in " + command);
        checkArgument(sentReplace, "Missing sentReplace in " + command);
        checkArgument(sentDelete, "Missing sentDelete in " + command);
        checkArgument(receivedAdd, "Missing receivedAdd in " + command);
        checkArgument(receivedReplace, "Missing receivedReplace in " + command);
        checkArgument(receivedDelete, "Missing receivedDelete in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncExchangedData(source, Integer.parseInt(sentAdd), Integer.parseInt(sentReplace),
                                         Integer.parseInt(sentDelete), Integer.parseInt(receivedAdd),
                                         Integer.parseInt(receivedReplace), Integer.parseInt(receivedDelete),
                                         syncMonitor);
    }
    
    /**
     * checks the final data exchanged after a sync
     * @param command the check data command related Stirng formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkSyncErrors(String command, Vector args) throws Throwable {

        String source          = getParameter(args, 0);
        String sendingErrors   = getParameter(args, 1);
        String receivingErrors = getParameter(args, 2);

        checkArgument(source, "Missing source name in " + command);
        checkArgument(sendingErrors, "Missing sendingErrors in " + command);
        checkArgument(receivingErrors, "Missing receivingErrors in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncErrors(source,
                Integer.parseInt(sendingErrors),
                Integer.parseInt(receivingErrors),
                syncMonitor);
    }

    /**
     * Checks the final status code after a sync
     * @param command the check data command related String formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkSyncStatusCode(String command, Vector args) throws Throwable {

        String source          = getParameter(args, 0);
        String code            = getParameter(args, 1);

        checkArgument(source, "Missing source name in " + command);
        checkArgument(code, "Missing code in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncStatusCode(source, Integer.parseInt(code),
                                         syncMonitor);
    }
    
    /**
     * Checks the last notification's content
     * 
     * @param command the check data command related String formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkLastNotification(String command, Vector args) throws Throwable {

        String id          = getParameter(args, 0);
        if (id == null) {
            id = "-1";
        }
        String severity    = getParameter(args, 1);
        if (severity == null) {
            severity = "-1";
        }
        String ticker      = getParameter(args, 2);
        if (ticker.length() == 0) {
            ticker = null;
        }
        String title       = getParameter(args, 3);
        if (title.length() == 0) {
            title = null;
        }
        String message     = getParameter(args, 4);
        if (message.length() == 0) {
            message = null;
        }
        
        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastNotification(
                Integer.parseInt(id), 
                Integer.parseInt(severity),
                ticker,
                title,
                message);
    }

    /**
     * Checks the resumed data of the last sync.
     * @param command the check data command related Stirng formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkResumedData(String command, Vector args) throws Throwable {

        String source          = getParameter(args, 0);
        String sentResumed     = getParameter(args, 1);
        String receivedResumed = getParameter(args, 2);

        checkArgument(source, "Missing source name in " + command);
        checkArgument(sentResumed, "Missing sentResumed in " + command);
        checkArgument(receivedResumed, "Missing receivedResumed in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncResumedData(source, Integer.parseInt(sentResumed),
                Integer.parseInt(receivedResumed), syncMonitor);
    }

    /**
     * Check that the latest sync request was requested with a specific sync
     * mode
     * @param command the check sync mode command related String formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkLastSyncRequestedSyncMode(String command, Vector args) throws Throwable {

        String source = getParameter(args, 0);
        String mode = getParameter(args, 1);

        checkArgument(source, "Missing source in " + command);
        checkArgument(mode, "Missing mode in " + command);
        int modeValue = Integer.parseInt(mode);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncRequestedSyncMode(source, modeValue, syncMonitor);
    }

    /**
     * Check that the latest sync request was requested with a specific sync
     * alert mode
     * @param command the check sync alert command related String formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkLastSyncAlertedSyncMode(String command, Vector args) throws Throwable {

        String source = getParameter(args, 0);
        String mode = getParameter(args, 1);

        checkArgument(source, "Missing source in " + command);
        checkArgument(mode, "Missing mode in " + command);
        int modeValue = Integer.parseInt(mode);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncAlertedSyncMode(source, modeValue, syncMonitor);
    }

    /**
     * Check that the latest sync request was requested with a specific sync
     * uri
     * @param command the check sync uri command related String formatted
     * representation
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void checkLastSyncRemoteUri(String command, Vector args) throws Throwable {

        String source = getParameter(args, 0);
        String remoteUri = getParameter(args, 1);

        checkArgument(source, "Missing source in " + command);
        checkArgument(remoteUri, "Missing remoteUri in " + command);

        checkObject(syncMonitor, "Run StartMainApp before command: " + command);

        getBasicRobot().checkLastSyncRemoteUri(source, remoteUri, syncMonitor);
    }

    /**
     * Command to reset a specific source anchors in order to provoke a slow
     * sync next time
     * @param command the String formatted command to break the anchors
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void resetSourceAnchor(String command, Vector args) throws Throwable {

        String source = getParameter(args, 0);

        checkArgument(source, "Missing source in " + command);

        getBasicRobot().resetSourceAnchor(source);
    }

    /**
     * Command to simulate a keypress
     * @param command the String formatted command to simualte the key press
     * @param args the command's related String arguments.
     * @throws Throwable if an error occurred
     */
    private void keyPress(String command, Vector args) throws Throwable {

        String keyName = getParameter(args, 0);
        String count = getParameter(args, 1);

        checkArgument(keyName, "Missing key name in " + command);

        int pressCount = 1;
        if(count != null) {
            try {
                pressCount = Integer.parseInt(count);
            } catch(NumberFormatException ex) {
                throw new ClientTestException("Invalid count: " + count
                         + " in command: " + command);
            }
        }
        getBasicRobot().keyPress(keyName, pressCount);
    }

    /**
     * Command to simulate a input text action into an editable text field
     * @param command the String formatted command to edit text
     * @param args the command's arguments string formatted
     * @throws Throwable if anything goes wrong
     */
    private void writeString(String command, Vector args) throws Throwable {

        String text = getParameter(args, 0);

        checkArgument(text, "Missing string in " + command);

        getBasicRobot().writeString(text);
    }

    /**
     * A generic wait request command. Useful if the tester must wait for a
     * limited amount of time before continuing the test execution.
     * @param command the String formatted command to tell tes runner to wait
     * @param args the command's arguments string formatted
     * @throws Throwable if anything goes wrong
     */
    private void wait(String command, Vector args) throws Throwable {

        String delay = getParameter(args, 0);

        checkArgument(delay, "Missing delay in " + command);

        int d = Integer.parseInt(delay);

        if(d > 0) {
            Robot.waitDelay(d);
        } else {
            // Wait forever
            Robot.waitDelay(10000);
        }
    }

    private void resetFirstRunTimestamp(String command, Vector args) throws Throwable {
        getBasicRobot().resetFirstRunTimestamp();
    }
}
 
