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

#include "base/util/utils.h"
#include "base/stringUtils.h"
#include "vocl/WinRecurrence.h"
#include "vocl/constants.h"
#include <oleauto.h>
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;


// Constructor
WinRecurrence::WinRecurrence() {
    rrule = L"";
    startDate = 0;
    useTimezone = false;
}

// Constructor: fills propertyMap parsing the passed RRULE
WinRecurrence::WinRecurrence(const wstring dataString, const DATE date) {
    rrule = L"";
    startDate = date;
    useTimezone = false;
    parse(dataString);
}

// Destructor
WinRecurrence::~WinRecurrence() {
}




// Format and return a RRULE string from the propertyMap.
wstring& WinRecurrence::toString() {

    rrule = L"";

    WCHAR* days = NULL;
    WCHAR recurrence[100];
    wsprintf(recurrence, L"");

    //
    // Conversion: WinRecurrence -> RRULE.
    // -----------------------------------
    //
    // Init with default values.
    int  recType     = -1;
    int  interval    = 1;
    int  occurrences = 0;
    int  dayofweek   = 0;
    int  dayofmonth  = 0;
    int  instance    = 1;
    int  monthofyear = 0;
    bool noEnd       = false;
    wstring pStart   = L"";

    // Read all values from propertyMap.
    wstring tmp;
    getProperty(L"RecurrenceType", tmp);       recType     = _wtoi(tmp.c_str());
    getProperty(L"Interval",       tmp);       interval    = _wtoi(tmp.c_str());
    getProperty(L"Occurrences",    tmp);       occurrences = _wtoi(tmp.c_str());
    getProperty(L"DayOfWeekMask",  tmp);       dayofweek   = _wtoi(tmp.c_str());
    getProperty(L"DayOfMonth",     tmp);       dayofmonth  = _wtoi(tmp.c_str());
    getProperty(L"Instance",       tmp);       instance    = _wtoi(tmp.c_str());
    getProperty(L"MonthOfYear",    tmp);       monthofyear = _wtoi(tmp.c_str());
    getProperty(L"NoEndDate",      tmp);       noEnd       = (tmp != TEXT("0"));
    getProperty(L"PatternStartDate", pStart);
    // Note: "PatternEndDate" is no more sent: using only "Occurrences"

    replaceAll(L"-", L"", pStart);   // **** To be removed!!! ****


    switch(recType) {

        // Daily = 0
        case winRecursDaily: {
            if(interval > 0)
                wsprintf(recurrence, TEXT("D%ld #%ld"), interval, occurrences);
            else if (dayofweek > 0) {
                WCHAR* days = daysOfWeekToString(dayofweek);
                if (days) {
                    wsprintf(recurrence, TEXT("W1 %s #%ld"), days, occurrences);
                    delete [] days;
                }
            }
            break;
        }

        // Weekly = 1
        case winRecursWeekly: {
            if (dayofweek > 0) {
                days = daysOfWeekToString(dayofweek);
                if (days) {
                    if(noEnd)
                        wsprintf(recurrence, TEXT("W%ld %s #0"), interval, days);
                    else
                        wsprintf(recurrence, TEXT("W%ld %s #%ld"), interval, days, occurrences);
                    delete [] days;
                }
            }
            break;
        }
        
        // Monthly = 2
        case winRecursMonthly: {
            if(dayofmonth > 0) {
                if(noEnd)
                    wsprintf(recurrence, TEXT("MD%ld %ld #0"), interval, dayofmonth);
                else {
                    wsprintf(recurrence, TEXT("MD%ld %ld #%ld"), interval, dayofmonth, occurrences);
                }
                break;
            }
        }
        
        // MonthNth = 3
        case winRecursMonthNth: {
            if(instance>0 && dayofweek>0) {
                days = daysOfWeekToString(dayofweek);
                if(days) {
                    if(noEnd)
                        wsprintf(recurrence, TEXT("MP%ld %ld+ %s #0"), interval, instance, days);
                    else
                        wsprintf(recurrence, TEXT("MP%ld %ld+ %s #%ld"), interval, instance, days, occurrences);
                    delete [] days;
                }
            }
            break;
        }

        // Yearly = 5
        case winRecursYearly: {
            if(dayofmonth>0 && monthofyear>0) {
                if(noEnd)
                    wsprintf(recurrence, TEXT("YM1 %ld #0"), monthofyear);
                else
                    wsprintf(recurrence, TEXT("YM1 %ld #%ld"), monthofyear, occurrences);
            }
            break;
        }
        
        // YearNth = 6
        case winRecursYearNth: {
            if(dayofweek>0 && instance>0) {
                days = daysOfWeekToString(dayofweek);
                if(days) {
                    if(noEnd)
                        wsprintf(recurrence, TEXT("MP12 %ld+ %s #0"), instance, days);
                    else
                        wsprintf(recurrence, TEXT("MP12 %ld+ %s #%ld"), instance, days, occurrences);
                    delete [] days;
                }
            }
            break;
        }

        default: {
            // Error!
            break;
        }
    }

    rrule = recurrence;
    return rrule;
}




// Parse a RRULE string and fills the propertyMap.
int WinRecurrence::parse(const wstring dataString) {

    int ret = 0;
    WCHAR* str = wstrdup(dataString.c_str());
    WCHAR seps[] = TEXT(" ");
    WCHAR* token = wcstok(str, seps);

    int recType     = -1;
    int interval    =  0;
    int occurences  =  0;
    int dayOfMonth  = -1;
    int weekOfMonth = -1;
    int monthOfYear = -1;

    if (!token) {
        goto error;
    }


    //
    // First token will be: "D|W|MP|MD|YM|YD<interval>"
    //
    if(token[0] == TEXT('D')) {
        recType = winRecursDaily;
        token ++;
    }
    else if(token[0] == TEXT('W')) {
        recType = winRecursWeekly;
        token ++;
    }
    else if(token[0] == TEXT('M') && token[1] == TEXT('D')) {
        recType = winRecursMonthly;
        token += 2;
    }
    else if(token[0] == TEXT('M') && token[1] == TEXT('P')) {
        recType = winRecursMonthNth;
        token += 2;
    }
    else if(token[0] == TEXT('Y') && token[1] == TEXT('D')) {
        // "YD" Not supported!!
        LOG.info("WinRecurrence::parse - Warning: RecurrenceType 'YD' not supported: \"%ls\"", dataString.c_str());
        goto finally;
    }
    else if(token[0] == TEXT('Y') && token[1] == TEXT('M')) {
        recType = winRecursYearly;
        token += 2;
    }

    interval = _wtoi(token);
    if(!interval || recType == -1) {
        goto error;
    }


    WCHAR* days = new WCHAR[30];
    wcscpy(days, L"");
    WCHAR* mOfYear = new WCHAR[20];
    wcscpy(mOfYear, L"");


    token = wcstok(NULL, seps);
    while (token) {
        //
        // Daily = 0
        //
        if(recType == winRecursDaily) {
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else                            setIntProperty(L"Occurrences", occurences);
                token ++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
            }
        }
        //
        // Weekly = 1
        //
        else if(recType == winRecursWeekly) {
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else                            setIntProperty(L"Occurrences", occurences);
                if(wcscmp(days, L""))           setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                else                            setIntProperty(L"DayOfWeekMask", getWeekDayFromDate(startDate)); 
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
                if(wcscmp(days, L""))          setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                else                           setIntProperty(L"DayOfWeekMask", getWeekDayFromDate(startDate)); 
            }
            else if(isWeekDay(token)) {
                wcscat(days, token);
                wcscat(days, TEXT(" "));
            }
        }
        //
        // Monthly = 2
        //
        else if(recType == winRecursMonthly) {
            // VCalendar supports as Monthly by day rules: MD<interval> listofdates|listofdates(from end of month) <end tag>
            // On Outlook recurrence pattern only MD<interval> day <end tag> can be represented
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
                setIntProperty(L"Interval",      interval);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else                            setIntProperty(L"Occurrences", occurences);
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
            }
            else {
                if(dayOfMonth != -1) {
                    goto error;
                }
                dayOfMonth = _wtoi(token);
                if(dayOfMonth == 0) {
                    goto error;
                }
            }
        }
        //
        // MonthNth = 3
        //
        else if(recType == winRecursMonthNth) {
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"Instance",   weekOfMonth);
                setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else                            setIntProperty(L"Occurrences", occurences);
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"Instance",   weekOfMonth);
                setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
            }
            else if(isWeekDay(token)) {
                wcscat(days, token);
                wcscat(days, TEXT(" "));
            }
            else {
                if(token[1] != TEXT('+') && token[1] != TEXT('-')) {
                    goto error;
                }
                WCHAR sWeek[] = TEXT("\0\0");
                sWeek[0] = token[0];
                weekOfMonth = _wtoi(sWeek);
                if(token[1] == TEXT('-')) {
                    // it's the # of weeks to the end of month...
                    weekOfMonth = 5 - weekOfMonth;
                }
                if(weekOfMonth > 5 || weekOfMonth < 1) {
                    goto error;
                }
            }
        }
        //
        // Yearly = 5
        //
        else if(recType == winRecursYearly) {
            // Expected sequence will be YM1 month <end tag>
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else                            setIntProperty(L"Occurrences", occurences);
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
            }
            else {
                if(wcscmp(mOfYear, L"")) {
                    goto error;
                }
                wcscat(mOfYear, token);
                monthOfYear = _wtoi(mOfYear);
                SYSTEMTIME st;
                VariantTimeToSystemTime(startDate, &st);
                dayOfMonth = st.wDay;
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
            }
        }
        token = wcstok(NULL, seps);
    }

    ret = 0;
    goto finally;

error:
    LOG.error("WinRecurrence::parse error, bad RRULE format: %ls", dataString.c_str());
    ret = 1;
    goto finally;

finally:
    if (str)     delete [] str;
    if (days)    delete [] days;
    if (mOfYear) delete [] mOfYear;
    return ret;
}
