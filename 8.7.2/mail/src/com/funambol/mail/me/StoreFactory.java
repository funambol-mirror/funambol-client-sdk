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

package com.funambol.mail;

import com.funambol.util.StringUtil;
import com.funambol.util.Log;

public class StoreFactory {

    private static final String TAG_LOG = "StoreFactory";

    private static String PLATFORM_VERSION = "sw_platform_version=";

    /** The instance of RMS Mail Store */
    private static Store store;

    private static boolean nokiaFP2 = false;

    /**
     * Return the static instance of RMSStore (avoid garbage)
     */
    public static synchronized Store getStore() {
        if(store == null) {

            nokiaFP2 = getIsNokiaFP2();

            // Nokia FP2 requires a special RMSStore with a workaround to avoid
            // its corruption
            if (nokiaFP2) {
                Log.info(TAG_LOG, "Detected Nokia FP2 family, using custom RMSStore");
                store = new NokiaFP2RMSStore();
            } else {
                Log.info(TAG_LOG, "Using standard RMSStore");
                store = new RMSStore();
            }
        }
        return store;
    }

    public static boolean getNokiaFP2RMSStore() {
        return nokiaFP2;
    }

    private static boolean getIsNokiaFP2() {
        boolean res = false;
        try {
            String platform = System.getProperty("microedition.platform");
            if (platform != null) {
                platform = platform.toLowerCase();

                Log.trace(TAG_LOG, "platform = " + platform);

                int idx = platform.indexOf(PLATFORM_VERSION);
                if (platform.indexOf("nokia") != -1 && idx >= 0) {

                    int equal = platform.indexOf('=', idx);
                    if (equal >= 0 && (equal +1) < platform.length()) {
                        int semicolon = platform.indexOf(';',equal + 1);
                        if (semicolon == -1) {
                            semicolon = platform.length() - 1;
                        }
                        String version = platform.substring(equal + 1, semicolon);
                        String ver[] = StringUtil.split(version, ".");
                        if (ver.length > 0) {
                            int major = 0;
                            try {
                                major = Integer.parseInt(ver[0]);
                            } catch (Exception e) {
                                Log.error(TAG_LOG, "Cannot get platform major version", e);
                            }
                            if (major > 3) {
                                res = true;
                            } else if (ver.length > 1) {
                                int minor = 0;
                                try {
                                    minor = Integer.parseInt(ver[1]);
                                } catch (Exception e) {
                                    Log.error(TAG_LOG, "Cannot get platform minor version", e);
                                }

                                if (minor >= 2) {
                                    res = true;
                                }
                            }
                        }
                    }
                }
            }
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot detect platform");
        }
        return res;
    }
}

