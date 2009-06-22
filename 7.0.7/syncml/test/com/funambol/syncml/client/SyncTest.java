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

package com.funambol.syncml.client;

import com.funambol.util.SyncListener;
import jmunit.framework.cldc10.TestCase;

import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.spds.SyncManager;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncConfig;
import com.funambol.syncml.spds.SourceConfig;
import java.util.Random;

import com.funambol.storage.NamedObjectStore;
import com.funambol.util.Log;

public class SyncTest extends TestCase {
    
    private static final String STORE_NAME = "TESTCONFIG";
    private static final String SOURCE_NAME = "source.briefcase";
    private static final String URL  = "http://localhost:8080/funambol/ds";
    
    private SourceConfig sc;
    private SyncConfig conf;
    private SyncManager sm;
    private TestSyncSource testsrc;
    private TestSyncListener sl ;
    
    public SyncTest() {
        super(6, "SyncTest");
        
        Log.setLogLevel(Log.DEBUG);
    }
    
    public void setUp() {
        sc = new SourceConfig();
        conf = new SyncConfig();
        
        // Set defaults
        conf.syncUrl = URL;
        conf.userName = "test2";
        conf.password ="test2";  
        conf.deviceConfig.devID = generateDeviceId();
        // Set defaults
        sc.setType("text/plain");
        sc.setEncoding(SyncSource.ENCODING_NONE);
        
        sm = new SyncManager(conf);
        testsrc = new TestSyncSource(sc);
        sl = new TestSyncListener();
        testsrc.setListener(sl);
    }
    
    public void tearDown() {
        sc = null;
        conf = null;
        sm = null;
        testsrc = null;
        sl = null;
    }
    
    public void test(int testNumber) throws Throwable {
        
        
        switch(testNumber) {
            case 0:
                testSlow();
                break;
            case 1:
                testFast();
                break;
            case 2:
                testOneWayFromClient();
                break;
            case 3:
                testOneWayFromServer();
                break;
            case 4:
                testRefreshFromClient();
                break;
            case 5:
                testRefreshFromServer();
                break;
            default:
                break;
        }
        
    }
    
    public void testSlow() throws Exception {
        Log.info("------------ BEGIN TEST SLOW --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_SLOW);
        assertSync();
        Log.info("------------- END TEST SLOW ---------------");
    }
    
    public void testFast() throws Exception {
        Log.info("------------ BEGIN TEST FAST --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_FAST);
        assertSync();
        Log.info("------------- END TEST FAST --------------");
    }
    
    public void testOneWayFromClient()
    throws Exception {
        Log.info("------------ BEGIN TEST ONE WAY FROM CLIENT --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_ONE_WAY_FROM_CLIENT);
        assertSync();
        // Test if the # of status received from server equals the # of sent items
        Log.info("############### Server Item Status counter: " + testsrc.counter);
        assertEquals(testsrc.ITEMS_NUMBER, testsrc.counter);
        Log.info("------------- END TEST ONE WAY FROM CLIENT --------------");
    }
    
    public void testOneWayFromServer()
    throws Exception {
        Log.info("------------ BEGIN TEST ONE WAY FROM SERVER --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_ONE_WAY_FROM_SERVER);
        assertSync();
        
        Log.info("------------- END TEST ONE WAY FROM SERVER --------------");
    }
    
    public void testRefreshFromClient()
    throws Exception {
        Log.info("------------ BEGIN TEST REFRESH FROM CLIENT --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_REFRESH_FROM_CLIENT);
        assertSync();
        Log.info("------------- END TEST REFRESH FROM CLIENT --------------");
    }
    
    public void testRefreshFromServer()
    throws Exception {
        Log.info("------------ BEGIN TEST REFRESH FROM SERVER --------------");
        sm.sync(testsrc, SyncML.ALERT_CODE_REFRESH_FROM_SERVER);
        assertSync();
        Log.info("------------- END TEST REFRESH FROM SERVER --------------");
    }
    
    private void assertSync() throws Exception {
        while (sl.RUN) {
            Thread.sleep(100);
        }
        assertEquals(sl.result, testsrc.STATUS_SUCCESS);
        
        
    }

    /**
     * Generates a new device id.
     */
    public String generateDeviceId() {
        Random r = new Random();
        StringBuffer s = new StringBuffer("fsc-j2me-api-test-");

        s.append(Long.toString(System.currentTimeMillis(),16));
        s.append(Integer.toHexString(r.nextInt()));

        String deviceId = s.toString();

        Log.info("New device id: " + deviceId);
        return deviceId;
    }
    
}
