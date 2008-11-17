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

 #ifndef INCL_SYNC_MAP
    #define INCL_SYNC_MAP
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/util/ArrayElement.h"


class SyncMap : public ArrayElement {

    private:
        char*  luid;
        char*  guid;

    public:
        SyncMap(const char*  guid = NULL, const char*  luid = NULL);
        ~SyncMap();

        /*
         * Returns the guid of this mapping.
         */
        const char*  getGUID();


        /*
         * Returns the luid of this mapping.
         */
        const char*  getLUID();

        /**
         * Sets a new value for the LUID property. The value is copied in the
         * class internal memory.
         *
         * @param luid the new value
         */
        void setLUID(const char*  luid);

        /**
         * Sets a new value for the LUID property (as unsigned int). It internally
         * calls setLUID(const char* )
         *
         * @param luid the new value
         */
        void setLUID(unsigned long luid);


        /**
         * Sets a new value for the GUID property. The value is copied in the
         * class internal memory.
         *
         * @param guid the new value
         */
        void setGUID(const char*  guid);

        /**
         * Creates a new instance of SyncMap from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone();
    };

/** @endcond */
#endif
