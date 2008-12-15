/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "base/util/utils.h"
#include "filter/SourceFilter.h"
#include "base/globalsdef.h"

USE_NAMESPACE


SourceFilter::SourceFilter() : clause(NULL), inclusive(false), type(NULL) {
}

SourceFilter::~SourceFilter() {
    delete clause;
    delete [] type;
}

void SourceFilter::setInclusive(bool i) {
    inclusive = i;
}

bool SourceFilter::isInclusive() {
    return (inclusive == true);
}

bool SourceFilter::isExclusive() {
    return (inclusive == false);
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
    delete [] this->type;

    if (type) {
        this->type = stringdup(type);
    } else {
        this->type = NULL;
    }
}
