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

#include "sapi/SapiServiceProfiling.h"
#include "spdm/constants.h"
#include "http/URL.h"
#include "base/util/utils.h"

USE_NAMESPACE


SapiServiceProfiling::SapiServiceProfiling(AbstractSyncConfig& c) : config(c), 
                                                                    expireTime(0),
                                                                    err(ESMRSuccess),
                                                                    autoSync(false),
                                                                    networkWarning(false) {
}

SapiServiceProfiling::~SapiServiceProfiling() {}


int SapiServiceProfiling::loginMock() {
    
    // reset local data
    sources.clear();
    propertyStringMap.clear();
    expireTime = 0;
    autoSync = false;
    networkWarning = false;
    
    // simulate some connection time
    sleepMilliSeconds(1500);
    
    StringBuffer username(config.getUsername());
    if ( (username.find("3462926190") != username.npos ) ||     // daily
		 (username.find("3357655849") != username.npos ) ) {    // transactional
        
        // FREEMIUM EXAMPLE
        sources.put("card",    "enabled");
        sources.put("cal",     "enabled");
        sources.put("event",   "enabled");
        sources.put("task",    "enabled");
        sources.put("note",    "enabled");
        sources.put("picture", "disabled");
        sources.put("video",   "disabled");
        sources.put("file",    "disabled");
        expireTime = (unsigned long)time(NULL) + 120L; // 2 minutes to expiration
        autoSync = false;
        networkWarning = true;
        err = ESMRSuccess;
		return 0;
    }
    else if ( (username.find("3387171573") != username.npos ) ||    // weekly
			  (username.find("3346237551") != username.npos ) ||    // weekly
			  (username.find("3666659583") != username.npos ) ) {   // montly

        // PREMIUM EXAMPLE
        sources.put("card",    "enabled");
        sources.put("cal",     "enabled");
        sources.put("event",   "enabled");
        sources.put("task",    "enabled");
        sources.put("note",    "enabled");
        sources.put("picture", "enabled");
        sources.put("video",   "enabled");
        sources.put("file",    "enabled");
        expireTime = (unsigned long)time(NULL) + 120L;   // 2 minutes to expiration
        autoSync = true;
        networkWarning = false;
        err = ESMRSuccess;
		return 0;
    }
    
    err = ESMRGenericError;
    return -1; // some errors ocured
}


int SapiServiceProfiling::login() {
	
    // return loginMock(); // mock, delete this row when mock is not necessary yet.

    // reset local data
    sources.clear();
	propertyStringMap.clear();
    expireTime = 0;
    autoSync = false;
    networkWarning = false;
    sessionID = "";
    
    //
    // Read params from config and call SapiMediaRequestManager::login()
    //
    URL url(config.getSyncURL());
    StringBuffer host = url.getHostURL();

    SapiMediaRequestManager req(host, 
                                ESapiMediaSourceUndefined, 
                                config.getUserAgent(), 
                                config.getUsername(),
                                config.getPassword());
    req.setRequestTimeout (config.getSapiRequestTimeout());
    req.setResponseTimeout(config.getSapiResponseTimeout());
    
    time_t serverTime;
    err = req.login(config.getDevID(), 
                            &serverTime, 
							// new parameters
							&expireTime,
							&sources,
							&propertyStringMap
							); // added new parameters expireTime , sources, propertyStringMap...
	
    StringBuffer sb;
	sb = propertyStringMap.get("auto-sync");
	if (!sb.empty()) {
		autoSync = (sb == "enabled");
	}

	sb = propertyStringMap.get("network-warning");
	if (!sb.empty()) {
		networkWarning = (sb == "enabled");
	}
	
	sessionID = req.getSessionID();
	
	if (err == ESMRSuccess) {
		return ESMRSuccess;
	}
    return -1; // some errors occured
}


