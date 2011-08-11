/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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



#include "spds/CustomConfig.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"
#include "base/util/KeyValuePair.h"
#include "spdm/constants.h"

USE_NAMESPACE



CustomConfig::CustomConfig() {
    extraProps.clear();
}

CustomConfig::~CustomConfig() {
    extraProps.clear();
}



void CustomConfig::setProperty(const char* propertyName, const char* propertyValue) {
    extraProps.put(propertyName, propertyValue);
}

void CustomConfig::setIntProperty(const char* propertyName, int propertyValue) {
    StringBuffer s;
    s.sprintf("%d", propertyValue);
    extraProps.put(propertyName, s.c_str());
}

void CustomConfig::setBoolProperty(const char* propertyName, bool propertyValue) {
    StringBuffer s;
    s = (propertyValue) ? "1" : "0";
    extraProps.put(propertyName, s.c_str());
}

void CustomConfig::setLongProperty(const char* propertyName, long propertyValue) {
    StringBuffer s;
    s.sprintf("%ld", propertyValue);
    extraProps.put(propertyName, s.c_str());
}

const char* CustomConfig::getProperty(const char* propertyName) {
    return extraProps.get(propertyName);
}

int CustomConfig::getIntProperty(const char* propertyName, bool* err) {
    const char* ret = extraProps.get(propertyName);
    int result = -1;
    
    if (ret == NULL) {
        *err = true;
    } else {
        *err = false;
        result = atoi(ret);
    }
    return result;
}

bool CustomConfig::getBoolProperty(const char* propertyName, bool* err) {
    const char* ret = extraProps.get(propertyName);
    bool result = false;
    
    if (ret == NULL) {
        *err = true;
    } else {
        *err = false;
        StringBuffer s(ret);
        if (s == "0" || s.icmp("false") == true) {
            result = false;
        } else {
            result = true;
        }
    }
    return result;
}


long CustomConfig::getLongProperty(const char* propertyName, bool *err) {
    const char* ret = extraProps.get(propertyName);
    long result = -1;
    
    if (ret == NULL) {
        *err = true;
    } else {
        *err = false;
        result = atol(ret);
    }
    return result;
    
}

bool CustomConfig::removeProperty(const char* propertyName) {

    if (!propertyName) {
        return false;
    }
    return extraProps.remove(propertyName);
}


void CustomConfig::printStringMap() {
    if (extraProps.empty()) {
        LOG.debug("config StringMap is empty");
    }
    KeyValuePair kvp = (KeyValuePair)extraProps.front(); 
    while (kvp.null() == false) {
        LOG.debug("config key: %s, value: %s", kvp.getKey().c_str(), kvp.getValue().c_str());
        kvp = extraProps.next();
    }
}


void CustomConfig::assign(const CustomConfig& c) {
    if (&c == this) {
        return;
    }
    extraProps = c.getExtraProps();
}
