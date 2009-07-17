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

import j2meunit.framework.*;
import java.io.IOException;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;

/**
 * Test the ConnectionManager class methods.
 */
public class ConnectionManagerTest extends FunBasicTest {
    private static final String FUNAMBOL_SERVER_URL = "http://my.funambol.com/sync";

    /** 
     * Creates a new instance of ConnectionManagerTest
     */
    public ConnectionManagerTest() {
        super(5, "ConnectionManagerTest");
        Log.setLogLevel(Log.DEBUG);
    }
    
    /**
     * Set up the test environment
     * @throws java.lang.Exception
     */
    protected void setUp() throws Exception {
        //Init the default test object
    }

    /**
     * Tear down the test environment
     * @throws java.lang.Exception
     */
    protected void tearDown() throws Exception {
        ConnectionManager.getInstance().setConnectionListener(new BasicConnectionListener());
    }
    
    public void test(int i) throws Throwable {
        switch(i) {
            case 0:
                Log.debug("TestGetInstance");
                testGetInstance();
                break;
            case 1:
                //Log.debug("TestOpen");
                //testOpen();
                break;
            case 2:
                //Log.debug("TestOpenWithArguments");
                //testOpenWithArguments();
                break;
            case 3:
                Log.debug("TestSetConnectionListener");
                testSetConnectionListener();
                break;
            case 4:
                Log.debug("TestGetConnectionListener");
                testGetConnectionListener();
                break;
            default:
                break;
        }
    }

    /**
     * test the accessor method setConnectionListener
     */
    private void testSetConnectionListener() {
        ConnectionManager.getInstance().setConnectionListener(new TestConnectionListener());
        assertTrue(ConnectionManager.getInstance().getConnectionListener() instanceof TestConnectionListener);
    }
    

    /**
     * test the accessor method getConnectionListener
     */
    private void testGetConnectionListener() {
        assertTrue(ConnectionManager.getInstance().getConnectionListener() instanceof BasicConnectionListener);
    }

    /**
     * Test the Singleton Pattern implementation
     */
    private void testGetInstance() {
        ConnectionManager expectedSingleton = ConnectionManager.getInstance();
        ConnectionManager returnedObject = ConnectionManager.getInstance();
        assertEquals(expectedSingleton, returnedObject);
    }

    /**
     * test the open method with only the URL argument
     */
    private void testOpen() throws IOException {
        HttpConnection hc = (HttpConnection) ConnectionManager.getInstance().open(FUNAMBOL_SERVER_URL);
        assertTrue(hc!=null);
        hc.close();
    }

    /**
     * test the open method with the URL argument, the stream mode and the 
     * timeout exception set to false
     */
    private void testOpenWithArguments() throws IOException {
        HttpConnection hc = (HttpConnection) ConnectionManager.getInstance().open(FUNAMBOL_SERVER_URL, Connector.READ_WRITE, false);
        assertTrue(hc!=null);
        hc.close();
    }

    private class TestConnectionListener implements ConnectionListener {

        public boolean isConnectionConfigurationAllowed(String configDescription) {
            return true;
        }

        public void connectionOpened() {
        }

        public void requestWritten() {
        }

        public void responseReceived() {
        }

        public void connectionClosed() {
        }

        public void connectionConfigurationChanged() {
        }
    } 
}
