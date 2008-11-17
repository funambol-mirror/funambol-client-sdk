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

#ifndef INCL_SYNCML_PROCESSOR
    #define INCL_SYNCML_PROCESSOR
/** @cond DEV */

    #include "base/util/ArrayList.h"
    #include "base/util/XMLProcessor.h"
    #include "spds/SyncSource.h"
    #include "spds/SyncItem.h"
    #include "spds/SyncItemStatus.h"
    #include "syncml/core/TagNames.h"
    #include "syncml/core/ObjectDel.h"
    #include "syncml/parser/Parser.h"
    #include "spds/SyncReport.h"

    /*
     * This class is responsible for the processing of the incoming messages.
     */

    class __declspec(dllexport) SyncMLProcessor : public XMLProcessor {

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
        int processItemStatus(SyncSource& source, SyncBody* syncBody);

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

/** @endcond */
#endif
