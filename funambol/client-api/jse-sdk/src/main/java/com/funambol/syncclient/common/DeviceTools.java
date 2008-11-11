/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2007 Funambol, Inc.
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

import java.net.InetAddress;
import java.net.UnknownHostException;

import com.funambol.framework.tools.Base64;
import com.funambol.framework.tools.MD5;


/**
 * This class supplies a method for the creation of a device id based on
 * host address/name and username.
 *
 * @version $Id: DeviceTools.java,v 1.4 2008-01-02 19:24:54 nichele Exp $
 */
public class DeviceTools {

    /**
     * Creates a device id as <code>prefix + B64(MD5(hostName:hostAddress:userName))</code>
     * @param prefix the device id prefix
     * @param defaultValue the default value
     * @return device id as B64(MD5(hostName:hostAddress:userName)) or the default
     *         value if there are errors retrieving host address/name
     */
    public static String createDeviceID(String prefix, String defaultValue) {
        String userName    = System.getProperty("user.name");
        String hostName    = null;
        String hostAddress = null;
        InetAddress addr   = null;
        try {
            addr = InetAddress.getLocalHost();
        } catch (UnknownHostException ex) {
            return defaultValue;
        }
        hostName    = addr.getHostName();
        hostAddress = addr.getHostAddress();

        return computeDeviceID(prefix, userName, hostName, hostAddress);
    }

    // --------------------------------------------------------- Private methods

    private static String computeDeviceID(String prefix,
                                          String userName,
                                          String hostName,
                                          String hostAddress) {

        StringBuffer sb = new StringBuffer(hostName);
        sb.append(':').append(hostAddress).append(':').append(userName);
        String s = new String(Base64.encode(MD5.digest(sb.toString().getBytes())));

        return prefix + s;
    }

}
