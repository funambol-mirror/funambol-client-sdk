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


#ifndef INCL_ITEM
#define INCL_ITEM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Target.h"
#include "syncml/core/Source.h"
#include "syncml/core/Meta.h"
#include "syncml/core/ComplexData.h"



class Item : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        Target*      target;
        Source*      source;
        char*      targetParent;
        char*      sourceParent;
        Meta*        meta;
        ComplexData* data;
        BOOL         moreData;

        void initialize();

    public:

        Item();
        ~Item();

        /**
         * Creates a new Item object.
         *
         * @param target item target - NULL ALLOWED
         * @param source item source - NULL ALLOWED
         * @param targetParent item target parent - NULL ALLOWED (DEFAULT)
         * @param sourceParent item source parent - NULL ALLOWED (DEFAULT)
         * @param meta item meta data - NULL ALLOWED
         * @param data item data - NULL ALLOWED
         *
         */
        Item(Target* target,
             Source* source,
             char*  targetParent,
             char*  sourceParent,
             Meta*   meta  ,
             ComplexData* data,
             BOOL moreData);

        /**
         * Creates a new Item object.
         *
         * @param target item target - NULL ALLOWED
         * @param source item source - NULL ALLOWED
         * @param meta item meta data - NULL ALLOWED
         * @param data item data - NULL ALLOWED
         *
         */
        Item(Target* target,
             Source* source,
             Meta*   meta  ,
             ComplexData* data,
             BOOL moreData);

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
         * Returns the item targetParent
         *
         * @return the item target parent
         */
        const char* getTargetParent();

        /**
         * Sets the item targetParent
         *
         * @param parent the target parent
         *
         */
        void setTargetParent(const char* parent);

        /**
         * Returns the item sourceParent
         *
         * @return the item source parent
         */
        const char* getSourceParent();

        /**
         * Sets the item sourceParent
         *
         * @param parent the source parent
         *
         */
        void setSourceParent(const char* parent);

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
        ComplexData* getData();

        /**
         * Sets the item data
         *
         * @param data the item data
         *
         */
        void setData(ComplexData* data);

        /**
         * Gets moreData property
         *
         * @return true if the data item is incomplete and has further chunks
         *         to come, false otherwise
         */
        BOOL isMoreData();

        /**
         * Gets the Boolean value of moreData
         *
         * @return true if the data item is incomplete and has further chunks
         *         to come, false otherwise
         */
        BOOL getMoreData();

        /**
         * Sets the moreData property
         *
         * @param moreData the moreData property
         */
        void setMoreData(BOOL moreData);

        ArrayElement* clone();

};

/** @endcond */
#endif
