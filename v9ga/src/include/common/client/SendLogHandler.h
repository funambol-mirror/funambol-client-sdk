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

#ifndef INCL_SENDLOG_HANDLER
#define INCL_SENDLOG_HANDLER

#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"
#include "http/HttpConnection.h"
#include "spds/SyncManagerConfig.h"

BEGIN_FUNAMBOL_NAMESPACE


class SendLogHandler {
    
    SyncManagerConfig& config;
    
    StringBuffer serverURL;     // 
    
    StringBuffer composeURL(const char* url);
    void setRequestHeaders(HttpConnection& httpConnection, 
                           InputStream& inputStream);
    
    StringBuffer convertBoolToStringBuffer(bool v);    
    StringBuffer convertLongToStringBuffer(unsigned long val);
    
    
public:
    /**
     * Constructor.
     */
    SendLogHandler(SyncManagerConfig& conf);
    
    /**
     * Populate a standard header that can be attached to the log to be sent.
     * It uses info that are in the SyncManagerConfig. If the client want to use them, 
     * it can get them and add into its own InputStream before passing to the sendLog method
     */
    virtual StringBuffer createLogHeader();
    
    /**
     * send really the log to the server service
     */
    int sendLog(InputStream& log);
    
    /**
     * Uses this complete serverURL where to send the log
     * It could be something like http://log.funambol.com/client-log
     */
    void setServerLogURL(const char* serURL) { serverURL = serURL; }
};

END_FUNAMBOL_NAMESPACE
#endif