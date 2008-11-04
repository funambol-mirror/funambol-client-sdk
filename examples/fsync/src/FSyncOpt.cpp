/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#include "base/fscapi.h"
#include "FSyncOpt.h"
#include "FSyncConfig.h"

USE_NAMESPACE

FSyncOpt::FSyncOpt(const char *progname) : parser(progname), verbose(NORMAL)
{
    parser.addOption('s', "server", "set server url", true);
    parser.addOption('d', "dir", "set the local folder to sync", true);
    parser.addOption('l', "loglevel", "set log level [error, info, debug]", true);
    parser.addOption('u', "user", "set the user name", true);
    parser.addOption('p', "password", "set the user password", true);
    parser.addOption('v', "verbose", "increase verbosity");
    parser.addOption('q', "quiet", "decrease verbosity");

}

bool FSyncOpt::parseCmdline(int args_num, char** args_val) 
{
    if (parser.parse(args_num, const_cast<const char **>(args_val), opts, args) == false) {
        return false;
    }

    if (optionSet("verbose")) {
        verbose = VERBOSE;
    } else if (optionSet("quiet")) {
        verbose = QUIET;
    }
    FSyncConfig *config = FSyncConfig::getInstance();

    // Get log options
    StringBuffer logLevelName = opts["loglevel"];
    if (!logLevelName.null()) {
        LogLevel logLevel;
        if (logLevelName == "error") {
            logLevel = LOG_LEVEL_NONE;
        } else if (logLevelName == "info") {
            logLevel = LOG_LEVEL_INFO;	
        } else if (logLevelName == "debug") {
            logLevel = LOG_LEVEL_DEBUG;
        } else {
            fprintf(stderr, "%s: unrecognized log level: '%s'\n",
                parser.getProgramName().c_str(), logLevelName.c_str());

            exit(EXIT_FAILURE);
        }
        config->getDeviceConfig().setLogLevel(logLevel);
    }

    // Get server option
    StringBuffer serverUrl = opts["server"];
    if (!serverUrl.null()) {
        config->getAccessConfig().setSyncURL(serverUrl);
    }

    // Get local dir
    StringBuffer dir = opts["dir"];
    if (!dir.null()) {
        config->setSyncPath(dir);
        // TODO: reset anchors if different.
    }

    // Get username
    StringBuffer user = opts["user"];
    if (!user.null()) {
        config->getAccessConfig().setUsername(user);
    }

    // Get password (not secure, it's an example!) 
    StringBuffer pass = opts["password"];
    if (!pass.null()) {
        config->getAccessConfig().setPassword(pass);
    }

    return true;
}

const char* FSyncOpt::getOptionVal(const char *ln)
{ 
    const StringBuffer &optValue = opts.get(ln);

    return optValue.c_str();
}

