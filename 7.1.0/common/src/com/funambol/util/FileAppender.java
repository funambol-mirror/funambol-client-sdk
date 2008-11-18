/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.util.Date;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;
import java.util.Enumeration;

/**
 * This appender logs messages to a file using JSR75 (FileConnection)
 * The appender tracks the file size and if it exceeds a given maximum size
 * (customizable by clients) then the current log file is renamed appending a
 * .old to the log name and a new one is created. Therefore the maximum size
 * on this is about 2 times the maxFileSize (this is not accurate as there is
 * no limit on the size of the single message printed).
 */
public class FileAppender implements Appender {

    private String fileUrl   = "file:///root1/synclog.txt";
    private String fileName  = "synclog.txt";
    private String oldSuffix = ".sav.txt";
    private OutputStream os;
    private FileConnection fc;
    private long maxFileSize = 512 * 1024;

    /**
     * Default constructor
     */
    public FileAppender(String path, String fileName) {
        if (path != null && fileName != null) {
            if (path.endsWith("/")) {
                this.fileUrl = path + fileName;
            } else {
                this.fileUrl = path + "/" + fileName;
            }
            this.fileName = fileName;
        }
        os = null;
    }

    /**
     * Sets the maximum file size. Once this is size is reached, the current log
     * file is renamed and a new one is created. This way we have at most 2 log
     * files whose size is (roughly) bound to maxFileSize.
     * The minimum file size is 1024 as smaller size does not really make sense.
     * If a client needs smaller files it should probably the usage of other
     * Appenders or modify the behavior of this one by deriving it.
     */
    public void setMaxFileSize(long maxFileSize) {
        if (maxFileSize > 1024) {
            this.maxFileSize = maxFileSize;
        }
    }

    //----------------------------------------------------------- Public Methods
    /**
     * FileAppender writes one message to the output file
     */
    public void writeLogMessage(String level, String msg) {
        String levelMsg = " [" + level + "] ";
        try {
            if (os != null) {
                StringBuffer logMsg = new StringBuffer(MailDateFormatter.dateToUTC(new Date()));
                logMsg.append(levelMsg);
                logMsg.append(msg);
                logMsg.append("\r\n");
                synchronized (os) {
                    os.write(logMsg.toString().getBytes());
                    os.flush();

                    // If the file grows beyond the limit, we rename it and create a new
                    // one
                    if (fc.fileSize() > maxFileSize) {
                        try {
                            FileConnection oldFile;
                            String oldFileName = fileUrl + oldSuffix;
                            oldFile = (FileConnection)Connector.open(oldFileName, Connector.READ_WRITE);
                            if (oldFile.exists()) {
                                oldFile.delete();
                            }
                            fc.rename(fileName + oldSuffix);
                            fc.close();
                            // Reopen the file
                            initLogFile();
                        } catch (Exception ioe) {
                            System.out.println("Exception while renaming " + ioe);
                        }
                    }
                }
            }
        } catch (Exception e) {
            System.out.println("Exception while logging. " + e);
            e.printStackTrace();
        }
    }

    /**
     * Init the logger
     */
    public void initLogFile() {
       
        try {
            fc = (FileConnection) Connector.open(fileUrl, Connector.READ_WRITE);
            if (!fc.exists()) {
                fc.create();
                os = fc.openOutputStream();
            } else {
                // Open in append mode
                os = fc.openOutputStream(fc.fileSize());
            }
        } catch (Exception e) {
            System.out.println("Cannot open or create file at: " + fileUrl);
            e.printStackTrace();
        }
    }

    /**
     * FileAppender doesn't implement this method
     */
    public void openLogFile() {
    }

    /**
     * Close connection and streams
     */
    public void closeLogFile() {

        try {
            if (os != null) {
                os.close();
            }
            if (fc != null) {
                fc.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Delete the log file
     */
    public void deleteLogFile() {
        try {
            fc = (FileConnection) Connector.open(fileUrl, Connector.READ_WRITE);
            if (fc.exists()) {
                fc.delete();
            }
        } catch (Exception e) {
            System.out.println("Cannot open or create file at: " + fileUrl);
            e.printStackTrace();
        }

    }
}
