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


#ifndef INCL_ICALENDAR_PROPERTY
#define INCL_ICALENDAR_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif
