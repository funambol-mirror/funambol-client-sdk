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

#include "base/util/BasicTime.h"
#include "base/globalsdef.h"

USE_NAMESPACE

BasicTime::BasicTime() {
    year = 1970;
    month = 1;
    day = 1;
    weekday = -1;
    hour = 0;
    min = 0;
    sec = 0;
    tzHour = 0;
    tzMin = 0;
}

int BasicTime::set(int /* yy */, int /* mon */, int /* dd */, int /* wd */,
                   int /* hh */, int /* mm */, int /* ss */, int /* tzh */, int /* tzm */)
{
    //  TODO
    return 0;
}

/**
 * Parse the date in RF 822 format
 *
 * Some examples:
 * Date: Fri, 01 Aug 2003 14:04:55 +0800
 * Date: Wed, 30 Jul 2003 13:24:21 -0700
 * Date: 20 Jun 2003 15:42:12 -0500
 *
 * RFC822 date time
 *
 *   date-time   =  [ day "," ] date time        ; dd mm yy
 *                                              ;  hh:mm:ss zzz
 *
 *   day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
 *               /  "Fri"  / "Sat" /  "Sun"
 *
 *   date        =  1*2DIGIT month 2DIGIT        ; day month year
 *                                               ;  e.g. 20 Jun 82
 *
 *   month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
 *               /  "May"  /  "Jun" /  "Jul"  /  "Aug"
 *               /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"
 *
 *   time        =  hour zone                    ; ANSI and Military
 *
 *   hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
 *                                               ; 00:00:00 - 23:59:59
 *
 *   zone        =  "UT"  / "GMT"                ; Universal Time
 *                                               ; North American : UT
 *               /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
 *               /  "CST" / "CDT"                ;  Central:  - 6/ - 5
 *               /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
 *               /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
 *               /  1ALPHA                       ; Military: Z = UT;
 *                                               ;  A:-1; (J not used)
 *                                               ;  M:-12; N:+1; Y:+12
 *               / ( ("+" / "-") 4DIGIT )        ; Local differential
 *                                               ;  hours+min. (HHMM)
**/
int BasicTime::parseRfc822(const char *date)
{

    int ret = 0;
    if (!isADate(date)) {
        return -1;
    }

    const char *days[] = {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat"
    };
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    };
    char dayOfWeek[6] = "---,";
    char mon[4] = "---";
    char time[10] = "00:00:00";
    char timeZone[20] = "GMT";

    // Wed Feb 01 14:40:45 Europe/Amsterdam 2006
    // do we have day of week?
    const char *pdate = strstr( date, "," );
    if ( pdate == 0 ) {
        ret=sscanf(date, "%d %s %d %s %s",
            &day, mon, &year, time, timeZone);
    }
    else {
        ret=sscanf(date, "%s %d %s %d %s %s",
            dayOfWeek, &day, mon, &year, time, timeZone);
        if (ret >= 1 && ret < 6) {
            // it can be an error in the format: Mon,12 Feb 2007 09:00:01 +0100
            // the comma is attached to the day
            if (*(pdate + 1) != ' ') {
                ret = sscanf(pdate + 1, "%d %s %d %s %s",
                    &day, mon, &year, time, timeZone);

            }
        }
        // Convert day of week
        for (int i = 0; i < 7; i++) {
            if ( strncmp(days[i], dayOfWeek, 3) == 0 ) {
                weekday = i;
                break;
            }
        }
        // Do not exit on error on weekday, treat it like if it was
        // not present.
    }
    // Trap parsing error
    if(ret == EOF || ret == 0){
        return -1;
    }
    // Check validity
    if (year > 3000 || day < 0 || day > 31){
        *this = BasicTime();
        return -1;
    }

    // Get month
    int i;
    for (i = 0; i < 12; i++) {
        if ( strcmp(months[i], mon) == 0 ) {
            month = i+1;
            break;
        }
    }
    // Trap parsing error
    if (i==13) {
        return -1;
    }

    // Year ---------------------------------
    if (year <= 60) {
        year += 2000;
    } else if (year > 60 && year < 100) {
        year += 1900;
    }

    // hh:mm:ss -------------------------
    // do we have sec?
    if (strlen(time) > 6 && time[5] == ':')
        sscanf(time, "%d:%d:%d", &hour, &min, &sec);
    else
        sscanf(time, "%d:%d", &hour, &min);

    // Timezone ---------------------------------
    if ( strcmp(timeZone, "GMT") != 0 && strcmp(timeZone, "UT") != 0) {
        // is this explicit time?
        if ( timeZone[0] == '+' || timeZone[0]== '-' ) {
            char wcH[4] = "+00";
            char wcM[4] = "00";

            // get hour
            if ( strlen(timeZone) > 3) {
                wcH[0] = timeZone[0];
                wcH[1] = timeZone[1];
                wcH[2] = timeZone[2];
                wcH[3] = '\0';
            }
            // get min
            if ( strlen(timeZone) >= 5)    {
                wcM[0] = timeZone[3];
                wcM[1] = timeZone[4];
                wcM[2] = '\0';
            }
            tzHour = atoi(wcH);
            tzMin = atoi(wcM);
        }
        // otherwise it could be one string with the time
        else if ( strcmp(timeZone, "EDT") == 0) {
            tzHour = -4;
        }
        else if ( strcmp(timeZone, "EST") == 0
            ||  strcmp(timeZone, "CDT") == 0) {
                tzHour = -5;
        }
        else if ( strcmp(timeZone, "CST") == 0
            ||  strcmp(timeZone, "MDT") == 0) {
                tzHour = -6;
        }
        else if ( strcmp(timeZone, "MST") == 0
            ||  strcmp(timeZone, "PDT") == 0 ){
                tzHour = -7;
        }
        else if ( strcmp(timeZone, "PST") == 0) {
            tzHour = -8;
        }
    }

    // clean up
    return 0;
}

/*
* The function return if the argument passed is a date in a format
* we are searching. To decide it the date must contain the month, a space
* and the millennium
*
* Mar 2007, Jun 2007, Mar 07, Mar 98. We search Mar 2, Mar 0...
* If no one of them is found try with the millenium 1XXX
*
*/
bool BasicTime::isADate(const char* date) {
    const char *months[] = {
        "Jan 2", "Feb 2", "Mar 2", "Apr 2",
        "May 2", "Jun 2", "Jul 2", "Aug 2",
        "Sep 2", "Oct 2", "Nov 2", "Dec 2",
        "Jan 1", "Feb 1", "Mar 1", "Apr 1",
        "May 1", "Jun 1", "Jul 1", "Aug 1",
        "Sep 1", "Oct 1", "Nov 1", "Dec 1",
        "Jan 0", "Feb 0", "Mar 0", "Apr 0",
        "May 0", "Jun 0", "Jul 0", "Aug 0",
        "Sep 0", "Oct 0", "Nov 0", "Dec 0",
        "Jan 9", "Feb 9", "Mar 9", "Apr 9",
        "May 9", "Jun 9", "Jul 9", "Aug 9",
        "Sep 9", "Oct 9", "Nov 9", "Dec 9"
    };   
    for (int i = 0; i < 48; i++) {
        if (strstr(date, months[i]) != NULL) {
            return true;
        }
    }
    return false;

}



// Date: Fri, 01 Aug 2003 14:04:55 +0800
char *BasicTime::formatRfc822() const {
    const char *days[] = {
        "Sun", "Mon", "Tue", "Wed",
        "Thu", "Fri", "Sat", "Sun"
    };
    const char *months[] = {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    };
    char *ret = new char[60]; // FIXME: avoid sprintf and static size

    // If the weekday is valid, print it, otherwise ignore it
    if(weekday > 0 && weekday < 7) {
        sprintf(ret, "%s, %d %s %d %02d:%02d:%02d %+03d%02d",
                      days[weekday], day, months[month-1], year,
                      hour, min, sec,
                      tzHour, tzMin);
    }
    else {
        sprintf(ret, "%d %s %d %02d:%02d:%02d %+03d%02d",
                      day, months[month-1], year,
                      hour, min, sec,
                      tzHour, tzMin);
    }

    return ret;
}

ArrayElement *BasicTime::clone() {
    return new BasicTime(*this);
};

BasicTime& BasicTime::operator=(const BasicTime& o) {
    year = o.year;
    month = o.month;
    day = o.day;
    weekday = o.weekday;
    hour = o.hour;
    min = o.min;
    sec = o.sec;
    tzHour = o.tzHour;
    tzMin = o.tzMin;

    return *this;
}

bool BasicTime::operator==(const BasicTime& o) const {
    return (
        year == o.year &&
        month == o.month &&
        day == o.day &&
        weekday == o.weekday &&
        hour == o.hour &&
        min == o.min &&
        sec == o.sec &&
        tzHour == o.tzHour &&
        tzMin == o.tzMin
    );
}

