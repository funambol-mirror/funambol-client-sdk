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


#ifndef INCL_SYNC_CAP
#define INCL_SYNC_CAP
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/SyncTypeArray.h"

class SyncCap {

     // ------------------------------------------------------------ Private data
    private:
       ArrayList* syncType;

    // ---------------------------------------------------------- Public data
    public:

        SyncCap();
        ~SyncCap();

        /**
         * Creates a new SyncCap object that specifies the synchronization
         * capabilities of the given datastore
         *
         * @param syncTypes an array of type of the supported
         *                  synchronization - NOT NULL
         *
         */
        SyncCap(ArrayList* syncTypes);

        /**
         *
         * @return The return value is guaranteed to be non-null.
         *      Also, the array's elements are guaranteed to
         *      be non-null.
         *
         */
        ArrayList* getSyncType();

        void setSyncType(ArrayList* syncTypes);

        SyncCap* clone();

};

/** @endcond */
#endif
