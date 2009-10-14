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

#ifndef INCL_SYMBIAN_LOG
#define INCL_SYMBIAN_LOG
/** @cond DEV */

#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include <f32file.h>
#include "base/globalsdef.h"

// Default values, used if the Client don't set them.
#define SYMBIAN_LOG_HEADER          "Funambol Symbian Sync Client Log"
#define SYMBIAN_LOG_PATH            "C:\\data\\"
#define SYMBIAN_LOG_NAME            "synclog.txt"

#define SYMBIAN_LOG_SIZE            (500 * 1024)  // bytes

#define ERR_SEMAPHORE_CREATION      30
#define ERR_SEMAPHORE_CREATION_MSG  "Error creating Log Semaphore"

BEGIN_NAMESPACE


class SymbianLog : public Log {

private:

    void printMessage(const char*  level, const char*  msg, PLATFORM_VA_LIST argList);
    void RollLogFile(void);
    TInt LogSize(void);
    
    /// The file server session.
    RFs fsSession;
    
    /// Use this object to represent the file to be written to.
    RFile file;
    
    /// The complete log name (default = SYMBIAN_LOG_NAME)
    RBuf iLogName;
    /// The complete roll log name (default = SYMBIAN_ROLL_LOG_NAME)
    RBuf iRollLogName;
    /// The log file name
    RBuf iLogFileName;
    /// The lof file path name
    RBuf iLogPathName;
    
    /**
     * Semaphore for the resource RFs, to avoid accessing the FileSystem at
     * the same time by different threads.
     */ 
    RSemaphore iSemaphore;

    
    StringBuffer iFormattedBias;
    
    void createFormattedBias();
    
    StringBuffer createHeader(const char*  title = NULL);
    
    
    /**
     * Returns the time to write into log file. If complete is true, it return
     * the date too, else only hours, minutes, seconds.
     */
    StringBuffer createCurrentTime(bool complete);
    
    
public:

    SymbianLog(bool reset = false, const char*  path = NULL, const char*  name = NULL);
    virtual ~SymbianLog();

    void setLogPath(const char*  configLogPath);
    void setLogName(const char*  configLogName);
    void reset     (const char*  title = NULL);

    void error    (const char* msg, ...);
    void info     (const char* msg, ...);
    void debug    (const char* msg, ...);
    void developer(const char* msg, ...);

    /// Returns the log file size [bytes].
    size_t getLogSize();
};


END_NAMESPACE

/** @endcond */
#endif  // INCL_SYMBIAN_LOG
