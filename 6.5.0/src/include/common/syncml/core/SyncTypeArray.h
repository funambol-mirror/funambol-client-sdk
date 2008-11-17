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


#ifndef INCL_SYNC_TYPE_ARRAY
#define INCL_SYNC_TYPE_ARRAY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/SyncType.h"


/**
* Class that is used to store an array of SyncType Object.
* The constructor create the SyncType object that are used to fill the syncTypeArray.
* The getSyncTypeArray is used to get the ArrayList.
* There isn't the set method because the array list is filled only with the constructor
*/

class SyncTypeArray {

     // ------------------------------------------------------------ Private data
    private:
       ArrayList* syncTypeArray;

    // ---------------------------------------------------------- Public data
    public:

        SyncTypeArray();
        ~SyncTypeArray();

        /*
        * Returns the ArrayList that contains the SyncType object
        */
        ArrayList* getSyncTypeArray();

};

/** @endcond */
#endif
