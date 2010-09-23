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

#ifndef INCL_UPDATE_CONFIG
#define INCL_UPDATE_CONFIG
/** @cond DEV */

#include "base/fscapi.h"
#include "client/DMTClientConfig.h"
#include "spdm/ManagementNode.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/// This is the context for the updater parameters
#define CONTEXT_UPDATER                "/updater"    


class UpdaterConfig {

private:

    StringBuffer    version;         //  = "-1";    
    StringBuffer    currentVersion;  //  = "-1";
    StringBuffer    recommended;     // = "0"       Note: this string MUST be set
    StringBuffer    activationDate;     // = "0";
    StringBuffer    urlUpdate;       // = "";
    StringBuffer    urlComment;      // = "";
    StringBuffer    urlCheck;        // = "";
    size_t          size;            // = "0";
    StringBuffer    lastCheckUpdate; // = "0";  // the last time there was a check of the update
    uint32_t        intervalCheckUpdateHttp; // = "1440"; // 24 hours
    uint32_t        intervalRemind;  //  = "120";
    StringBuffer    lastUpdated;     //     = "0";  
    StringBuffer    skipped;         // false         = "0";
    StringBuffer    later;           // false           = "0";
    StringBuffer    now;             // false           = "0";
    StringBuffer    updateType;     // UP_TYPE_OPTIONAL, UP_TYPE_RECOMMENDED, UP_TYPE_MANDATORY
    StringBuffer    nextRemindCheck;

    StringBuffer applicationUri;

    static StringBuffer versionPropertyName;
    static StringBuffer recommendedPropertyName;
    static StringBuffer activationDatePropertyName;
    static StringBuffer urlCheckPropertyName;
    static StringBuffer urlUpdatePropertyName;
    static StringBuffer urlCommentPropertyName;
    static StringBuffer sizePropertyName;
    static StringBuffer lastCheckUpdatePropertyName;
    static StringBuffer intervalCheckUpdateHttpPropertyName;
    static StringBuffer intervalRemindPropertyName;
    static StringBuffer lastUpdatedPropertyName;
    static StringBuffer skippedPropertyName;
    static StringBuffer laterPropertyName;
    static StringBuffer nowPropertyName;
    static StringBuffer updateTypePropertyName;
    static StringBuffer nextRemindCheckPropertyName;

public:        
   
    /**
    * Constructor
    */
    UpdaterConfig(const StringBuffer appUri);
    
    /**
    * Destructor
    */
    ~UpdaterConfig();

    /**
    * Read the parameter configuration from the DM
    */
    bool read();
    
    /**
    * Save the parameter configuration to the registry.
    * Actually the only needed parameter to be saved is the
    * nonce sent by the server
    */
    void save();
    
    /**
     * Creates the default configuration.
     * Overwrites all parameters with the default values, no save is done.
     */
    void createDefaultConfig();

    uint32_t getIntervalRemind() const;
    uint32_t getIntervalCheckUpdateHttp() const;

    const StringBuffer& getLastCheckUpdate() const;
    void  setLastCheckUpdate(const StringBuffer& time);

    void  setVersion(const StringBuffer& version);
    const StringBuffer& getVersion() const;

    void  setCurrentVersion(const StringBuffer& v);
    const StringBuffer& getCurrentVersion() const;

    void  setRecommended(const StringBuffer& recommended);
    const StringBuffer& getRecommended() const;

    void  setUrlCheck(const StringBuffer& urlCheck);
    const StringBuffer& getUrlCheck() const;

    void  setSkipped(const StringBuffer& skipped);
    const StringBuffer& getSkipped() const;

    void  setLater(const StringBuffer& later);
    const StringBuffer& getLater() const;

    void  setNow(const StringBuffer& now);
    const StringBuffer& getNow() const;

    void  setUpdateType(const StringBuffer& type);
    const StringBuffer& getUpdateType() const;

    void  setUrlUpdate(const StringBuffer& urlUpdate);
    const StringBuffer& getUrlUpdate() const;

    void  setReleaseDate(const StringBuffer& releaseData);
    const StringBuffer& getReleaseDate() const;
    
    void  setNextRemindCheck(const StringBuffer& time);
    const StringBuffer& getNextRemindCheck() const;

    void  setSize(uint32_t size);
    void  setUrlComment(const StringBuffer& comment);
};

END_NAMESPACE

/** @endcond */
#endif
