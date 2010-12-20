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
#include "spds/SyncMLProcessor.h"
#include "syncml/parser/Parser.h"
#include "client/DMTClientConfig.h"

#include "testUtils.h"


USE_NAMESPACE

class DMTClientConfigTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(DMTClientConfigTest);
    CPPUNIT_TEST(testDataStoresWithSlash);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {
        // will also init the adapter
        config = getNewDMTClientConfig("DMTClientConfigTest", true);
    }
    void tearDown() {
        delete config;
    }

private:

    DMTClientConfig* config;


    /**
     * Utility: will process Server devInf from file 'filename', 
     * store data in the config
     */
    void processDevInf(const char* filename) {

        char* xml = loadTestFile(".", filename);

        ArrayList commands;
        Parser::getCommands(commands, xml);
        CPPUNIT_ASSERT_MESSAGE("Failed to parse devInf Results", (commands.size() == 1) );

        // Process the Server devInf
        SyncMLProcessor syncMLProcessor;
        AbstractCommand* cmd = (AbstractCommand*)commands.get(0);
        bool found = syncMLProcessor.processServerDevInf(cmd, *config);
        CPPUNIT_ASSERT(found);

        delete [] xml;
    }


    /**
     * Tests the validity of saveDataStores() method, in case a datastore name contains
     * a slash char "/" that needs to be correctly escaped by ManagementNode. 
     */
    void testDataStoresWithSlash() {

        // this file has a datastore named: "./contacts"
        processDevInf("devInfResults2.xml");

        // Save and read the config
        config->save();
        config->read();

        // Check datastores: 1 expected "./contacts"
        const ArrayList* dataStores = config->getServerDataStores();
        CPPUNIT_ASSERT(dataStores);
        CPPUNIT_ASSERT(dataStores->size() == 1);
        DataStore* dataStore = (DataStore*)dataStores->get(0);
        CPPUNIT_ASSERT(dataStore);

        StringBuffer value = dataStore->getSourceRef()->getValue();
        CPPUNIT_ASSERT(value == "./contacts");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( DMTClientConfigTest );
