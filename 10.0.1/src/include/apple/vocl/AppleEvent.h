/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

#ifndef INCL_APPLE_EVENT
#define INCL_APPLE_EVENT

/** @cond API */
/** @addtogroup apple_adapter */
/** @{ */


#include "base/fscapi.h"
#include "base/Log.h"
#include "vocl/VObject.h"
#include "vocl/Timezone.h"

// Version supported
#define VCALENDAR_VERSION                   TEXT("1.0")

// Error messages:
#define ERR_ITEM_VOBJ_PARSE                 "VConverter: error occurred parsing the item data."
#define ERR_ITEM_VOBJ_WRONG_TYPE            "Error: wrong vobject type \"%ls\" (\"%ls\" expected)"
#define ERR_ITEM_VOBJ_TYPE_NOTFOUND         "Error: vobject type not specified (\"%ls\" expected)"
#define ERR_SIFFIELDS_NULL                  "Parsing error: sifFields must be initialized before parsing data."
#define INFO_ITEM_VOBJ_WRONG_VERSION        "Warning! Wrong vobject version \"%ls\" (\"%ls\" expected)"
#define INFO_ITEM_VOBJ_VERSION_NOTFOUND     "Warning! VObject version not specified (\"%ls\" expected)"

#include "base/globalsdef.h"

BEGIN_FUNAMBOL_NAMESPACE

typedef enum {
    NormalEvt,
    PersonalEvt,
    PrivateEvt,
    ConfidentialEvt    
} Sensitivity; 

typedef enum {
    ImportanceLow,
    ImportanceNormal,
    ImportanceHigh
} Importance;

typedef enum {
    Free,
    Tentative,
    Busy,
    OutOfOffice
} BusyStatus;


/**
 ****************************************************************************
 * Represents an item object for Apple Event.
 ****************************************************************************
 */
class AppleEvent {
    
protected:
    
    /**
     * The start date of the event. Only date year, month, day are used for an all day event
     */
    NSDate* start;
    
    /**
     * The end date of the event. Only date year, month, day are used for an all day event
     */
    NSDate* end;
    
    /**
     * The event is all day (true) or not (false);
     */
    bool allDayEvent;
    
    /**
     * The event is recurring (true) or not (false); TO BE CONFIRMED IF USED
     */    
    bool isRecurring;
    
    /**
     * The title (subject) of the event
     */   
    StringBuffer title;
    
    /**
     * The note (body) of the event
     */   
    StringBuffer note;
    
    /**
     * The location of the event
     */   
    StringBuffer location;
    
    /**
     * It represents the alarms of the event. Only 2 alarms are allowed in the system. Only one is
     * populated by the sync. reminder is a shortcut to understand if an alarm exists
     */ 
	bool       reminder;
    NSDate*    alarmDate1;
    NSDate*    alarmDate2;
    
    // TO BE DEVELOPED
    // Recurrence* recurrence;
    Timezone timezone;
    
    BusyStatus    busyStatus; 
    StringBuffer  categories;   // It is the calendar the event belongs
    Importance    importance;   // Returns or sets the relative importance level for the event.  ImportanceHigh(2), ImportanceLow(0) or ImportanceNormal(1)
    Sensitivity   sensitivity;  // Returns or sets the sensitivity for the event. Confidential(3), Normal(0), Personal(1) or Private(2). 
        
    /// The table used to calculate the crc.
    static unsigned long crc32Table[256];
    
public:
    
    /// Default Constructor
    AppleEvent();
    
    /// Destructor
    virtual ~AppleEvent();
    
    /**
     * Format and return a string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the string formatted, reference to internal wstring
     */
    virtual StringBuffer toString() = 0;
    
    /**
     * Parse a string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCard string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const StringBuffer& dataString) = 0;
    
	/**
     * Utility to safe-retrieve the property value inside VObject 'vo'.
     * @param vo           : VObject to read from
     * @param propertyName : the property name requested
     * @return               the property value (NULL if not found)
     */
    char* getVObjectPropertyValue(VObject* vo, const char* propertyName);
	
    /**
     * Return the crc value of the internal map with all values.
     * It uses only the values of the map not the key.
     * Can be overridden by derived classes if other properties are involved
     * (e.g. Events have recurrence props and exceptions)
     */
    virtual long getCRC();
    
    void setAllDayEvent(bool v)    { allDayEvent = v;    }
    bool getAllDayEvent()  const   { return allDayEvent; }
    
    void setStart(NSDate* d)       {
        [start autorelease];
        start = [d retain];
    }
    NSDate* getStart()   const     { return start; }
    
    void setEnd(NSDate* d)         { 
        [end autorelease];
        end = [d retain];              
    }
    NSDate* getEnd()  const       { return end;   }    
    
    void setIsRecurring(bool v)    { isRecurring = v;      }
    bool getIsRecurring()          { return isRecurring;   }
    
    void setTitle(const char* v)   { title = v;            }
    const char* getTitle() const   { return title;         }        
    
    void setNote(const char* v)    {  note = v;             }
    const char* getNote() const    { return note;          }            
    
    void setLocation(const char* v) { location = v;        }
    const char* getLocation() const { return location;     }            
    
	void setReminder(bool v)        { reminder = v;        }
	bool getReminder()   const      { return reminder;     }
	
    void setAlarm1(NSDate* d)       { 
        [alarmDate1 autorelease];
        alarmDate1 = [d retain];            
    }
    NSDate* getAlarm1()  const      { return alarmDate1;         }
    
    void setAlarm2(NSDate* d)       { 
        [alarmDate2 autorelease];
        alarmDate2 = [d retain];            
    }
    NSDate* getAlarm2()      const  { return alarmDate2;         }
    
    // void setRecurrence(Recurrence* r) { recurrence = r; }
    // Recurrence* getRecurrence() { return recurrence; }      
    
    void setEventTimezone(const Timezone& t)  { timezone = t;          }
    Timezone& getEventTimezone() { return timezone;       }
    
    void setBusyStatus(BusyStatus v) { busyStatus = v; }
    BusyStatus getBusyStatus() { return busyStatus; }
    
    void setImportance(Importance v) { importance = v; }
    Importance getImportance() { return importance; }    
    
    void setSensitivity(Sensitivity v) { sensitivity = v; }
    Sensitivity getSensitivity() { return sensitivity; }    
    
    void setCategories(const char* v) { categories = v; }
    const char* getCategories() const { return categories; }    
};


END_FUNAMBOL_NAMESPACE

/** @} */
/** @endcond */
#endif
