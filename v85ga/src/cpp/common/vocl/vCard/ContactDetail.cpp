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


#include "base/fscapi.h"
#include "vocl/vCard/ContactDetail.h"
#include "base/globalsdef.h"

USE_NAMESPACE

ContactDetail::ContactDetail() {
    phones   = NULL;
    emails   = NULL;
    webPages = NULL;
}

ContactDetail::~ContactDetail() {
    if (phones) {
        delete phones; phones = NULL;
    }
    if (emails) {
        delete emails; emails = NULL;
    }
    if (webPages) {
        delete webPages; webPages = NULL;
    }
}

ArrayList* ContactDetail::getPhones() {
    return phones;
}

void ContactDetail::setPhones(ArrayList& list) {
    if (phones) {
        phones->clear();
    } else {
        phones = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        phones->add(*list[i]);
    }
}

ArrayList* ContactDetail::getEmails() {
    return emails;
}

void ContactDetail::setEmails(ArrayList& list) {
    if (emails) {
        emails->clear();
    } else {
        emails = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        emails->add(*list[i]);
    }
}

ArrayList* ContactDetail::getWebPages() {
    return webPages;
}

void ContactDetail::setWebPages(ArrayList& list) {
    if (webPages) {
        webPages->clear();
    } else {
        webPages = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        webPages->add(*list[i]);
    }
}

ContactDetail* ContactDetail::clone() {
    ContactDetail* ret = new ContactDetail();

    if (phones) {
        ret->setPhones(*phones);
    }
    if (emails) {
        ret->setEmails(*emails);
    }
    if (webPages) {
        ret->setWebPages(*webPages);
    }

    return ret;
}
