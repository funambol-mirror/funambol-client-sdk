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


#ifndef INCL_EXEC
#define INCL_EXEC
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ModificationCommand.h"

#define EXEC_COMMAND_NAME "Exec"

class Exec : public ModificationCommand {

     // ------------------------------------------------------------ Private data
    private:
        char*  COMMAND_NAME;


    // ---------------------------------------------------------- Public data
    public:

        Exec();
        ~Exec();

        /**
         * Creates a new Exec object with the given command identifier,
         * noResponse, credential and item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp true if no response is required
         * @param cred the authentication credential
         * @param item the item - NOT NULL
         *
         */
        Exec(CmdID* cmdID,
             BOOL   noResp,
             Cred*  cred,
             ArrayList*  items);

        // ---------------------------------------------------------- Public methods
        /**
         * Gets the command name property
         *
         * @return the command name property
         */
        const char* getName();

        /**
         * Gets an Item object
         *
         * @return an Item object
         */
        Item* getItem();

        ArrayElement* clone();

};

/** @endcond */
#endif
