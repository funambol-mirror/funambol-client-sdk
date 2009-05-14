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


#include "vocl/iCalendar/iCalConverter.h"
#include "base/util/WString.h"
#include "base/globalsdef.h"

USE_NAMESPACE


iCalConverter::iCalConverter() {
    iCalendar = NULL;
    calendar = NULL;
}

iCalConverter::~iCalConverter() {
    if (iCalendar) {
        delete [] iCalendar; iCalendar = NULL;
    }
    if (calendar) {
        delete calendar; calendar = NULL;
    }
}

void iCalConverter::setSource(WCHAR* inputICalendar) {
    if(iCalendar) {
        delete[] iCalendar;
        iCalendar = NULL;
    }
    if(inputICalendar) {
        iCalendar = new WCHAR[wcslen(inputICalendar) + 1];
        wcscpy(iCalendar, inputICalendar);
    }
    if(calendar) {
        delete calendar; calendar = NULL;
    }
}

void iCalConverter::setSource(Calendar& inputCalendar) {
    if(calendar)
        delete calendar;
    calendar = (Calendar *)inputCalendar.clone();
    if(iCalendar) {
        delete [] iCalendar; iCalendar = NULL;
    }
}

void iCalConverter::getICalendar(WCHAR* ouputICalendar) {
    if(iCalendar) {
        if(!ouputICalendar)
            return;
        wcscpy(ouputICalendar, iCalendar);
    }
    else
        ouputICalendar = NULL;
}

void iCalConverter::getCalendar(Calendar** outputCalendar) {
    if(calendar) {
        *outputCalendar = (Calendar *)calendar->clone();
    }
    else
        *outputCalendar = NULL;
}

bool iCalConverter::convert(WString& errorDescription, long* errorCode) {
    errorDescription = TEXT("");
    *errorCode = ERROR_SUCCESS;

    if(!calendar && !iCalendar)
        return false;
    if(calendar && iCalendar)
        return true;
    if(calendar) {

        iCalendar = calendar->toString();

        if(!calendar->getProdID() || !calendar->getProdID()->getValue()) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("'PRODID' property is missing");
            delete [] iCalendar; iCalendar = NULL;
            return false;
        }

        if(!calendar->getVersion() || !calendar->getVersion()->getValue()) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("'VERSION' property is missing");
            delete [] iCalendar; iCalendar = NULL;
            return false;
        }
        if(calendar->getEvents())
            for(int i=0; i<calendar->getEvents()->size(); i++)
                if(!validateEvent(((Event*)calendar->getEvents()->get(i)), errorDescription, errorCode)) {
                    delete [] iCalendar; iCalendar = NULL;
                    return false;
            }
        if(calendar->getToDos())
            for(int i=0; i<calendar->getToDos()->size(); i++)
                if(!validateTodo(((ToDo*)calendar->getToDos()->get(i)), errorDescription, errorCode)) {
                    delete [] iCalendar; iCalendar = NULL;
                    return false;
                }

        return true;
   }
    if (iCalendar) {

        calendar = new Calendar();
        VObject* vo = VConverter::parse(iCalendar);
        if(!vo) {
            *errorCode = ERROR_PARSING_ERROR;
            errorDescription = TEXT("Invalid VObject returned");
            return false;
        }

        int n = vo->propertiesCount();

        if(wcscmp(vo->getProperty(0)->getName(), TEXT("BEGIN")) ||
            !vo->getProperty(0)->getValue() ||
            wcscmp(vo->getProperty(0)->getValue(), TEXT("VCALENDAR"))) {
                *errorCode = ERROR_KEY_PROPERTY_MISSING;
                errorDescription = TEXT("'BEGIN:VCALENDAR' property is missing");
                return false;
            }

        if(wcscmp(vo->getProperty(n-1)->getName(), TEXT("END")) ||
            !vo->getProperty(n-1)->getValue() ||
            wcscmp(vo->getProperty(n-1)->getValue(), TEXT("VCALENDAR"))) {
                *errorCode = ERROR_KEY_PROPERTY_MISSING;
                errorDescription = TEXT("'END:VCALENDAR' property is missing");
                return false;
            }

        if(!vo->containsProperty(TEXT("VERSION"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("'VERSION' property is missing");
            return false;
        }

        if(vo->containsProperty(TEXT("VERSION")) &&
            (!vo->getProperty(TEXT("VERSION")) || wcscmp(vo->getProperty(TEXT("VERSION"))->getValue(), TEXT("2.0")))) {
                *errorCode = ERROR_ILLEGAL_VERSION_NUMBER;
                if(vo->getProperty(TEXT("VERSION"))) {
                    errorDescription = TEXT("Illegal version number : ");
                    errorDescription += vo->getProperty(TEXT("VERSION"))->getValue();
                }
                else {
                    errorDescription = TEXT("Illegal version number");
                }
                return false;
            }
        else {
            iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("VERSION"))->getValue());
            calendar->setVersion(*prop);
            vo->removeProperty(TEXT("VERSION"));
            delete prop;
        }

        if(!vo->containsProperty(TEXT("PRODID")) || !vo->getProperty(TEXT("PRODID"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("'PRODID' property is missing");
            return false;
        }
        else {
            iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("PRODID"))->getValue());
            calendar->setProdID(*prop);
            vo->removeProperty(TEXT("PRODID"));
            delete prop;
        }

        if(vo->containsProperty(TEXT("CALSCALE")) ||
            vo->getProperty(TEXT("CALSCALE"))) {
                iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("CALSCALE"))->getValue());
                calendar->setCalScale(*prop);
                vo->removeProperty(TEXT("CALSCALE"));
                delete prop;
            }

        if(vo->containsProperty(TEXT("METHOD")) ||
            vo->getProperty(TEXT("METHOD"))) {
                iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("METHOD"))->getValue());
                calendar->setMethod(*prop);
                vo->removeProperty(TEXT("METHOD"));
                delete prop;
            }

        //extract VEVENTs from vo
        Event* ev;
        while((ev = extractEvent(vo, errorDescription, errorCode))) {
            if (!validateEvent(ev, errorDescription, errorCode)) {
                delete ev; ev = NULL;
                return false;
            }
            calendar->addEvent(ev);
            delete ev; ev = NULL;
        }

        //extract VTODOs from vo
        ToDo* task;
        while((task = extractTask(vo, errorDescription, errorCode))) {
            if (!validateTodo(task, errorDescription, errorCode)) {
                delete task; task = NULL;
                return false;
            }
            calendar->addToDo(task);
            delete task; task = NULL;
        }
    }

    return true;
}

Event* iCalConverter::extractEvent(VObject* vo, WString& errorDescription, long* errorCode) {
    int i,m;
	int beginEvent = -1;
    int endEvent = -1;
    for(i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginEvent == -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VEVENT")))
            beginEvent = i;
        if(endEvent == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VEVENT"))) {
                endEvent = i;
                break;
            }
    }

    if(beginEvent == -1)
        return NULL;

    if(beginEvent > endEvent) {
        *errorCode = ERROR_INVALID_EVENT_BLOCK;
        errorDescription = TEXT("BEGIN:VEVENT property found, but END:VEVENT is missing");
        return NULL;
    }

    Event* ret = new Event();
    for(i = beginEvent; i <= endEvent; i++) {
        ret->addProperty(vo->getProperty(i));
        vo->removeProperty(i);
        --i;
        --endEvent;
    }

    extractAlarm((VObject*) ret);

    return ret;
}

ToDo* iCalConverter::extractTask(VObject* vo, WString& errorDescription, long* errorCode) {
    int i,m;
	int beginTask = -1;
    int endTask = -1;
    for(i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginTask == -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VTODO")))
            beginTask = i;
        if(endTask == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VTODO"))) {
                endTask = i;
                break;
            }
    }

    if(beginTask == -1)
        return NULL;

    if(beginTask > endTask) {
        *errorCode = ERROR_INVALID_TODO_BLOCK;
        errorDescription = TEXT("BEGIN:VTODO property found, but END:VTODO is missing");
        return NULL;
    }

    ToDo* ret = new ToDo();
    for(i = beginTask; i <= endTask; i++) {
        ret->addProperty(vo->getProperty(i));
        vo->removeProperty(i);
        --i;
        --endTask;
    }

    extractAlarm((VObject*) ret);

    return ret;
}

void iCalConverter::extractAlarm(VObject* vo){
    int beginAlarm = -1;
    int endAlarm = -1;
    for(int i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginAlarm== -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VALARM")))
            beginAlarm = i;
        if(endAlarm == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VALARM"))) {
                endAlarm = i;
                break;
            }
    }

    if(beginAlarm != -1 && endAlarm > beginAlarm)
        for(int i = beginAlarm; i <= endAlarm; i++) {
            vo->removeProperty(i);
            --i;
            --endAlarm;
        }
}

bool iCalConverter::validateEvent(Event* ev, WString& errorDescription, long* errorCode) {
  //validate BEGIN, END, UID
    if(wcscmp(ev->getProperty(0)->getName(), TEXT("BEGIN")) ||
        wcscmp(ev->getProperty(0)->getValue(), TEXT("VEVENT"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("Invalid EVENT: 'BEGIN' property is missing");
            return false;
        }
    if(wcscmp(ev->getProperty(ev->propertiesCount()-1)->getName(), TEXT("END")) ||
        wcscmp(ev->getProperty(ev->propertiesCount()-1)->getValue(), TEXT("VEVENT"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("Invalid EVENT: 'END' property is missing");
            return false;
        }
    if(!ev->containsProperty(TEXT("UID"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription = TEXT("Invalid EVENT: 'UID' property is missing");
        return false;
    }
    for(int i = 0; i < ev->propertiesCount(); i++) {
        if(!wcsstr(EVENT_PROPERTIES_LIST, ev->getProperty(i)->getName()) &&
            wcsstr(ev->getProperty(i)->getName(),TEXT("X-")) != ev->getProperty(i)->getName()) {
                *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
                errorDescription = TEXT("EVENT - Illegal property name : %s");
                errorDescription += ev->getProperty(i)->getName();
                return false;
            }
        if(ev->getProperty(i)->getValue() && !validatePropery(ev->getProperty(i), errorDescription, errorCode))
            return false;
    }
    return true;
}
bool iCalConverter::validateTodo(ToDo* task, WString& errorDescription, long* errorCode) {
    //validate BEGIN, END, UID
    if(wcscmp(task->getProperty(0)->getName(), TEXT("BEGIN")) ||
        wcscmp(task->getProperty(0)->getValue(), TEXT("VTODO"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("Invalid TODO: 'BEGIN' property is missing");
            return false;
        }
    if(wcscmp(task->getProperty(task->propertiesCount()-1)->getName(), TEXT("END")) ||
        wcscmp(task->getProperty(task->propertiesCount()-1)->getValue(), TEXT("VTODO"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            errorDescription = TEXT("Invalid TODO: 'END' property is missing");
            return false;
        }
    if(!task->containsProperty(TEXT("UID"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription = TEXT("Invalid TODO: 'UID' property is missing");
        return false;
    }
    for(int i = 0; i < task->propertiesCount(); i++) {
        if(!wcsstr(TODO_PROPERTIES_LIST, task->getProperty(i)->getName()) &&
            wcsstr(task->getProperty(i)->getName(),TEXT("X-")) != task->getProperty(i)->getName()) {
                *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
                errorDescription = TEXT("TODO - Illegal property name : ");
                errorDescription += task->getProperty(i)->getName();
                return false;
            }
            if(task->getProperty(i)->getValue() && !validatePropery(task->getProperty(i), errorDescription, errorCode))
                return false;
    }
    return true;
}
bool iCalConverter::validatePropery(VProperty* vp, WString& errorDescription, long* errorCode) {

    if(!wcscmp(vp->getName(), TEXT("CLASS"))) {
        if(!wcsstr(CLASS_PROPERTY_VALUE, vp->getValue()) &&
            wcsstr(vp->getValue(),TEXT("X-")) != vp->getValue()) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property CLASS, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
    }
    else if(!wcscmp(vp->getName(), TEXT("CREATED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property CREATED, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DESCRIPTION"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property DESCRIPTION, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTSTART"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property DTSTART, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property DTSTART, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(!validateDT(vp->getValue()) && !validateDate(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property DTSTART, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("GEO"))) {
        if(!validateGeo(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property GEO, Invalid value format : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("LAST-MODIFIED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property LAST-MODIFIED, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("LOCATION"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property LOCATION, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("ORGANIZER"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(ORGANIZER_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property ORGANIZER, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("PRIORITY"))) {
        if(vp->getValue()) {
            if(wcslen(vp->getValue()) > 1 || !isdigit(vp->getValue()[0])) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property PRIORITY, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTSTAMP"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property DTSTAMP, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("SUMMARY"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property SUMMARY, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("TRANSP"))) {
        if(wcscmp(vp->getName(), TEXT("OPAQUE")) && wcscmp(vp->getName(), TEXT("TRANSPARENT"))) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property TRANSP, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }

    else if(!wcscmp(vp->getName(), TEXT("RECURRENCE-ID"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i) &&
                wcscmp(vp->getParameter(i), TEXT("RANGE"))) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property RECURRENCE-ID, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property RECURRENCE-ID, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property RECURRENCE-ID, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTEND"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property DTEND, Invalid parameter: ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property DTEND, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(!validateDT(vp->getValue()) && !validateDate(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property DTEND, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DUE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property DUE, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property DUE, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property DUE, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("COMPLETED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property COMPLETED, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("PERCENT-COMPLETE"))) {
        if(wcslen(vp->getValue()) > 2 ||
            (wcslen(vp->getValue()) == 1 && !isdigit(vp->getValue()[0])) ||
            (wcslen(vp->getValue()) == 2 && (!isdigit(vp->getValue()[0]) || !isdigit(vp->getValue()[1])))) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property PERCENT, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
            }
    }
    else if(!wcscmp(vp->getName(), TEXT("ATTACH"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(ATTACH_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property ATTACH, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
            if(!wcscmp(vp->getParameter(i), TEXT("ENCODING"))
                && wcscmp(vp->getParameterValue(TEXT("ENCODING")), TEXT("BASE64"))) {
                    *errorCode = ERROR_UNSUPPORTED_ENCODING;
                    errorDescription = TEXT("Property ATTACH, unsupported encoding : ");
                    errorDescription += vp->getParameterValue(TEXT("ENCODING"));
                    return false;
                }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("ATTENDEE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(ATTENDEE_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property ATTENDEE, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("COMMENT"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property COMMENT, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("CONTACT"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property CONTACT, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("EXDATE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property EXDATE, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            WCHAR seps[] = TEXT(",");
            WCHAR *token;
            token = wcstok( vp->getValue(), seps );
            while( token != NULL ) {
                if(!validateDate(token)) {
                    *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                    errorDescription = TEXT("Property EXDATE, Invalid value : ");
                    errorDescription += vp->getValue();
                    return false;
                }
                token = wcstok( NULL, seps );
            }
        }
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property EXDATE, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("EXRULE"))) {
        if(!validateRecur(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property EXRULE, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("REQUEST-STATUS"))) {
        //statcode ";" statdesc [";" extdata]
        WCHAR* value = new WCHAR[wcslen(vp->getName())+1];
        wcscpy(value, vp->getName());
        if(!isdigit(value[0]) || value[1] != '.' || !wcschr(value, ';')) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property REQUEST-STATUS, Invalid value : ");
            errorDescription += vp->getValue();
            delete [] value;
            return false;
        }
        delete [] value;
    }
    else if(!wcscmp(vp->getName(), TEXT("RELATED-TO"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(wcscmp(TEXT("RELTYPE"), vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property RELATED-TO, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("RESOURCES"))) {
        for(int i = 0; i < vp->parameterCount(); i++)
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property RESOURCES, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("RDATE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(RDATE_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Property RDATE, Invalid parameter : ");
                    errorDescription += vp->getParameter(i);
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property RDATE, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("PERIOD"))) {
            WCHAR* delimiter;
            delimiter = wcschr(vp->getValue(), '/');
            if(!delimiter) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                errorDescription = TEXT("Property RDATE, Invalid value : ");
                errorDescription += vp->getValue();
                return false;
            }
        }
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property RDATE, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("RRULE"))) {
        if(!validateRecur(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property RRULE, Invalid value : ");
            errorDescription += vp->getValue();
            return false;
        }
    }
    return true;
}

bool iCalConverter::validateGeo(WCHAR* geo) {
    if(!geo)
        return false;

    //expected format: longitude;latitude
    WCHAR* pDest = NULL;
    pDest = wcschr(geo, ';');

    if(!pDest)
        return false;

    WCHAR* longitude = new WCHAR[wcslen(geo)+1];
    wcsncpy(longitude, geo, pDest-geo);

    WCHAR* latitude = new WCHAR[wcslen(geo)+1];
    if(++pDest)
        wcscpy(latitude, pDest);
    else
        return false;

	WCHAR* stopstring;
   // double lon = _wtof(longitude);
   // double lat = _wtof(latitude);

	double lon = wcstod(longitude, &stopstring);
	double lat = wcstod(latitude, &stopstring);

    if(lon == 0.0 || lat == 0.0)
        return false;

    return true;
}

bool iCalConverter::validateDT(WCHAR* dt) {
    //1997 06 30 T 23 59 60 Z
    int len = int(wcslen(dt));
    if(len != 15 && len != 16)
        return false;

    if(len == 16) {
        if(dt[15] != 'Z')
            return false;
    }

    if(dt[8] != 'T')
        return false;

    WCHAR* date;
    date = new WCHAR[9];
    wcsncpy(date, dt, 8);
    date[8] = 0;
    if(!validateDate(date)) {
        delete [] date;
        return false;
    }
    delete [] date;

    WCHAR hour[2];
    hour[0] = dt[9];
    hour[1] = dt[10];
    int h = _wtoi(hour);
    if(h < 0 || h > 23)
        return false;

    WCHAR min[2];
    min[0] = dt[11];
    min[1] = dt[12];
    int m = _wtoi(min);
    if(m < 0 || m > 59)
        return false;

    WCHAR sec[2];
    sec[0] = dt[13];
    sec[1] = dt[14];
    int s = _wtoi(sec);
    if(s < 0 || s > 59)
        return false;

    return true;

}

bool iCalConverter::validateDate(WCHAR* date) {
    if(wcslen(date) != 8)
        return false;

    WCHAR month[2];
    month[0] = date[4];
    month[1] = date[5];
    int mo = _wtoi(month);

    if(mo > 12 || mo < 1)
        return false;

    WCHAR day[2];
    day[0] = date[6];
    day[1] = date[7];

    int d = _wtoi(day);
    if(d > 31 || d < 1)
        return false;

    return true;
}

bool iCalConverter::validateRecur(WCHAR* recur) {

    if(wcsstr(recur, TEXT("FREQ")) != recur)
        return false;

    WCHAR seps[] = TEXT(";");
    WCHAR* token;
    WCHAR* delimiter;

    token = wcstok( recur, seps );
    while( token != NULL ) {
        delimiter = wcschr(token, '=');
        if(!delimiter)
            return false;
        int len = int(wcslen(token));
        WCHAR* item = new WCHAR[len + 1];
        wcsncpy(item, token, delimiter - token);
        item[delimiter - token] = 0;

        if(!wcsstr(RECRUL_ITEMS_LIST, item) && wcsstr(item,TEXT("X-")) != item) {
            delete [] item; item = NULL;
            return false;
        }

        if(!wcscmp(item, TEXT("FREQ"))) {
            WCHAR* value = new WCHAR[len + 1];
            wcscpy(value, ++delimiter);
            if(!wcsstr(FREQUENCY_VALUES_LIST, value)) {
                delete [] value; value = NULL;
                return false;
            }
            delete [] value; value = NULL;
        }

        delete [] item; item = NULL;

        token = wcstok( NULL, seps );
    }
    return true;
}
