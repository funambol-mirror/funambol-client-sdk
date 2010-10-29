/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.util;

import java.util.Date;
import java.io.IOException;

import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import com.funambol.storage.AbstractRecordStore;
import com.funambol.storage.DataAccessException;

/**
 * Logger class for debugging porposes
 *
 */
public class RMSAppender implements Appender {
    //---------------------------------------------------------------- Constants
    //RMS Recordstore name:
    public static final String LOGDBNAME = "funambol.log";
    public static final int DEFAULTLOGFILESIZE = 5120;
    
    //--------------------------------------------------------------- Attributes
    /**
     * Default Log RMS Index Variables: when a RecordStoreFull Exception occurs
     * indicates the first record to be deleted before a new one could be added
     */
    
    //Default RMS RecordStore Name
    private String dbName = null;
    private AbstractRecordStore dbStore = null;
    private static int defaultLogFileSize;
    private static int defaultLogStoreSizeToBeFree;
    private int firstRecord;
    private int logFileSize = 0;
    private static int firstRecordtoBeDeleted;
    
    //------------------------------------------------------------- Constructors
    public RMSAppender(String logfilename) {
        this.dbName = logfilename;
        this.defaultLogFileSize = DEFAULTLOGFILESIZE;
        this.defaultLogStoreSizeToBeFree = DEFAULTLOGFILESIZE / 2;
        this.firstRecord = 1;
    }

    public RMSAppender(String logFileName, int size) {
        this(logFileName);
        this.defaultLogFileSize = size;
        this.defaultLogStoreSizeToBeFree = size / 2;
    }
    //----------------------------------------------------------- Public methods
    /**
     * Logs a string to standard output and to the device's event logger
     * @param msg the string to be logged
     **/
    public void writeLogMessage(String level, String msg)
    throws DataAccessException {
        
        
        //Put a timestamp on log message
        msg = MailDateFormatter.dateToUTC(new Date() )
                        +"\n[" + level + "] " + msg;
        
        boolean writeSuccess = true;
        //Open DS-Log RecordStore
        openLogFile();
        
        this.logFileSize = getLogFilesize();
        
        //Rotate Log records if Log RecordStore exceed default dimension;
        //If log message is too big for default Log File size throws
        //DataAccessexception
        if (this.logFileSize + msg.getBytes().length>defaultLogFileSize) {
            if (msg.getBytes().length>defaultLogFileSize) {
                throw new DataAccessException("Log Message too big.\n" +
                        " Total Size: " +
                        logFileSize + msg.getBytes().length +
                        ".\n " +
                        "Default size set to: " + defaultLogFileSize +
                        "\nChange default log file size to be written\n");
            } else {
                rotateLogStore(msg.getBytes().length);
            }
        }
        if (writeSuccess) {
            //Add new record to the RecordStore
            try {
                dbStore.addRecord(msg.getBytes(), 0, msg.getBytes().length);
            } catch (Exception ex) {
                ex.printStackTrace();
            } finally {
                closeLogFile();
            }
        } else {
            closeLogFile();
        }
    }
    
    /**
     * Opens DS-Log store and catch all possible exception
     */
    public void openLogFile(){
        try {
            dbStore = AbstractRecordStore.openRecordStore(dbName, true);
        } catch (Exception e) {
            System.out.println("Exception opening " + dbName);
            System.out.println(e);
            e.printStackTrace();
        }
    }
    
    /**
     * Closes DS-Log store and catch all possible exception
     */
    public void closeLogFile() {
        try {
            dbStore.closeRecordStore();
        } catch (Exception e) {
            System.out.println("Exception Closing " + dbName);
            System.out.println(e);
            e.printStackTrace();
        }
    }
    
    /**
     * Resets DS-Log store and catch all possible exception
     */
    public void deleteLogFile(){
        try {
            //Close the log first
            closeLogFile();
            //Delete Log RecordStore
            AbstractRecordStore.deleteRecordStore(dbName);
            System.out.println("Log File deleted");
        } catch (RecordStoreNotFoundException ex) {
            System.out.println("Log file not found.");
        } catch (RecordStoreException ex) {
            System.out.println("RecordStoreException: " + ex);
        }
    }
    
    /**
     * Mandatory method for all applications that means to use
     * DS-Log store method (non jsr-75 compliant applications)
     *
     */
    public void initLogFile() {
        try {
            // Open the record store without removing the old content
            AbstractRecordStore.openRecordStore(dbName, true);
        } catch (RecordStoreNotFoundException ex1) {
            //Create new Log RecordStore
            System.out.println("Log file not found. Creating new Log file");
            try {
                dbStore = AbstractRecordStore.openRecordStore(dbName, true);
                dbStore.closeRecordStore();
            } catch (RecordStoreException ex2) {
                ex2.printStackTrace();
            }
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        } finally {
            try {
                System.out.println("Creating new Log file");
                //Create new Log RecordStore
                dbStore = AbstractRecordStore.openRecordStore(dbName, true);
                dbStore.closeRecordStore();
            } catch (RecordStoreNotOpenException notOpenEx) {
                notOpenEx.printStackTrace();
            } catch (RecordStoreFullException fullEx) {
                fullEx.printStackTrace();
                System.out.println("Cannot create log file: " +
                        "Application reached RecordStore Maximum dimension");
            } catch (RecordStoreException rmsEx) {
                rmsEx.printStackTrace();
                System.err.println("Cannot create Log  file");
            }
        }
    }
    
    /**
     * Resizes DS-Log RecordStore
     * In case of Log RecordStore exceed default first indexed records are
     * deleted:
     * The size to be free will be decided by user setting the private variable
     * defaultLogStoreSizeToBefree; this class provides the default size:
     * <code>DEFAULTLOGFILESIZE</code>
     * @param msgSize number of byte of the next log message
     *
     */
    private void rotateLogStore(int msgSize) {
        System.out.println("Rotate log store");
        int actualFreeSize = 0;
        int sizeToBeFree = defaultLogStoreSizeToBeFree;
        if (msgSize >= defaultLogStoreSizeToBeFree) {
            sizeToBeFree = msgSize;
        }
        while (actualFreeSize < sizeToBeFree) {
            try {
                int nextRecord = dbStore.getNextRecordID();
                firstRecord = nextRecord - dbStore.getNumRecords();
                firstRecordtoBeDeleted = firstRecord;
                int nextRecordSize = dbStore.getRecordSize(firstRecord);
                dbStore.deleteRecord(firstRecord);
                actualFreeSize += nextRecordSize;
                //System.out.println("Deleted Record: " + firstRecord);
                //System.out.println("Actual free size: " + actualFreeSize);
                //System.out.println("Size to be free: " + sizeToBeFree);
            } catch (RecordStoreNotOpenException ex) {
                ex.printStackTrace();
            } catch (InvalidRecordIDException ex) {
                if (this.logFileSize - actualFreeSize == 0) {
                    break;
                } else {
                    ex.printStackTrace();
                    System.err.println("Record number: " 
                                               + firstRecord + " not exixtent");
                }
            } catch (RecordStoreException ex) {
                ex.printStackTrace();
            }
        }
    }

    /**
     * Perform additional actions needed when setting a new level.
     */
    public void setLogLevel(int newlevel) {
    }

    /**
     * Setter method for defaultLogFileSize
     * @param newDefaultLogFileSize is the new default log size in bytes
     */
    public static void setDefaultLogFileSize(int newDefaultLogFileSize) {
        defaultLogFileSize = newDefaultLogFileSize;
    }
    
    /**
     * Getter method for defaultLogFileSize
     * @return defaultLogFileSize default log size in bytes
     */
    public static int getDefaultLogFileSize() {
        return defaultLogFileSize;
    }
    
    /**
     * Setter method for defaultLogFileSize
     * @param newDefaultLogStoreSizeToBeFree is the new default log size to be free
     */
    public static void setDefaultLogStoreSizeToBeFree
            (int newDefaultLogStoreSizeToBeFree) {
        defaultLogStoreSizeToBeFree = newDefaultLogStoreSizeToBeFree;
    }
    
    /**
     * Getter method for defaultLogFileSizeToBeFree
     * @return defaultLogFileSize log size to be free in bytes
     */
    public static int getDefaultLogStoreSizeToBeFree() {
        return defaultLogStoreSizeToBeFree;
    }
    
    /**
     * Getter method for firstRecord
     * @return defaultLogFileSize log size to be free in bytes
     */
    public static int getFirstRecord() {
        return firstRecordtoBeDeleted;
    }
    
    private int getLogFilesize() {
        int storeSize = 0;
        RecordEnumeration re;
        try {
            re = dbStore.enumerateRecords(null, null, false);
            
            while (re.hasNextElement()) {
                try {
                    storeSize += dbStore.getRecordSize(re.nextRecordId());
                } catch (RecordStoreNotOpenException ex) {
                    ex.printStackTrace();
                } catch (InvalidRecordIDException ex) {
                    ex.printStackTrace();
                } catch (RecordStoreException ex) {
                    ex.printStackTrace();
                }
            }
            return storeSize;
        } catch (RecordStoreNotOpenException ex) {
            ex.printStackTrace();
        } finally {
            return storeSize;
        }
    }

    public LogContent getLogContent() throws IOException {
        StringBuffer log = new StringBuffer();
        LogViewer lv = new LogViewer();
        String[] logEntries= lv.getLogEntries(lv.RMSLOG);
        for (int i =0; i < logEntries.length; i++) {
            log.append(logEntries[i]);
            log.append("\n");
        }
        
        return new LogContent(LogContent.STRING_CONTENT, log.toString());
    }
}


