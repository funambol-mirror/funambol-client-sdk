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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "client/DMTClientConfig.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"

#include "updater/UpdaterConfig.h"

USE_NAMESPACE

StringBuffer UpdaterConfig::versionPropertyName("version");
StringBuffer UpdaterConfig::recommendedPropertyName("recommended");
StringBuffer UpdaterConfig::activationDatePropertyName("activation-date");
StringBuffer UpdaterConfig::urlCheckPropertyName("url-check");
StringBuffer UpdaterConfig::urlUpdatePropertyName("url");
StringBuffer UpdaterConfig::urlCommentPropertyName("comment");
StringBuffer UpdaterConfig::sizePropertyName("size");
StringBuffer UpdaterConfig::lastCheckUpdatePropertyName("last-check");
StringBuffer UpdaterConfig::intervalCheckUpdateHttpPropertyName("interval-check");
StringBuffer UpdaterConfig::intervalRemindPropertyName("remind-interval");
StringBuffer UpdaterConfig::lastUpdatedPropertyName("last-updated");
StringBuffer UpdaterConfig::skippedPropertyName("skipped");
StringBuffer UpdaterConfig::laterPropertyName("later");
StringBuffer UpdaterConfig::nowPropertyName("now");

UpdaterConfig::UpdaterConfig(const StringBuffer appUri) : applicationUri(appUri)
{
}
    
UpdaterConfig::~UpdaterConfig() {
}

bool UpdaterConfig::read()
{
    LOG.debug("Reading Updater configuration");

    // Now save client specific settings
    StringBuffer context(applicationUri);
    
    //
    // Load the main node settings
    //
    DMTree mainTree(context.c_str());
    ManagementNode* mainNode = mainTree.getNode(CONTEXT_UPDATER);
    if (mainNode == NULL) {
        LOG.error("Cannot find updater config");
        return false;
    }

    char* tmp;
    tmp     = mainNode->readPropertyValue(versionPropertyName.c_str());
    version = tmp;
    delete tmp;
    if (version.empty()) {
        version = "0";
    }
    LOG.debug("version=%s", version.c_str());

    tmp       = mainNode->readPropertyValue(recommendedPropertyName.c_str());
    recommended = tmp;
    delete tmp;
    LOG.debug("recommended=%s", recommended.c_str());

    tmp         = mainNode->readPropertyValue(activationDatePropertyName.c_str());
    LOG.debug("activation=%s", tmp);
    activationDate = tmp;
    delete tmp;

    tmp         = mainNode->readPropertyValue(urlUpdatePropertyName.c_str());
    LOG.debug("url update=%s", tmp);
    urlUpdate   = tmp;
    delete tmp;

    tmp          = mainNode->readPropertyValue(urlCommentPropertyName.c_str());
    LOG.debug("url comment=%s", tmp);
    urlComment   = tmp;
    delete tmp;

    tmp          = mainNode->readPropertyValue(urlCheckPropertyName.c_str());
    LOG.debug("url check=%s", tmp);
    urlCheck     = tmp;
    delete tmp;

    tmp          = mainNode->readPropertyValue(sizePropertyName.c_str());
    LOG.debug("size=%s", tmp);
    size         = tmp[0] != 0 ? atoi(tmp) : 0;
    delete tmp;

    tmp             = mainNode->readPropertyValue(lastCheckUpdatePropertyName.c_str());
    LOG.debug("last-check=%s", tmp);
    lastCheckUpdate = tmp;
    delete tmp;

    tmp             = mainNode->readPropertyValue(
                                          intervalCheckUpdateHttpPropertyName.c_str());
    LOG.debug("interval-check-http=%s", tmp);
    intervalCheckUpdateHttp = tmp[0] != 0 ? atoi(tmp) : 0;
    delete tmp;

    tmp            = mainNode->readPropertyValue(intervalRemindPropertyName.c_str());
    LOG.debug("interval-remind=%s", tmp);
    intervalRemind = tmp[0] != 0 ? atoi(tmp) : 0;
    delete tmp;

    tmp         = mainNode->readPropertyValue(lastUpdatedPropertyName.c_str());
    LOG.debug("last-update=%s", tmp);
    lastUpdated = tmp;
    delete tmp;

    tmp     = mainNode->readPropertyValue(skippedPropertyName.c_str());
    LOG.debug("skipped=%s", tmp);
    skipped = tmp;
    delete tmp;

    tmp   = mainNode->readPropertyValue(laterPropertyName.c_str());
    LOG.debug("later=%s", tmp);
    later = tmp;
    delete tmp;

    tmp = mainNode->readPropertyValue(nowPropertyName.c_str());
    LOG.debug("now=%s", tmp);
    now = tmp;
    delete tmp;

    delete mainNode;
    return true;
}
    
void UpdaterConfig::save()
{
    LOG.debug("Saving Updater configuration");
    //LOG.debug("url update = %s", urlUpdate.c_str());

    // Now save client specific settings
    StringBuffer context(applicationUri);

    //
    // Save the main node settings
    //
    DMTree mainTree(context.c_str());
    ManagementNode* mainNode = mainTree.getNode(CONTEXT_UPDATER);
    if (mainNode == NULL) {
        return;
    }

    mainNode->setPropertyValue(versionPropertyName.c_str(), version.c_str());
    mainNode->setPropertyValue(recommendedPropertyName.c_str(), recommended.c_str());
    mainNode->setPropertyValue(activationDatePropertyName.c_str(), activationDate.c_str());
    mainNode->setPropertyValue(urlUpdatePropertyName.c_str(), urlUpdate.c_str());
    mainNode->setPropertyValue(urlCommentPropertyName.c_str(), urlComment.c_str());
    mainNode->setPropertyValue(urlCheckPropertyName.c_str(), urlCheck.c_str());

    StringBuffer s;
    s.sprintf("%d", size);
    mainNode->setPropertyValue(sizePropertyName.c_str(), s.c_str());

    mainNode->setPropertyValue(lastCheckUpdatePropertyName.c_str(),
                               lastCheckUpdate.c_str());
    s.sprintf("%ld", intervalCheckUpdateHttp);
    mainNode->setPropertyValue(intervalCheckUpdateHttpPropertyName.c_str(), s.c_str());

    s.sprintf("%ld", intervalRemind);
    mainNode->setPropertyValue(intervalRemindPropertyName.c_str(), s.c_str());

    mainNode->setPropertyValue(lastUpdatedPropertyName.c_str(), lastUpdated.c_str());
    mainNode->setPropertyValue(skippedPropertyName.c_str(), skipped.c_str());
    mainNode->setPropertyValue(laterPropertyName.c_str(), later.c_str());
    mainNode->setPropertyValue(nowPropertyName.c_str(), now.c_str());
    delete mainNode;
}


void UpdaterConfig::createDefaultConfig()
{
    // Overwrite all params, with default ones.
    LOG.debug("Generating default config for Updater...");
    
    version                 = "0";    
    recommended             = "0";	      // must be set
    activationDate          = "";
    urlUpdate               = "";
    urlComment              = "";
    urlCheck                = "";         // Will be set from "UpdateManager::setURLCheck()"
    size                    = 0;
    lastCheckUpdate         = "0";
    intervalCheckUpdateHttp = 86400;      // 1 day
    intervalRemind          = 7200;       // 2 hours
    lastUpdated             = "0";  
    skipped                 = "";
    later                   = "";
    now                     = "";
}


uint32_t UpdaterConfig::getIntervalRemind() const {
    return intervalRemind;
}

uint32_t UpdaterConfig::getIntervalCheckUpdateHttp() const {
    return intervalCheckUpdateHttp;
}

void UpdaterConfig::setLastCheckUpdate(const StringBuffer& time) {
    lastCheckUpdate = time;
}

const StringBuffer& UpdaterConfig::getLastCheckUpdate() const {
    return lastCheckUpdate;
}

void  UpdaterConfig::setVersion(const StringBuffer& version) {
    this->version = version;
}

const StringBuffer& UpdaterConfig::getVersion() const {
    return version;
}

void  UpdaterConfig::setCurrentVersion(const StringBuffer& v) {
    this->currentVersion = v;
}

const StringBuffer& UpdaterConfig::getCurrentVersion() const {
    return currentVersion;
}

void  UpdaterConfig::setRecommended(const StringBuffer& recommended) {
    this->recommended = recommended;
}

const StringBuffer& UpdaterConfig::getRecommended() const {
    return recommended;
}

void  UpdaterConfig::setUrlCheck(const StringBuffer& urlCheck) {
    this->urlCheck = urlCheck;
}
const StringBuffer& UpdaterConfig::getUrlCheck() const {
    return urlCheck;
}

void UpdaterConfig::setSkipped(const StringBuffer& skipped) {
    this->skipped = skipped;
}

const StringBuffer& UpdaterConfig::getSkipped() const {
    return skipped;
}

void  UpdaterConfig::setLater(const StringBuffer& later) {
    this->later = later;
}

const StringBuffer& UpdaterConfig::getLater() const {
    return later;
}

void  UpdaterConfig::setNow(const StringBuffer& now) {
    this->now = now;
}

const StringBuffer& UpdaterConfig::getNow() const {
    return now;
}

void UpdaterConfig::setReleaseDate(const StringBuffer& activationDate) {
    this->activationDate = activationDate;
}

void UpdaterConfig::setSize(uint32_t size) {
    this->size = size;
}

void UpdaterConfig::setUrlUpdate(const StringBuffer& urlUpdate) {
    this->urlUpdate = urlUpdate;
}

const StringBuffer& UpdaterConfig::getUrlUpdate() const {
    return urlUpdate;
}

void UpdaterConfig::setUrlComment(const StringBuffer& comment) {
    this->urlComment = comment;
}

