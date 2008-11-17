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
#include "base/util/StringBuffer.h"
#include "vocl/vCard/BusinessDetail.h"
#include "vocl/vCard/Title.h"


BusinessDetail::BusinessDetail() {
    role          = NULL;
    titles        = NULL;
    address       = NULL;
    company       = NULL;
    department    = NULL;
    contactDetail = NULL;
    logo          = NULL;
    manager       = NULL;
    assistant     = NULL;
}

BusinessDetail::~BusinessDetail() {
    if (role) {
        delete role; role = NULL;
    }
    if (titles) {
        delete titles; titles = NULL;
    }
    if (address) {
        delete address; address = NULL;
    }
    if (company) {
        delete company; company = NULL;
    }
    if (department) {
        delete department; department = NULL;
    }
    if (contactDetail) {
        delete contactDetail; contactDetail = NULL;
    }
    if (logo) {
        delete logo; logo = NULL;
    }
}

vCardProperty* BusinessDetail::getRole() {
    return role;
}

void BusinessDetail::setRole(vCardProperty& p) {
    set(&role, p);
}

ArrayList* BusinessDetail::getTitles() {
    return titles;
}

void BusinessDetail::setTitles(WCHAR* titleArray[], int n) {
    if (titles) {
        titles->clear();
    } else {
        titles = new ArrayList();
    }
    Title t;
    vCardProperty p;
    for (int i=0; i<n; ++i) {
        p.setValue(titleArray[i]);
        t.setTitle(p);
        titles->add(t);
    }
}


Address* BusinessDetail::getAddress() {
    return address;
}

void BusinessDetail::setAddress(Address& a) {
    if (address) {
        delete address;
    }
    address = a.clone();
}

vCardProperty* BusinessDetail::getCompany() {
    return company;
}

void BusinessDetail::setCompany(vCardProperty& p) {
    set(&company, p);
}


vCardProperty* BusinessDetail::getDepartment() {
    return department;
}

void BusinessDetail::setDepartment(vCardProperty& p) {
    set(&department, p);
}


ContactDetail* BusinessDetail::getContactDetail() {
    return contactDetail;
}

void BusinessDetail::setContactDetail(ContactDetail& cd) {
    if (contactDetail) {
        delete contactDetail;
    }
    contactDetail = cd.clone();
}

WCHAR* BusinessDetail::getManager(WCHAR* buf, int size) {
    if (buf == NULL) {
        return manager;
    }

    if (size >= 0) {
        wcsncpy(buf, manager, size);
    } else {
        wcscpy(buf, manager);
    }

    return buf;
}

WCHAR* BusinessDetail::getAssistant(WCHAR* buf, int size) {
    if (buf == NULL) {
        return assistant;
    }

    if (size >= 0) {
        wcsncpy(buf, assistant, size);
    } else {
        wcscpy(buf, assistant);
    }

    return buf;
}

vCardProperty* BusinessDetail::getLogo() {
    return logo;
}

void BusinessDetail::setLogo(vCardProperty& p) {
    set(&logo, p);
}

void BusinessDetail::setManager (WCHAR* m) {
    set(&manager, m);
}

void BusinessDetail::setAssistant (WCHAR* a) {
    set(&assistant, a);
}

void BusinessDetail::set(WCHAR** p, WCHAR* v) {
    if(*p)
        delete *p;
    *p = wstrdup(v);
}

void BusinessDetail::set(vCardProperty** oldProperty, vCardProperty& newProperty) {
    if (*oldProperty) delete *oldProperty;

    *oldProperty = newProperty.clone();
}

BusinessDetail* BusinessDetail::clone() {
    BusinessDetail* ret = new BusinessDetail();

    if (role) {
        ret->setRole(*role);
    }
    if (titles) {
        int s = titles->size();
        WSTRING* array=new WSTRING[s];
        for (int i=0; i<s; ++i) {
            array[i] = ((Title*)titles->get(i))->getTitle()->getValue();
        }

        ret->setTitles(array, s);

    }
    if (address) {
        ret->setAddress(*address);
    }
    if (company) {
        ret->setCompany(*company);
    }
    if (department) {
        ret->setDepartment(*department);
    }
    if (contactDetail) {
        ret->setContactDetail(*contactDetail);
    }
    if (manager) {
        ret->setManager(manager);
    }
    if (assistant) {
        ret->setAssistant(assistant);
    }
    if (logo) {
        ret->setLogo(*logo);
    }

    return ret;
}
