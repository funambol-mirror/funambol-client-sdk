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

#include "client/FILEClient.h"
#include "base/globalsdef.h"

USE_NAMESPACE



int main(int argc, char** argv) {

    //
    // get argv if exists -> path of folder to sync
    //
    char* dir = stringdup("test");

    // Init LOG
    LOG.setLogName(LOG_NAME);
    LOG.setLogPath(LOG_PATH);
    LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL_DEBUG);
    
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

    /////////////////////////////////////////////
    // TMP: reset config last time stamp! -> always slow
    // TMP: reset devinfHash -> always send devinf (server bug on loSupport)
    createConfig(config);
    /////////////////////////////////////////////

    //
    // Create the SyncSource passing its name and SyncSourceConfig.
    //
    FileSyncSource fileSource(WSOURCE_NAME, config.getSyncSourceConfig(SOURCE_NAME));
    fileSource.setDir(dir);

    SyncSource* ssArray[2];
    ssArray[0] = &fileSource;
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


    if (dir)
        delete [] dir;

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
    sc->setEncoding ("bin");
    sc->setType     ("application/*");
    sc->setURI      ("briefcase");
    sc->setSyncModes("slow");        // TBD: by now only slow
    config.setSyncSourceConfig(*sc);
    delete sc;
}

