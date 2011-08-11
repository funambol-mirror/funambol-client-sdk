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


#include "base/winlog.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/util/WString.h"
#include "base/globalsdef.h"

USE_NAMESPACE

#ifdef _WIN32_WCE
#define FUNAMBOL_HEADER "Funambol Windows Mobile Client Log"
#elif WIN32
#define FUNAMBOL_HEADER "Funambol Win32 Client Log"
#endif


static FILE* logFile = NULL;

static StringBuffer logDir("\\");
static StringBuffer logName(LOG_NAME);
static StringBuffer logPath("\\");
static WCHAR mutexName[128];

HANDLE winLog::wait() {
  
    HANDLE hMutex = CreateMutex(NULL, TRUE, mutexName);
    switch(GetLastError()) {
        case ERROR_SUCCESS:
            break;
        case ERROR_ALREADY_EXISTS:
            CloseHandle(hMutex);
            hMutex = NULL;
            break;
        default:
            {}
    }    
    return hMutex;        
}

void winLog::signal(HANDLE hMutex) {
    CloseHandle(hMutex);
}

winLog::winLog(bool resetLog, const char* path, const char* name) {

    setLogPath(path);
    setLogName(name);
    logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());

    //
    // Test to ensure the log file is writable (only if path is set)
    //
    if (path) {
        logFile = fileOpen(logDir.c_str(), "a+");
        if (logFile == NULL) {
            WCHAR tmp[512];
            wsprintf(tmp, TEXT("Unable to write log file: \"%S\".\nPlease check your user's permissions."), logDir.c_str());
            MessageBox(NULL, tmp, TEXT("Funambol"), MB_SETFOREGROUND | MB_OK);
        }
        else {
            fclose(logFile);

            if (resetLog) {
                reset(FUNAMBOL_HEADER);
            }
        }
    }
}


winLog::~winLog() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}


void winLog::setLogPath(const char* configLogPath) {

    if (configLogPath != NULL) {
        logPath = configLogPath;
    } else {
        logPath = ".";
    }
    if (logName.length() > 0){
        logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());
    }
}

void winLog::setLogName(const char* configLogName) {

    if (configLogName != NULL) {
        logName = configLogName;
        wsprintf(mutexName, TEXT("%S"), logName.c_str());
    }
    else {
        logName = LOG_NAME;
    }
    if (logPath){
        logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());
    }
}

/*
* return a the time to write into log file. If complete is true, it return
* the date too, else only hours, minutes, seconds and milliseconds
*/
static StringBuffer createCurrentTime(bool complete) {

    SYSTEMTIME sys_time;
    TIME_ZONE_INFORMATION timezone;
    GetLocalTime(&sys_time);
    GetTimeZoneInformation(&timezone);

    StringBuffer ret;

    // calculate offset from UTC/GMT in hours:min, positive value
    // means east of Greenwich (e.g. CET = GMT +1)
    char direction = timezone.Bias <= 0 ? '+' : '-';
    int hours      = abs(timezone.Bias / 60) ;
    int minutes    = abs(timezone.Bias % 60);

    if (complete) {
        char fmtComplete[] = "%04d-%02d-%02d %02d:%02d:%02d GMT %c%d:%02d";
        ret.sprintf(fmtComplete, sys_time.wYear, sys_time.wMonth, sys_time.wDay,
                    sys_time.wHour, sys_time.wMinute, sys_time.wSecond,
                    direction, hours, minutes);
    } 
    else {
        char fmt[] = "%02d:%02d:%02d GMT %c%d:%02d";
        ret.sprintf(fmt, sys_time.wHour, sys_time.wMinute, sys_time.wSecond,
                    direction, hours, minutes);
    }
    return ret;
}




void winLog::error(const char*  msg, ...) {
   HANDLE h = wait();
   if (!h) return;
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);
    va_end(argList);

   signal(h);
}

void winLog::info(const char*  msg, ...) {
    HANDLE h = wait();
    if (!h) return;
    if (isLoggable(LOG_LEVEL_INFO)) {
        va_list argList;
        va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);
        va_end(argList);
    }
    signal(h);
}

void winLog::debug(const char*  msg, ...) {
    HANDLE h = wait();
    if (!h) return;
    if (isLoggable(LOG_LEVEL_DEBUG)) {
        va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        va_end(argList);
    }
    signal(h);
}

void winLog::developer(const char*  msg, ...) {
    HANDLE h = wait();
    if (!h) return;
    if (isLoggable(LOG_LEVEL_INFO)) {
        va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        va_end(argList);
    }
    signal(h);
}


void winLog::printMessage(const char* level, const char* msg, va_list argList) {

    StringBuffer currentTime = createCurrentTime(false);
    logFile = fileOpen(logDir.c_str(), "a+");
    if (logFile) {
        fprintf(logFile, "%s [%s] - ", currentTime.c_str(), level);
        vfprintf(logFile, msg, argList);
	    fprintf(logFile, "\n");
	    fclose(logFile);
    }
}

void winLog::reset(const char* title) {
    const char *t = (title) ? title : FUNAMBOL_HEADER;

    StringBuffer currentTime = createCurrentTime(true);
    logFile = fileOpen(logDir.c_str(), "w+");
    if (logFile) {
        fprintf(logFile, "%s - # %s\n\n", currentTime.c_str(), t);
        fclose(logFile);
    }
}


size_t winLog::getLogSize() {
    size_t ret = 0;

    logFile = fileOpen(logDir.c_str(), "r");
    if (logFile) {
        ret = fgetsize(logFile);
        fclose(logFile);
    }
    return ret;
}

Log *Log::logger;

Log &Log::instance() {
    if (!logger) {
        logger = new winLog();
    }
    return *logger;
}


bool Log::rotateLogFile(unsigned int maxSize, unsigned int maxCount) {
#ifndef _WIN32_WCE
    unsigned int KB = 1024;
    unsigned int MB = KB*KB;
    logFile = fileOpen(logDir.c_str(), "r");

    if (logFile) 
    {
        // check file size
        size_t size = fgetsize(logFile);
        if (size>=maxSize*MB)
        {
            if (! fclose(logFile))
            {
                bool stopflag = false;
                unsigned int lastID = 1;
                StringBuffer newname;
                StringBuffer oldname;
                // find the next available name
                while(!stopflag && lastID<maxCount)
                {
                    newname.sprintf("%s.%d", logDir, lastID);
                    FILE* tmpfile = fileOpen(newname.c_str(), "r");
                    if (tmpfile)
                    {
                        fclose(tmpfile);
                        lastID++;
                    }
                    else
                    {
                        stopflag = true;
                    }
                }

                remove(newname.c_str());

                // rename
                for(int t=lastID;t>1;t--)
                {
                    newname.sprintf("%s.%d", logDir, t);
                    oldname.sprintf("%s.%d", logDir, t-1);

                    rename(oldname.c_str(), newname.c_str());
                }
                newname.sprintf("%s.%d", logDir, 1);

                remove(newname.c_str());
                rename(logDir.c_str(), newname.c_str());

                logFile = fileOpen(logDir, "a+");
                if (logFile == NULL && strcmp(logName,LOG_NAME)!=0) 
                {
                    error("Unable to open new log file: \"%s\".\nPlease check your user's permissions.", logPath.c_str());
                }

                reset();
                return true;
            }
        }
    }
#else
    reset();
#endif
    return false;
}
