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


#ifndef INCL_ITEMIZED_COMMAND
#define INCL_ITEMIZED_COMMAND
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/Meta.h"
#include "syncml/core/Item.h"
#include "syncml/core/CmdID.h"
#include "syncml/core/AbstractCommand.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class ItemizedCommand : public AbstractCommand {

     // ------------------------------------------------------------ Private data
    protected:
        ArrayList* items;  // Item[]
        Meta*      meta ;
        char*  correlator;

        void initialize();

    // ---------------------------------------------------------- Protected data
    public:

        ItemizedCommand();
        ~ItemizedCommand();
        /**
         * Create a new ItemizedCommand object with the given commandIdentifier,
         * meta object and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param meta the meta object
         * @param items an array of item - NOT NULL
         *
         */
        ItemizedCommand(CmdID* cmdID, Meta* meta, ArrayList* items);

        /**
         * Create a new ItemizedCommand object with the given commandIdentifier
         * and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param items an array of item - NOT NULL
         *
         */
        ItemizedCommand(CmdID*  cmdID, ArrayList* items);

        /**
         * Gets the correlator value
         *
         * @return the correlator value
         */
        const char * getCorrelator();

        /**
         * Sets the correlator value
         *
         * @param correlator the correlator value
         */
        void setCorrelator(const char *correlator);
	
        /**
         * Gets the array of items
         *
         * @return the array of items
         */
        ArrayList* getItems();

        /**
         * Sets an array of Item object. If the array of Item object is null this method
         * creates a new arraylist.
         * 
         * @param items an array of Item object.
         */
        void setItems(ArrayList* items);

        /**
         * Gets the Meta object
         *
         * @return the Meta object
         */
        Meta* getMeta();

        /**
         * Sets the Meta object
         *
         * @param meta the Meta object
         *
         */
        void setMeta(Meta* meta);

        /**
         * Gets the name of the command
         *
         * @return the name of the command
         */
        virtual const char* getName() = 0;

        virtual ArrayElement* clone() = 0;
};


END_NAMESPACE

/** @endcond */
#endif
