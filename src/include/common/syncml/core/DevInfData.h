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


#ifndef INCL_DEVINF_DATA
#define INCL_DEVINF_DATA
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/DevInf.h"
#include "syncml/core/Data.h"

class DevInfData : public Data {

     // ------------------------------------------------------------ Private data
    private:
        DevInf* devInf;

    // ---------------------------------------------------------- Public data
    public:

        DevInfData();
        ~DevInfData();

        /**
         * Creates a new DevInfData object with the given parameter
         *
         * @param devInf the DevInf object - NOT NULL
         *
         */
        DevInfData(DevInf* devInf);

        /**
         * Gets the devInf object
         *
         * @return devInf the devInf object
         */
        DevInf* getDevInf();

        /**
         * Sets the DevInf object
         *
         * @param devInf the DevInf object
         *
         */
        void setDevInf(DevInf* devInf);

        DevInfData* clone();
};

/** @endcond */
#endif
