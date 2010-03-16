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

#ifndef INCL_UPDATER
#define INCL_UPDATER

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"

#include "updater/UpdaterConfig.h"
#include "updater/UpdaterUI.h"

#define UP_URL_COMPONENT    "component="
#define UP_URL_VERSION      "version="
#define UP_URL_FORMAT       "format="

#define UP_TYPE             "type="
#define UP_ACTIVATION_DATE  "activation-date="
#define UP_SIZE             "size="
#define UP_VERSION          "version="
#define UP_URL_UPDATE       "url="
#define UP_URL_COMMENT      "url_description="

// To get data in the properties format (otherwise "JSON" is default)
#define UP_PROPERTIES       "properties"

// Update types:
#define UP_TYPE_OPTIONAL    "optional"
#define UP_TYPE_RECOMMENDED "recommended"     // This is the default
#define UP_TYPE_MANDATORY   "mandatory"


BEGIN_NAMESPACE

/**
 * This class is responsible to handle the update procedure of the client.
 * It contains all the action that has to be done in order to handle the udpate.
 */
class Updater {
  
private:
   
    /** Name of the component to check for updates */
    StringBuffer component;

    /** Current version of the component for which we check updates */
    StringBuffer version;
   
    /** Current time (this is not continuos, but updated when needed) */
    time_t currentTime;

    /** The time on which the next check (with the server) will be performed */
    time_t nextCheckTime;

    /** The time on which the next reminder to the user shall be presented */
    time_t nextRemindTime;

    /** 
     * Updater configuration. 
     * Note: it's owned externally!
     */
    UpdaterConfig& config;

    /** UI abstraction to interact with the user */
    UpdaterUI* ui;

private:

    /**
    * It checks if it is needed to check via url or locally only.
    * If locally only, there is no other stuff to do. Otherwise the data
    * are overwritten in the Updateconfig and then persisted
    */
    int32_t requestUpdate();
    
    /**
    * Used to ask the server. Parse the message from the server and populate 
    * the UpdateConfig
    */
    int32_t parseMessage(StringBuffer message);
    
    /**
    * utility to convert an unsigned long into a string
    */
    StringBuffer long2string(uint32_t v) ;
    
    /**
    * utility to convert a string into an unsigned long
    */
    uint32_t string2long(const StringBuffer& v) ;

    /**
     * Get all the strings in the server message
     */
    void getListedString(ArrayList& allString, const StringBuffer& s,
                         const StringBuffer& separator);


    /**
     * Build a numerical representation of a software version string. This
     * representation is suitable for comparison (>,<,==). Versions must be
     * triplets for this method to work properly.
     */
    int32_t buildVersionID(const StringBuffer& version);
 
public:
    
     /** 
      * Construct an update with the given component name, version and configuration.
      */
     Updater(const StringBuffer& component, const StringBuffer& version, UpdaterConfig& c);
    
     ~Updater();

    /**
     * Start the update check progress. This may result in connecting to the
     * update server, check if a new version is available. If this is not
     * required, we can still notify the user of a new version which was
     * previously postponed (later option).
     * The method returns true iff the check discovers a new version.
    */
    bool start();

    /**
     * This method forces the process of installing a new version. It does not
     * check if a new version is really available, but it calls the UI with
     * the last update URL (which may be invalid). It is responsability of the
     * caller to make sure it makes sense to invoke this method.
     */
    void forceUpdate();

    /**
     * Returns true if a new version is known to be available for upgrade. This
     * method does not query the upgrade server, but it uses the information
     * available.
     */
    bool isNewVersionAvailable();

    /**
     * This method handles a new version. It takes care of asking the user
     * what he intends to do and take the proper actions.
     * The onlyMandatoryExpired is usually false because the method is in the flow
     * of the udpate. Putting the onlyMandatoryExpired to true, means that the check
     * with the stored info is done only if the updateType is mandatory and the 
     * expiration date is done. It is useful for some check that can be done before starting 
     * the sync.
     * 
     * @param onlyMandatoryExpired - false to check everything during the whole process
     *                               true continue to check only if the mandatory udpate is expired
     *
     * @return true if the user has selected something. False if no action was performed by the 
     *         user.
     */
    bool newVersionAvailable(bool onlyMandatoryExpired = false);

    /**
     * Set the ui for calling bck when user actions are required. If this value
     * is not set, the update changes the config but does not need any user
     * interaction.
     */
    void setUI(UpdaterUI* ui);

    /**
    * check the update on the server. If it is necessary set a value to 1
    * return  false if there is no action to do
    * return  true  if there is something to download
    */
    bool checkIsToUpdate();
 
    /**
    * Check if the stored info are Mandatory update and the activation date has been exceeded. 
    * It checks between the activation date stored in the settings and the current date 
    * in the system plus the updateType  that must be mandatory...
    *
    * @return true if it is a mandatory and the activation date exceeded, false otherwise
    */
    bool isMandatoryUpdateActivationDateExceeded();

};

END_NAMESPACE

#endif

