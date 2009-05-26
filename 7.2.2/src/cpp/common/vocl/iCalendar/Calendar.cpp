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


#include "vocl/iCalendar/Calendar.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Calendar::Calendar() {
    prodID = NULL;
    version = NULL;
    calScale = NULL;
    method = NULL;
    xTags = NULL;
    events = NULL;
    todos = NULL;
}

Calendar::~Calendar() {
    if(prodID) {
        delete prodID; prodID = NULL;
    }
    if(version) {
        delete version; version = NULL;
    }
    if(calScale) {
        delete calScale; calScale = NULL;
    }
    if(method) {
        delete method; method = NULL;
    }
    if(xTags) {
        delete xTags; xTags = NULL;
    }
    if(events) {
        delete events; events = NULL;
    }
    if(todos) {
        delete todos; todos = NULL;
    }
}

void Calendar::set(iCalProperty** oldProperty, iCalProperty& newProperty) {
    if (*oldProperty) delete *oldProperty;

    *oldProperty = (iCalProperty*)newProperty.clone();
}

void Calendar::setProdID(iCalProperty& p) {
    set(&prodID, p);
}
void Calendar::setVersion(iCalProperty& p) {
    set(&version, p);
}
void Calendar::setCalScale(iCalProperty& p) {
    set(&calScale, p);
}
void Calendar::setMethod(iCalProperty& p) {
    set(&method, p);
}
void Calendar::setXTags(ArrayList& list) {
    if (xTags) {
        xTags->clear();
    } else {
        xTags = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        xTags->add(*list[i]);
    }
}
void Calendar::setEvents(ArrayList& list) {
    if (events) {
        events->clear();
    } else {
        events = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        events->add(*list[i]);
    }
}
void Calendar::setToDos(ArrayList& list) {
    if (todos) {
        todos->clear();
    } else {
        todos = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        todos->add(*list[i]);
    }
}
iCalProperty* Calendar::getProdID() {
    return prodID;
}
iCalProperty* Calendar::getVersion() {
    return version;
}
iCalProperty* Calendar::getCalScale() {
    return calScale;
}
iCalProperty* Calendar::getMethod() {
    return method;
}
ArrayList* Calendar::getXTags() {
    return xTags;
}
ArrayList* Calendar::getEvents() {
    return events;
}
ArrayList* Calendar::getToDos() {
    return todos;
}
WCHAR* Calendar::toString() {

    WCHAR eol[] = TEXT("\r\n");
    WCHAR* ret = new WCHAR[ICALENDAR_BUFFER];
    wcscpy(ret, TEXT("BEGIN:VCALENDAR\r\n"));
    if(version) {
        wcscat(ret, TEXT("VERSION:"));
        wcscat(ret, version->getValue());
        wcscat(ret, eol);
    }
    if(prodID) {
        wcscat(ret, TEXT("PRODID:"));
        wcscat(ret, prodID->getValue());
        wcscat(ret, eol);
    }
    if(calScale) {
        wcscat(ret, TEXT("CALSCALE:"));
        wcscat(ret, calScale->getValue());
        wcscat(ret, eol);
    }
    if(method) {
        wcscat(ret, TEXT("METHOD:"));
        wcscat(ret, method->getValue());
        wcscat(ret, eol);
    }
    if(events)
        for(int i=0; i < events->size(); i++) {
            Event* ev = (Event*)events->get(i);
            wcscat(ret, ev->toString());
        }
    if(todos)
        for(int i=0; i < todos->size(); i++) {
            ToDo* task = (ToDo*)todos->get(i);
            wcscat(ret, task->toString());
        }
    wcscat(ret, TEXT("END:VCALENDAR\r\n"));

    return ret;
}

ArrayElement* Calendar::clone() {
    Calendar* ret = new Calendar();
    if(prodID)
        ret->setProdID(*prodID);
    if(version)
        ret->setVersion(*version);
    if(calScale)
        ret->setCalScale(*calScale);
    if(method)
        ret->setMethod(*method);
    if(xTags)
        ret->setXTags(*xTags);
    if(events)
        ret->setEvents(*events);
    if(todos)
        ret->setToDos(*todos);

    return (ArrayElement*)ret;
}

iCalProperty* Calendar::getiCalPropertyFromVProperty(VProperty* vp) {
    if (vp) {
        iCalProperty *prop = new iCalProperty(vp->getValue());
        if(vp->getParameterValue(TEXT("ALTREP")))
            prop->setAltre(vp->getParameterValue(TEXT("ALTREP")));
        if(vp->getParameterValue(TEXT("CN")))
            prop->setCn(vp->getParameterValue(TEXT("CN")));
        if(vp->getParameterValue(TEXT("CUTYPE")))
            prop->setAltre(vp->getParameterValue(TEXT("CUTYPE")));
        if(vp->getParameterValue(TEXT("DELEGATED-FROM")))
            prop->setDelegatedFrom(vp->getParameterValue(TEXT("DELEGATED-FROM")));
        if(vp->getParameterValue(TEXT("DELEGATED-TO")))
            prop->setDelegatedTo(vp->getParameterValue(TEXT("DELEGATED-TO")));
        if(vp->getParameterValue(TEXT("DIR")))
            prop->setDir(vp->getParameterValue(TEXT("DIR")));
        if(vp->getParameterValue(TEXT("ENCODING")))
            prop->setEncoding(vp->getParameterValue(TEXT("ENCODING")));
        if(vp->getParameterValue(TEXT("FMTTYPE")))
            prop->setFormatType(vp->getParameterValue(TEXT("FMTTYPE")));
        if(vp->getParameterValue(TEXT("FBTYPE")))
            prop->setFbType(vp->getParameterValue(TEXT("FBTYPE")));
        if(vp->getParameterValue(TEXT("LANGUAGE")))
            prop->setLanguage(vp->getParameterValue(TEXT("LANGUAGE")));
        if(vp->getParameterValue(TEXT("MEMBER")))
            prop->setMember(vp->getParameterValue(TEXT("MEMBER")));
        if(vp->getParameterValue(TEXT("PARTSTAT")))
            prop->setPartStat(vp->getParameterValue(TEXT("PARTSTAT")));
        if(vp->getParameterValue(TEXT("RANGE")))
            prop->setRange(vp->getParameterValue(TEXT("RANGE")));
        if(vp->getParameterValue(TEXT("RELATED")))
            prop->setTrigRel(vp->getParameterValue(TEXT("RELATED")));
        if(vp->getParameterValue(TEXT("RELTYPE")))
            prop->setRelated(vp->getParameterValue(TEXT("RELTYPE")));
        if(vp->getParameterValue(TEXT("ROLE")))
            prop->setRole(vp->getParameterValue(TEXT("ROLE")));
        if(vp->getParameterValue(TEXT("RSVP")))
            prop->setRsvp(vp->getParameterValue(TEXT("RSVP")));
        if(vp->getParameterValue(TEXT("SENT-BY")))
            prop->setSentBy(vp->getParameterValue(TEXT("SENT-BY")));
        if(vp->getParameterValue(TEXT("TZID")))
            prop->setTzID(vp->getParameterValue(TEXT("TZID")));
        if(vp->getParameterValue(TEXT("VALUE")))
            prop->setValueType(vp->getParameterValue(TEXT("VALUE")));

        ArrayList* xParamList = NULL;
        WKeyValuePair* xTagParam = NULL;
        WCHAR* xParamName = NULL;
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(wcsstr(vp->getParameter(i),TEXT("X-")) == vp->getParameter(i)) {
                xParamName = new WCHAR[wcslen(vp->getParameter(i)) + 1];
                wcscpy(xParamName, vp->getParameter(i));

                xTagParam = new WKeyValuePair();
                xTagParam->setKey(xParamName);
                if(vp->getParameterValue(xParamName))
                    xTagParam->setValue(vp->getParameterValue(xParamName));
                if(!xParamList)
                    xParamList = new ArrayList();
                xParamList->add((ArrayElement&) *xTagParam);
                delete [] xParamName; xParamName = NULL;
                delete xTagParam; xTagParam = NULL;
            }
        }
        if(xParamList)
            prop->setXParam(*xParamList);
        delete xParamList; xParamList = NULL;

        return prop;
    }
    else
        return NULL;

}
VProperty* Calendar::getVPropertyFromiCalProperty(WCHAR* name, iCalProperty* prop) {
    if(name && prop) {
        VProperty *vprop = new VProperty(name, prop->getValue());

        if(prop->getAltre())
            vprop->addParameter(TEXT("ALTREP"), prop->getAltre());
        if(prop->getCn())
            vprop->addParameter(TEXT("CN"), prop->getCn());
        if(prop->getCutype())
            vprop->addParameter(TEXT("CUTYPE"), prop->getCutype());
        if(prop->getDelegatedFrom())
            vprop->addParameter(TEXT("DELEGATED-FROM"), prop->getDelegatedFrom());
        if(prop->getDelegatedTo())
            vprop->addParameter(TEXT("DELEGATED-TO"), prop->getDelegatedTo());
        if(prop->getDir())
            vprop->addParameter(TEXT("DIR"), prop->getDir());
        if(prop->getEncoding())
            vprop->addParameter(TEXT("ENCODING"), prop->getEncoding());
        if(prop->getFormatType())
            vprop->addParameter(TEXT("FMTTYPE"), prop->getFormatType());
        if(prop->getFbType())
            vprop->addParameter(TEXT("FBTYPE"), prop->getFbType());
        if(prop->getLanguage())
            vprop->addParameter(TEXT("LANGUAGE"), prop->getLanguage());
        if(prop->getMember())
            vprop->addParameter(TEXT("MEMBER"), prop->getMember());
        if(prop->getPartStat())
            vprop->addParameter(TEXT("PARTSTAT"), prop->getPartStat());
        if(prop->getRange())
            vprop->addParameter(TEXT("RANGE"), prop->getRange());
        if(prop->getTrigRel())
            vprop->addParameter(TEXT("RELATED"), prop->getTrigRel());
        if(prop->getRelated())
            vprop->addParameter(TEXT("RELTYPE"), prop->getRelated());
        if(prop->getRole())
            vprop->addParameter(TEXT("ROLE"), prop->getRole());
        if(prop->getRsvp())
            vprop->addParameter(TEXT("RSVP"), prop->getRsvp());
        if(prop->getSentBy())
            vprop->addParameter(TEXT("SENT-BY"), prop->getSentBy());
        if(prop->getTzID())
            vprop->addParameter(TEXT("TZID"), prop->getTzID());
        if(prop->getValueType())
            vprop->addParameter(TEXT("VALUE"), prop->getValueType());
        if(prop->getXParam()) {
            ArrayList* xParamList = new ArrayList();
            xParamList = prop->getXParam();
            for(int i = 0; i<xParamList->size(); i++) {
                WKeyValuePair* xParam = (WKeyValuePair*)xParamList->get(i);
                if(xParam->getKey())
                    if(xParam->getValue())
                        vprop->addParameter(xParam->getKey(), xParam->getValue());
                    else
                        vprop->addParameter(xParam->getKey(), NULL);
            }
            delete xParamList; xParamList = NULL;
        }

        return vprop;
    }
    else
        return NULL;
}
void Calendar::addEvent(Event* ev) {
    if(ev) {
        if(!events)
            events = new ArrayList();
        events->add((ArrayElement&)*ev);
    }
}
void Calendar::addToDo(ToDo* task) {
    if(task) {
        if(!todos)
            todos = new ArrayList();
        todos->add((ArrayElement&)*task);
    }
}
