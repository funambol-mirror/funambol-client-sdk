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
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;
import j2meunit.framework.*;

public class RMS_LogDisabledTest extends FunBasicTest {

    private RecordStore dbStore;

    /** Creates a new instance of RMS_LogDisabledTest */
    public RMS_LogDisabledTest() {
        super(3, "RMS_LogDisabledTest");
    }

    public void setUp() {
        try {
            dbStore = RecordStore.openRecordStore(RMSAppender.LOGDBNAME, true);
        } catch (RecordStoreException ex) {
            System.out.println("setUp: ex=" + ex.getMessage());
            ex.printStackTrace();
        }
    }

    public void tearDown() {
        try {
            dbStore.closeRecordStore();
        } catch (RecordStoreNotOpenException ex) {
            System.out.println("RecordStoreNotOpenException tearDown ex closing");
        } catch (RecordStoreException ex) {
            System.out.println("Generic RecordStoreException tearDown ex closing");
        }
        try {
            RecordStore.deleteRecordStore(RMSAppender.LOGDBNAME);
        } catch (RecordStoreException ex) {
            System.out.println("tearDown: ex deleting =" + ex.getMessage());
        }
    }

    public void test(int i) throws Throwable {
        switch (i) {
            case 0:
                testInitLogDISABLED();
                break;
            case 1:
                testNoLoggingConstructor();
                break;
            case 2:
                testNoLoggingSetLevel();
                break;
            default:
                break;
        }
    }

    private void testInitLogDISABLED() {
        System.out.println("testInitLogDISABLED");
        Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME), Log.DISABLED);
        assertTrue(Log.getLogLevel() == Log.DISABLED);
        System.out.println("Log.getLogLevel(): " + Log.getLogLevel());
    }

    private void testNoLoggingConstructor() {
        System.out.println("=== NoLoggingConstructor ========================");
        try {
            dbStore.closeRecordStore();
            RecordStore.deleteRecordStore(RMSAppender.LOGDBNAME);
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        }
        Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME), Log.DISABLED);

        Log.error("Error");
        Log.info("Info");
        Log.debug("Debug");
        Log.trace("Trace");

        try {
            // this should throw an exception!

            RecordStore rs = RecordStore.openRecordStore(RMSAppender.LOGDBNAME, false);
            // if we arrive here we've failed
            System.out.println("Exception not trown, failing");
            fail();
        } catch (RecordStoreNotFoundException n) {
            // here we're happy! :D
            System.out.println("Test succedeed");
            assertTrue(true);

            /*try {
                // we want to recreate the store to make the teardown method happy
                dbStore = RecordStore.openRecordStore(RMSAppender.LOGDBNAME, true);
            } catch (RecordStoreException ex) {
                ex.printStackTrace();
            }*/


        } catch (RecordStoreException ex) {
            System.out.println("wrong exception ");
            ex.printStackTrace();
            fail();
        }
        System.out.println("===========================================[ OK ]");


    }

    private void testNoLoggingSetLevel() {
        System.out.println("testNoLoggingSetLevel");
        
        try {
            dbStore.closeRecordStore();
        } catch (RecordStoreNotOpenException ex) {
            ex.printStackTrace();
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
        }
        
        Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME));
        System.out.println("log inited");
        Log.setLogLevel(Log.DISABLED);
        Log.error("Error");
        Log.info("Info");
        Log.debug("Debug");
        Log.trace("Trace");
        LogViewer lv = new LogViewer();
        String[] actualLog = lv.getLogEntries(lv.RMSLOG);
        ;
        assertTrue(actualLog[0].equals("No Log entries found."));
        System.out.println("===========================================[ OK ]");
    }

    private void testLoggingConstructor() {
        System.out.println("testLoggingConstructor");
        Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME), Log.DEBUG);
        Log.error("Error");
        Log.info("Info");
        Log.debug("Debug");
        Log.trace("Trace");
        LogViewer lv = new LogViewer();
        String[] actualLog = lv.getLogEntries(lv.RMSLOG);
        String[] expectedLog = new String[]{"Error", "Info", "Debug"};
        assertEquals(expectedLog, actualLog);
    }

    private void testLoggingSetLevel() {
        System.out.println("testLoggingSetLevel");
        // Log.initLog(new RMSAppender(RMSAppender.LOGDBNAME));
        Log.setLogLevel(Log.DEBUG);
        Log.error("Error");
        Log.info("Info");
        Log.debug("Debug");
        Log.trace("Trace");
        LogViewer lv = new LogViewer();
        String[] actualLog = lv.getLogEntries(lv.RMSLOG);
        String[] expectedLog = new String[]{"Error", "Info", "Debug"};
        assertEquals(expectedLog, actualLog);
    }
}
