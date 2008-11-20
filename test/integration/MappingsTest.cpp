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

#ifdef WIN32
#define Sleep       Sleep
#define INTERVAL    1000
#else
#define Sleep       sleep
#define INTERVAL    1
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
#include "MappingTestSyncSource.h"
//#include "spds/DataTransformerFactory.h"
//#include "spds/DefaultConfigFactory.h"
#include "spds/MappingsManager.h"
#include "spds/MappingStoreBuilder.h"
#include "MappingsTest.h"

USE_NAMESPACE

static StringBuffer FULLNAME_CON;
static StringBuffer FULLNAME_CON_JOUR;

/**
* This is an implementation of a derived class of the MappingStoreBuider.
* In the createNewInstance there is the choice to create a new MappingStore Builder.
* In this case the difference is only to have a different name for the same
* KeyValueStore PropertyFile.
* It could be to have different KeyValueStore for different source
*/
class CustomMappingStoreBuilder : public MappingStoreBuilder {
        
    KeyValueStore* createNewInstance(const char* name) const {
        
        if (strcmp(name, "contact") == 0) {
            StringBuffer fullName = PlatformAdapter::getHomeFolder();
            fullName += "/"; fullName += name;
            fullName += ".custom_contact";
            FULLNAME_CON = fullName;
            FULLNAME_CON_JOUR = FULLNAME_CON; FULLNAME_CON_JOUR += ".jour";
            return new PropertyFile(fullName);
        } else {
            //
            // implementation of a new KeyValueStore like
            // a SQLiteKeyValueStore....
            // returning NULL will make fail the SyncManager
            //
            return NULL;        
        }        
    }
};


bool existsFile(const char* name) {
    bool found = false;
    FILE* f = fopen(name, "r");
    if (f) {
        found = true;
        fclose(f);
    }
    return found;
}


MappingsTest::MappingsTest() {}
        

DMTClientConfig* getConfiguration(const char* name) {
    DMTClientConfig* config = new DMTClientConfig(name);
    config->read();
    DeviceConfig &dc(config->getDeviceConfig());
    if (!strlen(dc.getDevID())) {            
        config->setClientDefaults();
        config->setSourceDefaults("contact"); 
        StringBuffer devid("sc-pim-"); devid += name;
        dc.setDevID(devid);
        SyncSourceConfig* s = config->getSyncSourceConfig("contact");
        s->setEncoding("bin");
        s->setURI("card");
        s->setType("text/x-vcard");
    }
    return config;
}
    
    // Add 3 contacts from the source first to the server

void MappingsTest::testAdd3Contacts() {
        
        DMTClientConfig* config1 = getConfiguration("funambol_mappings_first");
        SyncSourceConfig *ccontact1 = config1->getSyncSourceConfig("contact");          
        CPPUNIT_ASSERT(ccontact1);
        ccontact1->setSync("two-way");
        config1->save();
        config1->open();

        CustomMappingStoreBuilder* custom = new CustomMappingStoreBuilder();
        MappingsManager::setBuilder(custom);        
        
        MappingTestSyncSource  scontact1(TEXT("contact"),  ccontact1);

        SyncSource* sources[2];
        sources[0] = &scontact1;
        sources[1] = NULL;        
                
        SyncClient client;
        int ret = 0;       
        ret = client.sync(*config1, sources);
        CPPUNIT_ASSERT(!ret);
        config1->save();

        SyncSourceReport *ssr = scontact1.getReport();
        int added = ssr->getItemReportSuccessfulCount(SERVER, COMMAND_ADD);

        CPPUNIT_ASSERT_EQUAL(3, added);  
        MappingsManager::setBuilder(NULL);
    }
    
    //
    // The second source syncs and fails at the first turn. Then it sends the
    // mappings and finish succesfully
    //
void MappingsTest::testMappings() {

        testPutClientServerInSync();
        Sleep(INTERVAL);
        testAdd3Contacts();
        Sleep(INTERVAL);

        DMTClientConfig* config2 = getConfiguration("funambol_mappings_second");
        SyncSourceConfig *ccontact2 = config2->getSyncSourceConfig("contact");          
        CPPUNIT_ASSERT(ccontact2);
        ccontact2->setSync("two-way");
        config2->save();
        config2->open();
        
        // other way to set the custom. Not used here just because the MappingsManager
        // delete the one set by the client if it is set two times
        //CustomMappingStoreBuilder custom;
        //MappingsManager::setBuilder(&custom);        
        
        CustomMappingStoreBuilder* custom = new CustomMappingStoreBuilder();
        MappingsManager::setBuilder(custom);

        MappingTestSyncSource  scontact2(TEXT("contact"),  ccontact2);

        SyncSource* sources[2];
        sources[0] = &scontact2;
        sources[1] = NULL;        
        
        SyncItemListenerClient* itemListener = new SyncItemListenerClient();    
        setSyncItemListener(itemListener);
                        
        SyncClient client;
        int ret = 0;
        try {
            client.sync(*config2, sources);            
        } catch (...) {            
            CPPUNIT_ASSERT(existsFile(FULLNAME_CON_JOUR));                    
        }

        // unregister the listener to avoid the exception they throw
        unsetSyncItemListener();
        Sleep(INTERVAL);
        ret = client.sync(*config2, sources);
        CPPUNIT_ASSERT(!ret);
        CPPUNIT_ASSERT(!existsFile(FULLNAME_CON));        
        config2->save();

        SyncSourceReport *ssr = scontact2.getReport();  
        int added = ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_ADD);
        
        // currently the client check the server sends back 3 items. But when the server
        // has a fix, this test should fails and it must be fixed properly. the item the 
        // server sends should be 1
        // CPPUNIT_ASSERT_EQUAL(3, added);
        // 2008-10-08 fixed applied on dog_food as server test
        CPPUNIT_ASSERT_EQUAL(1, added);
        
        delete config2;           
        MappingsManager::setBuilder(NULL);

    }

    //
    // Put client and server in sync with no item on the server through a 
    // refresh from client
    //
void MappingsTest::testPutClientServerInSync() { 
       // create the first configuration
        DMTClientConfig* config1 = getConfiguration("funambol_mappings_first");
        DMTClientConfig* config2 = getConfiguration("funambol_mappings_second");

        SyncSourceConfig *ccontact1 = config1->getSyncSourceConfig("contact");          
        CPPUNIT_ASSERT(ccontact1);
        ccontact1->setSync("refresh-from-client");

        SyncSourceConfig *ccontact2 = config2->getSyncSourceConfig("contact");                                
        CPPUNIT_ASSERT(ccontact2);
        ccontact2->setSync("refresh-from-client");
        
        config1->save();
        config1->open();

        config2->save();
        config2->open();
                 
        MappingTestSyncSource  scontact1(TEXT("contact"),  ccontact1);
        MappingTestSyncSource  scontact2(TEXT("contact"),  ccontact2);

        SyncSource* sources[2];
        sources[0] = &scontact1;
        sources[1] = NULL;        
                
        SyncClient client;
        int ret = 0;       
        ret = client.sync(*config1, sources);
        CPPUNIT_ASSERT(!ret);
        config1->save();
        Sleep(INTERVAL);
        sources[0] = &scontact2;

        ret = client.sync(*config2, sources);
        CPPUNIT_ASSERT(!ret);
        config2->save();
        Sleep(INTERVAL);
        delete config1; delete config2;

    }


#endif // ENABLE_INTEGRATION_TESTS
