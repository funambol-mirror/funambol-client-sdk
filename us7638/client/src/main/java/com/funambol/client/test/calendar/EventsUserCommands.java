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

package com.funambol.client.test.calendar;

/**
 * This component lists all the contacts-related commands available in the automatic test
 * scripting language.
 */
public interface EventsUserCommands {

    /**
     * This command can be used to simulate an event addition. It creates an empty
     * event in memory which will be saved as soon as the SaveEvent command 
     * is called.
     * Once this command is called you shall set the event's summary
     * SetEventField command before saving it.
     *
     * @example CreateEmptyEvent()
     */
    public static final String CREATE_EMPTY_EVENT_COMMAND = "CreateEmptyEvent";

    /**
     * This command can be used to simulate an event addition on the server.
     * It creates an empty event in memory which will be saved as soon as the
     * SaveEventOnServer command is called.
     * Once this command is called you shall set the event's summary
     * via the SetEventField command before saving it.
     *
     * @example CreateEmptyEventOnServer()
     */
    public static final String CREATE_EMPTY_EVENT_ON_SERVER_COMMAND = "CreateEmptyEventOnServer";

    /**
     * This command can be used to simulate an event update. It loads an existing
     * event identified by the given summary. If such an event is not found, then the test fails.
     *
     * @param summary is the event summary (must be unique)
     *
     * @example LoadEvent("Event1")
     */
    public static final String LOAD_EVENT_COMMAND = "LoadEvent";

    /**
     * This command can be used to simulate an event update on the server. It
     * loads an existing event identified by the given summary fields.
     * If such an event does not exist on server, then the test fails.
     *
     * @param summary is the event summary
     *
     * @example LoadEventOnServer("Event1")
     */
    public static final String LOAD_EVENT_ON_SERVER_COMMAND = "LoadEventOnServer";
    
    /**
     * This command can be used while simulating an event additon or update. It
     * sets the given field to the given value.
     *
     * @param fieldName is the event's field name to edit. It can take one of
     * the following values:
     * <ul>
     *  <li>Summary</li>
     *  <li>Location</li>
     *  <li>Start</li>
     *  <li>End</li>
     *  <li>AllDay</li>
     *  <li>Description</li>
     *  <li>Attendees</li>
     *  <li>Timezone</li>
     *  <li>Reminder</li>
     * </ul>
     * @param value is field value to set
     *
     * @example SetEventField("Summary", "Event1")
     */
    public static final String SET_EVENT_FIELD_COMMAND = "SetEventField";

    /**
     * This command can be used to simulate an event update. It empties the value
     * of the specified field. See SetEventField to see the available fields.
     *
     * @param fieldName is the event's field name to empty.
     *
     * @example EmptyEventField("Summary")
     */
    public static final String EMPTY_EVENT_FIELD_COMMAND = "EmptyEventField";

    /**
     * This command can be used to simulate an event addition or update. It saves
     * the event created or loaded through the CreateEmptyEvent
     * and LoadEvent respectively.
     *
     * @example SaveEvent()
     */
    public static final String SAVE_EVENT_COMMAND = "SaveEvent";

    /**
     * This command can be used to simulate an event addition or update on the
     * server. It saves the event created or loaded through the
     * CreateEmptyEventOnServer and LoadEventOnServer respectively.
     *
     * @example SaveEventOnServer()
     */
    public static final String SAVE_EVENT_ON_SERVER_COMMAND = "SaveEventOnServer";
    
    /**
     * This command can be used to simulate an event deletion. It removes from the
     * device store the event identified by the given summary.
     * If an event with the given summary is not found, then
     * the test fails.
     *
     * @param summary is the event summary
     *
     * @example DeleteEvent("Event1")
     */
    public static final String DELETE_EVENT_COMMAND = "DeleteEvent";

    /**
     * This command can be used to simulate an event deletion on the server. It
     * removes from the server the event identified by the given summary.
     * If such an event does not exist on server, then the test
     * fails.
     *
     * @param summary is the event summary
     *
     * @example DeleteEventOnServer("Event1")
     */
    public static final String DELETE_EVENT_ON_SERVER_COMMAND = "DeleteEventOnServer";
    
    /**
     * This command can be used to simulate the deletion of all the events stored
     * in the device.
     *
     * @example DeleteAllEvents()
     */
    public static final String DELETE_ALL_EVENTS_COMMAND = "DeleteAllEvents";

    /**
     * This command can used to simulate the deletion of all the events stored
     * in the server.
     *
     * @example DeleteAllEventsOnServer()
     */
    public static final String DELETE_ALL_EVENTS_ON_SERVER_COMMAND = "DeleteAllEventsOnServer";

    /**
     * This command allows to fill an event with a VCal. This is useful when
     * the user needs to generate events on the server. It is possible to
     * create an empty event, fill it with this command and save it.
     *
     * @param vcal is the event representation as vcal
     *
     * @example CreateEmptyEventOnServer() <br>
     *          SetEventAsVCal("BEGIN:VCALENDARD\r\nVERSION:1.0\r\nSUMMARY:Event1\r\nEND:VCALENDAR") <br>
     *          SaveEventOnServer()
     */
    public static final String SET_EVENT_AS_VCAL_COMMAND = "SetEventAsVCal";

    /**
     * This command simulates an item received from the server. The use of this
     * command is for local tests, where the behavior of the source must be
     * tested for incoming items. After this command is executed, there shall be
     * a new event on the device. It is possible to load such an event and
     * check for its correctness.
     *
     * @example SetEventFroSmServer("BEGIN:VCALENDAR\r\n......END:VCALENDAR\r\n");
     *          LoadEvent("Summary");
     *          CheckEventAsVCal("BEGIN:VCALENDAR\r\n.....END:VCALENDAR\r\n");
     *
     */
    public static final String SET_EVENT_FROM_SERVER = "SetEventFromServer";

    public static final String CHECK_EVENTS_COUNT_ON_SERVER_COMMAND = "CheckEventsCountOnServer";


    // These commands are used by automatically generated scripts
    public static final String CREATE_EMPTY_RAW_EVENT   = "CreateEmptyRawEvent";
    public static final String SET_RAW_EVENT_FIELD      = "SetRawEventField";
    public static final String SET_RAW_REMINDER_FIELD   = "SetRawReminderField";
    public static final String SAVE_RAW_EVENT           = "SaveRawEvent";
    public static final String CHECK_RAW_EVENT_AS_VCAL  = "CheckRawEventAsVCal";
    public static final String CHECK_RAW_EVENT_FIELD    = "CheckRawEventField";
    public static final String CHECK_RAW_REMINDER_FIELD = "CheckRawReminderField";
}
    

