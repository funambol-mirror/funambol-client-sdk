/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

import java.io.IOException;

import com.funambol.platform.HttpConnectionAdapter;
import com.funambol.platform.TestableHttpConnectionAdapter;
import com.funambol.platform.SocketAdapter;
import com.funambol.platform.net.ProxyConfig;

/**
 * Controls HTTP and HTTPS connections requested by the API implementations. 
 */
public class ConnectionManager {

    private static final String TAG_LOG = "ConnectionManager";

    /**
     * This class implements the singleton pattern only this instance can be 
     * used by other classes
     */
    private static ConnectionManager instance =  null;

    private String breakOnPhase = null;
    private String breakOnKey   = null;
    private int breakOnPos      = -1;
    private int breakOnItemIdx  = -1;
    private int breakOnItemCounter = 0;
    
    /**
     * Private constructor - Use getInstance() method
     */
    protected ConnectionManager() {
    }

    /**
     * Singleton implementation:
     * @return the current instance of this class or a new instance if it the 
     * current instance is null
     */
    public static ConnectionManager getInstance() {
        if (instance == null) {
            Log.debug(TAG_LOG, "Creating new connection manager");
            instance = new ConnectionManager();
            return instance;
        } else {
            return instance;
        }
    }

    /**
     * Open an http connection to the given URL
     * 
     * @param url is the url (in the form of "http://..." or "https://...")
     * @param proxyConfig proxy configuration, null if no proxy is required
     * @param extra is some extra information that can be specified to specific
     * implementations
     * @throws IOException if the connection cannot be established
     */
    public HttpConnectionAdapter openHttpConnection(String url, ProxyConfig proxyConfig, Object extra) throws IOException {

        /////////// This code is here only to support automatic test ////////////////
        // If the connection manager has been programmed to break on a given
        // operation, then we check for the extra to identify the proper
        // connection
        if (breakOnPhase != null && (breakOnKey != null || breakOnItemIdx != -1)) {
            String key = "";
            String phase = null;
            if (extra instanceof String) {
                String e[] = StringUtil.split((String)extra, ",");
                if (e != null && e.length > 0) {
                    // Check if this is a test specification info
                    for(int i=0;i<e.length;++i) {
                        String v = e[i];
                        if ("key".equals(v)) {
                            // The next value must be the item id
                            key = e[i+1];
                        } else if ("phase".equals(v)) {
                            phase = e[i+1];
                        }
                    }
                }
            }

            // Count the items for this phase only
            if (breakOnPhase.equals(phase)) {
                breakOnItemCounter++;
            }

            if (breakOnPhase.equals(phase) && (breakOnItemIdx == breakOnItemCounter || key.equals(breakOnKey))) {
                // Create a testable connection
                if (Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Creating a test connection set to break " + breakOnPhase + ", " + breakOnPos);
                }
                TestableHttpConnectionAdapter res = new TestableHttpConnectionAdapter();
                res.setBreakInfo(breakOnPhase, breakOnPos);
                // Now open the connection
                res.open(url, proxyConfig);
                // This is the item where we must break, reset the break info
                // (we do it only if we had no exception opening the connection)
                breakOnKey = null;
                breakOnPhase = null;
                breakOnItemIdx = -1;
                breakOnItemCounter = 0;
                return res;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////

        HttpConnectionAdapter res = new HttpConnectionAdapter();
        res.open(url, proxyConfig);
        return res;
    }

    /**
     * Open an http connection to the given URL
     * 
     * @param url is the url (in the form of "http://..." or "https://...")
     * @param extra is some extra information that can be specified to specific
     * implementations
     * @throws IOException if the connection cannot be established
     */
    public HttpConnectionAdapter openHttpConnection(String url, Object extra) throws IOException {

        return openHttpConnection(url, null, extra);
    }

    /**
     * Open a socket connection to the given URL
     * 
     * @param addr
     * @param port
     * @param mode
     * @param timeout
     * @param proxyConfig proxy configuration, null if no proxy is required
     * @return
     * @throws IOException
     */
    public SocketAdapter openSocketConnection(String addr, int port, int mode, boolean timeout, ProxyConfig proxyConfig) throws IOException {
        SocketAdapter res = new SocketAdapter(addr, port, mode, timeout, proxyConfig);
        return res;
    }
    
    /**
     * Open a socket connection to the given URL
     * 
     * @param addr
     * @param port
     * @param mode
     * @param timeout
     * @return
     * @throws IOException
     */
    public SocketAdapter openSocketConnection(String addr, int port, int mode, boolean timeout) throws IOException {
        return openSocketConnection(addr, port, mode, timeout, null);
    }

    //////////////////////// This code is for automatic tests only /////////////////////////
    public void setBreakInfo(String breakOnPhase, String breakOnKey, int breakOnPos, int itemIdx) {
        this.breakOnPhase = breakOnPhase;
        this.breakOnKey   = breakOnKey;
        this.breakOnPos   = breakOnPos;
        this.breakOnItemIdx = itemIdx;
    }
}

