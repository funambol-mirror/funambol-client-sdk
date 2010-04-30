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

import com.funambol.client.test.*;

public class CalendarCommandRunner extends CommandRunner {

    private static final String TAG_LOG = "CalendarCommandRunner";

    // Commands
    private static final String CREATE_EMPTY_EVENT_COMMAND      = "CreateEmptyEvent";
    private static final String LOAD_EVENT_COMMAND              = "LoadEvent";
    private static final String SET_EVENT_FIELD_COMMAND         = "SetEventField";
    private static final String SET_EVENT_REC_FIELD_COMMAND     = "SetEventRecurrenceField";
    private static final String EMPTY_EVENT_FIELD_COMMAND       = "EmptyEventField";
    private static final String SAVE_EVENT_COMMAND              = "SaveEvent";
    private static final String DELETE_EVENT_COMMAND            = "DeleteEvent";
    private static final String DELETE_ALL_EVENTS_COMMAND       = "DeleteAllEvents";
    private static final String CHECK_NEW_EVENT_COMMAND         = "CheckNewEvent";
    private static final String CHECK_UPDATED_EVENT_COMMAND     = "CheckUpdatedEvent";
    private static final String CHECK_DELETED_EVENT_COMMAND     = "CheckDeletedEvent";
    private static final String CHECK_NEW_EVENT_ON_SERVER_COMMAND     = "CheckNewEventOnServer";
    private static final String CHECK_UPDATED_EVENT_ON_SERVER_COMMAND = "CheckUpdatedEventOnServer";
    private static final String CHECK_DELETED_EVENT_ON_SERVER_COMMAND = "CheckDeletedEventsOnServer";
    private static final String CREATE_EMPTY_EVENT_ON_SERVER_COMMAND  = "CreateEmptyEventOnServer";
    private static final String LOAD_EVENT_ON_SERVER_COMMAND          = "LoadEventOnServer";
    private static final String SAVE_EVENT_ON_SERVER_COMMAND          = "SaveEventOnServer";
    private static final String DELETE_EVENT_ON_SERVER_COMMAND        = "DeleteEventOnServer";
    private static final String DELETE_ALL_EVENTS_ON_SERVER_COMMAND   = "DeleteAllEventsOnServer";

    // Event fields used by SetEventField and EmptyEventField commands
    public static final String EVENT_FIELD_SUMMARY         = "Summary";
    public static final String EVENT_FIELD_LOCATION        = "Location";
    public static final String EVENT_FIELD_START           = "Start";
    public static final String EVENT_FIELD_END             = "End";
    public static final String EVENT_FIELD_ALLDAY          = "AllDay";
    public static final String EVENT_FIELD_DESCRIPTION     = "Description";
    public static final String EVENT_FIELD_ATTENDEES       = "Attendees";
    public static final String EVENT_FIELD_REMINDER        = "Reminder";
    public static final String EVENT_FIELD_TIMEZONE        = "Timezone";

    // Event Recurrence fields used by SetEventRecurrenceField command
    public static final String EVENT_REC_FIELD_FREQUENCY    = "Frequency";
    public static final String EVENT_REC_FIELD_INTERVAL     = "Interval";
    public static final String EVENT_REC_FIELD_END_DATE     = "EndDate";
    public static final String EVENT_REC_FIELD_DAY_OF_WEEK  = "DayOfWeek";
    public static final String EVENT_REC_FIELD_DAY_OF_MONTH = "DayOfMonth";
    


    
    public CalendarCommandRunner(CalendarRobot robot) {
        super(robot);
    }

    public boolean runCommand(String command, String pars) throws Throwable {

        if (CREATE_EMPTY_EVENT_COMMAND.equals(command)) {
            createEmptyEvent(command, pars);
        } else if (LOAD_EVENT_COMMAND.equals(command)) {
            loadEvent(command, pars);
        } else if (SET_EVENT_FIELD_COMMAND.equals(command)) {
            setEventField(command, pars);
        } else if (SET_EVENT_REC_FIELD_COMMAND.equals(command)){
            setEventRecurrenceField(command, pars);
        } else if (EMPTY_EVENT_FIELD_COMMAND.equals(command)) {
            emptyEventField(command, pars);
        } else if (SAVE_EVENT_COMMAND.equals(command)) {
            saveEvent(command, pars);
        } else if (DELETE_EVENT_COMMAND.equals(command)) {
            deleteEvent(command, pars);
        } else if (DELETE_ALL_EVENTS_COMMAND.equals(command)) {
            deleteAllEvents(command, pars);
        } else if (CHECK_NEW_EVENT_COMMAND.equals(command)) {
            checkNewEvent(command, pars);
        } else if (CHECK_UPDATED_EVENT_COMMAND.equals(command)) {
            checkUpdatedEvent(command, pars);
        } else if (CHECK_DELETED_EVENT_COMMAND.equals(command)) {
            checkDeletedEvent(command, pars);
        } else if (CHECK_NEW_EVENT_ON_SERVER_COMMAND.equals(command)) {
            checkNewEventOnServer(command, pars);
        } else if (CHECK_UPDATED_EVENT_ON_SERVER_COMMAND.equals(command)) {
            checkUpdatedEventOnServer(command, pars);
        } else if (CHECK_DELETED_EVENT_ON_SERVER_COMMAND.equals(command)) {
            checkDeletedEventOnServer(command, pars);
        } else if (CREATE_EMPTY_EVENT_ON_SERVER_COMMAND.equals(command)) {
            createEmptyEventOnServer(command, pars);
        } else if (LOAD_EVENT_ON_SERVER_COMMAND.equals(command)) {
            loadEventOnServer(command, pars);
        } else if (SAVE_EVENT_ON_SERVER_COMMAND.equals(command)) {
            saveEventOnServer(command, pars);
        } else if (DELETE_EVENT_ON_SERVER_COMMAND.equals(command)) {
            deleteEventOnServer(command, pars);
        } else if (DELETE_ALL_EVENTS_ON_SERVER_COMMAND.equals(command)) {
            deleteAllEventsOnServer(command, pars);
        } else {
            return false;
        }
        return true;
    }

    private CalendarRobot getCalendarRobot() {
        return (CalendarRobot)robot;
    }

    private void createEmptyEvent(String command, String args) throws Throwable {
        getCalendarRobot().createEmptyEvent();
    }

    private void createEmptyEventOnServer(String command, String args) throws Throwable {
        getCalendarRobot().createEmptyEvent();
    }

    private void setEventField(String command, String args) throws Throwable {

        String field = getParameter(args, 0);
        String value = getParameter(args, 1);

        checkArgument(field, "Missing field name in " + command);
        checkArgument(value, "Missing value in " + command);

        getCalendarRobot().setEventField(field, value);
    }
    private void setEventRecurrenceField(String command, String args) throws Throwable {

        String field = getParameter(args, 0);
        String value = getParameter(args, 1);

        checkArgument(field, "Missing recurrence field name in " + command);
        checkArgument(value, "Missing recurrence value in " + command);

        getCalendarRobot().setEventRecurrenceField(field, value);
    }

    private void emptyEventField(String command, String args) throws Throwable {

        String field = getParameter(args, 0);

        checkArgument(field, "Missing field in " + command);

        getCalendarRobot().setEventField(field, "");
    }

    private void loadEvent(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);

        checkArgument(summary, "Missing summary in " + command);

        getCalendarRobot().loadEvent(summary);
    }

    private void saveEvent(String command, String args) throws Throwable {
        getCalendarRobot().saveEvent();
    }

    private void deleteEvent(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);
        checkArgument(summary, "Missing summary in " + command);

        getCalendarRobot().deleteEvent(summary);
    }

    private void deleteAllEvents(String command, String args) throws Throwable {
        getCalendarRobot().deleteAllEvents();
    }


    private void checkNewEvent(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);
        String checkContent  = getParameter(args, 1);

        checkArgument(summary, "Missing summary in " + command);
        checkArgument(checkContent, "Missing checkContent in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkNewEvent(summary, checkSyncClient,
                parseBoolean(checkContent));
    }

    private void checkUpdatedEvent(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);
        String checkContent  = getParameter(args, 1);

        checkArgument(summary, "Missing summary in " + command);
        checkArgument(checkContent, "Missing checkContent in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkUpdatedEvent(summary, checkSyncClient,
                parseBoolean(checkContent));
    }

    private void checkDeletedEvent(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);

        checkArgument(summary, "Missing summary in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkDeletedEvent(summary, checkSyncClient);
    }


    private void checkNewEventOnServer(String command, String args) throws Throwable {

        String summary      = getParameter(args, 0);
        String checkContent  = getParameter(args, 1);

        checkArgument(summary, "Missing summary in " + command);
        checkArgument(checkContent, "Missing checkContent in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkNewEventOnServer(summary, checkSyncClient,
                parseBoolean(checkContent));
    }

    private void checkUpdatedEventOnServer(String command, String args) throws Throwable {

        String summary      = getParameter(args, 0);
        String checkContent = getParameter(args, 1);

        checkArgument(summary, "Missing summary in " + command);
        checkArgument(checkContent, "Missing checkContent in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkUpdatedEventOnServer(summary, checkSyncClient,
                parseBoolean(checkContent));
    }

    private void checkDeletedEventOnServer(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);

        checkArgument(summary, "Missing summary in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().checkDeletedEventOnServer(summary, checkSyncClient);
    }

    public void loadEventOnServer(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);

        checkArgument(summary, "Missing summary in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().loadEventOnServer(summary, checkSyncClient);
    }

    public void saveEventOnServer(String command, String args) throws Throwable {

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().saveEventOnServer(checkSyncClient);
    }

    public void deleteEventOnServer(String command, String args) throws Throwable {

        String summary = getParameter(args, 0);

        checkArgument(summary, "Missing summary in " + command);

        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().deleteEventOnServer(summary, checkSyncClient);
    }

    public void deleteAllEventsOnServer(String command, String args) throws Throwable {
        checkObject(checkSyncClient, "Run StartMainApp before command: " + command);

        getCalendarRobot().deleteAllEventsOnServer(checkSyncClient);
    }

}
 
