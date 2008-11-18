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

package com.funambol.util;

import com.funambol.util.LogViewer;
import javax.microedition.rms.RecordStore;
import jmunit.framework.cldc10.TestCase;
import jmunit.framework.cldc10.AssertionFailedException;

/**
 * Test Class for RMS Log
 */
public class RMSAppenderTest extends TestCase {
    
    //---------------------------------------------------------------- Constants
    public static final int LOGMESSAGESNUMBER = 50;
    
    //------------------------------------------------------------- Constructors
    /**
     * New instance of LogTest class
     */
    public RMSAppenderTest() {
        super(5, "RMS Log Store Test");
        try {
            InitLogFile();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    //----------------------------------------------------------- Public Methods
    /**
     * Test Cases
     */
    public void test(int testNumber) throws Throwable {
        
        switch(testNumber) {
            case 0:
                testWrite(Log.ERROR);
                break;
            case 1:
                testWrite(Log.INFO);
                break;
            case 2:
                testWrite(Log.DEBUG);
                break;
            case 3:
                testWrite(Log.TRACE);
                break;
            case 4:
                testWriteWithRotation();
                break;
            default:
                break;
                
                }
    }
    
    /**
     * test a single write
     */
    private void testWrite(int logLevel) throws  Exception {
        System.out.println("=== Test Write at level " + logLevel +
                " =======================");
        Log.setLogLevel(logLevel);
        String msg = "LOG MESSAGE level: " + logLevel;
        switch (logLevel) {
            case Log.ERROR:
                Log.error(msg);
                break;
            case Log.INFO:
                Log.info(msg);
                break;
            case Log.DEBUG:
                Log.debug(msg);
                break;
            case Log.TRACE:
                Log.trace(msg);
                break;
            default:
                break;
        }
        
        
        // check if log message has been stored
        LogViewer lv = new LogViewer();
        String[] actualLog = lv.getLogEntries(lv.RMSLOG);
        
        //most recent entry is at actuallog[0]
        assertTrue(actualLog[0].endsWith(msg.toString()));
        System.out.println("===========================================[ OK ]");
        
    }
    
    /**
     * test write with rotation
     */
    private void testWriteWithRotation() throws Exception {
        System.out.println("=== Test Write With Rotation ==============[ OK ]");
        
        int size = RMSAppender.DEFAULTLOGFILESIZE;
        System.out.println("log file size = " + size);
        
        StringBuffer s = new StringBuffer();
        for (int i = 0; i < size/2; i++) {
            //filling the log
            s.append('a');
        }
        
        Log.error(s.toString());
        
        
        //this should cause a rotation
        
        Log.error(s.toString());
        Log.error("rotated");
        
        // the first entry should have been removed, we should have 2 entries
        LogViewer lv = new LogViewer();
        String[] actualLog = lv.getLogEntries(lv.RMSLOG);
        boolean ret = true;
        
        ret &= (actualLog.length == 2);
        
        // check we've written the last entry
        ret &= (actualLog[0].endsWith("rotated"));
        
        System.out.println("actualLog[0] = " + actualLog[0]);
        System.out.println("actual length= " + actualLog.length);
        
        assertTrue(ret);
        System.out.println("===========================================[ OK ]");
        
        
    }
    
    
    /**
     * Create RMS Log RecordStore
     */
    private void InitLogFile() throws Exception {
        Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME));
    }
    
 
}

