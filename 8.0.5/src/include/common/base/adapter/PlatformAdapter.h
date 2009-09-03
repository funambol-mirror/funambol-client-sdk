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

#ifndef INCL_PLATFORM_ADAPTER
#define INCL_PLATFORM_ADAPTER
/** @cond API */

#include "base/fscapi.h"
#include "base/globalsdef.h"
#include "base/util/ArrayList.h"

BEGIN_NAMESPACE

/**
 * This is the application context used before the init() method is called.
 * Applications must not use it, to avoid collision between different apps.
 */
#define DEFAULT_APP_CONTEXT "Funambol/SDK"

class StringBuffer;
class StringMap;

class PlatformAdapter {

public:

    /**
     * Initializes the library adapter.
     *
     * The implementation depends on the platform. The common part, that
     * must be implemented by all adapters, is to save the application 
     * context, that is used by the library to store the configuration
     * parameters and temp files.
     *
     * The platform adapter should be initialized once and kept unchanged 
     * during the whole application runtime. 
     * The caller can force the adapter initialization by setting the 
     * force flag to true.
     * 
     * @param appContext an identifier of the application that is using
     *                   the library. The suggested format to guarantee
     *                   the uniqueness of this identifier is
     *                   "VendorName/AppName", for example:
     *                   "Funambol/DemoSyncClient".
     *
     * @param force force the platform adaper initialization. This option 
     *              is not enabled by default.
     *
     * @return none
     */
    static void init(const char *appcontext, const bool force = false);

    /**
     * Initializes the library adapter, using custom values.
     *
     * The implementation depends on the platform. The common part, that
     * must be implemented by all adapters, is to save the application 
     * context, that is used by the library to store the configuration
     * parameters and temp files.
     *
     * The platform adapter should be initialized once and kept unchanged 
     * during the whole application runtime. 
     * The caller can force the adapter initialization by setting the 
     * force flag to true.
     * 
     * The caller provides also a StringMap containing values for the
     * other system paths, overriding the system-dependent ones.
     * 
     * @param appContext an identifier of the application that is using
     *                   the library. The suggested format to guarantee
     *                   the uniqueness of this identifier is
     *                   "VendorName/AppName", for example:
     *                   "Funambol/DemoSyncClient".
     *
     * @param env a StringMap containing the values of the system path to
     *            be used by the current application. They must be valid
     *            paths, and it's caller responsibility to check that.
     *            If one of the values is not specified, the system-dependent
     *            default is used.<br/>
     *            Currently used values are:
     *            HOME_FOLDER   - set value returned by getHomeFolder()
     *            CONFIG_FOLDER - set value returned by getConfigFolder()
     *
     * @param force force the platform adaper initialization. This option 
     *              is not enabled by default.
     * 
     * @return none
     */
    static void init(const char *appcontext, StringMap& env, const bool force = false);

    /**
     * Returns the application context set by the init method.
     *
     * @return the application context, as it was set by the init() method.
     */
    static const StringBuffer& getAppContext();

    /**
     * Returns the home folder of the user, depending on the platform.
     *
     * @return
     *      <li> Posix:   $HOME </li>
     *      <li> Windows: the user's document folder </li>
     *      <li> iPhone:  the application's document folder </li>
     *      <li> Symbian: TBD.</li>
     */
    static const StringBuffer& getHomeFolder();

    /**
     * Returns the folder where the application can store data files for the 
     * current user. It contains the application context.
     *
     * Note: the path returned is not created if it does not exists. It
     *       is responsibility of the caller to check the presence of the folder.
     *
     * @return the config folder, dependent on the platform:
     *          <li> Posix: $XDG_CONFIG_HOME/&lt;AppContext&gt;
     *                      or $HOME/.config/&lt;AppContext&gt;</li>
     *          <li> Windows: &lt;CSIDL_APPDATA&gt;/&lt;AppContext&gt;
     *          <li> Posix: $XDG_CONFIG_HOME/&lt;AppContext&gt;
     *                      or $HOME/.config/&lt;AppContext&gt;</li>
     */
    static const StringBuffer& getConfigFolder();

private:

    static StringBuffer appContext;
    static StringBuffer homeFolder;
    static StringBuffer configFolder;
    static bool initialized;
};

END_NAMESPACE

/** @endcond */
#endif
