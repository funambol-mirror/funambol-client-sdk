/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#include <base/test.h>

#include "base/util/BasicTime.h"

BasicTime::BasicTime() {
    year = 1970;
    month = 1;
    day = 1;
    weekday = 0;
    hour = 0;
    min = 0;
    sec = 0;
    tzHour = 0;
    tzMin = 0;
}

int BasicTime::set(int yy, int mon, int dd, int wd,
                   int hh, int mm, int ss, int tzh, int tzm)
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
    }
    // Trap parsing error
    if(ret == EOF || ret == 0){
        return -1;
    }
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
    if (i==13)
        return -1;

	// Year ---------------------------------
	if (year < 100) year += 1900;

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
			if ( strlen(timeZone) >= 5)	{
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
* Mar 2007, Jun 2007. We search Mar 2, Jun 2.
* If no one of them is found try with the millenium 1XXX
*
*/
bool BasicTime::isADate(const char* date) {
    const char *months2000[] = {
        "Jan 2", "Feb 2", "Mar 2", "Apr 2",
        "May 2", "Jun 2", "Jul 2", "Aug 2",
        "Sep 2", "Oct 2", "Nov 2", "Dec 2"
    };

    const char *months1000[] = {
        "Jan 1", "Feb 1", "Mar 1", "Apr 1",
        "May 1", "Jun 1", "Jul 1", "Aug 1",
        "Sep 1", "Oct 1", "Nov 1", "Dec 1"
    };
    for (int i = 0; i < 12; i++) {
        if (strstr(date, months2000[i]) != NULL) {
            return true;
        }
    }
    for (int i = 0; i < 12; i++) {
        if (strstr(date, months1000[i]) != NULL) {
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

    sprintf(ret, "%s, %d %s %d %02d:%02d:%02d %+03d%02d",
                  days[weekday], day, months[month-1], year, hour, min, sec,
                  tzHour, tzMin);

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

#ifdef ENABLE_UNIT_TESTS


class BasicTimeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicTimeTest);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testConversion);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        // millenium.set(2000, 01, 01, 6,
        //              00, 00, 00,
        //              00, 00);
        millenium.setYear(2000);
        buffer = NULL;
    }
    void tearDown() {
        if (buffer) {
            delete [] buffer;
        }
    }

protected:
    void testEqual() {
        BasicTime empty;
        CPPUNIT_ASSERT(empty != millenium);

        BasicTime copy(millenium);
        CPPUNIT_ASSERT(millenium == copy);
        copy = millenium;
        CPPUNIT_ASSERT(millenium == copy);
    }

    void testConversion() {
        buffer = millenium.formatRfc822();

        BasicTime copy;
        CPPUNIT_ASSERT_EQUAL(0, copy.parseRfc822(buffer));
        CPPUNIT_ASSERT(millenium == copy);
        delete [] buffer; buffer = NULL;

        CPPUNIT_ASSERT_EQUAL(-1, copy.parseRfc822("this is garbage"));

        static const char convertStr[] = "Mon, 6 Nov 2006 20:30:15 +0100";
        BasicTime convert;
        CPPUNIT_ASSERT_EQUAL(0, convert.parseRfc822(convertStr));
        buffer = convert.formatRfc822();
        CPPUNIT_ASSERT(!strcmp(buffer, convertStr));
        delete [] buffer; buffer = NULL;
    }

private:
    BasicTime millenium;
    char *buffer;
};

FUNAMBOL_TEST_SUITE_REGISTRATION(BasicTimeTest);

#endif
