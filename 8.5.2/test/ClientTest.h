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

#ifndef INCL_TESTSYNCCLIENT
#define INCL_TESTSYNCCLIENT
/** @cond API */
/** @addtogroup Test */
/** @{ */

#include <string>
#include <vector>
#include <list>
#include "spds/SyncSource.h"
#include "spds/SyncReport.h"

#ifdef ENABLE_INTEGRATION_TESTS

#include <cppunit/TestSuite.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include "base/globalsdef.h"
#include "MappingsTest.h"
#include "ConfigSyncSourceTest.h"
#include "SyncManagerTest.h"
#include "LOItemTest.h"

USE_NAMESPACE

/**
 * This class encapsulates logging and checking of a SyncReport.
 * When constructed with default parameters, no checking will be done.
 * Otherwise the sync report has to contain exactly the expected result.
 * When multiple sync sources are active, @b all of them have to behave
 * alike (which is how the tests are constructed).
 *
 * No item is ever supposed to fail.
 */
class CheckSyncReport {
  public:
    CheckSyncReport(int clAdded = -1, int clUpdated = -1, int clDeleted = -1,
                    int srAdded = -1, int srUpdated = -1, int srDeleted = -1) :
        clientAdded(clAdded),
        clientUpdated(clUpdated),
        clientDeleted(clDeleted),
        serverAdded(srAdded),
        serverUpdated(srUpdated),
        serverDeleted(srDeleted)
        {}

    virtual ~CheckSyncReport() {}

    const int clientAdded, clientUpdated, clientDeleted,
        serverAdded, serverUpdated, serverDeleted;

    /**
     * checks that the sync completed as expected and throws
     * CPPUnit exceptions if something is wrong
     *
     * @param res     return code from SyncClient::sync()
     * @param report  the sync report stored in the SyncClient
     */
    virtual void check(int res, SyncReport &report) const;
};

class LocalTests;
class SyncTests;

/**
 * This is the interface expected by the testing framework for sync
 * clients.  It defines several methods that a derived class must
 * implement if it wants to use that framework. Note that this class
 * itself is not derived from SyncClient. This gives a user of this
 * framework the freedom to implement it in two different ways:
 * - implement a class derived from both SyncClient and ClientTest
 * - add testing of an existing subclass of SyncClient by implementing
 *   a ClientTest which uses that subclass
 *
 * The client is expected to support change tracking for multiple
 * servers. Although the framework always always tests against the
 * same server, for most tests it is necessary to access the database
 * without affecting the next synchronization with the server. This is
 * done by asking the client for two different sync sources via
 * Config::createSourceA and Config::createSourceB which have to
 * create them in a suitable way - pretty much as if the client was
 * synchronized against different server. A third, different change
 * tracking is needed for real synchronizations of the data.
 *
 * Furthermore the client is expected to support multiple data sources
 * of the same kind, f.i. two different address books. This is used to
 * test full client A <-> server <-> client B synchronizations in some
 * tests or to check server modifications done by client A with a
 * synchronization against client B. In those tests client A is mapped
 * to the first data source and client B to the second one.
 *
 * Finally the SyncSource API is used in slightly different ways which
 * go beyond what is normally expected from a SyncSource implementation:
 * - beginSync() may be called without setting a sync mode:
 *   when SyncSource::getSyncMode() returns SYNC_NONE the source is
 *   expected to make itself ready to iterate over all, new, updated and
 *   deleted items
 * - items may be added via SyncSource::addItem() with a type of "raw":
 *   this implies that the type is the one used for items in the
 *   ClientTest::Config below
 *
 * Handling configuration and creating classes is entirely done by the
 * subclass of ClientTest, the frameworks makes no assumptions
 * about how this is done. Instead it queries the ClientTest for
 * properties (like available sync sources) and then creates several
 * tests.
 */
class ClientTest {
  public:
    ClientTest(int serverSleepSec = 0, const std::string &serverLog= "");
    virtual ~ClientTest();

    /**
     * This function registers tests using this instance of ClientTest for
     * later use during a test run.
     *
     * The instance must remain valid until after the tests were
     * run. To run them use a separate test runner, like the one from
     * client-test-main.cpp.
     */
    virtual void registerTests();

    struct Config;

    /**
     * Creates an instance of LocalTests (default implementation) or a
     * class derived from it.  LocalTests provides tests which cover
     * the SyncSource interface and can be executed without a SyncML
     * server. It also contains utility functions for working with
     * SyncSources.
     *
     * A ClientTest implementation can, but doesn't have to extend
     * these tests by instantiating a derived class here.
     */
    virtual LocalTests *createLocalTests(const std::string &name, int sourceParam, ClientTest::Config &co);

    /**
     * Creates an instance of SyncTests (default) or a class derived
     * from it.  SyncTests provides tests which cover the actual
     * interaction with a SyncML server.
     *
     * A ClientTest implementation can, but doesn't have to extend
     * these tests by instantiating a derived class here.
     */
    virtual SyncTests *createSyncTests(const std::string &name, std::vector<int> sourceIndices, bool isClientA = true);

    /**
     * utility function for dumping items which are C strings with blank lines as separator
     */
    static int dump(ClientTest &client, SyncSource &source, const char *file);

    /**
     * utility function for importing items with blank lines as separator
     */
    static int import(ClientTest &client, SyncSource &source, const char *file);

    /**
     * utility function for comparing vCard and iCal files with the external
     * synccompare.pl Perl script
     */
    static bool compare(ClientTest &client, const char *fileA, const char *fileB);

    /**
     * A derived class can use this call to get default test
     * cases, but still has to add callbacks which create sources
     * and execute a sync session.
     *
     * Some of the test cases are compiled into the library, other
     * depend on the auxiliary files from the "test" directory.
     * Currently supported types:
     * - vcard30 = vCard 3.0 contacts
     * - vcard21 = vCard 2.1 contacts
     * - ical20 = iCal 2.0 events
     * - vcal10 = vCal 1.0 events
     * - itodo20 = iCal 2.0 tasks
     */
    static void getTestData(const char *type, Config &config);

    /**
     * Information about a data source. For the sake of simplicity all
     * items pointed to are owned by the ClientTest and must
     * remain valid throughout a test session. Not setting a pointer
     * is okay, but it will disable all tests that need the
     * information.
     */
    struct Config {
        /**
         * The name is used in test names and has to be set.
         */
        const char *sourceName;

        /**
         * A default URI to be used when creating a client config.
         */
        const char *uri;

        /**
         * A member function of a subclass which is called to create a
         * sync source referencing the data. This is used in tests of
         * the SyncSource API itself as well as in tests which need to
         * modify or check the data sources used during synchronization.
         *
         * The test framework will call beginSync() and then some of
         * the functions it wants to test. After a successful test it
         * will call endSync() which is then expected to store all
         * changes persistently. Creating a sync source again
         * with the same call should not report any
         * new/updated/deleted items until such changes are made via
         * another sync source.
         *
         * The instance will be deleted by the caller. Because this
         * may be in the error case or in an exception handler,
         * the sync source's desctructor should not thow exceptions.
         *
         * @param client    the same instance to which this config belongs
         * @param source    index of the data source (from 0 to ClientTest::getNumSources() - 1)
         * @param isSourceA true if the requested SyncSource is the first one accessing that
         *                  data, otherwise the second
         */
        typedef SyncSource *(*createsource_t)(ClientTest &client, int source, bool isSourceA);

        /**
         * Creates a sync source which references the primary database;
         * it may report the same changes as the sync source used during
         * sync tests.
         */
        createsource_t createSourceA;

        /**
         * A second sync source also referencing the primary data
         * source, but configured so that it tracks changes
         * independently from the the primary sync source.
         *
         * In local tests the usage is like this:
         * - add item via first SyncSource
         * - iterate over new items in second SyncSource
         * - check that it lists the added item
         *
         * In tests with a server the usage is:
         * - do a synchronization with the server
         * - iterate over items in second SyncSource
         * - check that the total number and number of
         *   added/updated/deleted items is as expected
         */
        createsource_t createSourceB;

        /**
         * The framework can generate vCard and vCalendar/iCalendar items
         * automatically by copying a template item and modifying certain
         * properties.
         *
         * This is the template for these automatically generated items.
         */
        const char *templateItem;

         /**
         * This is a colon (:) separated list of properties which need
         * to be modified in templateItem.
         */
        const char *uniqueProperties;

        /**
         * the number of items to create during stress tests
         */
        int numItems;

        /**
         * This is a single property in templateItem which can be extended
         * to increase the size of generated items.
         */
        const char *sizeProperty;

        /**
         * A very simple item that is inserted during basic tests. Ideally
         * it only contains properties supported by all servers.
         */
        const char *insertItem;

        /**
         * A slightly modified version of insertItem. If the source has UIDs
         * embedded into the item data, then both must have the same UID.
         * Again all servers should better support these modified properties.
         */
        const char *updateItem;

        /**
         * A more heavily modified version of insertItem. Same UID if necessary,
         * but can test changes to items only supported by more advanced
         * servers.
         */
        const char *complexUpdateItem;

        /**
         * To test merge conflicts two different updates of insertItem are
         * needed. This is the first such update.
         */
        const char *mergeItem1;

        /**
         * The second merge update item. To avoid true conflicts it should
         * update different properties than mergeItem1, but even then servers
         * usually have problems perfectly merging items. Therefore the
         * test is run without expecting a certain merge result.
         */
        const char *mergeItem2;

        /**
         * These two items are related: one is main one, the other is
         * a subordinate one. The semantic is that the main item is
         * complete on it its own, while the other normally should only
         * be used in combination with the main one.
         *
         * Because SyncML cannot express such dependencies between items,
         * a SyncSource has to be able to insert, updated and remove
         * both items independently. However, operations which violate
         * the semantic of the related items (like deleting the parent, but
         * not the child) may have unspecified results (like also deleting
         * the child). See LINKED_ITEMS_RELAXED_SEMANTIC.
         *
         * One example for main and subordinate items are a recurring
         * iCalendar 2.0 event and a detached recurrence.
         */
        const char *parentItem, *childItem;

        /**
         * define to 0 to disable tests which slightly violate the
         * semantic of linked items by inserting children
         * before/without their parent
         */
#ifndef LINKED_ITEMS_RELAXED_SEMANTIC
# define LINKED_ITEMS_RELAXED_SEMANTIC 1
#endif

        /**
         * called to dump all items into a file, required by tests which need
         * to compare items
         *
         * ClientTest::dump can be used: it will simply dump all items of the source
         * with a blank line as separator.
         *
         * @param source     sync source A already created and with beginSync() called
         * @param file       a file name
         * @return error code, 0 for success
         */
        int (*dump)(ClientTest &client, SyncSource &source, const char *file);

        /**
         * import test items: which these are is determined entirely by
         * the implementor, but tests work best if several complex items are
         * imported
         *
         * ClientTest::import can be used if the file contains items separated by
         * empty lines.
         *
         * @param source     sync source A already created and with beginSync() called
         * @param file       the name of the file to import
         * @return error code, 0 for success
         */
        int (*import)(ClientTest &client, SyncSource &source, const char *file);

        /**
         * a function which compares two files with items in the format used by "dump"
         *
         * @param fileA      first file name
         * @param fileB      second file name
         * @return true if the content of the files is considered equal
         */
        bool (*compare)(ClientTest &client, const char *fileA, const char *fileB);

        /**
         * a file with test cases in the format expected by import and compare
         */
        const char *testcases;

        /**
         * the item type normally used by the source (not used by the tests
         * themselves; client-test.cpp uses it to initialize source configs)
         */
        const char *type;
    };

    /**
     * Data sources are enumbered from 0 to n-1 for the purpose of
     * testing. This call returns n.
     */
    virtual int getNumSources() = 0;

    /**
     * Called to fill the given test source config with information
     * about a sync source identified by its index. It's okay to only
     * fill in the available pieces of information and set everything
     * else to zero.
     */
    virtual void getSourceConfig(int source, Config &config) = 0;

    /**
     * The instance to use as second client. Returning NULL disables
     * all checks which require a second client. The returned pointer
     * must remain valid throughout the life time of the tests.
     *
     * The second client must be configured to access the same server
     * and have data sources which match the ones from the primary
     * client.
     */
    virtual ClientTest *getClientB() = 0;

    /**
     * Returning true enables tests which only work if the server is
     * a Funambol server which supports the "b64" encoding of items
     * on the transport level.
     */
    virtual bool isB64Enabled() = 0;

    /**
     * Execute a synchronization with the selected sync sources
     * and the selected synchronization options. The log file
     * in LOG has been set up already for the synchronization run
     * and should not be changed by the client.
     *
     * @param activeSources a -1 terminated array of sync source indices
     * @param syncMode     the synchronization mode to be used
     * @param checkReport  has to be called after a successful or unsuccessful sync,
     *                     will dump the report and (optionally) check the result;
     *                     beware, the later may throw exceptions inside CPPUNIT macros
     * @param maxMsgSize   >0: enable the maximum message size, else disable it
     * @param maxObjSize   same as maxMsgSize for maximum object size
     * @param loSupport    if true, then the sync is expected to enable Large Object support
     * @param encoding     if non-empty, then let client library transform all items
     *                     into this format (guaranteed to be not NULL)
     *
     * @return return code of SyncClient::sync()
     */
    virtual int sync(
        const int *activeSources,
        SyncMode syncMode,
        const CheckSyncReport &checkReport,
        long maxMsgSize = 0,
        long maxObjSize = 0,
        bool loSupport = false,
        const char *encoding = "") = 0;

    /**
     * This is called after successful sync() calls (res == 0) as well
     * as after unsuccessful ones (res != 1). The default implementation
     * sleeps for the number of seconds specified when constructing this
     * instance and copies the server log if one was named.
     *
     * @param res       result of sync()
     * @param logname   base name of the current sync log (without ".client.[AB].log" suffix)
     */
    virtual void postSync(int res, const std::string &logname);

  protected:
    /**
     * time to sleep in postSync()
     */
    int serverSleepSeconds;

    /**
     * server log file which is copied by postSync() and then
     * truncated (Unix only, Windows does not allow such access
     * to an open file)
     */
    std::string serverLogFileName;

  private:
    /**
     * really a CppUnit::TestFactory, but declared as void * to avoid
     * dependencies on the CPPUnit header files: created by
     * registerTests() and remains valid until the client is deleted
     */
    void *factory;
};

/**
 * helper class to encapsulate ClientTest::Config::createsource_t
 * pointer and the corresponding parameters
 */
class CreateSource {
public:
    CreateSource(ClientTest::Config::createsource_t createSourceParam, ClientTest &clientParam, int sourceParam, bool isSourceAParam) :
        createSource(createSourceParam),
        client(clientParam),
        source(sourceParam),
        isSourceA(isSourceAParam) {}

    SyncSource *operator() () {
        CPPUNIT_ASSERT(createSource);
        return createSource(client, source, isSourceA);
    }

    const ClientTest::Config::createsource_t createSource;
    ClientTest &client;
    const int source;
    const bool isSourceA;
};


/**
 * local test of one sync source and utility functions also used by
 * sync tests
 */
class LocalTests : public CppUnit::TestSuite, public CppUnit::TestFixture {
public:
    /** the client we are testing */
    ClientTest &client;

    /** number of the source we are testing in that client */
    const int source;

    /** configuration that corresponds to source */
    const ClientTest::Config config;

    /** helper funclets to create sources */
    CreateSource createSourceA, createSourceB;

    LocalTests(const std::string &name, ClientTest &cl, int sourceParam, ClientTest::Config &co) :
        CppUnit::TestSuite(name),
        client(cl),
        source(sourceParam),
        config(co),
        createSourceA(co.createSourceA, cl, sourceParam, true),
        createSourceB(co.createSourceB, cl, sourceParam, false)
        {}

    /**
     * adds the supported tests to the instance itself;
     * this is the function that a derived class can override
     * to add additional tests
     */
    virtual void addTests();

    /**
     * opens source and inserts the given item; can be called
     * regardless whether the data source already contains items or not
     *
     * The type of the item is unset; it is assumed that the source
     * can handle that.
     *
     * @param relaxed   if true, then disable some of the additional checks after adding the item
     * @return the UID of the inserted item
     */
    virtual std::string insert(CreateSource createSource, const char *data, bool relaxed = false);

    /**
     * assumes that exactly one element is currently inserted and updates it with the given item
     *
     * The type of the item is cleared, as in insert() above.
     *
     * @param check     if true, then reopen the source and verify that the reported items are as expected
     */
    virtual void update(CreateSource createSource, const char *data, bool check = true);

    /** deletes all items locally via sync source */
    virtual void deleteAll(CreateSource createSource);

    /**
     * takes two databases, exports them,
     * then compares them using synccompare
     *
     * @param refFile      existing file with source reference items, NULL uses a dump of sync source A instead
     * @param copy         a sync source which contains the copied items, begin/endSync will be called
     * @param raiseAssert  raise assertion if comparison yields differences (defaults to true)
     */
    virtual void compareDatabases(const char *refFile, SyncSource &copy, bool raiseAssert = true);

    /**
     * insert artificial items, number of them determined by TEST_EVOLUTION_NUM_ITEMS
     * unless passed explicitly
     *
     * @param createSource    a factory for the sync source that is to be used
     * @param startIndex      IDs are generated starting with this value
     * @param numItems        number of items to be inserted if non-null, otherwise TEST_EVOLUTION_NUM_ITEMS is used
     * @param size            minimum size for new items
     * @return number of items inserted
     */
    virtual int insertManyItems(CreateSource createSource, int startIndex = 1, int numItems = 0, int size = -1);


    /* for more information on the different tests see their implementation */

    virtual void testOpen();
    virtual void testIterateTwice();
    virtual void testSimpleInsert();
    virtual void testLocalDeleteAll();
    virtual void testComplexInsert();
    virtual void testLocalUpdate();
    virtual void testChanges();
    virtual void testImport();
    virtual void testImportDelete();
    virtual void testManyChanges();
    virtual void testLinkedItemsParent();
    virtual void testLinkedItemsChild();
    virtual void testLinkedItemsParentChild();
    virtual void testLinkedItemsChildParent();
    virtual void testLinkedItemsChildChangesParent();
    virtual void testLinkedItemsRemoveParentFirst();
    virtual void testLinkedItemsRemoveNormal();
    virtual void testLinkedItemsInsertParentTwice();
    virtual void testLinkedItemsInsertChildTwice();
    virtual void testLinkedItemsParentUpdate();
    virtual void testLinkedItemsUpdateChild();
    virtual void testLinkedItemsInsertBothUpdateChild();
    virtual void testLinkedItemsInsertBothUpdateParent();

};

enum itemType {
    NEW_ITEMS,
    UPDATED_ITEMS,
    DELETED_ITEMS,
    TOTAL_ITEMS
};

/**
 * utility function which counts items of a certain kind known to the sync source
 * @param source      valid source ready to iterate; NULL triggers an assert
 * @param itemType    determines which iterator functions are used
 * @return number of valid items iterated over
 */
int countItemsOfType(SyncSource *source, itemType type);

typedef std::list<std::string> UIDList;
/**
 * generates list of UIDs in the specified kind of items
 */
UIDList listItemsOfType(SyncSource *source, itemType type);

/**
 * Tests synchronization with one or more sync sources enabled.
 * When testing multiple sources at once only the first config
 * is checked to see which tests can be executed.
 */
class SyncTests : public CppUnit::TestSuite, public CppUnit::TestFixture {
public:
    /** the client we are testing */
    ClientTest &client;

    SyncTests(const std::string &name, ClientTest &cl, std::vector<int> sourceIndices, bool isClientA = true);
    ~SyncTests();

    /** adds the supported tests to the instance itself */
    virtual void addTests();

protected:
    /** list with all local test classes for manipulating the sources and their index in the client */
    std::vector< std::pair<int, LocalTests *> > sources;
    typedef std::vector< std::pair<int, LocalTests *> >::iterator source_it;

    /** the indices from sources, terminated by -1 (for sync()) */
    int *sourceArray;

    /** utility functions for second client */
    SyncTests *accessClientB;

    /** Listeners for all sync tests */
    SyncItemListener* itemListener;
    // TODO: add other listeners

    enum DeleteAllMode {
        DELETE_ALL_SYNC,   /**< make sure client and server are in sync,
                              delete locally,
                              sync again */
        DELETE_ALL_REFRESH /**< delete locally, refresh server */
    };

    /** compare databases of first and second client */
    virtual void compareDatabases();

    /** deletes all items locally and on server */
    virtual void deleteAll(DeleteAllMode mode = DELETE_ALL_SYNC);

    /** get both clients in sync with empty server, then copy one item from client A to B */
    virtual void doCopy();

    /**
     * replicate server database locally: same as SYNC_REFRESH_FROM_SERVER,
     * but done with explicit local delete and then a SYNC_SLOW because some
     * servers do no support SYNC_REFRESH_FROM_SERVER
     */
    virtual void refreshClient();

    /* for more information on the different tests see their implementation */

    // do a two-way sync without additional checks
    virtual void testTwoWaySync() {
        sync(SYNC_TWO_WAY);
    }

    // do a slow sync without additional checks
    virtual void testSlowSync() {
        sync(SYNC_SLOW);
    }
    // do a refresh from server sync without additional checks
    virtual void testRefreshFromServerSync() {
        sync(SYNC_REFRESH_FROM_SERVER);
    }

    // do a refresh from client sync without additional checks
    virtual void testRefreshFromClientSync() {
        sync(SYNC_REFRESH_FROM_CLIENT);
    }

    // delete all items, locally and on server using two-way sync
    virtual void testDeleteAllSync() {
        deleteAll(DELETE_ALL_SYNC);
    }

    virtual void testDeleteAllRefresh();
    virtual void testRefreshSemantic();
    virtual void testRefreshStatus();

    // test that a two-way sync copies an item from one address book into the other
    void testCopy() {
        doCopy();
        compareDatabases();
    }

    virtual void testUpdate();
    virtual void testComplexUpdate();
    virtual void testDelete();
    virtual void testMerge();
    virtual void testTwinning();
    virtual void testOneWayFromServer();
    virtual void testOneWayFromClient();
    virtual void testItems();
    virtual void testAddUpdate();


    // test copying with maxMsg and no large object support
    void testMaxMsg() {
        doVarSizes(true, false, NULL);
    }
    // test copying with maxMsg and large object support
    void testLargeObject() {
        doVarSizes(true, true, NULL);
    }
    // test copying with maxMsg and large object support using explicit "bin" encoding
    void testLargeObjectBin() {
        doVarSizes(true, true, "bin");
    }
    // test copying with maxMsg and large object support using B64 encoding
    void testLargeObjectEncoded() {
        doVarSizes(true, true, "b64");
    }

    virtual void testManyItems();

    // Check the tests
    virtual void testMappings() {
        MappingsTest mapping;
        mapping.runTests();        
    }

    virtual void testConfigSyncSource() {
        ConfigSyncSourceTest css;
        css.runTests();        
    }

    virtual void testServerError506() {
        SyncManagerTest test;
        test.testServerError506();        
    }

    virtual void testLargeObject2() {
        SyncManagerTest test;
        test.testLargeObject2();        
    }

    virtual void testLOItem() {
        LOItemTest test;
        test.testLOItem();        
    }
    virtual void testLOItemb64() {
        LOItemTest test;               
        test.testLOItemb64();        
    }
    virtual void testLOItemSlowSync() {
        LOItemTest test;               
        test.testLOItemSlowSync();        
    }
    virtual void testLOItemSlowSyncb64() {
        LOItemTest test;               
        test.testLOItemSlowSyncb64();        
    }
    virtual void testLOItemReplaceb64() {
        LOItemTest test;               
        test.testLOItemReplaceb64();
    }
    
    virtual void testLOItemWithItemEncoding() {
        LOItemTest test;               
        test.testLOItemWithItemEncoding();
    }
    virtual void testLOItemDES() {
        LOItemTest test;               
        test.testLOItemDES();
    }
    virtual void testFileSyncSource() {
        LOItemTest test;               
        test.testFileSyncSource();
    }
    
    /**
     * implements testMaxMsg(), testLargeObject(), testLargeObjectEncoded()
     * using a sequence of items with varying sizes
     */
    virtual void doVarSizes(bool withMaxMsgSize,
                            bool withLargeObject,
                            const char *encoding);

    /**
     * executes a sync with the given options,
     * checks the result and (optionally) the sync report
     */
    virtual void sync(SyncMode syncMode,
                      const std::string &logprefix = "",
                      CheckSyncReport checkReport = CheckSyncReport(),
                      long maxMsgSize = 0,
                      long maxObjSize = 0,
                      bool loSupport = false,
                      const char *encoding = "");
};


/** assert equality, include string in message if unequal */
#define CLIENT_TEST_EQUAL( _prefix, \
                           _expected, \
                           _actual ) \
    CPPUNIT_ASSERT_EQUAL_MESSAGE( std::string(_prefix) + ": " + #_expected + " == " + #_actual, \
                                  _expected, \
                                  _actual )

/** execute _x and then check the status of the _source pointer */
#define SOURCE_ASSERT_NO_FAILURE(_source, _x) \
{ \
    CPPUNIT_ASSERT_NO_THROW(_x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** check _x for true and then the status of the _source pointer */
#define SOURCE_ASSERT(_source, _x) \
{ \
    CPPUNIT_ASSERT(_x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** check that _x evaluates to a specific value and then the status of the _source pointer */
#define SOURCE_ASSERT_EQUAL(_source, _value, _x) \
{ \
    CPPUNIT_ASSERT_EQUAL(_value, _x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** same as SOURCE_ASSERT() with a specific failure message */
#define SOURCE_ASSERT_MESSAGE(_message, _source, _x)     \
{ \
    CPPUNIT_ASSERT_MESSAGE((_message), (_x)); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}


/**
 * convenience macro for adding a test name like a function,
 * to be used inside addTests() of an instance of that class
 *
 * @param _class      class which contains the function
 * @param _function   a function without parameters in that class
 */
#define ADD_TEST(_class, _function) \
    addTest(new CppUnit::TestCaller<_class>(getName() + "::" #_function, &_class::_function, *this))


#endif // ENABLE_INTEGRATION_TESTS


/** @} */
/** @endcond */
#endif // INCL_TESTSYNCCLIENT
