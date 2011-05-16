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

#ifndef INCL_SAPI_SYNC_ITEM_INFO
#define INCL_SAPI_SYNC_ITEM_INFO
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/ArrayElement.h"
#include "base/globalsdef.h"
#include "ioStream/FileInputStream.h"
#include "ioStream/FileOutputStream.h"

BEGIN_NAMESPACE

/**
 *
 */
class SapiSyncItemInfo : public ArrayElement {

private:

    StringBuffer guid;
    StringBuffer luid;
    StringBuffer name;
    
    StringBuffer contentType;
    size_t size;
    StringBuffer serverUrl;
    StringBuffer url;
   
    time_t creationDate;
    time_t modificationDate;
    bool rename;
    
public:

    /**
     * Initializes the SyncItemInfo
     * @param guid  const char*
     * @param luid  const char*
     * @param name  const char*
     * @param size  size_t
     * @param serverUrl const char* (server name)
     * @param url   const char*     (url path relative to server name)
     * @param contentType const char* (mime content type)
     * @param creationDate      time_t
     * @param modificationDate  time_t
     */
    SapiSyncItemInfo(const char* guid, const char* luid, const char* name, size_t size, const char* serverUrl, const char* url, const char* contentType, 
                    time_t creationDate, time_t modificationDate, bool rename = false)
    {
        if (guid) {
            this->guid = guid;
        } else {
            this->guid = "";
        }
        if (luid) {
            this->luid = luid;
        } else {
            this->luid = "";
        }

        if (name) {
            this->name = name;
        } else {
            this->name = "";
        }
        
        this->size = size;
        
        if (serverUrl) {
            this->serverUrl = serverUrl;
        } else {
            this->serverUrl = "";
        }
        
        if (url) {
            this->url = url;
        } else {
            this->url = "";
        }
        
        if (contentType) {
            this->contentType = contentType; 
        } else {
            this->contentType = "";
        }
        
        this->creationDate = creationDate;
        this->modificationDate = modificationDate;
        this->rename = rename;
    }
    
    // default ctor
    SapiSyncItemInfo(){
        guid = "";
        luid = "";
        contentType = "";
        size = 0;
        
        creationDate = 0;   
        modificationDate = 0;
        rename = false;
    }

    SapiSyncItemInfo(const char* guid, const char* luid) {
        if (guid) {
            this->guid = guid;
        } else {
            guid = "";
        }
        if (luid) {
            this->luid = luid;
        } else {
            luid = "";
        }
        
        contentType = "";
        size = 0;
        
        creationDate = 0;       
        modificationDate = 0;
        rename = false;
    }

    ~SapiSyncItemInfo() {}
    
    void setGuid(const char* guid){
        if (guid) {
            this->guid = guid;
        } else {
            guid = "";
        }
        
    }

    StringBuffer& getGuid() {
        return guid;
    }

    void setLuid(const char* luid){
        if (luid) {
            this->luid = luid;
        } else {
            luid = "";
        }
    }

    StringBuffer& getLuid() {
        return luid;
    }

    void setName(const char* name){
        if (name) {
            this->name = name;
        } else {
            name = "";
        }
    }

    StringBuffer& getName() {
        return name;
    }

    void setSize(size_t size) {
        this->size = size;
    }

    size_t getSize() const {
        return size;
    }

    void setServerUrl(const char* url){
        if (url) {
            this->serverUrl = url;
        } else {
            serverUrl = "";
        }        
    }

    StringBuffer& getServerUrl() {
        return serverUrl;
    }

    void setUrl(const char* url){
        if (url) {
            this->url = url;
        } else {
            url = "";
        }        
    }

    StringBuffer& getUrl() {
        return url;
    }
    
    void setContentType(const char* contentType) {
        if (contentType) {
            this->contentType = contentType;
        } else {
            contentType = "";
        }
    }
    
    StringBuffer& getContentType() {
        return contentType;
    }
    
    void setCreationDate(time_t date){
        this->creationDate = date;
    }
    
    time_t getCreationDate() const {
        return creationDate;
    }
        
    void setModificationDate(time_t date){
        this->modificationDate = date;
    }
    
    time_t getModificationDate() const {
        return modificationDate;
    }
    
    bool isRename() {
        return rename;
    }

    void setRename(bool val) {
        rename = val;
    }

    /**
     * Creates a new instance of FileSyncItem from the content of this
     * object. The new instance is created the the C++ new operator and
     * must be removed with the C++ delete operator.
     */
    ArrayElement* clone(){
        SapiSyncItemInfo* ret = new SapiSyncItemInfo(getGuid().c_str(),
                                                     getLuid().c_str(),
                                                     getName().c_str(),
                                                     getSize(),
                                                     getServerUrl().c_str(),
                                                     getUrl().c_str(),
                                                     getContentType().c_str(), 
                                                     getCreationDate(),
                                                     getModificationDate(),
                                                     isRename());
        return ret;
    }

};

END_NAMESPACE

/** @endcond */
#endif
