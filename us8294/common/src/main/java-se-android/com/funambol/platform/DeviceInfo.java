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
package com.funambol.platform;

import java.util.TimeZone;

import android.os.Build;
import android.content.Context;
import android.telephony.TelephonyManager;
import android.accounts.Account;
import android.accounts.AccountManager;
import android.util.DisplayMetrics;
import com.funambol.util.Log;


public class DeviceInfo implements DeviceInfoInterface {

    private static final String TAG_LOG = "DeviceInfo";
    
    private Context context;

    private TelephonyManager tm;

    public DeviceInfo(Context context) {
        this.context = context;
        tm = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    }

    /**
     * Returns the phone number or null if not available.
     */
    public String getPhoneNumber() {
        return tm.getLine1Number();
    }

    /**
     * Returns the platform or null if not available. The platform here is a
     * Funambol identification of the client build.
     */
    public String getFunambolPlatform() {
        return "android";
    }

    /**
     * Returns the main email adddress or null if not available.
     */
    public String getEmailAddress() {
        AccountManager am = AccountManager.get(context);
        Account[] gAccounts = am.getAccountsByType("com.google");
        for(int i=0; i<gAccounts.length; i++) {
            String email = gAccounts[i].name;
            if(isValidEmailAddress(email)) {
                return email;
            }
        }
        return "";
    }

    private boolean isValidEmailAddress(String email) {
        if(email != null) {
            return email.contains("@") && email.contains(".");
        } else {
            return false;
        }
    }

    /**
     * Returns the device timezone or null if not available.
     */
    public String getTimezone() {
        return TimeZone.getDefault().getID();
    }

    /**
     * Returns the device manufacturer or null if not available.
     */
    public String getManufacturer() {
        return Build.MANUFACTURER;
    }

    /**
     * Returns the device model or null if not available.
     */
    public String getDeviceModel() {
        return Build.MODEL;
    }

    /**
     * Returns the carrier name, or null if not available.
     */
    public String getCarrier() {
        return tm.getNetworkOperatorName();
    }

    /**
     * Returns the A2 country code, or null if not available.
     */
    public String getCountryCode() {
        return tm.getNetworkCountryIso();
    }

    public String getHardwareVersion() {
        return Build.FINGERPRINT;
    }

    public String getDeviceId() {
        return tm.getDeviceId();
    }

    public boolean isRoaming() {
        return tm.isNetworkRoaming();
    }

    public boolean isTablet() {
        return DeviceRole.TABLET == getDeviceRole();
    }
    
    public boolean isSmartphone() {
        return DeviceRole.SMARTPHONE == getDeviceRole();
    }
    
    public DeviceRole getDeviceRole() {
        try {
            // Compute screen size
            DisplayMetrics dm = context.getResources().getDisplayMetrics();
            float screenWidth  = dm.widthPixels / dm.xdpi;
            float screenHeight = dm.heightPixels / dm.ydpi;
            double size = Math.sqrt(Math.pow(screenWidth, 2) +
                                    Math.pow(screenHeight, 2));
            //some debug info
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Device recognition data:" +
                        " final size: " + size +
                        ", width pixels: " + dm.widthPixels +
                        ", height pixels: " + dm.heightPixels +
                        ", xdpi: " + dm.xdpi +
                        ", ydpi: " + dm.ydpi);
            }
            
            //fix for Motorola Droid Pro II, wrongly recognized as a tablet
            //Log shows: Device recognition data: final size: 6.803131430828442,
            //           width pixels: 480, height pixels: 854, xdpi: 144.0, ydpi: 144.0
            if ((((480 == dm.widthPixels) && (854 == dm.heightPixels)) ||
                    ((854 == dm.widthPixels) && (480 == dm.heightPixels))) &&
                    (144 == dm.xdpi) &&
                    (144 == dm.ydpi)) {
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Probably we are on a Motorola Droid Pro 2 phone, or similar phone with strange screen size/dpi");
                }
                return DeviceRole.SMARTPHONE;
            }
            
            return size >= 6 ? DeviceRole.TABLET : DeviceRole.SMARTPHONE;
        } catch(Throwable t) {
            Log.error(TAG_LOG, "Failed to compute screen size", t);
            return DeviceRole.UNDEFINED;
        }
    }

    /**
     * Returns the device OS version
     */
    public String getOSVersion() {
        return "" + android.os.Build.VERSION.SDK_INT;
    }

}
