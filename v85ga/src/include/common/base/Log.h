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

#ifndef INCL_LOG
    #define INCL_LOG
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/util/StringBuffer.h"

    /** prefix for error messages */
    #define LOG_ERROR "ERROR"
    /** prefix for informational messages */
    #define LOG_INFO  "INFO"
    /** prefix for debug or developer messages */
    #define LOG_DEBUG "DEBUG"

    /** default is to create this file in the current directory */
    #define LOG_NAME "synclog.txt"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

    /**
     * valid parameters for setLevel()
     */
    typedef enum {
        /**
         * log level not configured: if used in setLevel(), then only
         * error messages will be printed
         */
        LOG_LEVEL_NONE  = 0,
        /**
         * errors and info messages for users and developers
         * will be printed: use this to keep the log consise and
         * small
         */
        LOG_LEVEL_INFO  = 1,
        /**
         * all messages will be printed: the log can become very large!
         */
        LOG_LEVEL_DEBUG = 2,
    } LogLevel;

    /**
     *
     */
    class Log {

    private:

        /**
         * Which log level is set?
         */
        LogLevel logLevel;

        /**
         * the singleton implementing logging
         */
        static Log *logger;

    public:

        Log() : logLevel(LOG_LEVEL_INFO) {}
        virtual ~Log() {}

        /**
         * Grants access to the singleton which implements logging.
         * The implementation of this function and thus the Log
         * class itself is platform specific: if no Log instance
         * has been set yet, then this call has to create one.
         */
        static Log &instance();

        /**
         * Overrides the default Log implementation. The Log class
         * itself will never delete the active logger.
         *
         * @param logger    will be used for all future logging activities;
         *                  NULL is allowed and implies that the default
         *                  Log implementation will be created if needed
         */
        static void setLogger(Log *logger) { Log::logger = logger; }

        /** clients can use #ifdef to detect this new feature */
# define LOG_HAVE_SET_LOGGER 1

        /**
         * Sets the directory where the log file will be created,
         * which is done in reset() or any of the logging functions.
         */
        virtual void setLogPath(const char*  configLogPath) = 0;

        /**
         * Sets the file name of the log file without creating it;
         * that is done in reset() or any of the logging functions.
         */
        virtual void setLogName(const char*  configLogName) = 0;

        /**
         * creates the log file under the selected name and path,
         * optionally logging the given title
         */
        virtual void reset(const char*  title = NULL) = 0;

        virtual void setLevel(LogLevel level) { logLevel = level; }
        virtual LogLevel getLevel() { return logLevel; }
        virtual bool isLoggable(LogLevel level) { return level <= logLevel; }

        /**
         * error(), info(), developer(), debug() all print one message,
         * using printf() style formatting. Whether the message is really
         * written into the log file depends on the current log level
         * (see LogLevel above).
         *
         * Which of these calls is the right one for a certain message
         * is a somewhat subjective choice. Here is a definition how they
         * are supposed to be used:
         * - error: severe problem which the user and developer have to
         *          know about
         * - info: information about a sync session which the user
         *         will want to read during/after each sync session
         * - developer: information about a sync session that is not
         *              interesting for a user (for example, because it
         *              is constant and already known) but which should
         *              be in each log because developers need to know
         *              it. Messages logged with this calls will be included
         *              at LOG_LEVEL_INFO, therefore messages should be small and
         *              not recur so that the log file size remains small.
         * - debug: most detailed logging, messages may be arbitrarily large
         *
         * Here is a decision tree which helps to pick the right level:
         * - an error: => error()
         * - it changes during each sync or marks important steps
         *   in the sync: info()
         * - small, non-recurring message which is important for developers
         *   who read a log produced at LOG_LEVEL_INFO: developer()
         * - everything else: debug()
         */
        virtual void error(const char*  msg, ...) = 0;
        virtual void info(const char*  msg, ...) = 0;
        virtual void developer(const char* msg, ...) = 0;
        virtual void debug(const char*  msg, ...) = 0;

        /** clients can use #ifdef to detect this new feature */
# define LOG_HAVE_DEVELOPER 1

        /**
         * Adds a fixed string to each following line of output. NULL
         * removes the prefix again. Some logging implementations
         * might ignore the prefix. The prefix is copied by the
         * implementation, i.e. the caller can free it after this
         * call.
         */
        virtual void setPrefix(const char *prefix) {
            // Avoid compiler warning
            prefix = NULL;
        }

        /**
         * Returns the log file size [bytes].
         */
        virtual size_t getLogSize() = 0;
    };

# define LOG Log::instance()


END_NAMESPACE

/** @endcond */
#endif
