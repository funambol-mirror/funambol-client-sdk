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
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "spds/SyncManagerConfig.h"

#include "InputStreamTest.h"
#include "inputStream/FileDataInputStream.h"
#include "testUtils.h"


#define FILE_NAME_1    "test_LICENSE_unix.txt"      // a txt file with unix newlines
#define FILE_NAME_2    "test_LICENSE_dos.txt"       // a txt file with dos newlines
#define FILE_NAME_3    "pic.jpg"                    // a small picture (binary file)

USE_NAMESPACE

class FileDataInputStreamTest : public InputStreamTest {

    CPPUNIT_TEST_SUITE(FileDataInputStreamTest);
    CPPUNIT_TEST(testFileDataReadBigChunk);
    CPPUNIT_TEST(testFileDataReadTwoChunks);
    CPPUNIT_TEST(testFileDataReadManyChunks);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
    }

    void tearDown() {
    }

private:


    StringBuffer fileName;
    const char* fileDataContent;
    int fileDataSize;


    /// Loads data for testing the 'testFileName' file.
    void loadTestFileData(const char* testFileName) {

        fileName = getTestFileFullPath(SAMPLE_FILES_DIR, testFileName);
        const char* fileContent = loadTestFile(SAMPLE_FILES_DIR, testFileName, true);  // must be binary mode!
        int fileSize = (int)fgetsize(fileName.c_str());

        const WCHAR* wtestFileName = toWideChar(testFileName);
        
        FileData fileData;
        fileData.setName(wtestFileName);
        fileData.setSize(fileSize);
        fileData.setBody(fileContent, fileSize);

        unsigned long tstamp = getFileModTime(fileName);
        StringBuffer modTime = unixTimeToString(tstamp, true);  // file's mod time is already in UTC
        WString wmodTime;
        wmodTime = modTime;
        fileData.setModified(wmodTime);

        fileDataContent = fileData.format();
        fileDataSize = (int)strlen(fileDataContent);

        delete [] wtestFileName;
        delete [] fileContent;
    }

    void cleanTestFileData() {
        delete [] fileDataContent;
        fileDataContent = NULL;
    }


    /// Reads a big chunk (size > stream size)
    void testBigChunk(const char* testFileName) {

        loadTestFileData(testFileName);
        FileDataInputStream stream(fileName);
        testReadBigChunk(stream, fileDataContent, fileDataSize);
        stream.close();
        cleanTestFileData();
    }

    /// Reads the stream in 2 chunks
    void testTwoChunks(const char* testFileName) {

        loadTestFileData(testFileName);
        FileDataInputStream stream(fileName);
        testReadTwoChunks(stream, fileDataContent, fileDataSize);
        stream.close();
        cleanTestFileData();
    }

    /// Reads the stream in many small chuncks
    void testManyChunks(const char* testFileName, const int chunkSize) {

        loadTestFileData(testFileName);
        FileDataInputStream stream(fileName);
        testReadManyChunks(stream, fileDataContent, fileDataSize, chunkSize);
        stream.close();
        cleanTestFileData();
    }



    /// Runs 'testBigChunk' on 3 sample files
    void testFileDataReadBigChunk() {

        testBigChunk(FILE_NAME_1);
        testBigChunk(FILE_NAME_2);
        testBigChunk(FILE_NAME_3);
    }

    /// Runs 'testTwoChunks' on 3 sample files
    void testFileDataReadTwoChunks() {

        testTwoChunks(FILE_NAME_1);
        testTwoChunks(FILE_NAME_2);
        testTwoChunks(FILE_NAME_3);
    }

    /// Runs 'testManyChunks' on 3 sample files
    /// Use different chunk sizes
    void testFileDataReadManyChunks() {

        testManyChunks(FILE_NAME_1, 200);
        testManyChunks(FILE_NAME_1, 199);

        testManyChunks(FILE_NAME_2, 500);
        testManyChunks(FILE_NAME_2, 1499);

        testManyChunks(FILE_NAME_3, 665);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( FileDataInputStreamTest );
