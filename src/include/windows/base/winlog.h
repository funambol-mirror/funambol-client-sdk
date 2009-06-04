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

#ifndef INCL_WIN_LOG
#define INCL_WIN_LOG
/** @cond DEV */
#include <base/fscapi.h>
#include <base/Log.h>
#include "base/util/StringBuffer.h"

#define LOG_ERROR "ERROR"
#define LOG_INFO  "INFO"
#define LOG_DEBUG "DEBUG"

#define LOG_NAME "synclog.txt"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

//typedef enum {
//    LOG_LEVEL_NONE  = 0,
//    LOG_LEVEL_INFO  = 1,
//    LOG_LEVEL_DEBUG = 2
//} LogLevel;


class winLog : public Log {

private:
    
    HANDLE wait();
    void signal(HANDLE hMutex);
    void printMessage(const char*  level, const char*  msg, va_list argList);
    
    /*
    * Which log level is set?
    */
    LogLevel logLevel;

public:

    winLog(bool reset = false, const char*  path = NULL, const char*  name = NULL);
    ~winLog();

    void setLogPath(const char*  configLogPath);
    void setLogName(const char*  configLogName);

    void error(const char*  msg, ...);
    void info(const char*  msg, ...);
    void debug(const char*  msg, ...);
    void developer(const char* msg, ...);

    void reset(const char*  title = NULL);

    /// Returns the log file size [bytes].
    size_t getLogSize();

    // FIXME!
    //void printMessageW(const char* level, const WCHAR* msg, va_list argList);

};

//extern Log LOG;
//#define LOG (winLog &)Log::instance()

END_NAMESPACE

/** @endcond */
#endif
