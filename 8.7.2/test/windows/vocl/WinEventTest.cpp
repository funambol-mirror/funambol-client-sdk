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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include "base/fscapi.h"
#include "vocl/WinEvent.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

# define TESTDIR "testcases"


USE_NAMESPACE

class WinEventTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(WinEventTest);
    CPPUNIT_TEST(testOneAttendee);
    CPPUNIT_TEST(testTwoAttendees);
    CPPUNIT_TEST(testAllDayMidnightToElevenFiftyNine);
    CPPUNIT_TEST(testAllDayMidnightToMidnight);
    CPPUNIT_TEST(testNotAllDay);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){
    }

    void tearDown(){
    }

private:

    /**
     * Loads a given vcal string and verifies that it contains the given lines.
     * 
     * @param char*  vcalcontent The vcal to load
     * @param char** contains    An array of lines that are supposed to be
     *                           contained in the result
     * @param int    numLines    The size of the contains array
     */
    void testVcal(const char* vcalcontent, const char** contains, int numLines){
        WinEvent* wevent = new WinEvent();
        WCHAR* temp = toWideChar(vcalcontent);
        wstring wcal(temp);
        wevent->parse(wcal);
        wstring result = wevent->toString();
        const char* tempc = toMultibyte(result.c_str());
        StringBuffer resultc(tempc);
        delete [] tempc; tempc = NULL;
        delete wevent;
        delete [] temp; temp = NULL;

        //printf("\n\n\tresultc:\n%s\n", resultc.c_str());
        
        int i;
        for(i = 0; i < numLines; i++) {
            //printf("\tcontains:\n%s\n", contains[i]);
            CPPUNIT_ASSERT( strstr(resultc.c_str(), contains[i]) != NULL );
        }
    }

    void testOneAttendee(){
        const char* vnote = "BEGIN:VCALENDAR\r\n\
VERSION:1.0\r\n\
BEGIN:VEVENT\r\n\
SUMMARY:test\r\n\
CLASS:PUBLIC\r\n\
DESCRIPTION:An event with an attendee\r\n\
DTSTART:20091001T120000Z\r\n\
DTEND:20091001T120000Z\r\n\
ATTENDEE;ROLE=OWNER;STATUS=CONFIRMED:Bruce Wayne <b@wayneenterprises.com>\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        const char* contains[] = {
            "ATTENDEE;STATUS=CONFIRMED;ROLE=OWNER:Bruce Wayne <b@wayneenterprises.com>\r\n"
        };

        testVcal(vnote, contains, 1);
    }

    void testTwoAttendees(){
        const char* vnote = "BEGIN:VCALENDAR\r\n\
VERSION:1.0\r\n\
BEGIN:VEVENT\r\n\
SUMMARY:test\r\n\
CLASS:PUBLIC\r\n\
DESCRIPTION:An event with two attendees\r\n\
DTSTART:20091001T120000Z\r\n\
DTEND:20091002T120000Z\r\n\
ATTENDEE;ROLE=OWNER;STATUS=CONFIRMED:Bruce Wayne <b@wayneenterprises.com>\r\n\
ATTENDEE;ROLE=ATTENDEE;STATUS=TENTATIVE:Dick Grayson <r@wayneenterprises.com>\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        const char* contains[] = {
            "ATTENDEE;STATUS=CONFIRMED;ROLE=OWNER:Bruce Wayne <b@wayneenterprises.com>\r\n",
            "ATTENDEE;STATUS=TENTATIVE;ROLE=ATTENDEE:Dick Grayson <r@wayneenterprises.com>\r\n"
        };

        testVcal(vnote, contains, 2);
    }

    void testAllDayMidnightToElevenFiftyNine() {
        const char* vnote = "BEGIN:VCALENDAR\r\n\
VERSION:1.0\r\n\
BEGIN:VEVENT\r\n\
SUMMARY:test\r\n\
CLASS:PUBLIC\r\n\
DESCRIPTION:An all day event\r\n\
DTSTART:20091001T000000Z\r\n\
DTEND:20091001T235900Z\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        const char* contains[] = {
            "DTSTART:20091001\r\n",
            "DTEND:20091002\r\n",
            "X-FUNAMBOL-ALLDAY:1\r\n"
        };

        testVcal(vnote, contains, 3);
    }

    void testAllDayMidnightToMidnight() {
        const char* vnote = "BEGIN:VCALENDAR\r\n\
VERSION:1.0\r\n\
BEGIN:VEVENT\r\n\
SUMMARY:test\r\n\
CLASS:PUBLIC\r\n\
DESCRIPTION:An all day event\r\n\
DTSTART:20091001T000000Z\r\n\
DTEND:20091002T000000Z\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        const char* contains[] = {
            "DTSTART:20091001\r\n",
            "DTEND:20091002\r\n",
            "X-FUNAMBOL-ALLDAY:1\r\n"
        };

        testVcal(vnote, contains, 3);
    }

    void testNotAllDay() {
        const char* vnote = "BEGIN:VCALENDAR\r\n\
VERSION:1.0\r\n\
BEGIN:VEVENT\r\n\
SUMMARY:test\r\n\
CLASS:PUBLIC\r\n\
DESCRIPTION:A one hour event\r\n\
DTSTART:20091001T120000Z\r\n\
DTEND:20091001T130000Z\r\n\
END:VEVENT\r\n\
END:VCALENDAR\r\n";

        const char* contains[] = {
            "DTSTART:20091001T120000Z\r\n",
            "DTEND:20091001T130000Z\r\n",
            "X-FUNAMBOL-ALLDAY:0\r\n"
        };

        testVcal(vnote, contains, 3);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( WinEventTest );
