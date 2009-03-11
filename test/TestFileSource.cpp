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

#include "TestFileSource.h"

#ifdef ENABLE_INTEGRATION_TESTS

USE_NAMESPACE

TestFileSource::TestFileSource(const std::string &id) :
        ClientTest(getenv("CLIENT_TEST_DELAY") ? atoi(getenv("CLIENT_TEST_DELAY")) : 0,
                   getenv("CLIENT_TEST_LOG") ? getenv("CLIENT_TEST_LOG") : ""), clientID(id) 
{

        const char *sourcelist = getenv("CLIENT_TEST_SOURCES");
        const char *server =     getenv("CLIENT_TEST_SERVER");
        const char *serverUrl =  getenv("CLIENT_TEST_SERVER_URL");
        const char *username =   getenv("CLIENT_TEST_USERNAME");
        const char *password =   getenv("CLIENT_TEST_PASSWORD");
    
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
        std::string root = std::string("client-test/") + server + "_" + clientID;
    
        PlatformAdapter::init(root.c_str(), true);
        config.reset(new DMTClientConfig());
        //config.reset(new DMTClientConfig(root.c_str()));
        config->read();
        DeviceConfig &dc(config->getDeviceConfig());
        if (!strlen(dc.getDevID())) {
            // no configuration yet
            config->setClientDefaults();
            dc.setDevID(id == "A" ? "sc-api-nat" : "sc-pim-ppc");
        }
        
        //set custom configuration
        if(serverUrl) {
            config->getAccessConfig().setSyncURL(serverUrl);
        }
        if(username) {
            config->getAccessConfig().setUsername(username);
        }
        if(password) {
            config->getAccessConfig().setPassword(password);
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


void TestFileSource::getSourceConfig(int source, Config &config) {
    memset(&config, 0, sizeof(config));

    getTestData(sources[source].c_str(), config);
    config.createSourceA =
    config.createSourceB = createSource;
}


int TestFileSource::sync(
        const int *activeSources,
        SyncMode syncMode,
        const CheckSyncReport &checkReport,
        long maxMsgSize,
        long maxObjSize,
        bool loSupport,
        const char *encoding) {
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

SyncSource *TestFileSource::createSource(int source, const char *trackingSuffix) {
        class TestFileSyncSourceWithReport : public TestFileSyncSource {
        public:
            TestFileSyncSourceWithReport(const char* nodeName, const WCHAR* name, SyncSourceConfig* sc) :
                TestFileSyncSource(name, sc),
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
        TestFileSyncSource *ss = new TestFileSyncSourceWithReport(
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

#endif // ENABLE_INTEGRATION_TESTS
/** @} */
/** @endcond */
