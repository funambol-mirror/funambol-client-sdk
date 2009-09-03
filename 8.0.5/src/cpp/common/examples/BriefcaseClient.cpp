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

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "spds/DefaultConfigFactory.h"
#include "examples/BriefcaseCacheSyncSource.h"


#ifdef WIN32

#else
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "base/globalsdef.h"

USE_NAMESPACE
#endif

#define APPLICATION_URI         "Funambol/SyncclientFILE"
#define LOG_TITLE		        "Funambol FILEClient Log"
#define LOG_PATH		        "."
#define LOG_LEVEL		        LOG_LEVEL_DEBUG
#define SOURCE_NAME             "briefcase"
#define WSOURCE_NAME            TEXT("briefcase")
#define DEVICE_ID               "Funambol FILEClient"
#define SW_VERSION              "1.0"
//#define FILE_USER_AGENT         "Funambol FILEClient 1.0"


// Function to create a default config.
void createConfig(DMTClientConfig& config);

#define BRIEFCASE_REP   "funambol_briefcase"

/**
 * Retrieve the default path for briefcase.
 * It is {HOME_FOLDER}/Briefcase, where HOME_FOLDER depends on the operating system.
 *
 * @return 0 on success, -1 otherwise.
 */
int getDefaultBriefcasePath(StringBuffer& path) {
#ifdef WIN32    
    wchar_t p[MAX_PATH];

    if( !SHGetSpecialFolderPath(NULL, p, CSIDL_PERSONAL, 0)) {
        return -1;
    }
    
    char* t = toMultibyte(p);
    path = t;
    delete [] t;

#else
    const char *home = getenv("HOME");
    if( !home ) {
        return -1;
    }
    path = home;
#endif
    
    path.append("/");
    path.append(BRIEFCASE_REP);
    
    return 0;
}
    
/**
 * Retrieve the default path for briefcase.
 * It is {HOME_FOLDER}/Briefcase, where HOME_FOLDER depends on the operating system.
 *
 * @return 0 on success, -1 otherwise.
 */
int createFolder(const char *path) {
#ifdef WIN32
    wchar_t* p = toWideChar(path);
    DWORD attr = CreateDirectory(p, NULL);    
    delete [] p;
#else
    DIR* d = opendir(path);
    if (!d) {
        mkdir(path, 0777);
    } else {
        closedir(d);        
    }   
#endif
    return 0;
}

int main(int argc, char** argv) {

    // Init LOG
    LOG.setLogName(LOG_NAME);
    LOG.setLogPath(LOG_PATH);
    LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL_DEBUG);
    
    // Create the default briefcase folder if not already present
    StringBuffer briefcase;

    if(getDefaultBriefcasePath(briefcase) != 0) {
        LOG.error("Error retrieving default briefcase path.");
        fprintf(stderr, "Error retrieving default briefcase path.");
        exit(1);
    }

    if(createFolder(briefcase) != 0) {
        LOG.error("Error creating briefcase folder.");
        perror("filesync");
        exit(1);
    }

    //
    // Create the configuration.
    //
    DMTClientConfig config(APPLICATION_URI);

    // Read config from registry.
    if (!config.read() ||
        strcmp(config.getDeviceConfig().getSwv(), SW_VERSION)) {
        // generate a default config
        createConfig(config);
    }
    
    //
    // Create the SyncSource passing its name, the SyncSourceConfig and the ItemHandler
    // class in which there is the file cache or the listener
    //    
    //ItemHandler* itemHandler = new CacheItemHandler(StringBuffer("."));
    BriefcaseCacheSyncSource bSource(WSOURCE_NAME, config.getSyncSourceConfig(SOURCE_NAME));    
    // the directory in which there is the file to be synced
    bSource.setDir(briefcase);
    

    SyncSource* ssArray[2];
    ssArray[0] = &bSource;
    ssArray[1] = NULL;

    //
    // Create the SyncClient passing the config.
    //
    SyncClient fileClient;

    // SYNC!
    if( fileClient.sync(config, ssArray) ) {
        LOG.error("Error in sync.");
    }

    // Save config to registry.
    config.save();
    
    return 0;
}



//
// Function to create a default config.
//
void createConfig(DMTClientConfig& config) {

    AccessConfig* ac = DefaultConfigFactory::getAccessConfig();
    ac->setMaxMsgSize(60000);
    //ac->setUserAgent (FILE_USER_AGENT);
    config.setAccessConfig(*ac);
    delete ac;

    DeviceConfig* dc = DefaultConfigFactory::getDeviceConfig();
    dc->setDevID    (DEVICE_ID);
    dc->setMan      ("Funambol");
    dc->setLoSupport(true);
    dc->setSwv      (SW_VERSION);  // So next time won't be generated, we always save config at the end.
    config.setDeviceConfig(*dc);
    delete dc;

    SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(SOURCE_NAME);
    sc->setEncoding ("b64");
    sc->setType     ("application/*");
    sc->setURI      ("briefcase");
    sc->setSyncModes("slow");        // TBD: by now only slow
    config.setSyncSourceConfig(*sc);
    delete sc;
}

