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

#ifndef INCL_SYNCCLIENT
    #define INCL_SYNCCLIENT
/** @cond API */
/** @addtogroup Client */
/** @{ */

    #include "base/fscapi.h"
    #include "base/Log.h"
    #include "spds/AbstractSyncConfig.h"
    #include "spds/SyncSource.h"
    #include "spds/constants.h"
    #include "spds/SyncReport.h"
#include "base/globalsdef.h"
#include "http/TransportAgent.h"

BEGIN_NAMESPACE

    /**
     * This class wraps the common operations executed by a typical
     * client. It contains default implementations for calls invoked
     * by the library core to parameterize or control the
     * synchronization. A client can override these calls by
     * subclassing SyncClient and using an instance of its own class
     * instead.
     *
     * Warning: the library contains global data in several different
     * places. Therefore it is not possible to execute multiple
     * sessions in parallel.
     */
    class SyncClient {
      public:

        SyncClient();
        virtual ~SyncClient();

        /*
         * Execute a synchronization on the specified sources.
         * The sources will be configured automatically using the
         * client configuration set in the constructor.
         *
         * @param config  - the configuration to be used for this sync
         * @param sources - NULL terminated array of sources to sync.
         *
         * @return - 0 on success, an error otherwise
         */
        virtual int sync(AbstractSyncConfig& config, SyncSource** sources);

        /**
         * Execute a synchronization with sources that are chosen based
         * on the sourceNames param or, if sourceNames is NULL, based
         * on the configuration: in this case all available sources will be used.
         *
         * - calls prepareSync() to give a derived class the chance to
         *   setup syncing and get ready for creating sources
         * - iterates over sources to sync, asks for the corresponding
         *   client source with createSource()
         * - calls beginSync() to give the derived class a class to do
         *   after all sources have been configured successfully
         * - executes the synchronization: call sync(sources**)
         *
         * This version of the call is easier to use for clients whose
         * sync sources are determined by the configuration or by an array
         * of desired sources to sync
         *
         * @param  config:      the configuration to be used for this sync
         * @param  sourceNames: optional, a NULL terminated array of source names that
         *                      we want to sync. If NULL, sources to sync are chosen
         *                      from the configuration.
         * @return 0 on success, an error otherwise
         */
        virtual int sync(AbstractSyncConfig& config, char** sourceNames = NULL);

        /*
         * Returns a pointer to the internal syncReport.
         * Used to get detailed results on the executed synchronization.
         * Must be called after sync() method.
         */
        SyncReport* getSyncReport();

        /**
         * Sets a defined TransportAgent to be used during sync.
         * Note: the passed pointer will be owned and deleted by the APIs, so 
         * it MUST NOT be deleted by the caller.
         */
        void setTransportAgent(TransportAgent* t);

        /**
         * Sets the last sync results in the passed config, and the last source resupt
         * for all active sources in SyncReport. The values are read from
         * the SyncReport and SyncSourceReport array.
         * This method is called at the end of sync, to save the last error informations.
         */
        void setLastSyncResults(AbstractSyncConfig& config);


      protected:
        /**
         * A callback into a derived class which is called after
         * reading the SyncML configuration and before creating sources.
         * Not used by the sync(SyncSource**) call.
         *
         * @param config   a reference to SyncManager configuration
         *                 can be used to read client specific properties
         * @return 0       for success, an error code otherwise - an error code
         *                 aborts the whole synchronization
         */
        virtual int prepareSync(AbstractSyncConfig& /* config */) {
            return ERR_NONE;
        }

        /**
         * A factory method that is used by the sync() call to create
         * the sync sources that are to be synchronized. Not used by the
         * sync(SyncSource**) call.
         *
         * @param name     name of the sync source
         * @param pos      position of the SyncSource in the SSConfig array
         *                 (index: 0 - numSources-1)
         * @param config   a pointer to the source's configuration: this
         *                 includes all properties that the client library
         *                 knows and uses itself (only valid during this call)
         *                 This pointer is owned by AbstractSyncConfig, sources uses it
         *                 to initialize the internal config by reference.
         * @retval source  the sync source created by the client or NULL if
         *                 there is no sync source currently associated with
         *                 the config or it is inactive; instance is a new SyncSource*
         *                 and it's freed by the caller (see finally)
         * @return 0 for success, an error code otherwise - an error code
         *         aborts the whole synchronization and the value of *source is
         *         ignored
         */
        virtual int createSyncSource(const char * /* name */, const int /* pos */,
                                     AbstractSyncSourceConfig* /* config */,
                                     SyncSource **source) {
            *source = NULL;
            return ERR_UNSPECIFIED;
        }

        /**
         * Callback invoked after creating all sources.
         * @return 0 for success, an error code otherwise - an error code stops
         *         immediately
         */
        virtual int beginSync(SyncSource ** /* source */) {
            return ERR_NONE;
        }

        /**
         * Callback invoked after the sync process.
         * @return: 0 for success, an error code otherwise
         */
        virtual int endSync(SyncSource ** /* source */) {
            return ERR_NONE;
        }

        /**
         * Callback invoked after the prepareSync function.
         * @return 0 for success, an error code otherwise - an error code stops
         *         immediately
         */
        virtual int continueAfterPrepareSync() {
            return ERR_NONE;
        }

        /**
         * Callback invoked after the sync function.
         * @return: 0 for success, an error code otherwise
         */
        virtual int continueAfterSync() {
            return ERR_NONE;
        }

        /// The report of the synchronization process.
        /// Sources reports are initializated during sync(sources**) call.
        SyncReport syncReport;

        /** 
         * If not NULL, this TransportAgent will be used during sync.
         * Clients can set thir own TransportAgent calling method setTransportAgent().
         * Note: it's not owned by the SyncClient, if not NULL it will be deleted by
         *       SyncManager at the end of the sync.
         */
        TransportAgent* transportAgent;
    };


END_NAMESPACE

/** @} */
/** @endcond */
#endif

