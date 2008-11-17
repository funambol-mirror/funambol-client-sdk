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


#ifndef INCL_MODIFICATION_COMMAND
#define INCL_MODIFICATION_COMMAND
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"


class ModificationCommand : public ItemizedCommand {

     // ------------------------------------------------------------ Protected data
    protected:
       ModificationCommand();

    // ---------------------------------------------------------- Public data
    public:
        ~ModificationCommand();
         ModificationCommand(CmdID* cmdID, Meta* meta, ArrayList* items);

        /**
         * Creates a new ModificationCommand object.
         *
         * @param cmdID the command identifier
         * @param items the modification items
         */
        ModificationCommand(CmdID* cmdID, ArrayList* items);

        virtual ArrayElement* clone() = 0;


};

/** @endcond */
#endif
