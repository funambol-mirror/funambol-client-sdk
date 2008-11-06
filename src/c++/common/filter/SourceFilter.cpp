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

#include "base/util/utils.h"
#include "filter/SourceFilter.h"


SourceFilter::SourceFilter() : clause(NULL), type(NULL), inclusive(FALSE) {
}

SourceFilter::~SourceFilter() {
    if (clause) delete clause;
}

void SourceFilter::setInclusive(BOOL i) {
    inclusive = i;
}

BOOL SourceFilter::isInclusive() {
    return (inclusive == TRUE);
}

BOOL SourceFilter::isExclusive() {
    return (inclusive == FALSE);
}

/*
 * Gets clause
 *
 * @return  the current clause's value
 *
 */
LogicalClause* SourceFilter::getClause() {
    return clause;
}

/*
 * Sets clause
 *
 * @param clause the new value
 *
 */
void SourceFilter::setClause(LogicalClause* clause) {
    if (this->clause) {
        delete this->clause; this->clause = NULL;
    }

    if (clause) {
        this->clause = (LogicalClause*)clause->clone();
    }
}

/*
 * Sets clause
 *
 * @param clause the new value
 *
 */
void SourceFilter::setClause(LogicalClause& clause) {
    if (this->clause) {
        delete this->clause;
    }

    this->clause = (LogicalClause*)clause.clone();
}

/**
 * Returns type
 *
 * @return type
 */
const char* SourceFilter::getType() {
    return type;
}

/**
 * Sets type
 *
 * @param type the new type value
 */
void SourceFilter::setType(const char*type) {
    if (this->type) {
        delete [] this->type; this->type = NULL;
    }

    if (type) {
        this->type = stringdup(type);
    }
}
