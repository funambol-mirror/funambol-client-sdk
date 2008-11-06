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


#ifndef INCL_ICALENDAR_PROPERTY
#define INCL_ICALENDAR_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class iCalProperty : public ArrayElement {

private:
    WCHAR* altre;         //Alternate text representation
    WCHAR* cn;            //Common name
    WCHAR* cutype;        //Calendar user type
    WCHAR* delegatedFrom; //Delegator
    WCHAR* delegatedTo;   //Delegatee
    WCHAR* dir;           //Directory entry
    WCHAR* encoding;      //Inline encoding
    WCHAR* formattype;    //Format type
    WCHAR* fbtype;        //free-busy type
    WCHAR* language;      //Language for text
    WCHAR* member;        //Group or list membership
    WCHAR* partstat;      //Participation status
    WCHAR* range;         //Recurrence identifier range
    WCHAR* trigrel;       //Alarm trigger relationship
    WCHAR* related;       //Relationship type
    WCHAR* role;          //Participation role
    WCHAR* rsvp;          //RSVP expectation
    WCHAR* sentby;        //Sent by
    WCHAR* tzid;          //Reference to time zone object
    WCHAR* valuetype;     //Property value data type
    WCHAR* value;         //the value of property
    ArrayList* xParams;

    void set(WCHAR** property, WCHAR* v);

public:
    iCalProperty (WCHAR* v = NULL);
    ~iCalProperty();

    // ---------------------------------------------------------- Public methods

    WCHAR* getAltre (WCHAR* buf = NULL, int size = -1);
    WCHAR* getCn (WCHAR* buf = NULL, int size = -1);
    WCHAR* getCutype (WCHAR* buf = NULL, int size = -1);
    WCHAR* getDelegatedFrom (WCHAR* buf = NULL, int size = -1);
    WCHAR* getDelegatedTo (WCHAR* buf = NULL, int size = -1);
    WCHAR* getDir (WCHAR* buf = NULL, int size = -1);
    WCHAR* getEncoding (WCHAR* buf = NULL, int size = -1);
    WCHAR* getFormatType (WCHAR* buf = NULL, int size = -1);
    WCHAR* getFbType (WCHAR* buf = NULL, int size = -1);
    WCHAR* getLanguage (WCHAR* buf = NULL, int size = -1);
    WCHAR* getMember (WCHAR* buf = NULL, int size = -1);
    WCHAR* getPartStat (WCHAR* buf = NULL, int size = -1);
    WCHAR* getRange (WCHAR* buf = NULL, int size = -1);
    WCHAR* getTrigRel (WCHAR* buf = NULL, int size = -1);
    WCHAR* getRelated (WCHAR* buf = NULL, int size = -1);
    WCHAR* getRole (WCHAR* buf = NULL, int size = -1);
    WCHAR* getRsvp (WCHAR* buf = NULL, int size = -1);
    WCHAR* getSentBy (WCHAR* buf = NULL, int size = -1);
    WCHAR* getTzID (WCHAR* buf = NULL, int size = -1);
    WCHAR* getValueType (WCHAR* buf = NULL, int size = -1);
    WCHAR* getValue (WCHAR* buf = NULL, int size = -1);
    ArrayList* getXParam();

    void setAltre (WCHAR* v);
    void setCn (WCHAR* v);
    void setCutype (WCHAR* v);
    void setDelegatedFrom (WCHAR* v);
    void setDelegatedTo (WCHAR* v);
    void setDir (WCHAR* v);
    void setEncoding (WCHAR* v);
    void setFormatType (WCHAR* v);
    void setFbType (WCHAR* v);
    void setLanguage (WCHAR* v);
    void setMember (WCHAR* v);
    void setPartStat (WCHAR* v);
    void setRange (WCHAR* v);
    void setTrigRel (WCHAR* v);
    void setRelated (WCHAR* v);
    void setRole (WCHAR* v);
    void setRsvp (WCHAR* v);
    void setSentBy (WCHAR* v);
    void setTzID (WCHAR* v);
    void setValueType (WCHAR* v);
    void setValue (WCHAR* v);
    void setXParam(ArrayList& list);

    ArrayElement* clone();
};

/** @endcond */
#endif
