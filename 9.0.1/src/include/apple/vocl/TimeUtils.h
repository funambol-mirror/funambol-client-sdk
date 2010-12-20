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


#include "base/util/StringBuffer.h"
#include "base/Log.h"
#include "CoreFoundation/CoreFoundation.h"
#include "vocl/Timezone.h"

BEGIN_FUNAMBOL_NAMESPACE

/**
 * Transforms a representation of a date of a vcal (YYYYMMddTHHmmss or
 * YYYYMMdd) into a NSDate taking care of the passed timezone
 */
NSDate* stringToNSdate(const char* stringdate, Timezone& timezone);

/**
 * Used to convert the date from the NSDate into a string representation.
 * isAllDay means that the format will be as YYYYMMdd
 * isRecurring means that if it is false, the time is converted in UTC and a Z is put at the end
 */
StringBuffer NSDateToString(NSDate* date, Timezone& timezone, bool isAllDay, bool isRecurring);

/**
 * Returns true if startdate = 00:00 and enddate = 23:59.
 * or if startdate = 00:00 and enddate = 00:00
 * This is an all-day-event.
 */
bool isAllDayInterval(NSDate* sdate, NSDate* edate);

/**
 * Adds years, months, days, hours, min, secs to a given date.
 * Returns the new date.
 */
NSDate* addComponentsToDate(NSDate* date, Timezone& timezone, int years, int months, int days, int hours,
							int min, int secs);

/**
 * Normalize the end day to have the end date in the format 23.59.59
 */
NSDate* normalizeEndDayForAllDay(NSDate* sdate, Timezone& tz);

/**
 * Normalize the end day of an all day event that is 23:59.59 from the apple api. It add a minutes to have the
 * day next. 
 */
NSDate* normalizeEndDayForAllDayToString(NSDate* sdate, Timezone& tz);

/**
 * Reset the hours and minutes of the date to midnight of that days. It is used
 * in the all day to be sure to get the right values
 */
NSDate* resetHoursMinSecs(NSDate* date, Timezone& tz);

/**
 * Returns a NSDateComponent from a long that represents a date
 */
NSDateComponents* getDateComponentsFromDate1970(long date);

/**
 * Returns a long representing an interval from the 1 january of 2001, 
 * given a NSDate.
 */
unsigned long getTimeIntervalSinceReferenceDate(NSDate* date);

END_FUNAMBOL_NAMESPACE
