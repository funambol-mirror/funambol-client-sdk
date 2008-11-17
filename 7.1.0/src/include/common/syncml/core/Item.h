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


#ifndef INCL_ITEM
#define INCL_ITEM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Target.h"
#include "syncml/core/Source.h"
#include "syncml/core/Meta.h"
#include "syncml/core/ComplexData.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE



class Item : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        Target*      target;
        Source*      source;
        char*      targetParent;
        char*      sourceParent;
        Meta*        meta;
        ComplexData* data;
        bool         moreData;

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
             const char*  targetParent,
             const char*  sourceParent,
             Meta*   meta  ,
             ComplexData* data,
             bool moreData);

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
             bool moreData);

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
         * Gets the Boolean value of moreData
         *
         * @return true if the data item is incomplete and has further chunks
         *         to come, false otherwise
         */
        bool getMoreData();

        /**
         * Sets the moreData property
         *
         * @param moreData the moreData property
         */
        void setMoreData(bool moreData);

        ArrayElement* clone();

};


END_NAMESPACE

/** @endcond */
#endif
