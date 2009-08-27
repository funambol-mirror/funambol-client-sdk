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

#include "http/TransportAgent.h"
#include "http/TransportAgentFactory.h"
#include "http/URL.h"
#include "http/Proxy.h"
#include "base/errors.h"
#include "base/Log.h"

#include "updater/Updater.h"
#include "updater/UpdaterConfig.h"
#include "updater/UpdaterUI.h"

#define UP_URL_RESOURCE     "/updateserver/update" // "/funambol/update"
#define UP_URL_COMPONENT    "component="
#define UP_URL_VERSION      "version="

#define UP_MANDATORY        "type="
#define UP_RELEASE_DATE     "delivery_date="
#define UP_SIZE             "size="
#define UP_VERSION          "version="
#define UP_URL_UPDATE       "url="
#define UP_URL_COMMENT      "url_description="

USE_NAMESPACE

Updater::Updater(const StringBuffer& component, const StringBuffer& version, UpdaterConfig& c)
                  :  component(component),
                     version(version),
                     nextRemindTime(0),
                     config(c),
                     ui(NULL)
{
    bool readOk = config.read();

    if (!readOk || config.getMandatory().empty()) {
        // Config not found or corrupted -> generate and save it
        config.createDefaultConfig();
        config.save();
    }

    // Init all times

    uint32_t checkInterval   = config.getIntervalCheckUpdateHttp();
    uint32_t lastCheckUpdate = string2long(config.getLastCheckUpdate().c_str());

    currentTime = time(NULL);
    // We want the upgrade check to be performed after the first sync on app
    // startup, we don't want to wait for the reminder time to elapse

    nextCheckTime = lastCheckUpdate + checkInterval;
    LOG.debug("Next http check: %d", nextCheckTime);
}

Updater::~Updater() { }

void Updater::setUI(UpdaterUI* ui) {
    this->ui = ui;
}

/**
* This method combines information and returns true iff the user must be
* notified about a new version available.
* The method is responsible for querying the server if the http interval
* has expired. 
*/
bool Updater::checkIsToUpdate() {
    
    bool ret = true;

    // Update current time
    currentTime = time(NULL);
    LOG.debug("Current time is: %d", currentTime);
    LOG.debug("Next check time is: %d", nextCheckTime);
   
    // First of all we check if it is time to query the
    // server for new updates. This is done regardless of the remind time,
    // as we may have a newer version to install and we want to show this
    // asap, and not wait for the remind interval to expire
    if (currentTime > nextCheckTime) {
        LOG.info("Check on server location for update");
        int32_t req = requestUpdate();
        if (req != -1) {
            // set the lastCheckUpdate in the config to be saved later
            config.setLastCheckUpdate(long2string(currentTime));
            config.save();
        }
    }

    // If it is too early to notify the user, we don't do it
    // (if a new version is detected, the nextRemindTime is reset
    // by requestUpdate)
    if (currentTime < nextRemindTime) {  
        LOG.debug("No time to check on local");
        ret = false;
        goto finally;
    }

    if (config.getVersion() == "0" || config.getVersion() == version) {       
        LOG.debug("No new version available");        
        ret = false;
    }

finally:
    return ret;
}



bool Updater::start() {
    LOG.debug("Starting update check");

    bool updateAvailable = checkIsToUpdate();
    if (updateAvailable) {
        LOG.debug("New version available");
        newVersionAvailable();
        return true;
    }
    return false;
}

int32_t Updater::requestUpdate() {
   
    int32_t res = 0;
    URL url;
    StringBuffer s;

    if (config.getUrlCheck().empty() || config.getUrlCheck() == "0") {
        LOG.debug("Invalid update url, cannot proceed");
        return -1;
    } else {
        s = config.getUrlCheck().c_str();        
    }
    
    // add the component and version to the url
    s += "?";
    s += UP_URL_COMPONENT;
    s += component;
    s += "&";
    s += UP_URL_VERSION;
    s += version;

    LOG.debug("Update url: %s", s.c_str());

    url.setURL(s.c_str());
    
    Proxy proxy;
    
    TransportAgent* transportAgent = NULL;
    transportAgent = TransportAgentFactory::getTransportAgent(url, proxy);
           
    StringBuffer message = "";
    char* response = transportAgent->sendMessage(message.c_str());

    if (response) {
        LOG.debug("Update server response: %s", response);
    } else {
        LOG.error("No response from update server");
    }

    StringBuffer nextVersion = config.getVersion();

    if (response != NULL) {
        int32_t parseRes = parseMessage(response);
        if (parseRes < 0) {
            LOG.error("Cannot parse update server response message");
        } else {
            // Update times
            config.setLastCheckUpdate(long2string(currentTime));
            uint32_t checkInterval = config.getIntervalCheckUpdateHttp();
            nextCheckTime = currentTime + checkInterval;
            LOG.debug("nextCheckTime=%d", nextCheckTime);
            // If there is a new version available (newer then the one not
            // installed yet), then we do not wait for the remind time
            // to expire, but we query the user right away
            if (nextVersion != "0" && nextVersion != config.getVersion()) {
                nextRemindTime = 0;
                config.setSkipped("0");
            }
        }
    } else {
        //LOG.error("Client update: %i: %s", lastErrorCode, lastErrorMsg);        
        LOG.error("Client update error - uses the stored parameters");
        res = -1;
    } 

    if (config.getMandatory() != "0") {
        config.setMandatory(config.getVersion());
        config.setSkipped("0");
    }
    config.save();
    delete response;
    delete transportAgent;

    return res;
}

StringBuffer getNewLine(const StringBuffer& s) {
    StringBuffer newline;
    size_t pos1 = s.find("\n");
    if(pos1 == StringBuffer::npos){
        LOG.error("Updater: no newlines in message?");
        return newline;
    }
    size_t pos2 = pos1 + 1;
    const char* chs = s.c_str();

    while (chs[pos1-1] == '\r'){
        pos1--;
    }
    newline = s.substr(pos1, pos2-pos1);
    return newline;
}

void Updater::getListedString(ArrayList& allString, const StringBuffer& s,
                              const StringBuffer& separator)
{
    size_t seplen = separator.length();
    char *base = (char*)s.c_str();
    char *p = strstr(base, separator.c_str());

    while (p) {
        StringBuffer token(base, p - base);
        allString.add(token);
        base = p + seplen;
        p = strstr(base, separator.c_str());
    }
    StringBuffer token(base);
    allString.add(token);
}

/*
* return -1: no new lines
*/
int32_t Updater::parseMessage(StringBuffer message) {
    
    StringBuffer newline = getNewLine(message);
    if (newline.empty()) {
        return -1;
    }
    StringBuffer line;
    ArrayList allString;
    
    getListedString(allString, message, newline);
    
    StringBuffer* it = (StringBuffer*)allString.front();
    while (it) {
        line = *it;
        if (line.find(UP_MANDATORY) == 0) {  
            StringBuffer mand = line.substr(strlen(UP_MANDATORY));        
            if (mand == "mandatory") {
                config.setMandatory("1");
            } else {
                // if the current update is not mandatory but the previous was, 
                // the parameter is not overwritten
                if (config.getMandatory() == "0") {
                    config.setMandatory("0");
                }
            }
        } else if (line.find(UP_RELEASE_DATE) == 0) {         
            config.setReleaseDate(line.substr(strlen(UP_RELEASE_DATE)));
        } else if (line.find(UP_SIZE) == 0) {
            StringBuffer size = line.substr(strlen(UP_SIZE));
            uint32_t s = atoi(size.c_str());
            config.setSize(s);
        } else if (line.find(UP_VERSION) == 0) {         
            config.setVersion(line.substr(strlen(UP_VERSION)));
        } else if (line.find(UP_URL_UPDATE) == 0) {         
            config.setUrlUpdate(line.substr(strlen(UP_URL_UPDATE)));
        } else if (line.find(UP_URL_COMMENT) == 0) {         
            config.setUrlComment(line.substr(strlen(UP_URL_COMMENT)));
        }
        it = (StringBuffer*)allString.next();
    }
    
    return 0;
}


StringBuffer Updater::long2string(uint32_t v) {
    
    StringBuffer s;
    s.sprintf("%d", v);
    return s;
}

uint32_t Updater::string2long(const StringBuffer& v) {
    
   return (uint32_t)atol(v.c_str());

}

void Updater::forceUpdate()
{
    if (ui) {
        config.setNow(config.getVersion());
        ui->startUpgrade(config);
    }
}

int32_t Updater::buildVersionID(const StringBuffer& version)
{
    int32_t major, minor, build;
    sscanf(version.c_str(), "%d.%d.%d", &major, &minor, &build);
    return major*10000 + minor*100 + build;
}

bool Updater::isNewVersionAvailable()
{
    StringBuffer newVersion = config.getVersion();
    LOG.debug("newVersion=%s", newVersion.c_str());
    LOG.debug("version=%s", version.c_str());
    // At the moment the server is rather "stupid" and
    // returns new version information regardless of our
    // version. We (the client) must check if this is an update
    // for ourselves or not
    if (newVersion.empty() || newVersion == "0") {
        return false;
    }

    // Compare the two versions
    int32_t newVersionID = buildVersionID(newVersion);
    int32_t versionID    = buildVersionID(version);
    return newVersionID > versionID;
}

void Updater::newVersionAvailable() {

    // Please note that at the moment the server returns a new version
    // regardless of our version, so we must check if the new version
    // is a true update or not
    LOG.debug("A new version (%s) is available", config.getVersion().c_str());
    if (!isNewVersionAvailable()) {
        LOG.debug("The available version is not an upgrade, ignore it");
        return;
    }

    // This flag specifies if the user accepts this upgrade. Possible values:
    // -1) the upgrade is not applicable or already skipped
    // 0)  the user accepts the upgrade (now)
    // 1)  the user postpones the upgrade (later)
    // 2)  the user skips the upgrade (skip)
    int32_t conf = -1;
    if (config.getMandatory() == "0") {
        if (config.getVersion() == config.getSkipped()) {
            LOG.debug("The version required is the same just skipped");
        } else {
            LOG.debug("Ask the user if he wants update (not mandatory)");
            if (ui) {
                conf = ui->askConfirmationForUpgrade(config);
            }
        }
    } else {
        LOG.debug("Ask the user if he wants update (mandatory)");
        if (ui) {
            conf = ui->askConfirmationForMandatoryUpgrade(config);
        }
    }

    // A long time may have elapsed between the dialog popping up and the
    // user clicking on one of the option. We better update the current time
    currentTime = time(NULL);

    switch (conf) {
        case 0:
            // Upgrade now
            if (ui) {
                config.setNow(config.getVersion());
                ui->startUpgrade(config);
            }
            break;
        case 1:
            // Postponed (later)
            config.setLater(config.getVersion());
            nextRemindTime = currentTime + config.getIntervalRemind();
            LOG.debug("The user postponed the upgrade. Remind him at: %d", nextRemindTime);
            break;
        case 2:
            // Skipped
            config.setSkipped(config.getVersion());
            break;
        default:
            // The question could not be prompted to the user, just ignore it
            break;
    }
    config.save();
}

