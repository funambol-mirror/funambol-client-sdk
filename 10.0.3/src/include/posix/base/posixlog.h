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

#ifndef INCL_POSIX_LOG
# define INCL_POSIX_LOG
/** @cond DEV */

#include <base/fscapi.h>
#include <base/Log.h>
#include "base/util/StringBuffer.h"

#include <stdio.h>
#include <time.h>
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * extended API, can only be used if it is certain that
 * Log::instance() returns a POSIXLog
 */
class POSIXLog : public Log {
 public:

    POSIXLog();
    ~POSIXLog();

    /**
     * Opens the specified file for logging of messages.
     *
     * By default the LOG instance of the Log class will
     * create the file specified via its set methods
     * as soon as the first message needs to be printed or
     * when explicitly asking for a reset.
     *
     * By calling this function instead one gets more detailed
     * control over logging and avoids the (currently) insecurely
     * implemented handling of file name strings in the Log class.
     *
     * @param path            directory where file is to be created, can be NULL
     * @param name            file name relative to path or "-" when asking for
     *                        logging to stdout
     * @param redirectStderr  if true, then file descriptor 2 (stderr)
     *                        will also be redirected into the log file;
     *                        the original stderr is preserved and will be
     *                        restored when turning this redirection off
     */
    virtual void setLogFile(const char *path, const char* name, bool redirectStderr = false);

    /**
     * returns active log file or NULL if none set (e.g. if logging to stdout directly)
     */
    virtual FILE *getLogFile() { return logFile; }

    /**
     * if a client developer wants to ignore the prefix, he can
     * derive his own Log implementation from POSIXLog, override this
     * call and then install his implementation via Log::setLogger()
     */
    virtual void setPrefix(const char *prefix) { this->prefix = prefix ? prefix : ""; }
    virtual const StringBuffer &getPrefix() const { return prefix; }

    virtual void setLogPath(const char*  configLogPath);
    virtual void setLogName(const char*  configLogName);
    virtual void error(const char*  msg, ...);
    virtual void info(const char*  msg, ...);
    virtual void developer(const char*  msg, ...);
    virtual void debug(const char*  msg, ...);
    virtual void reset(const char* title = NULL);
    virtual size_t getLogSize();

 protected:
    /**
     * Prints a single line to the current log file.
     * Can be overridden by derived class to also print
     * in a different way.
     *
     * @param firstLine     true if this is the first line of a new message
     * @param time          unformatted time stamp for line
     * @param fullTime      a time string including date and GMT offset
     * @param shortTime     a time string including just the local time of day in
     *                      the preferred time format according to the current locale
     * @param utcTime       a time string including just the UTC time of day in "hh:mm:ss UTC" format
     * @param level         the severity of the report
     * @param levelPrefix   a string representing the severity (may differ from level, e.g. for Log::developer())
     * @param line          the actual message string
     */
    virtual void printLine(bool firstLine,
                           time_t time,
                           const char *fullTime,
                           const char *shortTime,
                           const char *utcTime,
                           LogLevel level,
                           const char *levelPrefix,
                           const char *line);

 private:
    FILE* logFile;
    bool logFileStdout;
    StringBuffer logName;
    StringBuffer logPath;
    bool logRedirectStderr;

    /** a copy of stderr before it was redirected */
    int fderr;

    /**
     * additional prefix for each line
     */
    StringBuffer prefix;

    void printMessage(LogLevel level, const char* levelPrefix, const char* msg, va_list argList);
};

#define POSIX_LOG ((POSIXLog &)Log::instance())


END_NAMESPACE

/** @endcond */
#endif
