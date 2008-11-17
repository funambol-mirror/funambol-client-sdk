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
#include "syncml/core/TargetRef.h"


TargetRef::TargetRef() {
    initialize();
}

TargetRef::~TargetRef() {

    if (value) {
        delete [] value; value = NULL;
    }
    if (query) {
        delete [] query; query = NULL;
    }
    if (target) {
        delete target; target = NULL;
    }
}


/**
 * Creates a new TargetRef object given the referenced value. A null value
 * is considered an empty string
 *
 * @param value the referenced value - NULL ALLOWED
 *
 */
TargetRef::TargetRef(const char* value) {
    initialize();
    setValue(value);
}

/**
 * Creates a new TargetRef object from an existing target.
 *
 * @param target the target to extract the reference from - NOT NULL
 *
 *
 */
TargetRef::TargetRef(Target* target) {
    initialize();
    setTarget(target);
    setValue(target->getLocURI());
}

void TargetRef::initialize() {
    value  = NULL;
    query  = NULL;
    target = NULL;

}

void TargetRef::setQuery(const char*val) {
    if (query) {
        delete [] query; query = NULL;
    }
    if (val) {
        query = stringdup(val);
    }
}

// ---------------------------------------------------------- Public methods

/**
 * Returns the value
 *
 * @return the value
 */
const char* TargetRef::getValue() {
        return value;
    }

/**
 * Sets the reference value. If value is null, the empty string is adopted.
 *
 * @param value the reference value - NULL
 */
 void TargetRef::setValue(const char* valuer) {
    if (valuer == NULL) {
        this->value = stringdup("");
        this->query = stringdup("");
    } else {
        unsigned int qMark = strlen(valuer);
        char* value = stringdup(valuer);
        char* p1 = value;
        BOOL charFound = FALSE;
        for (unsigned int k = 0; k < qMark; k++) {
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == '?') {
                charFound = TRUE;
                p1 = p1 + 1;
                break;
            }
            p1 = p1 + 1;
        }

        if (charFound == FALSE) {
            if (this->value) {
                delete [] this->value; this->value = NULL;
            }
            this->value = stringdup(value);

            if (this->query) {
                delete [] this->query; this->query = NULL;
            }
            this->query = stringdup("");
        } else {
            char* p2 = p1 - 1;
            *p2 = 0;
            if (this->value) {
                delete [] this->value; this->value = NULL;
            }
            this->value = stringdup(value);

            if (this->query) {
                delete [] this->query; this->query = NULL;
            }
            this->query = stringdup(p1);

        }
        if (value) {
            delete [] value;
        }
    }
}

/**
 * Gets the Target property
 *
 * @return target the Target property
 */
Target* TargetRef::getTarget() {
    return this->target;
}

/**
 * Sets the Target property
 *
 * @param target the Target property
 */
void TargetRef::setTarget(Target* target) {
    if (target == NULL) {
        // TBD
    }
    if (this->target) {
         delete this->target; this->target = NULL;
    }
    this->target = target->clone();
}

ArrayElement* TargetRef::clone() {
    TargetRef* ret = NULL;
    if (value) {
        ret = new TargetRef(value);
        ret->setQuery(query);
        if (target)
            ret->setTarget(target);
    }
    else if (target) {
        ret = new TargetRef(target);
    }
    return ret;
}
