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


#ifndef INCL_DEVINF
#define INCL_DEVINF
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/VerDTD.h"
#include "syncml/core/DataStore.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/Ext.h"


class DevInf {

     // ------------------------------------------------------------ Private data
    private:
        VerDTD* verDTD;
        char*  man;
        char*  mod;
        char*  oem;
        char*  fwV;
        char*  swV;
        char*  hwV;
        char*  devID;
        char*  devTyp;
        ArrayList* dataStores;  //DataStore[]
        ArrayList* ctCap;       // CTCap[]
        ArrayList* ext;         // Ext[]
        BOOL utc;
        BOOL supportLargeObjs;
        BOOL supportNumberOfChanges;
        SyncCap* syncCap;

        void initialize();

    // ---------------------------------------------------------- Public data
    public:

        DevInf();
        ~DevInf();

        /**
         * Creates a new DevInf object with the given parameter
         *
         * @param verDTD the DTD version - NOT NULL
         * @param man the device manufacturer
         * @param mod the device model name
         * @param oem the device OEM
         * @param fwV the device firmware version
         * @param swV the device software version
         * @param hwV the device hardware version
         * @param devID the device ID - NOT NULL
         * @param devTyp the device type - NOT NULL
         * @param dataStores the array of datastore - NOT NULL
         * @param ctCap the array of content type capability - NOT NULL
         * @param ext the array of extension element name - NOT NULL
         * @param utc is true if the device supports UTC based time
         * @param supportLargeObjs is true if the device supports handling of large objects
         * @param supportNumberOfChanges is true if the device supports number of changes
         *
         */
        DevInf(VerDTD* verDTD,
               const  char*  man,
               const char*  mod,
               const char*  oem,
               const char*  fwV,
               const char*  swV,
               const char*  hwV,
               const char*  devID,
               const char*  devTyp,
               ArrayList* dataStores,
               ArrayList* ctCap,
               ArrayList* ext,
               BOOL utc,
               BOOL supportLargeObjs,
               BOOL supportNumberOfChanges,
               SyncCap* syncCap);

        // ---------------------------------------------------------- Public methods
        /**
         * Gets the DTD version property
         *
         * @return the DTD version property
         */
        VerDTD* getVerDTD();

        /**
         * Sets the DTD version property
         *
         * @param verDTD the DTD version
         */
        void setVerDTD(VerDTD* verDTD);

        /**
         * Gets the device manufacturer property
         *
         * @return the device manufacturer property
         */
        const char* getMan();

        /**
         * Sets the device manufacturer property
         *
         * @param man the device manufacturer property
         *
         */
        void setMan(const char*  man);

        /**
         * Gets the model name of device
         *
         * @return the model name of device
         */
        const char* getMod();

        /**
         * Sets the device model property
         *
         * @param mod the device model property
         *
         */
        void setMod(const char*  mod);

        /**
         * Gets the Original Equipment Manufacturer of the device
         *
         * @return the OEM property
         */
        const char* getOEM();

        /**
         * Sets the Original Equipment Manufacturer of the device
         *
         * @param oem the Original Equipment Manufacturer of the device
         *
         */
        void setOEM(const char*  oem);

        /**
         * Gets the firmware version property
         *
         * @return the firmware version property
         */
        const char* getFwV();

        /**
         * Sets the firmware version property
         *
         * @param fwV the firmware version property
         *
         */
        void setFwV(const char*  fwV);

        /**
         * Gets the software version property
         *
         * @return the software version property
         */
        const char* getSwV();

        /**
         * Sets the software version property
         *
         * @param swV the software version property
         *
         */
        void setSwV(const char*  swV);

        /**
         * Gets the hardware version property
         *
         * @return the hardware version property
         */
        const char* getHwV();

        /**
         * Sets the hardware version property
         *
         * @param hwV the hardware version property
         *
         */
        void setHwV(const char*  hwV);

        /**
         * Gets the device identifier
         *
         * @return the device identifier
         */
        const char* getDevID();

        /**
         * Sets the device identifier
         *
         * @param devID the device identifier
         *
         */
        void setDevID(const char*  devID);

        /**
         * Gets the device type
         *
         * @return the device type
         */
        const char* getDevTyp();

        /**
         * Sets the device type
         *
         * @param devTyp the device type
         *
         */
        void setDevTyp(const char*  devTyp);

        /**
         * Gets the array of datastore
         *
         * @return the array of datastore
         */
        ArrayList* getDataStore();

        /**
         * Sets an array of datastore
         *
         * @param dataStores an array of datastore
         *
         */
        void setDataStore(ArrayList* dataStores);
        /**
         * Gets the array of content type capability
         *
         * @return the array of content type capability
         */
        ArrayList* getCTCap();

        /**
         * Sets an array of content type capability
         *
         * @param ctCap an array of content type capability
         *
         */
        void setCTCap(ArrayList* ctCap);

        /**
         * Gets the array of extension
         *
         * @return the array of extension
         */
        ArrayList* getExt();

        /**
         * Sets an array of extensions
         *
         * @param ext an array of extensions
         *
         */
        void setExt(ArrayList* ext);

        /**
         * Gets true if the device supports UTC based time
         *
         * @return true if the device supports UTC based time
         */
        BOOL isUTC();

        /**
         * Sets the UTC property
         *
         * @param utc is true if the device supports UTC based time
         */
        void setUTC(BOOL utc);

        /**
         * Gets the Boolean value of utc
         *
         * @return true if the device supports UTC based time
         */
        BOOL getUTC();

        /**
         * Gets true if the device supports handling of large objects
         *
         * @return true if the device supports handling of large objects
         */
        BOOL isSupportLargeObjs();

        /**
         * Sets the supportLargeObjs property
         *
         * @param supportLargeObjs is true if the device supports handling of large objects
         *
         */
        void setSupportLargeObjs(BOOL supportLargeObjs);


        /**
         * Gets the Boolean value of supportLargeObjs
         *
         * @return true if the device supports handling of large objects
         */
        BOOL getSupportLargeObjs();

        /**
         * Gets true if the device supports number of changes
         *
         * @return true if the device supports number of changes
         */
        BOOL isSupportNumberOfChanges();

        /**
         * Sets the supportNumberOfChanges property
         *
         * @param supportNumberOfChanges is true if the device supports number of changes
         *
         */
        void setSupportNumberOfChanges(BOOL supportNumberOfChanges);

        /**
         * Gets the Boolean value of supportNumberOfChanges
         *
         * @return true if the device supports number of changes
         */
        BOOL getSupportNumberOfChanges();

        void setSyncCap(SyncCap* syncCap);

        SyncCap* getSyncCap();

        DevInf* clone();

};

/** @endcond */
#endif
