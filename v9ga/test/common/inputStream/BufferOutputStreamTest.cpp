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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "spds/SyncManagerConfig.h"

#include "inputStream/BufferOutputStream.h"
#include "inputStream/FileInputStream.h"
#include "testUtils.h"

#define TEST_STRING_1   "test 1"
#define TEST_STRING_2   "-test number 2"
#define PICT_NAME       "pic.jpg"                    // a small picture (binary file)

USE_FUNAMBOL_NAMESPACE

/**
 * This class defines protected methods to test a BufferOutputStream class
 */
class BufferOutputStreamTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(BufferOutputStreamTest);
    CPPUNIT_TEST(testWriteString);
    CPPUNIT_TEST(testWrite2Strings);
    CPPUNIT_TEST(testWritePicture);
    CPPUNIT_TEST(testWritePictureChunked);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()    {}
    void tearDown() {}

private:

    /// Create an OutputStream and write 1 string
    void testWriteString() {

        StringBuffer testString(TEST_STRING_1);

        BufferOutputStream os;
        int res = os.write(testString.c_str(), testString.length());
        
        CPPUNIT_ASSERT(res == testString.length());
        CPPUNIT_ASSERT(os.size() == testString.length());

        char* data = (char*)os.getData();
        res = strncmp(data, testString.c_str(), testString.length());
        CPPUNIT_ASSERT(res == 0);
    }

    /// Create an OutputStream and write 2 strings
    void testWrite2Strings() {

        StringBuffer testString1(TEST_STRING_1);
        StringBuffer testString2(TEST_STRING_2);

        BufferOutputStream os;
        int res = os.write(testString1.c_str(), testString1.length());
        res = os.write(testString2.c_str(), testString2.length());
        
        CPPUNIT_ASSERT(res == testString2.length());
        CPPUNIT_ASSERT(os.size() == testString1.length() + testString2.length());

        StringBuffer data((char*)os.getData());

        testString1.append(testString2);
        res = strncmp(data, testString1.c_str(), testString1.length());
        CPPUNIT_ASSERT(res == 0);
    }

    /// Create an OutputStream and write the whole picture content (1 shot)
    void testWritePicture() {

        StringBuffer fileName = getTestFileFullPath(SAMPLE_FILES_DIR, PICT_NAME);
        size_t len = 0;
        char* content = NULL;
        bool fileLoaded = readFile(fileName.c_str(), &content, &len, true);
        CPPUNIT_ASSERT_MESSAGE("Failed to load test file", fileLoaded);

        BufferOutputStream os;
        int res = os.write(content, (int)len);

        CPPUNIT_ASSERT(res == len);
        CPPUNIT_ASSERT(os.size() == len);

        void* data = os.getData();
        res = memcmp(data, content, len);
        CPPUNIT_ASSERT(res == 0);

        delete [] content;
    }

    /// Create an OutputStream and write the picture content in many chunks
    void testWritePictureChunked() {

        StringBuffer fileName = getTestFileFullPath(SAMPLE_FILES_DIR, PICT_NAME);
        FileInputStream is(fileName);

        BufferOutputStream os;

        char chunk[1000];
        while (!is.eof()) {
            int bytesRead = is.read(chunk, 1000);
            int bytesWritten = os.write(chunk, bytesRead);

            CPPUNIT_ASSERT(bytesRead == bytesWritten);
        }

        CPPUNIT_ASSERT(os.size() == is.getTotalSize());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( BufferOutputStreamTest );

