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

#include "syncml/core/Filter.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Filter::Filter() : field(NULL), record(NULL), filterType(NULL), meta(NULL)  {
}

Filter::Filter(Meta*    m,
               Item*    f,
               Item*    r,
               char* t): field(NULL), record(NULL), filterType(NULL), meta(NULL) {
    setMeta(m);
    setField(f);
    setRecord(r);
    setFilterType(t);
}

Filter::~Filter() {
    delete meta;
    meta = NULL;
    delete field;
    field = NULL;
    delete record;
    record   = NULL;
    delete [] filterType;
    filterType   = NULL;
}

Meta* Filter::getMeta() {
    return meta;
}

void Filter::setMeta(Meta* m) {
    delete this->meta;
    this->meta = NULL;
    if (m) {
        this->meta = m->clone();
    }
}

Item* Filter::getField() {
    return field;
}

void Filter::setField(Item* f) {
    delete field;
    field = NULL;
    if (f) {
        field = (Item*)f->clone();
    }
}

Item* Filter::getRecord() {
    return record;
}

void Filter::setRecord(Item* r) {
    delete record;
    record = NULL;
    if (r) {
        record = (Item*)r->clone();
    }
}

const char* Filter::getFilterType() {
    return filterType;
}

void Filter::setFilterType(const char*t) {
    delete [] filterType;
    filterType = NULL;
    if (t) {
        filterType = stringdup(t);
    }
}

Filter* Filter::clone() {
    return new Filter(meta, field, record, filterType);
}

