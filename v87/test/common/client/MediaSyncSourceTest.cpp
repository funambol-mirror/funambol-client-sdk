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
#include "client/MediaSyncSource.h"
#include "testUtils.h"

USE_NAMESPACE

#define TEST_INPUT_DIR      SAMPLE_FILES_DIR      // here we have input files
#define HTTPUPLOADER_URL    "http://localhost"
#define TEST_OUTPUT_DIR     "MediaSyncSourceTest"

#define TEST_FILE_NAME1     "pic.jpg"
#define TEST_FILE_NAME2     "pic2.jpg"
#define TEST_FILE_NAME3     "pic3.jpg" 



/// Fake HTTPUploader, does nothing.
class FakeHttpUploader : public HttpUploader {

public:
    int upload(const StringBuffer& luid, InputStream* inputStream) {
        return HTTP_OK;     // 200
    }
};


/// Extends MediaSyncSource, to test it.
class FakeMediaSource: public MediaSyncSource {
public:
    FakeMediaSource(const WCHAR* wname,
                   AbstractSyncSourceConfig* sc,
                   const StringBuffer& aDir, 
                   MediaSyncSourceParams mediaParams):MediaSyncSource (wname, sc, aDir, mediaParams){};

    SyncItem* fakeFillSyncItem(StringBuffer* key, const bool fillData){
        return this->fillSyncItem(key, fillData);
    }

    ArrayList getLUIDArray(){
        return LUIDsToSend;
    }

    KeyValueStore* getLuidMap(){
        return LUIDMap;
    }

    int fakeAddItem(SyncItem& item){
        return this->addItem(item);
    }

    StringBuffer fakeReadCachePropertyValue(const char* prop){
        return this->readCachePropertyValue(prop);
    }

    /// Reimplemented, to return a fake HttpUploader when endSync() is called.
    HttpUploader* getHttpUploader() {
        return new FakeHttpUploader();
    }

};


class MediaSyncSourceTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(MediaSyncSourceTest);
    /*CPPUNIT_TEST(testAdd);
    CPPUNIT_TEST(testAddSameName);
    CPPUNIT_TEST(testAddSameNameSecondFile);
    CPPUNIT_TEST(testAddSameNameSecondFile2);
    CPPUNIT_TEST(testAddSameNameThirdFile);
    CPPUNIT_TEST(testAddNoName);
    CPPUNIT_TEST(testAddRawFile);
    CPPUNIT_TEST(cleanup);*/
    CPPUNIT_TEST(testFillSyncItem);
    CPPUNIT_TEST(testSetItemStatus);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {

        // Create the output dir
        outputDir = getTestDirFullPath(TEST_INPUT_DIR);
        createFolder(outputDir.c_str());
        MediaSyncSourceParams params_;
        
        params_.setUrl(HTTPUPLOADER_URL);
        SyncSourceConfig* fssc = new SyncSourceConfig();
        fssc->setURI("testMss");
        // Create the FileSyncSource
        fmss = new FakeMediaSource(TEXT("testMss"), fssc, outputDir, params_);

        ssr = new SyncSourceReport("testMss");
        fmss->setReport(ssr);
    }

    void tearDown() {
        delete fmss;
        delete ssr;
    }

private:

    StringBuffer outputDir;
    FakeMediaSource* fmss;
    SyncSourceReport* ssr;


    /// Util method: creates a OMA file data object
    FileData createFileData(const char* inputFileName) {

        // Read the input file
        char* content = NULL;
        size_t len;
        StringBuffer inFile = getTestFileFullPath(TEST_INPUT_DIR, inputFileName);
        bool fileLoaded = readFile(inFile.c_str(), &content, &len, true);
        CPPUNIT_ASSERT_MESSAGE("Failed to load test file", fileLoaded);

        FileData file;
        file.setBody(content, (int)len);
        file.setSize((int)len);
        WString wname;
        wname = inputFileName;
        file.setName(wname.c_str());

        delete [] content;
        return file;
    }


    void insertItem(const char* testFileName, const int expectedResult, 
                    const char* expectedOutName, const char* forceFileName = NULL) {

        FileData file = createFileData(testFileName);
        if (forceFileName) {
            WString wname;
            StringBuffer tmp(forceFileName);
            wname = tmp;
            file.setName(wname.c_str());
        }

        // Create a SyncItem containing the OMA file data
        SyncItem item(TEXT("test1"));
        char* data = file.format();
        item.setData(data, (long)strlen(data));
        delete [] data;

        int ret = fmss->insertItem(item);

        // TESTS
        CPPUNIT_ASSERT_MESSAGE("wrong status returned", ret == expectedResult);
        if (fmss->isErrorCode(ret)) {
            return;
        }
        StringBuffer outKey;
        outKey.convert(item.getKey());
        CPPUNIT_ASSERT_MESSAGE("wrong item's key returned", outKey == expectedOutName);

        StringBuffer outFile(outputDir);
        outFile.append(expectedOutName);
        CPPUNIT_ASSERT_MESSAGE("output file not found", fileExists(outFile.c_str()) );

        char* outContent = NULL;
        size_t outLen;
        readFile(outFile.c_str(), &outContent, &outLen, true);
        CPPUNIT_ASSERT( (int)outLen == file.getSize() );
        delete [] outContent;
    }


    void insertRawItem(const char* testFileName, const int expectedResult, 
                       const WCHAR* inputItemKey, const WCHAR* expectedOutName) {


        // Read the input file
        char* content = NULL;
        size_t len;
        StringBuffer inFile = getTestFileFullPath(TEST_INPUT_DIR, testFileName);
        bool fileLoaded = readFile(inFile.c_str(), &content, &len, true);
        CPPUNIT_ASSERT_MESSAGE("Failed to load test file", fileLoaded);
        
        // Create a SyncItem containing the raw file data
        SyncItem item(inputItemKey);
        item.setData(content, (long)len);
        delete [] content;

        int ret = fmss->insertItem(item);

        // TESTS
        CPPUNIT_ASSERT_MESSAGE("wrong status returned", ret == expectedResult);
        if (fmss->isErrorCode(ret)) {
            return;
        }
        WString outKey(item.getKey());
        CPPUNIT_ASSERT_MESSAGE("wrong item's key returned", outKey == expectedOutName);

        StringBuffer outFile(outputDir);
        StringBuffer outName;
        outName.convert(expectedOutName);
        outFile.append(outName);
        CPPUNIT_ASSERT_MESSAGE("output file not found", fileExists(outFile.c_str()) );

        char* outContent = NULL;
        size_t outLen;
        readFile(outFile.c_str(), &outContent, &outLen, true);
        CPPUNIT_ASSERT( outLen == len );
        delete [] outContent;
    }


    //
    ///////////////////////////////////////// TESTS /////////////////////////////////////////
    //

    /**
     * 1. Adds a file TEST_FILE_NAME1, expected ok (200).
     */
    void testAdd() {
        insertItem(TEST_FILE_NAME1, STC_OK, TEST_FILE_NAME1);
    }

    /**
     * 2. Adds the same file (name & content), expected code 418.
     */
    void testAddSameName() {
        insertItem(TEST_FILE_NAME1, STC_ALREADY_EXISTS, TEST_FILE_NAME1);
    }

    /**
     * 3. Adds a second file with same name (different content), expected ok (200) 
     * and the file name with "_01" suffix.
     */
    void testAddSameNameSecondFile() {
        StringBuffer expectedOutName(TEST_FILE_NAME1);
        expectedOutName.replace(".", "_01.");
        insertItem(TEST_FILE_NAME2, STC_OK, expectedOutName, TEST_FILE_NAME1);
    }

    /**
     * 4. Adds again the second file, expected code 418 and the file name with "_01" suffix.
     */
    void testAddSameNameSecondFile2() {
        StringBuffer expectedOutName(TEST_FILE_NAME1);
        expectedOutName.replace(".", "_01.");
        insertItem(TEST_FILE_NAME2, STC_ALREADY_EXISTS, expectedOutName, TEST_FILE_NAME1);
    }

    /**
     * 5. Adds a third file with same name (different content), expected ok (200) 
     * and the file name with "_02" suffix.
     */
    void testAddSameNameThirdFile() {
        StringBuffer expectedOutName(TEST_FILE_NAME1);
        expectedOutName.replace(".", "_02.");
        insertItem(TEST_FILE_NAME3, STC_OK, expectedOutName, TEST_FILE_NAME1);
    }

    /**
     * 6. Adds a file with no name, expected error (code 500).
     */
    void testAddNoName() {
        insertItem(TEST_FILE_NAME1, STC_COMMAND_FAILED, NULL, "");
    }

    /**
     * 7. Adds a raw file, expected ok (200).
     * Note: it's a raw file so the file name is the item's key from server.
     */
    void testAddRawFile() {
        WString itemKey(TEXT("key-from-server"));
        insertRawItem(TEST_FILE_NAME1, STC_OK, itemKey.c_str(), itemKey.c_str());
    }

    void testFillSyncItem(){
        StringBuffer inFile = getTestFileFullPath(TEST_INPUT_DIR, TEST_FILE_NAME1);
        SyncItem* si = fmss->fakeFillSyncItem(&inFile, true);

        StringBuffer* data = new StringBuffer((char*)si->getData());

        CPPUNIT_ASSERT(si != NULL);
        CPPUNIT_ASSERT(data != NULL);
      
        CPPUNIT_ASSERT(data->find("body") == StringBuffer::npos);
        CPPUNIT_ASSERT(data->find("size") != StringBuffer::npos);
    }

    void testSetItemStatus(){
        StringBuffer inFile = getTestFileFullPath(TEST_INPUT_DIR, TEST_FILE_NAME1);
        SyncItem* si = fmss->fakeFillSyncItem(&inFile, true);

        const WCHAR* wKey = si->getKey();
        const char* key = toMultibyte(wKey);
        fmss->setItemStatus(wKey, 200, "Add");
        ArrayList luidToSend = fmss->getLUIDArray();

        bool luidPresent = false;
        for (int i = 0; i < luidToSend.size(); i++){
            if( strcmp( ((StringBuffer*)luidToSend.get(i))->c_str(), key) == 0 ){
                luidPresent = true;
            }
        }
        CPPUNIT_ASSERT(luidPresent);

        int ret = fmss->endSync();
        CPPUNIT_ASSERT( ret == 0 );
    
        StringBuffer propval = fmss->fakeReadCachePropertyValue(inFile.c_str());
        CPPUNIT_ASSERT( !propval.empty() );
    }

    /// Cleans up the destination folder, removing the 4 files created.
    /// This should be launched at the end of all tests for FileSyncSourceTest.
    void cleanup() {
        removeFileInDir(outputDir.c_str());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( MediaSyncSourceTest );
