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
 *
 *
 */
package com.funambol.util;

/*
 * This class is intended to function as a container for blackberry API specific methods. 
 */
import java.util.Hashtable;
import net.rim.device.api.servicebook.ServiceBook;
import net.rim.device.api.servicebook.ServiceRecord;
import net.rim.device.api.system.DeviceInfo;

public class BlackberryHelper {

    private static int CONNECTION_TIMEOUT = 10 * 60 * 1000; // 10 minutes
    /**
     * no config has been set. value = -1
     */
    public static int CONFIG_NONE = -1;
    /**
     * a working config has been found, but user refused to use it
     */
    public static int CONFIG_REFUSED = -2;

    //used for logging purpose only
    private static String[] configurationsDescriptions;
    //working config
    private static int workingConfig = CONFIG_NONE;
    //selected config (may not be a working one!)
    private static int selectedConfig = CONFIG_NONE;
    //current config
    private static int currentConfig = 0;
    //config array
    private static String[] configurations;
    //the hashtable where we store the APNs
    private static Hashtable apnTable = new Hashtable();
    private static ConnectionHandler connectionHandler = new BasicConnectionHandler();
    private static String COUNTRY_US = "US";
    private static String COUNTRY_IT = "IT";

    // Initialize configurations
    static {

        // building APN table

        Log.debug("[BBhelper] creating apn table...");
        //------ US OPERATORS ----- //
        //ATT Orange (Formerly Cingular)
        apnTable.put("wap.cingular",
                new WapGateway("wap.cingular", "WAP@CINGULARGPRS.COM", "CINGULAR1", COUNTRY_US));
        //T-Mobile US1
        apnTable.put("internet2.voicestream.com",
                new WapGateway("internet2.voicestream.com", null, null, COUNTRY_US));
        //T-Mobile US2
        apnTable.put("wap.voicestream.com",
                new WapGateway("wap.voicestream.com", null, null, COUNTRY_US));
        //Sprint: don't know what sb entry will give, apn should be internet.com
        apnTable.put("internet.com",
                new WapGateway("internet.com", null, null, COUNTRY_US));

        //Verizon should work with no apn



        // ----  IT OPERATORS ---- //
        //Tim, both gprs and wap
        WapGateway tim = new WapGateway("ibox.tim.it", null, null, COUNTRY_IT);
        apnTable.put("ibox.tim.it", tim);
        apnTable.put("wap.tim.it", tim);
        // Wind, normal, wap and biz profiles
        WapGateway wind = new WapGateway("internet.wind", null, null, COUNTRY_IT);
        apnTable.put("internet.wind", wind);
        apnTable.put("internet.wind.biz", wind);
        apnTable.put("wap.wind.biz", wind);
        // Omnitel
        WapGateway omni = new WapGateway("web.omnitel.it", null, null, COUNTRY_IT);
        apnTable.put("web.omnitel.it", omni);
        apnTable.put("wap.omnitel.it", omni);
        Log.debug("[BBhelper] apntable created");
        initConfigs();
    }

    /**
     * 
     * @return true if app should ask the user before connecting with a non-empty
     *  apn. currently this is done for non-us operators
     */
    public static boolean shouldAskUserBeforeConnecting() {

        Log.debug("[BBHelper] Checking if we're in US");
        String[] serviceBookApn = getAllActiveServiceBookApns();

        if (serviceBookApn != null) {
            WapGateway gateway = findGatewayByApn(serviceBookApn);
            if (gateway != null) {
                //if in us return false
                return (!COUNTRY_US.equals(gateway.getCountry()));
            }
        }

        return true;
    }

    public static int getSavedConfigID() {
        return workingConfig;
    }

    public static String getAPNFromConfig(int configId) {

        if (configId == 2) {
            return getServiceBookApn();
        }
        
        // looking for apn
        String sep = ";apn=";
        String config = configurations[configId];
        Log.debug("config= " + config);
        int start = config.indexOf(sep);
        if (start == -1) {
            Log.debug("apn not found");
            return null;
        }
        start += sep.length();
        int stop = config.substring(start).indexOf(";");
        Log.debug("start= " + start + " stop= " + stop);


        if (stop == -1) {
            return config.substring(start);
        } else {
            return config.substring(start, start + stop);
        }
    }

    public static void setConnectionHandler(ConnectionHandler handler) {
        connectionHandler = handler;
    }

    public static ConnectionHandler getConnectionHandler() {
        return connectionHandler;
    }

    /**
     * method that return a string containing given config id description
     * @param configId the config id 
     * @return given config descritpion
     */
    public static String getConfigurationDescription(int configId) {
        return configurationsDescriptions[configId];
    }

    public static String getSavedConfigDescription() {

        if (getSavedConfigID() == CONFIG_NONE) {
            return "no saved config found";
        } else if (getSavedConfigID() == CONFIG_REFUSED) {
            return "connection was refused by user";
        } else {
            return getConfigurationDescription(getSavedConfigID());
        }
    }

    public static int getConfigCount() {
        return configurations.length;
    }

    static public boolean isSimulator() {
        return DeviceInfo.isSimulator();
    }

    static public void saveCurrentConfig() {

        Log.debug("[BBhelper] saving config " + currentConfig);
        workingConfig = currentConfig;
    }

    /**
     *  remove saved config, if present.
     *  this method is used when we're unable to connect, to avoid
     *  trying the same configuration every time
     */
    static public void removeSavedConfig() {
        if (workingConfig != CONFIG_NONE) {

            Log.debug("[BBhelper] removing saved config (" + workingConfig + ")");
            workingConfig = CONFIG_NONE;
        }
    }

    /**
     * 
     * @return true if a working config has been found
     */
    public static boolean workingConfigHasBeenFound() {
        return (workingConfig != CONFIG_NONE) && (workingConfig != CONFIG_REFUSED);
    }

    static public String getOptions(int retry) {


        // Keep rotating all possible configurations till we found one that
        // works
        if (workingConfig >= 0) {
            Log.debug("[getOptions] returning stored configuration " +
                    configurations[workingConfig] +
                    " ( " + configurationsDescriptions[workingConfig] + ")");

            return configurations[workingConfig];
        } else {

            if (selectedConfig != CONFIG_NONE) {
                Log.debug("[getoptions] returning selected configuration " + selectedConfig + " " + configurations[selectedConfig] + " (" +
                        configurationsDescriptions[selectedConfig] + ")");
                return configurations[selectedConfig];
            }

            
            // Try the next one (if it exists)
            
          
            
            
            currentConfig = retry % configurations.length;

            if (configurations[currentConfig] != null) {
                Log.debug("[getoptions] returning configuration " + currentConfig + " " + configurations[currentConfig] + " (" +
                        configurationsDescriptions[currentConfig] + ")");
                return configurations[currentConfig];
            } else {
                Log.debug("[getoptions] returning  configuration ");
                return "";
            }

        }

    }

    /**
     *  @return true if all config are equals to default config (i.e. no sb / 
     *  Magic Table entries have been found)
     */ 
    public static boolean areConfigsEmpty() {
       
        
        for (int i=1; i < configurations.length; i++) {
            
            if (!configurations[i].equals(configurations[0]))  {
                return false;
            }
        }
        
        return true;
    }
    
    
    /**
     * get the options to force the use of the apn from servicebook
     * 
     * @return a string that should be used to connect to the SB apn
     */
    static private String getSBOptions() {
        ServiceBook sb = ServiceBook.getSB();
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);

        //Obtain WAP2 ServiceBook Record
        for (int i = 0; i < records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];

            //check if CID is WPTCP and UID. I think UID can be different per carrier. We could build a list. 
            if (StringUtil.equalsIgnoreCase(sr.getCid(), "WPTCP") &&
                    StringUtil.equalsIgnoreCase(sr.getUid(), "WAP2 trans")) {
                /*
                if (StringUtil.equalsIgnoreCase(sr.getCid(), "BrowserConfig")) {
                 */
                if (records[i].getAPN() != null) {
                    return ";ConnectionUID=" + records[i].getUid();
                }
            }
        }
        return "";
    }

    /**
     * 
     * looks into the apntable to find the right wapgateway apn, 
     * username and password to use with current network. 
     */
    static private String getAPNGatewayOptions() {
        StringBuffer options = new StringBuffer("");
        // String serviceBookApn = getServiceBookApn();
        String[] serviceBookApn = getAllActiveServiceBookApns();
        Log.debug("Trying to find gateway for APN: " + serviceBookApn);
        System.out.println("Trying to find gateway for APN: " + serviceBookApn);

        if (serviceBookApn != null) {
            WapGateway gateway = findGatewayByApn(serviceBookApn);
            if (gateway != null) {
                //We matched with a gateway in our list. Build connection options
                options.append(";apn=" + gateway.getApn());
                options.append(";WapGatewayAPN=" + gateway.getApn());
                if (gateway.getUsername() != null) {
                    options.append(";TunnelAuthUsername=" + gateway.getUsername());
                }
                if (gateway.getPassword() != null) {
                    options.append(";TunnelAuthPassword=" + gateway.getPassword());
                }
            }
        }
        return options.toString();

    }

    static public boolean isOkToUseConfig(int configNumber) {

        int config = configNumber % configurations.length;

        //if we have a config that's equals to the default one, we can use it
        //TODO use getdefaultconfigoptions to check if it's the case
        if (config == 0 || (configurations[config].equals(configurations[0]))) {
            return true;
        } else if (workingConfig == CONFIG_REFUSED) {
            return false;
        } else {
            boolean ok = connectionHandler.isOkToUseConfig(getAPNFromConfig(config));
            if (!ok) {
                workingConfig = CONFIG_REFUSED;
            } else {
            //  selectedConfig = configNumber;
            }
            return ok;
        }
    }

    /**
     * 
     * @return something
     */
    static public String getServiceBookApn() {

        //get only active service records
        ServiceBook sb = ServiceBook.getSB();
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);
        String apn = null;
        //Obtain WAP2 ServiceBook Record
        for (int i = 0; i <
                records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];
            //check if CID is WPTCP and UID. I think UID can be different per carrier. We could build a list. 
            if (isValidServiceRecordEntry(sr)) {
                apn = records[i].getAPN();
            }
        }

        return apn;
    }

    static public String[] getAllActiveServiceBookApns() {

        //get only active service records
        ServiceBook sb = ServiceBook.getSB();
        ServiceRecord[] records = sb.findRecordsByType(ServiceRecord.SRT_ACTIVE);

        //       String apn = null;
        String[] apn = new String[records.length];
        //Obtain WAP2 ServiceBook Record
        for (int i = 0; i < records.length; i++) {
            //get the record
            ServiceRecord sr = records[i];
            //check if CID is WPTCP and UID. I think UID can be different per carrier. We could build a list. 
            apn[i] = sr.getAPN();

        }

        return apn;
    }

    /**
     * look into the apntable to find the correct gateway given the apn.
     * gateway is composed by apn, username and password.
     * @param apn the apn to look for
     * @return the correct gateway object. Note that given an apn, tha gateway may
     * use a different apn, e.g. given wap.tim.it the returned gateway has 
     * ibox.tin.it as apn, this is due to the fact that some apns does not allow
     *  tcp communications
     */
    private static WapGateway findGatewayByApn(String[] apn) {


        /*  WapGateway ret = (WapGateway) apnTable.get(apn.toLowerCase());
        Log.debug("apnTable returned " + ret.getApn() + " for apn " + apn);
        return ret;
         */
        for (int i = 0; i < apn.length; i++) {
            if (apn[i] != null) {
                WapGateway ret = (WapGateway) apnTable.get(apn[i].toLowerCase());
                if (ret != null) {
                    Log.debug("apnTable returned " + ret.getApn() + " for apn " + apn);
                    return ret;
                } else {
                    Log.debug("apnTable found no entry for apn " + apn);
                }
            }

        }
        Log.info("[BBHelper] apnTable entry not found... returning null");
        return null;

    }

    // initing configs
    public static void initConfigs() {
        Log.info("[BBHelper] init configs");
        configurations = new String[3];
        configurationsDescriptions = new String[configurations.length];

        for (int j = 0; j < configurations.length; ++j) {
            configurations[j] = null;
        }

        // filling configs

        // apn from user
        configurations[0] = getBaseConfigOptions();
        configurationsDescriptions[0] = "settings from device";


        //get info from table
        configurations[1] = getBaseConfigOptions() + getAPNGatewayOptions();
        configurationsDescriptions[1] = "settings from apn table";



        // http http get data from the table tries the sb apn with sb
        configurations[2] = getBaseConfigOptions() + getSBOptions();
        configurationsDescriptions[2] = "settings from service book";


        Log.info("[BBHelper] Configs created");

    }

    /**
     * 
     * 
     * @return the default config options
     */
    private static String getBaseConfigOptions() {

       return ";deviceside=true";

    }

    /**
     * @return a string describing current configs
     */
    public static String getConfigsDescription() {
        StringBuffer configs = new StringBuffer("Config:");

        for (int i = 0; i < configurations.length; i++) {
            configs.append("\n[" + i + "] " + configurations[i]);

        }

        return configs.toString();
    }

    private static boolean isValidServiceRecordEntry(ServiceRecord sr) {


        //TODO: use a table or something to store this data

        return ( // wind, tim & US
                (StringUtil.equalsIgnoreCase(sr.getCid(), "WPTCP") &&
                StringUtil.equalsIgnoreCase(sr.getUid(), "WAP2 trans")) ||
                // Vodafone it
                (StringUtil.equalsIgnoreCase(sr.getCid(), "WAP") &&
                StringUtil.equalsIgnoreCase(sr.getUid(), "vfit WAPtrans")));

    }

    static final class WapGateway {

        private String apn;
        private String username;
        private String password;
        private String country;

        public WapGateway(String apn, String username, String password, String country) {

            super();
            this.apn = apn;
            this.username = username;
            this.password = password;
            this.country = country;
        }

        public String getApn() {

            return apn;
        }

        public String getUsername() {

            return username;
        }

        public String getPassword() {

            return password;
        }

        public String getCountry() {

            return country;

        }
    }
}

