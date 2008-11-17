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


#ifndef INCL_DEVINF_ITEM
#define INCL_DEVINF_ITEM
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/Item.h"
#include "syncml/core/Target.h"
#include "syncml/core/Source.h"
#include "syncml/core/Meta.h"
#include "syncml/core/DevInfData.h"


class DevInfItem : public Item {

     // ------------------------------------------------------------ Private data
    private:
        Target* target;
        Source* source;
        Meta* meta;
        DevInfData* data;

    public:

        DevInfItem();
        ~DevInfItem();

        /**
         * Creates a new DevInfItem object.
         *
         * @param target item target - NULL ALLOWED
         * @param source item source - NULL ALLOWED
         * @param meta item meta data - NULL ALLOWED
         * @param data item data - NULL ALLOWED
         *
         */
        DevInfItem(Target*     target,
                   Source*     source,
                   Meta*       meta  ,
                   DevInfData* data  );

        /**
         * Returns the item target
         *
         * @return the item target
         */
        Target* getTarget();

        /**
         * Sets the item target
         *
         * @param target the target
         *
         */
        void setTarget(Target* target);

        /**
         * Returns the item source
         *
         * @return the item source
         */
        Source* getSource();

        /**
         * Sets the item source
         *
         * @param source the source
         *
         */
        void setSource(Source* source);

        /**
         * Returns the item meta element
         *
         * @return the item meta element
         */
        Meta* getMeta();

        /**
         * Sets the meta item
         *
         * @param meta the item meta element
         *
         */
        void setMeta(Meta* meta);

        /**
         * Returns the item data
         *
         * @return the item data
         *
         */
        DevInfData* getDevInfData();

        /**
         * Sets the item data
         *
         * @param data the item data
         *
         */
        void setDevInfData(DevInfData* data);

        ArrayElement* clone();

};

/** @endcond */
#endif
