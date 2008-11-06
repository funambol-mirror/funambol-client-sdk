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



#ifndef INCL_MEM
#define INCL_MEM
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/Mem.h"

class Mem {

     // ------------------------------------------------------------ Private data
    private:
        BOOL    sharedMem;
        long    freeMem  ;
        long    freeID   ;
    public:

        ~Mem();

        /**
         * Creates a new Mem object from memory characteristics.
         *
         * @param sharedMem is the datastore memory shared
         * @param freeMem free memory size in bytes (>= 0)
         * @param freeID number of available item IDs (>= 0)
         *
         */
        Mem(BOOL sharedMem, long freeMem, long freeID);

         /**
         * Returns the memoryShared status
         *
         * @return <i>true</i> if the datastore memory is shared, <i>false</i> otherwise
         *
         */
        BOOL isSharedMem();

        /**
         * Sets the memoryShared status
         *
         * @param sharedMem the new memoryShared status
         */
        void setSharedMem(BOOL sharedMem);

        /**
         * Gets the Boolean shared memory property
         *
         * @return sharedMem the Boolean shared memory property
         */
        BOOL getSharedMem();

        /**
         * Returns the freeMem property (in bytes)
         *
         * @return the freeMem property
         *
         */
        long getFreeMem();

        /**
         * Sets the freeMem property.
         *
         * @param freeMem the freeMem value (>= 0)
         *
         */

        void setFreeMem(long freeMem);

        /**
         * Returns the number of available item IDs (>= 0)
         *
         * @return the number of available item IDs (>= 0)
         *
         */
        long getFreeID();

        /**
         * Sets the freeID property.
         *
         * @param freeID the freeIDCount value (>= 0)
         *
         */
        void setFreeID(long freeID);

        Mem* clone();

};

/** @endcond */
#endif
