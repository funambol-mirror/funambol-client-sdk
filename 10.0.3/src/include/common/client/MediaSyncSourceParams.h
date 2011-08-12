/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
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

#ifndef MEDIASOURCESYNCPARAMS_H_
#define MEDIASOURCESYNCPARAMS_H_

#include "base/fscapi.h"
#include "spds/constants.h"


BEGIN_NAMESPACE

#define MEDIA_CACHE_FILE_NAME   "funambol_cache.dat"
#define MEDIA_LUID_MAP_FILE_NAME "funambol_luid.dat"

#define CACHE_PROPERTY_URL      "_SERVER_URL_"
#define CACHE_PROPERTY_USERNAME "_USERNAME_"
#define CACHE_PROPERTY_SWV      "_CLIENT_SWV_"

#define CONFIG_PROPS_EXT         "_params.ini"      // config props file will be "<sourcename>_params.ini"
#define PROPERTY_NEXT_LUID       "nextLUID"


/**
 * Container for parameters used by this MediaSyncSource class.
 * Server URL, Username and Swv are stored inside the MediaSyncSource cache to check 
 * its validity before every sync.
 * filterBySize and filterByDate can be set to specify dynamic file filtering.
 */
class MediaSyncSourceParams
{
private:
    StringBuffer url;           /**< The Sync Server URL. */
    StringBuffer username;      /**< The current username. */
    StringBuffer password;      /**< The current password. */
    StringBuffer swv;           /**< The current Client software version. */
    StringBuffer deviceID;      /**< The device id. */
    StringBuffer userAgent;     /**< The user agent. */
    
    /** 
     * Incremental number, used as the next LUID of media items.
     * The MediaSyncSource will use (and then increment) this value to send a unique
     * item's key to the Server.
     */
    int nextLUID;
    
    /**
     * Can be set to enable a filter on media files (outgoing items only).
     * If not 0, media files with size > filterBySize will be filtered out (not synced)
     * The value is expressed in KBytes.
     */
    unsigned int filterBySize;
    
    /**
     * Can be set to enable a filter on media files (outgoing items only).
     * If not 0, media files modified AFTER this date will be filtered out (not synced)
     * The value is a unix timestamp, in UTC (seconds since 1970-01-01)
     */
    unsigned long filterByDate;
    
public:
    MediaSyncSourceParams()  { 
        nextLUID     = 0;
        filterBySize = 0;
        filterByDate = 0;
    }
    ~MediaSyncSourceParams() {};
    
    const StringBuffer& getUrl()          { return url;          }
    const StringBuffer& getUsername()     { return username;     }
    const StringBuffer& getPassword()     { return password;     }
    const StringBuffer& getSwv()          { return swv;          }
    const StringBuffer& getDeviceID()     { return deviceID;     }
    const StringBuffer& getUserAgent()    { return userAgent;    }
    const int           getNextLUID()     { return nextLUID;     }
    const unsigned int  getFilterBySize() { return filterBySize; }
    const unsigned long getFilterByDate() { return filterByDate; }
    
    void setUrl          (const StringBuffer& v) { url          = v; }
    void setUsername     (const StringBuffer& v) { username     = v; }
    void setPassword     (const StringBuffer& v) { password     = v; }
    void setSwv          (const StringBuffer& v) { swv          = v; }
    void setDeviceID     (const StringBuffer& v) { deviceID     = v; }
    void setUserAgent    (const StringBuffer& v) { userAgent    = v; }
    void setNextLUID     (const int           v) { nextLUID     = v; }
    void setFilterBySize (const unsigned int  v) { filterBySize = v; }
    void setFilterByDate (const unsigned long v) { filterByDate = v; }
};



END_NAMESPACE

#endif /*MEDIASOURCESYNCPARAMS_H_*/
