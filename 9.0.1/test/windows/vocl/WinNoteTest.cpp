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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include "base/fscapi.h"
#include "vocl/WinNote.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

# define TESTDIR "testcases"


USE_NAMESPACE

class WinNoteTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(WinNoteTest);
    CPPUNIT_TEST(testSubjectAndBody);
    CPPUNIT_TEST(testNoSubjectWithBody);
    CPPUNIT_TEST(testSubjectWithNoBody);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){
    }

    void tearDown(){
    }

private:

    /**
     * Loads a given vnote string and verifies that it contains the given lines.
     * 
     * @param char*  vnotecontent The vnote to load
     * @param char** contains     An array of lines that are supposed to be
     *                            contained in the result
     * @param int    numLines     The size of the contains array
     */
    void testVnote(const char* vnotecontent, const char** contains, int numLines){
        WinNote* wnote = new WinNote();
        WCHAR* temp = toWideChar(vnotecontent);
        wstring wvnote(temp);
        wnote->parse(wvnote);
        wstring result = wnote->toString();
        const char* tempc = toMultibyte(result.c_str());
        StringBuffer resultc(tempc);
        delete [] tempc; tempc = NULL;
        delete wnote;
        delete [] temp; temp = NULL;
        
        int i;
        for(i = 0; i < numLines; i++) {
            CPPUNIT_ASSERT( strstr(resultc.c_str(), contains[i]) != NULL );
        }
    }

    void testSubjectAndBody(){
        const char* vnote = "BEGIN:VNOTE\r\n\
VERSION:1.1\r\n\
SUMMARY:Subject\r\n\
BODY:What a great example!\r\n\
DCREATED:20091001T120000Z\r\n\
LAST-MODIFIED:20091001T120000Z\r\n\
END:VNOTE\r\n";

        const char* contains[] = {
            "SUMMARY:Subject\r\n",
            "BODY:What a great example!\r\n"
        };

        testVnote(vnote, contains, 2);
    }

    void testNoSubjectWithBody() {
        const char* vnote = "BEGIN:VNOTE\r\n\
VERSION:1.1\r\n\
SUMMARY:\r\n\
BODY:What a great example!\r\n\
DCREATED:20091001T120000Z\r\n\
LAST-MODIFIED:20091001T120000Z\r\n\
END:VNOTE\r\n";

        const char* contains[] = {
            "SUMMARY:\r\n",
            "BODY:What a great example!\r\n"
        };

        testVnote(vnote, contains, 2);
    }

    void testSubjectWithNoBody() {
        const char* vnote = "BEGIN:VNOTE\r\n\
VERSION:1.1\r\n\
SUMMARY:Subject\r\n\
BODY:\r\n\
DCREATED:20091001T120000Z\r\n\
LAST-MODIFIED:20091001T120000Z\r\n\
END:VNOTE\r\n";

        const char* contains[] = {
            "SUMMARY:Subject\r\n",
            "BODY:\r\n"
        };

        testVnote(vnote, contains, 2);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( WinNoteTest );
