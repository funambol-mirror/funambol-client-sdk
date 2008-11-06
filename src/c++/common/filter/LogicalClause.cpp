/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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


#include "filter/LogicalClause.h"


LogicalClause::LogicalClause() {
    type = LOGICAL_CLAUSE;
    operands = new ArrayList();
}

LogicalClause::~LogicalClause() {
    if (operands) {
        delete operands;
    }
}

LogicalClause::LogicalClause(LogicalClauseOperator op, ArrayList& ops) {
    this->type     = LOGICAL_CLAUSE;
    this->op       = op            ;
    this->operands = ops.clone()   ;
}

LogicalClauseOperator LogicalClause::getOperator() {
    return op;
}

void LogicalClause::setOperator(LogicalClauseOperator o) {
    op = o;
}

ArrayList* LogicalClause::getOperands() {
    return operands;
}

void LogicalClause::setOperands(ArrayList& ops) {
    if (operands) {
        delete operands; operands = NULL;
    }

    operands = ops.clone();
}

BOOL LogicalClause::isUnaryOperator() {
    return (op == NOT);
}

ArrayElement* LogicalClause::clone() {
    return (ArrayElement*)new LogicalClause(op, *operands);
}
