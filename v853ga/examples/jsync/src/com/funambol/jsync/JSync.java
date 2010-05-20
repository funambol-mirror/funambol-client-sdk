/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.jsync;

import java.io.IOException;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.OutputStream;
import java.io.DataOutputStream;

import com.funambol.syncml.spds.SyncManager;
import com.funambol.syncml.spds.SyncConfig;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.client.CacheTracker;
import com.funambol.syncml.client.FileSyncSource;
import com.funambol.storage.StringKeyValueStore;
import com.funambol.storage.StringKeyValueMemoryStore;
import com.funambol.storage.StringKeyValueFileStore;
import com.funambol.platform.FileAdapter;
import com.funambol.util.Log;
import com.funambol.util.ConsoleAppender;

public class JSync {

    private String args[];
    private String username  = null;
    private String password  = null;
    private String url       = null;
    private int    logLevel  = Log.DISABLED;
    private int    syncMode  = SyncML.ALERT_CODE_FAST;
    private String remoteUri = "briefcase";
    private boolean raw      = false;

    public JSync(String args[]) {
        this.args = args;
    }

    public void run() {
        // Parse the arguments
        parseArgs();

        ConsoleAppender appender = new ConsoleAppender();
        Log.initLog(appender);
        Log.setLogLevel(logLevel);

        SyncConfig  config  = new SyncConfig();

        config.syncUrl  = url;
        config.userName = username;
        config.password = password;

        config.compress = false;
        SyncManager manager = new SyncManager(config);
        SourceConfig sc = new SourceConfig(SourceConfig.BRIEFCASE, SourceConfig.FILE_OBJECT_TYPE, remoteUri);

        // If a configuration exists, then load it
        try {
            FileAdapter ssConfig = new FileAdapter("briefcaseconfig.dat");
            if (ssConfig.exists()) {
                InputStream is = ssConfig.openInputStream();
                DataInputStream dis = new DataInputStream(is);
                sc.deserialize(dis);
                is.close();
                ssConfig.close();
            }
        } catch (IOException ioe) {
            System.err.println("Cannot load configuration");
        }

        if (!raw) {
            sc.setType(SourceConfig.FILE_OBJECT_TYPE);
            sc.setEncoding(SyncSource.ENCODING_NONE);
        } else {
            sc.setType(SourceConfig.BRIEFCASE_TYPE);
            sc.setEncoding(SyncSource.ENCODING_B64);
        }
        sc.setRemoteUri(remoteUri);

        StringKeyValueFileStore ts = new StringKeyValueFileStore("briefcasecache.txt");
        CacheTracker ct = new CacheTracker(ts);
        sc.setSyncMode(syncMode);
        FileSyncSource fss = new FileSyncSource(sc, ct, "./briefcase/");

        try {
            manager.sync(fss);
            // Save the configuration
            FileAdapter ssConfig = new FileAdapter("briefcaseconfig.dat");
            OutputStream os = ssConfig.openOutputStream();
            DataOutputStream dos = new DataOutputStream(os);
            sc.serialize(dos);
            dos.close();
            ssConfig.close();
        } catch (Exception e) {
            Log.error(e.toString());
        }

    }

    public static void main(String args[]) {
        JSync jsync = new JSync(args);
        jsync.run();
    }

    private void parseLogLevel(String log) {
        if (log.equals("none")) {
            logLevel = Log.DISABLED;
        } else if (log.equals("info")) {
            logLevel = Log.INFO;
        } else if (log.equals("debug")) {
            logLevel = Log.DEBUG;
        } else if (log.equals("trace")) {
            logLevel = Log.TRACE;
        } else {
            System.err.println("Unknown log level " + log);
            usage();
            System.exit(1);
        }
    }

    private void parseSyncMode(String mode) {
        if (mode.equals("fast")) {
            syncMode = SyncML.ALERT_CODE_FAST;
        } else if (mode.equals("slow")) {
            syncMode = SyncML.ALERT_CODE_SLOW;
        } else if (mode.equals("refresh_from_server")) {
            syncMode = SyncML.ALERT_CODE_REFRESH_FROM_SERVER;
        } else if (mode.equals("refresh_from_client")) {
            syncMode = SyncML.ALERT_CODE_REFRESH_FROM_CLIENT;
        } else {
            System.err.println("Unknown sync mode " + mode);
            usage();
            System.exit(1);
        }
    }

    private void parseArgs() {
        int i = 0;
        while(i<args.length) {
            String arg = args[i];
            if (arg.equals("--user")) {
                if (i+1 < args.length) {
                    username = args[++i];
                } else {
                    System.err.println("Missing username");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--pwd")) {
                if (i+1 < args.length) {
                    password = args[++i];
                } else {
                    System.err.println("Missing password");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--url")) {
                if (i+1 < args.length) {
                    url = args[++i];
                } else {
                    System.err.println("Missing url");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--log")) {
                if (i+1 < args.length) {
                    parseLogLevel(args[++i]);
                } else {
                    System.err.println("Missing log level");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--mode")) {
                if (i+1 < args.length) {
                    parseSyncMode(args[++i]);
                } else {
                    System.err.println("Missing sync mode");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--uri")) {
                if (i+1 < args.length) {
                    remoteUri = args[++i];
                } else {
                    System.err.println("Missing remote uri");
                    usage();
                    System.exit(1);
                }
            } else if (arg.equals("--raw")) {
                raw = true;
            } else {
                usage();
                System.exit(1);
            }
            ++i;
        }

        if (username == null || password == null || url == null) {
            usage();
            System.exit(2);
        }
    }

    private void usage() {
        System.out.println("JSync --user <username> --pwd <password> --url <url> " +
                           "[log trace|debug|info|error|none] " +
                           "[--mode fast|slow|refresh_from_client|refresh_from_server] " +
                           "[--uri remote_uri] [--raw]");
    }


}

