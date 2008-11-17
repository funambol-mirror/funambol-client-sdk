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


#ifndef INCL_ICALENDAR_CALENDAR
#define INCL_ICALENDAR_CALENDAR
/** @cond DEV */

#define ICALENDAR_BUFFER 30000

#include "vocl/VProperty.h"
#include "vocl/iCalendar/iCalProperty.h"
#include "vocl/iCalendar/Event.h"
#include "vocl/iCalendar/ToDo.h"

class Calendar
{
private:
    iCalProperty* prodID;
    iCalProperty* version;
    iCalProperty* calScale;
    iCalProperty* method;
    ArrayList* xTags;
    ArrayList* events;
    ArrayList* todos;
    void set(iCalProperty** oldProperty, iCalProperty& newProperty);
    iCalProperty* getiCalPropertyFromVProperty(VProperty* vp);
    VProperty* getVPropertyFromiCalProperty(WCHAR* name, iCalProperty* prop);

public:
    Calendar();
    ~Calendar();
    void setProdID(iCalProperty& p);
    void setVersion(iCalProperty& p);
    void setCalScale(iCalProperty& p);
    void setMethod(iCalProperty& p);
    void setXTags(ArrayList& list);
    void setEvents(ArrayList& list);
    void setToDos(ArrayList& list);
    void addEvent(Event* ev);
    void addToDo(ToDo* task);
    iCalProperty* getProdID();
    iCalProperty* getVersion();
    iCalProperty* getCalScale();
    iCalProperty* getMethod();
    ArrayList* getXTags();
    ArrayList* getEvents();
    ArrayList* getToDos();
    WCHAR* toString();

    ArrayElement* clone();
};

/** @endcond */
#endif
