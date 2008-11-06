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


#include "spds/ItemReport.h"



//--------------------------------------------------- Constructor & Destructor
ItemReport::ItemReport() {
    status = 0;
    id  = NULL;
    statusMessage = NULL;
}
ItemReport::ItemReport(const WCHAR* luid, const int statusCode, const WCHAR* statusMess) {
    id  = NULL;
    statusMessage = NULL;
    setStatus(statusCode);
    setId(luid);
    setStatusMessage(statusMess);
}

ItemReport::ItemReport(ItemReport& ir) {
    status = 0;
    id  = NULL;
    statusMessage = NULL;
    assign(ir);
}

ItemReport::~ItemReport() {
    if (id) {
        delete [] id;
        id = NULL;
    }
    if (statusMessage) {
        delete [] statusMessage;
        statusMessage = NULL;
    }
}

//------------------------------------------------------------- Public Methods

const WCHAR* ItemReport::getId() const {
    return id;
}
void ItemReport::setId(const WCHAR* v) {
    if (id) {
        delete [] id;
        id = NULL;
    }

	id = wstrdup(v);
}

const int ItemReport::getStatus() const {
    return status;
}
void ItemReport::setStatus(const int v) {
    status = v;
}

const WCHAR* ItemReport::getStatusMessage() const {
    return statusMessage;
}
void ItemReport::setStatusMessage(const WCHAR* v) {
    if (statusMessage) {
        delete [] statusMessage;
        statusMessage = NULL;
    }

	statusMessage = wstrdup(v);
}

ArrayElement* ItemReport::clone() {
    ItemReport* it = new ItemReport(getId(), getStatus(), getStatusMessage());
    return it;
}

//------------------------------------------------------------- Private Methods
void ItemReport::assign(const ItemReport& ir) {
    setId    (ir.getId    ());
    setStatus(ir.getStatus());
    setStatusMessage(ir.getStatusMessage());
}
