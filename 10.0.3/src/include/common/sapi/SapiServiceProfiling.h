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

#ifndef SAPI_SERVICE_PROFILING
#define SAPI_SERVICE_PROFILING

/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "base/util/StringMap.h"
#include "spds/constants.h"
#include "spds/AbstractSyncConfig.h"
#include "sapi/SapiMediaRequestManager.h"

BEGIN_FUNAMBOL_NAMESPACE

/**
 * Used to get profile informations for the current user account.
 * It executes a SAPI login, including the '&details' parameter to retrieve additional
 * informations such as:
 *  - dataplan expire time
 *  - auto-sync enabled/disabled
 *  - list of sources allowed
 *  - network warning enabled/disabled
 */
class SapiServiceProfiling {

public:

    /**
     * Constructor
     * @param c  configuration object to read all required param for SAPI login
     */
    SapiServiceProfiling(AbstractSyncConfig& c);

    /// Destructor
    virtual ~SapiServiceProfiling();

    /**
     * Executes the SAPI login with "&details" parameter.
     * Stores the returned configuration parameters from the Server.
     * @return 0  if login was SUCCESSFUL
     *        -1  if there was some errors
     */
    int login();

    /**
     * this is the mocked version for login, used for testing.
     * SapiServiceProfiling::login() calls this method.
     * @return 0  if login was sucessuful
     *        -1  if there was some errors
     */
	int loginMock();


    // getters
    unsigned long getExpireTime()       { return expireTime;     }
    bool          getAutoSync()         { return autoSync;       }
    bool          getNetworkWarning()   { return networkWarning; }
    StringMap&    getSources()          { return sources;        }
    ESMRStatus    getError()            { return err;            }
    
    StringBuffer getSessionID()         { return sessionID;      }


private:

    /// The configuration object, containing all sync settings.
    AbstractSyncConfig& config;

    /// Expire time for the current profile. 0 means "no expire time".
    unsigned long expireTime;

    /// False means the auto sync (push and scheduled services) is not allowed
    bool          autoSync;

    /// If true a warning popup is required before every http connection
    bool          networkWarning;

    /**
     * Key-value pair of sources allowed by the server.
     * Key is the source name, value is "enabled" or "disabled", like:
     *   "card"    - "enabled"
     *   "event    - "enabled"
     *   "picture" - "disabled"
     */
    StringMap     sources;

	StringMap     propertyStringMap;
	
	/**
	 * The sessionID, returned by the SAPI login.
	 * Stored in order to use again this session for next sapi calls.
	 */
	StringBuffer  sessionID;

	/// The error code returned by SapiMediaRequestManager::login().
	ESMRStatus err;


};


END_FUNAMBOL_NAMESPACE

/** @} */
/** @endcond */
#endif

