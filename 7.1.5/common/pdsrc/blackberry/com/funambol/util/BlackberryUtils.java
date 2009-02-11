/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2008 Funambol, Inc.
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

import net.rim.device.api.servicebook.ServiceBook;
import net.rim.device.api.servicebook.ServiceRecord;
import net.rim.device.api.system.CoverageInfo;
import net.rim.device.api.system.DeviceInfo;
import net.rim.device.api.system.RadioInfo;

/**
 * This class is a wrapper for J2me Funambol Common API. It provides 
 * informations about Blackberry devices configurations and allow to access some 
 * system properties that are peculiar of Blackberry devices only. For this 
 * reason this class has only been implemented into the Blackberry devices 
 * platform dependent module.
 */
public class BlackberryUtils {
    private static final String COVERAGE_CARRIER = "Carrier full Coverage";
    private static final String COVERAGE_MDS = "BES coverage";
    private static final String COVERAGE_NONE = "No coverage";
    private static final String NOT_SUPPORTED_WAF = "Not supported by the device";

    /**
     * Access the net.rim.device.api.system.DeviceInfo class in order to 
     * understand if the running system is a simulator or a real device.
     * @return true if the current application is running on a Blackberry 
     * simulator, false otherwise
     */
    public boolean isSimulator() {
        return DeviceInfo.isSimulator();
    }

    /**
     * Give the information about the presence o a wifi bearer on the device
     * @return true if the wifi communication interface bearer is supported by 
     * the device, false otherwise
     */
    protected static boolean isWifiAvailable() {
        Log.info("Checking WIFI Availability");
        boolean isWifiEnabled;
        if (RadioInfo.areWAFsSupported(RadioInfo.WAF_WLAN)) {
            Log.info("WIFI Supported");
            isWifiEnabled = true;
        } else {
            Log.info("WIFI NOT Supported");
            isWifiEnabled = false;
        }
        return isWifiEnabled;
    }

    /**
     * Give information about the presence of active wifi connections. 
     * @return true if the device is connected to a wifi network with its wifi 
     * bearer, false otherwise
     */
    protected static boolean isWifiActive() {
        Log.info("Checking WIFI Availability");

        int active = RadioInfo.getActiveWAFs();
        int wifi = RadioInfo.WAF_WLAN;

        Log.debug("Active WAFs Found: " + active);
        Log.debug("WIFI WAF DEFINITION: " + wifi);

        return active >= wifi;
    }

    protected static boolean isWapGprsDataBearerOffline() {
        return RadioInfo.getState()==RadioInfo.STATE_OFF ||
               RadioInfo.getSignalLevel() == RadioInfo.LEVEL_NO_COVERAGE;
    }

    public static String getNetworkCoverageReport() {
        StringBuffer sb = new StringBuffer();
        
        
        sb.append("\nWireless Access Families:");
        sb.append("\n3GPP: " + getNetworkCoverage(RadioInfo.WAF_3GPP));
        sb.append("\nCDMA: " + getNetworkCoverage(RadioInfo.WAF_CDMA));
        sb.append("\nWLAN: " + getNetworkCoverage(RadioInfo.WAF_WLAN));
        sb.append("\nCDMA: " + getNetworkCoverage(RadioInfo.NETWORK_CDMA));
        sb.append("\nBands:");
        sb.append("\nCDMA_800: " + getNetworkCoverage(RadioInfo.BAND_CDMA_800));
        sb.append("\nCDMA_1900: " + getNetworkCoverage(RadioInfo.BAND_CDMA_1900));
        sb.append("\nNetworks:");
        sb.append("\n802_11: " + getNetworkCoverage(RadioInfo.NETWORK_802_11));
        sb.append("\nGPRS: " + getNetworkCoverage(RadioInfo.NETWORK_GPRS));
        sb.append("\nNetwork services:");
        sb.append("\nVOICE: " + getNetworkCoverage(RadioInfo.NETWORK_SERVICE_VOICE));
        sb.append("\nUMTS: " + getNetworkCoverage(RadioInfo.NETWORK_SERVICE_UMTS));
        sb.append("\nEDGE: " + getNetworkCoverage(RadioInfo.NETWORK_SERVICE_EDGE));
        return sb.toString();
    }

    private static String getNetworkCoverage(int networkType) {
        if (RadioInfo.areWAFsSupported(networkType)) {
            int status = CoverageInfo.getCoverageStatus(networkType, false);
            switch (status) {
                case CoverageInfo.COVERAGE_CARRIER:
                    return COVERAGE_CARRIER;
                case CoverageInfo.COVERAGE_MDS:
                    return COVERAGE_MDS;
                case CoverageInfo.COVERAGE_NONE:
                    return COVERAGE_NONE;
                default:
                    break;
            }
        } 
        return NOT_SUPPORTED_WAF;
    }
    
    /**
     * Validate the given ServiceRecord entry: in order to be validated it must 
     * be a WAP or WAP2 transport entry 
     * @param sr is the ServiceRecord to be checked
     * @return true if 
     */
    public static boolean isWapTransportServiceRecord(ServiceRecord sr) {
        //TODO: use a table to store this data
        return ( 
            // wind, tim & US
            (StringUtil.equalsIgnoreCase(sr.getCid(), "WPTCP") && StringUtil.equalsIgnoreCase(sr.getUid(), "WAP2 trans")) ||
            // Vodafone it
            (StringUtil.equalsIgnoreCase(sr.getCid(), "WAP") && StringUtil.equalsIgnoreCase(sr.getUid(), "vfit WAPtrans")));
    }
    
    /**
     * Retrieves the WAP/WAP2 Transport APN from service book
     * @return the Stirng formatted WAP/WAP2 Transport APN. This entry is 
     * unique for every ServiceBook.
     */
    public static String getServiceBookWapTransportApn() {
        //get only active service records
        ServiceBook sb = ServiceBook.getSB();
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);
        String apn = null;
        //Obtain WAP2 ServiceBook Record
        for (int i = 0; i <records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];
            //check if CID is WPTCP and UID. UID could be different per carrier. 
            //TODO: We could build a list. 
            if (BlackberryUtils.isWapTransportServiceRecord(sr)) {
                apn = records[i].getAPN();
            }
        }
        return apn;
    }

    /**
     * Retrieves only ACTIVE ServiceRecords (WAP2 type)from the native device's 
     * ServiceBook checking if their CID is WPTCP and UID
     * @return String[] with the active APN found into the device's ServiceBook
     */
    public static String[] getAllActiveServiceBookAPNs() {
        ServiceBook sb = ServiceBook.getSB();
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);

        String[] apn = new String[records.length];
        
        //Retrieve "WAP2" ServiceBook Record
        for (int i = 0; i < records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];
            //check if CID is WPTCP and UID. UID can be different 
            //per carrier. We could build a list. 
            apn[i] = sr.getAPN();
        }
        return apn;
    }
    
        /**
     * Get the options to use the list of APN included into the device
     * ServiceBook
     * @return a string that should be added to the url parameters
     */
    public static String getServiceBookOptions() {
        ServiceBook sb = ServiceBook.getSB();
        
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);

        
        //Obtain WAP2 ServiceBook Record
        for (int i = 0; i < records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];

            //check if CID is WPTCP and UID. 
            //UID could be different per carrier. 
            //TODO - We could build a list. 
            if (StringUtil.equalsIgnoreCase(sr.getCid(), "WPTCP") &&
                    StringUtil.equalsIgnoreCase(sr.getUid(), "WAP2 trans")) {
                if (records[i].getAPN() != null) {
                    return ";ConnectionUID=" + records[i].getUid();
                }
            }
        }
        return "";
    }
    
    /**
     * Give global information about the data connection bearer activity
     * @return true if there is an active bearer including the check for 
     * WIFI and WAP/GPRS bearer
     */
    public static boolean isDataConnectionAvailable() {
        boolean ret = 
                (isWifiAvailable()&&isWifiActive())||
                !isWapGprsDataBearerOffline();
        Log.debug("[BlackberryUtils.isDataConnectionAvailable]Data connection availability: " + ret);
        return ret;
    }
}
