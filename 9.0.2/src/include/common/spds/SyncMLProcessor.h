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

#ifndef INCL_SYNCML_PROCESSOR
    #define INCL_SYNCML_PROCESSOR
/** @cond DEV */

    #include "base/globalsdef.h"
    #include "base/util/ArrayList.h"
    #include "base/util/XMLProcessor.h"
    #include "spds/SyncSource.h"
    #include "spds/SyncItem.h"
    #include "spds/SyncItemStatus.h"
    #include "syncml/core/TagNames.h"
    #include "syncml/core/ObjectDel.h"
    #include "syncml/parser/Parser.h"
    #include "spds/SyncReport.h"
    #include "spds/SyncItemKeys.h"

BEGIN_NAMESPACE

    /*
     * This class is responsible for the processing of the incoming messages.
     */

    class SyncMLProcessor : public XMLProcessor {

    private:

        /*
         * It include the common part of getSyncHdrStatusCode and getAlertStatusCode
         */
        int getStatusCode(SyncBody* syncBody, SyncSource* source, const char* commandName);
        /*
         * Returns the status code for the SyncHeader command included
         * in the message sent by the client.
         *
         * @param syncBody - the SyncBody content
         */
        int getSyncHeaderStatusCode(Status* s);
        /*
         * Returns the status code for the Alert relative to the given source.
         *
         * @param syncBody - the SyncBody content
         * @param sourceName - the name of the source
         */
        int getAlertStatusCode(Status* status, const char*  sourceName);

        /*
        * Return the command of the given commandName
        */
        AbstractCommand* getCommand(SyncBody* syncBody, const char* commandName, int index);

         /*
        * To get a generic array element. It returns the <index> arrayElement it founds.
        * 0-based.
         */
        ArrayElement* getArrayElement(ArrayList* list, int index);

    public:

        /*
         * Constructor
         */
        SyncMLProcessor();

        /*
        * Process a generic syncml message and return a SyncML object
        */
        SyncML* processMsg(char*  msg);

        /*
         * Processes the initialization response. Returns 0 in case of success, an
         * error code in case of error.
         *
         * @param msg the response from the server
         */
        int processInitResponse(SyncSource& source, SyncML* syncml, Alert* alert);

        int processSyncHdrStatus(SyncML* syncml);
        int processAlertStatus(SyncSource& source, SyncML* syncml, ArrayList* alerts);

        int processServerAlert(SyncSource& source, SyncML* syncml);

        /**
         * Process Get command from the Server.
         * If Server asks for Client's devInf, we reply sending them (only if devInf is not NULL).
         * Anyway if there was a Get command, we always reply with a 200 status.
         *
         * @param cmd    the SyncML command containing the Server Put to process
         * @param devInf the deviceInfo corresponding to Client's capabilities
         * @return       new allocated ArrayList of commands to be added in the response msg
         */
        ArrayList* processGetCommand(AbstractCommand* cmd, DevInf* devInf);

        /**
         * Process Put command from the Server.
         * If Server devInf found, they're processed and stored (processServerDevInf).
         * Anyway if there was a Put command, we always reply with a 200 status.
         *
         * @param cmd    the SyncML command containing the Server Put to process
         * @param config reference to AbstractSyncConfig configuration
         * @return       new allocated ArrayList of commands to be added in the response msg
         */
        ArrayList* processPutCommand(AbstractCommand* cmd, AbstractSyncConfig& config);

        /**
         * Parse and process Server device capabilities if found.
         * If so, stores all the data in configuration (under Server node).
         * It can be called either in case of Server response to our Get command (Results)
         * or in case of a Put command from the Server.
         * 
         * @param cmd    the SyncML command containing the Server devInf to process
         * @param config reference to AbstractSyncConfig configuration
         * @return true  if Server devInf found and processed
         */
        bool processServerDevInf(AbstractCommand* cmd, AbstractSyncConfig& config);

        /*
        * Get the chal from a syncBody object. It is used to get the auth type and next nonce if needed
        */
        Chal* getChal(SyncBody* syncBody);

        /*
        * Get server credential. It is used by the SyncManager to get the server credentials and check them
        */
        Cred* getServerCred(SyncHdr* syncHdr);


        /*
         * Process the SyncBody and looks for the item status of the sent items.
         * It calls the setItemStatus method of the sync source.
         */
        int processItemStatus(SyncSource& source, SyncBody* syncBody, SyncItemKeys& syncItemKeys);

        /*
         * Processes the response and get the Sync command of the given source
         *
         * @param source the source
         * @param syncml the syncML Object the response from the server
         */
        Sync* processSyncResponse(SyncSource& source, SyncML* syncml);

        /*
         * Processes the map message response. Returns 0 in case of success.
         * Currently it return always 0. TBD
         *
         * @param source the source
         * @param msg the response from the server
         */
        int processMapResponse(SyncSource& source, SyncBody* syncBody);

        /*
         * Returns the SyncHeader/RespURI element of the given message. If the element is not
         * found it returns NULL. The returned respURI is allocated with the new operator
         * and must be discarded with delete by the caller.
         *
         * @param msg - the SyncHdr message - NOT NULL
         */
        const char* getRespURI(SyncHdr* msg);

        /*
         * Returns an ArrayList containing the command given by commandName. It uses the getCommand method
         */
        ArrayList* getCommands(SyncBody* syncBody, const char* commandName);

        /* To retrieve a (NULL terminated) list of source names from list of Alert commands from server.
         * @return: a new array of source names (NULL terminated) - must be freed by the caller.
         */
        char** getSortedSourcesFromServer(SyncML* syncml, int sourcesNumber);

        /* To retrieve a Sync pointer from ArrayList of Sync objects.
         * It gets the order like the server sends
         */
        Sync* getSyncResponse(SyncML* syncml, int index);
    };


END_NAMESPACE

/** @endcond */
#endif
