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



#ifndef INCL_SYNC_SOURCE_CONFIG
    #define INCL_SYNC_SOURCE_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

    #include "spds/AbstractSyncSourceConfig.h"
#include "base/globalsdef.h"
#include "base/util/StringMap.h"
#include "base/Log.h"

BEGIN_NAMESPACE

/**
 * This class is one possible implementation of the
 * AbstractSyncSourceConfig API: it implements all attributes as
 * read/write members.
 */
class SyncSourceConfig : public AbstractSyncSourceConfig {

    protected:

        char*  name             ;
        char*  uri              ;
        char*  syncModes        ;
        char*  type             ;
        char*  sync             ;
        char*  encodings        ;
        char*  version          ;
        char*  supportedTypes   ;
        char*  encryption       ;
        // last anchor, set by the engine.
        unsigned long last      ;
        // CTCap data
        bool fieldLevel         ;
        ArrayList ctCaps        ;
        
        /**
         * If false, the source exists but is disabled.
         * Usually a source can be disabled by the user from Client's UI, to avoid syncing it.
         * By default it's true.
         */
        bool enabled            ;

        /**
         * The last error code, for this source (0 means "last sync successfull").
         * If using the SyncClient object to trigger the sync, the last sync result
         * code of the corresponding source is set at the end of the sync.
         * This way the information will be available even after the sync ended.
         */
        int lastSourceError       ;

        /**
        * Create a new CTCap object, based on the current source configuration.
        * To obtain a complete CTCap, the client must have set previously the 
        * list of CTCap properties with the method setCtCapProperties().
        */
        CTCap* createCtCap(ArrayList *props, const char *ct_Type, const char *ver_CT, bool fLevel);

        /**
         * Contains the list of extra properties that the client wants to add.
         * They are strings but useful method of conversion are provided
         */
        StringMap extraProps;
    
    
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
         * - smart-one-way-from-client
         * - smart-one-way-from-server
         * - incremental-smart-one-way-from-client
         * - incremental-smart-one-way-from-server
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
        const char* getType() const;

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
        const char* getVersion() const;

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
        const char* getSupportedTypes() const;

        /**
         * Sets the supported source types for this source.
         *
         * @param s the supported types string
         */
        void setSupportedTypes(const char*  s);
        
        /**
         * Checks if the source is enabled
         * @return true if the source is enabled
         */
        bool isEnabled() const;
        
        /**
         * Sets the flag 'enabled' for this source
         * @param s  true to enable this source, false to disable it
         */
        void setIsEnabled(const bool s);

        /// Returns the sync source's error code on the last sync.
        int getLastSourceError() const;

        /// Sets the sync source's last error code.
        void setLastSourceError(const int errorCode);

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
         * Sets the list of properties to use in the CTCap sent to the server in
         * the DevInfo.
         *
         * @param props an ArrayList of Property 
         */
        //void setCtCapProperties(ArrayList* props);

        /**
         * Returns the list of properties to use in the CTCap sent to the server
         * in the DevInfo.
         *
         * @returns an ArrayList of Property
         */
        //ArrayList& getCtCapProperties();

        /**
         * Returns an array of CtCap with all the capabilities for this Source
         *
         * @return an ArrayList of CTCap
         */
        const ArrayList& getCtCaps() const {return ctCaps;};
        ArrayList& getCtCaps() {return ctCaps;};

        /**
         * add a CTCap to the ArrayList. If ctType, verCT and FieldLevel are null
         * we set a CTCap with the default verCT, fieldLevel and ctType taken
         * from the config
         *
         * the fieldLevel param has three status 
         *      FLEVEL_DISABLED   - to disable the FieldLevel param in the CtCap
         *      FLEVEL_ENABLED    - to enable the FieldLevel param in the CtCap
         *      FLEVEL_UNDEFINED  - not set. In this case the code sets the param to
         *                          the value in the config. Now setted to disable the FieldLevel param 
         *
         * @param ArrayList* props - the arrayList with the properties
         * @param const char* ct_Type - optional
         * @param const char* ver_CT - optional
         * @param int fieldLevel - three different status 
         */
        void  addCtCap(ArrayList* props, const char* ct_Type = 0 , const char* ver_CT = 0, int fLevel = FLEVEL_UNDEFINED);

        /**
         * return the reference of the current extraProps
         */
        const StringMap& getExtraProps() const { return extraProps; }
    
        /**
         * set a key/value couple
         */
        void setProperty(const char* propertyName, const char* propertyValue);
    
        /**
         * set a key/ int value couple
         */
        void setIntProperty(const char* propertyName, int propertyValue);
        
        /**
         * set a key/ bool value couple
         */
        void setBoolProperty(const char* propertyName, bool propertyValue);
        
        /**
         * set a key/ long value couple
         */
        void setLongProperty(const char* propertyName, long propertyValue);
        
        /**
         * get a value from a given key. 
         * @return NULL if the key is not found
         */
        const char* getProperty(const char* propertyName);
        
        
        /**
         * get a int value from a given key. 
         * @return -1 and err = false if the key is not found
         */
        int getIntProperty(const char* propertyName, bool* err);
    
        
        /**
         * get a bool value from a given key. 
         * @return false and err = false if the key is not found
         */
        bool getBoolProperty(const char* propertyName, bool* err);
        
    
        /**
         * get a long value from a given key. 
         * @return -1 and err = false if the key is not found
         */
        long getLongProperty(const char* propertyName, bool* err);
    
    
        /**
         * Initialize this object with the given SyncSourceConfig
         *
         * @param sc the source config object
         */
        void assign(const SyncSourceConfig& sc);

        /**
         * Return fieldLevel param. Not implemented yet. Now just returns false
         *
         * @return bool fieldLevel
         */
        bool getFieldLevel() const { return false;}

        /**
         * Assign operator
         */
        SyncSourceConfig& operator = (const SyncSourceConfig& sc) {
            assign(sc);
            return *this;
        }
        /**
         * Useful mdebug method to log the values in the extra prop StringMap
         */
        void printStringMap() {
            if (extraProps.empty()) {
                LOG.debug("[%s] StringMap is empty", getName());
            }
            KeyValuePair kvp = (KeyValuePair)extraProps.front(); 
            while (kvp.null() == false) {
                LOG.debug("[%s] key: %s, value: %s", getName(), kvp.getKey().c_str(), kvp.getValue().c_str());
                kvp = extraProps.next();
            }
        }
    
    };


END_NAMESPACE

/** @} */
/** @endcond */
#endif
