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


#ifndef INCL_MAP
#define INCL_MAP
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/ModificationCommand.h"

#define MAP_COMMAND_NAME "Map"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif
