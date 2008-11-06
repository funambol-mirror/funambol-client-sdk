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


#ifndef INCL_SEQUENCE
#define INCL_SEQUENCE
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"


#define SEQUENCE_COMMAND_NAME "Sequence"

class Sequence : public AbstractCommand {

    private:
        char*  COMMAND_NAME;
        ArrayList* commands;

    // ---------------------------------------------------------- Public data
    public:
        Sequence();
        ~Sequence();

        /**
         * Create a new Sequence object. The commands in <i>commands</i>
         * must be of the allowed types.
         *
         * @param cmdID command identifier - NOT NULL
         * @param noResp is &lt;NoREsponse/&gt; required?
         * @param meta meta information
         * @param commands the sequenced commands - NOT NULL
         *
         */
        Sequence(CmdID*       cmdID ,
                        BOOL         noResp,
                        Meta*        meta  ,
                        ArrayList*   commands);  // AbstractCommand[]

        /**
         * Gets an array of AbstractCommand
         *
         * @return an array of command objects
         */
        ArrayList* getCommands();

        /**
         * Sets the sequenced commands. The given commands must be of the allowed
         * types.
         *
         * @param commands the commands - NOT NULL and o the allawed types
         *
         */
        void setCommands(ArrayList* commands);

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

