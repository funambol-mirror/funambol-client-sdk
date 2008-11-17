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

#include <unistd.h>

Log LOG = Log(false);
char logmsg[512];

static FILE* logFile = NULL;
static BOOL logFileStdout = FALSE;

static char logName[1024] = LOG_NAME;
static char logPath[1024] = "/tmp" ;
static BOOL logRedirectStderr = FALSE;

// a copy of stderr before it was redirected
static int fderr = -1;


void setLogFile(const char *path, const char* name, BOOL redirectStderr) {
    if (logName != name) {
        strncpy(logName, name ? name : "", sizeof(logName));
        logName[sizeof(logName) - 1] = 0;
    }
    if (logPath != path) {
        strncpy(logPath, path ? path : "", sizeof(logPath));
        logPath[sizeof(logPath) - 1] = 0;
    }
    logRedirectStderr = redirectStderr;

    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
    logFileStdout = FALSE;

    if (!strcmp(name, "-")) {
        // write to stdout
        logFileStdout = TRUE;
    } else if (path) {
        char *filename = new char[strlen(path) + strlen(name) + 3];

        sprintf(filename, "%s/%s", path, name);
        logFile = fopen(filename, "a+" );
        delete [] filename;
    } else {
        logFile = fopen(name, "a+" );
    }

    if (redirectStderr && logFile) {
        if (fderr == -1) {
            // remember original stderr
            fderr = dup(2);
        }
        // overwrite stderr with log file fd,
        // closing the current stderr if necessary
        dup2(fileno(logFile), 2);
    } else {
        if (fderr != -1) {
            // restore original stderr
            dup2(fderr, 2);
        }
    }
}

void setLogFile(const char* name, BOOL redirectStderr)
{
    setLogFile(0, name, redirectStderr);
}

/*
* return a the time to write into log file. If complete is true, it return
* the date too, else only hours, minutes, seconds and milliseconds
*/
static char* createCurrentTime(BOOL complete) {
    time_t t = time(NULL);
    struct tm *sys_time = localtime(&t);
    const size_t len = 64;
    char *ret = new char [len];

    if (complete) {
        strftime(ret, len, "%F %T GMT %z", sys_time);
    } else {
        strftime(ret, len, "%T GMT %z", sys_time);
    }
    return ret;
}


//---------------------------------------------------------------------- Constructors

Log::Log(BOOL resetLog, const char*  path, const char*  name) {

    setLogPath(path);
    setLogName(name);
    if (resetLog) {
        reset();
    }
}

Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}

//---------------------------------------------------------------------- Public methods

void Log::setLogPath(const char*  configLogPath) {
    if (configLogPath != NULL) {
        sprintf(logPath, "%s/", configLogPath);
    } else {
        sprintf(logPath, "%s", "./");
    }
}

void Log::setLogName(const char*  configLogName) {

    if (configLogName != NULL) {
        sprintf(logName, "%s", configLogName);
    }
    else {
        sprintf(logName, "%s", LOG_NAME);
    }
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

void Log::printMessage(const char*  level, const char*  msg, va_list argList) {

    char* currentTime = NULL;

    currentTime = createCurrentTime(false);
    if (!logFileStdout && !logFile) {
        reset();
    }
    FILE *out = logFile ? logFile : stdout ;
    fprintf(out, "%s [%s] - ", currentTime, level );
    vfprintf(out, msg, argList);
    fputs("\n", out);
    fflush(out);

    delete[] currentTime;
}


void Log::reset(const char*  title) {
    setLogFile(logPath, logName, logRedirectStderr);

    if (logFile) {
        ftruncate(fileno(logFile), 0);
    }
}

