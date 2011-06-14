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



#ifndef INCL_FILTER
#define INCL_FILTER
/** @cond DEV */

#include "syncml/core/Item.h"
#include "syncml/core/Meta.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class represents a SyncML 1.2 Filter element.
 *
 */

class Filter {
    // ------------------------------------------------------------ Private data
    private:
       Item*   field      ;
       Item*   record     ;
       char*  filterType;
       Meta*    meta;

       void initialize();

    // ------------------------------------------------------------- Public data
    public:

        /**
         * Creates a new Filter object.
         */
        Filter();

        /**
         * Creates a new Filter object.
         *
         * @param meta       the meta information - NOT NULL
         * @param field      the field item data
         * @param record     the record item data
         * @param filterType the type of filtering
         */
        Filter(Meta*    meta      ,
               Item*    field     ,
               Item*    record    ,
               const char*  filterType);

        ~Filter();

        /**
         * Returns the filter meta element
         *
         * @return the filter meta element
         */
        Meta* getMeta();

        /**
         * Sets the filter meta
         *
         * @param the filter meta element
         *
         */
        void setMeta(Meta* meta);

        /**
         * Returns the filter field element
         *
         * @return the filter field element
         */
        Item* getField();

        /**
         * Sets the filter field
         *
         * @param the filter field element
         *
         */
        void setField(Item* meta);

        /**
         * Returns the filter record element
         *
         * @return the filter record element
         */
        Item* getRecord();

        /**
         * Sets the filter record
         *
         * @param the filter record element
         *
         */
        void setRecord(Item* meta);

        /**
         * Returns the filter type
         *
         * @return the filter type
         */
        const char* getFilterType();

        /**
         * Sets the filter type
         *
         * @param the filter type
         *
         */
        void setFilterType(const char* type);

        /**
         * Creates a clone of this Filter
         *
         * @return the newly created instance
         */
        Filter* clone();
};


END_NAMESPACE

/** @endcond */
#endif
