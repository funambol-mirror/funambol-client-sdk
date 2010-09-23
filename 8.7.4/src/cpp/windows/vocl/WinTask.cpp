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
#include "vocl/WinTask.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"
#include "base/timeUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;



// Constructor
WinTask::WinTask() {
}

// Constructor: fills propertyMap parsing the passed data
WinTask::WinTask(const wstring & dataString) {
    parse(dataString);
}

// Destructor
WinTask::~WinTask() {
}




// Format and return a vCalendar string from the propertyMap.
wstring WinTask::toString() {

    wstring vCalendar;
    vCalendar = L"";

    //
    // Conversion: WinTask -> vObject.
    // -------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    wstring element;
    bool statusIsSet = false;

    vo->addProperty(TEXT("BEGIN"), TEXT("VCALENDAR"));     
    vo->addProperty(TEXT("VERSION"), VCALENDAR_VERSION);
    vo->addProperty(TEXT("BEGIN"), TEXT("VTODO"));

    // Folder path.
    if (getProperty(L"Folder", element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-FOLDER"), element.c_str());       
    }

    // Tasks are ALWAYS all-day-event!
    vo->addProperty(TEXT("X-FUNAMBOL-ALLDAY"), TEXT("1"));

    if (getProperty(L"Subject", element)) {
        vo->addProperty(TEXT("SUMMARY"), element.c_str());        
    }
    if (getProperty(L"Body", element)) {
        vo->addProperty(TEXT("DESCRIPTION"), element.c_str());                
    }

    if (getProperty(L"DateCompleted", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T000000");
        //}
        vo->addProperty(TEXT("COMPLETED"), element.c_str());        
    }
    if (getProperty(L"DueDate", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T235900");
        //}
        vo->addProperty(TEXT("DUE"), element.c_str());        
    }
    if (getProperty(L"StartDate", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T000000");
        //}
        vo->addProperty(TEXT("DTSTART"), element.c_str());        
    }

    if (getProperty(L"Categories", element)) {
        vo->addProperty(TEXT("CATEGORIES"), element.c_str());        
    }

    //
    // STATUS: if "Complete" is set, we set it to COMPLETED and ignore the Status value (that should be = 2 = winTaskComplete)
    // 
    if (getProperty(L"Complete", element)) {
        bool isCompleted = (element != TEXT("0"));
        if (isCompleted) { 
            statusIsSet = true;
            vo->addProperty(TEXT("STATUS"), TEXT("COMPLETED"));            
        }
    }
    if (!statusIsSet && getProperty(L"Status", element)) {
        // Map the status field (Client -> Server)
        // 
        // Outlook    | SIF |  vCalendar     
        // -------------------------------------------------
        // NotStarted |  0  |  ACCEPTED     (vCal 1.0 & 2.0)  
        // InProgress |  1  |  CONFIRMED    (vCal 1.0)
        // Complete   |  2  |  COMPLETED    (vCal 1.0 & 2.0)
        // Waiting    |  3  |  NEEDS ACTION (vCal 1.0)
        // Deferred   |  4  |  DECLINED     (vCal 1.0 & 2.0)
        // 
        int status = _wtoi(element.c_str());
        wstring value(TEXT("ACCEPTED"));        

        if      (status == winTaskNotStarted) value = TEXT("ACCEPTED");
        else if (status == winTaskInProgress) value = TEXT("CONFIRMED");
        else if (status == winTaskComplete)   value = TEXT("COMPLETED");
        else if (status == winTaskWaiting)    value = TEXT("NEEDS ACTION");
        else if (status == winTaskDeferred)   value = TEXT("DECLINED");

        vo->addProperty(TEXT("STATUS"), value.c_str());            
    }


    if (getProperty(L"Importance", element)) {
        // PRIORITY:1 in vCal = High, subsequent numbers specify a decreasing ordinal priority.
        // We set: 1=High, 2=normal, 3=low
        int importance = _wtoi(element.c_str());
        wstring value;
        if (importance == winImportanceHigh) {
            value = TEXT("1");
        } else if (importance == winImportanceLow) {
            value = TEXT("3");
        } else {  
            value = TEXT("2");
        }
        vo->addProperty(TEXT("PRIORITY"), value.c_str()); 
    }
    if (getProperty(L"PercentComplete", element)) {
        vo->addProperty(TEXT("PERCENT-COMPLETE"), element.c_str()); 
    }
    if (getProperty(L"Sensitivity", element)) {
        long sensitivity = _wtoi(element.c_str());
        wstring value;
        if(sensitivity == winPrivate) {
            value = TEXT("PRIVATE");
        }
        else if (sensitivity == winConfidential) {
            value = TEXT("CONFIDENTIAL");
        }
        else {  // default value
            value = TEXT("PUBLIC");
        }
        vo->addProperty(TEXT("CLASS"), value.c_str());        
    }

    //
    // **** "TeamTask"? mapping is missing! ****
    //

    //
    // Recurrence pattern -> RRULE
    //
    if (getProperty(L"IsRecurring", element)) {
        bool isRec = (element != TEXT("0"));
        if(isRec) {
            wstring rRule = recPattern.toString();
            if(rRule != L"") {
                vo->addProperty(TEXT("RRULE"), rRule.c_str());                
            }
        }
        else {
            // Not recurring: send empty "RRULE:"
            vo->addProperty(TEXT("RRULE"), NULL); 
            /*vp = new VProperty(TEXT("RRULE"));
            vo->addProperty(vp);
            delete vp; vp = NULL;*/
        }
    }


    //
    // ReminderSet
    //
    if (getProperty(L"ReminderSet", element)) {
        bool bReminder = (element != TEXT("0"));
        if(bReminder == true) {
            if (getProperty(L"ReminderTime", element)) {

                vp = new VProperty(L"AALARM");
                vp->addValue(element.c_str());                      // "RunTime"
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
            vo->addProperty(TEXT("AALARM"), NULL); 
            /*vp = new VProperty(L"AALARM");
            vo->addProperty(vp);
            delete vp; vp = NULL;*/
        }
    }


    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (getProperty(TEXT("ActualWork"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-ACTUALWORK"), element.c_str());           
    }
    if (getProperty(TEXT("BillingInformation"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-BILLINGINFO"), element.c_str()); 
    }
    if (getProperty(TEXT("Companies"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-COMPANIES"), element.c_str());        
    }
    if (getProperty(TEXT("Mileage"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-MILEAGE"), element.c_str()); 
    }
    if (getProperty(L"ReminderOptions", element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-AALARMOPTIONS"), element.c_str());
    }
    if (getProperty(TEXT("TeamTask"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-TEAMTASK"), element.c_str());
    }
    if (getProperty(TEXT("TotalWork"), element)) {
        vo->addProperty(TEXT("X-FUNAMBOL-TOTALWORK"), element.c_str());
    }

    vo->addProperty(TEXT("END"), TEXT("VTODO"));
    vo->addProperty(TEXT("END"), TEXT("VCALENDAR"));

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




//
// Parse a vCalendar string and fills the propertyMap.
//
int WinTask::parse(const wstring & dataString) {

    WCHAR* element = NULL;
    DATE startDate = NULL;

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
    // Conversion: vObject -> WinTask.
    // -------------------------------
    // Note: properties found are added to the propertyMap, so that the 
    //       map will contain only parsed properties after this process.
    //
    if (element = getVObjectPropertyValue(vo, L"SUMMARY")) {
        setProperty(L"Subject", element);
    }
    if (element = getVObjectPropertyValue(vo, L"DESCRIPTION")) {
        setProperty(L"Body", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-FOLDER")) {
        setProperty(L"Folder", element);
    }

    // Tasks are ALWAYS all-day-events!
    setProperty(L"AllDayEvent", TEXT("1"));

    if (element = getVObjectPropertyValue(vo, L"DTSTART")){
        setProperty(L"StartDate", element);
        stringTimeToDouble(element, &startDate);            // 'startDate' will be used also for RRULE parsing
    }
    if (element = getVObjectPropertyValue(vo, L"DUE")){
        setProperty(L"DueDate", element);
    }
    if (element = getVObjectPropertyValue(vo, L"COMPLETED")){
        setProperty(L"DateCompleted", element);
    }
    if (element = getVObjectPropertyValue(vo, L"CATEGORIES")) {
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
    if (element = getVObjectPropertyValue(vo, L"PERCENT-COMPLETE")) {
        setProperty(L"PercentComplete", element);
    }
    if (element = getVObjectPropertyValue(vo, L"PRIORITY")) {
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
    if (element = getVObjectPropertyValue(vo, L"STATUS")) {
        // Map the status field.
        // 
        // vCalendar                        | SIF |  Outlook      
        // ------------------------------------------------------    
        // ACCEPTED      (vCal 1.0 & 2.0)   |  0  |  NotStarted  
        // SENT          (vCal 1.0)         |  0  |  NotStarted        
        // TENTATIVE     (vCal 1.0 & 2.0)   |  0  |  NotStarted     
        // IN-PROCESS    (vCal 2.0)         |  1  |  InProgress        
        // CONFIRMED     (vCal 1.0)         |  1  |  InProgress        
        // COMPLETED     (vCal 1.0 & 2.0)   |  2  |  Complete          
        // NEEDS-ACTION  (vCal 2.0)         |  3  |  Waiting
        // NEEDS ACTION  (vCal 1.0)         |  3  |  Waiting          
        // DELEGATED     (vCal 1.0 & 2.0)   |  3  |  Waiting        
        // DECLINED      (vCal 1.0 & 2.0)   |  4  |  Deferred  
        //
		WinTaskStatus status = winTaskNotStarted;
        if ( !wcscmp(element, TEXT("COMPLETED")) ) {
            setProperty(L"Complete", TEXT("1"));
            status = winTaskComplete;
        }
        else if ( !wcscmp(element, TEXT("ACCEPTED")) || 
			      !wcscmp(element, TEXT("SENT"))     ||
				  !wcscmp(element, TEXT("TENTATIVE")) ) {
			status = winTaskNotStarted;
		}
        else if ( !wcscmp(element, TEXT("IN-PROCESS")) ||
			      !wcscmp(element, TEXT("CONFIRMED")) ) {
			status = winTaskInProgress;
		}
		else if ( !wcscmp(element, TEXT("NEEDS-ACTION")) ||
		          !wcscmp(element, TEXT("NEEDS ACTION")) ||
			      !wcscmp(element, TEXT("DELEGATED")) ) {
			status = winTaskWaiting;
		}
		else if (!wcscmp(element, TEXT("DECLINED"))) {
			status = winTaskDeferred;
		}
		setIntProperty(L"Status", status);
    }

    //
    // **** "TeamTask"? mapping is missing! ****
    //

    //
    // AALARM
    // The value consists of: RunTime, SnoozeTime, RepeatCount, AudioContent
    //
    if(element = getVObjectPropertyValue(vo, L"AALARM")) {
        WCHAR* runTimeValue = vo->getProperty(TEXT("AALARM"))->getPropComponent(1);
        if (wcslen(runTimeValue) > 0) {
            setProperty(TEXT("ReminderSet"),  TEXT("1"));
            setProperty(TEXT("ReminderTime"), runTimeValue);

            // Reminder sound file path
            WCHAR* filePath = vo->getProperty(TEXT("AALARM"))->getPropComponent(4);
            if (filePath && wcslen(filePath)>0) {
                setProperty(TEXT("ReminderSoundFile"), filePath);
            }
            else {
                setProperty(TEXT("ReminderSoundFile"), TEXT(""));
            }
        }
        else {
            // RunTime not found -> no reminder
            setProperty(TEXT("ReminderSet"), TEXT("0"));
        }
    }
    else {
        // AALARM not found -> reset reminder!
        // Note: this is done for compatibility with most devices: if alarm not set
        //       AALARM property is not sent.
        setProperty(TEXT("ReminderSet"), TEXT("0"));
    }


    //
    // RRULE -> Recurrence pattern
    // Fill recPattern propertyMap.
    //
    if ( (element = getVObjectPropertyValue(vo, L"RRULE")) && 
         (wcslen(element) > 0) ) {
        setProperty(L"IsRecurring", L"1");
        recPattern.setStartDate(startDate);
        recPattern.parse(element);
    }
    else {
        // Not recurring.
        setProperty(L"IsRecurring", L"0");
    }



    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-ACTUALWORK"))) {
        setProperty(TEXT("ActualWork"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-BILLINGINFO"))) {
        setProperty(TEXT("BillingInformation"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-COMPANIES"))) {
        setProperty(TEXT("Companies"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-MILEAGE"))) {
        setProperty(TEXT("Mileage"), element);
    }
    if(element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-AALARMOPTIONS")) {
        setProperty(L"ReminderOptions", element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-TEAMTASK"))) {
        setProperty(TEXT("TeamTask"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-TOTALWORK"))) {
        setProperty(TEXT("TotalWork"), element);
    }
    
    if (vo) { delete vo; vo = NULL; }

    return 0;
}



// Utility to check the productID and version of VObject passed.
bool WinTask::checkVCalendarTypeAndVersion(VObject* vo) {

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



WinRecurrence* WinTask::getRecPattern() {
    return &recPattern;
}


long WinTask::getCRC() {

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
            it = getRecPattern()->propertyMap.begin();
            while (it != getRecPattern()->propertyMap.end()) {
                values.append(it->second);
                it ++;
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
