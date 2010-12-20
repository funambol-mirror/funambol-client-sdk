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
#include "filter/WhereClause.h"
#include "base/globalsdef.h"

USE_NAMESPACE


WhereClause::WhereClause() : property(NULL), value(NULL), op(UNKNOWN), caseSensitive(true) {
    type = WHERE_CLAUSE;
}

WhereClause::WhereClause(const char* p, const char* v, WhereClauseOperator o, bool s) {
    type = WHERE_CLAUSE;
    property = NULL; if (p) property = stringdup(p);
    value = NULL; if (v) value = stringdup(v);
    op = o;
    caseSensitive = s;
}

WhereClause::~WhereClause() {
    if (property) {
        delete [] property;
    }
    if (value) {
        delete [] value;
    }
}

/*
 * Gets property
 *
 * @return  the current property's value
 *
 */
const char* WhereClause::getProperty() {
    return property;
}

/*
 * Sets property
 *
 * @param property the new value
 *
 */
void WhereClause::setProperty(const char*property) {
    if (this->property) {
        delete this->property; this->property = NULL;
    }

    if (property) {
        this->property = stringdup(property);
    }
}

/*
 * Gets value
 *
 * @return  the current value's value
 *
 */
const char* WhereClause::getValue() {
    return value;
}

/*
 * Sets value
 *
 * @param value the new value
 *
 */
void WhereClause::setValue(const char*value) {
    if (this->value) {
        delete this->value; this->value = NULL;
    }

    if (value) {
        this->value = stringdup(value);
    }
}

/*
 * Gets operator
 *
 * @return  the current operator's value
 *
 */
WhereClauseOperator WhereClause::getOperator() {
    return op;
}

/*
 * Sets operator
 *
 * @param operator the new value
 *
 */
void WhereClause::setOperator(WhereClauseOperator o) {
    op = o;
}

bool WhereClause::isCaseSensitive() {
    return (caseSensitive == true);
}

void WhereClause::setCaseSensitive(bool s) {
    caseSensitive = s;
}

ArrayElement* WhereClause::clone() {
    return (ArrayElement*)new WhereClause(property, value, op, caseSensitive);
}
