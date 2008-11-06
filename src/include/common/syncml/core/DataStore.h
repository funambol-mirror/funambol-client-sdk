/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_DATA_STORE
#define INCL_DATA_STORE
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/SourceRef.h"
#include "syncml/core/ContentTypeInfo.h"
#include "syncml/core/DSMem.h"
#include "syncml/core/SyncCap.h"


class DataStore : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        SourceRef*       sourceRef;
        char*          displayName;
        long             maxGUIDSize;
        ContentTypeInfo* rxPref;
        ArrayList*       rx; // ContentTypeInfo[]
        ContentTypeInfo* txPref;
        ArrayList*       tx; // ContentTypeInfo[]
        DSMem*           dsMem;
        SyncCap*         syncCap;

        void initialize();

    // ---------------------------------------------------------- Public data
    public:

        DataStore();
        ~DataStore();

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
        DataStore(SourceRef* sourceRef,
                  char*  displayName,
                  long maxGUIDSize,
                  ContentTypeInfo* rxPref,
                  ArrayList* rx,
                  ContentTypeInfo* txPref,
                  ArrayList* tx,
                  DSMem* dsMem,
                  SyncCap* syncCap);

        /**
         * Gets the sourceRef properties
         *
         * @return the sourceRef properties
         */
        SourceRef* getSourceRef();

        /**
         * Sets the reference URI
         *
         * @param sourceRef the reference URI
         *
         */
        void setSourceRef(SourceRef* sourceRef);

        /**
         * Gets the displayName properties
         *
         * @return the displayName properties
         */
        const char* getDisplayName();

        /**
         * Sets the displayName property
         *
         * @param displayName the displauName property
         *
         */
        void setDisplayName(const char* displayName);

        /**
         * Gets the maxGUIDSize properties
         *
         * @return the maxGUIDSize properties
         */
        long getMaxGUIDSize();

        void setMaxGUIDSize(long maxGUIDSize);

        /**
         * Gets the ContentTypeInfo corresponds to &lt;Rx-Pref&gt; element
         *
         * @return the ContentTypeInfo corresponds to &l;tRx-Pref&gt; element
         */
        ContentTypeInfo* getRxPref();

        /**
         * Sets the preferred type and version of a content type received by the device
         *
         * @param rxPref the preferred type and version of a content type
         */
        void setRxPref(ContentTypeInfo* rxPref);

        /**
         * Gets the ContentTypeInfo corresponds to &lt;Rx&gt; element
         *
         * @return the ContentTypeInfo corresponds to &lt;Rx&gt; element
         */
        ArrayList* getRx();

        /**
         * Sets the supported type and version of a content type received by the device
         *
         * @param rxCTI and array of supported type and version of a content type
         */
        void setRx(ArrayList* rxCTI);


        /**
         * Gets the ContentTypeInfo corresponds to &lt;Tx-Pref&gt; element
         *
         * @return the ContentTypeInfo corresponds to &lt;Tx-Pref&gt; element
         */
        ContentTypeInfo* getTxPref();

        /**
         * Sets the preferred type and version of a content type trasmitted by the device
         *
         * @param txPref the preferred type and version of a content type
         */
        void setTxPref(ContentTypeInfo* txPref);

        /**
         * Gets an array of ContentTypeInfo corresponds to &lt;Tx&gt; element
         *
         * @return an array of ContentTypeInfo corresponds to &lt;Tx&gt; element
         */
        ArrayList* getTx();

        /**
         * Sets the supported type and version of a content type trasmitted by the device
         *
         * @param txCTI and array of supported type and version of a content type
         */
        void setTx(ArrayList* txCTI);

        /**
         * Gets the datastore memory information.
         *
         * @return the datastore memory information.
         */
        DSMem* getDSMem();

        /**
         * Sets the datastore memory information
         *
         * @param dsMem the datastore memory information
         */
        void setDSMem(DSMem* dsMem);

        /**
         * Gets the synchronization capabilities of a datastore.
         *
         * @return the synchronization capabilities of a datastore.
         */
        SyncCap* getSyncCap();

        /**
         * Sets the synchronization capabilities of a datastore.
         *
         * @param syncCap the synchronization capabilities of a datastore
         *
         */
        void setSyncCap(SyncCap* syncCap);

        ArrayElement* clone();

};

/** @endcond */
#endif
