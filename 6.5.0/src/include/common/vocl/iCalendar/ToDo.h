/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_ICALENDAR_TODO
#define INCL_ICALENDAR_TODO
/** @cond DEV */

#include "vocl/VObject.h"
#include "vocl/iCalendar/iCalProperty.h"

class ToDo : public VObject, public ArrayElement {

private:
    iCalProperty* classEvent;
    iCalProperty* completed;
    iCalProperty* created;
    iCalProperty* description;
    iCalProperty* dtStamp;
    iCalProperty* dtStart;
    iCalProperty* geo;
    iCalProperty* lastMod;
    iCalProperty* location;
    iCalProperty* organizer;
    iCalProperty* percent;
    iCalProperty* priority;
    iCalProperty* recurID;
    iCalProperty* seq;
    iCalProperty* status;
    iCalProperty* summary;
    iCalProperty* uid;
    iCalProperty* url;
    iCalProperty* due;
    iCalProperty* duration;
    ArrayList* attach;
    ArrayList* attendee;
    ArrayList* categories;
    ArrayList* comment;
    ArrayList* contact;
    ArrayList* exDate;
    ArrayList* exRule;
    ArrayList* rStatus;
    ArrayList* related;
    ArrayList* resources;
    ArrayList* rDate;
    ArrayList* rRule;
    ArrayList* xProp;
    void set(iCalProperty** oldProperty, iCalProperty& newProperty);
    iCalProperty* getiCalPropertyFromVProperty(VProperty* vp);
    VProperty* getVPropertyFromiCalProperty(WCHAR* name, iCalProperty* prop);

public:
    ToDo();
    ~ToDo();
    WCHAR* toString();
    void setClass(iCalProperty& a);
    void setCompleted(iCalProperty& a);
    void setCreated(iCalProperty& a);
    void setDescription(iCalProperty& a);
    void setDtStamp(iCalProperty& a);
    void setDtStart(iCalProperty& a);
    void setGeo(iCalProperty& a);
    void setLastMod(iCalProperty& a);
    void setLocation(iCalProperty& a);
    void setOrganizer(iCalProperty& a);
    void setPercent(iCalProperty& a);
    void setPriority(iCalProperty& a);
    void setRecurID(iCalProperty& a);
    void setSequence(iCalProperty& a);
    void setStatus(iCalProperty& a);
    void setSummary(iCalProperty& a);
    void setUid(iCalProperty& a);
    void setUrl(iCalProperty& a);
    void setDue(iCalProperty& a);
    void setDuration(iCalProperty& a);
    void setAttach(ArrayList& list);
    void setAttendee(ArrayList& list);
    void setCategories(ArrayList& list);
    void setComment(ArrayList& list);
    void setContact(ArrayList& list);
    void setExDate(ArrayList& list);
    void setExRule(ArrayList& list);
    void setRStatus(ArrayList& list);
    void setRelated(ArrayList& list);
    void setResources(ArrayList& list);
    void setRDate(ArrayList& list);
    void setRRule(ArrayList& list);
    void setXProp(ArrayList& list);

    iCalProperty* getClass();
    iCalProperty* getCompleted();
    iCalProperty* getCreated();
    iCalProperty* getDescription();
    iCalProperty* getDtStamp();
    iCalProperty* getDtStart();
    iCalProperty* getGeo();
    iCalProperty* getLastMod();
    iCalProperty* getLocation();
    iCalProperty* getOrganizer();
    iCalProperty* getPercent();
    iCalProperty* getPriority();
    iCalProperty* getRecurID();
    iCalProperty* getSequence();
    iCalProperty* getStatus();
    iCalProperty* getSummary();
    iCalProperty* getUid();
    iCalProperty* getUrl();
    iCalProperty* getDue();
    iCalProperty* getDuration();
    ArrayList* getAttach();
    ArrayList* getAttendee();
    ArrayList* getCategories();
    ArrayList* getComment();
    ArrayList* getContact();
    ArrayList* getExDate();
    ArrayList* getExRule();
    ArrayList* getRStatus();
    ArrayList* getRelated();
    ArrayList* getResources();
    ArrayList* getRDate();
    ArrayList* getRRule();
    ArrayList* getXProp();

	ArrayElement* clone();

};
/** @endcond */
#endif
