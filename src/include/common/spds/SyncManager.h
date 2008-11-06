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

#ifndef INCL_SYNC_MANAGER
#define INCL_SYNC_MANAGER
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/util/ArrayList.h"
#include "http/TransportAgent.h"
#include "spds/constants.h"
#include "spds/SyncManagerConfig.h"
#include "spds/SyncSource.h"
#include "spds/SyncMLBuilder.h"
#include "spds/SyncMLProcessor.h"
#include "spds/CredentialHandler.h"
#include "spds/CredentialHandler.h"
#include "spds/SyncReport.h"


typedef enum {
                STATE_START        = 0,
                STATE_PKG1_SENDING = 1,
                STATE_PKG1_SENT    = 2,
                STATE_PKG3_SENDING = 3,
                STATE_PKG3_SENT    = 4,
                STATE_PKG5_SENDING = 5,
                STATE_PKG5_SENT    = 6
             } SyncManagerState ;


// Tolerance to data size for incoming items (106%) -> will be allocated some more space.
#define DATA_SIZE_TOLERANCE      1.06


static void fillContentTypeInfoList(ArrayList &l, const char*  types);


/**
 * This is the core class which encodes the flow of messages between
 * client and server throughout a session. It is configured via the
 * DMTClientConfig with which it is constructed by the
 * SyncClient::setDMConfig() and the (optional) DevInf provided
 * to it by the client.
 */
class SyncManager {

    public:
        /**
         * Initialize a new sync manager. Parameters provided to it
         * have to remain valid while this sync manager exists.
         *
         * @param config     required configuration
         * @param report     sync report reference to store sync results
         */
        SyncManager(SyncManagerConfig& config, SyncReport& report);
        ~SyncManager();

        int prepareSync(SyncSource** sources);

        int sync();

        int endSync();

        /**
         * Gathers the various bits and pieces known about the client and
         * its sources and builds a SyncML devinfo 1.1 instance.
         *
         * For simplicity reasons this function is called for the currently
         * active sync sources, changing them between runs thus causes
         * a (valid!) retransmission of the device info.
         *
         * @return device infos, to be deleted by caller, or NULL if unavailable
         */
        virtual DevInf *createDeviceInfo();

    private:

        // SyncManager makes local key safe to use in SyncML by
        // encoding it as b64 if it contains special characters. The
        // SyncML standard says that the key should be a "URI" or
        // "URN"; we interpret that less strictly as "do not
        // include characters which actually break XML".
        //
        // Encoded keys are sent as "funambol-b64-<encoded original
        // key>". When receiving a key from the server it is only decoded
        // if it contains this magic tag, therefore an updated client
        // remains compatible with a server that already contains keys.
        static const char encodedKeyPrefix[];

        void encodeItemKey(SyncItem *syncItem);
        void decodeItemKey(SyncItem *syncItem);

        // Struct used to pass command info to the method processSyncItem
        struct CommandInfo {
            const char* commandName;
            const char* cmdRef;
            const char* format;
            const char* dataType;
            long size;
        };

        DevInf* devInf;
        SyncManagerConfig& config;
        SyncReport& syncReport;

        CredentialHandler credentialHandler;
        SyncMLBuilder syncMLBuilder;
        SyncMLProcessor syncMLProcessor;
        TransportAgent* transportAgent;

        SyncManagerState currentState;
        SyncSource** sources;
        ArrayList* commands;
        ArrayList** mappings;

        // Now using sources[i].checkState() method
        //int* check;

        int  sourcesNumber;
        int  count;

        /* A list of syncsource names from server. The server sends sources
         * modifications sorted as alerts in this list. This array is retrieved from
         * SyncMLProcessor::getSortedSourcesFromServer.
         */
        char** sortedSourcesFromServer;

		ArrayList** allItemsList;

        StringBuffer syncURL;
        StringBuffer deviceId;
        int responseTimeout;  // the response timeout for a rensponse from server (default = 5min) [in seconds]
        int maxMsgSize;       // the max message size. Default = 512k. Setting it implies LargeObject support.
        int maxObjSize;       // The maximum object size. The server gets this in the Meta init message and should obey it.
        BOOL loSupport;             // enable support for large objects - without it large outgoing items are not split
        unsigned int readBufferSize; // the size of the buffer to store chunk of incoming stream.
        char  credentialInfo[1024]; // used to store info for the des;b64 encription

        // Handling of incomplete incoming objects by processSyncItem().
        // Always active, even if Large Object support is off,
        // just in case the server happens to rely on it.
        //
        class IncomingSyncItem : public SyncItem {
          public:
            IncomingSyncItem(const WCHAR* key,
                             const CommandInfo &cmdInfo,
                             int currentSource) :
                SyncItem(key),
                offset(0),
                cmdName(cmdInfo.commandName),
                cmdRef(cmdInfo.cmdRef),
                sourceIndex(currentSource) {
            }

            long offset;                // number of bytes already received, append at this point
            const StringBuffer cmdName; // name of the command which started the incomplete item
            const StringBuffer cmdRef;  // reference of the command which started the incomplete item
            const int sourceIndex;      // the index of the source to which the incomplete item belongs
        } *incomingItem;       // sync item which is not complete yet, more data expected

        void initialize();
        BOOL readSyncSourceDefinition(SyncSource& source);
        BOOL commitChanges(SyncSource& source);
        int assignSources(SyncSource** sources);

        Status *processSyncItem(Item* item, const CommandInfo &cmdInfo, SyncMLBuilder &syncMLBuilder);
        BOOL checkForServerChanges(SyncML* syncml, ArrayList &statusList);

        const char*  getUserAgent(SyncManagerConfig& config);
        bool isToExit();
        void setSourceStateAndError(unsigned int index, SourceState  state,
                                    unsigned int code,  const char*  msg);


        // Used to reserve some more space (DATA_SIZE_TOLERANCE) for incoming items.
        long getToleranceDataSize(long size);
        bool testIfDataSizeMismatch(long allocatedSize, long receivedSize);

        /**
         * A wrapper around the sync source's first/next iterator functions.
         * By default the data is encoded according to the "encoding"
         * SyncSourceConfig property, unless the SyncSource already set an encoding.
         *
         * In case of an error the error is logged and the item is set to NULL, just as
         * if the source itself had returned NULL.
         */
        SyncItem* getItem(SyncSource& source, SyncItem* (SyncSource::* getItem)());
};

/** @} */
/** @endcond */
#endif

