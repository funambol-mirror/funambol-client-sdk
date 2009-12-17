/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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
#include "base/util/utils.h"
#include "spds/DeviceConfig.h"
#include "spdm/constants.h"
#include "base/globalsdef.h"

USE_NAMESPACE


DeviceConfig::DeviceConfig() {

    man             = NULL;
    mod             = NULL;
    oem             = NULL;
    fwv             = NULL;
    swv             = NULL;
    hwv             = NULL;
    devID           = NULL;
    devType         = NULL;
    dsV             = NULL;
    utc             = true;
    loSupport       = false;
    nocSupport      = false;
    logLevel        = LOG_LEVEL_INFO;
    maxObjSize      = 0;
    devInfHash      = NULL;

    smartSlowSync   = 2;
    multipleEmailAccount = 0;

	verDTD			= NULL;

    sendDevInfo     = true;         // Default = send Client devInf normally
    forceServerDevInfo = false;     // Default = ask Server devInf if necessary (don't force)
    serverLastSyncURL = NULL;
}

DeviceConfig::DeviceConfig(DeviceConfig& s) {
    assign(s);
}

DeviceConfig::~DeviceConfig() {

    safeDelete(&man       );
    safeDelete(&mod       );
    safeDelete(&oem       );
    safeDelete(&fwv       );
    safeDelete(&swv       );
    safeDelete(&hwv       );
    safeDelete(&devID     );
    safeDelete(&devType   );
    safeDelete(&dsV       );
    safeDelete(&devInfHash);
	safeDelete(&verDTD    );
    safeDelete(&serverLastSyncURL);

}


void DeviceConfig::set(char** buf, const char* v) {
	safeDelete(buf);

	if (v == NULL) {
		v = "";
	}
	int len = strlen(v);
	*buf = new char[len+2];

	strcpy(*buf, v);
}


/*
 * Methods to get/set data values.
 * -----------------------------------------------------
 * get: return the internal value.
 *      The caller MUST NOT release the memory itself.
 *
 * set: set the internal value.
 *      The given data are copied in an internal
 *      buffer so that the caller is assured that the
 *      given address can be released after the call.
 */
const char* DeviceConfig::getMan() const {
    return man;
}
void DeviceConfig::setMan(const char* v){
	set(&man, v);
}

const char* DeviceConfig::getMod() const {
    return mod;
}
void DeviceConfig::setMod(const char* v){
	set(&mod, v);
}

const char* DeviceConfig::getOem() const {
    return oem;
}
void DeviceConfig::setOem(const char* v){
	set(&oem, v);
}

const char* DeviceConfig::getFwv() const {
    return fwv;
}
void DeviceConfig::setFwv(const char* v){
	set(&fwv, v);
}

const char* DeviceConfig::getSwv() const {
    return swv;
}
void DeviceConfig::setSwv(const char* v){
	set(&swv, v);
}

const char* DeviceConfig::getHwv() const {
    return hwv;
}
void DeviceConfig::setHwv(const char* v){
	set(&hwv, v);
}

const char* DeviceConfig::getDevID() const {
    return devID;
}
void DeviceConfig::setDevID(const char* v){
	set(&devID, v);
}

const char* DeviceConfig::getDevType() const {
    return devType;
}
void DeviceConfig::setDevType(const char* v){
	set(&devType, v);
}

const char* DeviceConfig::getDsV() const {
    return dsV;
}
void DeviceConfig::setDsV(const char* v){
	set(&dsV, v);
}

bool DeviceConfig::getUtc() const {
    return utc;
}
void DeviceConfig::setUtc(bool v) {
    utc = v;
}

bool DeviceConfig::getLoSupport() const {
    return loSupport;
}
void DeviceConfig::setLoSupport(bool v) {
    loSupport = v;
}

bool DeviceConfig::getNocSupport() const {
    return nocSupport;
}
void DeviceConfig::setNocSupport(bool v) {
    nocSupport = v;
}

LogLevel DeviceConfig::getLogLevel() const {
    return logLevel;
}
void DeviceConfig::setLogLevel(LogLevel v) {
    logLevel = v;
}

unsigned int DeviceConfig::getMaxObjSize() const {
    return maxObjSize;
}
void DeviceConfig::setMaxObjSize(unsigned int v) {
    maxObjSize = v;
}

const char* DeviceConfig::getDevInfHash() const {
    return devInfHash;
}
void DeviceConfig::setDevInfHash(const char *v) {
    set(&devInfHash, v);
}

int DeviceConfig::getSmartSlowSync() const {
    return smartSlowSync;
}

int DeviceConfig::getMultipleEmailAccount() const {
    return multipleEmailAccount;
}

void DeviceConfig::setSmartSlowSync(int v) {
    smartSlowSync = v;
}

void DeviceConfig::setMultipleEmailAccount(int v) {
    multipleEmailAccount = v;
}

const char* DeviceConfig::getVerDTD() const {
    return verDTD;
}
void DeviceConfig::setVerDTD(const char *v) {
    set(&verDTD, v);
}

bool DeviceConfig::getSendDevInfo() const {
     return sendDevInfo;
}

void DeviceConfig::setSendDevInfo(bool v) {
    sendDevInfo = v; 
}

bool DeviceConfig::getForceServerDevInfo() const {
     return forceServerDevInfo;
}

void DeviceConfig::setForceServerDevInfo(bool v) {
    forceServerDevInfo = v; 
}

const char* DeviceConfig::getServerLastSyncURL() const {
    return serverLastSyncURL;
}
void DeviceConfig::setServerLastSyncURL(const char *v) {
    set(&serverLastSyncURL, v);
}

 
/*
 * Sets the values of this object with with the values from the given
 * DeviceConfig source object.
 *
 * @param config the new value.
 */
void DeviceConfig::assign(const DeviceConfig& s) {

    setMan          (s.getMan()           );
    setMod          (s.getMod()           );
    setOem          (s.getOem()           );
    setFwv          (s.getFwv()           );
    setSwv          (s.getSwv()           );
    setHwv          (s.getHwv()           );
    setDevID        (s.getDevID()         );
    setDevType      (s.getDevType()       );
    setDsV          (s.getDsV()           );
    setUtc          (s.getUtc()           );
    setLoSupport    (s.getLoSupport()     );
    setNocSupport   (s.getNocSupport()    );
    setLogLevel     (s.getLogLevel()      );
    setMaxObjSize   (s.getMaxObjSize()    );
    setDevInfHash   (s.getDevInfHash()    );

    setSmartSlowSync( s.getSmartSlowSync() );
    setMultipleEmailAccount( s.getMultipleEmailAccount() );
	setVerDTD       (s.getVerDTD	   () );

	setSendDevInfo       (s.getSendDevInfo   ()    );
    setForceServerDevInfo(s.getForceServerDevInfo());
    setServerLastSyncURL (s.getServerLastSyncURL() );
}

