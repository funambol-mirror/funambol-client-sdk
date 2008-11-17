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


#ifndef INCL_MAP_ITEM
#define INCL_MAP_ITEM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Target.h"
#include "syncml/core/Source.h"

class MapItem : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        Target* target;
        Source* source;
        void initialize();
    public:

        /**
         * This is for serialization purposes
         */
        MapItem();
        ~MapItem();

        /**
         * Creates a MapItem object from its target and source.
         *
         *  @param target the mapping target - NOT NULL
         *  @param source the mapping source - NOT NULL
         *
         *
         */
        MapItem(Target* target, Source* source);

        /**
         * Returns the MapItem's target
         *
         * @return Tthe MapItem's target
         *
         */
        Target* getTarget();

        /**
         * Sets the MapItem's target
         *
         * @param target he MapItem's target - NOT NULL
         *
         */
        void setTarget(Target* target);

        /**
         * Returns the MapItem's source
         *
         * @return Tthe MapItem's source
         *
         */
        Source* getSource();

        /**
         * Sets the MapItem's source
         *
         * @param source he MapItem's source - NOT NULL
         *
         */
        void setSource(Source* source);

        ArrayElement* clone();

};

/** @endcond */
#endif
