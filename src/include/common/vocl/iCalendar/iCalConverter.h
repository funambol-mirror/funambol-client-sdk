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


#ifndef INCL_VIRTUAL_CALENDAR_CONVERTER
#define INCL_VIRTUAL_CALENDAR_CONVERTER
/** @cond DEV */

#include "base/fscapi.h"
#include "vocl/iCalendar/Calendar.h"
#include "vocl/VConverter.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class WString;

#define EVENT_PROPERTIES_LIST TEXT("BEGIN,CLASS,CREATED,DESCRIPTION,DTSTART,GEO,LAST-MODIFIED,LOCATION,ORGANIZER,PRIORITY,") \
                                   TEXT("DTSTAMP,SEQUENCE,STATUS,SUMMARY,TRANSP,UID,URL,RECURRENCE-ID,DTEND,DURATION,ATTACH,ATTENDEE,") \
                                   TEXT("CATEGORIES,COMMENT,CONTACT,EXDATE,EXRULE,REQUEST-STATUS,RELATED,RESOURCES,RDATE,RRULE,END,GROUP")

#define TODO_PROPERTIES_LIST  TEXT("BEGIN,CLASS,COMPLETED,CREATED,DESCRIPTION,DTSTAMP,DTSTART,GEO,LAST-MODIFIED,LOCATION,ORGANIZER,PERCENT-COMPLETE,") \
                                   TEXT("PRIORITY,RECURRENCE-ID,SEQUENCE,STATUS,SUMMARY,UID,URL,DUE,DURATION,ATTACH,ATTENDEE,") \
                                   TEXT("CATEGORIES,COMMENT,CONTACT,EXDATE,EXRULE,REQUEST-STATUS,RELATED,RESOURCES,RDATE,RRULE,END,GROUP")

#define CLASS_PROPERTY_VALUE  TEXT("PUBLIC,PRIVATE,CONFIDENTIAL")

#define COMMENT_PARAM_LIST      TEXT("ALTREP,LANGUAGE")
#define DT_PARAM_LIST           TEXT("VALUE,TZID")
#define ORGANIZER_PARAM_LIST    TEXT("CN,DIR,SENT-BY,LANGUAGE,ROLE")
#define ATTACH_PARAM_LIST       TEXT("FMTTYPE,ENCODING,VALUE")
#define ATTENDEE_PARAM_LIST     TEXT("CUTYPE,MEMBER,ROLE,PARTSTAT,RSVP,DELEGATED-TO,DELEGATED-FROM,SENT-BY,CN,DIR,LANGUAGE")
#define RDATE_PARAM_LIST        TEXT("VALUE,DATE-TIME,DATE,PERIOD,TZID")
#define RECRUL_ITEMS_LIST       TEXT("FREQ,UNTIL,COUNT,INTERVAL,BYSECOND,BYMINUTE,BYHOUR,BYDAY,BYMONTHDAY,BYYEARDAY,BYWEEKNO,BYMONTH,BYSETPOS,WKST")
#define FREQUENCY_VALUES_LIST   TEXT("SECONDLY,MINUTELY,HOURLY,DAILY,WEEKLY,MONTHLY,YEARLY")

#define ERROR_SUCCESS                0L
#define ERROR_KEY_PROPERTY_MISSING   100L
#define ERROR_ILLEGAL_VERSION_NUMBER 101L
#define ERROR_ILLEGAL_PROPERTY_NAME  102L
#define ERROR_UNSUPPORTED_ENCODING   103L
#define ERROR_ILLEGAL_TYPE_PARAMETER 104L
#define ERROR_ILLEGAL_PARAMETER      105L
#define ERROR_INVALID_PROPERTY_VALUE 106L
#define ERROR_PARSING_ERROR          200L
#define ERROR_INVALID_EVENT_BLOCK    301L
#define ERROR_INVALID_TODO_BLOCK     302L

//#define isdigit(c) ((c) >= '0' && (c)<= '9') FIXME


class iCalConverter {
private:
    WCHAR* iCalendar;
    Calendar* calendar;
    bool validateEvent(Event*, WString& error, long* errorCode);
    bool validateTodo(ToDo*, WString& error, long* errorCode);
    bool validatePropery(VProperty*, WString& error, long* errorCode);
    Event* extractEvent(VObject* vo, WString& errorDescription, long* errorCode);
    ToDo* extractTask(VObject* vo, WString& errorDescription, long* errorCode);
    bool validateGeo(WCHAR* geo);
    bool validateDT(WCHAR* dt);
    bool validateDate(WCHAR* date);
    bool validateRecur(WCHAR* recur);
    void extractAlarm(VObject* vo);

public:
    iCalConverter();
    ~iCalConverter();
    void setSource(WCHAR* inputICalendar);
    void setSource(Calendar& inputCalendar);
    void getICalendar(WCHAR* vCard);
    void getCalendar(Calendar** outputContact);
    bool convert(WString& error, long* errorCode);
};

END_NAMESPACE

/** @endcond */
#endif
