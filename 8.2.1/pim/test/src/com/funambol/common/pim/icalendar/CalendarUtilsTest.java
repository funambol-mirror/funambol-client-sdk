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

import java.util.Calendar;
import java.util.TimeZone;

import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

import junit.framework.*;

/**
 * This a collection of tests for the CalendarUtils methods
 */
public class CalendarUtilsTest extends TestCase {

    long date_millis = 0;
    long date_time_millis = 0;
    TimeZone timezone = TimeZone.getTimeZone("UTC");

    private final String TEST_DATE_YYYYMMDD = "20090801";
    private final String TEST_DATE_YYYY_MM_DD = "2009-08-01";

    private final String TEST_DATE_TIME = "20090801T211023Z";

    private final int YEAR_N   = 2009;
    private final int MONTH_N  = 8;
    private final int DAY_N    = 1;
    private final int HOUR_N   = 21;
    private final int MINUTE_N = 10;
    private final int SECOND_N = 23;
    
    public CalendarUtilsTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.TRACE);
    }
    
    public void setUp() {
        Log.setLogLevel(Log.TRACE);

        Calendar date = Calendar.getInstance(timezone);
        date.set(Calendar.DAY_OF_MONTH, DAY_N);
        date.set(Calendar.MONTH, MONTH_N - 1);
        date.set(Calendar.YEAR, YEAR_N);
        date.set(Calendar.HOUR_OF_DAY, 0);
        date.set(Calendar.MINUTE, 0);
        date.set(Calendar.SECOND, 0);
        date.set(Calendar.MILLISECOND, 0);
        date_millis = date.getTime().getTime();

        Calendar date_time = Calendar.getInstance(timezone);
        date_time.set(Calendar.DAY_OF_MONTH, DAY_N);
        date_time.set(Calendar.MONTH, MONTH_N - 1);
        date_time.set(Calendar.YEAR, YEAR_N);
        date_time.set(Calendar.HOUR_OF_DAY, HOUR_N);
        date_time.set(Calendar.MINUTE, MINUTE_N);
        date_time.set(Calendar.SECOND, SECOND_N);
        date_time.set(Calendar.MILLISECOND, 0);
        date_time_millis = date_time.getTime().getTime();
    }
    
    public void tearDown() {
    }
    
    public void testParseDate_yyyyMMdd() throws AssertionFailedError {
        Log.debug("--- testParseDate_yyyyMMdd ---");
        Calendar parsed = CalendarUtils.parseDate(TEST_DATE_YYYYMMDD, timezone);
        assertEquals("testParseDate_yyyyMMdd", parsed.getTime().getTime(), date_millis);
    }

    public void testParseDate_yyyy_MM_dd() throws AssertionFailedError {
        Log.debug("--- testParseDate_yyyy_MM_dd ---");
        Calendar parsed = CalendarUtils.parseDate(TEST_DATE_YYYY_MM_DD, timezone);
        assertEquals("testParseDate_yyyy_MM_dd", parsed.getTime().getTime(), date_millis);
    }

    public void testParseDateTime() throws AssertionFailedError {
        Log.debug("--- testParseDateTime ---");
        Calendar parsed = CalendarUtils.parseDateTime(TEST_DATE_TIME, timezone);
        assertEquals("testParseDateTime", parsed.getTime().getTime(), date_time_millis);
    }

    public void testParseDateTimeSHORT() throws AssertionFailedError {
        Log.debug("--- testParseDateTimeSHORT ---");
        Calendar parsed = CalendarUtils.parseDateTime(TEST_DATE_YYYYMMDD, timezone);
        assertEquals("testParseDateTime", parsed.getTime().getTime(), date_millis);
    }

    public void testGetFullInt() throws AssertionFailedError {
        Log.debug("--- testGetFullInt ---");
        assertEquals("testGetFullInt1", CalendarUtils.getFullInt(10, 2), "10");
        assertEquals("testGetFullInt2", CalendarUtils.getFullInt(1, 2), "01");
        assertEquals("testGetFullInt3", CalendarUtils.getFullInt(10, 3), "010");
    }
}

