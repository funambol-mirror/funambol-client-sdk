/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include "base/Log.h"
#include "base/util/utils.h"

#ifdef _WIN32_WCE
#define FUNAMBOL_HEADER "Funambol Windows Mobile Plug-in Log"
#elif WIN32
#define FUNAMBOL_HEADER "Funambol Win32 Plug-in Log"
#endif

Log LOG = Log(false);

char logmsg[512];

static FILE* logFile = NULL;
static char logDir[512] = "\\";
static char logName[128] = LOG_NAME;
static char logPath[256] = "\\" ;


Log::Log(BOOL resetLog, const char* path, const char* name) {

    setLogPath(path);
    setLogName(name);
    sprintf(logDir, "%s\\%s", logPath, logName);

    //
    // Test to ensure the log file is writable (only if path is set)
    //
    if (path) {
        logFile = fopen(logDir, "a+");
        if (logFile == NULL) {
            char tmp[512];
            sprintf(tmp, "Unable to write log file: \"%s\".\nPlease check your user's permissions.", logDir);
            WCHAR* wtmp = toWideChar(tmp);
            MessageBox(NULL, wtmp, TEXT("Funambol"), MB_SETFOREGROUND | MB_OK);
            delete [] wtmp;
        }
        else {
            fclose(logFile);

            if (resetLog) {
                reset(FUNAMBOL_HEADER);
            }
        }
    }
}


Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}


void Log::setLogPath(const char* configLogPath) {

    if (configLogPath != NULL) {
        sprintf(logPath, "%s", configLogPath);
    } else {
        sprintf(logPath, ".");
    }
}

void Log::setLogName(const char* configLogName) {

    if (configLogName != NULL) {
        sprintf(logName, "%s", configLogName);
    }
    else {
        sprintf(logName, "%s", LOG_NAME);
    }
}

/*
* return a the time to write into log file. If complete is true, it return
* the date too, else only hours, minutes, seconds and milliseconds
*/
static char* createCurrentTime(BOOL complete) {

    SYSTEMTIME sys_time;
    TIME_ZONE_INFORMATION timezone;
    GetLocalTime(&sys_time);
    GetTimeZoneInformation(&timezone);

    char fmtComplete[] = "%04d-%02d-%02d %02d:%02d:%02d GMT %c%d:%02d";
    char fmt[]         = "%02d:%02d:%02d GMT %c%d:%02d";

    char* ret = new char [64];

    // calculate offset from UTC/GMT in hours:min, positive value
    // means east of Greenwich (e.g. CET = GMT +1)

    char direction = timezone.Bias <= 0 ? '+' : '-';
    int hours = abs(timezone.Bias / 60) ;
    int minutes = abs(timezone.Bias % 60);

    if (complete) {
        sprintf(ret, fmtComplete, sys_time.wYear, sys_time.wMonth, sys_time.wDay,
                 sys_time.wHour, sys_time.wMinute, sys_time.wSecond,
                direction, hours, minutes);
    } else {
        sprintf(ret, fmt, sys_time.wHour, sys_time.wMinute, sys_time.wSecond,
                direction, hours, minutes);
    }
    return ret;
}




void Log::error(const char*  msg, ...) {
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);
    va_end(argList);
}

void Log::info(const char*  msg, ...) {
    if (logLevel >= LOG_LEVEL_INFO) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);
	    va_end(argList);

    }
}

void Log::debug(const char*  msg, ...) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        va_end(argList);

    }
}

void Log::trace(const char*  msg) {
}

void Log::setLevel(LogLevel level) {
    logLevel = level;
}

LogLevel Log::getLevel() {
    return logLevel;
}

BOOL Log::isLoggable(LogLevel level) {
    return (level >= logLevel);
}

void Log::printMessage(const char* level, const char* msg, va_list argList) {

	char* currentTime = createCurrentTime(false);
    logFile = fopen(logDir, "a+");
    if (logFile) {
	    fprintf(logFile, "%s [%s] - ", currentTime, level);
        vfprintf(logFile, msg, argList);
	    fprintf(logFile, "\n");
	    fclose(logFile);
    }
    delete[] currentTime;
}

void Log::reset(const char* title) {
    const char *t = (title) ? title : FUNAMBOL_HEADER;

    char* currentTime = createCurrentTime(true);
    logFile = fopen(logDir, "w+");
    if (logFile) {
        fprintf(logFile, "%s - # %s\n\n", currentTime, t);
        fclose(logFile);
    }
    delete[] currentTime;
}


