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

#include "spds/SyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "client/SyncClient.h"
#include "spds/spdsutils.h"
#include "spds/SyncSourceConfig.h"
#include "base/globalsdef.h"

USE_NAMESPACE

static SyncSourceConfig defaultConfig;

const AbstractSyncSourceConfig& SyncSource::getConfig() const
{
    return config ? *config : defaultConfig;
}
AbstractSyncSourceConfig& SyncSource::getConfig()
{
    return config ? *config : defaultConfig;
}

void SyncSource::setConfig(AbstractSyncSourceConfig* sc)
{
    config = sc;
    setPreferredSyncMode(sc ? syncModeCode(sc->getSync()) : SYNC_NONE);
}

SyncSource::SyncSource(const WCHAR* sourceName, AbstractSyncSourceConfig *sc) :
    config(NULL)
{
    name   = NULL;
    report = NULL;

    setConfig(sc);
    if ((sourceName == NULL) || (*sourceName == 0)) {
        setError(ERR_PARAMETER_IS_EMPTY,
                "name cannot be empty (NULL or 0-length)");
        goto finally;
    }
    name = wstrdup(sourceName);

  finally:

    lastSync = 0;
	nextSync = 0;

    filter       = NULL;
}

/**
 * Release dynamically allocated resources
 */
SyncSource::~SyncSource() {
    if (name) {
        delete [] name;
    }
    if (filter) {
       delete filter;
    }
    report = NULL;
}


/*
 * Returns the source name.
 */
const WCHAR *SyncSource::getName() {
     return name;
 }


// Return pointer to report object
SyncSourceReport* SyncSource::getReport() {
    return report;
}

// Set the report pointer with the given one
// (no copy, only assign the pointer to the external one)
void SyncSource::setReport(SyncSourceReport* sr) {
    report = sr;
}


/*
 * Sets the synchronization mode required for the
 * SyncSource.
 *
 * @param syncMode - sync synchronization mode
 */
void SyncSource::setPreferredSyncMode(SyncMode sourceSyncMode) {
    syncMode = sourceSyncMode;
}

/*
 * Returns the preferred synchronization mode for the SyncSource
 */
SyncMode SyncSource::getPreferredSyncMode() {
    return syncMode;
}

/*
 * Sets the server imposed synchronization mode for the SyncSource.
 *
 * @param syncMode - sync synchronization mode
 */
void SyncSource::setSyncMode(SyncMode mode) {
    syncMode = mode;
}

/*
 * Returns the synchronization mode.
 */
SyncMode SyncSource::getSyncMode() {
    return syncMode;
}

int SyncSource::beginSync() {
    return 0;
}

int SyncSource::endSync() {
    return 0;
}

/*
 * Returns the timestamp in milliseconds of the last synchronization.
 * The reference time of the timestamp is platform specific.
 */
unsigned long SyncSource::getLastSync() {
    return lastSync;
}

/*
 * Sets the timestamp in millisencods of the last synchronization.
 * The reference time of the timestamp is platform specific.
 */
void SyncSource::setLastSync(unsigned long timestamp) {
    lastSync = timestamp;
}

/*
 * Returns the timestamp in milliseconds of the next synchronization.
 * The reference time of the timestamp is platform specific.
 */
unsigned long SyncSource::getNextSync() {
    return nextSync;
}

/*
 * Sets the timestamp in millisencods of the next synchronization.
 * The reference time of the timestamp is platform specific.
 */
void SyncSource::setNextSync(unsigned long timestamp) {
    nextSync = timestamp;
}

/*
 * Sets the last anchor associated to the source
 *
 * @param lastAnchor last anchor
 */
void SyncSource::setLastAnchor(const char* lastAnchor) {
    strncpy(last, (lastAnchor != NULL) ? lastAnchor : "", DIM_ANCHOR);
    last[DIM_ANCHOR-1] = 0;
}

/*
 * Gets the last anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const char* SyncSource::getLastAnchor() {
    return last;
}

/*
 * Sets the next anchor associated to the source
 *
 * @param next next anchor
 */
void SyncSource::setNextAnchor(const char* nextAnchor) {
    strncpy(next, (nextAnchor != NULL) ? nextAnchor : "", DIM_ANCHOR);
    next[DIM_ANCHOR-1] = 0;
}

/*
 * Gets the next anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const char* SyncSource::getNextAnchor() {
    return next;
}


/*
 * Gets filter
 *
 * @return  the current filter's value
 *
 */
SourceFilter* SyncSource::getFilter() {
    return filter;
}

/*
 * Sets filter
 *
 * @param filter the new value
 *
 */
void SyncSource::setFilter(SourceFilter* f) {
    if (filter) {
        delete filter; filter = NULL;
    }

    if (f) {
        filter = new SourceFilter();
        filter->setInclusive(f->isInclusive());
        filter->setClause(f->getClause());
    }
}

void SyncSource::assign(SyncSource& s) {
    setSyncMode(s.getSyncMode());
    setLastSync(s.getLastSync());
    setNextSync(s.getNextSync());
    setLastAnchor(s.getLastAnchor());
    setNextAnchor(s.getNextAnchor());
    setFilter(s.getFilter());
}

/**
* Indicates that all the server status of the current package 
* of the client items has been processed by the engine.
* This signal can be useful to update the modification arrays
*/
void SyncSource::serverStatusPackageEnded() {

}
    
/**
* Indicates that all the client status of the current package 
* of the server items that has been processed by the client and 
* are going to be sent to the server.
* This signal can be useful to update the modification arrays
*/
void SyncSource::clientStatusPackageEnded() {
}

