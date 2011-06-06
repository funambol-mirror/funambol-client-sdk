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

package com.funambol.client.test.util;

import com.funambol.platform.FileAdapter;
import com.funambol.platform.HttpConnectionAdapter;
import com.funambol.sync.SyncConfig;
import com.funambol.util.CodedException;
import com.funambol.util.ConnectionManager;
import com.funambol.util.HttpTransportAgent;
import com.funambol.util.Log;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Singleton implementation of the test files core management logic. Use the
 * singleton instance of this class through the getInstance() method. The
 * exposed interface can be useful to retrieve generic file to be loaded from
 * different contents: known supported content protocol are "http" and "file".
 */
public class TestFileManager {

    private static final String TAG_LOG = "TestFileManager";

    protected HttpTransportAgent ta = null;
    protected SyncConfig config = null;

    public TestFileManager() {
    }

    /**
     * Get a script content given the absolute script url.
     * @param scriptUrl is the String representation of the script location
     * @return String the String formatted content of the given script
     * @throws Exception if any error occurred while retrieving the content
     * of the script. This Exception can
     */
    public String getFile(String scriptUrl) throws Exception {
        try {
            if (scriptUrl.startsWith("http")) {
                return getScriptViaHttp(scriptUrl);
            } else if (scriptUrl.startsWith("file")) {
                return getScriptViaFile(scriptUrl);
            } else {
                Log.error(TAG_LOG, "Unknwon protocol to fetch script " + scriptUrl);
                throw new IllegalArgumentException("Cannot fetch script");
            }
        } catch (IOException ioe){
            Log.error(TAG_LOG, "Cannot read file at " + scriptUrl);
            throw ioe;
        } catch (CodedException ce){
            Log.error(TAG_LOG, "Cannot read url for file  at " + scriptUrl);
            throw ce;
        }
    }

    /**
     * Accessor Method to get the base url related to the tests location
     * @param scriptUrl the String formatted script url
     * @return String the String formatted url to be used as the base url for
     * tests. Example: be the main script url
     * "http://url.somewhere.com/folder1/folder2/Test.txt", this method will
     * return be "http://url.somewhere.com/folder1/folder2". This method just
     * return the url after the computation. Can return null if the baseUrl
     * has not yet been calculated.
     */
    public String getBaseUrl(String scriptUrl) {
        int pos = scriptUrl.indexOf('/');
        int lastPos = pos;

        while (pos != -1) {
            lastPos = pos;
            pos = scriptUrl.indexOf('/', pos + 1);
        }

        if (lastPos != -1) {
            return scriptUrl.substring(0, lastPos);
        } else {
            return null;
        }
    }

    //TODO: use HttpConnectionAdapter in place of HttpTransportAgent
    protected String getScriptViaHttp(String url) throws CodedException {
        //Create the transport agent with the given url and encoding
        //compression and cookie usage are set to false
        //the user agent is null
        ta = new HttpTransportAgent(url, null, "UTF-8", false, false);
        // Force messages to be resent in case of errors
        ta.setResendMessageOnErrors(true);
        ta.setRequestContentType(ta.getRequestContentType() + ";charset=utf-8");
        String response = ta.sendMessage("");
        return response;
    }

    protected String getScriptViaFile(String url) throws Exception {
        FileAdapter fa = new FileAdapter(url, true);
        int size = (int) fa.getSize();
        byte data[] = new byte[size];
        InputStream is = fa.openInputStream();
        is.read(data);
        is.close();
        fa.close();
        return new String(data);
    }

    /**
     * Download a file to the given OutputStream and return a proper mime type.
     * @param url
     * @param output
     * @return the mimetype of the downloaded file
     * @throws Throwable
     */
    public String getFile(String url, OutputStream output) throws Exception {
        if (url.startsWith("http")) {
            return getFileViaHttp(url, output);
        } else if (url.startsWith("file")) {
            return getFileViaFile(url, output);
        } else {
            throw new IllegalArgumentException("Invalid protocol");
        }
    }
    
    protected String getFileViaHttp(String url, OutputStream output) throws Exception {
        HttpConnectionAdapter conn = null;
        InputStream is = null;
        try {
            conn = ConnectionManager.getInstance().openHttpConnection(url, null);
            conn.setRequestMethod(HttpConnectionAdapter.GET);
            conn.execute(null, -1);
            if (conn.getResponseCode() == HttpConnectionAdapter.HTTP_OK) {
                is = conn.openInputStream();
                int b;
                do {
                    b = is.read();
                    if (b != -1) {
                        output.write(b);
                    }
                } while(b != -1);
                output.flush();
                output.close();
            }
            return conn.getHeaderField("Content-Type");
        } finally {
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {}
                is = null;
            }
            if (conn != null) {
                try {
                    conn.close();
                } catch (IOException ioe) {}
                conn = null;
            }
        }
    }

    protected String getFileViaFile(String url, OutputStream output) throws Exception {
        FileAdapter fa = new FileAdapter(url, true);
        int size = (int) fa.getSize();
        byte data[] = new byte[size];
        InputStream is = fa.openInputStream();
        is.read(data);
        is.close();
        fa.close();
        output.write(data);
        return getMimeTypeFromUrl(url);
    }

    protected String getMimeTypeFromUrl(String url) {
        if(url.endsWith(".jpg") || url.endsWith(".jpeg")) {
            return "image/jpeg";
        } else if(url.endsWith(".gif")) {
            return "image/gif";
        } else if(url.endsWith(".png")) {
            return "image/png";
        } else if(url.endsWith(".svg")) {
            return "image/svg+xml";
        } else if(url.endsWith(".3gp")) {
            return "video/3gpp";
        } else if(url.endsWith(".mp4")) {
            return "video/mp4";
        } else if(url.endsWith(".avi")) {
            return "video/avi";
        } else {
            return "application/*";
        }
    }
}
