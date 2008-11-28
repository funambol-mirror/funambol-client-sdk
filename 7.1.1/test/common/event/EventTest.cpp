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

#include "base/globalsdef.h"
#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/TransportListener.h"
#include "event/ManageListener.h"
#include "event/FireEvent.h"

USE_NAMESPACE

#define TEST_ERROR_MSG "Error Message"

/**
 * This class performs some basic tests on the Events and Listeners framework
 * of the Funambol C++ SDK.
 */
class EventTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(EventTest);
    CPPUNIT_TEST(testSetListeners);
    CPPUNIT_TEST(testGetListeners);
    CPPUNIT_TEST(testSyncEvent);
    CPPUNIT_TEST(testSyncSourceEvent);
    CPPUNIT_TEST(testSyncItemEvent);
    CPPUNIT_TEST(testSyncStatusEvent);
    CPPUNIT_TEST(testTransportEvent);
    CPPUNIT_TEST(testUnsetListeners);
    CPPUNIT_TEST_SUITE_END();

public:

    /**
     * Test code for the listeners: used by all test listeners to check the number
     * of events received.
     */
    class ListenerTest {
    public:
        ListenerTest(int e) : counter(0), expected(e) {}

        void check() { CPPUNIT_ASSERT_EQUAL(counter, expected); }
        
    protected:
        int counter;
        const int expected;
    };

    /** Test class for the SyncListener */
    class TSyncListener : public SyncListener, public ListenerTest {
    public:

        TSyncListener(const char *name): SyncListener(name),ListenerTest(6) {};

        void syncBegin(SyncEvent& event) {
            CPPUNIT_ASSERT_EQUAL(event.getType(), SYNC_BEGIN);
            counter++;
        };
        void syncEnd(SyncEvent& event) {
            CPPUNIT_ASSERT_EQUAL(event.getType(), SYNC_END);
            counter++;
        };
        void sendInitialization(SyncEvent& event) {
            CPPUNIT_ASSERT_EQUAL(event.getType(), SEND_INITIALIZATION);
            counter++;
        };
        void sendModifications(SyncEvent& event) {
            CPPUNIT_ASSERT_EQUAL(event.getType(), SEND_MODIFICATION);
            counter++;
        };
        void sendFinalization(SyncEvent& event) {
            CPPUNIT_ASSERT_EQUAL(event.getType(), SEND_FINALIZATION);
            counter++;
        };
        void syncError(SyncEvent& event) {
            CPPUNIT_ASSERT(strcmp(TEST_ERROR_MSG, event.getMessage()) == 0);
            counter++;
        };

    }; 

    class TSyncItemListener : public SyncItemListener, public ListenerTest {
    public:

        TSyncItemListener(const char *name): SyncItemListener(name),ListenerTest(6) {};

        void itemAddedByServer  (SyncItemEvent& event) {counter++;};
        void itemDeletedByServer(SyncItemEvent& event) {counter++;};
        void itemUpdatedByServer(SyncItemEvent& event) {counter++;};
        void itemAddedByClient  (SyncItemEvent& event) {counter++;};
        void itemDeletedByClient(SyncItemEvent& event) {counter++;};
        void itemUpdatedByClient(SyncItemEvent& event) {counter++;};

    };

    class TSyncSourceListener : public SyncSourceListener, public ListenerTest {
    public:

        TSyncSourceListener(const char *name): SyncSourceListener(name),ListenerTest(5){};

        void syncSourceBegin             (SyncSourceEvent& event) {counter++;};
        void syncSourceEnd               (SyncSourceEvent& event) {counter++;};
        void syncSourceSyncModeRequested (SyncSourceEvent& event) {counter++;};
        void syncSourceTotalClientItems  (SyncSourceEvent& event) {counter++;};
        void syncSourceTotalServerItems  (SyncSourceEvent& event) {counter++;};
    };

    class TSyncStatusListener : public SyncStatusListener, public ListenerTest {
    public:

        TSyncStatusListener(const char *name): SyncStatusListener(name),ListenerTest(2){};

        void statusReceived(SyncStatusEvent& event) {counter++;};
        void statusSending (SyncStatusEvent& event) {counter++;};

    };

    class TTransportListener : public TransportListener, public ListenerTest {
    public:

        TTransportListener(const char *name): TransportListener(name),ListenerTest(5) {};

        void sendDataBegin   (TransportEvent& event) {counter++;};
        void sendDataEnd     (TransportEvent& event) {counter++;};
        void receiveDataBegin(TransportEvent& event) {counter++;};
        void receivingData   (TransportEvent& event) {counter++;};
        void receiveDataEnd  (TransportEvent& event) {counter++;};

    };

    // Setup the listeners.
    void setUp() {
        ManageListener& m = ManageListener::getInstance();

        m.setSyncListener(new TSyncListener("T1"));
        m.setSyncListener(new TSyncListener("T2"));

        m.setSyncItemListener(new TSyncItemListener("T1"));
        m.setSyncItemListener(new TSyncItemListener("T2"));

        m.setSyncSourceListener(new TSyncSourceListener("T1"));
        m.setSyncSourceListener(new TSyncSourceListener("T2"));

        m.setSyncStatusListener(new TSyncStatusListener("T1"));
        m.setSyncStatusListener(new TSyncStatusListener("T2"));

        m.setTransportListener(new TTransportListener("T1"));
        m.setTransportListener(new TTransportListener("T2"));
    }

    // Release all listeners at the end of the test.
    void tearDown() {
        ManageListener::releaseAllListeners();
    }

private:

    void testSetListeners() {
        ManageListener& m = ManageListener::getInstance();

        CPPUNIT_ASSERT_EQUAL(2, m.countSyncListeners());
        CPPUNIT_ASSERT_EQUAL(2, m.countSyncItemListeners());
        CPPUNIT_ASSERT_EQUAL(2, m.countSyncSourceListeners());
        CPPUNIT_ASSERT_EQUAL(2, m.countSyncStatusListeners());
        CPPUNIT_ASSERT_EQUAL(2, m.countTransportListeners());
    }

    void testGetListeners() {
        ManageListener& m = ManageListener::getInstance();

        CPPUNIT_ASSERT(m.getSyncListener("T1") != NULL);
        CPPUNIT_ASSERT(m.getSyncListener("T2") != NULL);
        
        CPPUNIT_ASSERT(m.getSyncItemListener("T1") != NULL);
        CPPUNIT_ASSERT(m.getSyncItemListener("T2") != NULL);
        
        CPPUNIT_ASSERT(m.getSyncStatusListener("T1") != NULL);
        CPPUNIT_ASSERT(m.getSyncStatusListener("T2") != NULL);
        
        CPPUNIT_ASSERT(m.getTransportListener("T1") != NULL);
        CPPUNIT_ASSERT(m.getTransportListener("T2") != NULL);
    }

    /** Test the SyncEvent */
    void testSyncEvent() {
        fireSyncEvent(NULL, SYNC_BEGIN);
        fireSyncEvent(NULL, SYNC_END);
        fireSyncEvent(NULL, SEND_INITIALIZATION);
        fireSyncEvent(NULL, SEND_MODIFICATION);
        fireSyncEvent(NULL, SEND_FINALIZATION);
        fireSyncEvent(TEST_ERROR_MSG, SYNC_ERROR);

        TSyncListener* s = 
            static_cast<TSyncListener*>(
                ManageListener::getInstance().getSyncListener("T1"));
        s->check();

        s = static_cast<TSyncListener*>(
            ManageListener::getInstance().getSyncListener("T2"));
        s->check();
    }

    /** Test the SyncSourceEvent */
    void testSyncSourceEvent() {
        const char *uri="testUri", *name="test";
        fireSyncSourceEvent(uri, name, SYNC_TWO_WAY, 0, SYNC_SOURCE_BEGIN);
        fireSyncSourceEvent(uri, name, SYNC_TWO_WAY, 0, SYNC_SOURCE_END);
        fireSyncSourceEvent(uri, name, SYNC_TWO_WAY, 0, SYNC_SOURCE_SYNCMODE_REQUESTED);
        fireSyncSourceEvent(uri, name, SYNC_TWO_WAY, 0, SYNC_SOURCE_TOTAL_CLIENT_ITEMS);
        fireSyncSourceEvent(uri, name, SYNC_TWO_WAY, 0, SYNC_SOURCE_TOTAL_SERVER_ITEMS);

        TSyncSourceListener* s = 
            static_cast<TSyncSourceListener*>(
                ManageListener::getInstance().getSyncSourceListener("T1"));
        s->check();

        s = static_cast<TSyncSourceListener*>(
            ManageListener::getInstance().getSyncSourceListener("T2"));
        s->check();
    }

    /** Test the SyncItemEvent */
    void testSyncItemEvent() {
        const char *uri="testUri", *name="test";
        const WCHAR *key=TEXT("key");

        fireSyncItemEvent(uri, name, key, ITEM_ADDED_BY_SERVER);
        fireSyncItemEvent(uri, name, key, ITEM_DELETED_BY_SERVER);
        fireSyncItemEvent(uri, name, key, ITEM_UPDATED_BY_SERVER);
        fireSyncItemEvent(uri, name, key, ITEM_ADDED_BY_CLIENT);
        fireSyncItemEvent(uri, name, key, ITEM_DELETED_BY_CLIENT);
        fireSyncItemEvent(uri, name, key, ITEM_UPDATED_BY_CLIENT);

        TSyncItemListener* s = 
            static_cast<TSyncItemListener*>(
                ManageListener::getInstance().getSyncItemListener("T1"));
        s->check();

        s = static_cast<TSyncItemListener*>(
            ManageListener::getInstance().getSyncItemListener("T2"));
        s->check();
    }

    /** Test the SyncStatusEvent */
    void testSyncStatusEvent() {
        const char *uri="testUri", *name="test";
        const WCHAR *key=TEXT("key");

        fireSyncStatusEvent("command", 200, name, uri, key, CLIENT_STATUS);
        fireSyncStatusEvent("command", 200, name, uri, key, SERVER_STATUS);

        TSyncStatusListener* s = 
            static_cast<TSyncStatusListener*>(
                ManageListener::getInstance().getSyncStatusListener("T1"));
        s->check();

        s = static_cast<TSyncStatusListener*>(
            ManageListener::getInstance().getSyncStatusListener("T2"));
        s->check();
    }

    /** Test the TransportEvent */
    void testTransportEvent() {

        fireTransportEvent(0, SEND_DATA_BEGIN);
        fireTransportEvent(0, SEND_DATA_END);
        fireTransportEvent(0, RECEIVE_DATA_BEGIN);
        fireTransportEvent(0, RECEIVE_DATA_END);
        fireTransportEvent(0, DATA_RECEIVED);

        TTransportListener* s = 
            static_cast<TTransportListener*>(
                ManageListener::getInstance().getTransportListener("T1"));
        s->check();

        s = static_cast<TTransportListener*>(
            ManageListener::getInstance().getTransportListener("T2"));
        s->check();
    }

    void testUnsetListeners() {
        ManageListener& m = ManageListener::getInstance();

        m.unsetSyncListener("T1");
        m.unsetSyncListener("T2");
        CPPUNIT_ASSERT_EQUAL(0, m.countSyncListeners());

        m.unsetSyncItemListener("T1");
        m.unsetSyncItemListener("T2");
        CPPUNIT_ASSERT_EQUAL(0, m.countSyncItemListeners());

        m.unsetSyncSourceListener("T1");
        m.unsetSyncSourceListener("T2");
        CPPUNIT_ASSERT_EQUAL(0, m.countSyncSourceListeners());

        m.unsetSyncStatusListener("T1");
        m.unsetSyncStatusListener("T2");
        CPPUNIT_ASSERT_EQUAL(0, m.countSyncStatusListeners());

        m.unsetTransportListener("T1");
        m.unsetTransportListener("T2");
        CPPUNIT_ASSERT_EQUAL(0, m.countTransportListeners());

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );
