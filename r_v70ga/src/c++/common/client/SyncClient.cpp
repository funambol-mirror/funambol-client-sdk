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


#include "client/SyncClient.h"
#include "spds/SyncManager.h"
#include "spds/spdsutils.h"
#include "base/LogErrorHandler.h"
#include "syncml/core/CTTypeSupported.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/CTPropParam.h"
#include "base/globalsdef.h"

USE_NAMESPACE

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
int SyncClient::sync(AbstractSyncConfig& config, SyncSource** sources) {

    resetError();
    int ret = 0;

    if (!config.getAbstractSyncSourceConfigsCount()) {
        //sprintf(lastErrorMsg, "Error in sync() - configuration not set correctly.");
        ret = 1;
        setError(ret, "Error in sync() - configuration not set correctly.");
        
        LOG.error("%s", getLastErrorMsg());
        return ret;
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
        LOG.error("Error in preparing sync: %s", getLastErrorMsg());
        goto finally;
    }

    ret = continueAfterPrepareSync();
    if (ret) {
        LOG.error("SyncClient: continueAfterPrepareSync returns error code: %d.", ret);
        goto finally;
    }

    if ((ret = syncManager.sync())) {
        LOG.error("Error in syncing: %s", getLastErrorMsg());
        goto finally;
    }

    ret = continueAfterSync();
    if (ret) {
        LOG.error("SyncClient: continueAfterSync returns error code: %d.", ret);
        goto finally;
    }

    if ((ret = syncManager.endSync())) {
        LOG.error("Error in ending sync: %s", getLastErrorMsg());
        goto finally;
    }

finally:

    // Update SyncReport with last error from sync
    syncReport.setLastErrorCode(getLastErrorCode());
    syncReport.setLastErrorMsg(getLastErrorMsg());

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
int SyncClient::sync(AbstractSyncConfig& config, char** sourceNames) {
    AbstractSyncSourceConfig* sc = NULL;
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
        numSources = config.getAbstractSyncSourceConfigsCount();
    }

    // make room for all potential sync sources
    sources = new SyncSource* [numSources + 1];

    // iterate over all configs and add those which the client
    // wants to have synchronized
    while (currSource < numSources) {

        // use only sources indicated in 'sourceNames' param
        if (sourceNames) {
            currName = sourceNames[currSource];
            if (! (sc = config.getAbstractSyncSourceConfig(currName)) ) {
                if (sources)
                    delete [] sources;
                return getLastErrorCode();
            }
        }
        // use all available sources from config
        else {
            if (! (sc = config.getAbstractSyncSourceConfig(currSource)) ) {
                if (sources)
                    delete [] sources;
                return getLastErrorCode();
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

