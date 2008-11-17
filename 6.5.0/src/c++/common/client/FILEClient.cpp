/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#include "client/FILEClient.h"



int main(int argc, char** argv) {


    //
    // get argv if exists -> path of folder to sync
    //
    char* dir = stringdup("test");

    // Init LOG
    Log(0, LOG_PATH, LOG_NAME);
	LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL);

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
    dc->setLoSupport(TRUE);
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

