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


#ifndef INCL_ATOMIC
#define INCL_ATOMIC
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/AbstractCommand.h"


#define ATOMIC_COMMAND_NAME "Atomic"

class Atomic : public AbstractCommand {
     // ------------------------------------------------------------ Private data
    private:
       char*  COMMAND_NAME;
       ArrayList* commands;

    // ---------------------------------------------------------- Public data
    public:

    Atomic();
    ~Atomic();

    /**
     * Creates a new Atomic object with the given command identifier, noResponse,
     * meta and an array of abstract command
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResp is true if no response is required
     * @param meta the meta data
     * @param commands an array of abstract command - NOT NULL
     */
    Atomic(CmdID* cmdID,
           BOOL noResp,
           Meta* meta,
           ArrayList* commands); // AbstractCommand[]

    /**
     * Gets an array of AbstractCommand
     *
     * @return an array of command objects
     */
    ArrayList* getCommands();

    /**
     * Sets an array of AbstractCommand
     *
     * @param commands the array of AbstractCommand
     *
     */
    void setCommands(ArrayList* commands);
    /**
     * Gets the command name property
     *
     * @return the command name property
     */
    const char* getName();

    ArrayElement* clone();

};

/** @endcond */
#endif
