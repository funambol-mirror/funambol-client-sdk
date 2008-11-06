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

#include "filter/FieldClause.h"

/*
 * FieldClause constructor
 *
 */
FieldClause::FieldClause() : properties(NULL) {
    type = FIELD_CLAUSE;
}


/*
 * FieldClause constructor
 *
 * @param p
 */
FieldClause::FieldClause(ArrayList* p) : properties(NULL) {
    type = FIELD_CLAUSE;
    setProperties(p);
}

/*
 * FieldClause constructor
 *
 * @param p
 */
FieldClause::FieldClause(ArrayList& p) : properties(NULL) {
    type = FIELD_CLAUSE;
    setProperties(p);
}


/*
 * FieldClause destructor
 *
 */
FieldClause::~FieldClause() {
    if (properties) {
        delete properties;
    }
}

/*
 * setProperty
 *
 * @param p0
 */
void FieldClause::setProperties(ArrayList* p) {
    if (properties) {
        delete properties; properties = NULL;
    }


    if (p) {
        properties = p->clone();
    }
}

/*
 * setProperty
 *
 * @param p0
 */
void FieldClause::setProperties(ArrayList& p) {
    setProperties(&p);
}


/*
 * getProperty
 *
 */
ArrayList* FieldClause::getProperties() {
    return properties;
}

ArrayElement* FieldClause::clone() {
    return (ArrayElement*)new FieldClause(properties);
}
