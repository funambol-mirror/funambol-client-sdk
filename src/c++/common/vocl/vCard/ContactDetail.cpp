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


#include "base/fscapi.h"
#include "vocl/vCard/ContactDetail.h"

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
