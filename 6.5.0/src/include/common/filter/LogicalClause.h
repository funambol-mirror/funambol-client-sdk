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
#ifndef INCL_LOGICALCLAUSE
#define INCL_LOGICALCLAUSE
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "filter/Clause.h"

typedef enum {
    NOT = 0,
    AND = 1,
    OR  = 2
} LogicalClauseOperator;

class LogicalClause : public Clause {

    // ------------------------------------------------------- Private interface
private:

    LogicalClauseOperator op;
    ArrayList* operands;



    // ----------------------------------------------------- Protected interface
protected:
    /*
     * LogicalClause constructor
     *
     */
    LogicalClause();



    // -------------------------------------------------------- Public interface
public:

    ~LogicalClause();


    /*
     * LogicalClause constructor
     *
     * @param o the logical operator
     * @param ops the operands ArrayList
     * @param n how many operands are passed in ops
     */
    LogicalClause(LogicalClauseOperator o, ArrayList& ops);

    /*
     * getOperator
     *
     */
    LogicalClauseOperator getOperator();


    /*
     * setOperator
     *
     * @param p0
     */
    void setOperator(LogicalClauseOperator p0);


    /*
     * getOperands
     *
     */
    ArrayList* getOperands();


    /*
     * setOperands
     *
     * @param ops the operands ArrayList
    */
    void setOperands(ArrayList& ops);


    /*
     * isUnaryOperator
     *
     */
    BOOL isUnaryOperator();

    /*
     * Creates a new instance of this Clause
     *
     * @return the clone
     */
    ArrayElement* clone();


};


/** @endcond */
#endif
