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

package com.funambol.syncclient.common;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.net.MalformedURLException;

/**
 * This class supplies some methods of usefullness
 * for the management of HTTP connection.
 *
 * @version $Id: HttpTools.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class HttpTools {


    // -----------------------------------------------------------Public methods
    /**
     * Download file from the given location.
     *
     * @param fileUrl location from which download the file.
     * @return the content of the file downloaded.
     * @throws IOException if an I/O error occurs.
     */
    public static byte[] downloadPackage(String fileUrl)
    throws IOException {
        byte[] content = null;

        int size = -1;

        URL url = new URL(fileUrl);

        URLConnection urlConn = url.openConnection();
        url.openStream();
        size = urlConn.getContentLength();
        content = new byte[size];

        InputStream iStream =  url.openStream();
        int read = -1;
        int offset = 0;
        int toRead = size;
        while ( (read =  iStream.read(content, offset,toRead)) != -1) {
            toRead = toRead - read;
            offset = offset + read;
        }
        iStream.close();
        return content;

    }

    /**
     * Returns true if the given url is a valid http(s) url.
     *
     * @param url - NOT NULL
     *
     * @return true if <i>url</i>is a valid http(s) url, false otherwise
     */
    public static boolean isHTTPURL(String url) {
        if (!(url.startsWith("http://") || url.startsWith("https://"))) {
            return false;
        }
        
        try {
            new URL(url);
            return true;
        } catch (MalformedURLException e) {}
        
        return false;
    }


}