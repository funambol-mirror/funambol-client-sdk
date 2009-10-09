/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2009 Funambol, Inc.
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

import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.PIMList;

import com.funambol.common.pim.Utils;
import com.funambol.common.pim.ParserProperty;
import com.funambol.common.pim.ParserParam;
import com.funambol.common.pim.ArrayList;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

/**
 * This class implements the ICalendarSyntaxParserListener interface in order
 * to listen all the events which happen during the iCalendar parsing process.
 * Depending on the item type (event or task) it will be filled a JSR75 PIMItem
 * (Event or ToDo), provided through the constructor.
 *
 * It includes some methods which should be implemented by a subclass in order
 * to store extended fields, not directly supported by JSR75:
 * <li>setTZID(String value);</li>
 * <li>setTZOffset(long offset);</li>
 * <li>setAllDay(boolean allday);</li>
 * <li>setTaskAlarm(int value);</li>
 * <li>addAttendee(String value);</li>
 */
public class CalendarParserListener implements ICalendarSyntaxParserListener {

    private static final String defaultCharset = ICalendar.UTF8;

    protected Utils   pimUtils  = new Utils(defaultCharset);
    protected PIMItem pimItem   = null;
    protected PIMList pimList   = null;

    protected String  tzid = null;
    protected boolean allDay = false;
    
    protected long eventStartTime = CalendarUtils.UNDEFINED_TIME;
    
    protected long alarmStartRelatedTime = CalendarUtils.UNDEFINED_TIME;
    protected long alarmEndRelatedTime =   CalendarUtils.UNDEFINED_TIME;
    
    // Used to keep track alarm infos
    protected VAlarm  alarm = null;

    /**
     * The construtor accepts a PIMItem object that will be populated of all
     * the iCalendar properties
     * @param pimItem the PIMItem object
     */
    public CalendarParserListener(PIMItem pimItem) {
        this.pimItem = pimItem;
        this.pimList = (PIMList) pimItem.getPIMList();
    }

    /**
     * Methods which should be implemented by a subclass in order to store
     * additional data, not supported by JSR75:
     *  - TZID
     *  - X-FUNAMBOL-TZ-OFFSET
     *  - ALLDAY
     *  - ATTENDEE
     *  - TASK TRIGGER
     */
    protected void setTZID(String value) { }
    protected void setTZOffset(long offset) { }
    protected void setAllDay(boolean allday) {
        if(allday) {
            // In the  standard JSR75 implementation, allday events must have
            // the same start/end datetimes
            if(pimItem.countValues(Event.END) > 0) {
                pimItem.setDate(Event.END, 0, Event.ATTR_NONE, eventStartTime);
            }
        }
    }
    protected void setTaskAlarm(VAlarm alarm) { }
    protected void addAttendee(String value) { }

    public void endEvent() throws ParseException {
        // Set Event additional data
        if(alarm != null) {
            setEventAlarm(alarm);
        }
        setAllDay(allDay);
    }

    public void endToDo() throws ParseException {
        // Set Task additional data
        if(alarm != null) {
            setTaskAlarm(alarm);
        }
    }

    public void end() {
        // Set common additional data
        setTZID(tzid);
    }

    /** Unused methods **/
    public void start() { }
    public void addProperty(ParserProperty property) throws ParseException { }
    public void startEvent() throws ParseException { }
    public void startToDo() throws ParseException { }
    public void addAlarm() throws ParseException { }
    public void endAlarm() throws ParseException { }

    public void addEventProperty(ParserProperty property) throws ParseException {

        Log.trace("[CalendarParserListener.addEventProperty] " + property.getName());

        String name  = property.getName();
        String value = getClearValue(property);

        if(pimItem instanceof Event) {
            if (StringUtil.equalsIgnoreCase(ICalendar.SUMMARY, name)) {
                if (pimList.isSupportedField(Event.SUMMARY)) {
                    setSummary(Event.SUMMARY, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.CLASS, name)) {
                if (pimList.isSupportedField(Event.CLASS)) {
                    setClass(Event.CLASS, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DESCRIPTION, name)) {
                if (pimList.isSupportedField(Event.NOTE)) {
                    setNote(Event.NOTE, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.UID, name)) {
                if (pimList.isSupportedField(Event.UID)) {
                    setUID(Event.UID, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.LAST_MODIFIED, name)) {
                if (pimList.isSupportedField(Event.REVISION)) {
                    setRevision(Event.REVISION, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.LOCATION, name)) {
                if (pimList.isSupportedField(Event.LOCATION)) {
                    setLocation(value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DTSTART, name)) {
                updateAllDay(property.getParameters());
                String tz = getParameter(property.getParameters(), ICalendar.TZID);
                updateTZID(tz);
                if (pimList.isSupportedField(Event.START)) {
                    setStart(value, tz);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DTEND, name)) {
                updateAllDay(property.getParameters());
                String tz = getParameter(property.getParameters(), ICalendar.TZID);
                updateTZID(tz);
                if (pimList.isSupportedField(Event.END)) {
                    setEnd(value, tzid);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DURATION, name)) {
                Log.error("[CalendarParserListener.addEventProperty] " +
                        "Duration property not supported, cannot convert ISO 8601 duration");
                throw new ParseException("Duration property not supported");
            } else if (StringUtil.equalsIgnoreCase(ICalendar.ATTENDEE, name)) {
                addAttendee(value);
            } else if (StringUtil.equalsIgnoreCase(ICalendar.X_FUNAMBOL_TZ_OFFSET, name)) {
                try {
                    setTZOffset(Long.parseLong(value));
                } catch(Exception e) {
                    Log.error("Cannot convert timezone offset: " + value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.RRULE, name)) {
                setRRULE(value);
            } else {
                Log.error("[CalendarParserListener.addEventProperty] Unsupported property: " + name);
            }
        }
        else {
            Log.error("[CalendarParserListener.addEventProperty] Found an VEVENT property into a VTODO item");
            throw new ParseException("Found an VEVENT property into a VTODO item");
        }
    }

    public void addToDoProperty(ParserProperty property) throws ParseException {
        Log.trace("[CalendarParserListener.addToDoProperty] " + property.getName());

        String name  = property.getName();
        String value = getClearValue(property);

        if(pimItem instanceof ToDo) {
            if (StringUtil.equalsIgnoreCase(ICalendar.SUMMARY, name)) {
                if (pimList.isSupportedField(ToDo.SUMMARY)) {
                    setSummary(ToDo.SUMMARY, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.CLASS, name)) {
                if (pimList.isSupportedField(ToDo.CLASS)) {
                    setClass(ToDo.CLASS, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DESCRIPTION, name)) {
                if (pimList.isSupportedField(ToDo.NOTE)) {
                    setNote(ToDo.NOTE, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.UID, name)) {
                if (pimList.isSupportedField(ToDo.UID)) {
                    setUID(ToDo.UID, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.LAST_MODIFIED, name)) {
                if (pimList.isSupportedField(ToDo.REVISION)) {
                    setRevision(ToDo.REVISION, value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.STATUS, name)) {
                if (pimList.isSupportedField(ToDo.COMPLETED)) {
                    setStatus(value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.COMPLETED, name)) {
                String tz = getParameter(property.getParameters(), ICalendar.TZID);
                updateTZID(tz);
                if (pimList.isSupportedField(ToDo.COMPLETION_DATE)) {
                    setCompleted(value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DUE, name)) {
                updateAllDay(property.getParameters());
                String tz = getParameter(property.getParameters(), ICalendar.TZID);
                updateTZID(tz);
                if (pimList.isSupportedField(ToDo.DUE)) {
                    setDue(value, tzid);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.DURATION, name)) {
                Log.error("[CalendarParserListener.addToDoProperty] " +
                        "Duration property not supported, cannot convert ISO 8601 duration");
                throw new ParseException("Duration property not supported");
            } else if (StringUtil.equalsIgnoreCase(ICalendar.PRIORITY, name)) {
                if (pimList.isSupportedField(ToDo.PRIORITY)) {
                    setPriority(value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.X_FUNAMBOL_TZ_OFFSET, name)) {
                try {
                    setTZOffset(Long.parseLong(value));
                } catch(Exception e) {
                    Log.error("Cannot convert timezone offset: " + value);
                }
            } else if (StringUtil.equalsIgnoreCase(ICalendar.RRULE, name)) {
                setRRULE(value);
            } else {
                Log.error("[CalendarParserListener.addToDoProperty] Unsupported property: " + name);
            }
        }
        else {
            Log.error("[CalendarParserListener.addToDoProperty] Found an VTODO property into a VEVENT item");
            throw new ParseException("Found an VEVENT property into a VTODO item");
        }
    }

    public void startAlarm() throws ParseException {
        alarm = new VAlarm();
    }
    public void addAlarmProperty(ParserProperty property) throws ParseException {
        Log.trace("[CalendarParserListener.addAlarmProperty] " + property.getName());
        String name  = property.getName();
        String value  = property.getValue();
        if (StringUtil.equalsIgnoreCase(ICalendar.TRIGGER, name)) {
            updateTrigger(property);
        } else if(StringUtil.equalsIgnoreCase(ICalendar.ACTION, name)) {
            if(!StringUtil.equalsIgnoreCase(ICalendar.ACTION_AUDIO, value)) {
                // we support only AUDIO action type (as the funambol server does),
                // in order to format the same action for outgoing items
                Log.error("[CalendarParserListener.addAlarmProperty] Unsupported alarm action: " + value);
                throw new ParseException("Unsupported alarm action: " + value);
            }
        }else {
            Log.error("[CalendarParserListener.addAlarmProperty] Unsupported alarm property: " + name);
        }
    }

    private void updateTrigger(ParserProperty property) throws ParseException {

        // The trigger type could be DURATION (default) or DATE-TIME
        // JSR75 accepts only DURATION alarms in seconds. 
        String type = getParameter(property.getParameters(), ICalendar.VALUE);
        String related = getParameter(property.getParameters(), ICalendar.RELATED);
        alarm.setTriggerRelated(related);
        if(StringUtil.equalsIgnoreCase(type, ICalendar.DATE_TIME_VALUE)) {
            alarm.setTriggerAbsoluteTime(property.getValue());
        } else {
            alarm.setTriggerRelativeTime(property.getValue());
        }
    }

    /**
      *  In the default implementation timezones are not supported because
      *  JSR75 is timezoneless. These methods can be extended on platforms
      *  where the timezone can be somehow supported.
      *  The only thing we can do here is to save the TZID field value,
      *  in order to keep the timezone info.
      *
      **/

    public void startTimezone() throws ParseException { }
    public void endTimezone() throws ParseException { }
    public void addTimezoneStandardC() throws ParseException {}
    public void addTimezoneDayLightC() throws ParseException { }
    public void startTimezoneStandardC() throws ParseException { }
    public void endTimezoneStandardC() throws ParseException { }
    public void addStandardCProperty(ParserProperty property) throws ParseException { }
    public void startTimezoneDayLightC() throws ParseException { }
    public void endTimezoneDayLightC() throws ParseException { }
    public void addDayLightCProperty(ParserProperty property) throws ParseException { }
    
    public void addTimezoneProperty(ParserProperty property) throws ParseException {
        Log.trace("[CalendarParserListener.addTimezoneProperty]");
        String name  = property.getName();
        String value = property.getValue();
        if (StringUtil.equalsIgnoreCase(ICalendar.TZID, name)) {
            tzid = value;
        }
    }

    /************************ End listener methods ****************************/
    
    /***
     * Set common properties:
     *    - SUMMARY
     *    - REVISION
     *    - NOTE
     *    - CLASS
     *    - UID
     *    - RRULE
     ***/
    protected void setSummary(int pimField, String value) throws ParseException {
        Log.trace("[CalendarParserListener.setSummary]");
        pimItem.addString(pimField, 0, value);
    }
    protected void setRevision(int pimField, String value) throws ParseException {
        Log.trace("[CalendarParserListener.setRevision]");
        if(pimItem.countValues(pimField) == 0) {
            pimItem.addDate(pimField, 0, CalendarUtils.getLocalDateTime(value, "GMT"));
        }
    }
    protected void setNote(int pimField, String value) throws ParseException {
        Log.trace("[CalendarParserListener.setNote]");
        pimItem.addString(pimField, 0, value);
    }
    protected void setClass(int pimField, String value) throws ParseException {
        Log.trace("[CalendarParserListener.setClass]");
        int classValue;
        if (StringUtil.equalsIgnoreCase(value, ICalendar.CLASS_PRIVATE)) {
            classValue = (pimItem instanceof Event) ? Event.CLASS_PRIVATE:
                                                      ToDo.CLASS_PRIVATE;
        } else if (StringUtil.equalsIgnoreCase(value, ICalendar.CLASS_CONFIDENTIAL)) {
            classValue = (pimItem instanceof Event) ? Event.CLASS_CONFIDENTIAL:
                                                      ToDo.CLASS_CONFIDENTIAL;
        } else {
            classValue = (pimItem instanceof Event) ? Event.CLASS_PUBLIC:
                                                      ToDo.CLASS_PUBLIC;
        }
        pimItem.addInt(pimField, 0, classValue);
    }
    protected void setUID(int pimField, String value) throws ParseException {
        Log.trace("[CalendarParserListener.setUID]");
        pimItem.addString(pimField, 0, value);
    }
    protected void setRRULE(String value) throws ParseException {
        Log.trace("[CalendarParserListener.setRRULE]");
        // TBD
    }
    
    /***
     * Set Event specific properties:
     *    - LOCATION
     *    - DTSTART
     *    - DTEND
     *    - TRIGGER
     ***/
    protected void setLocation(String value) throws ParseException {
        Log.trace("[CalendarParserListener.setLocation]");
        pimItem.addString(Event.LOCATION, Event.ATTR_NONE, value);
    }
    protected void setStart(String value, String tzid) throws ParseException {
        Log.trace("[CalendarParserListener.setStart]");
        eventStartTime = CalendarUtils.getLocalDateTime(value, tzid);
        alarmStartRelatedTime = eventStartTime;
        pimItem.addDate(Event.START, Event.ATTR_NONE, eventStartTime);
    }
    protected void setEnd(String value, String tzid) throws ParseException {
        Log.trace("[CalendarParserListener.setEnd]");
        long eventEndTime = CalendarUtils.getLocalDateTime(value, tzid);
        alarmEndRelatedTime = eventEndTime;
        pimItem.addDate(Event.END, Event.ATTR_NONE, eventEndTime);
    }
    protected void setEventAlarm(VAlarm al) throws ParseException {
        Log.trace("[CalendarParserListener.setEventAlarm]");
        if (pimList.isSupportedField(Event.ALARM)) {
            al.setCalStartAbsoluteTime(alarmStartRelatedTime);
            al.setCalEndAbsoluteTime(alarmEndRelatedTime);
            int interval = (int)al.getAlarmInterval()/1000;
            if(interval != CalendarUtils.UNDEFINED_TIME) {
                pimItem.addInt(Event.ALARM, 0, interval);
            }
        }
    }

    /***
     * Set Task specific properties:
     *    - STATUS
     *    - COMPLETED (completion date)
     *    - DUE
     *    - PRIORITY
     ***/
    protected void setStatus(String value) {
        Log.trace("[CalendarParserListener.setStatus]");
        boolean completed = StringUtil.equalsIgnoreCase(value, ICalendar.STATUS_COMPLETED);
        pimItem.addBoolean(ToDo.COMPLETED, ToDo.ATTR_NONE, completed);
    }
    protected void setCompleted(String value) throws ParseException {
        Log.trace("[CalendarParserListener.setCompleted]");
        pimItem.addDate(ToDo.COMPLETION_DATE, ToDo.ATTR_NONE, CalendarUtils.getLocalDateTime(value, "GMT"));
    }
    protected void setDue(String value, String tzid) throws ParseException {
        Log.trace("[CalendarParserListener.setDue]");
        alarmStartRelatedTime = eventStartTime;
        pimItem.addDate(ToDo.DUE, ToDo.ATTR_NONE, CalendarUtils.getLocalDateTime(value, tzid));
    }
    protected void setPriority(String value) throws ParseException {
        Log.trace("[CalendarParserListener.setPriority]");
        pimItem.addInt(ToDo.PRIORITY, ToDo.ATTR_NONE, Integer.parseInt(value));
    }

    /**
     * Update the allday property, depending on the value type
     * @param params
     */
    private void updateAllDay(ArrayList params) {
        String valueType = getParameter(params, ICalendar.VALUE);
        allDay = StringUtil.equalsIgnoreCase(valueType, ICalendar.DATE_VALUE);
    }

    /**
     * Update the current tzid property value
     * @param params
     * @throws com.funambol.common.pim.icalendar.ParseException
     */
    private void updateTZID(String newTZID) throws ParseException  {
        if(tzid != null && newTZID != null && !StringUtil.equalsIgnoreCase(tzid, newTZID)) {
            Log.error("[CalendarParserListener.updateTZID] There are different TZID values on the same item");
            throw new ParseException("There are different TZID values on the same item");
        }
        tzid = newTZID;
    }

    /*** Common utility methods **/
    
    /**
     * Get the clear value from ParserProperty: unfolded, decoded, unescaped
     * @param property
     * @return the clear value
     */
    private String getClearValue(ParserProperty property) {

        String enc = getEncoding(property.getParameters());
        String charset = getCharset(property.getParameters());
        String value = property.getValue();

        value = pimUtils.unfold(value);
        value = pimUtils.decode(value, enc, charset);
        value = pimUtils.unescape(value);
        return value;
    }

    /**
     * Get a parameter value from the specified params array
     * @param params the array of the params
     * @param paramName the param name
     * @return the param value
     */
    private String getParameter(ArrayList params, String paramName) {
        for (int i=0;i<params.size();++i) {
            ParserParam param = (ParserParam) params.elementAt(i);
            if (StringUtil.equalsIgnoreCase(paramName, param.getName())) {
                return param.getValue();
            }
        }
        return null;
    }
    private String getEncoding(ArrayList params) {
        return getParameter(params, ICalendar.ENCODING);
    }
    private String getCharset(ArrayList params) {
        return getParameter(params, ICalendar.CHARSET);
    }
}
