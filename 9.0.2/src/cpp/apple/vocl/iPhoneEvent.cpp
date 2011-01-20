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
#include "vocl/iPhoneEvent.h"
#include "vocl/TimeUtils.h"
#include "vocl/VConverter.h"
#include "base/globalsdef.h"

BEGIN_FUNAMBOL_NAMESPACE


// Constructor
iPhoneEvent::iPhoneEvent() : AppleEvent() {
}

// Constructor: fills propertyMap parsing the passed data
iPhoneEvent::iPhoneEvent(const StringBuffer & dataString) {
    parse(dataString);
}

// Destructor
iPhoneEvent::~iPhoneEvent() {
}

//
// Parse a vCalendar string and fills the propertyMap.
//
#pragma mark Parsing vCalendar
int iPhoneEvent::parse(const StringBuffer& dataString) {
    
	char* element = NULL;
	
	StringBuffer startDateValue;
	StringBuffer endDateValue;
	NSDate* sdate;
	NSDate* edate;
	
	
	VObject* vo = VConverter::parse(dataString.c_str());
	if (!vo) {        
		LOG.error("Error in parsing event");
		return 1;
	}       
	
	// Check if VObject type and version are the correct ones.
    if (!checkVCalendarTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }
	
	// TIMEZONE HANDLING 
	// parsing the timezone anyway it returns the current default timezone even if it is not set
	Timezone tz(vo);
	setEventTimezone(tz);
	
	// 
	// DTSTART, DTEND:
	// Set the start and end date. If the start is 00:00 and end is 23:59 the appointment is decided to be
	// an all day event. So the AllDayEvent property is set to '1' in the propertyMap.
	//
	if(element = getVObjectPropertyValue(vo, "DTSTART")){
		LOG.debug("the element in the parser is %s", element);     
		startDateValue = element;
		sdate = stringToNSdate(element, tz);
		setStart(sdate);
		
	}
    
	if(element = getVObjectPropertyValue(vo, "DTEND")) {
		endDateValue = element;
		edate = stringToNSdate(element, tz);
		setEnd(edate);
	}
	
	// ALL-DAY EVENT
	if (startDateValue.length() > 0 && endDateValue.length() > 0) {
        bool isAllDay = false;
        
		if (element = getVObjectPropertyValue(vo, "X-FUNAMBOL-ALLDAY")) {
			isAllDay = ((strcmp(element, "1") == 0) || (strcmp(element, "TRUE") == 0)) ? true : false;
		}      
		
        if (!isAllDay) {
			// All-day check #2: interval [00:00 - 23:59]
			isAllDay = isAllDayInterval(sdate, edate);
		}
        
		if (!isAllDay) {
			// All-day check #3: format "yyyyMMdd"
			if (startDateValue.length() == 8 && endDateValue.length() == 8 ) {
				isAllDay = true;
			}
		}

		if (isAllDay) {			
			// Safe check on endDate: min value is 'startDate + 1 min'
			if (edate <= sdate) {
				NSDate* tmp = addComponentsToDate(sdate, tz, 0, 0, 0, 0, 1, 0);
				edate = tmp;
			}
			
			// for EndDates like "20071121T235900": Apple need we convert into "20071121T235959"
			edate = normalizeEndDayForAllDay(edate, tz);
			sdate = resetHoursMinSecs(sdate, tz);
			
			setStart(sdate);
			setEnd(edate);
		}
		
        setAllDayEvent(isAllDay);               
	}        
    
	if(element = getVObjectPropertyValue(vo, "SUMMARY")) {        
		setTitle(element);
	}
	if(element = getVObjectPropertyValue(vo, "LOCATION")) {
		setLocation(element);
	}    
	if(element = getVObjectPropertyValue(vo, "DESCRIPTION")) {
		setNote(element);
	}                     
	if(element = getVObjectPropertyValue(vo, "CATEGORIES")) {
		setCategories(element);
	}
	if(element = getVObjectPropertyValue(vo, "PRIORITY")) {        
		Importance imp = ImportanceNormal;
		if (!strcmp(element, "0") ) {
			imp =  ImportanceLow; 
		}
		else if (!strcmp(element, "2") ) {
			imp = ImportanceHigh;
		}                
		setImportance(imp);
	}
	if (element = getVObjectPropertyValue(vo, "CLASS")) {
		Sensitivity s = NormalEvt;
		if (!strcmp(element, "CONFIDENTIAL") ) {
			s = ConfidentialEvt; 
		}
		else if (!strcmp(element, "PRIVATE") ) {
			s = PrivateEvt;
		}
		setSensitivity(s);
	}
	if(element = getVObjectPropertyValue(vo, "X-MICROSOFT-CDO-BUSYSTATUS")) {
		BusyStatus busy = Free;
		if (!strcmp(element, "1") ) {
			busy = Tentative; 
		}
		else if (!strcmp(element, "2") ) {
			busy = Busy;
		} 
		else if (!strcmp(element, "3") ) {
			busy = OutOfOffice;
		}                
		setBusyStatus(busy);        
	}
	
	if ((element = getVObjectPropertyValue(vo, "RRULE")) && (strlen(element) > 0)) {
		
		setIsRecurring(true);    
		/*
         RecurrenceDataConverter rdc;
         Recurrence* rec = rdc.parse(element, event->getStart());
         
         if (rec) {
         event->setRecurrence(rec); 
         } else {
         LOG.error("EventDataConverter::parse - Error creating the recurrence object. It is not added to the event");
         }    
         
         // @todo: handle the exception dates
         /*
		 StringBuffer timezone = parseTimezone(vo);
		 if (timezone != "") {
		 Timezone tz;
		 tz.setName(timezone);
		 event->setEventTimezone(tz);
		 }
		 */ 
		
	}
	else {
		// Not recurring.
		setIsRecurring(false);
	}
    
	if(element = getVObjectPropertyValue(vo, "AALARM")) {
		char* runTimeValue = vo->getProperty("AALARM")->getPropComponent(1);
		if ((runTimeValue != NULL) && (strlen(runTimeValue) > 0)) {
			setReminder(true);
			NSDate* date = stringToNSdate(runTimeValue, tz);
			setAlarm1(date);
		} else {
			setReminder(false);
		}
		
	}
    
	if (vo) { delete vo; vo = NULL; }	
    
    return 0;
}


// Utility to check the productID and version of VObject passed.
bool iPhoneEvent::checkVCalendarTypeAndVersion(VObject* vo) {
    
    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    
    if (!prodID) {
        LOG.error(ERR_ITEM_VOBJ_TYPE_NOTFOUND, "VCALENDAR");
        return false;
    }
    if (strcmp(prodID, "VCALENDAR")) {
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

void iPhoneEvent::addTimezone(VObject* vo) {
	
    // TODO: it uses the Timezone object to retrieve the part needed to be put in
    // the VObject
	
	/*
	 VProperty* vp  = NULL;
	 wstring element;
	 
	 //
	 // TZ: "signed numeric indicating the number of hours and possibly minutes from UTC."
	 // TZ = - (Bias + StandardBias)     [StandardBias is usually = 0]
	 //
	 wstring bias = formatBias(tzInfo.Bias + tzInfo.StandardBias);
	 vo->addProperty(TEXT("TZ"), bias.c_str());   
	 
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
	 vo->addProperty(TEXT("DAYLIGHT"), TEXT("FALSE"));        
	 }
	 */
}


bool iPhoneEvent::parseTimezone(VObject* vo) {
    // it has to be moved into the parse in the Timezone object	
    /*
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
     */
    return true;
    
}


long iPhoneEvent::getCRC() {
    /*
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
     */
	return 0;
    
}

#pragma mark Format object to vCalendar
// Format and return a vCalendar string from the propertyMap.
StringBuffer iPhoneEvent::toString() {
	
    StringBuffer vCalendar("");
	
    //
    // Conversion: WinEvent -> vObject.
    // --------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    
    vo->addProperty("BEGIN", "VCALENDAR");
    vo->addProperty("VERSION", VCALENDAR_VERSION);
	
    // TIMEZONE: placed out of VEVENT. 
    // Adding it only if the event is recurring.
    if (getIsRecurring()) { 
        addTimezone(vo);
    }
	
    vo->addProperty("BEGIN", "VEVENT");
	
	// AllDayEvent
	vo->addProperty("X-FUNAMBOL-ALLDAY", getAllDayEvent() ? "TRUE" : "FALSE");          
    
	// start date
	// 
	StringBuffer sDate = NSDateToString(getStart(), getEventTimezone(), getAllDayEvent(), getIsRecurring());
	vo->addProperty(TEXT("DTSTART"), sDate.c_str());         
    
	// end date
    NSDate* tmpEnd = getEnd();
    if (getAllDayEvent()) {
        tmpEnd = normalizeEndDayForAllDayToString(getEnd(), getEventTimezone());
    }
	StringBuffer eDate = NSDateToString(tmpEnd, getEventTimezone(), getAllDayEvent(), getIsRecurring());
	vo->addProperty(TEXT("DTEND"), eDate.c_str());         
	
	// BusyStatus
	BusyStatus b = getBusyStatus();
	StringBuffer value("FREE");
	if (b == Tentative) {
		value = "TENTATIVE";
	} else if (b == Busy) {
		value = "BUSY";
	} else if (b == OutOfOffice) {
		value = "OOF";
	}
	vo->addProperty("X-MICROSOFT-CDO-BUSYSTATUS", value.c_str());
	
	// Categories
	vo->addProperty("CATEGORIES", getCategories());
	
	// Note
	vo->addProperty("DESCRIPTION", getNote());
	
	// Location
	vo->addProperty("LOCATION", getLocation());         
    
	// Importance
	Importance imp = getImportance();
	value = "2";
	if (imp == ImportanceHigh ) {
		value = "1";
	} else if (imp = ImportanceLow) {
		value = "3";
	}
	vo->addProperty("PRIORITY", value.c_str());
	
	// Title
	vo->addProperty("SUMMARY", getTitle());  
    
	// Sensitivity
	Sensitivity sen = getSensitivity();
	value = "PUBLIC";
	if (sen == PrivateEvt) {
		value = "PRIVATE";
	} else if (sen == ConfidentialEvt) {
		value = "CONFIDENTIAL";
	} 
	vo->addProperty("CLASS", value.c_str());   
	
    //
    // ReminderSet
    //
    if (getReminder()) {		
		
		StringBuffer alarmDate = NSDateToString(getAlarm1(), getEventTimezone(), false, false);
		// TODO: complete the alarm
		vp = new VProperty("AALARM");
		vp->addValue(alarmDate.c_str());              // "RunTime"
		vp->addValue("");                             // "Snooze Time" (empty)
		vp->addValue("0");                            // "Repeat Count"
		vp->addValue("");                             // "Audio Content" = sound file path
		vo->addProperty(vp);
		delete vp; vp = NULL;
	}
	else {
		// No reminder: send empty "AALARM:"
		vo->addProperty(TEXT("AALARM"), NULL);
	}    
	
	// TODO: implementing recurrence rules
	/*
     if (getIsRecurring()) {
     //
     // Recurrence pattern -> RRULE
     //
     
     wstring rRule = recPattern.toString();
     if(rRule != "") {
     vo->addProperty("RRULE", rRule.c_str());             
     }
     
     // Exceptions: EXDATE
     vp = new VProperty("EXDATE");
     for (it  = excludeDate.begin(); it != excludeDate.end(); it++) {
     wstring date = (*it);
     vp->addValue(date.c_str());
     }
     vo->addProperty(vp);
     delete vp; vp = NULL;
     
     // Exceptions: RDATE (should be empty for Outlook and WM)
     vp = new VProperty("RDATE");
     vo->addProperty(vp);
     delete vp; vp = NULL;
     
     }
     else {
     // Not recurring: send empty "RRULE:"
     vo->addProperty("RRULE", NULL);
     }
     */
    vo->addProperty("END", "VEVENT");         
    vo->addProperty("END", "VCALENDAR");    
	
	
    //
    // Format the vCalendar.
    // ---------------------
    //
    char* tmp = vo->toString();
    if (tmp) {
        vCalendar = tmp;
        delete [] tmp;
    }
    if (vo) { delete vo; vo = NULL; }
    return vCalendar;
}

END_FUNAMBOL_NAMESPACE






