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


#include "client/SyncClient.h"
#include "spds/SyncManager.h"
#include "spds/spdsutils.h"
#include "base/LogErrorHandler.h"
#include "syncml/core/CTTypeSupported.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/CTPropParam.h"

//--------------------------------------------------- Constructor & Destructor
SyncClient::SyncClient() {
}

SyncClient::~SyncClient() {
}

//------------------------------------------------------------- Public Methods

/*
* Used to start the sync process. The argument is an array of SyncSources
* that have to be synched with the sync process
*/
int SyncClient::sync(SyncManagerConfig& config, SyncSource** sources) {

    resetError();
    int ret = 0;

    if (!config.getSyncSourceConfigsCount()) {
        sprintf(lastErrorMsg, "Error in sync() - configuration not set correctly.");
        LOG.error(lastErrorMsg);
        return 1;
    }

    //
    // Synchronization report.
    //
    syncReport.setSyncSourceReports(config);
    // Set source report on each SyncSource (assign pointer)
    int i=0;
    while (sources[i]) {
        char* name = toMultibyte(sources[i]->getName());
        SyncSourceReport *ssr = syncReport.getSyncSourceReport(name);
        ssr->setState(SOURCE_ACTIVE);
        sources[i]->setReport(ssr);

        delete[] name;
        i++;
    }

    SyncManager syncManager(config, syncReport);

    if ((ret = syncManager.prepareSync(sources))) {
        LOG.error("Error in preparing sync: %s", lastErrorMsg);
        goto finally;
    }

    ret = continueAfterPrepareSync();
    if (ret) {
        LOG.error("SyncClient: continueAfterPrepareSync returns error code: %d.", ret);
        goto finally;
    }

    if ((ret = syncManager.sync())) {
        LOG.error("Error in syncing: %s", lastErrorMsg);
        goto finally;
    }

    ret = continueAfterSync();
    if (ret) {
        LOG.error("SyncClient: continueAfterSync returns error code: %d.", ret);
        goto finally;
    }

    if ((ret = syncManager.endSync())) {
        LOG.error("Error in ending sync: %s", lastErrorMsg);
        goto finally;
    }

finally:

    // Update SyncReport with last error from sync
    syncReport.setLastErrorCode(lastErrorCode);
    syncReport.setLastErrorMsg(lastErrorMsg);

    return ret;
}


/*
 * Start the sync process.
 * SyncSources are managed (created, initialized, deleted) inside
 * this method. When SyncSource array is ready, the method 'sync(sources**)'
 * is called to start the sync process.
 *
 * @param sourceNames: optional, a NULL terminated array of source names that
 *                     we want to sync. If NULL, sources to sync are chosen
 *                     from the configuration object (config).
 * @return:            0 on success, an error otherwise
 */
int SyncClient::sync(SyncManagerConfig& config, char** sourceNames) {
    SyncSourceConfig* sc = NULL;
    SyncSource **sources = NULL;
    const char* currName;
    int currSource = 0, numActive = 0, numSources = 0;
    int ret = 0;

    ret = prepareSync(config);
    if (ret) {
        LOG.error("SyncClient: prepareSync returned error code: %d.", ret);
        goto finally;
    }

    // Get number of sources: from passed parameter or (if NULL) from config.
    numSources = 0;
    if (sourceNames) {
        while (sourceNames[numSources]) {
            numSources ++;
        }
    }
    else {
        numSources = config.getSyncSourceConfigsCount();
    }

    // make room for all potential sync sources
    sources = new SyncSource* [numSources + 1];

    // iterate over all configs and add those which the client
    // wants to have synchronized
    while (currSource < numSources) {

        // use only sources indicated in 'sourceNames' param
        if (sourceNames) {
            currName = sourceNames[currSource];
            if (! (sc = config.getSyncSourceConfig(currName)) ) {
                if (sources)
                    delete [] sources;
                return lastErrorCode;
            }
        }
        // use all available sources from config
        else {
            if (! (sc = config.getSyncSourceConfig(currSource)) ) {
                if (sources)
                    delete [] sources;
                return lastErrorCode;
            }
            currName = sc->getName();
        }

        ret = createSyncSource(currName, currSource, sc, sources + numActive);
        if (ret) {
            LOG.error("SyncClient: createSyncSource returned error code: %d.", ret);
            goto finally;
        }

        if (sources[numActive]) {
            numActive++;
        }
        currSource++;
    }
    sources[numActive] = NULL;


    ret = beginSync(sources);
    if (ret) {
        LOG.error("SyncClient: beginSync returned error code: %d.", ret);
        goto finally;
    }

    //
    // ready to synchronize
    //
    ret = sync(config, sources);
    if (ret) {
        goto finally;
    }

    ret = endSync(sources);
    if (ret) {
        LOG.error("SyncClient: endSync returned error code: %d.", ret);
    }

finally:
    if (sources) {
        for (int i=0; sources[i]; i++) {
            delete sources[i];
        }
        delete [] sources;
    }

    return ret;
}


SyncReport* SyncClient::getSyncReport() {
    return &syncReport;
}

