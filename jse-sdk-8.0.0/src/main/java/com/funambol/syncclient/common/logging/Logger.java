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
package com.funambol.syncclient.common.logging;

import java.util.Calendar;
import java.util.Date;

import java.io.File;
import java.io.IOException;

import com.funambol.syncclient.common.FileSystemTools;

/**
 * This class implement log methods
 *
 * @version $Id: Logger.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 **/
public class Logger {

    //-------------------------------------------------------------- Constants

    public static final int       NONE                   = 0         ;
    public static final int       ERROR                  = 1         ;
    public static final int       INFO                   = 5         ;
    public static final int       DEBUG                  = 10        ;

    public static final String    DEFAULT_LOG_FILE_NAME  = "sync.log";

    public static final String    PROP_NONE              = "none"    ;
    public static final String    PROP_ERROR             = "error"   ;
    public static final String    PROP_INFO              = "info"    ;
    public static final String    PROP_DEBUG             = "debug"   ;

    private static final int      DEFAULT_LOGGER_LEVEL   = INFO      ;
    private static final boolean  DEFAULT_CONSOLE_ENABLE = true      ;

    private static final String   MSG_LOG_START = "# SyncClient API J2SE Log";

    private static boolean fileInitialized           = false;
    private static boolean consoleHandlerInitialized = false;
    private static boolean handlerInitialized        = false;

    //------------------------------------------------------------ Private data
    private static Handler  handler        = null;
    private static Handler  consoleHandler = new OutputHandler();

    private static int      level          = 0    ;
    private static boolean  consoleEnable  = false;
    private static File     logFile        = null ;

    //------------------------------------------------------------ Public methods

    /**
     * set handler
     *
     * @param h the client handler
     * (default com.funambol.syncclient.common.logging.OutputHandler)
     **/
    public static void setHandler(Handler h) {
        if (h != handler) {
            // force handler init
            handlerInitialized = false;
        }
        handler = h;
    }

    /**
     * return the client handler
     **/
    public static Handler getHandler() {
        return handler;
    }

    /**
     * set logger level
     *
     * @param l the logger level
     **/
    public static void setLevel(int l) {
        level = l;
    }

    /**
     * set logger level
     *
     * @param l the logger level
     **/
    public static void setLevel(String l) {

        if (PROP_NONE.equals(l)) {
            level = NONE;
        } else if (PROP_ERROR.equals(l)) {      
            level = ERROR;
        } else if (PROP_INFO.equals(l)) {
            level = INFO;
        } else if (PROP_DEBUG.equals(l)) {
            level = DEBUG;
        }
    }

    /**
     * set default logger level
     **/
    public static void setDefaultLevel() {
        level = DEFAULT_LOGGER_LEVEL;
    }

    /**
     * set log file
     *
     * @param fileName
     **/
    public static void setLogFile(String fileName) {
        logFile = new File(fileName);
    }

    /**
     * set default log file name
     **/
    public static void setDefaultLogFile() {
        logFile = new File(DEFAULT_LOG_FILE_NAME);
    }

    /**
     * set enable / disable log console
     *
     * @param enable
     **/
    public static void setEnableConsole(boolean enable) {
        consoleEnable = enable;
    }

    /**
     * set default enable console
     **/
    public static void setDefaultEnableConsole() {
        consoleEnable = DEFAULT_CONSOLE_ENABLE;
    }

    /**
     * check log level
     *
     * @param l the log level to check
     *
     * @return <p>true</p> if level is setting
     **/
    public static boolean isLoggable(int l) {
        if (level >= l) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Logs a string at info level
     *
     * @param text the string to be logged
     **/
    public static void info(String text) {
        if (!isLoggable(INFO)) {
            return;
        }
        composeMessage("INFO", text);
    }

    /**
     * Logs a string at debug level.
     *
     * @param text the string to be logged
     **/
    public static void debug(String text) {
        if (!isLoggable(DEBUG)) {
            return;
        }
        composeMessage("DEBUG", text);
    }

    /**
     * Logs a string only if level is info or debug: this is used for the
     * exception or error messages.
     *
     * @param text the string to be logged
     **/
    public static void error(String text) {
        if (!isLoggable(ERROR)) {
            return;
        }
        composeMessage("ERROR", text);
    }

    //---------------------------------------------------------- Private methods

    /**
     * Write and Print logger message
     *
     * @param level could be INFO, DEBUG or ERROR
     * @param text the text of the message
     */
    private static void composeMessage(String level, String text) {
        checkInit();

        try {

            text = getDateTime(true)
                 + " [" + level + "] - "
                 + text
                 + "\n"
                 ;

            FileSystemTools.writeTextFile(logFile, text, true);
            if (consoleEnable) {
                consoleHandler.printMessage(text);
            }

            if (handler != null) {
                handler.printMessage(text);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Initialize log file, consoleHandler and handler if necessary
     */
    private static void checkInit() {
        if (!fileInitialized) {
          initLogFile();
        }

        if (!consoleHandlerInitialized) {
            initConsoleHandler();
        }

        if (handler != null && !handlerInitialized) {
            initHandler();
        }
    }

    /**
     * Init log file
     **/
    private static synchronized void initLogFile() {

        if (fileInitialized) {
            return ;
        }

        try {

            String initMsg = null;

            initMsg = getDateTime(false) +
                      " - "              +
                      MSG_LOG_START      +
                      "\n"               ;

            FileSystemTools.writeTextFile(logFile, initMsg, false);

            fileInitialized = true;

        } catch (Exception e) {
            System.err.println("Unable to initialize the logger [" + e.getMessage() + "]");
        }

    }

    /**
     * Init handler
     **/
    private static synchronized void initHandler() {

        if (handlerInitialized) {
            return ;
        }

        try {

            String initMsg = getDateTime(false) + " - " + MSG_LOG_START + "\n";
            handler.printMessage(initMsg);
            handlerInitialized = true;

        } catch (Exception e) {
            System.err.println("Unable to initialize the handler [" + e.getMessage() + "]");
        }

    }

    /**
     * Init console handler
     **/
    private static synchronized void initConsoleHandler() {

        if (!consoleEnable || consoleHandlerInitialized) {
            return ;
        }

        try {

            String initMsg = getDateTime(false) + " - " + MSG_LOG_START + "\n";
            consoleHandler.printMessage(initMsg);
            consoleHandlerInitialized = true;

        } catch (Exception e) {
            System.err.println("Unable to initialize the handler [" + e.getMessage() + "]");
        }

    }

    /**
     *
     * Return now in "[YYYY-MM-DD hh:mm:ss:SSS]"
     * format
     *
     **/
    private static String getDateTime(boolean onlyTime) {

        Calendar date        = null ;

        String   year        = null ;
        String   month       = null ;
        String   day         = null ;
        String   hour        = null ;
        String   minute      = null ;
        String   second      = null ;
        String   millisecond = null ;

        String   time        = null ;
        String   dateTime    = null ;

        date = Calendar.getInstance();

        date.setTime(new Date(System.currentTimeMillis()));

        year        = String.valueOf( date.get(Calendar.YEAR         ) ) ;
        month       = String.valueOf( date.get(Calendar.MONTH        ) ) ;
        day         = String.valueOf( date.get(Calendar.DAY_OF_MONTH ) ) ;
        hour        = String.valueOf( date.get(Calendar.HOUR_OF_DAY  ) ) ;
        minute      = String.valueOf( date.get(Calendar.MINUTE       ) ) ;
        second      = String.valueOf( date.get(Calendar.SECOND       ) ) ;
        millisecond = String.valueOf( date.get(Calendar.MILLISECOND  ) ) ;

        if (month.length()       == 1 )        {
            month       = "0"  + month;
        }
        if (day.length()         == 1 )        {
            day         = "0"  + day;
        }
        if (hour.length()        == 1 )        {
            hour        = "0"  + hour;
        }
        if (minute.length()      == 1 )        {
            minute      = "0"  + minute;
        }
        if (second.length()      == 1 )        {
            second      = "0"  + second;
        }
        if (millisecond.length() == 1 )        {
            millisecond = "00" + millisecond;
        }
        if (millisecond.length() == 2 )        {
            millisecond = "0"  + millisecond;
        }

        time =  hour         +
                ":"          +
                minute       +
                ":"          +
                second       +
                ":"          +
                millisecond  ;

        if (onlyTime) {
            return time;
        }

        dateTime =  year         +
                    "-"          +
                    month        +
                    "-"          +
                    day          +
                    " "          +
                    time         ;

        return dateTime;

    }

}
