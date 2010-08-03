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
#include "vocl/WinRecurrence.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

# define TESTDIR "testcases"


USE_NAMESPACE

class WinRecurrenceTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(WinRecurrenceTest);
    CPPUNIT_TEST(testMonthNth_FirstFriday);
    CPPUNIT_TEST(testMonthNth_LastFriday);
    CPPUNIT_TEST(testMonthNth_SecondToLastSunday);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){
    }

    void tearDown(){
    }

private:

    /**
     * Verify that the given input results in the given expected output.
     * 
     * @param char* input    The recurrence input to parse
     * @param char* expected The expected output of the parsed recurrence string
     */
    void testVrecur(const char* input, const char* expected){
        WinRecurrence* wrecurrence = new WinRecurrence();
        WCHAR* temp = toWideChar(input);
        wstring wcal(temp);
        wrecurrence->parse(wcal);
        wstring result = wrecurrence->toString();
        const char* tempc = toMultibyte(result.c_str());
        StringBuffer resultc(tempc);
        delete [] tempc; tempc = NULL;
        delete wrecurrence;
        delete [] temp; temp = NULL;
        
        CPPUNIT_ASSERT( strcmp(resultc.c_str(), expected) == 0 );  
    }

    void testMonthNth_FirstFriday() {
        const char* input = "MP1 1+ FR #2";
        const char* expected = "MP1 1+ FR #2";
        testVrecur(input, expected);
    }

    void testMonthNth_LastFriday() {
        const char* input = "MP1 1- FR #2";
        const char* expected = "MP1 1- FR #2";
        testVrecur(input, expected);
    }

    void testMonthNth_SecondToLastSunday() {
        const char* input = "MP1 2- SU #2";
        const char* expected = "MP1 3+ SU #2";
        testVrecur(input, expected);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( WinRecurrenceTest );
