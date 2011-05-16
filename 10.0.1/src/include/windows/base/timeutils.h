/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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

#ifndef INCL_TIMEUTILS_WIN
#define INCL_TIMEUTILS_WIN

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */


typedef double DATE;

// Date/time definitions
#define REFERRED_MAX_DATE                   949998.000000                   /**< this is "4501-01-01" in double format: the error date of Outlook   */
#define LIMIT_MAX_DATE                      767011.000000                   /**< this is "4000-01-01" in double format: the max date accepted       */



#include "base/fscapi.h"
#include <string>
#include <list>


void   doubleToStringTime(std::wstring& stringDate, const DATE doubleDate, bool onlyDate = false);
void   stringTimeToDouble(const std::wstring& dataString, DATE* date);
void   stringDurationToEndDouble(const std::wstring& dataString, DATE* start, DATE* end);

bool   isAllDayFormat    (const std::wstring& dataString);
bool   isAllDayInterval  (const DATE startdate, const DATE enddate);

WCHAR* daysOfWeekToString(int l);
int    stringToDaysOfWeek(WCHAR* in);
int    getWeekDayFromDate(DATE date);
bool   isWeekDay         (WCHAR* data);

/**
 * Client to Server.
 * Returns the formatted date of the given year, from the passed timezone rule.
 * SYSTEMTIME structure is used to store dates, but also to store a timezone rule.
 * In the latter, the member 'wDay' is the # of week inside the month (values: 1-4),  
 * '5' means "the last week". 'wYear' is = 0 to mean "every year".
 * For example: wMonth=3, wDayOfWeek=0, wDay=2 -> means "the second Sunday of March".
 * Given the tz rule and the desired year, we can extract the date rapresented
 * by this rule, as a SYSTEMTIME, and then format it like a string "yyyyMMddThhmmss".
 *
 * @param year    the desired year to extract the date
 * @param tzRule  the timezone rule, as a SYSTEMTIME structure
 * @return        the date formatted like "yyyyMMddThhmmss"  (yyyy = year)
 */
std::wstring getDateFromTzRule(const int year, SYSTEMTIME tzRule);

/** 
 * Server to Client.
 * Returns the timezone rule as a SYSTEMTIME, from a list of dates.
 * The list of dates passed are referred to different years, so that
 * analyzing them we can calculate the tz rule and store it in a SYSTEMTIME.
 * SYSTEMTIME structure is used to store dates, but also to store a timezone rule.
 * Usually only 1 date is enough to get the timezone rule 
 * (e.g. date = 2008/10/5 -> tz rule = first Sunday of october).
 * We always suppose that if the occurrence is the 4th of the month we
 * set as the 5th due to a research in the registry of windows. There are
 * no timezone that are in the 4th week but all are in the latest week (5th)
 * 
 * @param dates   list of dates for different years, in format "yyyyMMddThhmmss" (local time)
 * @param found   if the right value is found really or guess by the rule
 * @return        the timezone rule, as a SYSTEMTIME structure
 */
SYSTEMTIME getTzRuleFromDates(std::list<std::wstring>& dates, bool *found);


/**
 * Returns a wstring with the given bias formatted like "+/-hhmm".
 * Example: bias = 330 -> "-0530"
 */
std::wstring formatBias(const int bias);

/**
 * Parse a wide-char string containing bias, like "+0130" or "-08".
 * Returns the bias as a signed integer (e.g. "+0230" -> -150)
 */
int parseBias(const WCHAR* data);

/** 
 *Returns true if the passed tz information are the same.
 * Checks bias values and dates (names are ignored).
 */
bool isSameTimezone(const TIME_ZONE_INFORMATION* tz1, const TIME_ZONE_INFORMATION* tz2);

/**
 * Returns true if the passed systemtime are the same.
 * Checks all properties, ignoring only seconds and milliseconds.
 */
bool isSameSystemtime(const SYSTEMTIME* st1, const SYSTEMTIME* st2);

/**
* Check if the timezone information has a day light saving. If all the 
* parameters (day, hours, etc... )are empty, then no daylight saving is supported.
*
* @param tz the timezone information to check
* @return true if timezone needed, false otherwise
*/
bool hasDayLightSaving(const TIME_ZONE_INFORMATION* tz) ;

/** @} */
/** @endcond */
#endif
