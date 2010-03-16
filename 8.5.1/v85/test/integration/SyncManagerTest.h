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

#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DefaultConfigFactory.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

#include "integration/TestSyncSource.h"
#include "common/http/TransportAgentReplacement.h"

BEGIN_NAMESPACE

/**
 * This class is intended to test the SyncManager class, the core of sync process.
 *
 * Tests implemented:
 * - testServerError506: checks for a loop in SyncManager::sync() simulating an excetion Server Side (fixed in v.8.0)
 */
class SyncManagerTest : public CppUnit::TestFixture {


public:

    SyncManagerTest() {}

    void runAllTests() {
        testServerError506();
        testLargeObject2();
    }

    /**
     * Checks for a loop in SyncManager::sync(), because of a missing <Final> 
     * tag in case of Server error while inserting items (fixed in v.8.0).
     *
     * The test:
     * - Sync 2 sources (contacts and calendar), with many items for calendar (50).
     * - Set a low max_msg_size (5k), so that calendar items are split in multimessage
     * - Use a defined TransportAgent (TransportAgentTestError506) to capture the 3rd message from
     *   Server and modify it to simulate an exception Server side (error 506)
     * Test passes if the sync does not loop infinite.
     */
    void testServerError506();

    /**
     * Test Large object issue (bug #7794) with a Replace command split in 2 msg, 
     * and also a Delete command in the second msg. If fails, the Server commands
     * are not parsed in the correct order by Client APIs (fixed in v8SP1).
     */
    void testLargeObject2();
};



/**
 * This transportAgent will check all SyncML messages and do 2 things:
 * - replace the 3rd message with a "506" error status on the Sync command
 *   (simulates an exception Server side)
 * - checks if the sync is in infinite loop (checks the msgID value)
 */
class TransportAgentTestError506 : public TransportAgentReplacement {

protected:

    /**
     * Checks the MsgID, to understand if the sync ended in a infinite loop.
     * @param msgToSend [IN-OUT] the syncML message formatted by SyncManager, to send to the Server
     */
    void beforeSendingMessage(StringBuffer& msgToSend);

    /**
     * Modifies the msg returned by the Server as a response to Clients calendar modifications,
     * to simulate an exception Server side (code 506).
     * @param msgReceived [IN-OUT] the syncML message received from Server, to be returned to SyncManager
     */
    void afterReceivingResponse(StringBuffer& msgReceived);

public:

    TransportAgentTestError506(URL& url, 
                              Proxy& proxy, 
                              unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT,
                              unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE) 
                              : TransportAgentReplacement(url, proxy, responseTimeout, maxmsgsize) {}

};


/**
 * Used by testLargeObject2 test: It's reimplemented in order to simulate a Server 
 * communication: the syncML responses are read from xml files under testcases/testLargeObject2 dir.
 */
class TransportAgentTestLargeObject2 : public TransportAgentReplacement {

protected:

    // nothing to do, just return
    void beforeSendingMessage  (StringBuffer& msgToSend)   { return; }
    void afterReceivingResponse(StringBuffer& msgReceived) { return; }

public:

    TransportAgentTestLargeObject2(URL& url, 
                              Proxy& proxy, 
                              unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT,
                              unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE) 
                              : TransportAgentReplacement(url, proxy, responseTimeout, maxmsgsize) {}

    /**
     * The main method, to send/receive messages.
     * It's reimplemented in order to simulate a Server communication: the syncML responses
     * are read from xml files under testcases/testLargeObject2 dir.
     */
    char* sendMessage(const char* msg);
};


/**
 * Used by testLargeObject2 test: extends TestSyncSource, redefines the updateItem
 * method in order to check the items are correctly joined by SyncManager.
 */
class  SyncSourceTestLargeObject2 : public TestSyncSource {

public:

    SyncSourceTestLargeObject2(const WCHAR* name, SyncSourceConfig *sc, int numItems = 10) : TestSyncSource(name, sc, numItems) {}

    /// Checks if the vCard received can be correctly parsed
    int updateItem(SyncItem& item);
};



END_NAMESPACE
#endif // ENABLE_INTEGRATION_TESTS
