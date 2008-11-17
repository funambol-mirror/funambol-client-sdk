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

#include "base/util/utils.h"
#include "syncml/core/DataStore.h"

DataStore::DataStore() {
    initialize();
}

DataStore::~DataStore() {
   if(sourceRef   )   { delete sourceRef      ;  sourceRef       = NULL; }
   if(displayName )   { delete [] displayName    ;  displayName     = NULL; }
   maxGUIDSize = 0;
   if(rxPref      )   { delete    rxPref         ;  rxPref          = NULL; }
   if(rx          )   { rx->clear(); } //delete rx; rx = NULL;                  }
   if(txPref      )   { delete    txPref         ;  txPref          = NULL; }
   if(tx          )   { tx->clear(); }//delete tx; tx = NULL;                  }
   if(dsMem       )   { delete    dsMem          ;  dsMem           = NULL; }
   if(syncCap     )   { delete    syncCap        ;  syncCap         = NULL; }
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
 * @param dsMem the datastore memory info
 * @param syncCap the synchronization capabilities - NOT NULL
 *
 */
DataStore::DataStore(SourceRef* sourceRef,
                      char* displayName,
                      long maxGUIDSize,
                      ContentTypeInfo* rxPref,
                      ArrayList* rx,
                      ContentTypeInfo* txPref,
                      ArrayList* tx,
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
        setDisplayName(displayName);
        setDSMem(dsMem);
}

void DataStore::initialize() {
    sourceRef       = NULL;
    displayName     = NULL;
    maxGUIDSize     = 0;
    rxPref          = NULL;
    rx              = new ArrayList();
    txPref          = NULL;
    tx              = new ArrayList();
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
        if (this->sourceRef) {
            delete this->sourceRef; this->sourceRef = NULL;
        }
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
    if (this->displayName) {
        delete [] this->displayName; this->displayName = NULL;
    }
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
        if (this->rxPref) {
            delete this->rxPref; this->rxPref = NULL;
        }
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
        if (this->txPref) {
            delete this->txPref; this->txPref = NULL;
        }
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
    if (this->dsMem) {
        delete this->dsMem; this->dsMem = NULL;
    }
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
        if (this->syncCap) {
		    delete this->syncCap; this->syncCap = NULL;
        }
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
                                    dsMem       ,
                                    syncCap     );
    return ret;

}
