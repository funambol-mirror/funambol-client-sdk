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

#include "base/util/utils.h"
#include "vocl/WinEvent.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"
#include "base/timeUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;



// Constructor
WinEvent::WinEvent() {
    vCalendar = L"";
    excludeDate.clear();
    includeDate.clear();
    recipients.clear();
    useTimezone = false;
}

// Constructor: fills propertyMap parsing the passed data
WinEvent::WinEvent(const wstring dataString) {
    vCalendar = L"";
    excludeDate.clear();
    includeDate.clear();
    recipients.clear();
    useTimezone = false;

    parse(dataString);
}

// Destructor
WinEvent::~WinEvent() {
}




// Format and return a vCalendar string from the propertyMap.
wstring& WinEvent::toString() {

    vCalendar = L"";

    //
    // Conversion: WinEvent -> vObject.
    // --------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    DATE startdate = NULL;
    wstring element;


    bool isRecurring = false;
    if (getProperty(L"IsRecurring", element)) {
        isRecurring = (element != TEXT("0"));
    }


    vp = new VProperty(TEXT("BEGIN"), TEXT("VCALENDAR"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("VERSION"), VCALENDAR_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;


    // TIMEZONE: placed out of VEVENT. 
    // Adding it only if the event is recurring.
    if (useTimezone && isRecurring) {
        addTimezone(vo);
    }

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
        stringTimeToDouble(element, &startdate);                // Used later for reminder...
        vp = new VProperty(TEXT("DTSTART"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"End", element)) {
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
        // PRIORITY:1 in vCal = High, subsequent numbers specify a decreasing ordinal priority.
        // We set: 1=High, 2=normal, 3=low
        int importance = _wtoi(element.c_str());
        vp = new VProperty(TEXT("PRIORITY"));
        if (importance == winImportanceHigh) {
            vp->addValue(TEXT("1"));
        } else if (importance == winImportanceLow) {
            vp->addValue(TEXT("3"));
        } else {  
            // default value 
            vp->addValue(TEXT("2"));
        }
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
        bool bReminder = (element != TEXT("0"));

        if(bReminder == true) {
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


    if (isRecurring) {
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
            wstring date = (*it);
            vp->addValue(date.c_str());
        }
        vo->addProperty(vp);
        delete vp; vp = NULL;

        // Exceptions: RDATE (should be empty for Outlook and WM)
        vp = new VProperty(TEXT("RDATE"));
        for (it  = includeDate.begin(); it != includeDate.end(); it++) {
            wstring date = (*it);
            vp->addValue(date.c_str());
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



    //
    // TODO: format ATTENDEES
    //


    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (getProperty(TEXT("BillingInformation"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-BILLINGINFO"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
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
    if (getProperty(L"NoAging", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-NOAGING"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"ReminderOptions", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-AALARMOPTIONS"), element.c_str());
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
    if (vo) { delete vo; vo = NULL; }
    return vCalendar;
}



void WinEvent::addTimezone(VObject* vo) {

    VProperty* vp  = NULL;
    wstring element;

    //
    // TZ: "signed numeric indicating the number of hours and possibly minutes from UTC."
    // TZ = - (Bias + StandardBias)     [StandardBias is usually = 0]
    //
    wstring bias = formatBias(tzInfo.Bias + tzInfo.StandardBias);
    vp = new VProperty(TEXT("TZ"), bias.c_str());
    vo->addProperty(vp);
    delete vp; vp = NULL;

    //
    // DAYLIGHT: "sequence of components that define the daylight savings time rule."
    //
    int yearBegin = 0;
    int yearEnd = 5000;
    getIntervalOfRecurrence(&yearBegin, &yearEnd);


    // DST offset = - (Bias + StandardBias + DaylightBias)
    // [StandardBias is usually = 0]
    wstring hasDST;
    int diffBias = tzInfo.Bias +  + tzInfo.StandardBias + tzInfo.DaylightBias;
    wstring daylightBias;
    if (diffBias != 0) { 
        hasDST = TEXT("TRUE");
        daylightBias = formatBias(diffBias);
    }
    else {
        hasDST = TEXT("FALSE");
    }


    // Max 6 iterations (for infinite recurrences).
    if (yearEnd - yearBegin > MAX_DAYLIGHT_PROPS) {
        yearEnd = yearBegin + MAX_DAYLIGHT_PROPS;
    }
    if (hasDayLightSaving(&tzInfo)) {
        // Add a DAYLIGHT property for every year that this appointment occurr. (max = 6)
        for (int year = yearBegin; year <= yearEnd; year++) {

            wstring daylightDate = getDateFromTzRule(year, tzInfo.DaylightDate);
            wstring standardDate = getDateFromTzRule(year, tzInfo.StandardDate);

            // "DAYLIGHT:TRUE;-0900;20080406T020000;20081026T020000;Pacific Standard Time;Pacific Daylight Time"
            vp = new VProperty(TEXT("DAYLIGHT"));
            vp->addValue(hasDST.c_str());               // DST flag
            if (hasDST == TEXT("TRUE")) {
                vp->addValue(daylightBias.c_str());     // DST offset = (Bias + DaylightBias)
                vp->addValue(daylightDate.c_str());     // Date and time when the DST begins
                vp->addValue(standardDate.c_str());     // Date and time when the DST ends
                vp->addValue(tzInfo.StandardName);      // Standard time designation (optional, could be empty)
                vp->addValue(tzInfo.DaylightName);      // DST designation (optional, could be empty)
            }
            vo->addProperty(vp);
            delete vp; vp = NULL;

            if (hasDST == TEXT("FALSE")) {
                break;    // Send only 1 property, are all the same.
            }
        }
    } else {
        // there is no DAYLIGHT saving
        vp = new VProperty(TEXT("DAYLIGHT"));
        vp->addValue(TEXT("FALSE"));
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

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
        setError(1, ERR_ITEM_VOBJ_PARSE);
        LOG.error("%s", getLastErrorMsg());
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
            }
            
            // for EndDates like "20071121T235900": we convert into "20071122"
            double endDateTime = endDate - (int)endDate;
            if (endDateTime > 0.9) {
                endDate = (int)endDate + 1;
            }

            // Format dates like: "yyyyMMdd"
            doubleToStringTime(endDateValue,   endDate,   true);
            doubleToStringTime(startDateValue, startDate, true);

        }
        setProperty(L"Start",       startDateValue       );
        setProperty(L"End",         endDateValue         );
        setProperty(L"AllDayEvent", isAllDay? L"1" : L"0");
    }


    if(element = getVObjectPropertyValue(vo, L"X-MICROSOFT-CDO-BUSYSTATUS")) {
        wstring value(TEXT("0")); // FREE  
        wstring elementw(element);              
        if (elementw == TEXT("BUSY") || elementw == TEXT("2")) {
            value = TEXT("2");
        } else if (elementw == TEXT("TENTATIVE") || elementw == TEXT("1")) {
            value = TEXT("1");
        } else if (elementw == TEXT("OOF") || elementw == TEXT("3")) { // Out of office
            value = TEXT("3");
        }                     
        setProperty(TEXT("BusyStatus"), value);
    }
    if(element = getVObjectPropertyValue(vo, L"CATEGORIES")) {
        setProperty(L"Categories", element);
    }
    if (element = getVObjectPropertyValue(vo, L"CLASS")) {
        WCHAR tmp[10];
        if (!wcscmp(element, TEXT("CONFIDENTIAL")) ) {
            wsprintf(tmp, TEXT("%i"), winConfidential);     // Confidential = 3
        }
        else if (!wcscmp(element, TEXT("PRIVATE")) ) {
            wsprintf(tmp, TEXT("%i"), winPrivate);          // Private = 2
        }
        else {
            wsprintf(tmp, TEXT("%i"), winNormal);           // Normal = 0
        }
        setProperty(L"Sensitivity", tmp);
    }
    if(element = getVObjectPropertyValue(vo, L"PRIORITY")) {
        // PRIORITY:1 in vCal = High, subsequent numbers specify a decreasing ordinal priority.
        int priority = _wtoi(element);

        int importance;
        if (priority >= 3) {
            importance = winImportanceLow;
        } else if (priority == 1) {
            importance = winImportanceHigh;
        } else {  
            // default value 
            importance = winImportanceNormal;
        }
        setIntProperty(L"Importance", importance);
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
    else {
        // AALARM not found -> reset reminder!
        // Note: this is done for compatibility with most devices: if alarm not set
        //       AALARM property is not sent.
        setProperty(L"ReminderSet", L"0");
    }


    if ( (element = getVObjectPropertyValue(vo, L"RRULE")) && 
         (wcslen(element) > 0) ) {
        setProperty(L"IsRecurring", L"1");

        // RRULE -> Recurrence pattern
        // Fill recPattern propertyMap.
        recPattern.setStartDate(startDate);
        recPattern.parse(element);

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

        // TIMEZONE
        bool tzFound = parseTimezone(vo);
        useTimezone = tzFound;
        getRecPattern()->setUseTimezone(tzFound);

    }
    else {
        // Not recurring.
        setProperty(L"IsRecurring", L"0");
    }

    //
    // TODO: parse ATTENDEES and fill recipients list
    //

    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-BILLINGINFO")) {
        setProperty(TEXT("BillingInformation"), element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-COMPANIES")) {
        setProperty(L"Companies", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-MILEAGE")) {
        setProperty(L"Mileage", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-NOAGING")) {
        setProperty(L"NoAging", element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-AALARMOPTIONS")) {
        setProperty(L"ReminderOptions", element);
    }
    
    if (vo) { delete vo; vo = NULL; }

    return 0;
}


// Utility to check the productID and version of VObject passed.
bool WinEvent::checkVCalendarTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    
    if (!prodID) {
        LOG.error(ERR_ITEM_VOBJ_TYPE_NOTFOUND, L"VCALENDAR");
        return false;
    }
    if (wcscmp(prodID, L"VCALENDAR")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VCALENDAR");
        return false;
    }

    if (!version) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_VERSION_NOTFOUND, VCALENDAR_VERSION);
    }
    else if (wcscmp(version, VCALENDAR_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VCALENDAR_VERSION);
    }
    return true;
}



WinRecurrence* WinEvent::getRecPattern() {
    return &recPattern;
}

exceptionList* WinEvent::getExcludeDates() {
    return &excludeDate;
}

exceptionList* WinEvent::getIncludeDates() {
    return &includeDate;
}

recipientList* WinEvent::getRecipients() {
    return &recipients;
}



const TIME_ZONE_INFORMATION* WinEvent::getTimezone() {
    
    if (useTimezone) {
        return &tzInfo;
    }
    return NULL;
}

void WinEvent::setTimezone(const TIME_ZONE_INFORMATION* tz) {
    
    if (tz) {
        // Copy all values.
        tzInfo.Bias         = tz->Bias;
        tzInfo.DaylightBias = tz->DaylightBias;
        tzInfo.DaylightDate = tz->DaylightDate;
        tzInfo.StandardBias = tz->StandardBias;
        tzInfo.StandardDate = tz->StandardDate;
        
        //wsprintf(tzInfo.DaylightName, TEXT("%s"), tz->DaylightName);
        //wsprintf(tzInfo.StandardName, TEXT("%s"), tz->StandardName);
        wcsncpy(tzInfo.DaylightName, tz->DaylightName, 32);
        wcsncpy(tzInfo.StandardName, tz->StandardName, 32);

        // use the method, to access right specialized object.
        WinRecurrence* rec = getRecPattern();
        if (rec) { rec->setUseTimezone(true); }
        useTimezone = true;
    }
}

bool WinEvent::hasTimezone() {
    return useTimezone;
}



bool WinEvent::parseTimezone(VObject* vo) {

    bool found = false;
    WCHAR* element = NULL;

    if ((element = getVObjectPropertyValue(vo, L"TZ")) && wcslen(element) > 0) {
        
        int bias = parseBias(element);

        wstring dstFlag, dstOffset, standardName, daylightName;
        list<wstring> daylightDates;
        list<wstring> standardDates;

        //
        // Search all DAYLIGHT properties (one for every year)
        //
        for(int i=0; i < vo->propertiesCount(); i++) {

            VProperty* vp = vo->getProperty(i);
            if (!wcscmp(vp->getName(), TEXT("DAYLIGHT"))) {
                // Found a DAYLIGHT property. Many props are redundant, now are overwritten.
                if (element = vp->getPropComponent(1)) { dstFlag   = element;              }
                if (element = vp->getPropComponent(2)) { dstOffset = element;              }
                if (element = vp->getPropComponent(3)) { daylightDates.push_back(element); }
                if (element = vp->getPropComponent(4)) { standardDates.push_back(element); }
                if (element = vp->getPropComponent(5)) { standardName = element;           }
                if (element = vp->getPropComponent(6)) { daylightName = element;           }
            }
            // - to be faster? -
            //else if (!wcscmp(vp->getName(), TEXT("VEVENT"))) {
            //    break;
            //}
        }

        //
        // If we have all required data, fill the tzInfo structure.
        //
        if (dstFlag == TEXT("FALSE")) {
            // Easy timezone, no DST
            found = true;
            tzInfo.Bias         = bias;
            tzInfo.StandardBias = 0;        // Cannot retrieve it, assume = 0 (usually is 0)
            tzInfo.DaylightBias = -60;     // most of the tiemzone is -60. only 1 is not (baghdad)
            memset((void*)(&tzInfo.DaylightDate), 0, sizeof(SYSTEMTIME));
            memset((void*)(&tzInfo.StandardDate) , 0, sizeof(SYSTEMTIME));                                  
            wcsncpy(tzInfo.StandardName, standardName.c_str(), 32);
            wcsncpy(tzInfo.DaylightName, daylightName.c_str(), 32);
        }
        else if (dstFlag.size() && dstOffset.size() && daylightDates.size() && standardDates.size() ) {
            // Standard timezone, the DST rules are extracted from list of dates
            // >> Bias = -TZ
            // >> StandardBias = 0  (Cannot retrieve it, assume = 0 as usually is 0)
            // >> DaylightBias = - (DSTOffset + Bias)
            bool rightValue = true;
            found = true;
            tzInfo.Bias         = bias;
            tzInfo.StandardBias = 0;
            tzInfo.DaylightBias = parseBias(dstOffset.c_str()) - bias;
            tzInfo.DaylightDate = getTzRuleFromDates(daylightDates, &rightValue);
            tzInfo.StandardDate = getTzRuleFromDates(standardDates, &rightValue);
            wcsncpy(tzInfo.StandardName, standardName.c_str(), 32);
            wcsncpy(tzInfo.DaylightName, daylightName.c_str(), 32);
        }
    }
    else {
        // No timezone received.
        found = false;
    }

    return found;
}


void WinEvent::getIntervalOfRecurrence(int* yearBegin, int* yearEnd) {

    wstring element;
    if (getProperty(L"Start", element)) {
        if (element.size() >= 4) {
            swscanf(element.c_str(), L"%4d", yearBegin);
        }

        if (getRecPattern()->getProperty(L"NoEndDate", element) && (element == L"0")) {
            if (getRecPattern()->getProperty(L"PatternEndDate", element)) {
                if (element.size() >= 4) {
                    swscanf(element.c_str(), L"%4d", yearEnd);
                }
            }
        }
    }
}



long WinEvent::getCRC() {

    wstring values;

    // Event props
    mapIterator it = propertyMap.begin();
    while (it != propertyMap.end()) {
        values.append(it->second);
        it ++;
    }

    // Append rec props only if recurring
    wstring isRec;
    if (getProperty(TEXT("IsRecurring"), isRec)) {
        if (isRec == TEXT("1")) {

            // note: use 'getRecPattern()' to retrieve the correct recPattern object
            it = getRecPattern()->propertyMap.begin();
            while (it != getRecPattern()->propertyMap.end()) {
                values.append(it->second);
                it ++;
            }

            // Exceptions
            exceptionsIterator ex = excludeDate.begin();
            while (ex != excludeDate.end()) {
                values.append(*ex);
                ex ++;
            }
            ex = includeDate.begin();
            while (ex != includeDate.end()) {
                values.append(*ex);
                ex ++;
            }
        }
    }


    const WCHAR* s = values.c_str();
    unsigned long crc32 = 0;
    unsigned long dwErrorCode = NO_ERROR;
    unsigned char byte = 0;

    crc32 = 0xFFFFFFFF;
    while(*s != TEXT('\0')) {
        byte = (unsigned char) *s;
        crc32 = ((crc32) >> 8) ^ crc32Table[(byte) ^ ((crc32) & 0x000000FF)];
        s++;
    }
    crc32 = ~crc32;

    return crc32;
}
