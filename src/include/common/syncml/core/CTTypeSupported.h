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


#ifndef INCL_CTTYPE_SUPPORTED
#define INCL_CTTYPE_SUPPORTED
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/util/ArrayElement.h"


class CTTypeSupported : public ArrayElement{

     // ------------------------------------------------------------ Private data
    private:
    char*  ctType;
    ArrayList* ctPropParams;    // CTPropParam[]

    void initialize();

    public:

    CTTypeSupported();
    ~CTTypeSupported();

    /**
     * Creates a new CTTypeSupported object with the given information
     *
     * @param ctType an String CTType - NOT NULL
     * @param ctPropParams the array of content type properties and/or content
     *                     content type parameters - NOT NULL
     *
     */
    CTTypeSupported(char*  ctType, ArrayList* ctPropParams );


    /**
     * Get a CTType String
     *
     * @return a CTType String
     */
    const char* getCTType();

    /**
     * Sets a CTType object
     *
     * @param ctType a CTType object
     */
    void setCTType(const char* ctType);

    /**
     * Gets an array of content type properties and parameters
     *
     * @return an array of content type properties and parameters
     *
     */
    ArrayList* getCTPropParams();

    /**
     * Sets an array of content type properties and parameters
     *
     * @param ctPropParams array of content type properties and parameters
     *
     */
    void setCTPropParams(ArrayList* ctPropParams);

    ArrayElement* clone();

};

/** @endcond */
#endif
