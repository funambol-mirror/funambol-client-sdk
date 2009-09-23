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


#ifndef INCL_ICALENDAR_TODO
#define INCL_ICALENDAR_TODO
/** @cond DEV */

#include "vocl/VObject.h"
#include "vocl/iCalendar/iCalProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
    VProperty* getVPropertyFromiCalProperty(const WCHAR* name, iCalProperty* prop);

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

END_NAMESPACE

/** @endcond */
#endif
