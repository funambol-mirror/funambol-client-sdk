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


#ifndef INCL_DSMEM
#define INCL_DSMEM
/** @cond DEV */

#include "base/fscapi.h"


class DSMem {

     // ------------------------------------------------------------ Private data
    private:

        BOOL    sharedMem;
        long    maxMem   ;
        long    maxID    ;
    // ---------------------------------------------------------- Protected data
    public:

        DSMem();
        ~DSMem();

        /**
         * Creates a new DSMem object with the given sharedMem, maxMem and maxID
         *
         * @param sharedMem is true if the datastore uses shared memory
         * @param maxMem the maximum memory size for o given datastore
         * @param maxID the maximum number of items that can be stored in a given
         *              datastore
         *
         */
        DSMem(BOOL sharedMem, long maxMem, long maxID);

        /**
         * Returns the memoryShared status
         *
         * @return <i>true</i> if the datastore memory is shared, <i>false</i> otherwise
         */
        BOOL isSharedMem();

        /**
         * Sets the memoryShared status
         *
         * @param sharedMem the new memoryShared status
         */
        void setSharedMem(BOOL sharedMem);

        /**
         * Gets Boolean shared memory
         *
         * @return sharedMem the Boolean sharedMem
         */
        BOOL getSharedMem();

        /**
         * Gets the maximum memory size in bytes
         *
         * @return if value is -1 indicates that the property value is unspecified
         */
        long getMaxMem();

        /**
         * Sets the max memory property
         *
         * @param maxMem the value of max memory property
         *
         */
        void setMaxMem(long maxMem);

        /**
         * Gets the maximum number of items
         *
         * @return if value is -1 indicates that the property value is unspecified
         */
        long getMaxID();

        /**
         * Sets the max ID property
         *
         * @param maxID the value of maxID property
         */
        void setMaxID(long maxID);

        DSMem* clone();
};

/** @endcond */
#endif
