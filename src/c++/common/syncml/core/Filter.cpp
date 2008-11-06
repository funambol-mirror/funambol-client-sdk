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

#include "syncml/core/Filter.h"

Filter::Filter() : meta(NULL), field(NULL), record(NULL), filterType(NULL) {
}

Filter::Filter(Meta*    m,
               Item*    f,
               Item*    r,
               char* t): meta(NULL), field(NULL), record(NULL), filterType(NULL) {
    setMeta(m);
    setField(f);
    setRecord(r);
    setFilterType(t);
}

Filter::~Filter() {
    if (meta)       { delete meta      ; meta = NULL;         }
    if (field)      { delete field     ; field = NULL;        }
    if (record  )   { delete record    ; record   = NULL;     }
    if (filterType) { delete filterType; filterType   = NULL; }
}

Meta* Filter::getMeta() {
    return meta;
}

void Filter::setMeta(Meta* m) {
    if (this->meta) {
		delete this->meta; this->meta = NULL;
    }

    this->meta = m->clone();
}

Item* Filter::getField() {
    return field;
}

void Filter::setField(Item* f) {
    if (field) {
		delete field; field = NULL;
    }
    if (f) {
	    field = (Item*)f->clone();
    }
}

Item* Filter::getRecord() {
    return record;
}

void Filter::setRecord(Item* r) {
    if (record) {
		delete record; record = NULL;
    }
    if (r) {
	    record = (Item*)r->clone();
    }
}

const char* Filter::getFilterType() {
    return filterType;
}

void Filter::setFilterType(const char*t) {
    if (filterType) {
        delete [] filterType; filterType = NULL;
    }

    if (t) {
        filterType = stringdup(t);
    }
}

Filter* Filter::clone() {
    return new Filter(meta, field, record, filterType);
}
