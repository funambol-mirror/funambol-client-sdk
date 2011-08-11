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

#include "sapi/FileSapiSyncSource.h"
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
class FileSapiSyncSourceTest : public CppUnit::TestFixture {
    
    CPPUNIT_TEST_SUITE(FileSapiSyncSourceTest);
    CPPUNIT_TEST(allItem_delItem);    
    CPPUNIT_TEST(addNewItem);
    CPPUNIT_TEST(modifyItem);  
    CPPUNIT_TEST(modifyIncomingItem);      
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
        sc->setName                 ("file");
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
        sc->setProperty             (PROPERTY_EXTENSION, ".txt");
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
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);
 
        StringBuffer file = dirMedia;
        file.append("/");
        file.append("docTest.bak");
        
        StringBuffer file2(file);
        file2.append(".txt");

        rename(file.c_str(), file2.c_str());

        int err;
        SyncSourceReport rep("file");        
        FileSapiSyncSource fileSource(*ss, rep, 0, 0);

        fileSource.beginSync(true, *config);
        int res = fileSource.getItemsNumber("NEW");
        CPPUNIT_ASSERT(res == 1);    
        
        UploadSapiSyncItem* item = fileSource.getNextNewItem(&err);
        CPPUNIT_ASSERT(item != NULL);
        
        delete item;

        item = fileSource.getNextNewItem(&err);
        CPPUNIT_ASSERT(item == NULL);
        
        rename(file2.c_str(), file.c_str());
        
    }

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
        SyncSourceReport rep("file");        
        FileSapiSyncSource fileSource(*ss, rep, 0, 0);
        cleanup(fileSource);

        int numFiles = 0;
        readDir(dirMedia.c_str(), &numFiles, true);
        
        fileSource.beginSync(false, *config);
        int res = fileSource.getItemsNumber("ALL");
        CPPUNIT_ASSERT_EQUAL(numFiles, res);    
        
        UploadSapiSyncItem* item = fileSource.getNextItem(&err);
        count = 1;
        while (item != NULL) {             
            fileSource.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_ADD);
            delete item;
            item = fileSource.getNextItem(&err);
            if (item) {
                count++;
            }
        }
        CPPUNIT_ASSERT_EQUAL(numFiles, count);
        
        ss->setLast((unsigned long)time(NULL));
        config->saveSyncSourceConfig("file");

        // adding new Item
        SapiSyncItemInfo info; 
        info.setGuid("110");
        info.setSize(10);
        info.setName("test1.txt");
        DownloadSapiSyncItem* download = fileSource.createItem(info);
        CPPUNIT_ASSERT(download != NULL);

        OutputStream* ostream = download->getStream();
        ostream->write("1234567890", 10);
        StringBuffer luid = fileSource.addItem(download, &errCode);
        delete download;
        fileSource.endSync();
        info.setLuid(luid);

        fileSource.deleteItem(info);
        fileSource.endSync();        
        
        ss->setLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, (unsigned long)time(NULL));
        config->saveSyncSourceConfig("file");
    }

    void modifyItem() {
        
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
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);
 
        StringBuffer file = dirMedia;
        file.append("/");
        file.append("docTest.bak");
        
        
        int err;
        SyncSourceReport rep("file");        
        FileSapiSyncSource fileSource(*ss, rep, 0, 0);
        cleanup(fileSource);

        int numFiles = 0;
        readDir(dirMedia.c_str(), &numFiles, true);

        fileSource.beginSync(false, *config);       
        int res = fileSource.getItemsNumber("ALL");
        CPPUNIT_ASSERT_EQUAL(numFiles, res);    
        
        UploadSapiSyncItem* item = fileSource.getNextItem(&err);
        count = 1;
        while (item != NULL) {             
            fileSource.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_ADD);
            delete item;
            item = fileSource.getNextItem(&err);
            if (item) {
                count++;
            }
        }
        CPPUNIT_ASSERT_EQUAL(numFiles, count);

        ss->setLast((unsigned long)time(NULL));
        config->saveSyncSourceConfig("file");
        fileSource.endSync();
       

        StringBuffer file2(dirMedia);
        file2.append("/");        
        file2.append("test1.txt");

        FILE *f = fileOpen(file2.c_str(), "w");
        fwrite("1234567890", 1, 10, f);
        fclose(f);
        
        config->read();
        count = config->getSyncSourceConfigsCount();
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        FileSapiSyncSource fileSource2(*ss, rep, 0, 0);
        fileSource2.beginSync(true, *config); 

        numFiles = fileSource2.getItemsNumber("NEW");

        item = fileSource2.getNextNewItem(&err);
        CPPUNIT_ASSERT(item != NULL);
        
        count = 1;
        while (item != NULL) {             
            fileSource2.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_ADD);
            delete item;
            item = fileSource2.getNextNewItem(&err);
            if (item) {
                count++;
            }
        }
        
        CPPUNIT_ASSERT_EQUAL(numFiles, count);
        fileSource2.endSync();
        
        // delay of one second to get the file modification now based on the modification time
        Sleep(1000);        
        
        f = fileOpen(file2.c_str(), "w");
        fwrite("12345678901234567890", 1, 20, f);
        fclose(f);
        
        config->read();
        count = config->getSyncSourceConfigsCount();
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }

        FileSapiSyncSource fileSource3(*ss, rep, 0, 0);
        fileSource3.beginSync(true, *config); 

        item = fileSource3.getNextModItem(&err);
        CPPUNIT_ASSERT(item != NULL);
        
        count = 1;
        while (item != NULL) {             
            fileSource3.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_REPLACE);
            delete item;
            item = fileSource3.getNextModItem(&err);
            if (item) {
                count++;
            }
        }
                
        fileSource3.endSync();
        cleanup(fileSource3);
        removeFileInDir(dirMedia, "test1.txt");
                
    }
    
    void modifyIncomingItem() {
        ESapiSyncSourceError errCode;
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
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        CPPUNIT_ASSERT(ss != NULL);
         
        int err;
        SyncSourceReport rep("file");        
        FileSapiSyncSource fileSource(*ss, rep, 0, 0);
        cleanup(fileSource);

        int numFiles = 0;
        readDir(dirMedia.c_str(), &numFiles, true);

        fileSource.beginSync(false, *config);       
        int res = fileSource.getItemsNumber("ALL");
        CPPUNIT_ASSERT_EQUAL(numFiles, res);    
        
        UploadSapiSyncItem* item = fileSource.getNextItem(&err);
        count = 1;
        while (item != NULL) {             
            fileSource.setItemStatus(*(item->getSapiSyncItemInfo()), 0, COMMAND_ADD);
            delete item;
            item = fileSource.getNextItem(&err);
            if (item) {
                count++;
            }
        }
        CPPUNIT_ASSERT_EQUAL(numFiles, count);

        ss->setLast((unsigned long)time(NULL));
        config->saveSyncSourceConfig("file");
        fileSource.endSync();
       
        config->read();
        count = config->getSyncSourceConfigsCount();
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }

        FileSapiSyncSource fileSource2(*ss, rep, 0, 0);
        fileSource2.beginSync(true, *config); 


        // adding new Item
        SapiSyncItemInfo info; 
        info.setGuid("110");
        info.setSize(10);
        info.setName("test1.txt");
        DownloadSapiSyncItem* download = fileSource2.createItem(info);
        CPPUNIT_ASSERT(download != NULL);

        OutputStream* ostream = download->getStream();
        ostream->write("1234567890", 10);
        StringBuffer luid = fileSource2.addItem(download, &errCode);
        delete download;
        fileSource2.endSync();
        info.setLuid(luid);
        
        
        config->read();
        count = config->getSyncSourceConfigsCount();
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        FileSapiSyncSource fileSource3(*ss, rep, 0, 0);
        fileSource3.beginSync(true, *config); 
        
        info.setSize(20);
        download = fileSource3.createItem(info);
        ostream = download->getStream();
        ostream->write("12345678901234567890", 20);
        StringBuffer luid2 = fileSource3.updateItem(download, &errCode);
        CPPUNIT_ASSERT_EQUAL(luid, luid2);
        fileSource3.endSync();

        // renaming
        StringBuffer resLuid = dirMedia;
        resLuid.append("/");
        resLuid.append("test2.txt");                

        config->read();
        count = config->getSyncSourceConfigsCount();
        for (int i = 0; i < count; i++) {
            ss = config->getSyncSourceConfig(i);
            if (strcmp(ss->getName(), "file") == 0) {
                break;
            }
        }
        FileSapiSyncSource fileSource4(*ss, rep, 0, 0);
        fileSource4.beginSync(true, *config); 
        
        info.setSize(20);
        info.setName("test2.txt");
        info.setRename(true);
        download = new DownloadSapiSyncItem(&info, NULL);
        luid2 = fileSource4.updateItem(download, &errCode);
        fileSource4.endSync();
        

        resLuid.replaceAll("\\","");
        resLuid.replaceAll("/","");
        luid2.replaceAll("\\","");
        luid2.replaceAll("/","");

        CPPUNIT_ASSERT_EQUAL(resLuid, luid2);
        
        cleanup(fileSource4);
        removeFileInDir(dirMedia, "test2.txt");
              
    }



    /// Cleans up the source's cache/mappings/resume tables.
    void cleanup(FileSapiSyncSource& source) {

        source.getMappings().removeAllProperties();
        source.getMappings().close();

        source.getCache().removeAllProperties();
        source.getCache().close();

        source.getResume().removeAllProperties();
        source.getResume().close();
     }


    
};

CPPUNIT_TEST_SUITE_REGISTRATION( FileSapiSyncSourceTest );

