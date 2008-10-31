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

#include "vocl/iCalendar/ToDo.h"
#include "base/globalsdef.h"

USE_NAMESPACE

ToDo::ToDo() {
     classEvent = NULL;
     completed = NULL;
     created = NULL;
     description = NULL;
     dtStamp = NULL;
     dtStart = NULL;
     geo = NULL;
     lastMod = NULL;
     location = NULL;
     organizer = NULL;
     percent = NULL;
     priority = NULL;
     recurID = NULL;
     seq = NULL;
     status = NULL;
     summary = NULL;
     uid = NULL;
     url = NULL;
     due = NULL;
     duration = NULL;
     attach = NULL;
     attendee = NULL;
     categories = NULL;
     comment = NULL;
     contact = NULL;
     exDate = NULL;
     exRule = NULL;
     rStatus = NULL;
     related = NULL;
     resources = NULL;
     rDate = NULL;
     rRule = NULL;
     xProp = NULL;
}

ToDo::~ToDo() {
    if (classEvent) {
        delete classEvent; classEvent = NULL;
    }
    if (completed) {
        delete completed; completed = NULL;
    }
    if (created) {
        delete created; created = NULL;
    }
    if (description) {
        delete description; description = NULL;
    }
    if (dtStamp) {
        delete dtStamp; dtStamp = NULL;
    }
    if (dtStart) {
        delete dtStart; dtStart = NULL;
    }
    if (geo) {
        delete geo; geo = NULL;
    }
    if (lastMod) {
        delete lastMod; lastMod = NULL;
    }
    if (location) {
        delete location; location = NULL;
    }
    if (organizer) {
        delete organizer; organizer = NULL;
    }
    if (percent) {
        delete percent; percent = NULL;
    }
    if (priority) {
        delete priority; priority = NULL;
    }
    if (recurID) {
        delete recurID; recurID = NULL;
    }
    if (seq) {
        delete seq; seq = NULL;
    }
    if (status) {
        delete status; status = NULL;
    }
    if (summary) {
        delete summary; summary = NULL;
    }
    if (uid) {
        delete uid; uid = NULL;
    }
    if (url) {
        delete url; url = NULL;
    }
    if (due) {
        delete due; due = NULL;
    }
    if (duration) {
        delete duration; duration = NULL;
    }
    if (attach) {
        delete attach; attach = NULL;
    }
    if (attendee) {
        delete attendee; attendee = NULL;
    }
    if (categories) {
        delete categories; categories = NULL;
    }
    if (comment) {
        delete comment; comment = NULL;
    }
    if (contact) {
        delete contact; contact = NULL;
    }
    if (exDate) {
        delete exDate; exDate = NULL;
    }
    if (exRule) {
        delete exRule; exRule = NULL;
    }
    if (rStatus) {
        delete rStatus; rStatus = NULL;
    }
    if (related) {
        delete related; related = NULL;
    }
    if (resources) {
        delete resources; resources = NULL;
    }
    if (rDate) {
        delete rDate; rDate = NULL;
    }
    if (rRule) {
        delete rRule; rRule = NULL;
    }
    if (xProp) {
        delete xProp; xProp = NULL;
    }
}


iCalProperty* ToDo::getClass() {
    if(!classEvent) {
        if(containsProperty(TEXT("CLASS")) && getProperty(TEXT("CLASS"))->getValue())
            classEvent = getiCalPropertyFromVProperty(getProperty(TEXT("CLASS")));
    }
    return classEvent;
}
iCalProperty* ToDo::getCreated() {
    if(!created) {
        if(containsProperty(TEXT("CREATED")) && getProperty(TEXT("CREATED"))->getValue())
            created = getiCalPropertyFromVProperty(getProperty(TEXT("CREATED")));
    }
    return created;
}
iCalProperty* ToDo::getDescription() {
    if(!description) {
        if(containsProperty(TEXT("DESCRIPTION")) && getProperty(TEXT("DESCRIPTION"))->getValue())
            description = getiCalPropertyFromVProperty(getProperty(TEXT("DESCRIPTION")));
    }
    return description;
}
iCalProperty* ToDo::getDtStart() {
    if(!dtStart) {
        if(containsProperty(TEXT("DTSTART")) && getProperty(TEXT("DTSTART"))->getValue())
            dtStart = getiCalPropertyFromVProperty(getProperty(TEXT("DTSTART")));
    }
    return dtStart;
}
iCalProperty* ToDo::getGeo() {
    if(!geo) {
        if(containsProperty(TEXT("GEO")) && getProperty(TEXT("GEO"))->getValue())
            geo = getiCalPropertyFromVProperty(getProperty(TEXT("GEO")));
    }
    return geo;
}
iCalProperty* ToDo::getLastMod() {
    if(!lastMod) {
        if(containsProperty(TEXT("LAST-MODIFIED")) && getProperty(TEXT("LAST-MODIFIED"))->getValue())
            lastMod = getiCalPropertyFromVProperty(getProperty(TEXT("LAST-MODIFIED")));
    }
    return lastMod;
}
iCalProperty* ToDo::getLocation() {
    if(!location) {
        if(containsProperty(TEXT("LOCATION")) && getProperty(TEXT("LOCATION"))->getValue())
            location = getiCalPropertyFromVProperty(getProperty(TEXT("LOCATION")));
    }
    return location;
}
iCalProperty* ToDo::getOrganizer() {
    if(!organizer) {
        if(containsProperty(TEXT("ORGANIZER")) && getProperty(TEXT("ORGANIZER"))->getValue())
            organizer = getiCalPropertyFromVProperty(getProperty(TEXT("ORGANIZER")));
    }
    return organizer;
}
iCalProperty* ToDo::getPriority() {
    if(!priority) {
        if(containsProperty(TEXT("PRIORITY")) && getProperty(TEXT("PRIORITY"))->getValue())
            priority = getiCalPropertyFromVProperty(getProperty(TEXT("PRIORITY")));
    }
    return priority;
}
iCalProperty* ToDo::getDtStamp() {
    if(!dtStamp) {
        if(containsProperty(TEXT("DTSTAMP")) && getProperty(TEXT("DTSTAMP"))->getValue())
            dtStamp = getiCalPropertyFromVProperty(getProperty(TEXT("DTSTAMP")));
    }
    return dtStamp;
}
iCalProperty* ToDo::getSequence() {
    if(!seq) {
        if(containsProperty(TEXT("SEQUENCE")) && getProperty(TEXT("SEQUENCE"))->getValue())
            seq = getiCalPropertyFromVProperty(getProperty(TEXT("SEQUENCE")));
    }
    return seq;
}
iCalProperty* ToDo::getStatus() {
    if(!status) {
        if(containsProperty(TEXT("STATUS")) && getProperty(TEXT("STATUS"))->getValue())
            status = getiCalPropertyFromVProperty(getProperty(TEXT("STATUS")));
    }
    return status;
}
iCalProperty* ToDo::getSummary() {
    if(!summary) {
        if(containsProperty(TEXT("SUMMARY")) && getProperty(TEXT("SUMMARY"))->getValue())
            summary = getiCalPropertyFromVProperty(getProperty(TEXT("SUMMARY")));
    }
    return summary;
}
iCalProperty* ToDo::getCompleted() {
    if(!completed) {
        if(containsProperty(TEXT("COMPLETED")) && getProperty(TEXT("COMPLETED"))->getValue())
            completed = getiCalPropertyFromVProperty(getProperty(TEXT("COMPLETED")));
    }
    return completed;
}
iCalProperty* ToDo::getUid() {
    if(!uid) {
        if(containsProperty(TEXT("UID")) && getProperty(TEXT("UID"))->getValue())
            uid = getiCalPropertyFromVProperty(getProperty(TEXT("UID")));
    }
    return uid;
}
iCalProperty* ToDo::getUrl() {
    if(!url) {
        if(containsProperty(TEXT("URL")) && getProperty(TEXT("URL"))->getValue())
            url = getiCalPropertyFromVProperty(getProperty(TEXT("URL")));
    }
    return url;
}
iCalProperty* ToDo::getPercent() {
    if(!percent) {
        if(containsProperty(TEXT("PERCENT-COMPLETE")) && getProperty(TEXT("PERCENT-COMPLETE"))->getValue())
            percent = getiCalPropertyFromVProperty(getProperty(TEXT("PERCENT-COMPLETE")));
    }
    return percent;
}
iCalProperty* ToDo::getRecurID() {
    if(!recurID) {
        if(containsProperty(TEXT("RECURRENCE-ID")) && getProperty(TEXT("RECURRENCE-ID"))->getValue())
            recurID = getiCalPropertyFromVProperty(getProperty(TEXT("RECURRENCE-ID")));
    }
    return recurID;
}
iCalProperty* ToDo::getDue() {
    if(!due) {
        if(containsProperty(TEXT("DUE")) && getProperty(TEXT("DUE"))->getValue())
            due = getiCalPropertyFromVProperty(getProperty(TEXT("DUE")));
    }
    return due;
}
iCalProperty* ToDo::getDuration() {
    if(!duration) {
        if(containsProperty(TEXT("DURATION")) && getProperty(TEXT("DURATION"))->getValue())
            duration = getiCalPropertyFromVProperty(getProperty(TEXT("DURATION")));
    }
    return duration;
}
ArrayList* ToDo::getAttach() {
    if(!attach)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("ATTACH")) && getProperty(i)->getValue()) {
                if(!attach)
                    attach = new ArrayList();
                attach->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return attach;
}
ArrayList* ToDo::getAttendee() {
    if(!attendee)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("ATTENDEE")) && getProperty(i)->getValue()) {
                if(!attendee)
                    attendee = new ArrayList();
                attendee->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return attendee;
}
ArrayList* ToDo::getCategories() {
    if(!categories)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("CATEGORIES")) && getProperty(i)->getValue()) {
                if(!categories)
                    categories = new ArrayList();
                categories->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return categories;
}
ArrayList* ToDo::getComment() {
    if(!comment)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("COMMENT")) && getProperty(i)->getValue()) {
                if(!comment)
                    comment = new ArrayList();
                comment->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return comment;
}
ArrayList* ToDo::getContact() {
    if(!contact)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("CONTACT")) && getProperty(i)->getValue()) {
                if(!contact)
                    contact = new ArrayList();
                contact->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return contact;
}
ArrayList* ToDo::getExDate() {
    if(!exDate)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("EXDATE")) && getProperty(i)->getValue()) {
                if(!exDate)
                    exDate = new ArrayList();
                exDate->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return exDate;
}
ArrayList* ToDo::getExRule() {
    if(!exRule)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("EXRULE")) && getProperty(i)->getValue()) {
                if(!exRule)
                    exRule = new ArrayList();
                exRule->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return exRule;
}
ArrayList*ToDo::getRStatus() {
    if(!rStatus)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("REQUEST-STATUS")) && getProperty(i)->getValue()) {
                if(!rStatus)
                    rStatus = new ArrayList();
                rStatus->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return rStatus;
}
ArrayList* ToDo::getRelated() {
    if(!related)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("RELATED")) && getProperty(i)->getValue()) {
                if(!related)
                    related = new ArrayList();
                related->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return related;
}
ArrayList* ToDo::getResources() {
    if(!resources)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("RESOURCES")) && getProperty(i)->getValue()) {
                if(!resources)
                    resources = new ArrayList();
                resources->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return resources;
}
ArrayList* ToDo::getRDate() {
    if(!rDate)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("RDATE")) && getProperty(i)->getValue()) {
                if(!rDate)
                    rDate = new ArrayList();
                rDate->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return rDate;
}
ArrayList* ToDo::getRRule() {
    if(!rRule)
        for(int i = 0; i < propertiesCount(); i++)
            if(!wcscmp(getProperty(i)->getName(),TEXT("RRULE")) && getProperty(i)->getValue()) {
                if(!rRule)
                    rRule = new ArrayList();
                rRule->add((ArrayElement&) *getiCalPropertyFromVProperty(getProperty(i)));
            }
    return rRule;
}
ArrayList* ToDo::getXProp() {
    if(!xProp)
        for(int i = 0; i < propertiesCount(); i++)
            if((wcsstr(getProperty(i)->getName(),TEXT("X-")) == getProperty(i)->getName()) && getProperty(i)->getValue()) {
                if(!xProp)
                    xProp = new ArrayList();
                WKeyValuePair *kvp = new WKeyValuePair(getProperty(i)->getName(), getProperty(i)->getValue());
                xProp->add((ArrayElement&) *kvp);
                delete kvp; kvp = NULL;
            }
    return xProp;
}
void ToDo::set(iCalProperty** oldProperty, iCalProperty& newProperty) {
    if (*oldProperty) delete *oldProperty;
    *oldProperty = (iCalProperty*)newProperty.clone();
}
void ToDo::setClass(iCalProperty& p) {
    set(&classEvent, p);
    removeProperty(TEXT("CLASS"));
    if(classEvent) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("CLASS"), classEvent));
    }
}
void ToDo::setCreated(iCalProperty& p) {
    set(&created, p);
    removeProperty(TEXT("CREATED"));
    if(created) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("CREATED"), created));
    }
}
void ToDo::setDescription(iCalProperty& p) {
    set(&description, p);
    removeProperty(TEXT("DESCRIPTION"));
    if(description) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("DESCRIPTION"), description));
    }
}
void ToDo::setDtStart(iCalProperty& p) {
    set(&dtStart, p);
    removeProperty(TEXT("DTSTART"));
    if(dtStart) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("DTSTART"), dtStart));
    }
}
void ToDo::setGeo(iCalProperty& p) {
    set(&geo, p);
    removeProperty(TEXT("GEO"));
    if(geo) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("GEO"), geo));
    }
}
void ToDo::setLastMod(iCalProperty& p) {
    set(&lastMod, p);
    removeProperty(TEXT("LAST-MODIFIED"));
    if(lastMod) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("LAST-MODIFIED"), lastMod));
    }
}
void ToDo::setLocation(iCalProperty& p) {
    set(&location, p);
    removeProperty(TEXT("LOCATION"));
    if(location) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("LOCATION"), location));
    }
}
void ToDo::setOrganizer(iCalProperty& p) {
    set(&organizer, p);
    removeProperty(TEXT("ORGANIZER"));
    if(organizer) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("ORGANIZER"), organizer));
    }
}
void ToDo::setPriority(iCalProperty& p) {
    set(&priority, p);
    removeProperty(TEXT("PRIORITY"));
    if(priority) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("PRIORITY"), priority));
    }
}
void ToDo::setDtStamp(iCalProperty& p) {
    set(&dtStamp, p);
    removeProperty(TEXT("DTSTAMP"));
    if(dtStamp) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("DTSTAMP"), dtStamp));
    }
}
void ToDo::setSequence(iCalProperty& p) {
    set(&seq, p);
    removeProperty(TEXT("SEQUENCE"));
    if(seq) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("SEQUENCE"), seq));
    }
}
void ToDo::setStatus(iCalProperty& p) {
    set(&status, p);
    removeProperty(TEXT("STATUS"));
    if(status) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("STATUS"), status));
    }
}
void ToDo::setSummary(iCalProperty& p) {
    set(&summary, p);
    removeProperty(TEXT("SUMMARY"));
    if(summary) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("SUMMARY"), summary));
    }
}
void ToDo::setCompleted(iCalProperty& p) {
    set(&completed, p);
    removeProperty(TEXT("COMPLETED"));
    if(completed) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("COMPLETED"), completed));
    }
}
void ToDo::setUid(iCalProperty& p) {
    set(&uid, p);
    removeProperty(TEXT("UID"));
    if(uid) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("UID"), uid));
    }
}
void ToDo::setUrl(iCalProperty& p) {
    set(&url, p);
    removeProperty(TEXT("URL"));
    if(url) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("URL"), url));
    }
}
void ToDo::setRecurID(iCalProperty& p) {
    set(&recurID, p);
    removeProperty(TEXT("RECURRENCE-ID"));
    if(recurID) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("RECURRENCE-ID"), recurID));
    }
}
void ToDo::setDue(iCalProperty& p) {
    set(&due, p);
    removeProperty(TEXT("DUE"));
    if(due) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("DUE"), due));
        if(duration) {
            delete duration;
            duration = NULL;
        }
        if(containsProperty(TEXT("DURATION")))
            removeProperty(TEXT("DURATON"));
    }
}
void ToDo::setDuration(iCalProperty& p) {
    set(&duration, p);
    removeProperty(TEXT("DURATION"));
    if(duration) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("DURATON"), duration));
        if(due) {
            delete due;
            due = NULL;
        }
        if(containsProperty(TEXT("DUE")))
            removeProperty(TEXT("DUE"));
    }
}
void ToDo::setPercent(iCalProperty& p) {
    set(&percent, p);
    removeProperty(TEXT("PERCENT-COMPLETE"));
    if(percent) {
        insertProperty(getVPropertyFromiCalProperty(TEXT("PERCENT-COMPLETE"), percent));
    }
}
void ToDo::setAttach(ArrayList& list) {
    if (attach) {
        attach->clear();
    } else {
        attach = new ArrayList();
    }
    removeAllProperies(TEXT("ATTACH"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        attach->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("ATTACH"), (iCalProperty*)list[i]));
    }
}
void ToDo::setAttendee(ArrayList& list) {
    if (attendee) {
        attendee->clear();
    } else {
        attendee = new ArrayList();
    }
    removeAllProperies(TEXT("ATTENDEE"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        attendee->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("ATTENDEE"), (iCalProperty*)list[i]));
    }
}
void ToDo::setCategories(ArrayList& list) {
    if (categories) {
        categories->clear();
    } else {
        categories = new ArrayList();
    }
    removeAllProperies(TEXT("CATEGORIES"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        categories->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("CATEGORIES"), (iCalProperty*)list[i]));
    }
}
void ToDo::setComment(ArrayList& list) {
    if (comment) {
        comment->clear();
    } else {
        comment = new ArrayList();
    }
    removeAllProperies(TEXT("COMMENT"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        comment->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("COMMENT"), (iCalProperty*)list[i]));
    }
}
void ToDo::setContact(ArrayList& list) {
    if (contact) {
        contact->clear();
    } else {
        contact = new ArrayList();
    }
    removeAllProperies(TEXT("CONTACT"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        contact->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("CONTACT"), (iCalProperty*)list[i]));
    }
}
void ToDo::setExDate(ArrayList& list) {
    if (exDate) {
        exDate->clear();
    } else {
        exDate = new ArrayList();
    }
    removeAllProperies(TEXT("EXDATE"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        exDate->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("EXDATE"), (iCalProperty*)list[i]));
    }
}
void ToDo::setExRule(ArrayList& list) {
    if (exRule) {
        exRule->clear();
    } else {
        exRule = new ArrayList();
    }
    removeAllProperies(TEXT("EXRULE"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        exRule->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("EXRULE"), (iCalProperty*)list[i]));
    }
}
void ToDo::setRStatus(ArrayList& list) {
    if (rStatus) {
        rStatus->clear();
    } else {
        rStatus = new ArrayList();
    }
    removeAllProperies(TEXT("REQUEST-STATUS"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        rStatus->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("REQUEST-STATUS"), (iCalProperty*)list[i]));
    }
}
void ToDo::setRelated(ArrayList& list) {
    if (related) {
        related->clear();
    } else {
        related = new ArrayList();
    }
    removeAllProperies(TEXT("RELATED"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        related->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("RELATED"), (iCalProperty*)list[i]));
    }
}
void ToDo::setResources(ArrayList& list) {
    if (resources) {
        resources->clear();
    } else {
        resources = new ArrayList();
    }
    removeAllProperies(TEXT("RESOURCES"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        resources->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("RESOURCES"), (iCalProperty*)list[i]));
    }
}
void ToDo::setRDate(ArrayList& list) {
    if (rDate) {
        rDate->clear();
    } else {
        rDate = new ArrayList();
    }
    removeAllProperies(TEXT("RDATE"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        rDate->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("RDATE"), (iCalProperty*)list[i]));
    }
}
void ToDo::setRRule(ArrayList& list) {
    if (rRule) {
        rRule->clear();
    } else {
        rRule = new ArrayList();
    }
    removeAllProperies(TEXT("RRULE"));
    int s = list.size();
    for (int i=0; i<s; ++i) {
        rRule->add(*list[i]);
        insertProperty(getVPropertyFromiCalProperty(TEXT("RRULE"), (iCalProperty*)list[i]));
    }
}
void ToDo::setXProp(ArrayList& list) {
    if (xProp) {
        xProp->clear();
    } else {
        xProp = new ArrayList();
    }
    for(int i = 0, m = propertiesCount(); i < m ; i++)
        if(wcsstr(getProperty(i)->getName(),TEXT("X-")) == getProperty(i)->getName()) {
            removeProperty(i);
            --i;
            --m;
        }
        int s = list.size();
        for (int j=0; j<s; ++j) {
            xProp->add(*list[j]);
            insertProperty(new VProperty(
                                    ((WKeyValuePair*)list[j])->getKey(),
                                    ((WKeyValuePair*)list[j])->getValue()));
        }
}

ArrayElement* ToDo::clone() {
    ToDo* ret = new ToDo();
    if (getClass()) ret->setClass(*classEvent);
    if (getCompleted()) ret->setCompleted(*completed);
    if (getCreated()) ret->setCreated(*created);
    if (getDescription()) ret->setDescription(*description);
    if (getDtStamp()) ret->setDtStamp(*dtStamp);
    if (getDtStart()) ret->setDtStart(*dtStart);
    if (getGeo()) ret->setGeo(*geo);
    if (getLastMod()) ret->setLastMod(*lastMod);
    if (getLocation()) ret->setLocation(*location);
    if (getOrganizer()) ret->setOrganizer(*organizer);
    if (getPercent()) ret->setPercent(*percent);
    if (getPriority()) ret->setPriority(*priority);
    if (getRecurID()) ret->setRecurID(*recurID);
    if (getSequence()) ret->setSequence(*seq);
    if (getStatus()) ret->setStatus(*status);
    if (getSummary()) ret->setSummary(*summary);
    if (getUid()) ret->setUid(*uid);
    if (getUrl()) ret->setUrl(*url);
    if (getDue()) ret->setDue(*due);
    if (getDuration()) ret->setDuration(*duration);
    if (getAttach()) ret->setAttach(*attach);
    if (getAttendee()) ret->setAttendee(*attendee);
    if (getCategories()) ret->setCategories(*categories);
    if (getComment()) ret->setComment(*comment);
    if (getContact()) ret->setContact(*contact);
    if (getExDate()) ret->setExDate(*exDate);
    if (getExRule()) ret->setExRule(*exRule);
    if (getRStatus()) ret->setRStatus(*rStatus);
    if (getRelated()) ret->setRelated(*related);
    if (getResources()) ret->setResources(*resources);
    if (getRDate()) ret->setRDate(*rDate);
    if (getRRule()) ret->setRRule(*rRule);
    if (getXProp()) ret->setXProp(*xProp);

    return ret;
}
WCHAR* ToDo::toString() {
    if(propertiesCount()<1 ||
        wcscmp(getProperty(0)->getName(),TEXT("BEGIN")) ||
        wcscmp(getProperty(0)->getValue(),TEXT("VTODO")) ||
        wcscmp(getProperty(propertiesCount()-1)->getName(),TEXT("END")) ||
        wcscmp(getProperty(propertiesCount()-1)->getValue(),TEXT("VTODO"))) {

            addFirstProperty(new VProperty(TEXT("BEGIN"), TEXT("VTODO")));
            addProperty(new VProperty(TEXT("END"), TEXT("VTODO")));
        }

        return ((VObject*)this)->toString();
}

iCalProperty* ToDo::getiCalPropertyFromVProperty(VProperty* vp) {
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
VProperty* ToDo::getVPropertyFromiCalProperty(WCHAR* name, iCalProperty* prop) {
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

