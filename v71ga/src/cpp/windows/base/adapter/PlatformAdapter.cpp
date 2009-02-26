/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#include "base/adapter/PlatformAdapter.h"
#include "base/util/StringBuffer.h"
#include "base/util/StringMap.h"
#include "base/Log.h"
#include <ShlObj.h>

BEGIN_NAMESPACE

StringBuffer PlatformAdapter::appContext(DEFAULT_APP_CONTEXT);
StringBuffer PlatformAdapter::homeFolder;
StringBuffer PlatformAdapter::configFolder;
bool PlatformAdapter::initialized = false;

// Initializes the platform-dependent parameters of the library using defaults.
void PlatformAdapter::init(const char *appcontext, const bool force) {
    if(!initialized || force) {
        appContext = appcontext;
        homeFolder = "";
        configFolder = "";
        initialized = true;
    }
    else {
        LOG.error("PlatformAdapter::init(): already initialized.");
    }
    if(initialized && force) {
        LOG.debug("PlatformAdapter::init(): forcing to a new initialization.");
    }
}

// Initializes the platform-dependent parameters of the library with custom values.
void PlatformAdapter::init(const char *appcontext, StringMap& env, const bool force) {
    if(!initialized || force) {
        appContext = appcontext;
        homeFolder = env["HOME_FOLDER"];
        configFolder = env["CONFIG_FOLDER"];
        initialized = true;
    }
    else {
        LOG.error("PlatformAdapter::init(): already initialized.");
    }
    if(initialized && force) {
        LOG.debug("PlatformAdapter::init(): forcing to a new initialization.");
    }
}


// Returns the application context
const StringBuffer& PlatformAdapter::getAppContext() {
    return appContext;
}

// Returns the home folder, or an empty string on failure.
const StringBuffer& PlatformAdapter::getHomeFolder() {
    if (homeFolder.empty()) {
        wchar_t p[MAX_PATH];

        SHGetSpecialFolderPath(NULL, p, CSIDL_PERSONAL, 0);
        homeFolder.convert(p);
    }
    return homeFolder;
}

// Returns the home folder, or an empty string on failure.
const StringBuffer& PlatformAdapter::getConfigFolder() {
    if (configFolder.empty()){
        wchar_t appdir[MAX_PATH];
        SHGetSpecialFolderPath(NULL, appdir, CSIDL_APPDATA, 0); 
        configFolder.convert(appdir);
        configFolder += "/" ; 
        configFolder += appContext;
    }
    return configFolder;
}


END_NAMESPACE
