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

import android.content.Context;
import android.telephony.TelephonyManager;
import android.telephony.ServiceState;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

public class NetworkStatus implements NetworkStatusI {

    private Context context;
    private ConnectivityManager connectivityManager;
    private TelephonyManager telephonyManager;

    public NetworkStatus() {
        PlatformEnvironment env = PlatformEnvironment.getInstance();
        this.context = env.getContext();
        connectivityManager = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
        telephonyManager = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
    }

    public boolean isWiFiConnected() {
        NetworkInfo netInfo = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (netInfo != null) {
            return netInfo.getState() == NetworkInfo.State.CONNECTED;
        } else {
            return false;
        }
    }

    public boolean isMobileConnected() {
        NetworkInfo netInfo = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
        if (netInfo != null) {
            return netInfo.getState() == NetworkInfo.State.CONNECTED;
        } else {
            return false;
        }
    }

    public int getMobileNetworkType() {
        boolean is3G = (telephonyManager.getNetworkType() == TelephonyManager.NETWORK_TYPE_UMTS);
        // Everything below UMTS is considered GPRS
        return is3G ? MOBILE_TYPE_UMTS : MOBILE_TYPE_GPRS;
    }

    public boolean isConnected() {
        NetworkInfo netInfo = connectivityManager.getActiveNetworkInfo();
        if (netInfo != null) {
            return netInfo.isConnected();
        } else {
            return false;
        }
    }


    public boolean isRadioOff() {
        ServiceState serviceState = new ServiceState();
        return serviceState.getState() == ServiceState.STATE_POWER_OFF;
    }


}
