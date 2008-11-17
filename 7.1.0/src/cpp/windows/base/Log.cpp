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
#define FUNAMBOL_HEADER "Funambol Windows Mobile Plug-in Log"
#elif WIN32
#define FUNAMBOL_HEADER "Funambol Win32 Plug-in Log"
#endif


static FILE* logFile = NULL;

static WString      wlogDir(TEXT("\\"));
static StringBuffer logName(LOG_NAME);
static StringBuffer logPath("\\");


winLog::winLog(bool resetLog, const char* path, const char* name) {

    setLogPath(path);
    setLogName(name);

    // MUST use a wchar_t* logDir to manage correctly international chars.
    // If using char* logDir and UTF-8, fopen() is not working correctly and
    // will fail to open the log file. (with UTF-8... why?)
    // So we use _wfopen() and a wchar_t* logDir.
    StringBuffer logDir;
    logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());
    WCHAR* tmp = toWideChar(logDir.c_str());
    wlogDir = tmp;
    delete [] tmp;

    //
    // Test to ensure the log file is writable (only if path is set)
    //
    if (path) {
        logFile = _wfopen(wlogDir.c_str(), TEXT("a+"));
        if (logFile == NULL) {
            WCHAR tmp[512];
            wsprintf(tmp, TEXT("Unable to write log file: \"%s\".\nPlease check your user's permissions."), wlogDir.c_str());
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
        StringBuffer logDir;
        logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());
        WCHAR* tmp = toWideChar(logDir.c_str());
        wlogDir = tmp;
        delete [] tmp;
    }
}

void winLog::setLogName(const char* configLogName) {

    if (configLogName != NULL) {
        logName = configLogName;
    }
    else {
        logName = LOG_NAME;
    }
    if (logPath){
        StringBuffer logDir;
        logDir.sprintf("%s\\%s", logPath.c_str(), logName.c_str());
        WCHAR* tmp = toWideChar(logDir.c_str());
        wlogDir = tmp;
        delete [] tmp;
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
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);
    va_end(argList);
}

void winLog::info(const char*  msg, ...) {
    if (isLoggable(LOG_LEVEL_INFO)) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);
	    va_end(argList);
    }
}

void winLog::debug(const char*  msg, ...) {
    if (isLoggable(LOG_LEVEL_DEBUG)) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        va_end(argList);
    }
}

void winLog::developer(const char*  msg, ...) {
    if (isLoggable(LOG_LEVEL_INFO)) {
        va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        va_end(argList);
    }
}


void winLog::printMessage(const char* level, const char* msg, va_list argList) {

    StringBuffer currentTime = createCurrentTime(false);
    logFile = _wfopen(wlogDir.c_str(), TEXT("a+"));
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
    logFile = _wfopen(wlogDir.c_str(), TEXT("w+"));
    if (logFile) {
        fprintf(logFile, "%s - # %s\n\n", currentTime.c_str(), t);
        fclose(logFile);
    }
}


size_t winLog::getLogSize() {
    size_t ret = 0;

    logFile = _wfopen(wlogDir.c_str(), TEXT("r"));
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
