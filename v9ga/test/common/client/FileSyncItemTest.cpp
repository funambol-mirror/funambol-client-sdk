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
#include "client/FileSyncItem.h"
#include "inputStream/FileDataInputStream.h"

#include "common/inputStream/InputStreamTest.h"
#include "testUtils.h"


#define TEST_FILE_NAME      "test_LICENSE_unix.txt"      // a txt file with unix newlines


USE_NAMESPACE

class FileSyncItemTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(FileSyncItemTest);
    CPPUNIT_TEST(testFileSyncItemRawInputStream);
    CPPUNIT_TEST(testFileSyncItemFileDataInputStream);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {}
    void tearDown() {}

private:

    /**
     * 1. Creates a FileSyncItem with a FileInputStream/FileDataInputStream
     *    and reads the stream from there
     * 2. Clones the FileSyncItem, then reads again the stream
     * Use the inputStream tests defined in InputStreamTest class.
     */
    void testFileSyncItemInputStream(const bool isFileData) {

        StringBuffer fileName = getTestFileFullPath(SAMPLE_FILES_DIR, TEST_FILE_NAME);

        FileSyncItem item(fileName, isFileData);
        InputStreamTest test;

        // raw data
        char* fileContent = loadTestFile(SAMPLE_FILES_DIR, TEST_FILE_NAME, true);  // must be binary mode!
        int fileSize = (int)fgetsize(fileName.c_str());

        // file data object
        if (isFileData) {
            FileData fileData;
            fileData.setName(TEXT(TEST_FILE_NAME));
            fileData.setSize(fileSize);
            fileData.setBody(fileContent, fileSize);

            unsigned long tstamp = getFileModTime(fileName);
            StringBuffer modTime = unixTimeToString(tstamp, true);  // file's mod time is already in UTC
            WString wmodTime;
            wmodTime = modTime;
            fileData.setModified(wmodTime);

            // re-set the expected results
            delete [] fileContent;
            fileContent = fileData.format();
            fileSize = (int)strlen(fileContent);
        }

        InputStream* stream = item.getInputStream();
        CPPUNIT_ASSERT (stream);
        CPPUNIT_ASSERT (item.getDataSize() == fileSize);

        if (stream) {
            test.testReadManyChunks(*stream, fileContent, fileSize, 1024);
        }

        // Clone the item
        FileSyncItem* itemCloned = (FileSyncItem*)item.clone();
        stream = itemCloned->getInputStream();
        CPPUNIT_ASSERT (stream);
        CPPUNIT_ASSERT (item.getDataSize() == fileSize);

        if (stream) {
            test.testReadManyChunks(*stream, fileContent, fileSize, 899);
        }

        delete itemCloned;
        delete [] fileContent;
    }

    /// Tests the raw file data read() from the FileSyncItem
    void testFileSyncItemRawInputStream() {
        testFileSyncItemInputStream(false);
    }

    /// Tests the file object data read() from the FileSyncItem
    void testFileSyncItemFileDataInputStream() {
        testFileSyncItemInputStream(true);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( FileSyncItemTest );
