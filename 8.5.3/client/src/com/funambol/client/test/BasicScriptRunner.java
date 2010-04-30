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

package com.funambol.client.test;

import java.io.InputStream;
import java.io.IOException;

import com.funambol.util.Log;
import com.funambol.util.CodedException;
import com.funambol.util.HttpTransportAgent;
import com.funambol.platform.FileAdapter;

import java.util.Vector;

public class BasicScriptRunner extends CommandRunner {
    
    private static final String TAG_LOG = "BasicScriptRunner";

    // Commands
    private static final String INCLUDE_COMMAND = "Include";

    private String baseUrl = null;

    // The list of command runners
    private Vector commandRunners = new Vector();

    public BasicScriptRunner() {
        super(null);
    }

    public void addCommandRunner(CommandRunner runner) {
        commandRunners.addElement(runner);
    }

    public void runScriptFile(String scriptUrl, boolean mainScript) throws Throwable {

        int lineNumber = 0;
        String script = null;

        if(baseUrl == null) {
            baseUrl = getBaseUrl(scriptUrl);
        }

        Log.info(TAG_LOG, "Running script at URL = " + scriptUrl);
        if (scriptUrl != null) {
            try {
                if (scriptUrl.startsWith("http")) {
                    script = getScriptViaHttp(scriptUrl);
                } else if (scriptUrl.startsWith("file")) {
                    script = getScriptViaFile(scriptUrl);
                } else {
                    Log.error(TAG_LOG, "Unknwon protocol to fetch script " + scriptUrl);
                    throw new IllegalArgumentException("Cannot fetch script");
                }
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot load script at " + scriptUrl + " because " + e.toString());
                throw new Exception("Cannot load script " + scriptUrl + " because " + e.toString());
            }
        }

        // Execute the script by interpreting it
        try {
            int idx = 0;
            int nextLine = -1;
            do {
                nextLine = script.indexOf('\n', idx);
                String line;
                if (nextLine != -1) {
                    line = script.substring(idx, nextLine);
                    idx = nextLine + 1;
                } else {
                    line = script.substring(idx);
                }
                lineNumber++;

                line = line.trim();
                if (line.length() > 0 && !line.startsWith("#")) {
                    int parPos = line.indexOf('(');
                    if (parPos == -1) {
                        Log.error(TAG_LOG, "Syntax error in script, missing '(' in: "
                                + line + " at line " + lineNumber);
                        return;
                    }
                    
                    String command = line.substring(0, parPos);
                    command = command.trim();
                    String pars = line.substring(parPos);

                    runCommand(command, pars);
                }
            } while (nextLine != -1);
        } catch (Throwable t) {
            Log.error(TAG_LOG, "Script " + scriptUrl + " interrupted at line: " + lineNumber);
            throw t;
        }
    }

    public boolean runCommand(String command, String pars) throws Throwable {

        Log.trace(TAG_LOG, "command=" + command);
        Log.trace(TAG_LOG, "pars=" + pars);

        if (INCLUDE_COMMAND.equals(command)) {
            includeScript(command, pars);
        } else {
            boolean ok = false;
            for(int i=0; i<commandRunners.size(); i++) {
                CommandRunner runner = (CommandRunner)commandRunners.elementAt(i);
                if(runner.runCommand(command, pars)) {
                    ok = true;
                    break;
                }
            }
            if(!ok) {
                throw new IllegalArgumentException("Unknown command " + command);
            }
        }
        return true;
    }

    private void includeScript(String command, String args) throws Throwable {
        String scriptUrl = getParameter(args, 0);

        checkArgument(scriptUrl, "Missing script url in " + command);

        if (!scriptUrl.startsWith("http") && !scriptUrl.startsWith("file")) {
            if (baseUrl != null) {
                scriptUrl = baseUrl + "/" + scriptUrl;
            }
        }
        runScriptFile(scriptUrl, false);
    }

    protected String getBaseUrl(String scriptUrl) {
        int pos = scriptUrl.indexOf('/');
        int lastPos = pos;

        while(pos != -1) {
            lastPos = pos;
            pos = scriptUrl.indexOf('/', pos + 1);
        }

        if (lastPos != -1) {
            return scriptUrl.substring(0, lastPos);
        } else {
            return null;
        }
    }

    public void setSyncMonitor(SyncMonitor monitor) {
        super.setSyncMonitor(monitor);
        for(int i=0; i<commandRunners.size(); i++) {
            CommandRunner runner = (CommandRunner)commandRunners.elementAt(i);
            runner.setSyncMonitor(monitor);
        }
    }


    public void setAuthSyncMonitor(SyncMonitor monitor) {
        super.setAuthSyncMonitor(monitor);
        for(int i=0; i<commandRunners.size(); i++) {
            CommandRunner runner = (CommandRunner)commandRunners.elementAt(i);
            runner.setAuthSyncMonitor(monitor);
        }
    }

    public void setCheckSyncClient(CheckSyncClient client) {
        super.setCheckSyncClient(client);
        for(int i=0; i<commandRunners.size(); i++) {
            CommandRunner runner = (CommandRunner)commandRunners.elementAt(i);
            runner.setCheckSyncClient(client);
        }
    }

    private String getScriptViaHttp(String url) throws CodedException {
        HttpTransportAgent ta = new HttpTransportAgent(url, null, "UTF-8", false, false);
        ta.setRequestContentType(ta.getRequestContentType() + ";charset=utf-8");
        String response = ta.sendMessage("");
        return response;
    }

    private String getScriptViaFile(String url) throws IOException {
        FileAdapter fa = new FileAdapter(url, true);
        int size = (int)fa.getSize();
        byte data[] = new byte[size];
        InputStream is = fa.openInputStream();
        is.read(data);
        is.close();
        fa.close();
        return new String(data);
    }
}

