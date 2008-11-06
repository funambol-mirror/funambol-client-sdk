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

#include "FSyncConfig.h"
#include "FSyncOpt.h"
#include "FSyncListener.h"
#include "FSyncSourceListener.h"
#include "FSyncItemListener.h"
#include "FSyncTransportListener.h"
#include "base/fscapi.h"
#include "client/SyncClient.h"
#include "client/FileSyncSource.h"
#include "event/ManageListener.h"
#include "base/util/StringBuffer.h"
#include "base/util/utils.h"
#include "base/Log.h"

#ifndef EXIT_SUCCESS
#	define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#	define EXIT_FAILURE 1
#endif

USE_NAMESPACE

static const char *progname = "fsync";

bool doSync(FSyncOpt& options) 
{
    SyncSourceConfig *srcconfig = FSyncConfig::getInstance()->getSyncSourceConfig(FSYNC_SOURCE_NAME);

    // Create the SyncSource passing its name, the SyncSourceConfig 
    FileSyncSource fSource(TEXT(FSYNC_SOURCE_NAME), srcconfig);    
    
    // Store the path to the folder to sync
    fSource.setDir(FSyncConfig::getInstance()->getSyncPath());

    // Initialize the SyncSource array to sync
    SyncSource* ssArray[] = { &fSource, NULL } ;

    // Create the SyncClient
    SyncClient fileClient;

    // SYNC!
    if (fileClient.sync(*FSyncConfig::getInstance(), ssArray)) {
        LOG.error("Error during sync.\n");
        return false;
    }

    // Save the anchors
    FSyncConfig::getInstance()->save();

    return true;
}

int main(int argc, char** argv) 
{
    // Get the config instance
    FSyncConfig *config = FSyncConfig::getInstance();
    // Initialize it (read from file or create the default one
    config->init();

    // Init LOG
    LOG.reset(FSYNC_LOG_TITLE);

    // Initialize the command line options handler
    FSyncOpt opts(progname); 

    // Parse command line options
    if (opts.parseCmdline(argc, argv) == false) {
        fprintf(stderr, "error parsing options: %s\n", opts.getErr());
        exit(EXIT_FAILURE);
    }

    // On user help request exit smoothly without doing anything 
    if (opts.optionSet("help")) { 
        exit(EXIT_SUCCESS);
    }

    // Get verbosity option
    VerboseLevel verbose = opts.getVerbosity();

    // Manage sync listeners
    ManageListener& manage = ManageListener::getInstance();

    if (verbose >= NORMAL) {
        manage.setSyncListener( new FSyncListener());
        if (verbose >= VERBOSE) {
            manage.setSyncItemListener  ( new FSyncItemListener());
            manage.setSyncSourceListener( new FSyncSourceListener());
            manage.setTransportListener ( new FSyncTransportListener());
        }
    } 


    // Check the presence of the sync folder
    if (createFolder(config->getSyncPath()) < 0) {
        LOG.error("error creating folder");
        exit(EXIT_FAILURE);
    }

    // Sync
    if (doSync(opts) == false) {
        // Sync failed
        exit(EXIT_FAILURE);
    }

    // Unset listeners
    LOG.debug("Unset listeners");
    manage.unsetSyncListener();
    manage.unsetSyncItemListener();
    manage.unsetSyncSourceListener();
    manage.unsetTransportListener();

    exit(EXIT_SUCCESS);
}

