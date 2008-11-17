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

 #ifndef INCL_SYNC_ITEM_STATUS
    #define INCL_SYNC_ITEM_STATUS
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "base/util/ArrayElement.h"
    #include "spds/constants.h"


    class SyncItemStatus : public ArrayElement {

    private:

        int      cmdID ;
        int      msgRef;
        int      cmdRef;
        char*  cmd   ;
        char*  key   ;
        int      data  ;

    public:
        /*
         * Default constructor
         */
        SyncItemStatus();

        /*
         * Constructs a new SyncItemStatus identified by the given key. The key must
         * not be longer than DIM_KEY_SYNC_ITEM_STATUS (see SPDS Constants).
         *
         * @param key - the key
         */
        SyncItemStatus(char*  key);

        ~SyncItemStatus();

        /*
         * Returns the SyncItemStatus's key. If key is NULL, the internal buffer is
         * returned; if key is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         */
        const char* getKey();

        /*
         * Changes the SyncItemStatus key. The key must not be longer than DIM_KEY_SYNC_ITEM_STATUS
         * (see SPDS Constants).
         *
         * @param key - the key
         */
        void setKey(const char* key);

         /*
         * Returns the SyncItemStatus's command name. If cmd is NULL, the internal buffer is
         * returned; if cmd is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         */
        const char* getCmd();

        /*
         * Changes the SyncItemStatus cmd. The cmd must not be longer than DIM_COMMAND_SYNC_ITEM_STATUS
         * (see SPDS Constants).
         *
         * @param cmd - the cmd
         */
        void setCmd(const char* cmd);


        /*
         * Sets the SyncItemStatus data. The passed data are copied into an
         * internal variable.
         */
        void setData(int data);

        /*
         * Returns the SyncItemStatus data variable.
         */
        int getData();


        /*
         * Sets the SyncItemStatus command ID. The passed data are copied into an
         * internal variable.
         */
        void setCmdID(int cmdId);

        /*
         * Returns the SyncItemStatus command ID variable.
         */
        int getCmdID();

        /*
         * Sets the SyncItemStatus message referring. The passed data are copied into an
         * internal variable.
         */
        void setMsgRef(int msgRef);

        /*
         * Returns the SyncItemStatus message referring variable.
         */
        int getMsgRef();

        /*
         * Sets the SyncItemStatus command referring. The passed data are copied into an
         * internal variable.
         */
        void setCmdRef(int cmdRef);

        /*
         * Returns the SyncItemStatus command referring variable.
         */
        int getCmdRef();

        /**
         * Creates a new instance of SyncItemStatus from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone();

    };

/** @endcond */
#endif
