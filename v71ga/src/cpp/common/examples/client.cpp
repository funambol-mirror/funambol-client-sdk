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

#if 0
#include "crtdbg.h"
#endif
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"

#include "examples/TestSyncSource.h"
#include "examples/TestSyncSource2.h"

#include "spds/DefaultConfigFactory.h"

#include "examples/listeners/TestSyncListener.h"
#include "examples/listeners/TestSyncSourceListener.h"
#include "examples/listeners/TestSyncStatusListener.h"
#include "examples/listeners/TestSyncItemListener.h"
#include "examples/listeners/TestTransportListener.h"
#include "event/SetListener.h"
#include "base/globalsdef.h"

USE_NAMESPACE


void createConfig(DMTClientConfig& config);

#define APPLICATION_URI "Funambol/SyncclientPIM"
#define LOG_TITLE		"Funambol Win32 Example Log"
#define LOG_PATH		"."
#define LOG_LEVEL		LOG_LEVEL_DEBUG
#define SOURCE_NAME     "briefcase"
#define WSOURCE_NAME    TEXT("briefcase")
#define DEVICE_ID       "Funambol Win32 Example"


#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd ) {
#else
int main(int argc, char** argv) {
#endif

    // Init LOG
    //Log(0, LOG_PATH, LOG_NAME);
    //LOG.setLogPath(LOG_PATH);
    LOG.setLogName(LOG_NAME);
    LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL_DEBUG);

#if 0
    _CrtSetDbgFlag (ON);

    // Get current flag
    int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

    // Turn on leak-checking bit
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    // Turn on automatic checks
    tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

    // Set flag to the new value
    _CrtSetDbgFlag( tmpFlag );
#endif


// ------------- Main sample client ------------------------
    #if 0
    //
    // Set listeners:
    //
    TestSyncListener*       listener1 = new TestSyncListener();
    TestSyncSourceListener* listener2 = new TestSyncSourceListener();
    TestSyncStatusListener* listener3 = new TestSyncStatusListener();
    TestSyncItemListener*   listener4 = new TestSyncItemListener();
    TestTransportListener*  listener5 = new TestTransportListener();

    setSyncListener      (listener1);
    setSyncSourceListener(listener2);
    setSyncStatusListener(listener3);
    setSyncItemListener  (listener4);
    setTransportListener (listener5);
    #endif
    //
    // Create the configuration.
    //
    DMTClientConfig config(APPLICATION_URI);

    // Read config from registry.
    if (!config.read() ||
        strcmp(config.getDeviceConfig().getDevID(), DEVICE_ID)) {
        // Config not found -> generate a default config
        createConfig(config);
        config.save();
    }
    //
    // Create the SyncSource passing its name and its config.
    //
    TestSyncSource source(WSOURCE_NAME, config.getSyncSourceConfig(SOURCE_NAME));
    SyncSource* ssArray[] = { &source, NULL };

    //
    // Create the SyncClient .
    //
    SyncClient sampleClient;

    // Start the sync!
    if( sampleClient.sync(config, ssArray) ) {
        LOG.error("Error in sync.");
    }

    // Print sync results.
    StringBuffer res;
    sampleClient.getSyncReport()->toString(res);
    printf("\n%s", res.c_str());

    // Save config to registry.
    config.save();

// ----------------------------------------------------------

    //
    // Unset Listeners
    //
    #if 0
    unsetSyncListener      ();
    unsetSyncSourceListener();
    unsetSyncStatusListener();
    unsetSyncItemListener  ();
    unsetTransportListener ();
    #endif
    return 0;
}


//
// Function to create a default config.
//
void createConfig(DMTClientConfig& config) {

    AccessConfig* ac = DefaultConfigFactory::getAccessConfig();
    config.setAccessConfig(*ac);
    delete ac;

    DeviceConfig* dc = DefaultConfigFactory::getDeviceConfig();
    dc->setDevID(DEVICE_ID);     // So next time won't be generated, we always save config at the end.
    dc->setMan  ("Funambol");
    config.setDeviceConfig(*dc);
    delete dc;

    SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(SOURCE_NAME);
    sc->setEncoding("bin");
    sc->setType    ("text");
    sc->setURI     ("briefcase");
    config.setSyncSourceConfig(*sc);
    delete sc;
}

