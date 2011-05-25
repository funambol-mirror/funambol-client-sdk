/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2011 Funambol, Inc.
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
package com.funambol.client.controller;

import java.util.Hashtable;

import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;



class SapiLoginMockData {

    private static final long ONE_DAY_MILLIS = 1000 * 60 * 60 * 24;
    private static final long ONE_WEEK_MILLIS = ONE_DAY_MILLIS * 7;
    private static final long ONE_MONTH_MILLIS = ONE_DAY_MILLIS * 30;

    private static Hashtable data = null;

    public static JSONObject getProfileInformation(String baseUrl, String username, String password) throws JSONException {

        if (data == null) {
            initProfileInfo();
        }

        JSONObject res = (JSONObject)data.get(username);
        if (res == null) {
            res = getDefaultProfileInformation();
        }

        return res;
    }

    private static void initProfileInfo() throws JSONException {

        data = new Hashtable();

        data.put("3357655849", getTransactionalPlan());
        data.put("3346237551", getWeeklyPlan());
        data.put("3666659583", getMonthlyPlan());
        data.put("3462926190", getDailyPlan());
        data.put("3387171573", getWeeklyPlan());
    }

    private static JSONObject getTransactionalPlan() throws JSONException {
        long now = System.currentTimeMillis();
        long expireTime = now + ONE_DAY_MILLIS;
        return getBasePlan(false, expireTime);
    }

    private static JSONObject getWeeklyPlan() throws JSONException {
        long now = System.currentTimeMillis();
        long expireTime = now + ONE_WEEK_MILLIS;
        return getBasePlan(true, expireTime);
    }

    private static JSONObject getMonthlyPlan() throws JSONException {
        long now = System.currentTimeMillis();
        long expireTime = now + ONE_MONTH_MILLIS;
        return getBasePlan(true, expireTime);
    }

    private static JSONObject getDailyPlan() throws JSONException {
        long now = System.currentTimeMillis();
        long expireTime = now + ONE_DAY_MILLIS;
        return getBasePlan(false, expireTime);
    }

    private static JSONObject getDefaultProfileInformation() throws JSONException {
        return getMonthlyPlan();
    }

    private static JSONObject getBasePlan(boolean premium, long expireDate) throws JSONException {

        JSONObject user1 = new JSONObject();        
        JSONObject user1Data = new JSONObject();        
        user1.put("data", user1Data);
        
        JSONObject user1Details = new JSONObject();
        JSONArray user1Sources = new JSONArray();

        JSONObject user1Source1 = new JSONObject();
        user1Source1.put("name","card");
        user1Source1.put("value","enabled");
        user1Sources.put(user1Source1);

        JSONObject user1Source2 = new JSONObject();
        user1Source2.put("name","event");
        user1Source2.put("value","enabled");
        user1Sources.put(user1Source2);

        if (premium) {
            JSONObject user1Source3 = new JSONObject();
            user1Source3.put("name","picture");
            user1Source3.put("value","enabled");
            user1Sources.put(user1Source3);

            JSONObject user1Source4 = new JSONObject();
            user1Source4.put("name","video");
            user1Source4.put("value","enabled");
            user1Sources.put(user1Source4);

            JSONObject user1Source5 = new JSONObject();
            user1Source5.put("name","file");
            user1Source5.put("value","enabled");
            user1Sources.put(user1Source5);
        }

        // Set auto sync mode
        JSONArray properties = new JSONArray();
        JSONObject autoSyncProp = new JSONObject();
        autoSyncProp.put("name","auto-sync");
        autoSyncProp.put("value", premium ? "enabled" : "disabled");
        properties.put(autoSyncProp);
        
        // Set network warning
        JSONObject networkWarningProp = new JSONObject();
        networkWarningProp.put("name","network-warning");
        networkWarningProp.put("value","enabled");
        properties.put(networkWarningProp);
        
        user1Details.put("properties",properties);

        // Set the expire date
        user1Details.put("expiretime", expireDate);

        user1Details.put("sources",user1Sources);
        //user1Details.put("details",user1Details);
        user1Data.put("details", user1Details);

        return user1;
    }


}
