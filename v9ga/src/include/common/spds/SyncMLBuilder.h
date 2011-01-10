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

#ifndef INCL_SYNCML_BUILDER
#define INCL_SYNCML_BUILDER
/** @cond DEV */

#include "spds/DataTransformer.h"
#include "spds/SyncSource.h"
#include "spds/SyncMap.h"
#include "syncml/core/TagNames.h"
#include "syncml/core/ObjectDel.h"
#include "syncml/formatter/Formatter.h"
#include "base/globalsdef.h"
#include "spds/Chunk.h"

BEGIN_NAMESPACE

typedef enum {
	SYNCML_DM_1_1 = 101,
	SYNCML_DM_1_2,
	SYNCML_1_0 = 200,
	SYNCML_1_1,
	SYNCML_1_2
} SyncMLProtocolFlavour;
		

class SyncMLBuilder {
    public:
        SyncMLBuilder(SyncMLProtocolFlavour protocol = SYNCML_1_2);
        ~SyncMLBuilder();
        SyncMLBuilder(char*  t, char*  d, SyncMLProtocolFlavour protocol = SYNCML_1_2);

        /*
         * Convert the SyncML object into an xml message
         */
        char*  prepareMsg(SyncML* syncml);

        /*
         * Convert the SyncML object into an xml message
         */
        SyncML*  prepareSyncML(ArrayList* commands, bool final);

        /*
        * Set init parameters.
        *
        */
        void     set(const char* t, const char* d);

        /*
        * Prepare the init SyncML* message with credential and db alert to sync
        *
        * @param maxMsgSize       used as MaxMsgSize value in Meta part of the message unless 0
        * @param maxObjSize       used as MaxObjSize value in Meta part of the message unless 0
        */
        SyncML*  prepareInitObject(Cred* cred, ArrayList* alerts, ArrayList* commands,
                                   unsigned long maxMsgSize = 0, unsigned long maxObjSize = 0);

        /*
        * Prepare the SyncHdr message with credential if not null
        *
        * @param maxMsgSize       used as MaxMsgSize value in Meta part of the message unless 0
        * @param maxObjSize       used as MaxObjSize value in Meta part of the message unless 0
        */
        SyncHdr* prepareSyncHdr(Cred* cred, unsigned long maxMsgSize = 0, unsigned long maxObjSize = 0);

        /*
        * Prepare the init alert
        */
        Alert*   prepareInitAlert(SyncSource& source, unsigned long maxObjSize = 0);

        /*
        * Prepare the special init alert for Address Change Notification
        */
        Alert*   prepareAddrChangeAlert(SyncSource& source);

        /*
        * Prepare alert with a specific code, defaults to 222 which requests
        * the server changes.
        */
        Alert*   prepareAlert(SyncSource& source, int code=222);

        /*
        * Prepare the status for Sync Header
        */
        Status*  prepareSyncHdrStatus(Chal* chal, int data);

        /*
        * Prepare the status for Alert command
        */
        Status*  prepareAlertStatus(SyncSource& s,  ArrayList* alert, int authStatusCode);

        /*
        * Prepare the status for Item command. COMMAND could be ADD, REPLACE, DEL
        */
        Status*  prepareItemStatus(const char*  COMMAND, const char*  key, const char*  cmdRef, int code);

        /*
        * Add the status to the corrent list of commands. It is responsible to collapse the status if needed
        */
        void     addItemStatus(ArrayList* previousStatus, Status* status);

        /*
        * Prepare the status for Sync command
        */
        Status*  prepareSyncStatus(SyncSource& source, Sync* sync);

        /**
         * Prepare the status for an arbitrary command
         */
        Status* prepareCmdStatus(AbstractCommand &cmd, int status);

        /**
         * Prepare either a Result with the device infos (if command given) or
         * a Put.
         */
        AbstractCommand *prepareDevInf(AbstractCommand *cmd, DevInf &devInf);
        
        /**
         * Prepare a Get for the Server device infos.
         */
        AbstractCommand* prepareServerDevInf();

        /*
        * Prepare the Sync object. It doesn't contain any items. It is to prepare the insert of items
        */
        Sync*    prepareSyncCommand(SyncSource& source);

        /*
        * Prepare the Map object. It doesn't containt anything. It is to prepare the insert of map items
        */
        Map*     prepareMapCommand(SyncSource& source);

        /*
        * Prepare the MapItem. It could contain only one MapItem
        */
        MapItem* prepareMapItem(SyncMap* syncMap);

        /*
         * @param[in, out] syncItemOffset           number of bytes of item data already sent, continue there and update it
         * @param maxBytes                          maximum amount of item data to send; always send at least one byte to ensure progress
         * @param[out] sentBytes                    number of bytes actually included in message
         */
        ArrayList* prepareItem(SyncItem* syncItem,
                               long &syncItemOffset, long maxBytes, long &sentBytes,
                               const char*  type, const char*  COMMAND);
        /*
         * @param syncItem                      
         * @param chunk                             the chunk item to be added
         * @param COMMAND                           REPLACE_COMMAND_NAME, ADD_COMMAND_NAME, DELETE_COMMAND_NAME        
         * @return item                             the Item object to be added in the list of command
         */
        Item* prepareItemChunk(SyncItem* syncItem,
                               Chunk* chunk,
                               const char* COMMAND);


        /*
        * Add the MapItem to the Map command.
        */
        void     addMapItem(Map* map, MapItem* mapItem);

        /*
        * Add a SyncItem into the modificationCommand. It is responsible to collapse if needed.
        * If the modificationCommand is NULL, then this is the first item and modificationCommand
        * is initialized.
        *
        * @param[in, out] modificationCommand      new items are added here, created if necessary
        * @param[in, out] syncItemOffset           number of bytes of item data already sent, continue there and update it
        * @param maxBytes                          maximum amount of item data to send; always send at least one byte to ensure progress
        * @param COMMAND                           REPLACE_COMMAND_NAME, ADD_COMMAND_NAME, DELETE_COMMAND_NAME
        * @param syncItem                          item to be added, NULL causes the call to return without doing anything
        * @param defaultType                       fallback if the syncItem does not define a type
        * @return number of bytes of item data included
        */
        long addItem(ModificationCommand* &modificationCommand,
                     long &syncItemOffset, long maxBytes,
                     const char*  COMMAND, SyncItem* syncItem,
                     const char*  defaultType);

        /*
        * Add a Chunk into the modificationCommand. 
        * If the modificationCommand is NULL, then this is the first item and modificationCommand
        * is initialized.
        *
        * @param[in, out] modificationCommand      new items are added here, created if necessary
        * @param COMMAND                           REPLACE_COMMAND_NAME, ADD_COMMAND_NAME, DELETE_COMMAND_NAME
        * @param syncItem                          
        * @param chunk                             chunk item to be added, NULL causes the call to return without doing anything
        * @param defaultType                       fallback if the syncItem does not define a type
        * @return number of bytes of item data included, that is the chunk size
        */
        long addChunk(ModificationCommand* &modificationCommand,                            
                            const char* COMMAND, SyncItem* syncItem, Chunk* chunk, 
                            const char* defaultType);
        /*
        * Reset the cmdID counter
        */
        void resetCommandID();

        /*
        * increase the msgID counter
        */
        void increaseMsgRef();

        /*
        * reset MsgId counter
        */

        void resetMessageID();

        /*
        * reset MsgRef counter
        */
        void resetMsgRef();

    private:

        void initialize();

        char*  target;
        char*  device;
        
        SyncMLProtocolFlavour protocol;
        
        /**
        * get the ComplexData given the chunk
        *
        * @param chunk   the chunk item 
        * @return the complex data
        */
        ComplexData* getComplexData(Chunk* chunk);

        unsigned long sessionID;
        unsigned int  msgID    ;
        unsigned int  cmdID    ;
        unsigned int  msgRef   ;

        ComplexData* getComplexData(SyncItem* syncItem, long &syncItemOffset, long maxBytes, long &sentBytes);

};


END_NAMESPACE

/** @endcond */
#endif
