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

#ifndef INCL_WHERECLAUSE
#define INCL_WHERECLAUSE
/** @cond DEV */

#include "base/fscapi.h"
#include "filter/Clause.h"

typedef enum {
    EQ       =  0,
    NE       =  1,
    GT       =  2,
    LT       =  3,
    GE       =  4,
    LE       =  5,
    CONTAIN  =  6,
    NCONTAIN =  7,
    UNKNOWN  = -1
} WhereClauseOperator;

class WhereClause : public Clause {

    // ------------------------------------------------------- Private interface
private:

    char*             property     ;
    char*             value        ;
    WhereClauseOperator op           ;
    BOOL                caseSensitive;

    // ----------------------------------------------------- Protected interface
protected:

    // -------------------------------------------------------- Public interface
public:

    /*
     * WhereClause constructor
     *
     */
    WhereClause();


    /*
     * WhereClause constructor
     *
     * @param property
     * @param value
     * @param o
     * @param s
     */
    WhereClause(const char*  property, const char*  value, WhereClauseOperator o, BOOL p3);


    /*
     * WhereClause destructor
     *
     */
    ~WhereClause();

    /*
     * setProperty
     *
     * @param p0
     */
    void setProperty(const char* p);


    /*
     * getProperty
     *
     */
    const char* getProperty();



    /*
     * setValue
     *
     * @param p0
     */
    void setValue(const char* v);


    /*
     * getvalue
     *
     */
    const char* getValue();


    /*
     * getOperator
     *
     */
    WhereClauseOperator getOperator();


    /*
     * setOperator
     *
     * @param o
     */
    void setOperator(WhereClauseOperator o);


    /*
     * isCaseSensitive
     *
     */
    BOOL isCaseSensitive();


    /*
     * setCaseSensitive
     *
     * @param s
     */
    void setCaseSensitive(BOOL s);

    /*
     * Creates a new instance of this Clause
     *
     * @return the clone
     */
    ArrayElement* clone();


};


/** @endcond */
#endif
