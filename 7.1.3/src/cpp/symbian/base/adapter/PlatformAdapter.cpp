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

#include "base/fscapi.h"
#include "base/adapter/PlatformAdapter.h"
#include "base/util/StringBuffer.h"
#include "base/util/StringMap.h"
#include "base/util/stringUtils.h"
#include "base/Log.h"

// include CEikonEnv framework for CEikonEnv::Static()
#include <eikapp.h> 
#include <eikappui.h> 
#include <eikenv.h>

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

// Returns the home folder. On Symbian platform home folder is 
// the application private folder 
const StringBuffer& PlatformAdapter::getHomeFolder() {
    if (homeFolder.empty()) {
        // Get ONLY the private path (e.g. "\private\2001BBA4")
        TBuf<KMaxPath> privatePath;
        CEikonEnv::Static()->FsSession().PrivatePath(privatePath);
        TParsePtr parse2(privatePath);
        TPtrC currentPath = parse2.Path();

#ifdef __GCCE__
        TFileName appFullName = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
        TParsePtr parse1(appFullName);
        TPtrC currentDrive = parse1.Drive();

        StringBuffer drive = bufToStringBuffer(currentDrive);
#else
        // emulator builds should use C: as drive
        StringBuffer drive = "C:";
#endif
        homeFolder.append(drive);
        StringBuffer path = bufToStringBuffer(currentPath);
        homeFolder.append(path);
    }

    return homeFolder;
}

// Returns the config folder 
const StringBuffer& PlatformAdapter::getConfigFolder() {
    if (configFolder.empty()) {
        StringBuffer privatePath = getHomeFolder();
        
        configFolder.append(privatePath);
    }

    return configFolder;
}

END_NAMESPACE

