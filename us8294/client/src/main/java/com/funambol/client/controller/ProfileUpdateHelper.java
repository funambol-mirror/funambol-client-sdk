package com.funambol.client.controller;

import java.util.Enumeration;

import com.funambol.client.configuration.Configuration;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceConfig;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;
import com.funambol.sapisync.SapiSyncHandler;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

public class ProfileUpdateHelper {

    private static final String TAG_LOG = "ProfileUpdateHelper";
    
    private JSONObject profile; 
    private String username;
    private String password;
    private String serverUri;
    private Configuration configuration;
    private AppSyncSourceManager appSyncSourceManager; 
    
    public ProfileUpdateHelper(AppSyncSourceManager appSyncSourceManager, Configuration configuration) {
        profile = null;
        
        this.appSyncSourceManager = appSyncSourceManager;
        this.configuration = configuration;
        
        serverUri = configuration.getSyncUrl();
        username = configuration.getUsername();
        password = configuration.getPassword();
    }
        
    public void updateProfile() throws JSONException {
        String baseUrl = StringUtil.extractAddressFromUrl(serverUri);
        SapiSyncHandler sapiHandler = new SapiSyncHandler(baseUrl, username, password);

        long now = System.currentTimeMillis();
        JSONObject response = sapiHandler.loginAndGetServerInfo();
        if (!response.has("data")) {
            // This server does not have the new login API. For backward
            // compatibility we condider all sources allowed
            return;
        }

        // If the expire date has already expired, then this info is no longer
        // valid, and we must fall to the default profile
        long deltaTime = 0;
        if (response.has("responsetime")) {
            long responseTime = response.getLong("responsetime");
            // Compute the delta time between server and client
            deltaTime = responseTime - now;
        }
        
        JSONObject data = response.getJSONObject("data");
        JSONObject details = data.getJSONObject("details");
        if (details.has("expiretime")) {
            long expireDate = details.getLong("expiretime");
            // Adjust the expireDate
            expireDate -= deltaTime;
            configuration.setProfileExpireDate(expireDate);
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Found a new profile expire date set to " + expireDate);
            }
        }

        JSONArray remoteSources = details.getJSONArray("sources");

        // Analyse the server response and check what's
        // available/allowed on the server
        Enumeration sources = appSyncSourceManager.getWorkingSources();
        while(sources.hasMoreElements()) {
            AppSyncSource appSource = (AppSyncSource)sources.nextElement();
            AppSyncSourceConfig appSourceConfig = appSource.getConfig();

            // Search if this source is available on server
            boolean found = false;
            for(int i=0;i<remoteSources.length();++i) {
                JSONObject s = remoteSources.getJSONObject(i);
                String sourceName  = s.getString("name");
                String sourceValue = s.getString("value");

                if (appSource.getSyncSource().getConfig().getRemoteUri().equals(sourceName)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Found a source available locally and on server " + sourceName
                                + "," + sourceValue);
                    }
                    // Source is available on server
                    if ("enabled".equals(sourceValue)) {
                        appSourceConfig.setAllowed(true);
                    } else {
                        appSourceConfig.setAllowed(false);
                    }
                    found = true;
                }
            }

            if (!found) {
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Source " + appSource.getName() + " not available on server, will be disabled");
                }
                appSourceConfig.setAllowed(false);
            }

            appSourceConfig.save();
        }

        // Now grab the other properties
        if (details.has("properties")) {
            JSONArray properties = details.getJSONArray("properties");
            for(int i=0;i<properties.length();++i) {
                JSONObject prop = properties.getJSONObject(i);
                String propName = prop.getString("name");
                String propValue = prop.getString("value");
                if ("auto-sync".equals(propName)) {
                    configuration.setProfileManualOnly("disabled".equals(propValue));
                    if ("disabled".equals(propValue))
                        configuration.setSyncMode(Configuration.SYNC_MODE_MANUAL);
                } else if ("network-warning".equals(propName)) {
                    configuration.setProfileNetworkUsageWarning("enabled".equals(propValue));
                } else {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Unsupported property " + propName);
                    }
                }
            }
        }

        configuration.save();
    }
}
