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


#ifndef INCL_MAP
#define INCL_MAP
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/ModificationCommand.h"

#define MAP_COMMAND_NAME "Map"

class Map : public AbstractCommand {

     // ------------------------------------------------------------ Private data
    private:
        char*  COMMAND_NAME;
        Target*    target;
        Source*    source;
        ArrayList* mapItems; //MapItem[]
        void initialize();

    public:


        // ------------------------------------------------------------ Constructors

        Map();
        ~Map();

        /**
         * Creates a new Map commands from its constituent information.
         *
         * @param cmdID command identifier - NOT NULL
         * @param target the target - NOT NULL
         * @param source the source - NOT NULL
         * @param cred authentication credential - NULL ALLOWED
         * @param meta the associated meta data - NULL ALLOWED
         * @param mapItems the mapping items - NOT NULL
         *
         */
        Map(CmdID* cmdID,
            Target* target,
            Source* source,
            Cred* cred,
            Meta* meta,
            ArrayList* mapItems);

        /**
         * Returns the target property
         * @return the target property
         *
         */
        Target* getTarget();

        /**
         * Sets the target property
         *
         * @param target the target - NOT NULL
         *
         */
        void setTarget(Target* target);

        /**
         * Returns the source property
         * @return the source property
         *
         */
        Source* getSource();

        /**
         * Sets the source property
         *
         * @param source the source - NOT NULL
         *
         */
        void setSource(Source* source);

        /**
         * Returns the map items
         *
         * @return the map items
         *
         */
        ArrayList* getMapItems();

        /**
         * Sets the mapItems property
         *
         * @param mapItems the map items - NOT NULL
         *
         */
        void setMapItems(ArrayList* mapItems);

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
