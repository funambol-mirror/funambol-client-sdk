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



#ifndef INCL_ABSTRACT_SYNC_SOURCE_CONFIG
    #define INCL_ABSTRACT_SYNC_SOURCE_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */

    #include "base/fscapi.h"
    #include "spds/constants.h"
    #include "syncml/core/CTCap.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class is the API expected by SyncClient/SyncManager for each sync source.
 * AbstractSyncSourceConfig is a part of SyncManagerConfig (along with AccessConfig
 * and an array of DeviceConfig).
 */
class AbstractSyncSourceConfig {
 public:
    static const int FLEVEL_UNDEFINED  =  -1;
    static const int FLEVEL_ENABLED    =   1;
    static const int FLEVEL_DISABLED   =   0;

    /**
     * Destructor
     */
    virtual ~AbstractSyncSourceConfig() {}

    /**
     * Returns the SyncSource name.
     */
    virtual const char*  getName() const = 0;

    /**
     * Returns the SyncSource URI (used in SyncML addressing).
     */
    virtual const char*  getURI() const = 0;

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
    virtual const char*  getSyncModes() const = 0;

    /**
     * Returns the mime type of the items handled by the sync source.
     */
    virtual const char* getType() const = 0;

    /**
     * Gets the default syncMode as one of the strings listed in setSyncModes.
     */
    virtual const char*  getSync() const = 0;
    
    /**
     * Specifies how the content of an outgoing item should be
     * encoded by the client library if the sync source does not
     * set an encoding on the item that it created. Valid values
     * are listed in SyncItem::encodings.
     */
    virtual const char*  getEncoding() const = 0;

    /**
     * Returns the version of the source type used by client.
     */
    virtual const char* getVersion() const = 0;

    /**
     * A string representing the source types (with versions) supported by the SyncSource.
     * The string must be formatted as a sequence of "type:version" separated by commas ','.
     * For example: "text/x-vcard:2.1,text/vcard:3.0".
     * The version can be left empty, for example: "text/x-s4j-sifc:".
     * Supported types will be sent as part of the DevInf.
     */
    virtual const char* getSupportedTypes() const = 0;

    /**
     * Sets the last sync timestamp. Called by the sync engine at
     * the end of a sync. The client must save that modified
     * value; it is needed to decide during the next sync whether
     * an incremental sync is possible.
     *
     * A client which wants to force a slow sync after a failed
     * sync can reset the time stamp. The sync engine itself won't
     * do that.
     *
     * @param timestamp the last sync timestamp
     */
    virtual void setLast(unsigned long timestamp) = 0;

    /**
     * Returns the last sync timestamp
     */
    virtual unsigned long getLast() const = 0;

    /**
     * Specifies if the content of an outgoing item should be encrypted.
     * If this property is not empty and valid, the 'encodings' value is ignored
     * for outgoing items. The only valid value is "des".
     */
    virtual const char* getEncryption() const = 0;

    /**
     * Returns an array of CtCap with all the capabilities for this Source
     *
     * @return an ArrayList of CTCap
     */
    virtual const ArrayList& getCtCaps() const = 0;
    
    /**
     * Return fieldLevel param. Not implemented yet. Now just returns false
     *
     * Note: explaining the method "getFieldLevel()" as "return
     * fieldLevel param" does not add any information and might as
     * well be left out.
     *
     * @return fieldLevel ... which is what?
     */
    virtual bool getFieldLevel() const { return false; }


    /// Returns the sync source's error code on the last sync.
    virtual int getLastSourceError() const { return 0; };

    /// Sets the sync source's last error code.
    virtual void setLastSourceError(const int /*errorCode*/) {};

};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
