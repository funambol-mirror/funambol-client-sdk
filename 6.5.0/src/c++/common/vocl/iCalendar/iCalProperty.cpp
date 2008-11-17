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
#include "vocl/iCalendar/iCalProperty.h"

iCalProperty::iCalProperty (WCHAR* v) {
     altre         = NULL;
     cn            = NULL;
     cutype        = NULL;
     delegatedFrom = NULL;
     delegatedTo   = NULL;
     dir           = NULL;
     encoding      = NULL;
     formattype    = NULL;
     fbtype        = NULL;
     language      = NULL;
     member        = NULL;
     partstat      = NULL;
     range         = NULL;
     trigrel       = NULL;
     related       = NULL;
     role          = NULL;
     rsvp          = NULL;
     sentby        = NULL;
     tzid          = NULL;
     valuetype     = NULL;
     xParams = NULL;

     value  = (v) ? wstrdup(v) : NULL;
}

iCalProperty::~iCalProperty() {
    if (altre) {
        delete [] altre; altre = NULL;
    }
    if (cn) {
        delete [] cn; cn = NULL;
    }
    if (cutype) {
        delete [] cutype; cutype = NULL;
    }
    if (delegatedFrom) {
        delete [] delegatedFrom; delegatedFrom = NULL;
    }
    if (delegatedTo) {
        delete [] delegatedTo; delegatedTo = NULL;
    }
    if (dir) {
        delete [] dir; dir = NULL;
    }
    if (encoding) {
        delete [] encoding; encoding = NULL;
    }
    if (formattype) {
        delete [] formattype; formattype = NULL;
    }
    if (fbtype) {
        delete [] fbtype; fbtype = NULL;
    }
    if (language) {
        delete [] language; language = NULL;
    }
    if (member) {
        delete [] member; member = NULL;
    }
    if (partstat) {
        delete [] partstat; partstat = NULL;
    }
    if (range) {
        delete [] range; range = NULL;
    }
    if (trigrel) {
        delete [] trigrel; trigrel = NULL;
    }
    if (related) {
        delete [] related; related = NULL;
    }
    if (role) {
        delete [] role; role = NULL;
    }
    if (rsvp) {
        delete [] rsvp; rsvp = NULL;
    }
    if (sentby) {
        delete [] sentby; sentby = NULL;
    }
    if (tzid) {
        delete [] tzid; tzid = NULL;
    }
    if (valuetype) {
        delete [] valuetype; valuetype = NULL;
    }
    if (xParams) {
        delete xParams; xParams = NULL;
    }
    if (value) {
        delete [] value; value = NULL;
    }
}

WCHAR* iCalProperty::getAltre (WCHAR* buf, int size) {
    if (buf == NULL) {
        return altre;
    }
    if (size >= 0) {
        wcsncpy(buf, altre, size);
    } else {
        wcscpy(buf, altre);
    }
    return buf;
}
WCHAR* iCalProperty::getCn (WCHAR* buf, int size) {
    if (buf == NULL) {
        return cn;
    }
    if (size >= 0) {
        wcsncpy(buf, cn, size);
    } else {
        wcscpy(buf, cn);
    }
    return buf;
}
WCHAR* iCalProperty::getCutype (WCHAR* buf, int size) {
    if (buf == NULL) {
        return cutype;
    }
    if (size >= 0) {
        wcsncpy(buf, cutype, size);
    } else {
        wcscpy(buf, cutype);
    }
    return buf;
}
WCHAR* iCalProperty::getDelegatedFrom (WCHAR* buf, int size) {
    if (buf == NULL) {
        return delegatedFrom;
    }
    if (size >= 0) {
        wcsncpy(buf, delegatedFrom, size);
    } else {
        wcscpy(buf, delegatedFrom);
    }
    return buf;
}
WCHAR* iCalProperty::getDelegatedTo (WCHAR* buf, int size) {
    if (buf == NULL) {
        return delegatedTo;
    }
    if (size >= 0) {
        wcsncpy(buf, delegatedTo, size);
    } else {
        wcscpy(buf, delegatedTo);
    }
    return buf;
}
WCHAR* iCalProperty::getDir (WCHAR* buf, int size) {
    if (buf == NULL) {
        return dir;
    }
    if (size >= 0) {
        wcsncpy(buf, dir, size);
    } else {
        wcscpy(buf, dir);
    }
    return buf;
}
WCHAR* iCalProperty::getEncoding (WCHAR* buf, int size) {
    if (buf == NULL) {
        return encoding;
    }
    if (size >= 0) {
        wcsncpy(buf, encoding, size);
    } else {
        wcscpy(buf, encoding);
    }
    return buf;
}
WCHAR* iCalProperty::getFormatType (WCHAR* buf, int size) {
    if (buf == NULL) {
        return formattype;
    }
    if (size >= 0) {
        wcsncpy(buf, formattype, size);
    } else {
        wcscpy(buf, formattype);
    }
    return buf;
}
WCHAR* iCalProperty::getFbType (WCHAR* buf, int size) {
    if (buf == NULL) {
        return fbtype;
    }
    if (size >= 0) {
        wcsncpy(buf, fbtype, size);
    } else {
        wcscpy(buf, fbtype);
    }
    return buf;
}
WCHAR* iCalProperty::getLanguage (WCHAR* buf, int size) {
    if (buf == NULL) {
        return language;
    }
    if (size >= 0) {
        wcsncpy(buf, language, size);
    } else {
        wcscpy(buf, language);
    }
    return buf;
}
WCHAR* iCalProperty::getMember (WCHAR* buf, int size) {
    if (buf == NULL) {
        return member;
    }
    if (size >= 0) {
        wcsncpy(buf, member, size);
    } else {
        wcscpy(buf, member);
    }
    return buf;
}
WCHAR* iCalProperty::getPartStat (WCHAR* buf, int size) {
    if (buf == NULL) {
        return partstat;
    }
    if (size >= 0) {
        wcsncpy(buf, partstat, size);
    } else {
        wcscpy(buf, partstat);
    }
    return buf;
}
WCHAR* iCalProperty::getRange (WCHAR* buf, int size) {
    if (buf == NULL) {
        return range;
    }
    if (size >= 0) {
        wcsncpy(buf, range, size);
    } else {
        wcscpy(buf, range);
    }
    return buf;
}
WCHAR* iCalProperty::getTrigRel (WCHAR* buf, int size) {
    if (buf == NULL) {
        return trigrel;
    }
    if (size >= 0) {
        wcsncpy(buf, trigrel, size);
    } else {
        wcscpy(buf, trigrel);
    }
    return buf;
}
WCHAR* iCalProperty::getRelated (WCHAR* buf, int size) {
    if (buf == NULL) {
        return related;
    }
    if (size >= 0) {
        wcsncpy(buf, related, size);
    } else {
        wcscpy(buf, related);
    }
    return buf;
}
WCHAR* iCalProperty::getRole (WCHAR* buf, int size) {
    if (buf == NULL) {
        return role;
    }
    if (size >= 0) {
        wcsncpy(buf, role, size);
    } else {
        wcscpy(buf, role);
    }
    return buf;
}
WCHAR* iCalProperty::getRsvp (WCHAR* buf, int size) {
    if (buf == NULL) {
        return rsvp;
    }
    if (size >= 0) {
        wcsncpy(buf, rsvp, size);
    } else {
        wcscpy(buf, rsvp);
    }
    return buf;
}
WCHAR* iCalProperty::getSentBy (WCHAR* buf, int size) {
    if (buf == NULL) {
        return sentby;
    }
    if (size >= 0) {
        wcsncpy(buf, sentby, size);
    } else {
        wcscpy(buf, sentby);
    }
    return buf;
}
WCHAR* iCalProperty::getTzID (WCHAR* buf, int size) {
    if (buf == NULL) {
        return tzid;
    }
    if (size >= 0) {
        wcsncpy(buf, tzid, size);
    } else {
        wcscpy(buf, tzid);
    }
    return buf;
}
WCHAR* iCalProperty::getValueType (WCHAR* buf, int size) {
    if (buf == NULL) {
        return valuetype;
    }
    if (size >= 0) {
        wcsncpy(buf, valuetype, size);
    } else {
        wcscpy(buf, valuetype);
    }
    return buf;
}
ArrayList* iCalProperty::getXParam () {
    return xParams;
}
WCHAR* iCalProperty::getValue (WCHAR* buf, int size) {
    if (buf == NULL) {
        return value;
    }
    if (size >= 0) {
        wcsncpy(buf, value, size);
    } else {
        wcscpy(buf, value);
    }
    return buf;
}

void iCalProperty::setAltre (WCHAR* e) {
    set(&altre, e);
}
void iCalProperty::setCn (WCHAR* e) {
    set(&cn, e);
}
void iCalProperty::setCutype (WCHAR* e) {
    set(&cutype, e);
}
void iCalProperty::setDelegatedFrom (WCHAR* e) {
    set(&delegatedFrom, e);
}
void iCalProperty::setDelegatedTo (WCHAR* e) {
    set(&delegatedTo, e);
}
void iCalProperty::setDir (WCHAR* e) {
    set(&dir, e);
}
void iCalProperty::setEncoding (WCHAR* e) {
    set(&encoding, e);
}
void iCalProperty::setFormatType (WCHAR* e) {
    set(&formattype, e);
}
void iCalProperty::setFbType (WCHAR* e) {
    set(&fbtype, e);
}
void iCalProperty::setLanguage (WCHAR* e) {
    set(&language, e);
}
void iCalProperty::setMember (WCHAR* e) {
    set(&member, e);
}
void iCalProperty::setPartStat (WCHAR* e) {
    set(&partstat, e);
}
void iCalProperty::setRange (WCHAR* e) {
    set(&range, e);
}
void iCalProperty::setTrigRel (WCHAR* e) {
    set(&trigrel, e);
}
void iCalProperty::setRelated (WCHAR* e) {
    set(&related, e);
}
void iCalProperty::setRole (WCHAR* e) {
    set(&role, e);
}
void iCalProperty::setRsvp (WCHAR* e) {
    set(&rsvp, e);
}
void iCalProperty::setSentBy (WCHAR* e) {
    set(&sentby, e);
}
void iCalProperty::setTzID (WCHAR* e) {
    set(&tzid, e);
}
void iCalProperty::setValueType (WCHAR* e) {
    set(&valuetype, e);
}
void iCalProperty::setXParam (ArrayList& list) {
    if (xParams) {
        xParams->clear();
    } else {
        xParams = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        xParams->add(*list[i]);
    }
}
void iCalProperty::setValue (WCHAR* e) {
    set(&value, e);
}

void iCalProperty::set(WCHAR** property, WCHAR* v) {
    if (*property) {
        delete [] *property;
    }
    *property = (v) ? wstrdup(v) : NULL;
}

ArrayElement * iCalProperty::clone() {
    iCalProperty* ret = new iCalProperty();

    if (altre) ret->setAltre(altre);
    if (cn) ret->setCn(cn);
    if (cutype) ret->setCutype(cutype);
    if (delegatedFrom) ret->setDelegatedFrom(delegatedFrom);
    if (delegatedTo) ret->setDelegatedTo(delegatedTo);
    if (dir) ret->setDir(dir);
    if (encoding) ret->setEncoding(encoding);
    if (formattype) ret->setFormatType(formattype);
    if (fbtype) ret->setFbType(fbtype);
    if (language) ret->setLanguage(language);
    if (member) ret->setMember(member);
    if (partstat) ret->setPartStat(partstat);
    if (range) ret->setRange(range);
    if (trigrel) ret->setTrigRel(trigrel);
    if (related) ret->setRelated(related);
    if (role) ret->setRole(role);
    if (rsvp) ret->setRsvp(rsvp);
    if (sentby) ret->setSentBy(sentby);
    if (tzid) ret->setTzID(tzid);
    if (valuetype) ret->setValueType(valuetype);
    if (xParams) ret->setXParam(*xParams);
    if (value) ret->setValue(value);

    return (ArrayElement *)ret;
}
