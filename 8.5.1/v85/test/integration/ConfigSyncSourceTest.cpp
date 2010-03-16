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
/*
#ifdef WIN32
#define Sleep       Sleep
#define INTERVAL    1000
#else
#define Sleep       sleep
#define INTERVAL    1
#endif
*/
#ifdef ENABLE_INTEGRATION_TESTS

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/globalsdef.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/adapter/PlatformAdapter.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "ConfigSyncSourceTest.h"
    
#include "client/ConfigSyncSource.h"
#include "testUtils.h"
#include "spdm/DMTree.h"
#include "spdm/ManagementNode.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#define SOURCE_CONFIG_NAME "config"
#define CONTEXT "funambol_configSyncSourceIntegration"
//#define CONTEXT "funambol_mappings_first"

USE_NAMESPACE

ConfigSyncSourceTest::ConfigSyncSourceTest() {
    LOG.setLogName("syncsourceconfig_tests.log");
    LOG.reset();
}
        

DMTClientConfig* getConf(const char* name) {

    DMTClientConfig* config = getNewDMTClientConfig(name, true);
    
	SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(SOURCE_CONFIG_NAME);
	sc->setEncoding ("bin");
	sc->setType     ("text/plain");
	sc->setURI      ("configuration");
	config->setSyncSourceConfig(*sc);
	delete sc;

	config->save();

    
    return config;
}

void setpropertyconfig(const char* prop, const char* value){
    DMTree tree(CONTEXT);
    ManagementNode* node = tree.getNode(prop);
    node->setPropertyValue(value, "");
    delete node;
}

void ConfigSyncSourceTest::testConfigSource() {
   // create the first configuration
    DMTClientConfig* config1 = getConf(CONTEXT);

    SyncSourceConfig *csconfig = config1->getSyncSourceConfig(SOURCE_CONFIG_NAME);          
    CPPUNIT_ASSERT(csconfig);

    ConfigSyncSource source(TEXT("config"), CONTEXT, csconfig);
    ArrayList properties;

    StringBuffer pushstatus("./Push/Status");
    StringBuffer filterstatus("./Email/FilterStatus");
    StringBuffer emailaddress("./Email/Address");
    StringBuffer displayname("./Email/DisplayName");

    properties.add(pushstatus);
    properties.add(filterstatus);
    properties.add(emailaddress);
    properties.add(displayname);
    source.setConfigProperties(properties);

    
    setpropertyconfig("Push","Status");
    setpropertyconfig("Email","FilterStatus");
    setpropertyconfig("Email","Address");
    setpropertyconfig("Email","DisplayName");

    SyncSource* sources[2];
    sources[0] = &source;
    sources[1] = NULL;        
            
    SyncClient client;
    int ret = 0;       
    ret = client.sync(*config1, sources);
    config1->save();
    
    CPPUNIT_ASSERT(ret == 0 || ret == 404);
    if (ret == 404) {
        std::cout << "ConfigSyncSource doesn't exists: Test Passed but ret = 404 ";        
        std::cout.flush();
    }
    delete config1; 

}


#endif // ENABLE_INTEGRATION_TESTS
