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

#ifndef INCL_MSUDEVICE_INFO
#define INCL_MSUDEVICE_INFO


#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"

BEGIN_FUNAMBOL_NAMESPACE

 /**
 * Represents the Device Info needed for the Mobile SignUp process.
 * Only phoneNumber, password and platform are mandatory.
 * Other fields can be filled by the virtual completeOptionalFields() method. 
 */
 
class MSUDeviceInfo {

private:
    StringBuffer phoneNumber;
    StringBuffer password;
    StringBuffer platform;
    StringBuffer emailAddress;
    StringBuffer timezone;
    StringBuffer manufacturer;
    StringBuffer model;
    StringBuffer carrier;
    StringBuffer countryCodeA2;
    
    
protected:
    void setPlatform(const char* v)    { platform = v;    }
    
public:

    // Constructor
    MSUDeviceInfo(const char* phoneNum, const char* passwd, const char* plat);    

    virtual ~MSUDeviceInfo();       
    
    void setPhoneNumber(const char* v) { phoneNumber = v; }
    void setPassword(const char* v)    { password = v;    }
    
    /**
     * Initialize the other optional fields. It may be implemented by
     * every platform
     */
    virtual void completeOptionalFields() { }
    
    const StringBuffer& getPhoneNumber() const {
        return phoneNumber;
    }
    
    const StringBuffer& getPassword() const {
        return password;
    }
    
    const StringBuffer& getPlatform() const {
        return platform;
    }

    const StringBuffer& getEmailAddress() {
        return emailAddress;
    }
    void setEmailAddress(const char* v) { emailAddress = v; }
    
    const StringBuffer& getTimezone() const {
        return timezone;
    }
    void setTimezone(const char* v) { timezone = v; }
    
    
    const StringBuffer& getManufacturer() const {
        return manufacturer;
    }
    void setManufacturer(const char* v) { manufacturer = v; }
    
    const StringBuffer& getModel() const {
        return model;
    }
    void setModel(const char* v) { model = v; }
    
    const StringBuffer& getCarrier() const {
        return carrier;
    }
    void setCarrier(const char* v) { carrier = v; }
    
    const StringBuffer& getCountryCodeA2() const {
        return countryCodeA2;
    }
    void setCountryCodeA2(const char* v) { countryCodeA2 = v; }
    
};


END_FUNAMBOL_NAMESPACE

#endif
