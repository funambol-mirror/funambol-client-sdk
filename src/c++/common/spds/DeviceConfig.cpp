/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include "base/fscapi.h"
#include "base/util/utils.h"
#include "spds/DeviceConfig.h"
#include "spdm/constants.h"


DeviceConfig::DeviceConfig() {

    verDTD          = NULL;
    man             = NULL;
    mod             = NULL;
    oem             = NULL;
    fwv             = NULL;
    swv             = NULL;
    hwv             = NULL;
    devID           = NULL;
    devType         = NULL;
    dsV             = NULL;
    utc             = TRUE;
    loSupport       = FALSE;
    nocSupport      = FALSE;
    logLevel        = LOG_LEVEL_INFO;
    maxObjSize      = 0;
    devInfHash      = NULL;

}

DeviceConfig::DeviceConfig(DeviceConfig& s) {
    assign(s);
}

DeviceConfig::~DeviceConfig() {

    safeDelete(&verDTD    );
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
const char* DeviceConfig::getVerDTD() const {
    return verDTD;
}
void DeviceConfig::setVerDTD(const char* v){
	set(&verDTD, v);
}

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

BOOL DeviceConfig::getUtc() const {
    return utc;
}
void DeviceConfig::setUtc(BOOL v) {
    utc = v;
}

BOOL DeviceConfig::getLoSupport() const {
    return loSupport;
}
void DeviceConfig::setLoSupport(BOOL v) {
    loSupport = v;
}

BOOL DeviceConfig::getNocSupport() const {
    return nocSupport;
}
void DeviceConfig::setNocSupport(BOOL v) {
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


/*
 * Sets the values of this object with with the values from the given
 * DeviceConfig source object.
 *
 * @param config the new value.
 */
void DeviceConfig::assign(const DeviceConfig& s) {

    setVerDTD       (s.getVerDTD()        );
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
}

