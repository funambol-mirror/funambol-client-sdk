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

#ifndef INCL_FIELDCLAUSE
#define INCL_FIELDCLAUSE
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "filter/Clause.h"


class FieldClause : public Clause {

    // ------------------------------------------------------- Private interface
private:

    ArrayList* properties;




    // ----------------------------------------------------- Protected interface
protected:




    // -------------------------------------------------------- Public interface
public:


    /*
     * FieldClause constructor
     *
     */
    FieldClause();


    /*
     * FieldClause constructor
     *
     * @param p0
     */
    FieldClause(ArrayList* p);

    /*
     * FieldClause constructor
     *
     * @param p0
     */
    FieldClause(ArrayList& p);


    /*
     * FieldClause destructor
     *
     */
    ~FieldClause();

    /*
     * setProperty
     *
     * @param p0
     */
    void setProperties(ArrayList* p);

    /*
     * setProperty
     *
     * @param p0
     */
    void setProperties(ArrayList& p);


    /*
     * getProperty
     *
     */
    ArrayList* getProperties();

    /*
     * Creates a new instance of this Clause
     *
     * @return the clone
     */
    ArrayElement* clone();

};


/** @endcond */
#endif
