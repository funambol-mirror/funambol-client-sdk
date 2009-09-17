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


#include "spds/ItemReport.h"
#include "base/globalsdef.h"

USE_NAMESPACE



//--------------------------------------------------- Constructor & Destructor
ItemReport::ItemReport() {
    status = 0;
    luid  = NULL;
    statusMessage = NULL;
}
ItemReport::ItemReport(const WCHAR* luid, const int statusCode, const WCHAR* statusMess) {
    this->luid = NULL;
    this->statusMessage = NULL;
    setStatus(statusCode);
    setId(luid);
    setStatusMessage(statusMess);
}

ItemReport::ItemReport(ItemReport& ir) {
    status = 0;
    luid  = NULL;
    statusMessage = NULL;
    assign(ir);
}

ItemReport::~ItemReport() {
    if (luid) {
        delete [] luid;
        luid = NULL;
    }
    if (statusMessage) {
        delete [] statusMessage;
        statusMessage = NULL;
    }
}

//------------------------------------------------------------- Public Methods

const WCHAR* ItemReport::getId() const {
    return luid;
}
void ItemReport::setId(const WCHAR* v) {
    if (luid) {
        delete [] luid;
        luid = NULL;
    }

	luid = wstrdup(v);
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
