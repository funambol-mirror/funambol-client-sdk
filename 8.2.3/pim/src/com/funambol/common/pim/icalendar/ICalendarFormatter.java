/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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

package com.funambol.common.pim.icalendar;

import java.io.OutputStream;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;

import com.funambol.util.QuotedPrintable;
import com.funambol.util.Log;
import com.funambol.common.pim.Utils;
import com.funambol.common.pim.PimUtils;

/**
 * This class implements an iCalendar formatter for JSR75 Event or ToDo objects.
 * This class should be extended if you want to format extended fields, not
 * directly included in the basic JSR75 implementation.
 * 
 * In particular the following methods should be implemented:
 * <li>getTZID(PIMItem pimItem);</li>
 * <li>getTZOffset(PIMItem pimItem);</li>
 * <li>getTaskAlarmInterval(PIMItem pimItem);</li>
 * <li>isAllDay(PIMItem pimItem);</li>
 * <li>formatAttendees(PIMItem pimItem, OutputStream os);</li>
 */
public class ICalendarFormatter {

    protected String defaultCharset = ICalendar.UTF8;
    protected VAlarm alarm = new VAlarm();
    protected PimUtils  pimUtils = new PimUtils(defaultCharset);

    /**
     * Create a new ICalendarFormatter using the provided default charset
     * @param defaultCharset
     */
    public ICalendarFormatter(String defaultCharset) {
        this.defaultCharset = defaultCharset;
    }

    public ICalendarFormatter() {
    }

    /**
     * Format the whole iCalendar component (VCALENDAR 2.0)
     * @param pimItem the PIMItem to format
     * @param os the output stream
     * @throws javax.microedition.pim.PIMException
     */
    public void format(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.format]");
        pimUtils.println(os, ICalendar.BEGIN_VCALENDAR);
        pimUtils.println(os, ICalendar.VERSION);
        formatTimezone(pimItem, os);
        if(pimItem instanceof Event) {
            // this is an Event
            formatEvent(pimItem, os);
        } else if(pimItem instanceof ToDo) {
            // this is a ToDo
            formatToDo(pimItem, os);
        }
        pimUtils.println(os, ICalendar.END_VCALENDAR);
    }

    /**
     * Format the VEVENT component
     * @param pimItem the PIMItem to format
     * @param os the output stream
     * @throws javax.microedition.pim.PIMException
     */
    public void formatEvent(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatEvent]");
        pimUtils.println(os, ICalendar.BEGIN_VEVENT);
        formatSummary(pimItem, Event.SUMMARY, os);
        formatLocation(pimItem, os);
        formatDTStart(pimItem, os);
        formatDTEnd(pimItem, os);
        formatNote(pimItem, Event.NOTE, os);
        formatUID(pimItem, Event.UID, os);
        formatRevision(pimItem, Event.REVISION, os);
        formatClass(pimItem, Event.CLASS, os);
        formatAttendees(pimItem, os);
        formatRRule(pimItem, os);
        formatAlarm(pimItem, os);
        pimUtils.println(os, ICalendar.END_VEVENT);
    }

    /**
     * Format the VTODO component
     * @param pimItem the PIMItem to format
     * @param os the output stream
     * @throws javax.microedition.pim.PIMException
     */
    public void formatToDo(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatToDo]");
        pimUtils.println(os, ICalendar.BEGIN_VTODO);
        formatSummary(pimItem, ToDo.SUMMARY, os);
        formatDue(pimItem, os);
        formatStatus(pimItem, os);
        formatCompleted(pimItem, os);
        formatNote(pimItem, ToDo.NOTE, os);
        formatUID(pimItem, ToDo.UID, os);
        formatRevision(pimItem, ToDo.REVISION, os);
        formatClass(pimItem, ToDo.CLASS, os);
        formatPriority(pimItem, os);
        formatRRule(pimItem, os);
        formatAlarm(pimItem, os);
        pimUtils.println(os, ICalendar.END_VTODO);
    }

    /**
     * Format the VALARM component
     * @param pimItem the PIMItem to format
     * @param os the output stream
     * @throws javax.microedition.pim.PIMException
     */
    public void formatAlarm(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatAlarm]");
        int alarmInterval = -1;
        if(pimItem instanceof Event) {
            if (isSupported(pimItem, Event.ALARM) && pimItem.countValues(Event.ALARM) > 0) {
                alarmInterval = pimItem.getInt(Event.ALARM, 0);
            }
        } else if(pimItem instanceof ToDo) {
            alarmInterval = getTaskAlarmInterval(pimItem);
        }
        if (alarmInterval >= 0) {
            if(alarm.setAlarmInterval(alarmInterval*1000)) {
                // The VALARM component has to be formatted
                pimUtils.println(os, ICalendar.BEGIN_VALARM);
                formatTrigger(pimItem, alarmInterval, os);
                // Format AUDIO ACTION type as default
                pimUtils.println(os, ICalendar.ACTION + ":" + ICalendar.ACTION_AUDIO);
                pimUtils.println(os, ICalendar.END_VALARM);
            }
        }
    }

    /**
     * Format the VTIMEZONE component
     * @param pimItem the PIMItem to format
     * @param os the output stream
     * @throws javax.microedition.pim.PIMException
     */
    public void formatTimezone(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatTimezone]");
        String tzid = getTZID(pimItem);
        if(tzid != null) {
            // The VTIMEZONE component has to be formatted
            pimUtils.println(os, ICalendar.BEGIN_VTIMEZONE);
            pimUtils.println(os, ICalendar.TZID + ":" + tzid);
            pimUtils.println(os, ICalendar.END_VTIMEZONE);
        }
    }

    /** Sigle field formatters **/

    protected void formatSummary(PIMItem pimItem, int pimField, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatSummary]");
        formatSimple(pimItem, pimField, ICalendar.SUMMARY, os, true);
    }
    protected void formatLocation(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatLocation]");
        formatSimple(pimItem, Event.LOCATION, ICalendar.LOCATION, os, true);
    }
    protected void formatDTStart(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatDTStart]");
        formatDateTime(os, pimItem, Event.START, ICalendar.DTSTART, true, true);
    }
    protected void formatDTEnd(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatDTEnd]");
        formatDateTime(os, pimItem, Event.END, ICalendar.DTEND, true, true);
    }
    protected void formatDue(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatDue]");
        formatDateTime(os, pimItem, ToDo.DUE, ICalendar.DUE, false, true);
    }
    protected void formatNote(PIMItem pimItem, int pimField, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatNote]");
        formatSimple(pimItem, pimField, ICalendar.DESCRIPTION, os, true);
    }
    protected void formatUID(PIMItem pimItem, int pimField, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatUID]");
        formatSimple(pimItem, pimField, ICalendar.UID, os, false);
    }
    protected void formatRevision(PIMItem pimItem, int pimField, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatRevision]");
        formatDateTime(os, pimItem, pimField, ICalendar.LAST_MODIFIED, false, false);
    }
    protected void formatPriority(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatPriority]");
        if(isSupported(pimItem, ToDo.PRIORITY) && pimItem.countValues(ToDo.PRIORITY) > 0) {
            int priority = pimItem.getInt(ToDo.PRIORITY, 0);
            if(priority > 0) {
                // The priority scale of JSR75 is the same of iCalendar
                pimUtils.println(os, ICalendar.PRIORITY + ":" + Integer.toString(priority));
            }
        }
    }
    protected void formatCompleted(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatCompleted]");
        formatDateTime(os, pimItem, ToDo.COMPLETION_DATE, ICalendar.COMPLETED, false, false);
    }
    protected void formatStatus(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatStatus]");
        if (isSupported(pimItem, ToDo.COMPLETED) && pimItem.countValues(ToDo.COMPLETED) > 0) {
            String status;
            if(pimItem.getBoolean(ToDo.COMPLETED, 0)) {
                status = ICalendar.STATUS_COMPLETED;
            }
            else {
                status = ICalendar.STATUS_IN_PROCESS;
            }
            pimUtils.println(os, ICalendar.STATUS + ":" + status);
        }
    }
    protected void formatClass(PIMItem pimItem, int pimField, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatClass]");
        String iCalClass = null;
        if (isSupported(pimItem, pimField) && pimItem.countValues(pimField) > 0) {
            int classValue = pimItem.getInt(pimField, 0);
            if(pimItem instanceof Event) {
                if(classValue == Event.CLASS_PUBLIC) {
                    iCalClass = ICalendar.CLASS_PUBLIC;
                } else if(classValue == Event.CLASS_PRIVATE) {
                    iCalClass = ICalendar.CLASS_PRIVATE;
                } else if(classValue == Event.CLASS_CONFIDENTIAL) {
                    iCalClass = ICalendar.CLASS_CONFIDENTIAL;
                } else {
                    Log.error("Unsupported class type: " + classValue);
                }
            } else if(pimItem instanceof ToDo) {
                if(classValue == ToDo.CLASS_PUBLIC) {
                    iCalClass = ICalendar.CLASS_PUBLIC;
                } else if(classValue == ToDo.CLASS_PRIVATE) {
                    iCalClass = ICalendar.CLASS_PRIVATE;
                } else if(classValue == ToDo.CLASS_CONFIDENTIAL) {
                    iCalClass = ICalendar.CLASS_CONFIDENTIAL;
                } else {
                    Log.error("Unsupported class type: " + classValue);
                }
            }
            if(iCalClass != null) {
                pimUtils.println(os, ICalendar.CLASS + ":" + iCalClass);
            }
        }
    }
    protected void formatRRule(PIMItem pimItem, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatRRule]");
        // TODO
    }
    protected void formatTrigger(PIMItem pimItem, int alarmInterval, OutputStream os) throws PIMException {
        Log.trace("[ICalendarFormatter.formatTrigger]"); 
        pimUtils.println(os, ICalendar.TRIGGER + ";" +
                    ICalendar.VALUE + "=" + ICalendar.DATE_TIME_VALUE + ":" +
                    alarm.getTriggerAbsoluteTime());
    }

    /**
     * Format a simple field
     * @param pimItem the PIMItem that contains the field value
     * @param pimField the PIMItem field index
     * @param iCalField the iCalendar field name
     * @param os the output stream
     * @param checkEncode check whether the field value shall be encoded
     * @throws javax.microedition.pim.PIMException
     */
    protected void formatSimple(PIMItem pimItem, int pimField, String iCalField,
                                OutputStream os, boolean checkEncode) throws PIMException {
        Log.trace("[ICalendarFormatter.formatSimple] Field: " + iCalField);
        if (isSupported(pimItem, pimField)) {
            // format the field also if it's empty
            if(pimItem.countValues(pimField) == 0) {
                pimUtils.println(os, iCalField + ":");
                return;
            }
            String value  = pimItem.getString(pimField, 0);
            value = pimUtils.escape(value,true);
            StringBuffer field = new StringBuffer(iCalField);
            if(checkEncode) {
                String encoded = null;
                if((encoded=encodeField(value)) != null) {
                    value = encoded;
                    field.append(";").append(ICalendar.ENCODING)
                         .append("=").append(ICalendar.QUOTED_PRINTABLE);
                    field.append(";").append(ICalendar.CHARSET)
                         .append("=").append(defaultCharset);
                }
            }
            value = pimUtils.fold(value);
            pimUtils.println(os, field.toString() + ":" + value);
        }
    }

    /**
     * Format a date-time field
     * @param os the output stream
     * @param pimItem the PIMItem
     * @param pimField the pim field
     * @param iCalField the iCalendar field
     * @param checkAllDay check whether the allday property shall be updated
     * @param checkTimezone check whether the TZID param shall be added, and the
     *
     * @throws javax.microedition.pim.PIMException
     */
    protected void formatDateTime(OutputStream os, PIMItem pimItem, int pimField,
            String iCalField, boolean checkAllDay, boolean checkTimezone) throws PIMException {

        Log.trace("[ICalendarFormatter.formatDateTime]");
        if(isSupported(pimItem, pimField) && pimItem.countValues(pimField) > 0) {
            long millis = pimItem.getDate(pimField, 0);
            boolean allday = checkAllDay ? isAllDay(pimItem) : false;
            if(allday) {
                iCalField += ";" + ICalendar.VALUE + "=" + ICalendar.DATE_VALUE;
                if((pimItem instanceof Event) && pimField == Event.END) {
                    millis = fixEndDate(millis);
                }
            }
            String tzid = "GMT";
            if(checkTimezone && !allday) {
                tzid = getTZID(pimItem);
                if(tzid != null) {
                    // Add the TZID param
                    iCalField += ";" + ICalendar.TZID + "=" + tzid;
                    long offset = getTZOffset(pimItem);
                    millis += offset;
                }
            }
            String dateValue = CalendarUtils.formatDateTime(millis, allday, tzid);
            pimUtils.println(os, iCalField + ":" + dateValue);
            if((pimItem instanceof Event) && pimField == Event.START) {
                alarm.setCalStartAbsoluteTime(millis);
            }
        }
    }

    /**
     * These methods should be implemented in order to support the followings fields:
     *  - TZID
     *  - X-FUNAMBOL-TZ-OFFSET
     *  - Task alarm interval
     *  - ALLDAY
     *  - ATTENDEES
     */

    protected String  getTZID(PIMItem pimItem)              { return null; }
    protected int     getTaskAlarmInterval(PIMItem pimItem) { return -1; }
    protected boolean isAllDay(PIMItem pimItem) {
        // implement the default behaviour: return true if the START and END date
        // are equals.
        // NOTE: the BlackBerryEvent implementation include the field ALLDAY
        //       which should be used for this scope
        if(pimItem instanceof Event) {
            long start = 0;
            long end = 1;
            if(isSupported(pimItem, Event.START) && pimItem.countValues(Event.START) > 0) {
                start = pimItem.getDate(Event.START, 0);
            }
            if(isSupported(pimItem, Event.END) && pimItem.countValues(Event.END) > 0) {
                end = pimItem.getDate(Event.END, 0);
            }
            if(start == end) {
                return true;
            }
        }
        return false;
    }
    protected long getTZOffset(PIMItem pimItem)  { return 0; }
    protected void formatAttendees(PIMItem pimItem, OutputStream os) throws PIMException { }

    /**
     * Add a day factor to the end date (used for allday events)
     * @param endDate
     */
    protected long fixEndDate(long endDate) {
        return endDate+CalendarUtils.DAY_FACTOR;
    }

    /**
     * Encode a field value if it requires encoding
     * @param value the field value
     * @return null if it doesn't require encoding
     * @throws javax.microedition.pim.PIMException
     */
    protected String encodeField(String value) throws PIMException {
        try {
            String qpEncoded = QuotedPrintable.encode(value, defaultCharset);
            if (qpEncoded.length() != value.length()) {
                value = qpEncoded;
                return value;
            }
            return null;
        } catch (Exception e) {
            throw new PIMException(e.toString());
        }
    }

    protected boolean isSupported(PIMItem pimItem, int pimField) {
        return pimItem.getPIMList().isSupportedField(pimField);
    }
}
