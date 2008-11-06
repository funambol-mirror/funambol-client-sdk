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


#ifndef INCL_MANAGEMENT_OBJECT
#define INCL_MANAGEMENT_OBJECT
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "base/util/KeyValuePair.h"
#include "spdm/constants.h"
#include "spdm/LeafManagementNode.h"
#include "spds/SyncSourceConfig.h"

/**
 * This class represents a management object. Properties are stored in an
 * ArrayList of KeyValuePair objects.
 */
class ManagementObject : public LeafManagementNode {

    public:
        // ------------------------------------------ Constructors & destructors
        ManagementObject( const char*    context,
                          const char*    name   );

        ~ManagementObject();

        // ------------------------------------------------------------- Methods

        void getPropertyValue(const char*  property, char* v, int size);
        void setPropertyValue(const char*  property, const char* value);

        /**
         * Returns the ManagementObject's properties as an ArrayList of
         * KeyValuePairs
         */
        ArrayList& getProperties();

        ArrayElement* clone();

        // -------------------------------------------------------- Data members

    private:
        ArrayList properties;
};

/** @endcond */
#endif
