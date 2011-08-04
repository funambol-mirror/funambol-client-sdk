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

package com.funambol.client.engine;

import java.util.Vector;

import com.funambol.platform.NetworkStatus;
import com.funambol.concurrent.TaskExecutor;
import com.funambol.concurrent.Task;

public class NetworkTaskExecutor extends TaskExecutor {

    private static final String TAG_LOG = "NetworkTaskExecutor";

    private static final int NUMBER_OF_THREADS_WIFI = 5;
    private static final int NUMBER_OF_THREADS_UMTS = 3;
    private static final int NUMBER_OF_THREADS_GPRS = 1;

    public NetworkTaskExecutor() {
        super();
    }

    /**
     * Schedule the given task with the given priority.
     * @param task
     * @param priority
     */
    public void scheduleTaskWithPriority(Task task, int priority) {

        // Everytime a new task needs to be scheduled, we update the number of
        // available threads
        updateMaxThreads();
        super.scheduleTaskWithPriority(task, priority);
    }

    private void updateMaxThreads() {
        // Depending on the network conditions we set different number of
        // threads
        NetworkStatus netStatus = new NetworkStatus();
        if (netStatus.isWiFiConnected()) {
            setMaxThreads(NUMBER_OF_THREADS_WIFI);
        } else if (netStatus.isMobileConnected()) {
            int networkType = netStatus.getMobileNetworkType();
            if (networkType == NetworkStatus.MOBILE_TYPE_UMTS) {
                setMaxThreads(NUMBER_OF_THREADS_UMTS);
            } else {
                setMaxThreads(NUMBER_OF_THREADS_GPRS);
            }
        } else {
            setMaxThreads(NUMBER_OF_THREADS_GPRS);
        }
    }
}



