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

#include "vocl/WinContact.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE
using namespace std;


// Constructor
WinContact::WinContact() {
    vCard     = L"";
    photoType = L"";
}

// Constructor: fills propertyMap parsing the passed vCard string
WinContact::WinContact(const wstring dataString) {
    vCard     = L"";
    photoType = L"";
    parse(dataString);
}

// Destructor
WinContact::~WinContact() {
}


//
// Format and return a vCard string from the propertyMap.
//
wstring& WinContact::toString() {

    vCard = L"";

    //
    // Conversion: WinContact -> vObject.
    // ----------------------------------
    //
    VObject* vo = new VObject();
    wstring element;
    VProperty* vp = NULL;
    bool found = false;


    vp = new VProperty(L"BEGIN", L"VCARD");
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(L"VERSION", VCARD_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;
        
    // ------- Name -------
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    found = false;
    vp = new VProperty(L"N");
    if (getProperty(L"LastName",   element))  found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"FirstName",  element))  found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"MiddleName", element))  found = true;  
    vp->addValue(element.c_str());
    if (getProperty(L"Title",      element))  found = true;  
    vp->addValue(element.c_str());
    if (getProperty(L"Suffix",     element))  found = true;  
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    if (getProperty(L"Birthday", element)) {
        vp = new VProperty(L"BDAY", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Body", element)) {
        vp = new VProperty(L"NOTE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"BusinessFaxNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"WORK", NULL);
        vp->addParameter(L"FAX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"BusinessTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vp->addParameter(L"WORK",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Business2TelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vp->addParameter(L"WORK",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"BusinessWebPage", element)) {
        vp = new VProperty(L"URL", element.c_str());
        vp->addParameter(L"WORK",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CarTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"CAR",   NULL);
        vp->addParameter(L"VOICE", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Categories", element)) {
        vp = new VProperty(L"CATEGORIES", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CompanyMainTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"WORK",  NULL);
        vp->addParameter(L"PREF",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"FileAs", element)) {
        vp = new VProperty(L"FN", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    // Mapping is:
    // Email1Address <-> EMAIL;INTERNET:
    // Email2Address <-> EMAIL;INTERNET;HOME:
    // Email3Address <-> EMAIL;INTERNET;WORK:
    if (getProperty(L"Email1Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Email2Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"HOME",     NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Email3Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"WORK",     NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"JobTitle", element)) {
        vp = new VProperty(L"TITLE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"HomeTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vp->addParameter(L"HOME",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Home2TelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vp->addParameter(L"HOME",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"HomeFaxNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"HOME", NULL);
        vp->addParameter(L"FAX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"HomeWebPage", element)) {
        vp = new VProperty(L"URL", element.c_str());
        vp->addParameter(L"HOME", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Importance", element)) {
        vp = new VProperty(L"PRIORITY", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"MobileTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"CELL", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"NickName", element)) {
        vp = new VProperty(L"NICKNAME", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"OtherFaxNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"FAX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"OtherTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"PagerNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"PAGER", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"PrimaryTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"PREF",  NULL);
        vp->addParameter(L"VOICE", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Profession", element)) {
        vp = new VProperty(L"ROLE", element.c_str());
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
    if (getProperty(L"WebPage", element)) {
        vp = new VProperty(L"URL", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    // ----- ORG -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    found = false;
    vp = new VProperty(L"ORG");
    if (getProperty(L"CompanyName",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"Department",     element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OfficeLocation", element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    // ----- Address HOME -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    vp->addParameter(L"HOME", NULL);
    if (getProperty(L"HomeAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    // ----- Address -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    if (getProperty(L"OtherAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    // ----- Address WORK -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    vp->addParameter(L"WORK", NULL);
    if (getProperty(L"BusinessAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;
    
     //PHOTO
    if (getProperty(L"Photo", element)) {
        vp = new VProperty(L"PHOTO", element.c_str());
        if (element != L"") {
            vp->addParameter(L"CONTENT-VALUE", L"UNCHANGED");        
            vp->addParameter(L"ENCODING", L"b");
            if (photoType.length() > 0) {
                vp->addParameter(L"TYPE", photoType.c_str());
            }
        }
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }    

    //
    // ---- Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCard.
    if (getProperty(L"Anniversary", element)) {
        vp = new VProperty(L"X-FUNAMBOL-ANNIVERSARY");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(TEXT("BillingInformation"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-BILLINGINFO"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CallbackTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"X-FUNAMBOL-CALLBACK",  NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Children", element)) {
        vp = new VProperty(L"X-FUNAMBOL-CHILDREN", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Companies", element)) {
        vp = new VProperty(L"X-FUNAMBOL-COMPANIES", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CustomerID", element)) {
        vp = new VProperty(L"X-FUNAMBOL-CUSTOMERID", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Folder", element)) {
        vp = new VProperty(L"X-FUNAMBOL-FOLDER");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Gender", element)) {
        vp = new VProperty(L"X-FUNAMBOL-GENDER", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"GovernmentIDNumber", element)) {
        vp = new VProperty(L"X-FUNAMBOL-GOVERNMENTID", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Hobby", element)) {
        vp = new VProperty(L"X-FUNAMBOL-HOBBIES", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"IMAddress", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"HOME",     NULL);
        vp->addParameter(L"X-FUNAMBOL-INSTANTMESSENGER", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"IM2Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"HOME",     NULL);
        vp->addParameter(L"X-FUNAMBOL-INSTANTMESSENGER", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"IM3Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"HOME",     NULL);
        vp->addParameter(L"X-FUNAMBOL-INSTANTMESSENGER", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Initials", element)) {
        vp = new VProperty(L"X-FUNAMBOL-INITIALS", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"Language", element)) {
        vp = new VProperty(L"X-FUNAMBOL-LANGUAGES", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"ManagerName", element)) {
        vp = new VProperty(L"X-FUNAMBOL-MANAGER", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Mileage", element)) {
        vp = new VProperty(L"X-FUNAMBOL-MILEAGE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"OrganizationalIDNumber", element)) {
        vp = new VProperty(L"X-FUNAMBOL-ORGANIZATIONALID", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"RadioTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"X-FUNAMBOL-RADIO", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Spouse", element)) {
        vp = new VProperty(L"X-FUNAMBOL-SPOUSE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Subject", element)) {
        vp = new VProperty(L"X-FUNAMBOL-SUBJECT", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"TelexNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"X-FUNAMBOL-TELEX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"YomiCompanyName", element)) {
        vp = new VProperty(L"X-FUNAMBOL-YOMICOMPANYNAME", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"YomiFirstName", element)) {
        vp = new VProperty(L"X-FUNAMBOL-YOMIFIRSTNAME", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"YomiLastName", element)) {
        vp = new VProperty(L"X-FUNAMBOL-YOMILASTNAME", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    vp = new VProperty(L"END", L"VCARD");
    vo->addProperty(vp);
    delete vp; vp = NULL;


    //
    // Format the vCard.
    // -----------------
    //
    WCHAR* tmp = vo->toString();
    if (tmp) {
        vCard = tmp;
        delete [] tmp;
    }
    return vCard;
}



//
// Parse a vCard string and fills the propertyMap.
//
int WinContact::parse(const wstring dataString) {

	int businessTel  = 0;
	int homeTel		 = 0;
    int imAddress    = 0;
	WCHAR* name      = NULL;
    WCHAR* element   = NULL;


    //
    // Parse the vCard and fill the VObject.
    // -------------------------------------
    //
    VObject* vo = VConverter::parse(dataString.c_str());
    if (!vo) {
        //sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        setError(1, ERR_ITEM_VOBJ_PARSE);
        LOG.error("%s", getLastErrorMsg());
        return 1;
    }
    // Check if VObject type and version are the correct ones.
    if (!checkVCardTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }


    //
    // Conversion: vObject -> WinContact.
    // ----------------------------------
    // Note: only properties found are added to the propertyMap, so that the 
    //       map will contain only properties effectively received.
    //
    propertyMap.clear();
    for(int i=0; i < vo->propertiesCount(); i++) {

        VProperty* vp = vo->getProperty(i);
		name    = vp->getName();
        element = vp->getValue(0);      // the first value of the property.


		if(!wcscmp(name, L"N")) {
			if(element = vp->getPropComponent(1))  setProperty(L"LastName",   element);
			if(element = vp->getPropComponent(2))  setProperty(L"FirstName",  element);
			if(element = vp->getPropComponent(3))  setProperty(L"MiddleName", element);
			if(element = vp->getPropComponent(4))  setProperty(L"Title",      element);
			if(element = vp->getPropComponent(5))  setProperty(L"Suffix",     element);
		}
        else if(!wcscmp(name, L"BDAY")) {
			setProperty(L"Birthday", element);
        }
        else if(!wcscmp(name, L"NOTE")) {
			setProperty(L"Body", element);
        }
        else if(!wcscmp(name, L"CLASS")) {
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
        else if(!wcscmp(name, L"CATEGORIES")) {
			setProperty(L"Categories", element);
        }
		else if(!wcscmp(name, L"EMAIL")) {
            // Mapping is:
            // Email1Address <-> EMAIL;INTERNET:
            // Email2Address <-> EMAIL;INTERNET;HOME:
            // Email3Address <-> EMAIL;INTERNET;WORK:
			if (vp->containsParameter(L"HOME")) {
                if (vp->containsParameter(L"X-FUNAMBOL-INSTANTMESSENGER")) {
                    if (imAddress == 0) {
                        setProperty(L"IMAddress", element);
                        imAddress ++;
                    }
                    else if (imAddress == 1) {
                        setProperty(L"IM2Address", element);
                        imAddress ++;
                    }
                    else {
                        setProperty(L"IM3Address", element);
                    }
                }
                else {
				    setProperty(L"Email2Address", element);
                    // TODO: check the parameter value (not used now, always SMTP)
                    if (wcslen(element) > 0) { setProperty(L"Email2AddressType", L"SMTP"); }
                    else                     { setProperty(L"Email2AddressType", L""); }
                }
			}
			else if (vp->containsParameter(L"WORK")) {
				setProperty(L"Email3Address", element);
                // TODO: check the parameter value (not used now, always SMTP)
                if (wcslen(element) > 0) { setProperty(L"Email3AddressType", L"SMTP"); }
                else                     { setProperty(L"Email3AddressType", L""); }
			}
            else {
				setProperty(L"Email1Address", element);
                // TODO: check the parameter value (not used now, always SMTP)
                if (wcslen(element) > 0) { setProperty(L"Email1AddressType", L"SMTP"); }
                else                     { setProperty(L"Email1AddressType", L""); }
            }
		}
        else if(!wcscmp(name, L"FN")) {
			setProperty(L"FileAs", element);
        }
        else if(!wcscmp(name, L"NICKNAME")) {
			setProperty(L"NickName", element);
        }
        else if(!wcscmp(name, L"PRIORITY")) {
			setProperty(L"Importance", element);
        }
		else if(!wcscmp(name, L"ORG")) {
            if(element = vp->getPropComponent(1))  setProperty(L"CompanyName",    element);
			if(element = vp->getPropComponent(2))  setProperty(L"Department",     element);
            if(element = vp->getPropComponent(3))  setProperty(L"OfficeLocation", element);
		}
        else if(!wcscmp(name, L"ROLE")) {
			setProperty(L"Profession", element);
        }

        //
        // ---- Telephone fields ----
        //
        else if(!wcscmp(name, L"TEL")) {
		    if(vp->containsParameter(L"WORK")) {
                if (vp->containsParameter(L"FAX")) {
				    setProperty(L"BusinessFaxNumber", element);
                }
                else if (vp->containsParameter(L"PREF")) {
                    setProperty(L"CompanyMainTelephoneNumber", element);
                }
                else {      // default, would be "else if (vp->containsParameter(L"VOICE"))"
				    if(businessTel == 0) {
					    setProperty(L"BusinessTelephoneNumber", element);
					    businessTel++;
				    }
				    else{
					    setProperty(L"Business2TelephoneNumber", element);
				    }
                }
		    }
            else if (vp->containsParameter(L"CAR")) {
			    setProperty(L"CarTelephoneNumber", element);
            }
		    else if(vp->containsParameter(L"HOME")) {
                if(vp->containsParameter(L"FAX")) {
				    setProperty(L"HomeFaxNumber", element);
                }
                else {      // default, would be "else if (vp->containsParameter(L"VOICE"))"
                    if(homeTel == 0) {
				        setProperty(L"HomeTelephoneNumber", element);
				        homeTel++;
			        }
                    else {
				        setProperty(L"Home2TelephoneNumber", element);
                    }
                }
            }
            else if (vp->containsParameter(L"CELL")) {
			    setProperty(L"MobileTelephoneNumber", element);
            }
            else if (vp->containsParameter(L"FAX")) {
			    setProperty(L"OtherFaxNumber", element);
            }
            else if(vp->containsParameter(L"PAGER")) {
			    setProperty(L"PagerNumber", element);
            }
            else if (vp->containsParameter(L"PREF")) {
                setProperty(L"PrimaryTelephoneNumber", element);
            }
            else if(vp->containsParameter(L"X-FUNAMBOL-CALLBACK")) {
			    setProperty(L"CallbackTelephoneNumber", element);
            }
            else if(vp->containsParameter(L"X-FUNAMBOL-RADIO")) {
			    setProperty(L"RadioTelephoneNumber", element);
            }
            else if(vp->containsParameter(L"X-FUNAMBOL-TELEX")) {
			    setProperty(L"TelexNumber", element);
            }
            else {      // default, would be "else if (vp->containsParameter(L"VOICE"))"
			    setProperty(L"OtherTelephoneNumber", element);
            }
        }

        else if(!wcscmp(name, L"TITLE")) {
			setProperty(L"JobTitle", element);
        }
        
        else if(!wcscmp(name, L"PHOTO")) {
			setProperty(L"Photo", element);     // Photo type is ignored
        }

        //
        // ---- URL fields ----
        //
        else if(!wcscmp(name, L"URL")) {
            if (vp->containsParameter(L"WORK")) {
			    setProperty(L"BusinessWebPage", element);
            }
            else if(vp->containsParameter(L"HOME")) {
			    setProperty(L"HomeWebPage", element);
            }
            else {
			    setProperty(L"WebPage", element);
            }
        }

        //
        // ---- ADR fields ----
        //
        else if(!wcscmp(name, L"ADR")) {
			if(vp->containsParameter(L"HOME")) {
				if(element = vp->getPropComponent(1))  setProperty(L"HomeAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"HomeAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"HomeAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"HomeAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"HomeAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"HomeAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"HomeAddressCountry",       element);
			}
			else if(vp->containsParameter(L"WORK")) {
				if(element = vp->getPropComponent(1))  setProperty(L"BusinessAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"BusinessAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"BusinessAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"BusinessAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"BusinessAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"BusinessAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"BusinessAddressCountry",       element);
			}
			else {
                if(element = vp->getPropComponent(1))  setProperty(L"OtherAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"OtherAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"OtherAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"OtherAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"OtherAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"OtherAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"OtherAddressCountry",       element);
			}
        }

        //
        // ---- Funambol defined properties ----
        //
        else if(!wcscmp(name, L"X-FUNAMBOL-ANNIVERSARY")) {
			setProperty(L"Anniversary", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-BILLINGINFO")) {
            setProperty(L"BillingInformation", element);
        }
		else if(!wcscmp(name, L"X-FUNAMBOL-CHILDREN")) {
			setProperty(L"Children", element);
		}
		else if(!wcscmp(name, L"X-FUNAMBOL-COMPANIES")) {
			setProperty(L"Companies", element);
		}
		else if(!wcscmp(name, L"X-FUNAMBOL-CUSTOMERID")) {
			setProperty(L"CustomerID", element);
		}
        else if(!wcscmp(name, L"X-FUNAMBOL-FOLDER")) {
            setProperty(L"Folder", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-GENDER")) {
            setProperty(L"Gender", element);
        }
		else if(!wcscmp(name, L"X-FUNAMBOL-GOVERNMENTID")) {
			setProperty(L"GovernmentIDNumber", element);
		}
        else if(!wcscmp(name, L"X-FUNAMBOL-HOBBIES")) {
            setProperty(L"Hobby", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-INITIALS")) {
            setProperty(L"Initials", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-LANGUAGES")) {
            setProperty(L"Language", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-MANAGER")) {
            setProperty(L"ManagerName", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-MILEAGE")) {
            setProperty(L"Mileage", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-ORGANIZATIONALID")) {
            setProperty(L"OrganizationalIDNumber", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-SPOUSE")) {
            setProperty(L"Spouse", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-SUBJECT")) {
            setProperty(L"Subject", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-TELEX")) {
            setProperty(L"TelexNumber", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-YOMICOMPANYNAME")) {
            setProperty(L"YomiCompanyName", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-YOMIFIRSTNAME")) {
            setProperty(L"YomiFirstName", element);
        }
        else if(!wcscmp(name, L"X-FUNAMBOL-YOMILASTNAME")) {
            setProperty(L"YomiLastName", element);
        }

        else {
            // Property not supported: log a warning?
        }
    }

    return 0;
}



// Utility to check the productID and version of VObject passed.
bool WinContact::checkVCardTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    
    if (!prodID) {
        LOG.error(ERR_ITEM_VOBJ_TYPE_NOTFOUND, L"VCARD");
        return false;
    }
    if (wcscmp(prodID, L"VCARD")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VCARD");
        return false;
    }

    if (!version) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_VERSION_NOTFOUND, VCARD_VERSION);
    }
    else if (wcscmp(version, VCARD_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VCARD_VERSION);
    }
    return true;
}



wstring& WinContact::getName() {
    bool exists = false;
    return getPropertyRef(TEXT("FileAs"), &exists);
}
