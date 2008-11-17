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



#ifndef INCL_SYNC_SOURCE_CONFIG
    #define INCL_SYNC_SOURCE_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

    #include "base/fscapi.h"
    #include "spds/constants.h"
    #include "syncml/core/CTCap.h"


/**
 * This class groups all configuration properties for a SyncSource.
 * SyncSourceConfig is a part of SyncManagerConfig (along with AccessConfig
 * and an array of DeviceConfig).
 */
class SyncSourceConfig {

    protected:

        char*  name          ;
        char*  uri           ;
        char*  syncModes     ;
        char*  type          ;
        char*  sync          ;
        char*  encodings     ;
        char*  version       ;
        char*  supportedTypes;
        CTCap  ctCap         ;
        char*  encryption    ;

        unsigned long last;

    public:

        /**
         * Constructs a new SyncSourceConfig object
         */
        SyncSourceConfig();

        /**
         * Destructor
         */
        ~SyncSourceConfig();

        /**
         * Returns the SyncSource name.
         */
        const char*  getName() const;

        /**
         * Sets the SyncSource name
         *
         * @param n the new name
         */
        void setName(const char*  n);


        /**
         * Returns the SyncSource URI (used in SyncML addressing).
         */
        const char*  getURI() const;

        /**
         * Sets the SyncSource URI (used in SyncML addressing).
         *
         * @param u the new uri
         */
        void setURI(const char*  u);

        /**
         * Returns a comma separated list of the possible syncModes for the
         * SyncSource. Sync modes can be one of
         * - slow
         * - two-way
         * - one-way-from-server
         * - one-way-from-client
         * - refresh-from-server
         * - refresh-from-client
         * - one-way-from-server
         * - one-way-from-client
         * - addrchange
         */
        const char*  getSyncModes() const;

        /**
         * Sets the available syncModes for the SyncSource as comma separated
         * values.
         *
         * @param s the new list
         */
        void setSyncModes(const char*  s);

        /**
         * Returns the mime type of the items handled by the sync source.
         */
        const char*  getType() const;

        /**
         * Sets the mime type of the items handled by the sync source.
         *
         * @param t the mime type
         */
        void setType(const char*  t);

        /**
         * Gets the default syncMode as one of the strings listed in setSyncModes.
         */
        const char*  getSync() const;

        /**
         * Returns the default syncMode as one of the strings above.
         */
        void setSync(const char*  s);

        /**
         * Specifies how the content of an outgoing item should be
         * encoded by the client library if the sync source does not
         * set an encoding on the item that it created. Valid values
         * are listed in SyncItem::encodings.
         */
        const char*  getEncoding() const;
        void setEncoding(const char*  s);


        /**
         * Returns the version of the source type used by client.
         */
        const char*  getVersion() const;

        /**
         * Sets the SyncSource version
         *
         * @param n the new version
         */
        void setVersion(const char*  n);


        /**
         * A string representing the source types (with versions) supported by the SyncSource.
         * The string must be formatted as a sequence of "type:version" separated by commas ','.
         * For example: "text/x-vcard:2.1,text/vcard:3.0".
         * The version can be left empty, for example: "text/x-s4j-sifc:".
         * Supported types will be sent as part of the DevInf.
         */
        const char*  getSupportedTypes() const;

        /**
         * Sets the supported source types for this source.
         *
         * @param s the supported types string
         */
        void setSupportedTypes(const char*  s);

        CTCap getCtCap() const          ;
        void setCtCap(CTCap v)          ;

        /**
         * Sets the last sync timestamp
         *
         * @param timestamp the last sync timestamp
         */
        void setLast(unsigned long timestamp);

        /**
         * Returns the last sync timestamp
         */
        unsigned long getLast() const;

        /**
         * Specifies if the content of an outgoing item should be encrypted.
         * If this property is not empty and valid, the 'encodings' value is ignored
         * for outgoing items. The only valid value is "des".
         */
        const char* getEncryption() const;

        /**
         * Sets the encryption type
         *
         * @param n the encryption type
         */
        void setEncryption(const char* n);

        /**
         * Initialize this object with the given SyncSourceConfig
         *
         * @param sc the source config object
         */
        void assign(const SyncSourceConfig& sc);

        /**
         * Assign operator
         */
        SyncSourceConfig& operator = (const SyncSourceConfig& sc) {
            assign(sc);
            return *this;
        }
    };

/** @} */
/** @endcond */
#endif
