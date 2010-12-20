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

#include "vocl/WinRecipient.h"
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;


// Constructor
WinRecipient::WinRecipient() {
}

// Constructor: fills propertyMap parsing the ATTENDEE
WinRecipient::WinRecipient(const wstring & attendee) {
    parse(attendee);
}

// Destructor
WinRecipient::~WinRecipient() {
}



// Parse a ATTENDEE string and fills the propertyMap.
int WinRecipient::parse(const wstring & attendee) {
    //"ATTENDEE;ROLE=ATTENDEE;STATUS=NEEDS ACTION: <email@example.com>"

    wstring::size_type pos1 = attendee.find(L":", 0);
    wstring value;
    if (pos1 == wstring::npos)
        value = attendee;
    else
        value = attendee.substr(pos1+1, attendee.length()-pos1-1);

    wstring::size_type em1 = value.find_last_of(L"<", 0);
    if (em1 == wstring::npos)
    {
        em1 = value.find(L"<", 0);
    }
    if (em1 != wstring::npos)
    {
        wstring::size_type em2 = value.find_last_of(L">", 0);
        if (em2 == wstring::npos)
        {
            em2 = value.find(L">", 0);
        }
        if (em2 != wstring::npos)
        {
            setProperty(L"AttendeeEmail", value.substr(em1+1,em2-em1-1).c_str());
        }
        while (em1 > 0 && value[em1-1] == L' ')
        {
            em1--;
        }
        if (em1 > 0)
        {
            setProperty(L"AttendeeName", value.substr(0, em1));
        }
    }

    wstring::size_type pos2 = attendee.find(L";", 0);
    wstring::size_type pos3 = pos2;
    wstring::size_type eq;
    wstring param, pname, pvalue;
    do 
    {
        pos2 = pos3+1;
        pos3 = attendee.find(L";", pos2);
        if (pos3 == wstring::npos)
        {
            param = attendee.substr(pos2, pos1-pos2);
        }
        else
        {
            param = attendee.substr(pos2, pos3-pos2);
        }
        eq = param.find(L"=", 0);
        if (eq != wstring::npos)
        {
            pname = param.substr(0, eq);
            pvalue = param.substr(eq+1);
            if (pname.compare(L"ROLE") == 0)
            {
                setProperty(L"AttendeeRole", pvalue);
            }

            if (pname.compare(L"STATUS") == 0)
            {
                setProperty(L"AttendeeStatus", pvalue);
            }
        }
    }
    while (pos2 != wstring::npos && pos3 < pos1);

    return 0;
}

// Format and return a ATTENDEE string from the propertyMap.
wstring WinRecipient::toString() {
    return L"";
}

int WinRecipient::getNamedEmail(wstring & attendee)
{
    wstring el;
    bool name = false;
    if (getProperty(L"AttendeeName", el))
    {
        name = true;
        attendee = el;
    }

    if (getProperty(L"AttendeeEmail", el))
    {
        if (name)
            attendee.append(L" ");
        attendee.append(L"(");
        attendee.append(el);
        attendee.append(L")");
    }

    return 0;
}

bool WinRecipient::toVProperty(VProperty * vp)
{
    if (!vp)
        return false;
    wstring str = L"";
    wstring element = L"";
    bool name = false;
    int lastIndex = 0;

    if (getProperty(L"AttendeeName", element))
    {
        str.append(element);
        name = true;
        lastIndex = str.size();
        if (lastIndex > 0) {
            lastIndex--;
        }
    }

    if (getProperty(L"AttendeeEmail", element))
    {
        if (name && str[lastIndex] != L' ') {
            str.append(L" ");
        }
        str.append(L"<").append(element).append(L">");
    }

    vp->addValue(str.c_str());

    if (getProperty(L"AttendeeStatus", element))
    {
        vp->addParameter(L"STATUS", element.c_str());
    }

    if (getProperty(L"AttendeeRole", element))
    {
        vp->addParameter(L"ROLE", element.c_str());
    }

    return true;
}

