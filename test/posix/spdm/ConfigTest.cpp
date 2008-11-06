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

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include "spdm/migrateConfig.h"
#include "spdm/DeviceManagementNode.h"
#include "base/util/StringBuffer.h"
#include "client/DMTClientConfig.h"
#include "spds/DefaultConfigFactory.h"
#include "base/util/utils.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/AutoRegisterSuite.h>
#include "base/globalsdef.h"

USE_NAMESPACE


#define APPLICATION_URI "FunambolTest/SyncclientPIM"
#define LOG_TITLE		"Funambol Win32 Example Log"
#define LOG_PATH		"."
#define LOG_LEVEL		LOG_LEVEL_DEBUG
#define SOURCE_NAME     "briefcase"
#define WSOURCE_NAME    TEXT("briefcase")
#define DEVICE_ID       "Funambol Win32 Example"


void createConfig(DMTClientConfig& config) {

    AccessConfig* ac = DefaultConfigFactory::getAccessConfig();
    config.setAccessConfig(*ac);
    delete ac;

    DeviceConfig* dc = DefaultConfigFactory::getDeviceConfig();
    dc->setDevID(DEVICE_ID);     // So next time won't be generated, we always save config at the end.
    dc->setMan  ("FunambolTest");
    config.setDeviceConfig(*dc);
    delete dc;

    SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(SOURCE_NAME);
    sc->setEncoding  ("bin");
    sc->setType      ("text");
    sc->setURI       ("briefcase");
    sc->setSync      ("two-way");
    sc->setSyncModes ("two-way,one-way-from-server,one-way-from-client");
    config.setSyncSourceConfig(*sc);
    delete sc;
}


class ConfigTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ConfigTest);
    CPPUNIT_TEST(testMigration);
    CPPUNIT_TEST(testCompatibility);
    CPPUNIT_TEST(testConfig);
    CPPUNIT_TEST(testConfigSyncModes);
    CPPUNIT_TEST_SUITE_END();

    public:
        void setUp() {
            // Save current directory information
            cwdfd = open(".", O_RDONLY);
        }

        void tearDown(){
            // Return to the original directory
            if (cwdfd >= 0) {
                fchdir(cwdfd);
                close(cwdfd);
            }
        }

        void testMigration(){

            //let's create a fake configuration under $HOME/testdir/
            StringBuffer oldpath;
            StringBuffer newpath;
            oldpath = getenv("HOME");
            oldpath += "/testdir";
            StringBuffer createdir;
            createdir = "mkdir ";
            createdir += oldpath;
            system(createdir);
            DeviceManagementNode::setConfigPath(oldpath.c_str());
            DMTClientConfig* config = new DMTClientConfig(APPLICATION_URI);
            // Read config from registry.
            if (!config->read() ||
                strcmp(config->getDeviceConfig().getDevID(), DEVICE_ID)) {
                // Config not found -> generate a default config
                createConfig(*config);
                config->save();
            }
            delete config;
            if(chdir(oldpath)!=0){
            }
            system("for file in `find -name config.ini`; do dirs=`dirname $file`; mv $file $dirs/config.txt; done");
            
            //ok now we have a testdir with a old style configuration... let's migrate!
            newpath = getenv("HOME");
            newpath += "/newtestdirpath";
            migrateConfig(oldpath, newpath);
            CPPUNIT_ASSERT( chdir(oldpath) != 0 );
            CPPUNIT_ASSERT( chdir(newpath) == 0 );
            
            DeviceManagementNode::setConfigPath(newpath);
            DMTClientConfig ctest(APPLICATION_URI);
            ctest.read();
            CPPUNIT_ASSERT( strcmp(ctest.getDeviceConfig().getDevID(), DEVICE_ID) == 0 );
            StringBuffer endcommand;
            endcommand = "rm -rf ";
            endcommand += newpath;
            system( endcommand );
            
            DeviceManagementNode::setConfigPath("");

        }

        void testCompatibility(){
            DeviceManagementNode::setCompatibilityMode(true);
            StringBuffer testdir;
            testdir = getenv("HOME");
            testdir += "/.sync4j/";
            if (chdir (testdir) != 0){
                StringBuffer sync4jpath = "mkdir ";
                sync4jpath += getenv("HOME");
                sync4jpath += "/.sync4j";
                system ( sync4jpath );
            }
            DMTClientConfig* config = new DMTClientConfig(APPLICATION_URI);

            // Read config from registry.
            if (!config->read() ||
                strcmp(config->getDeviceConfig().getDevID(), DEVICE_ID)) {
                // Config not found -> generate a default config
                createConfig(*config);
                config->save();
            }
            delete config;
            CPPUNIT_ASSERT( (chdir(testdir)) == 0 );
            DMTClientConfig ctest(APPLICATION_URI);
            ctest.read();
            CPPUNIT_ASSERT( strcmp(ctest.getDeviceConfig().getDevID(), DEVICE_ID) == 0 ); 
            DeviceManagementNode::setCompatibilityMode(false);
            StringBuffer endcommand = "rm -rf ";
            endcommand += getenv("HOME");
            endcommand += "/.sync4j/FunambolTest";
            system(endcommand);
        
        }
        
        void testConfig(){
            StringBuffer dir;
            dir = getenv("XDG_CONFIG_HOME");
            if (dir.empty()){
                dir = getenv("HOME");
                dir += "/.config/";
            }
            dir += "FunambolTest";
            DMTClientConfig* config = new DMTClientConfig(APPLICATION_URI);

            if (!config->read() ||
                strcmp(config->getDeviceConfig().getDevID(), DEVICE_ID)) {
                // Config not found -> generate a default config
                createConfig(*config);
                config->save();
            }
            delete config;
            CPPUNIT_ASSERT( (chdir(dir)) == 0 );
            DMTClientConfig ctest(APPLICATION_URI);
            ctest.read();
            CPPUNIT_ASSERT( strcmp(ctest.getDeviceConfig().getDevID(), DEVICE_ID) == 0 );
            StringBuffer endcommand = "rm -rf ";
            endcommand += dir;
            system(endcommand); 
        }

        void testConfigSyncModes(){
            StringBuffer dir;
            dir = getenv("XDG_CONFIG_HOME");
            if (dir.empty()){
                dir = getenv("HOME");
                dir += "/.config/";
            }
            dir += "FunambolTest";
            DMTClientConfig* config = new DMTClientConfig(APPLICATION_URI);

            if (!config->read() ||
                strcmp(config->getDeviceConfig().getDevID(), DEVICE_ID)) {
                // Config not found -> generate a default config
                createConfig(*config);
                config->save();
            }
            delete config;
            CPPUNIT_ASSERT( (chdir(dir)) == 0 );
            DMTClientConfig ctest(APPLICATION_URI);
            ctest.read();
            SyncSourceConfig* sc = ctest.getSyncSourceConfig(SOURCE_NAME);
            CPPUNIT_ASSERT( strcmp(sc->getSync(), "two-way") == 0 );
            CPPUNIT_ASSERT( strcmp(sc->getSyncModes(), "two-way,one-way-from-server,one-way-from-client") == 0 );
            StringBuffer endcommand = "rm -rf ";
            endcommand += dir;
            system(endcommand); 
        }
    private:
    int cwdfd; 
};

CPPUNIT_TEST_SUITE_REGISTRATION( ConfigTest );

