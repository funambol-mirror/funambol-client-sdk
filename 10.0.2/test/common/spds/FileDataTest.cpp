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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "spds/FileData.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/util/XMLProcessor.h"

USE_NAMESPACE

#define FILE_NAME_SPECIAL_CHARS1      "At&T.txt"
#define FILE_NAME_SPECIAL_CHARS2      "3<4.test"


class FileDataTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FileDataTest);
    CPPUNIT_TEST(testEscapeFileName);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {}

    void tearDown() {}

private:

    void testEscape(const StringBuffer& inputName) {

        WString wname;
        wname = inputName;
        FileData fileData;
        fileData.setName(wname);

        StringBuffer fileContent("body-content");
        int fileSize = fileContent.length();
        fileData.setSize(fileSize);
        fileData.setBody(fileContent.c_str(), fileSize);

        const char* tmp = fileData.format();
        StringBuffer out = tmp;
        delete [] tmp;
        
        StringBuffer name;
        unsigned int start = 0, end = 0;
        if( XMLProcessor::getElementContent (out, FILE_NAME, NULL, &start, &end) ) {
            name = out.substr(start, end-start);
        }

        StringBuffer expected = inputName;
        expected.replaceAll("&", "&amp;");
        expected.replaceAll("<", "&lt;");

        CPPUNIT_ASSERT (name == expected);
    }


    void testEscapeFileName() {

        testEscape(FILE_NAME_SPECIAL_CHARS1);
        testEscape(FILE_NAME_SPECIAL_CHARS2);

    }


};

CPPUNIT_TEST_SUITE_REGISTRATION( FileDataTest );
