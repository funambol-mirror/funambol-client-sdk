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

#include "base/util/utils.h"
#include "syncml/core/DataStore.h"
#include "base/globalsdef.h"

USE_NAMESPACE

DataStore::DataStore() {
    initialize();
}

DataStore::~DataStore() {
   delete sourceRef;
   delete [] displayName;
   maxGUIDSize = 0;
   delete rxPref;
   delete rx;
   delete txPref;
   delete tx;
   delete dsMem;
   delete syncCap;
}

/**
 * Creates a new DataStore object with the given input information
 *
 * @param sourceRef specifies the source address from the associated
 *                  command - NOT NULL
 * @param displayName the display name
 * @param maxGUIDSize the maximum GUID size. Set to -1 if the Maximum GUID
 *                  size is unknown or unspecified. Otherwise, this
 *                  parameter should be a positive number.
 * @param rxPref the relative information received to the content type
 *               preferred - NOT NULL
 * @param rx an array of the relative info received to the content type
 *           supported - NOT NULL
 * @param txPref the relative information trasmitted
 *                  to the content type preferred - NOT NULL
 * @param tx an array of the relative info trasmitted to the content type
 *           supported - NOT NULL
 * @param ctCaps an array of the relative CtCaps
 * @param dsMem the datastore memory info
 * @param syncCap the synchronization capabilities - NOT NULL
 *
 */
DataStore::DataStore(SourceRef* sourceRef,
                      const char* displayName,
                      long maxGUIDSize,
                      ContentTypeInfo* rxPref,
                      ArrayList* rx,
                      ContentTypeInfo* txPref,
                      ArrayList* tx,
                      ArrayList* ct_Caps,
                      DSMem* dsMem,
                      SyncCap* syncCap) {

        initialize();
        setSourceRef(sourceRef);
        setMaxGUIDSize(maxGUIDSize);
        setRxPref(rxPref);
        setRx(rx);
        setTxPref(txPref);
        setTx(tx);
        setSyncCap(syncCap);
        setCtCaps(ct_Caps);
        setDisplayName(displayName);
        setDSMem(dsMem);
}

void DataStore::initialize() {
    sourceRef       = NULL;
    displayName     = NULL;
    maxGUIDSize     = 0;
    rxPref          = NULL;
    rx              = NULL; //new ArrayList();
    txPref          = NULL;
    tx              = NULL; //new ArrayList();
    ctCaps          = NULL; //new ArrayList();
    dsMem           = NULL;
    syncCap         = NULL;
}

/**
 * Gets the sourceRef properties
 *
 * @return the sourceRef properties
 */
SourceRef* DataStore::getSourceRef() {
    return sourceRef;
}

/**
 * Sets the reference URI
 *
 * @param sourceRef the reference URI
 *
 */
void DataStore::setSourceRef(SourceRef* sourceRef) {
    if (sourceRef == NULL) {
        // TBD
    } else {
        delete this->sourceRef;
    }
    this->sourceRef = (SourceRef*)sourceRef->clone();

}

/**
 * Gets the displayName properties
 *
 * @return the displayName properties
 */
const char* DataStore::getDisplayName() {
    return displayName;
}

/**
 * Sets the displayName property
 *
 * @param displayName the displauName property
 *
 */
void DataStore::setDisplayName(const char*displayName) {
    delete [] this->displayName;
    this->displayName = stringdup(displayName);
}

/**
 * Gets the maxGUIDSize properties
 *
 * @return the maxGUIDSize properties
 */
long DataStore::getMaxGUIDSize() {
    return maxGUIDSize;
}

void DataStore::setMaxGUIDSize(long maxGUIDSize) {
    this->maxGUIDSize = maxGUIDSize;
}

/**
 * Gets the ContentTypeInfo corresponds to &lt;Rx-Pref&gt; element
 *
 * @return the ContentTypeInfo corresponds to &l;tRx-Pref&gt; element
 */
ContentTypeInfo* DataStore::getRxPref() {
    return rxPref;
}

/**
 * Sets the preferred type and version of a content type received by the device
 *
 * @param rxPref the preferred type and version of a content type
 */
void DataStore::setRxPref(ContentTypeInfo* rxPref) {
    if (rxPref == NULL) {
        // TBD
    } else {
        delete this->rxPref;
    }
    this->rxPref = (ContentTypeInfo*)rxPref->clone();
}

/**
 * Gets the ContentTypeInfo corresponds to &lt;Rx&gt; element
 *
 * @return the ContentTypeInfo corresponds to &lt;Rx&gt; element
 */
ArrayList* DataStore::getRx() {
    return rx;
}

/**
 * Sets the supported type and version of a content type received by the device
 *
 * @param rxCTI and array of supported type and version of a content type
 */
void DataStore::setRx(ArrayList* rxCTI) {
    if (rxCTI == NULL) {
        // TBD
    } else {
        if (rx) {
            rx->clear();
        }
        rx = rxCTI->clone();
    }
}


/**
 * Gets the ContentTypeInfo corresponds to &lt;Tx-Pref&gt; element
 *
 * @return the ContentTypeInfo corresponds to &lt;Tx-Pref&gt; element
 */
ContentTypeInfo* DataStore::getTxPref() {
    return txPref;
}

/**
 * Sets the preferred type and version of a content type trasmitted by the device
 *
 * @param txPref the preferred type and version of a content type
 */
void DataStore::setTxPref(ContentTypeInfo* txPref) {
    if (txPref == NULL) {
        // TBD
    } else {
        delete this->txPref;
        this->txPref = (ContentTypeInfo*)txPref->clone();
    }
}

/**
 * Gets an array of ContentTypeInfo corresponds to &lt;Tx&gt; element
 *
 * @return an array of ContentTypeInfo corresponds to &lt;Tx&gt; element
 */
ArrayList* DataStore::getTx() {
    return tx;
}

/**
 * Sets the supported type and version of a content type trasmitted by the device
 *
 * @param txCTI and array of supported type and version of a content type
 */
void DataStore::setTx(ArrayList* txCTI) {
    if (txCTI == NULL) {
        // TBD
    } else {
        if (tx) {
            tx->clear();
        }
        tx = txCTI->clone();
    }
}

/**
* Gets an array of CtCaps corresponds to &lt;CTCap&gt; element
*
* @return an array of CTCaps corresponds to &lt;CTCap&gt; element
*/
ArrayList* DataStore::getCtCaps(){
    return ctCaps;
}

/**
* Sets an array of CtCaps
*
* @param Ct_Caps an array of Ctcaps
*/
void DataStore::setCtCaps(ArrayList* ct_Caps){
    ctCaps = ct_Caps;
}

/**
 * Gets the datastore memory information.
 *
 * @return the datastore memory information.
 */
DSMem* DataStore::getDSMem() {
    return dsMem;
}

/**
 * Sets the datastore memory information
 *
 * @param dsMem the datastore memory information
 */
void DataStore::setDSMem(DSMem* dsMem) {
    delete this->dsMem;
    if (dsMem) {
        this->dsMem = dsMem->clone();
    }
}

/**
 * Gets the synchronization capabilities of a datastore.
 *
 * @return the synchronization capabilities of a datastore.
 */
SyncCap* DataStore::getSyncCap() {
    return syncCap;
}

/**
 * Sets the synchronization capabilities of a datastore.
 *
 * @param syncCap the synchronization capabilities of a datastore
 *
 */
void DataStore::setSyncCap(SyncCap* syncCap) {
     if (syncCap == NULL) {
            // TBD
     } else {
        delete this->syncCap;
        this->syncCap = syncCap->clone();
    }
}

ArrayElement* DataStore::clone() {
    DataStore* ret = new DataStore( sourceRef,
                                    displayName ,
                                    maxGUIDSize ,
                                    rxPref      ,
                                    rx          ,
                                    txPref      ,
                                    tx          ,
                                    ctCaps      ,
                                    dsMem       ,
                                    syncCap     );
    return ret;

}

