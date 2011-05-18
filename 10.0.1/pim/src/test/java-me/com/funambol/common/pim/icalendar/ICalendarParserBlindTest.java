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

import java.io.InputStream;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.EventList;
import javax.microedition.pim.ToDoList;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

import com.funambol.common.pim.vcalendar.CalendarTestUtils;

import junit.framework.*; 

/**
 * This is a blind test for the iCalendar parser. It checks only the ability to
 * parse supported iCalendar items and to refuse usupported items.
 */
public class ICalendarParserBlindTest extends TestCase {

    public ICalendarParserBlindTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.TRACE);
    }
    
    public void setUp() {
        Log.setLogLevel(Log.TRACE);
    }
    
    public void tearDown() {
    }

    public void testCorrectEvents() throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug("--- testCorrectEvents ---");
        }
        EventList list = (EventList) PIM.getInstance().openPIMList(PIM.EVENT_LIST,PIM.READ_WRITE);
        Event event = list.createEvent();
        readICalendars("/res/icalendar/goodEvents.txt", event, list, false);
    }
    
    public void testBadEvents() throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug("--- testBadEvents ---");
        }
        EventList list = (EventList) PIM.getInstance().openPIMList(PIM.EVENT_LIST,PIM.READ_WRITE);
        Event event = list.createEvent();
        readICalendars("/res/icalendar/badEvents.txt", event, list, true);
    }
    
    public void testCorrectTodos() throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug("--- testCorrectTodos ---");
        }
        ToDoList list = (ToDoList) PIM.getInstance().openPIMList(PIM.TODO_LIST,PIM.READ_WRITE);
        ToDo todo = list.createToDo();
        readICalendars("/res/icalendar/goodTodos.txt", todo, list, false);
    }
    
    public void readICalendars(String filename, PIMItem item, PIMList list, boolean failOnGoodICal) throws Throwable {
        InputStream icalsStream = getClass().getResourceAsStream(filename);
        String ical;
        int total = 0;
        do {
            if(item instanceof Event)     item = ((EventList)list).createEvent();
            else if(item instanceof ToDo) item = ((ToDoList)list).createToDo();
            ical = CalendarTestUtils.getNextCalendarItem(icalsStream);
            
            if (ical.length() > 0) {
                ++total;
                try {
                    ByteArrayInputStream is = new ByteArrayInputStream(ical.getBytes());
                    ICalendarSyntaxParserListener lis = new ICalendarParserListener(item);
                    ICalendarSyntaxParser parser = new ICalendarSyntaxParser(is);
                    parser.setListener(lis);
                    parser.parse();
                    // Save the event
                    item.commit();
                    if(failOnGoodICal) {
                        if (Log.isLoggable(Log.DEBUG)) {
                            Log.debug("Failed iCalendar: " + ical);
                        }
                        assertTrue(false);
                    }
                } catch (Exception e) {
                    Log.error("Error while parsing this iCalendar");
                    Log.error(ical);
                    Log.error(e.toString());
                    if(!failOnGoodICal) {
                        if (Log.isLoggable(Log.DEBUG)) {
                            Log.debug("Failed iCalendar: " + ical);
                        }
                        assertTrue(false);
                    }
                }
            }
        } while (ical.length() > 0);
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug("Number of iCals parsed: " + total);
        }
    }
}
