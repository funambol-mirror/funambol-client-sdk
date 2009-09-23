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

#ifdef ENABLE_INTEGRATION_TESTS

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"
#include "spds/DefaultConfigFactory.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"
#include "spds/MappingsManager.h"
#include "spds/MappingStoreBuilder.h"

/**
* This test class need to test the cache mapping feature. It means the SyncManager is able to
* cache the mappings that he has to send to the server until he receives an acknowledge from the 
* server. Then the current cache is deleted. If there is something wrong and the sync is broken, at the next
* sync the client sends again the mappings cached from previous sync.
* Summary of the tests:
* There is a MappingTestSyncSource with doesn't send anything for a slow sync and 3 items for a two-way.
* There is a listener implementation that throw an exception after 3 items came on the client
*
* testPutClientServerInSync: a refresh-from-client to empty the server and leave 2 client in sync
*
* testAdd3Contacts: the first client adds 3 contacts
*
* testMappings: it uses the testPutClientServerInSync and testAdd3Contacts to create the proper scenario.
*    Then the second client executes a two-way sync. It sends 3 items (due to the SS implementation)
*    and then receives 3 items. At the second, the listener hooked throws an exception to simulate a break.
*    Then, after unregistering the listener, a new sync is performed by the client and the mappings are
*    properly sent. Also the files that takes care of the cache deleting the files
*
* SETTING CONFIGURATION
*
* The first time the test is ran, the config is created using the api factory. 
* Launching the test with CLIENT_TEST_SERVER=funambol CLIENT_TEST_SOURCES=vcard21 make check the default
* server is localhost with user/pass guest/guest. Set the proper in the .config/funambol_mappings_* (posix)
* or the windows registry (HKCU/Software).
*/

BEGIN_NAMESPACE

class ConfigSyncSourceTest : public CppUnit::TestFixture {    

public:

    ConfigSyncSourceTest();

    void runTests() {
        testConfigSource();
    }

private:    
           
    void testConfigSource();
};

END_NAMESPACE
#endif // ENABLE_INTEGRATION_TESTS
