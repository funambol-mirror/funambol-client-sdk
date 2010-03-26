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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "spds/SyncManagerConfig.h"

#include "InputStreamTest.h"
#include "inputStream/FileInputStream.h"
#include "client/FileSyncItem.h"
#include "testUtils.h"


#define FILE_NAME_1    "test_LICENSE_unix.txt"      // a txt file with unix newlines
#define FILE_NAME_2    "test_LICENSE_dos.txt"       // a txt file with dos newlines
#define FILE_NAME_3    "pic.jpg"                    // a small picture (binary file)

USE_NAMESPACE

class FileInputStreamTest : public InputStreamTest {

    CPPUNIT_TEST_SUITE(FileInputStreamTest);
    CPPUNIT_TEST(testFileReadBigChunk);
    CPPUNIT_TEST(testFileReadTwoChunks);
    CPPUNIT_TEST(testFileReadManyChunks);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
        // Input txt test files
        fileName1    = getTestFileFullPath(SAMPLE_FILES_DIR, FILE_NAME_1);
        fileContent1 = loadTestFile       (SAMPLE_FILES_DIR, FILE_NAME_1, true);  // must be binary mode!
        fileSize1    = (int)strlen(fileContent1);

        fileName2    = getTestFileFullPath(SAMPLE_FILES_DIR, FILE_NAME_2);
        fileContent2 = loadTestFile       (SAMPLE_FILES_DIR, FILE_NAME_2, true);  // must be binary mode!
        fileSize2    = (int)strlen(fileContent2);

        fileName3    = getTestFileFullPath(SAMPLE_FILES_DIR, FILE_NAME_3);
        fileContent3 = loadTestFile       (SAMPLE_FILES_DIR, FILE_NAME_3, true);
    }

    void tearDown() {
        delete [] fileContent1;
        delete [] fileContent2;
        delete [] fileContent3;
    }

private:

    const char* fileContent1;
    StringBuffer fileName1;
    int fileSize1;

    const char* fileContent2;
    StringBuffer fileName2;
    int fileSize2;

    const char* fileContent3;
    StringBuffer fileName3;
    


    /// Reads a big chunk (size > stream size)
    void testFileReadBigChunk() {

        FileInputStream stream1(fileName1);
        testReadBigChunk(stream1, fileContent1, fileSize1);
        stream1.close();

        FileInputStream stream2(fileName2);
        testReadBigChunk(stream2, fileContent2, fileSize2);
        stream2.close();

        FileInputStream stream3(fileName3);
        int fileSize3 = stream3.getTotalSize();
        testReadBigChunk(stream3, fileContent3, fileSize3);
        stream3.close();
    }


    /// Reads the stream in 2 chunks
    void testFileReadTwoChunks() {

        FileInputStream stream1(fileName1);
        testReadTwoChunks(stream1, fileContent1, fileSize1);
        stream1.close();

        FileInputStream stream2(fileName2);
        testReadTwoChunks(stream2, fileContent2, fileSize2);
        stream2.close();

        FileInputStream stream3(fileName3);
        int fileSize3 = stream3.getTotalSize();
        testReadTwoChunks(stream3, fileContent3, fileSize3);
        stream3.close();
    }


    /// Reads the stream in many small chuncks
    void testFileReadManyChunks() {

        FileInputStream stream1(fileName1);
        testReadManyChunks(stream1, fileContent1, fileSize1, 200);     // Chunks of 200 bytes
        stream1.close();

        FileInputStream stream2(fileName2);
        testReadManyChunks(stream2, fileContent2, fileSize2, 500);     // Chunks of 500 bytes
        stream2.close();

        FileInputStream stream3(fileName3);
        int fileSize3 = stream3.getTotalSize();
        testReadManyChunks(stream3, fileContent3, fileSize3, 500);     // Chunks of 500 bytes
        stream3.close();
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( FileInputStreamTest );
