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

#ifndef INCL_SYNC_MANAGER
#define INCL_SYNC_MANAGER
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "http/TransportAgent.h"
#include "spds/constants.h"
#include "spds/AbstractSyncConfig.h"
#include "spds/SyncSource.h"
#include "spds/SyncMLBuilder.h"
#include "spds/SyncMLProcessor.h"
#include "spds/CredentialHandler.h"
#include "spds/CredentialHandler.h"
#include "spds/SyncReport.h"
#include "spds/MappingsManager.h"

// Tolerance to data size for incoming items (106%) -> will be allocated some more space.
#define DATA_SIZE_TOLERANCE      1.06

BEGIN_NAMESPACE

typedef enum {
                STATE_START        = 0,
                STATE_PKG1_SENDING = 1,
                STATE_PKG1_SENT    = 2,
                STATE_PKG3_SENDING = 3,
                STATE_PKG3_SENT    = 4,
                STATE_PKG5_SENDING = 5,
                STATE_PKG5_SENT    = 6
             } SyncManagerState ;


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
        SyncManager(AbstractSyncConfig& config, SyncReport& report);
        ~SyncManager();

        /**
         * Prepares the sync, performing authentication and 
         * device capability exchange.
         */
        int prepareSync(SyncSource** sources);

        /**
        * Starts the synchronization phase
        *
        * @return a status code for the sync. Can be:
        *              0 on success
        *              an interal code (see: TODO)
        *              a status code from the server
        */
        int sync();

        /**
         * Performs the commit phase of the synchronization.
         */
        int endSync();

        /**
         * Gathers the various bits and pieces known about the client and
         * its sources and builds a SyncML devinfo 1.2 instance.
         *
         * For simplicity reasons this function is called for the currently
         * active sync sources, changing them between runs thus causes
         * a (valid!) retransmission of the device info.
         *
         * @return device infos, to be deleted by caller, or NULL if unavailable
         */
        DevInf *createDeviceInfo();

        /**
         * Checks whether we need to ask the Server capabilities or not.
         * If we don't have enough Server information in the config, we need 
         * to ask the Server caps.
         * A Client can force to ask Server caps again by .
         * 
         * @return  true if we need to Get defInf to the Server
         */
        bool askServerDevInf();

        /**
         * Utility method, clears all the configuration params of Server caps.
         * It is used to cleanup the Server caps information for example if the
         * Server address changes.
         */
        void clearServerDevInf();


        /**
         * Sets a defined TransportAgent to be used during sync.
         * Note: the passed pointer will be owned and deleted by SyncManager, so 
         * it MUST NOT be deleted by the caller.
         */
        void setTransportAgent(TransportAgent* t);

        /**
         * Scans the mappings (enumeration of pairs LUID,GUID) and searches for the
         * correspondent LUID (key) associated to the passed GUID (value).
         * It's used in case the SourceParent sent by the Server on a ADD command is a 
         * GUID value, it can happen if the Client didn't reply yet with the corresponding mapping.
         * @note If GUID not found, the passed GUID is returned.
         * 
         * @param mappings the mappings Enumeration of KeyValuePair (LUID,GUID) to search into
         * @param guid     the GUID value to search
         * @return         the correspondent LUID value. If GUID not found, empty string is returned.
         */
        const StringBuffer lookupMappings(Enumeration& mappings, const StringBuffer& guid);


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
        AbstractSyncConfig& config;
        SyncReport& syncReport;

        CredentialHandler credentialHandler;
        SyncMLBuilder syncMLBuilder;
        SyncMLProcessor syncMLProcessor;

        /// The transportAgent used during sync.
        /// It will ALWAYS be deleted in the destructor.
        TransportAgent* transportAgent;

        SyncManagerState currentState;
        SyncSource** sources;
        ArrayList commands;        
        
        MappingsManager** mmanager;
        // Now using sources[i].checkState() method
        //int* check;

        int  sourcesNumber;
        int  count;

        /* A list of syncsource names from server. The server sends sources
         * modifications sorted as alerts in this list. This array is retrieved from
         * SyncMLProcessor::getSortedSourcesFromServer.
         */
        char** sortedSourcesFromServer;
        
        StringBuffer syncURL;
        StringBuffer deviceId;
        int responseTimeout;  // the response timeout for a rensponse from server (default = 5min) [in seconds]
        int maxMsgSize;       // the max message size. Default = 512k. Setting it implies LargeObject support.
        int maxObjSize;       // The maximum object size. The server gets this in the Meta init message and should obey it.
        bool loSupport;             // enable support for large objects - without it large outgoing items are not split
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
        bool readSyncSourceDefinition(SyncSource& source);
        bool commitChanges(SyncSource& source);
        int assignSources(SyncSource** sources);

        Status *processSyncItem(Item* item, const CommandInfo &cmdInfo, SyncMLBuilder &syncMLBuilder);
        bool checkForServerChanges(SyncML* syncml, ArrayList &statusList);

        /**
         * Initializes the transportAgent class attribute, setting all the
         * relevant information from the config.
         */
        void initTransportAgent(const URL& url);

        /**
         * Return true if there's no more work to do
         * (if no source has a correct status)
         */
        bool isToExit();

        //Utility method to set a SyncSource state + errorCode + errorMsg.
        void setSourceStateAndError(unsigned int index, SourceState  state,
                                    unsigned int code,  const char*  msg);


        // Used to reserve some more space (DATA_SIZE_TOLERANCE) for incoming items.
        long getToleranceDataSize(long size);
        // Used to verify if data size of incoming item is different from the one declared.
        bool testIfDataSizeMismatch(long allocatedSize, long receivedSize);

        /**
         * A wrapper around the sync source's first/next iterator functions.
         * By default the data is encoded according to the "encoding"
         * AbstractSyncSourceConfig property, unless the SyncSource already set an encoding.
         *
         * In case of an error the error is logged and the item is set to NULL, just as
         * if the source itself had returned NULL.
         */
        SyncItem* getItem(SyncSource& source, SyncItem* (SyncSource::* getItem)());

        /**
         * Add the map command according to the current value of the 
         * member 'mappings', and clean up the member afterwards.
         */
        void addMapCommand(int sourceIndex);    

};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

