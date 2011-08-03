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

void WinRecurrence::trim(wstring& str) {
    int idx = 0;
    while((idx=str.find_first_of(' ')) == 0 ) {
        str.replace( idx, 1, L"" );
    }
    while((idx=str.find_last_of(' ')) == str.size()-1 ) {
        str.replace( idx, 1, L"" );
    }    
}

// Constructor
WinRecurrence::WinRecurrence() {
    rrule = L"";
    startDate = 0;
    useTimezone = false;
}

// Constructor: fills propertyMap parsing the passed RRULE
WinRecurrence::WinRecurrence(const wstring & dataString, const DATE date) {
    rrule = L"";
    startDate = date;
    useTimezone = false;
    parse(dataString);
}

// Destructor
WinRecurrence::~WinRecurrence() {
}




// Format and return a RRULE string from the propertyMap.
wstring WinRecurrence::toString() {

    StringBuffer recurrence("");
    WCHAR* days = NULL;


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
    wstring pEnd     = L"";

    // Read all values from propertyMap.
    wstring tmp;
    getProperty(L"RecurrenceType", tmp);       recType     = _wtoi(tmp.c_str());
    getProperty(L"Interval",       tmp);       interval    = _wtoi(tmp.c_str());
    getProperty(L"Occurrences",    tmp);       occurrences = _wtoi(tmp.c_str());
    getProperty(L"DayOfWeekMask",  tmp);       dayofweek   = _wtoi(tmp.c_str());
    getProperty(L"DayOfMonth",     tmp);       dayofmonth  = _wtoi(tmp.c_str());
    getProperty(L"Instance",       tmp);       instance    = _wtoi(tmp.c_str());
    getProperty(L"MonthOfYear",    tmp);       monthofyear = _wtoi(tmp.c_str());
    getProperty(L"NoEndDate",      tmp);       noEnd       = (tmp == TEXT("1"));
    getProperty(L"PatternStartDate", pStart);
    getProperty(L"PatternEndDate",   pEnd);    // Since v8.7: always send pEndDate together with occurrences info.

    replaceAll(L"-", L"", pStart);   // **** To be removed!!! ****



    switch(recType) {
        //
        // Daily = 0
        //
        case winRecursDaily: {
            if(interval > 0) {
                recurrence.sprintf("D%ld", interval);
            }
            else if (dayofweek > 0) {
                WCHAR* days = daysOfWeekToString(dayofweek);
                if (days) {
                    wstring d(days);
                    trim(d);
                    recurrence.sprintf("W1 %ls", d.c_str());
                    delete [] days;
                }
            }
            break;
        }
        //
        // Weekly = 1
        //
        case winRecursWeekly: {
            if (dayofweek > 0) {
                days = daysOfWeekToString(dayofweek);
                if (days) {
                    wstring d(days);
                    trim(d);
                    recurrence.sprintf("W%ld %ls", interval, d.c_str());
                    delete [] days;
                }
            }
            break;
        }
        //
        // Monthly = 2
        //
        case winRecursMonthly: {
            if(dayofmonth > 0) {
                recurrence.sprintf("MD%ld %ld", interval, dayofmonth);
                break;
            }
        }
        //
        // MonthNth = 3
        //
        case winRecursMonthNth: {
            if(instance>0 && dayofweek>0) {
                days = daysOfWeekToString(dayofweek);
                StringBuffer pattern("MP%ld %ld+ %ls");
                if (instance == 5) {
                    pattern = "MP%ld %ld- %ls";  
                    instance = 1;
                }

                if(days) {
                    wstring d(days);
                    trim(d);
                    recurrence.sprintf(pattern.c_str(), interval, instance, d.c_str());
                    delete [] days;
                }
            }
            break;
        }
        //
        // Yearly = 5
        //
        case winRecursYearly: {
            if(dayofmonth>0 && monthofyear>0) {
                recurrence.sprintf("YM%ld %ld", interval, monthofyear);
            }
            break;
        }
        //
        // YearNth = 6
        // The recurrence year nth are written as montlynth after 12 months
        // TODO: this should be changed to the correct values "YD" since Outlook has been fixed 
        //       to manage these properties correctly, since v9.
        //
        case winRecursYearNth: {
            if(dayofweek>0 && instance>0) {
                days = daysOfWeekToString(dayofweek);
                if (interval <= 1) {
                    interval = 1;
                }
                int numberOfMonths = 12 * interval;
                StringBuffer pattern("MP%ld %ld+ %ls");
                if (instance == 5) {
                    pattern = "MP%ld %ld- %ls";  
                    instance = 1;
                }
                
                if(days) {
                    wstring d(days);
                    trim(d);
                    recurrence.sprintf(pattern.c_str(), numberOfMonths, instance, d.c_str());
                    //wsprintf(recurrence, pattern.c_str(), numberOfMonths, instance, d.c_str(), occurrences, pEnd.c_str());
                    delete [] days;
                }
            }
            break;
        }

        default: {
            LOG.error("Error formatting the RRULE property: unexpected rec type = %d", recType);
            break;
        }
    }

    //
    // Append duration info (it's the same for all rec types)
    //
    StringBuffer duration;
    if(noEnd) {
        duration.sprintf(" #0");
    } 
    else {
        // Both Occurrences and PatternEndDate are added since v9, for compatibility.
        // Keep the PatternEnd before the occurences, to make our portal happy.
        duration.sprintf(" %ls #%ld", pEnd.c_str(), occurrences);
    }
    recurrence.append(duration.c_str());


    WCHAR* buf = toWideChar(recurrence.c_str());
    rrule = buf;
    delete [] buf;
    return rrule;
}




// Parse a RRULE string and fills the propertyMap.
int WinRecurrence::parse(const wstring & dataString) {

    int ret = 0;
    WCHAR* str = wstrdup(dataString.c_str());
    WCHAR seps[] = TEXT(" ");
    WCHAR* token = wcstok(str, seps);
    WCHAR* days = NULL;
    WCHAR* mOfYear = NULL;

    int recType     = -1;
    int interval    =  0;
    int occurences  =  0;
    int dayOfMonth  = -1;
    int weekOfMonth = -1;
    int monthOfYear = -1;

    // it is a particular format of the yearly and yearlynth representation
    int yearly_format = 0;    

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
        if (dataString.find_first_of(TEXT("+-")) != wstring::npos) {
            recType = winRecursYearNth;
        }
        token += 2;
    }

    interval = _wtoi(token);
    if(!interval || recType == -1) {
        goto error;
    }


    days = new WCHAR[30];
    wcscpy(days, L"");
    mOfYear = new WCHAR[20];
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
                else {
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
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
                else {                           
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
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
            // MD1 7 #12
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
                setIntProperty(L"Interval",      interval);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else {
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
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
                // prevent something like MD1 7- #12 that we don't support
                if(wcschr(token, TEXT('-')) || wcschr(token, TEXT('+'))) {
                    LOG.error("Format not supported: %S", dataString.c_str());
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
            // Expected sequence will be MP<interval> <first|second...> dayOfWeekMask <end tag>
            // MP6 1+ MO #5
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"Instance",   weekOfMonth);
                setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else  {
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
                    
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
                    if (token[0] == TEXT('1')) {
                        weekOfMonth = 5; // means the last week of the month
                    } else {
                    // it's the # of weeks to the end of month...
                        weekOfMonth = 5 - weekOfMonth;
                    }
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
            // YM<interval> month <end tag>: YM2 6 #3 
            // YM<interval> month <end tag> dayofmonth: YM2 6 #3 MD1 12
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else {
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"DayOfMonth",  dayOfMonth);
            }
            else if(wcsstr(token, TEXT("MD"))) {
                
                // TEXT("YM2 6 #3 MD1 12"); 
                yearly_format = 1;
                token = wcstok(NULL, seps);
                dayOfMonth = _wtoi(token);
                if (dayOfMonth == 0) {
                    LOG.error("Wrong rrule representation: %S", dataString.c_str());
                    goto error;
                }
                setIntProperty(L"RecurrenceType", recType);
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
        //
        // Yearlynth = 6
        //
        else if(recType == winRecursYearNth) {
            // Expected sequence will be 
            // YM1 1+ SA 3 #0 or YM1 1+ SA 3 yyyyMMddTHHmmss
            // YM1 1 #5 MP1 1- FR or YM1 1 yyyyMMddTHHmmss MP1 1- FR
            if(wcschr(token, TEXT('#'))) {
                setIntProperty(L"RecurrenceType", recType);                
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"Instance",   weekOfMonth);
                setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                occurences = _wtoi(token+1);
                if(occurences == 0)             setIntProperty(L"NoEndDate",   1);
                else {
                    setIntProperty(L"Occurrences", occurences);
                    setIntProperty(L"NoEndDate",   0);
                }
                token++;
            }
            else if(token[8] == TEXT('T')) {
                setIntProperty(L"RecurrenceType", recType);
                setProperty(L"PatternEndDate",      token);
                setIntProperty(L"MonthOfYear",monthOfYear);
                setIntProperty(L"Interval",      interval);
                setIntProperty(L"Instance",   weekOfMonth);
                setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
            }
            else if(isWeekDay(token)) {
                wcscat(days, token);
                wcscat(days, TEXT(" "));

                if (yearly_format == 1) {
                    setIntProperty(L"RecurrenceType", recType);
                    setIntProperty(L"MonthOfYear",monthOfYear);
                    setIntProperty(L"Interval",      interval);
                    setIntProperty(L"Instance",   weekOfMonth);
                    setIntProperty(L"DayOfWeekMask", stringToDaysOfWeek(days));
                }
            }
            else if(token[1] == TEXT('+') || token[1] == TEXT('-')) {                 
                WCHAR sWeek[] = TEXT("\0\0");
                sWeek[0] = token[0];
                weekOfMonth = _wtoi(sWeek);
                if(token[1] == TEXT('-')) {
                    if (token[0] == TEXT('1')) {
                        weekOfMonth = 5; // means the last week of the month
                    } else {
                        // it's the # of weeks to the end of month...
                        weekOfMonth = 5 - weekOfMonth;
                    }
                }
                if(weekOfMonth > 5 || weekOfMonth < 1) {
                    goto error;
                }
            }
            else if(wcsstr(token, TEXT("MP"))) {  
                // TEXT("YM1 1 #5 MP1 1- FR"); 
                // in the first month of every year for 5 years, the last friday
                yearly_format = 1;
            }
            else {
                if(wcscmp(mOfYear, L"")) {
                    goto error;
                }
                wcscat(mOfYear, token);
                monthOfYear = _wtoi(mOfYear);                                
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
