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



#include "sapi/SapiConfig.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"
#include "base/util/KeyValuePair.h"
#include "spdm/constants.h"

USE_NAMESPACE



SapiConfig::SapiConfig() {}

SapiConfig::~SapiConfig() {}


void SapiConfig::setRequestTimeout(const int timeout) { 
    setIntProperty(PROPERTY_REQUEST_TIMEOUT, timeout); 
}

void SapiConfig::setResponseTimeout(const int timeout) { 
    setIntProperty(PROPERTY_RESPONSE_TIMEOUT, timeout); 
}

void SapiConfig::setUploadChunkSize(const int size) { 
    setIntProperty(PROPERTY_UPLOAD_CHUNK_SIZE, size); 
}

void SapiConfig::setDownloadChunkSize(const int size) { 
    setIntProperty(PROPERTY_DOWNLOAD_CHUNK_SIZE, size); 
}

void SapiConfig::setMaxRetriesOnError(const int retries) { 
    setIntProperty(PROPERTY_MAX_RETRIES_ON_ERROR, retries); 
}

void SapiConfig::setSleepTimeOnRetry(const long msec) { 
    setLongProperty(PROPERTY_SLEEP_TIME_ON_RETRY, msec); 
}

void SapiConfig::setResetStreamOnRetry(bool reset) {
    setBoolProperty(PROPERTY_RESET_STREAM_ON_RETRY, reset);
}

void SapiConfig::setMinDataSizeOnRetry(const long size) {
    setLongProperty(PROPERTY_MIN_DATA_SIZE_ON_RETRY, size);
}

void SapiConfig::setCaredServer(bool cared) {
    setBoolProperty(PROPERTY_IS_CARED_SERVER, cared);
}

int SapiConfig::getRequestTimeout() {
    bool err = false;
    int ret = getIntProperty(PROPERTY_REQUEST_TIMEOUT, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_REQUEST_TIMEOUT);
        return 0;
    }
    return ret;
}

int SapiConfig::getResponseTimeout() {
    bool err = false;
    int ret = getIntProperty(PROPERTY_RESPONSE_TIMEOUT, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_RESPONSE_TIMEOUT);
        return 0;
    }
    return ret;
}

int SapiConfig::getUploadChunkSize() {
    bool err = false;
    int ret = getIntProperty(PROPERTY_UPLOAD_CHUNK_SIZE, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_UPLOAD_CHUNK_SIZE);
        return 0;
    }
    return ret;
}

int SapiConfig::getDownloadChunkSize() {
    bool err = false;
    int ret = getIntProperty(PROPERTY_DOWNLOAD_CHUNK_SIZE, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_DOWNLOAD_CHUNK_SIZE);
        return 0;
    }
    return ret;
}

int SapiConfig::getMaxRetriesOnError() {
    bool err = false;
    int ret = getIntProperty(PROPERTY_MAX_RETRIES_ON_ERROR, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_MAX_RETRIES_ON_ERROR);
        return 0;
    }
    return ret;
}

long SapiConfig::getSleepTimeOnRetry() {
    bool err = false;
    long ret = getLongProperty(PROPERTY_SLEEP_TIME_ON_RETRY, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_SLEEP_TIME_ON_RETRY);
        return 0;
    }
    return ret;
}

bool SapiConfig::getResetStreamOnRetry() {
    bool err = false;
    bool ret = getBoolProperty(PROPERTY_RESET_STREAM_ON_RETRY, &err);
    
    if (err) {
        LOG.debug("%s: property %s not found: set to false", __FUNCTION__, PROPERTY_RESET_STREAM_ON_RETRY);
        return false;
    }
    
    return ret;
}

long SapiConfig::getMinDataSizeOnRetry() {
    bool err = false;
    long ret = getLongProperty(PROPERTY_MIN_DATA_SIZE_ON_RETRY, &err);
    if (err) {
        LOG.debug("%s: property %s not found: set to 0", __FUNCTION__, PROPERTY_MIN_DATA_SIZE_ON_RETRY);
        return 0;
    }
    
    return ret;
}

bool SapiConfig::isCaredServer() {
    bool err = true;
    bool ret = getBoolProperty(PROPERTY_IS_CARED_SERVER, &err);
    
    if (err) {
        return true;
    }
    
    return ret;
}

void SapiConfig::assign(const SapiConfig& sc) {
    if (&sc == this) {
        return;
    }
    extraProps = sc.getExtraProps();
}
