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
#ifndef INCL_DEVICE_CONFIG
#define INCL_DEVICE_CONFIG
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "syncml/core/VerDTD.h"
#include "base/Log.h"
#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "syncml/core/DataStore.h"

BEGIN_NAMESPACE

/*
 * ---------------------------- DeviceConfig class -------------------------------
 * This class groups all configuration properties related to the device.
 * Most of DeviceConfig properties are used to generate the
 * <DevInf> element for client capabilities.
 * DeviceConfig is a part of SyncManagerConfig (along with AccessConfig
 * and an array of SyncSourceConfig).
 */
class DeviceConfig {

    private:

        char*         man          ;
        char*         mod          ;
        char*         oem          ;
        char*         fwv          ;
        char*         swv          ;
        char*         hwv          ;
        char*         devID        ;
        char*         devType      ;
        char*         dsV          ;
        bool          utc          ;
        bool          loSupport    ;
        bool          nocSupport   ;
        LogLevel      logLevel     ;
        unsigned int  maxObjSize   ;
        char*         devInfHash   ;


		int          smartSlowSync;
        int          multipleEmailAccount;

		char*		 verDTD;
        bool         sendDevInfo;        // send device info on sync start (by default set to true)

        // Used only for ServerConfig:
        bool         forceServerDevInfo; // force to ask Server devInfo (by default set to false)
        char*        serverLastSyncURL;  // if ServerURL changed, Server devInf is invalid
        ArrayList*   dataStores;         // the array of Server DataStores.

        bool         mediaHttpUpload;    // if true, the server supports Media HTTP upload (only Server config)
        char*        noFieldLevelReplace;// if specified, it is provided a list of source uri. It specifies that
                                         // the item of the source is the complete one. So if the server
        /**
         * Dirty flag.
         * Used to execute the save operations only when really necessary.
         */
        unsigned int dirty;


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

        bool getUtc() const                    ;
        void setUtc(bool v)                    ;

        bool getLoSupport() const              ;
        void setLoSupport(bool v)              ;

        bool getNocSupport() const             ;
        void setNocSupport(bool v)             ;

        LogLevel getLogLevel() const           ;
        void setLogLevel(LogLevel v)           ;

        unsigned int getMaxObjSize() const     ;
        void setMaxObjSize(unsigned int v)     ;

        const char*  getDevInfHash() const     ;
        void setDevInfHash(const char *v)     ;

        int getSmartSlowSync() const;
        void setSmartSlowSync(int v);

        int getMultipleEmailAccount() const;
        void setMultipleEmailAccount(int v);

		const char*  getVerDTD() const        ;
        void setVerDTD(const char*  v)        ;

        bool getSendDevInfo() const             ;
        void setSendDevInfo(bool)               ;

        bool getForceServerDevInfo() const      ;
        void setForceServerDevInfo(bool)        ;

        const char* getServerLastSyncURL() const;
        void setServerLastSyncURL(const char *v);

        bool getMediaHttpUpload() const;
        void setMediaHttpUpload(bool v);

        const char* getNoFieldLevelReplace() const;
        void setNoFieldLevelReplace(const char *v);

        /**
         * Specifies the array of DataStores supported by Server (used only by ServerConfig).
         * Existing datastores will be replaced by these ones.
         * Passing a NULL pointer, will act as a reset. Will always set the dirty flag.
         * @param  dataStores  ArrayList of DataStore objects
         */
        void setDataStores(const ArrayList* dataStores);

        /**
         * Like setDataStores(NULL), but doesn't set the dirty flag.
         * This method is used when we just want to read again the dataStores, to avoid replacing
         * the same config. If you need to reset the dataStores and persist this change 
         * during save, please use setDataStores(NULL).
         */
        void resetDataStores();

        /** 
         * Returns the array of DataStores supported by Server (used only by ServerConfig).
         * @return  an ArrayList of DataStore objects
         */
        const ArrayList* getDataStores() const;

        /// Adds the passed dataStore (if nt NULL) to the array of dataStores.
        void addDataStore(DataStore* dataStore);

        /**
         * Returns a specific DataStore, given its name (sourceRef).
         * If not found, returns NULL.
         */
        DataStore* getDataStore(const char* sourceRef);

        /// Returns the value of dirty flag.
        unsigned int getDirty() const;

        /// Returns true if the passed flags are dirty for this object.
        bool isDirty(const unsigned int flags);

        /// Sets the dirty flag to the passed value. With flags=0 resets the dirty flag.
        void setDirty(const unsigned int flags);

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


END_NAMESPACE

/** @endcond */
#endif
