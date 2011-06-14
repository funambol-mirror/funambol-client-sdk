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
#include "spds/SyncSourceConfig.h"

#include "sapi/MediaSapiSyncSource.h"
#include "sapi/SapiSyncSource.h"
#include "sapi/UploadSapiSyncItem.h"


#include "testUtils.h"
#include <sys/types.h>
#include <sys/stat.h>


#ifdef WIN32
    #define GetCurrentDir _getcwd           
    #include <direct.h>
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif


USE_FUNAMBOL_NAMESPACE

/**
 * This class defines protected methods to test a BufferOutputStream class
 */
class MediaSapiSyncSourceTest : public CppUnit::TestFixture {
    
    CPPUNIT_TEST_SUITE(MediaSapiSyncSourceTest);
    CPPUNIT_TEST(allItem_delItem);    
    CPPUNIT_TEST(addNewItem);     
    CPPUNIT_TEST(addNewItemWithSameName);   
    CPPUNIT_TEST(localStoragePercentage);
    CPPUNIT_TEST(localStorageAbsolute);
    CPPUNIT_TEST_SUITE_END();
    
public:
    
    void setUp()    {}
    void tearDown() {}
    
    StringBuffer dirMedia;

    void initMediaDir() {
        char currentPath[FILENAME_MAX];
        GetCurrentDir(currentPath, sizeof(currentPath));
        dirMedia = currentPath;
        dirMedia.append("/");
        dirMedia.append(getTestDirFullPath("FileSapiSyncSourceTest"));
        dirMedia = dirMedia.substr(0, dirMedia.length() - 1);
    }


    void initConfigTree() {

        initMediaDir();

        DMTClientConfig* config = getNewDMTClientConfig("sapi-test-pictures", true);
        SyncSourceConfig* sc = new SyncSourceConfig();
        sc->setName                 ("picture");
        sc->setSyncModes            ("two-way, one-way-from-client, one-way-from-server");
        sc->setSync                 ("two-way");
        sc->setEncoding             ("");
        sc->setLast                 (0);
        sc->setSupportedTypes       ("");
        sc->setVersion              ("");
        sc->setEncryption           ("");
        sc->setURI                  ("picture");
        sc->setType                 ("application/*");
        sc->setProperty             (PROPERTY_DOWNLOAD_LAST_TIME_STAMP, "0");
        sc->setIntProperty          (PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, 0);
        sc->setIntProperty          (PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, 0);
        sc->setProperty             (PROPERTY_EXTENSION, ".jpg");
        sc->setProperty             (PROPERTY_MEDIAHUB_PATH, dirMedia.c_str());  
        config->setSyncSourceConfig(*sc);

        config->save();
        delete config;

    }

private:        

    /// Create an OutputStream and write 1 string
    void addNewItem() {
        
        initMediaDir();

        StringBuffer s = initAdapter("sapi-test-pictures");
        DMTClientConfig* config = new DMTClientConfig(s);        
        config->read();

        int count = config->getSyncSourceConfigsCount();
        if (count == 0) {
            delete config;
            initConfigTree();   
            config = new DMTClientConfig(s);        
            config->read();
            count = config->getSyncSourceConfigsCount();
            CPPUNIT_ASSERT(count > 0);
        }
        
        SyncSourceConfig* ss;
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "picture") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);
 
        StringBuffer file = dirMedia;
        file.append("/");
        file.append("toRename.tiff");
        
        StringBuffer file2(file);
        file2.append(".jpg");

        rename(file.c_str(), file2.c_str());

        int err;
        SyncSourceReport rep("picture");        
        MediaSapiSyncSource pictureSource(*ss, rep, 0, 0);

        pictureSource.beginSync(true, *config);
        int res = pictureSource.getItemsNumber("NEW");
        CPPUNIT_ASSERT(res == 1);    
        
        UploadSapiSyncItem* item = pictureSource.getNextNewItem(&err);
        CPPUNIT_ASSERT(item != NULL);
        
        delete item;

        item = pictureSource.getNextNewItem(&err);
        CPPUNIT_ASSERT(item == NULL);
        
        rename(file2.c_str(), file.c_str());
        
    }


    /// Create an OutputStream and write 1 string
    void addNewItemWithSameName() {
        ESapiSyncSourceError errCode;
        initMediaDir();

        StringBuffer s = initAdapter("sapi-test-pictures");
        DMTClientConfig* config = new DMTClientConfig(s);        
        config->read();

        int count = config->getSyncSourceConfigsCount();
        CPPUNIT_ASSERT(count > 0);
        
        SyncSourceConfig* ss;
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "picture") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);
 
        StringBuffer file = dirMedia;
        file.append("/");
        file.append("toRename.tiff");
        
        StringBuffer file2(file);
        file2.append(".jpg");

        rename(file.c_str(), file2.c_str());

        SyncSourceReport rep("picture");        
        MediaSapiSyncSource pictureSource(*ss, rep, 0, 0);
        
        pictureSource.beginSync(true, *config);
        
        // adding new Item
        SapiSyncItemInfo info; 
        info.setGuid("110");
        info.setSize(10);
        info.setName("test.jpg");

        SapiSyncItemInfo info2; 
        info2.setGuid("111");
        info2.setSize(10);
        info2.setName("test.jpg");

        DownloadSapiSyncItem* download = pictureSource.createItem(info);
        CPPUNIT_ASSERT(download != NULL);

        OutputStream* ostream = download->getStream();
        ostream->write("1234567890", 10);
        StringBuffer luid = pictureSource.addItem(download, &errCode);
        info.setLuid(luid);
        delete download;
        
        download = pictureSource.createItem(info2);
        CPPUNIT_ASSERT(download != NULL);

        ostream = download->getStream();
        ostream->write("1234567890", 10);
        luid = pictureSource.addItem(download, &errCode);
        delete download;        
        info2.setLuid(luid);

        pictureSource.deleteItem(info);
        pictureSource.deleteItem(info2);
        pictureSource.endSync();             
        
        rename(file2.c_str(), file.c_str());
        
    }
    /**
     * To test the sort move the method of the ssource to public
     *
    void sortListByModDate(){

        initMediaDir();

        StringBuffer s = initAdapter("sapi-test-pictures");
        DMTClientConfig* config = new DMTClientConfig(s);
        config->read();

        int count = config->getSyncSourceConfigsCount();
        CPPUNIT_ASSERT(count > 0);

        SyncSourceConfig* ss;
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "picture") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);

        SyncSourceReport rep("picture");
        MediaSapiSyncSource pictureSource(*ss, rep, 0);

        SapiSyncItemInfo info1;             // 2
        info1.setGuid("1");
        info1.setModificationDate(11);      

        SapiSyncItemInfo info2;             // 4
        info2.setGuid("2");
        info2.setModificationDate(21);
        
        SapiSyncItemInfo info3;             // 3
        info3.setGuid("3");
        info3.setModificationDate(17);
        
        SapiSyncItemInfo info4;             // 0
        info4.setGuid("4");
        info4.setModificationDate(1);
        
        SapiSyncItemInfo info5;             // 1 
        info5.setGuid("5");
        info5.setModificationDate(8);
        
        SapiSyncItemInfo info6;             // 5
        info6.setGuid("6");
        info6.setModificationDate(25);

        ArrayListEnumeration* list = new ArrayListEnumeration();

        list->add(info1);
        list->add(info2);
        list->add(info3);
        list->add(info4);
        list->add(info5);
        list->add(info6);

        pictureSource.sortByModificationTime(list);

        SapiSyncItemInfo* testInfo = (SapiSyncItemInfo*) list->get(0);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "4") == 0 );       
        testInfo = (SapiSyncItemInfo*) list->get(1);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "5") == 0 );
        testInfo = (SapiSyncItemInfo*) list->get(2);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "1") == 0 );
        testInfo = (SapiSyncItemInfo*) list->get(3);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "3") == 0 );
        testInfo = (SapiSyncItemInfo*) list->get(4);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "2") == 0 );
        testInfo = (SapiSyncItemInfo*) list->get(5);
        CPPUNIT_ASSERT(strcmp(testInfo->getGuid(), "6") == 0 );

    }
    */

    /// Create an OutputStream and write 1 string
    void allItem_delItem() {
        
        ESapiSyncSourceError errCode;
        initConfigTree();               
       
        StringBuffer s = initAdapter("sapi-test-pictures");
        DMTClientConfig* config = new DMTClientConfig(s);        
        config->read();

        int count = config->getSyncSourceConfigsCount();
        SyncSourceConfig* ss;
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "picture") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);

        int err;
        SyncSourceReport rep("picture");        
        MediaSapiSyncSource pictureSource(*ss, rep, 0, 0);
        cleanup(pictureSource);

        int numFiles = 0;
        readDir(dirMedia.c_str(), &numFiles, true);
        printf("%s", dirMedia.c_str());
        pictureSource.beginSync(false, *config);
        int res = pictureSource.getItemsNumber("ALL");
        CPPUNIT_ASSERT_EQUAL(numFiles, res);    
        
        UploadSapiSyncItem* item = pictureSource.getNextItem(&err);
        count = 1;
        while (item != NULL) {             
            pictureSource.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_ADD);
            delete item;
            item = pictureSource.getNextItem(&err);
            if (item) {
                count++;
            }
        }
        CPPUNIT_ASSERT_EQUAL(numFiles, count);
        
        ss->setLast((unsigned long)time(NULL));
        config->saveSyncSourceConfig("picture");

        // adding new Item
        SapiSyncItemInfo info; 
        info.setGuid("110");
        info.setSize(10);
        info.setName("test.jpg");
        DownloadSapiSyncItem* download = pictureSource.createItem(info);
        CPPUNIT_ASSERT(download != NULL);

        OutputStream* ostream = download->getStream();
        ostream->write("1234567890", 10);
        StringBuffer luid = pictureSource.addItem(download, &errCode);
        delete download;
        pictureSource.endSync();
        info.setLuid(luid);

        pictureSource.deleteItem(info);
        pictureSource.endSync();        
        
        ss->setLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, (unsigned long)time(NULL));
        config->saveSyncSourceConfig("picture");
    }


    void localStorageAbsolute() {

        // create config
        StringBuffer sourceName("picture");
        ArrayList sourceList;
        sourceList.add(sourceName);
        SyncManagerConfig* config = getNewSyncManagerConfig("FileSapiSyncSourceTest", true, &sourceList);
        CPPUNIT_ASSERT (config != NULL);
        SyncSourceConfig* ssc = config->getSyncSourceConfig("picture");
        CPPUNIT_ASSERT (ssc != NULL);
        SyncSourceReport ssr("picture");

        StringBuffer folderPath = getTestDirFullPath("FileSapiSyncSourceTest");
        ssc->setProperty(PROPERTY_FOLDER_PATH, folderPath.c_str());
        ssc->setProperty(PROPERTY_LOCAL_QUOTA_STORAGE, "10");   // 10 MB

        MediaSapiSyncSource source(*ssc, ssr, 0, 0);

        int err = 0;
        unsigned long long itemSize = 6 * 1000 * 1000;          // 6 MB
        bool accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(true, accepted);

        itemSize = 12 * 1000 * 1000;                            // 12 MB
        accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(false, accepted);

        source.populateAllItemInfoList(*config);
        long folderSize = source.getFolderSize();
        long spaceLeft = (10 * 1000 * 1000) - folderSize;

        itemSize = spaceLeft - 2 * 1000 * 1000;                 // 2 MB < space left
        accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(true, accepted);

        itemSize = spaceLeft + 2 * 1000 * 1000;                 // 2 MB > space left
        accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(false, accepted);

        delete config;
    }

    void localStoragePercentage() {

        // create config
        StringBuffer sourceName("picture");
        ArrayList sourceList;
        sourceList.add(sourceName);
        SyncManagerConfig* config = getNewSyncManagerConfig("FileSapiSyncSourceTest", true, &sourceList);
        CPPUNIT_ASSERT (config != NULL);
        SyncSourceConfig* ssc = config->getSyncSourceConfig("picture");
        CPPUNIT_ASSERT (ssc != NULL);
        SyncSourceReport ssr("picture");

        StringBuffer folderPath = getTestDirFullPath("FileSapiSyncSourceTest");
        ssc->setProperty(PROPERTY_FOLDER_PATH, folderPath.c_str());
        ssc->setProperty(PROPERTY_LOCAL_QUOTA_STORAGE, "99%");  // error if free space on disk is 1% or less

        unsigned long long totalBytes = 0, freeBytes = 0;
        int ret = getFreeDiskSpace(folderPath.c_str(), &totalBytes, &freeBytes);
        if (ret) {
            // error in API or not-implemented: abort test
            return;
        }

        if ( (freeBytes*100 / totalBytes) <= 1) {
            // Not enough free space on disk to run this test!
            return;
        }

        unsigned long long toRemainFree = (unsigned long long)(totalBytes * 0.01);    // 1% of tot space
        unsigned long long maxSize = freeBytes - toRemainFree;

        MediaSapiSyncSource source(*ssc, ssr, 0, 0);

        int err = 0;
        unsigned long long itemSize = maxSize + 1000*1000;         // item 1MB bigger
        bool accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(false, accepted);

        itemSize = maxSize - 1000*1000;                            // item 1MB smaller
        accepted = source.isLocalStorageAvailable(itemSize, &err);
        CPPUNIT_ASSERT(err == 0);
        CPPUNIT_ASSERT_EQUAL(true, accepted);

        delete config;
    }

    /// Cleans up the source's cache/mappings/resume tables.
    void cleanup(MediaSapiSyncSource& source) {

        source.getMappings().removeAllProperties();
        source.getMappings().close();

        source.getCache().removeAllProperties();
        source.getCache().close();

        source.getResume().removeAllProperties();
        source.getResume().close();
     }

    
};

CPPUNIT_TEST_SUITE_REGISTRATION( MediaSapiSyncSourceTest );

