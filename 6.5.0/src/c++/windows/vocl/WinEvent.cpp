 /*
 * Copyright (C) 2007 Funambol, Inc.
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
#include "vocl/WinEvent.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"
#include "base/timeUtils.h"

using namespace std;



// Constructor
WinEvent::WinEvent() {
    vCalendar = L"";
}

// Constructor: fills propertyMap parsing the passed data
WinEvent::WinEvent(const wstring dataString) {
    vCalendar = L"";
    parse(dataString);
}

// Destructor
WinEvent::~WinEvent() {
}




// Format and return a vCalendar string from the propertyMap.
wstring& WinEvent::toString() {

    vCalendar = L"";

    //
    // Conversion: WinContact -> vObject.
    // ----------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    DATE startdate = NULL;
    wstring element;


    vp = new VProperty(TEXT("BEGIN"), TEXT("VCALENDAR"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("VERSION"), VCALENDAR_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("BEGIN"), TEXT("VEVENT"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    // Folder path.
    if (getProperty(L"Folder", element)) {
        vp = new VProperty(L"X-FUNAMBOL-FOLDER");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"AllDayEvent", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-ALLDAY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Start", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        stringTimeToDouble(element, &startdate);                // Used later for reminder...
        vp = new VProperty(TEXT("DTSTART"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"End", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        vp = new VProperty(TEXT("DTEND"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    if (getProperty(L"BusyStatus", element)) {
        vp = new VProperty(TEXT("X-MICROSOFT-CDO-BUSYSTATUS"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Categories", element)) {
        vp = new VProperty(TEXT("CATEGORIES"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Body", element)) {
        vp = new VProperty(TEXT("DESCRIPTION"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Location", element)) {
        vp = new VProperty(TEXT("LOCATION"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Importance", element)) {
        vp = new VProperty(TEXT("PRIORITY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"MeetingStatus", element)) {
        vp = new VProperty(TEXT("STATUS"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"ReplyTime", element)) {
        vp = new VProperty(TEXT("X-MICROSOFT-CDO-REPLYTIME"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Subject", element)) {
        vp = new VProperty(TEXT("SUMMARY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Sensitivity", element)) {
        long sensitivity = _wtoi(element.c_str());
        vp = new VProperty(TEXT("CLASS"));
        if(sensitivity == winPrivate) {
            vp->addValue(TEXT("PRIVATE"));
        }
        else if (sensitivity == winConfidential) {
            vp->addValue(TEXT("CONFIDENTIAL"));
        }
        else {  // default value
            vp->addValue(TEXT("PUBLIC"));
        }
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    //
    // ReminderSet
    //
    if (getProperty(L"ReminderSet", element)) {
        BOOL bReminder = _wtoi(element.c_str());

        if(bReminder == TRUE) {
            long minBefore;
            if (getProperty(L"ReminderMinutesBeforeStart", element)) {
                minBefore = _wtoi(element.c_str());
                double minStartDate = startdate * 1440;
                //subtract the alarm
                minStartDate -= minBefore;
                wstring runtime;
                doubleToStringTime(runtime, minStartDate/1440);

                vp = new VProperty(L"AALARM");
                vp->addValue(runtime.c_str());                      // "RunTime"
                vp->addValue(L"");                                  // "Snooze Time" (empty)
                vp->addValue(L"0");                                 // "Repeat Count"

                getProperty(L"ReminderSoundFile", element);         // (empty if not found)
                vp->addValue(element.c_str());                      // "Audio Content" = sound file path

                vo->addProperty(vp);
                delete vp; vp = NULL;
            }
        }
        else {
            // No reminder: send empty "AALARM:"
            vp = new VProperty(L"AALARM");
            vo->addProperty(vp);
            delete vp; vp = NULL;
        }
    }


    if (getProperty(L"IsRecurring", element)) {
        BOOL isRec = _wtoi(element.c_str());
        if(isRec) {
            //
            // Recurrence pattern -> RRULE
            //
            wstring rRule = recPattern.toString();
            if(rRule != L"") {
                vp = new VProperty(TEXT("RRULE"), rRule.c_str());
                vo->addProperty(vp);
                delete vp; vp = NULL;
            }

            list<wstring>::iterator it;

            // Exceptions: EXDATE
            vp = new VProperty(TEXT("EXDATE"));
            for (it  = excludeDate.begin(); it != excludeDate.end(); it++) {
                vp->addValue((*it).c_str());
            }
            vo->addProperty(vp);
            delete vp; vp = NULL;

            // Exceptions: RDATE (should be empty for Outlook and WM)
            vp = new VProperty(TEXT("RDATE"));
            for (it  = includeDate.begin(); it != includeDate.end(); it++) {
                vp->addValue((*it).c_str());
            }
            vo->addProperty(vp);
            delete vp; vp = NULL;

        }
        else {
            // Not recurring: send empty "RRULE:"
            vp = new VProperty(TEXT("RRULE"));
            vo->addProperty(vp);
            delete vp; vp = NULL;
        }
    }

    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (getProperty(L"Companies", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-COMPANIES"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Mileage", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-MILEAGE"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    vp = new VProperty(TEXT("END"), TEXT("VEVENT"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("END"), TEXT("VCALENDAR"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    //
    // Format the vCalendar.
    // ---------------------
    //
    WCHAR* tmp = vo->toString();
    if (tmp) {
        vCalendar = tmp;
        delete [] tmp;
    }
    return vCalendar;
}




//
// Parse a vCalendar string and fills the propertyMap.
//
int WinEvent::parse(const wstring dataString) {

    WCHAR* element = NULL;
    DATE startDate = NULL;
    DATE endDate   = NULL;
    wstring startDateValue, endDateValue;

    //
    // Parse the vCalendar and fill the VObject.
    // -----------------------------------------
    //
    VObject* vo = VConverter::parse(dataString.c_str());
    if (!vo) {
        sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        LOG.error(lastErrorMsg);
        return 1;
    }
    // Check if VObject type and version are the correct ones.
    if (!checkVCalendarTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }


    //
    // Conversion: vObject -> WinEvent.
    // --------------------------------
    // Note: properties found are added to the propertyMap, so that the 
    //       map will contain only parsed properties after this process.
    //
    if(element = getVObjectPropertyValue(vo, L"SUMMARY")) {
        setProperty(L"Subject", element);
    }
    if(element = getVObjectPropertyValue(vo, L"LOCATION")) {
        setProperty(L"Location", element);
    }
    if(element = getVObjectPropertyValue(vo, L"DESCRIPTION")) {
        setProperty(L"Body", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-FOLDER")) {
        setProperty(L"Folder", element);
    }

    //
    // DTSTART, DTEND:
    // Set the start and end date. If the start is 00:00 and end is 23:59 the appointment is decided to be
    // an all day event. So the AllDayEvent property is set to '1' in the propertyMap.
    //
    if(element = getVObjectPropertyValue(vo, L"DTSTART")){
        startDateValue = element;
        stringTimeToDouble(element, &startDate);            // 'startDate' will be used also for RRULE parsing
    }
    if(element = getVObjectPropertyValue(vo, L"DTEND")) {
        endDateValue = element;
        stringTimeToDouble(element, &endDate);
    }

    if (startDate && endDate) {
        // ALL-DAY EVENT
        bool isAllDay = false;
        if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-ALLDAY")){
            isAllDay = wcscmp(element, L"1")?  false : true;
        }
        if (!isAllDay) {
            // All-day check #2: interval [00:00 - 23:59]
            isAllDay = isAllDayInterval(startDate, endDate);
        }
        if (!isAllDay) {
            // All-day check #3: format "yyyyMMdd"
            if (startDateValue.size() == 8 && endDateValue.size() == 8 ) {
                isAllDay = true;
            }
        }

        if (isAllDay) {
            // Safe check on endDate: min value is 'startDate + 1'
            if (endDate <= startDate) {
                endDate = startDate + 1;
                doubleToStringTime(endDateValue, endDate, true);
            }
        }
        setProperty(L"Start",       startDateValue       );
        setProperty(L"End",         endDateValue         );
        setProperty(L"AllDayEvent", isAllDay? L"1" : L"0");
    }


    if(element = getVObjectPropertyValue(vo, L"X-MICROSOFT-CDO-BUSYSTATUS")) {
        setProperty(L"BusyStatus", element);
    }
    if(element = getVObjectPropertyValue(vo, L"CATEGORIES")) {
        setProperty(L"Categories", element);
    }
    if(element = getVObjectPropertyValue(vo, L"CLASS")) {
        WCHAR tmp[10];
        if( !wcscmp(element, TEXT("PRIVATE"     )) || 
            !wcscmp(element, TEXT("CONFIDENTIAL")) ) {
            wsprintf(tmp, TEXT("%i"), winPrivate);          // Private = 2
        }
        else {
            wsprintf(tmp, TEXT("%i"), winNormal);           // Normal = 0
        }
        setProperty(L"Sensitivity", tmp);
    }
    if(element = getVObjectPropertyValue(vo, L"PRIORITY")) {
        setProperty(L"Importance", element);
    }
    if(element = getVObjectPropertyValue(vo, L"STATUS")) {
        setProperty(L"MeetingStatus", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-MICROSOFT-CDO-REPLYTIME")) {
        setProperty(L"ReplyTime", element);
    }


    // AALARM
    // The value consists of: RunTime, SnoozeTime, RepeatCount, AudioContent
    if(element = getVObjectPropertyValue(vo, L"AALARM")) {
        WCHAR tmp[10];
        WCHAR* runTimeValue = vo->getProperty(TEXT("AALARM"))->getPropComponent(1);
        if (wcslen(runTimeValue) > 0) {
            setProperty(L"ReminderSet", L"1");
            DATE runTime = 0;
            stringTimeToDouble(runTimeValue, &runTime);

            long minBeforeEvent = round((startDate - runTime) * 1440);
            // Safety check: values < 0 not accepted.
            if (minBeforeEvent < 0) {
                minBeforeEvent = 0;
            }
            wsprintf(tmp, TEXT("%i"), minBeforeEvent);
            setProperty(L"ReminderMinutesBeforeStart", tmp);

            // Reminder sound file path
            WCHAR* filePath = vo->getProperty(TEXT("AALARM"))->getPropComponent(4);
            if (filePath && wcslen(filePath)>0) {
                setProperty(L"ReminderSoundFile", filePath);
            }
            else {
                setProperty(L"ReminderSoundFile", L"");
            }
        }
        else {
            // RunTime not found -> no reminder
            setProperty(L"ReminderSet", L"0");
        }
    }


    if(element = getVObjectPropertyValue(vo, L"RRULE")) {
        setProperty(L"IsRecurring", L"1");

        // RRULE -> Recurrence pattern
        // Fill recPattern propertyMap.
        recPattern.parse(element, startDate);

        // EXDATE -> fill excludeDate list
        VProperty* vprop = vo->getProperty(L"EXDATE");
        if(vprop) {
            for (int i=0; element = vprop->getValue(i); i++) {
                if (wcslen(element) > 0) {
                    excludeDate.push_back(element);
                }
            }
        }
        // RDATE -> fill includeDate list
        vprop = vo->getProperty(L"RDATE");
        if(vprop) {
            for (int i=0; element = vprop->getValue(i); i++) {
                if (wcslen(element) > 0) {
                    includeDate.push_back(element);
                }
            }
        }
    }
    else {
        // Not recurring.
        setProperty(L"IsRecurring", L"0");
    }

    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-COMPANIES")) {
        setProperty(L"Companies", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-MILEAGE")) {
        setProperty(L"Mileage", element);
    }


    return 0;
}


// Utility to check the productID and version of VObject passed.
bool WinEvent::checkVCalendarTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    if (!prodID || !version) {
        return false;
    }

    if (wcscmp(prodID, L"VCALENDAR")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VCALENDAR");
        return false;
    }
    if (wcscmp(version, VCALENDAR_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VCALENDAR_VERSION);
    }

    return true;
}


// Utility to safe-retrieve the property value inside VObject 'vo'.
WCHAR* WinEvent::getVObjectPropertyValue(VObject* vo, const WCHAR* propertyName) {

    WCHAR* propertyValue = NULL;
    VProperty* vprop = vo->getProperty(propertyName);
    if (vprop && vprop->getValue()) {
        propertyValue = vprop->getValue();
    }
    return propertyValue;
}



WinRecurrence* WinEvent::getRecPattern() {
    return &recPattern;
}

list<wstring>* WinEvent::getExcludeDates() {
    return &excludeDate;
}

list<wstring>* WinEvent::getIncludeDates() {
    return &includeDate;
}

list<WinRecipient>* WinEvent::getRecipients() {
    return &recipients;
}
