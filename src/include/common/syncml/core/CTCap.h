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


#ifndef INCL_CTCAP
#define INCL_CTCAP
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/CTTypeSupported.h"


class CTCap : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        ArrayList* ctTypeSupported;  // CTTypeSupported[]

    // ---------------------------------------------------------- Public data
    public:

        CTCap();
        ~CTCap();

        /**
         * Creates a new CTCap object with the given array of information
         *
         * @param ctTypeSupported the array of information on content type
         *                        capabilities - NOT NULL
         *
         */
        CTCap(ArrayList* ctTypeSupported);


        /**
         * Get an array of content type information objects
         *
         * @return an array of content type information objects
         */
        ArrayList* getCTTypeSupported();

        /**
         * Sets an array of content type information objects
         *
         * @param ctTypeSupported an array of content type information objects
         */
        void setCTTypeSupported(ArrayList* ctTypeSupported);

        ArrayElement* clone();

};

/** @endcond */
#endif
