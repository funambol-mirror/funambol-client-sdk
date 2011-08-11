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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"
#include "base/util/utils.h"
#include "testUtils.h"

USE_NAMESPACE

/**
 * This is the test class for the SyncML Parser.
 */
class utilsTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(utilsTest);
    CPPUNIT_TEST(unixTimeToStringTest);
    CPPUNIT_TEST(getFileModTimeTest);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()    {}
    void tearDown() {}

private:

    /// Tests the unixTimeToString() function.
    void unixTimeToStringTest() {

        StringBuffer ret = unixTimeToString(0, true);
        CPPUNIT_ASSERT(ret == "19700101T000000Z");

        // this is: 2009-10-27 @17:25:23 UTC
        unsigned long unixTime = 1256664323;

        ret = unixTimeToString(unixTime, true);
        CPPUNIT_ASSERT(ret == "20091027T172523Z");

        // test with no "Z"
        ret = unixTimeToString(unixTime, false);
        CPPUNIT_ASSERT(ret == "20091027T172523");

        unixTime -= 60;
        ret = unixTimeToString(unixTime, false);
        CPPUNIT_ASSERT(ret == "20091027T172423");
    }

    /**
     * Tests the getFileModTime() function.
     * Please note that this test passes only if execution is not interrupted during the test,
     * so please don't break inside this test.
     */
    void getFileModTimeTest() {

        // Read and save a test file, to set the last modification time to now
        char* content = loadTestFile           (SAMPLE_FILES_DIR, "test_LICENSE_unix.txt", true);
        StringBuffer path = getTestFileFullPath(SAMPLE_FILES_DIR, "test_LICENSE_unix.txt");
        size_t size = fgetsize(path.c_str());
        saveFile(path.c_str(), content, size, true);
        delete [] content;
        
        time_t now = time(NULL);

        unsigned long modTime = getFileModTime(path);
        CPPUNIT_ASSERT( (now - modTime) <= 1 );      // 1 second = max error
    }


private:


};

CPPUNIT_TEST_SUITE_REGISTRATION( utilsTest );
