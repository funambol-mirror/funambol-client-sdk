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

/** @cond API */
/** @addtogroup ClientTest */
/** @{ */

#include "spdm/DeviceManagementNode.h"
#include "client/RawFileSyncSource.h"
#include "spds/spdsutils.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "test/ClientTest.h"
#include "base/test.h"

#include <string>
#include <vector>
#include <iomanip>
#include <memory>

#ifdef WIN32
#include <direct.h>
#endif
#include <sys/stat.h>
#include "base/globalsdef.h"

USE_NAMESPACE

#ifdef ENABLE_INTEGRATION_TESTS

/**
 * This code uses the ClientTest and RawFileSyncSource to test real
 * synchronization against a server. More than one RawFileSyncSource can
 * be active at once and each of them may (but does not have to be)
 * used for different kinds of data. The name of the source determines
 * which data is stored in it: it must be something supported by the
 * ClientTest class, because that is where the test data comes from.
 *
 * At least the following kinds of data are currently supported by the
 * ClientTest class (see ClientTest::getTestData() for more
 * information):
 * - vcard30 = vCard 3.0 contacts
 * - vcard21 = vCard 2.1 contacts
 * - ical20 = iCalendar 2.0 calendar events
 * - vcal10 = vCalendar 1.0 calendar events
 * - itodo20 = iCalendar 2.0 tasks
 *
 * Configuration is done by environment variables which indicate which
 * part below the root node "client-test" of the the configuration tree to use;
 * beyond that everything needed for synchronization is read from the
 * configuration tree.
 *
 * - CLIENT_TEST_SERVER = maps to name of root node in configuration tree
 * - CLIENT_TEST_SOURCES = comma separated list of active sources,
 *                         names as listed above
 * - CLIENT_TEST_DELAY = number of seconds to sleep between syncs, required
 *                       by some servers
 * - CLIENT_TEST_LOG = logfile name of a server, can be empty:
 *                     if given, then the content of that file will be
 *                     copied and stored together with the client log
 *                     (only works on Unix)
 * - CLIENT_TEST_NUM_ITEMS = numbers of contacts/events/... to use during
 *                           local and sync tests which create artificial
 *                           items
 *
 * For example, on Linux running
 * @verbatim
CLIENT_TEST_SERVER=funambol CLIENT_TEST_SOURCES=vcard21,vcal10 ./client-test
@endverbatim
 *
 * expects the following configuration layout:
 * @verbatim
~/.sync4j/client-test/
                      funambol_1/spds/
                                      syncml/config.text
                                      sources/
                                              vcard21/config.txt
                                              vcal10/config.txt
                      funambol_1/spds/
                                      <same as for funambol_1>
@endverbatim
 *
 * If any of the configuration nodes does not exist yet, then it will
 * be created, but further information may have to be added, in
 * particular:
 * - server URL
 * - server user name, password
 * - sources uri
 *
 * The CLIENT_TEST_SERVER also has another meaning: it is used as hint
 * by the synccompare.pl script and causes it to automatically ignore
 * known, acceptable data modifications caused by sending an item to
 * a server and back again. Currently the script recognizes "funambol",
 * "scheduleworld", "synthesis" and "egroupware" as special server
 * names.
 *
 * The two configurations are used to simulate synchronization between
 * two different clients.
 *
 * The file sources will store their items in sub directories of
 * a "client-data" directory created in the current working directory.
 *
 * Here is an example of using the CLIENT_TEST_LOG:
 * @verbatim
CLIENT_TEST_SERVER=funambol \
CLIENT_TEST_LOG=/opt/Funambol-3.0/ds-server/logs/funambol_ds.log \
CLIENT_TEST_SOURCES=vcard21 \
   ./client-test
@endverbatim
 *
 * will create files with the suffix .client.A.log for synchronizations with
 * the first client and .client.B.log for the second client. The base name
 * of these files is unique, so the corresponding part of the server log
 * is stored with the same base name and .server.log as suffix.
 */
class TestFileSource : public ClientTest {
public:
    TestFileSource(const std::string &id) :
        ClientTest(getenv("CLIENT_TEST_DELAY") ? atoi(getenv("CLIENT_TEST_DELAY")) : 0,
                   getenv("CLIENT_TEST_LOG") ? getenv("CLIENT_TEST_LOG") : ""),
        clientID(id) {
        const char *sourcelist = getenv("CLIENT_TEST_SOURCES");
        const char *server = getenv("CLIENT_TEST_SERVER");

        /* set up source list */
        if (!sourcelist) {
            sourcelist = "";
        }
        const char *eostr = strchr(sourcelist, ',');
        const char *start = sourcelist;

        while (eostr) {
            sources.push_back(std::string(start, 0, eostr - start));
            start = eostr + 1;
            eostr = strchr(start, ',');
        }
        if (start[0]) {
            sources.push_back(start);
        }

        /* check server */
        if (!server) {
            server = "funambol";
        }

        // get configuration and set obligatory fields
        LOG.setLevel(LOG_LEVEL_DEBUG);
        std::string root = std::string("client-test/") + server + "_" + id;
        config.reset(new DMTClientConfig(root.c_str()));
        config->read();
        DeviceConfig &dc(config->getDeviceConfig());
        if (!strlen(dc.getDevID())) {
            // no configuration yet
            config->setClientDefaults();
            dc.setDevID(id == "A" ? "sc-api-nat" : "sc-pim-ppc");
        }
        for (int source = 0; source < (int)sources.size(); source++) {
            ClientTest::Config testconfig;
            getSourceConfig(source, testconfig);
            CPPUNIT_ASSERT(testconfig.type);

            SyncSourceConfig* sc = config->getSyncSourceConfig(sources[source].c_str());
            if (!sc) {
                // no configuration yet
                config->setSourceDefaults(sources[source].c_str());
                sc = config->getSyncSourceConfig(sources[source].c_str());
                sc->setURI(testconfig.uri);
                CPPUNIT_ASSERT(sc);
            }

            sc->setType(testconfig.type);
        }
        config->save();
        config->open();

        if (id == "A") {
            /* we are the primary client, create a second one */
            clientB.reset(new TestFileSource("B"));
        }
    }

    virtual int getNumSources() {
        return (int)sources.size();
    }

    virtual void getSourceConfig(int source, Config &config) {
        memset(&config, 0, sizeof(config));

        getTestData(sources[source].c_str(), config);
        config.createSourceA =
            config.createSourceB = createSource;
    }

    virtual ClientTest* getClientB() {
        return clientB.get();
    }

    virtual bool isB64Enabled() {
        return false;
    }

    virtual int sync(
        const int *activeSources,
        SyncMode syncMode,
        const CheckSyncReport &checkReport,
        long maxMsgSize,
        long maxObjSize,
        bool loSupport,
        const char *encoding = 0) {
        SyncSource **syncSources = new SyncSource *[sources.size() + 1];
        int index, numsources = 0;
        memset(syncSources, 0, sizeof(syncSources[0]) * (sources.size() + 1));

        for (index = 0; activeSources[index] >= 0 && index < (int)sources.size(); index++) {
            // rewrite configuration as needed for test
            SyncSourceConfig *sourceConfig = config->getSyncSourceConfig(sources[activeSources[index]].c_str());
            CPPUNIT_ASSERT(sourceConfig);
            sourceConfig->setSync(syncModeKeyword(syncMode));
            sourceConfig->setEncoding(encoding);
            config->getAccessConfig().setMaxMsgSize(maxMsgSize);
            config->getDeviceConfig().setMaxObjSize(maxObjSize);
            config->getDeviceConfig().setLoSupport(loSupport);

            // create sync source using the third change tracking for syncs
            syncSources[numsources++] = createSource(activeSources[index], "S");
        }

        SyncClient client;
        int res = client.sync(*config, syncSources);
        CPPUNIT_ASSERT(client.getSyncReport());

        for (int source = 0; syncSources[source]; source++) {
            delete syncSources[source];
        }
        checkReport.check(res, *client.getSyncReport());
        return res;
    }

private:
    /** either "A" or "B" for first respectively second client */
    std::string clientID;

    /** only in "A": pointer to second client */
    std::auto_ptr<TestFileSource> clientB;

    /** vector of enabled sync sources, identified by a name which SyncClient::getConfig() supports */
    std::vector<std::string> sources;

    /** configuration tree itself */
    std::auto_ptr<DMTClientConfig> config;

    static SyncSource *createSource(ClientTest &client, int source, bool isSourceA) {
        // hand work over to real member function
        return ((TestFileSource &)client).createSource(source, isSourceA ? "A" : "B");
    }

    SyncSource *createSource(int source, const char *trackingSuffix) {
        class RawFileSyncSourceWithReport : public RawFileSyncSource {
        public:
            RawFileSyncSourceWithReport(const char* nodeName, const WCHAR* name, SyncSourceConfig* sc) :
                RawFileSyncSource(name, sc),
                fileNode(nodeName) {
                setReport(&report);
                setFileNode(&fileNode);
                /*
                 * Keeping track if changes is done via time() with a resolution of seconds.
                 * Sleep a bit to ensure that enough time passes.
                 */
#ifdef WIN32
        Sleep(1000);
#else
                sleep(1);
#endif
            }
        private:
            SyncSourceReport report;
            DeviceManagementNode fileNode;
        };

        CPPUNIT_ASSERT(source < (int)sources.size());
        ManagementNode *sourceNode = config->getSyncSourceNode(sources[source].c_str());
        CPPUNIT_ASSERT(sourceNode);
        char *fullName = sourceNode->createFullName();
        std::string nodeName = std::string(fullName) + "/changes_" + trackingSuffix;
        std::string dirName = sources[source] + "_" + clientID;
        WCHAR *name = toWideChar(sources[source].c_str());
        delete [] fullName;
        FileSyncSource *ss = new RawFileSyncSourceWithReport(
            nodeName.c_str(),
            name,
            config->getSyncSourceConfig(sources[source].c_str()));
        delete [] name;
#ifdef WIN32
        _mkdir(dirName.c_str());
#else
    mkdir(dirName.c_str(), S_IRWXU);
#endif
        ss->setDir(dirName.c_str());

        return ss;
    }
};


/**
 * the only purpose of this class is to own the first TestFileSource
 * and to register its tests at program startup
 */
static class RegisterTest {
public:
    RegisterTest() :
        testFileSource("A") {
        testFileSource.registerTests();
    }

private:
    TestFileSource testFileSource;
} registerTest;

/** @} */
/** @endcond */
#endif // ENABLE_INTEGRATION_TESTS
