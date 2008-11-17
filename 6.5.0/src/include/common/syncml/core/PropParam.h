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
#ifndef INCL_PROPPARAM
#define INCL_PROPPARAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class PropParam : public ArrayElement {

    // ------------------------------------------------------- Private interface
private:

    char*  paramName;
    char*  dataType;
    ArrayList* valEnums;
    char*  displayName;



    // ----------------------------------------------------- Protected interface
protected:




    // -------------------------------------------------------- Public interface
public:


    /*
     * PropParam constructor
     *
     */
    PropParam();

    ~PropParam();


    /*
     * PropParam constructor
     *
     * @param p0 prop name
     * @param p1 data type
     * @param p2 values enum
     * @param p3 display name
     */
    PropParam(char*  p0, char*  p1, ArrayList* p2, char*  p3);



    /*
     * getDisplayName
     *
     */
    const char* getDisplayName();


    /*
     * getParamName
     *
     */
    const char* getParamName();


    /*
     * setParamName
     *
     * @param p0
     */
    void setParamName(const char* p0);


    /*
     * getDataType
     *
     */
    const char* getDataType();


    /*
     * setDataType
     *
     * @param p0
     */
    void setDataType(const char* p0);


    /*
     * getValEnums
     *
     */
    ArrayList* getValEnums();


    /*
     * setValEnums
     *
     * @param p0
     */
    void setValEnums(ArrayList* p0);


    /*
     * setDisplayName
     *
     * @param p0
     */
    void setDisplayName(const char* p0);

    /*
     * Creates a clone of this instance
     *
     * @return the newly created instance
     */
    ArrayElement* clone();


};


/** @endcond */
#endif
