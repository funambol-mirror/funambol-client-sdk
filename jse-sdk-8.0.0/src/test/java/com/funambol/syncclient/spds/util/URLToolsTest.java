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

package com.funambol.syncclient.spds.util;

import junit.framework.TestCase;

/**
 * URLTools test cases
 * @version $Id$
 */
public class URLToolsTest extends TestCase {
    
    public URLToolsTest(String testName) {
        super(testName);
    }

    // -------------------------------------------------------------- Test cases

    /**
     * Test of getSyncManager method, of class SyncManager.
     */
    public void testReplaceServer() throws Throwable {

        String respURI = "http://server:1234/sync;jsessionid=abcde?param1=value1&param2=value2";
        String syncmlURL = "http://syncserver:5678/sync";
        String newURL = URLTools.replaceServer(syncmlURL, respURI);

        String expectedURL = "http://syncserver:5678/sync;jsessionid=abcde?param1=value1&param2=value2";

        assertEquals(expectedURL, newURL);

        respURI = "http://server:1234/funambol/ds;jsessionid=abcde?param1=value1&param2=value2";
        syncmlURL = "http://syncserver:5678/sync?paramA=valueA";
        newURL = URLTools.replaceServer(syncmlURL, respURI);

        expectedURL = "http://syncserver:5678/funambol/ds;jsessionid=abcde?param1=value1&param2=value2";

        assertEquals(expectedURL, newURL);

        respURI = "http://server:1234/sync;jsessionid=abcde?param1=value1&param2=value2";
        syncmlURL = "http://syncserver:5678/funambol/ds";
        newURL = URLTools.replaceServer(syncmlURL, respURI);

        expectedURL = "http://syncserver:5678/sync;jsessionid=abcde?param1=value1&param2=value2";

        assertEquals(expectedURL, newURL);
    }

}
