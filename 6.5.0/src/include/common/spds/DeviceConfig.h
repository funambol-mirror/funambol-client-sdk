/*
 * Copyright (C) 2003-2007 Funambol, Inc
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
#ifndef INCL_DEVICE_CONFIG
#define INCL_DEVICE_CONFIG
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "syncml/core/VerDTD.h"
#include "base/Log.h"

/*
 * ---------------------------- DeviceConfig class -------------------------------
 * This class groups all configuration properties related to the device.
 * Most of DeviceConfig properties are used to generate the
 * <DevInf> element for client capabilities.
 * DeviceConfig is a part of SyncManagerConfig (along with AccessConfig
 * and an array of SyncSourceConfig).
 *
 * Class members:
 * --------------
 * verDTD       : Specifies the major and minor version identifier of the
 *                Device Information DTD used in the representation
 *                of the Device Information. The value MUST be "1.1".
 *                This property is mandatory.
 * man          : Specifies the name of the manufacturer of the device.
 *                This property is optional.
 * mod          : Specifies the model name or model number of the device.
 *                This property is optional.
 * oem          : Specifies the OEM (Original Equipment Manufacturer) of the device.
 *                This property is optional.
 * fwv          : Specifies the firmware version of the device.
 *                This property is optional.
 * swv          : Specifies the software version of the device.
 *                This property is optional.
 * hwv          : Specifies the hardware version of the device.
 *                This property is optional.
 * devID        : Specifies the identifier of the source synchronization device.
 *                The content information MUST specify a theoretically,
 *                globally unique identifier. This property is mandatory.
 * devType      : Specifies the type of the source synchronization device.
 *                Type values for this element type can be e.g. "pager",
 *                "handheld", "pda", "phone", "smartphone", "server", "workstation".
 *                Other values can also be specified. This property is mandatory.
 * dsV          : Specifies the implemented DS version. This property is optional.
 * utc          : Boolean. Specifies that the device supports UTC based time.
 *                If utc = TRUE, the server SHOULD send time in UTC format,
 *                else MUST send in local time. Default value = TRUE.
 * loSupport    : Boolean. Specifies that the device supports handling of large objects.
 *                Default value = FALSE.
 * nocSupport   : Boolean. Specifies that the device supports number of changes.
 *                Default value = FALSE.
 * logLevel     : Specifies the logging level on the device.
 *                It can be one of 0 – 1 – 2 (none, info, debug).
 *                Default value = 1 (info).
 * maxObjSize   : Specifies the maximum object size allowed by the device.
 *                Default value = 0 (no maxObjSize set).
 * devInfHash   : This is a hash value generated from all properties that are used
 *                for the <DevInf> element, plus the syncURL property from AccessConfig.
 *                Initial value = "0".
 */
class DeviceConfig {

    private:

        char*         verDTD       ;
        char*         man          ;
        char*         mod          ;
        char*         oem          ;
        char*         fwv          ;
        char*         swv          ;
        char*         hwv          ;
        char*         devID        ;
        char*         devType      ;
        char*         dsV          ;
        BOOL          utc          ;
        BOOL          loSupport    ;
        BOOL          nocSupport   ;
        LogLevel      logLevel     ;
        unsigned int  maxObjSize   ;
        char*         devInfHash   ;


        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(char* * buf, const char*  v);


    public:

        DeviceConfig();
        DeviceConfig(DeviceConfig& s);
        ~DeviceConfig();


        /**
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
        const char*  getVerDTD() const         ;
        void setVerDTD(const char*  v)         ;

        const char*  getMan() const            ;
        void setMan(const char*  v)            ;

        const char*  getMod() const            ;
        void setMod(const char*  v)            ;

        const char*  getOem() const            ;
        void setOem(const char*  v)            ;

        const char*  getFwv() const            ;
        void setFwv(const char*  v)            ;

        const char*  getSwv() const            ;
        void setSwv(const char*  v)            ;

        const char*  getHwv() const            ;
        void setHwv(const char*  v)            ;

        const char*  getDevID() const          ;
        void setDevID(const char*  v)          ;

        const char*  getDevType() const        ;
        void setDevType(const char*  v)        ;

        const char*  getDsV() const            ;
        void setDsV(const char*  v)            ;

        BOOL getUtc() const                    ;
        void setUtc(BOOL v)                    ;

        BOOL getLoSupport() const              ;
        void setLoSupport(BOOL v)              ;

        BOOL getNocSupport() const             ;
        void setNocSupport(BOOL v)             ;

        LogLevel getLogLevel() const           ;
        void setLogLevel(LogLevel v)           ;

        unsigned int getMaxObjSize() const     ;
        void setMaxObjSize(unsigned int v)     ;

        const char*  getDevInfHash() const     ;
        void setDevInfHash(const char *v)     ;

        /**
         * Sets the values of this object with with the values from the given
         * DeviceConfig source object.
         *
         * @param s: the deviceConfig reference.
         */
        void assign(const DeviceConfig& s);

        /*
         * Assign operator
         */
        DeviceConfig& operator = (const DeviceConfig& dc) {
            assign(dc);
            return *this;
        }
};

/** @endcond */
#endif
