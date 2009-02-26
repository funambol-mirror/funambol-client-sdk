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
package com.funambol.updater;

import java.util.Date;
import com.funambol.util.Log;
import com.funambol.util.TransportAgent;
import com.funambol.util.FunBasicTest;
import j2meunit.framework.*;

public class UpdaterTest extends FunBasicTest {
    private Updater updater;
    private TestTransportAgent ta;
    private TestUpdaterConfig  config;
    private final String version = "1.0.0";

    class TestTransportAgent implements TransportAgent {

        private String response = null;

        public String sendMessage(String text, String charset) {
            return sendMessage(text);
        }

        public String sendMessage(String text) {
            return response;
        }

        public void setRetryOnWrite(int retries) {
        }

        public void setResponse(String text) {
            response = text;
        }
    }

    class TestUpdaterConfig extends BasicUpdaterConfig {
        public void save() {
        }

        public void load() {
        }
    }

    class TestUpdaterListener implements UpdaterListener {

        private String newVersion = null;
        private boolean mandatory = false;
        private boolean optional  = false;

        public void mandatoryUpdateAvailable(String newVersion) {
            this.newVersion = newVersion;
            mandatory = true;
        }

        public void optionalUpdateAvailable(String newVersion) {
            this.newVersion = newVersion;
            optional = true;
        }

        public boolean isOptional() {
            return optional;
        }

        public boolean isMandatory() {
            return mandatory;
        }

        public String getNewVersion() {
            return newVersion;
        }

        public void reset() {
            newVersion = null;
            mandatory  = false;
            optional   = false;
        }
    }

    public UpdaterTest() {
        super(10, "UpdaterTest");
        Log.setLogLevel(Log.TRACE);
    }

    /*
    public UpdaterTest(String testName, TestMethod testMethod) {
        super(testName, testMethod);
    }
    */

    public void setUp() {
        // Set up a basic configuration
        ta      = new TestTransportAgent();
        config  = new TestUpdaterConfig();
        updater = new Updater(config, version, ta);
    }



    public void test(int i) throws Throwable {
        switch(i) {
            case 0:
                testFirstCheck();
                break;
            case 1:
                testNewOptional();
                break;
            case 2:
                testNewMandatory();
                break;
            case 3:
                testTwoUpdates();
                break;
            case 4:
                testTwoDistantUpdates();
                break;
            case 5:
                testTwoDistantUpdatesWithReminder();
                break;
            case 6:
                testSkip();
                break;
            case 7:
                testSkip2();
                break;
            case 8:
                testSkip3();
                break;
            case 9:
                testCheckNewerVersion();
                break;
        }
    }


    // Test a check where no info is available on the server
    // We do not expect any change in the config
    private void testFirstCheck() {
        assertTrue(!config.isMandatory());
        assertTrue(!config.isOptional());
        // Now prepare the response (simulate an empty one)
        String response = "";
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        // Do the update check
        updater.check();
        // We expect the updater to report a no new version of any kind
        assertTrue(!lis.isOptional());
        assertTrue(!lis.isMandatory());
        assertTrue(lis.getNewVersion() == null);
        assertTrue(config.getLastCheck() > 0);
    }

    private String createResponse(String version, String type, String delivery,
                                  String size, String url) {

        StringBuffer response = new StringBuffer();
        response.append("swup_begin\r\n");
        response.append("version=").append(version).append("\r\n");
        response.append("type=").append(type).append("\r\n");
        response.append("delivery_date=").append(delivery).append("\r\n");
        response.append("size=").append(size).append("\r\n");
        response.append("url=").append(url).append("\r\n");
        response.append("swup_end");
        return response.toString();
    }

    private void resetConfig() {
        config.setLastCheck(0);
        config.setLastReminder(0);
        config.setAvailableVersion(null);
        config.setType(null);
        config.setSkip(false);
        config.setUrl("http://fakeaddress.com");
        config.setCheckInterval((long)(3600 * 1000));
        config.setReminderInterval((long)(60 * 1000));
    }

    // 1) Perform a check on the server
    // 2) we expect an optional update reported to the client
    private void testNewOptional() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("optional");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Do the update check
        updater.check();
        // We expect the updater to report a new optional version
        assertTrue(lis.isOptional());
        assertTrue(!lis.isMandatory());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) Perform a check on the server
    // 2) we expect a mandatory update reported to the client
    private void testNewMandatory() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("mandatory");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) Perform a check on the server
    // 2) check again
    // 3) we expect no check on the server and no update reported to the client
    private void testTwoUpdates() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("mandatory");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        assertTrue(updater.isUpdateAvailable());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        // Do the update check again
        long lastCheck = config.getLastCheck();
        updater.check();
        assertTrue(config.getLastCheck() == lastCheck);
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server
    // 2) perform another check
    // 3) since the check interval is 1 millisec we expect a new check on the server
    //    but no version reported to the client (remind interval i 1 minute)
    private void testTwoDistantUpdates() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("mandatory");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        long checkInterval = config.getCheckInterval();
        // Set the interval check very low
        config.setCheckInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        assertTrue(updater.isUpdateAvailable());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        // Do the update check again
        lis = new TestUpdaterListener();
        updater.setListener(lis);
        long lastCheck = config.getLastCheck();
        updater.check();
        // A new server check must have been performed
        // but the available version did not change, so the
        // listener must not have been invoked
        assertTrue(config.getLastCheck() != lastCheck);
        assertTrue(!lis.isOptional());
        assertTrue(!lis.isMandatory());
        assertTrue(version.equals(config.getAvailableVersion()));
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server
    // 2) perform another check
    // 3) since the intervals are 1 millisec we expect a new check on the server
    //    a new version reported to the client
    private void testTwoDistantUpdatesWithReminder() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("mandatory");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Set the interval check very low
        config.setCheckInterval(1);
        config.setReminderInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        assertTrue(updater.isUpdateAvailable());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        // Do the update check again
        lis = new TestUpdaterListener();
        updater.setListener(lis);
        long lastCheck = config.getLastCheck();
        updater.check();
        // A new server check must have been performed
        // but the available version did not change, so the
        // listener must not have been invoked
        assertTrue(config.getLastCheck() != lastCheck);
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        assertTrue(version.equals(config.getAvailableVersion()));
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server
    // 2) set skip
    // 3) perform another check and the server must be queried but no
    //    version reported to the client
    private void testSkip() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("optional");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Set the interval check very low
        config.setCheckInterval(1);
        config.setReminderInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(lis.isOptional());
        assertTrue(!lis.isMandatory());
        assertTrue(updater.isUpdateAvailable());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        config.setSkip(true);
        // Do the update check again
        lis = new TestUpdaterListener();
        updater.setListener(lis);
        long lastCheck = config.getLastCheck();
        updater.check();
        // A new server check must have been performed
        // but the available version did not change, so the
        // listener must not have been invoked
        assertTrue(config.getLastCheck() != lastCheck);
        assertTrue(!lis.isOptional());
        assertTrue(!lis.isMandatory());
        assertTrue(version.equals(config.getAvailableVersion()));
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server
    // 2) set skip
    // 3) perform another check and the server must be queried. A new version is
    //    reported even if the client set the skip flag because yet another
    //    newer version is present
    private void testSkip2() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("optional");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Set the interval check very low
        config.setCheckInterval(1);
        config.setReminderInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(lis.isOptional());
        assertTrue(!lis.isMandatory());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        config.setSkip(true);
        // Do the update check again
        String version2  = new String("1.0.2");
        String type2     = new String("mandatory");
        String delivery2 = new String("20080731");
        String size2     = new String("467");
        String url2      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response2 = createResponse(version2, type2, delivery2, size2, url2);
        ta.setResponse(response2);
 
        lis = new TestUpdaterListener();
        updater.setListener(lis);
        long lastCheck = config.getLastCheck();
        updater.check();
        // A new server check must have been performed
        // but the available version did not change, so the
        // listener must not have been invoked
        assertTrue(config.getLastCheck() != lastCheck);
        assertTrue(!lis.isOptional());
        assertTrue(lis.isMandatory());
        assertTrue(version2.equals(config.getAvailableVersion()));
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server
    // 2) set skip
    // 3) perform another check and the server must be queried. A new version is
    //    reported even if the client set the skip flag because yet another
    //    newer version is present
    private void testSkip3() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("1.0.1");
        String type     = new String("optional");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Set the interval check very low
        config.setCheckInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(lis.isOptional());
        assertTrue(!lis.isMandatory());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(version.equals(lis.getNewVersion()));
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        assertTrue(updater.isUpdateAvailable());
        config.setSkip(true);
        // Do the update check again
        String version2  = new String("1.0.2");
        String type2     = new String("mandatory");
        String delivery2 = new String("20080731");
        String size2     = new String("467");
        String url2      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response2 = createResponse(version2, type2, delivery2, size2, url2);
        ta.setResponse(response2);
 
        lis = new TestUpdaterListener();
        updater.setListener(lis);
        long lastCheck = config.getLastCheck();
        updater.check();
        // A new server check must have been performed
        // but the available version did not change, so the
        // listener must not have been invoked
        assertTrue(config.getLastCheck() != lastCheck);
        assertTrue(!lis.isOptional());
        assertTrue(!lis.isMandatory());
        assertTrue(version2.equals(config.getAvailableVersion()));
        assertTrue(updater.isUpdateAvailable());
    }

    // 1) perform a check on the server with a version newer than what is
    //    available on the server
    // 2) we expect no notification to the client
    private void testCheckNewerVersion() {
        // Now prepare the response (simulate an empty one)
        String version  = new String("0.9.2");
        String type     = new String("optional");
        String delivery = new String("20080731");
        String size     = new String("467");
        String url      = new String("url=http://fakeaddress.com:9080/bbpim/test.jad");
        String response = createResponse(version, type, delivery, size, url);
        ta.setResponse(response);
        // Set a listener
        TestUpdaterListener lis = new TestUpdaterListener();
        updater.setListener(lis);
        long now = System.currentTimeMillis();
        // Force the last check timestamp to force check on server
        resetConfig();
        // Set the interval check very low
        config.setCheckInterval(1);
        // Do the update check
        updater.check();
        // We expect the updater to report a new mandatory version
        assertTrue(!lis.isOptional());
        assertTrue(!lis.isMandatory());
        long lastRemind = System.currentTimeMillis();
        config.setLastReminder(lastRemind);
        assertTrue(config.getLastCheck() >= now);
        assertTrue(config.getLastReminder() == lastRemind);
        assertTrue(!updater.isUpdateAvailable());
    }
}


