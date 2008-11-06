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


#include "vocl/VObjectFactory.h"
#include "vocl/vCard/Contact.h"
#include "vocl/vCard/Contact30.h"
#include "vocl/iCalendar/Event.h"
#include "vocl/iCalendar/ToDo.h"

VObject* VObjectFactory::createInstance(WCHAR* objType, WCHAR* objVersion) {

    if(!objType)
        return new VObject();
    if (!wcscmp(objType, TEXT("VCARD")) && objVersion && !wcscmp(objVersion, TEXT("2.1")))
        return new Contact();
    if (!wcscmp(objType, TEXT("VCARD")) && objVersion && !wcscmp(objVersion, TEXT("3.0")))
        return new Contact30();
    else if(!wcscmp(objType, TEXT("VEVENT")))
        return new Event();
    else if(!wcscmp(objType, TEXT("VTODO")))
        return new ToDo();

   /*else if(!wcscmp(objType, TEXT("VJOURNAL"))
        return new Journal();
    else if(!wcscmp(objType, TEXT("VFREEBUSY"))
        return new Freebusy();
    else if(!wcscmp(objType, TEXT("VTIMEZONE"))
        return new Timezone();
    */
    else return new VObject(objType, objVersion);
}
