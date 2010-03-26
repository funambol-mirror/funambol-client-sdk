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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_INTEGRATION_TESTS

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/globalsdef.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/adapter/PlatformAdapter.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "LOSyncSource.h"
#include "LOItemTest.h"
#include "spds/SyncManager.h"
#include "testUtils.h"
#include "client/FileSyncSource.h"

USE_NAMESPACE


LOItemTest::LOItemTest() {
    LOG.setLogName("LOItemTest.log");
    LOG.reset();
}

bool isSuccessful(const int status) {
    if (status == 201 || status == 200)
        return true;
    else
        return false;
}

int getOperationSuccessful(SyncSourceReport *ssr, const char* target, const char* command) {
    ArrayList* list = ssr->getList(target, command);
    ItemReport* e;
    
    // Scan for successful codes
    int good = 0;
    if (list->size() > 0) {
        e = (ItemReport*)list->front();
        if ( isSuccessful(e->getStatus()) ) {
            good++;            
        }
        for (int i=1; i<list->size(); i++) {
            e = (ItemReport*)list->next();            
            if ( isSuccessful(e->getStatus())) {
                good++;
            }
        }
    }
    return good;
}
int getSuccessfullyAdded(SyncSourceReport *ssr) {
    return getOperationSuccessful(ssr, SERVER, COMMAND_ADD);
}
int getSuccessfullyReplaced(SyncSourceReport *ssr) {
    return getOperationSuccessful(ssr, SERVER, COMMAND_REPLACE);
}

int getSuccessfullyDeleted(SyncSourceReport *ssr) {
    return getOperationSuccessful(ssr, SERVER, COMMAND_DELETE);
}

DMTClientConfig* LOItemTest::resetItemOnServer(const char* sourceURI) {
    
    ArrayList asources;
    StringBuffer contact("contact");
    asources.add(contact);
    DMTClientConfig* config = getNewDMTClientConfig("funambol_LOItem", true, &asources);
    CPPUNIT_ASSERT(config);
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("refresh-from-client");
    conf->setURI(sourceURI);

    config->getAccessConfig().setMaxMsgSize(5500);                       
    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    

    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;

    SyncClient client;
    int ret = 0;
    ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);
    config->save();

    delete scontact;
    return config;

}


void LOItemTest::testLOItem() {
    
    DMTClientConfig* config = resetItemOnServer("card");
    CPPUNIT_ASSERT(config);
    config->read();    
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf); 
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);
}

void LOItemTest::testLOItemDES() {
    
    DMTClientConfig* config = resetItemOnServer("scard");
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setEncryption("des");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
    
    CPPUNIT_ASSERT_EQUAL(2, added);
 
}

void LOItemTest::testLOItemWithItemEncoding() {
    
    DMTClientConfig* config = resetItemOnServer("card");
    CPPUNIT_ASSERT(config);
    config->read();    
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf); 
    scontact->setUseAdd(true);
    scontact->setUseDataEncoding(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);
}


void LOItemTest::testLOItemb64() {
           
    DMTClientConfig* config = resetItemOnServer("scard");
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
    
    CPPUNIT_ASSERT_EQUAL(2, added);
    config->save();

}

void LOItemTest::testLOItemReplaceb64() {
    
    testLOItemb64();
    initAdapter("funambol_LOItem");    
    DMTClientConfig* config = new DMTClientConfig();    
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(false);
    scontact->setUseUpdate(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);

}


void LOItemTest::testLOItemSlowSync() {

    DMTClientConfig* config = resetItemOnServer("card");

    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("slow");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);   
    scontact->setUseSlowSync(true);
    
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int replaced = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, replaced);
}

void LOItemTest::testLOItemSlowSyncb64() {

    DMTClientConfig* config = resetItemOnServer("scard");

    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("slow");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);   
    scontact->setUseSlowSync(true);
    
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int replaced = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, replaced);
}

void LOItemTest::testFileSyncSource() {
    
    StringBuffer test_dir("./FileToSync");
    StringBuffer test_name("LOItemTest");    

    // empty the test dir if exists
    removeFileInDir(test_dir.c_str());

    ArrayList asources;
    StringBuffer briefcase("briefcase");
    asources.add(briefcase);
    DMTClientConfig* config = getNewDMTClientConfig("funambol_LOItem", true, &asources);
    CPPUNIT_ASSERT(config);
    config->getAccessConfig().setMaxMsgSize(5500);
    SyncSourceConfig *conf = config->getSyncSourceConfig(briefcase.c_str());
    conf->setSync("refresh-from-client");
    conf->setURI(briefcase.c_str());    
    
    // put 4 files inside the test dir
    createFolder(test_dir.c_str());
    for (int i = 0; i < 4; i++) {
        StringBuffer s; s.sprintf("sif%i.txt", i);
        StringBuffer path = getTestFileFullPath(test_name.c_str(), s.c_str());
        char* content = loadTestFile(test_name.c_str(), s.c_str(), true);
        struct stat st;
        stat(path.c_str(), &st);
        int size = st.st_size;
       
        StringBuffer name(test_dir);
        name.append("/");
        name.append(s);
        bool written = saveFile(name.c_str(), content, size, true) ;
        delete [] content;
    }
    FileSyncSource* fsss = new FileSyncSource(TEXT("briefcase"), conf, test_dir);       

    SyncSource* sources[2];
    sources[0] = fsss;
    sources[1] = NULL;
    
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);
    
    SyncSourceReport *ssr = fsss->getReport();
    int replaced = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(4, replaced);
    config->save();
    
    delete fsss;
     
    // remove 2 items and add a new one
    removeFileInDir(test_dir.c_str(), "sif0.txt"); 
    removeFileInDir(test_dir.c_str(), "sif1.txt"); 
    
    StringBuffer path = getTestFileFullPath(test_name.c_str(), "vcard0.txt");
    char* content = loadTestFile(test_name.c_str(), "vcard0.txt", true);
    struct stat st;
    stat(path.c_str(), &st);
    int size = st.st_size;
   
    StringBuffer name(test_dir);
    name.append("/");
    name.append("vcard0.txt");
    bool written = saveFile(name.c_str(), content, size, true) ;
    delete [] content;
    
    config->read();
    conf = config->getSyncSourceConfig(briefcase.c_str());
    conf->setSync("two-way");
   
    fsss = new FileSyncSource(TEXT("briefcase"), conf, test_dir);

    sources[0] = fsss;
    sources[1] = NULL;
    
    ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);
    
    ssr = fsss->getReport();
    int added = getSuccessfullyAdded(ssr);
    int deleted = getSuccessfullyDeleted(ssr);
    CPPUNIT_ASSERT_EQUAL(1, added);
    CPPUNIT_ASSERT_EQUAL(2, deleted);
    
    
}
#endif // ENABLE_INTEGRATION_TESTS
