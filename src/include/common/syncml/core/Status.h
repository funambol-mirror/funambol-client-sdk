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


#ifndef INCL_STATUS
#define INCL_STATUS
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ResponseCommand.h"
#include "syncml/core/Chal.h"
#include "syncml/core/Data.h"

#define STATUS_COMMAND_NAME "Status"

class Status : public ResponseCommand {

    private:
        char*   COMMAND_NAME;
        Chal*     chal;
        Data*     data;
        char*   cmd;

        void initialize();

    // ---------------------------------------------------------- Public data
    public:

        /**
         * For serialiazaion purposes
         */
        Status();
        ~Status();

        /**
         * Creates a new Status object.
         *
         * @param cmdID command identifier - NOT NULL
         * @param msgRef message reference
         * @param cmdRef command reference - NOT NULL
         * @param cmd command - NOT NULL
         * @param targetRefs target references. If null
         * @param sourceRefs source references. If null
         * @param cred authentication credentials
         * @param chal authentication challenge
         * @param data status data - NOT NULL
         * @param items command items - NOT NULL
         *
         *
         */
        Status(  CmdID*        cmdID     ,
                 const char*   msgRef    ,
                 const char*   cmdRef    ,
                 const char*   cmd       ,
                 ArrayList*    targetRefs,
                 ArrayList*    sourceRefs,
                 Cred*         cred      ,
                 Chal*         chal      ,
                 Data*         data      ,
                 ArrayList*    items     );


        /**
         * Returns the chal element
         *
         * @return the chal element
         *
         */
        Chal* getChal();

        /**
         * Sets the chal element
         *
         * @param chal the new chal
         */
        void setChal(Chal* chal);

        /**
         * Returns the status data
         *
         * @return the status data
         *
         */
        Data* getData();

        /**
         * Sets the status data
         *
         * @param data the new data
         *
         */
        void setData(Data* data);

        /**
         * Returns the cmd element
         *
         * @return the cmd element
         */
        const char* getCmd();

        /**
         * Sets the cmd element
         *
         * @param cmd the new cmd element - NOT NULL
         *
         */
        void setCmd(const char*  cmd);

        /**
         * Returns the status code as int
         *
         * @return the status code as int
         */
        int getStatusCode();

        /**
         * Returns the command name
         *
         * @return the command name
         */
        const char* getName();

        ArrayElement* clone();
};

/** @endcond */
#endif
