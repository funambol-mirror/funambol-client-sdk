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
#include "client/ConfigSyncSource.h"
#include "spds/SyncItem.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTree.h"

#include "base/adapter/PlatformAdapter.h"
#include "client/DMTClientConfig.h"

#define APPLICATIONURI "funambol_syncsourceconfig"
#define VALUE "value@email.com"
#define VALUEMOD "valuemod@email.com"

USE_NAMESPACE

class ConfigSyncSourceUnitTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ConfigSyncSourceUnitTest);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testAllItems);
    CPPUNIT_TEST(testUpdate);
    CPPUNIT_TEST_SUITE_END();

public:


    void setUp() {
        PlatformAdapter::init(APPLICATIONURI, true);
        config = new DMTClientConfig();
        config->read();
        s = config->getSyncSourceConfig("config");
    }

    void tearDown() {
        delete config;
        delete s;
    }

private:
    //insert a value via Syncsource and we look into the DMTree if is correct
    void testInsert() {
        
        ConfigSyncSource source(TEXT("config"), APPLICATIONURI, s);
    
        SyncItem item(TEXT("./Email/Address"));
       
        item.setData(VALUE,(long)strlen(VALUE));

        source.insertItem(item);

        DMTree tree(APPLICATIONURI);
        ManagementNode* node = tree.getNode("Email");
        char* value = node->readPropertyValue("Address");
        delete node;
        CPPUNIT_ASSERT((strcmp(VALUE,value)==0));
        delete [] value; 

    }
    //test te getFirstItem. We pass all the values into the properties ArrayList
    //and we get the first one
    void testAllItems() {
        ArrayList properties;
        ConfigSyncSource source(TEXT("config"), APPLICATIONURI, s);
        
        StringBuffer emailaddress("./Email/Address");
        
        properties.add(emailaddress);
        source.setConfigProperties(properties);

        Enumeration* val = source.getAllItemList();
        SyncItem* value = source.getFirstItem();
        StringBuffer valuedata((const char*)value->getData(),value->getDataSize());
        CPPUNIT_ASSERT(valuedata == VALUE);
	delete val;
	delete value;
    }
    //Test the update of the syncItem. In the ConfigSyncSource the value in not
    //modified but inserted
    void testUpdate() {

        ConfigSyncSource source(TEXT("config"), APPLICATIONURI, s);
        SyncItem item(TEXT("./Email/Address"));
        item.setData(VALUEMOD,(long)strlen(VALUEMOD));

        source.modifyItem(item);

        DMTree tree(APPLICATIONURI);
        ManagementNode* node = tree.getNode("Email");
        char* value = node->readPropertyValue("Address");
	delete node;
        CPPUNIT_ASSERT((strcmp(VALUEMOD,value)==0));
        delete [] value;
    }
private:
    DMTClientConfig* config;
    SyncSourceConfig* s ;
};

CPPUNIT_TEST_SUITE_REGISTRATION( ConfigSyncSourceUnitTest );
