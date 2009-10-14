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

#include "testUtils.h"
#include "base/adapter/PlatformAdapter.h"
#include "base/util/utils.h"


#define TEST_ROOT_CONTEXT   "Funambol/client-test"
#define DEFAULT_TEST_NAME   "cppTest"

// This is the dir where all the testcases are located
#ifdef _WIN32
# define TESTCASES_DIR "../../test/testcases/"
#else
# define TESTCASES_DIR "testcases"
#endif


USE_NAMESPACE


/**
 * Sets the default values (from DefaultConfigFactory) in the passed SyncManagerConfig.
 * @param config             the SyncManagerConfig to fill
 * @param testName           the name of the test which called this method (for deviceID)
 * @param setClientDefaults  if true, will set the default deviceConfig and accessConfig
 * @param defaultSources     ArrayList of source names (StringBuffer), to create default SyncSourceConfig.
 *                           If not specified, non is created. Default is NULL.
 */
static void setDefaults(SyncManagerConfig* config, const char* testName, 
                        const bool setClientDefaults, ArrayList* defaultSources = NULL) {

    if (!config) return;

    // set default values
    if (setClientDefaults) {
        config->setClientDefaults();
    }

    if (defaultSources) {
        for (int i=0; i<defaultSources->size(); i++) {
            StringBuffer* sourceName = (StringBuffer*)defaultSources->get(i);
            config->setSourceDefaults(sourceName->c_str());
        }
    }

    //set deviceID
    StringBuffer devID("sc-pim-");
    devID += testName;
    config->getDeviceConfig().setDevID(devID.c_str());

    // set credentials (if existing)
    const char *serverUrl = getenv("CLIENT_TEST_SERVER_URL");
    const char *username = getenv("CLIENT_TEST_USERNAME");
    const char *password = getenv("CLIENT_TEST_PASSWORD");

    if(serverUrl) {
        config->getAccessConfig().setSyncURL(serverUrl);
    }
    if(username) {
        config->getAccessConfig().setUsername(username);
    }
    if(password) {
        config->getAccessConfig().setPassword(password);
    }
}



const StringBuffer initAdapter(const char* testName) {

    StringBuffer rootContext(TEST_ROOT_CONTEXT);
    rootContext.append("/");
    if (testName) {
        rootContext.append(testName);
    } else {
        rootContext.append(DEFAULT_TEST_NAME);
    }

    PlatformAdapter::init(rootContext.c_str(), true);

    return rootContext;
}


SyncManagerConfig* getNewSyncManagerConfig(const char* testName, const bool setClientDefaults, ArrayList* defaultSources) {
    
    // Compose the Root context
    initAdapter(testName);
    
    SyncManagerConfig* config = new SyncManagerConfig();
    
    setDefaults(config, testName, setClientDefaults, defaultSources);

    return config;
}


DMTClientConfig* getNewDMTClientConfig(const char* testName, const bool setClientDefaults, ArrayList* defaultSources) {
    
    // Compose the Root context
    initAdapter(testName);
    
    DMTClientConfig* config = new DMTClientConfig();
    
    setDefaults(config, testName, setClientDefaults, defaultSources);

    return config;
}



char* loadTestFile(const char* testName, const char* fileName) {

    if (!testName || !fileName) {
        return NULL;
    }
    
    char* content = NULL;
    size_t len;

    StringBuffer path;
    path.sprintf("%s/%s/%s", TESTCASES_DIR, testName, fileName);

    bool fileLoaded = readFile(path.c_str(), &content, &len, false);
    CPPUNIT_ASSERT_MESSAGE("Failed to load test file", fileLoaded);
       
    return content;
}



/** @} */
/** @endcond */
