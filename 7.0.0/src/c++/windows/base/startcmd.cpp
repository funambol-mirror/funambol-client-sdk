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
 */


// Only used by WinMobile
#ifdef _WIN32_WCE

#include <windows.h>
#include <Winbase.h>
#include "base/fscapi.h"
#include "base/startcmd.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

// retrieve the Funambol program path and set it in a static
// buffer. Return the buffer
const WCHAR *getProgramPath()
{
    static WCHAR path[MAX_PATH] = TEXT("");

    if (!path[0]) {
        SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES , false);
        wcscat(path, TEXT("\\"));
        wcscat(path, PROGRAM_DIR);
    }
    return path;
}

/**
 * Start a command in a new process and return the pid
 * or 0 in case of error.
 */
unsigned long startcmd(const WCHAR *app, const WCHAR *cmdline)
{
    const WCHAR *path = getProgramPath();
    PROCESS_INFORMATION procinfo;

    WCHAR *cmd = new WCHAR[wcslen(path)+wcslen(app)+5];
    wsprintf(cmd, TEXT("%s\\%s"), path, app);

    char dbg[200];
    sprintf(dbg, "Running: %ls %ls\n", cmd, cmdline);
    LOG.info(dbg);
    if( CreateProcess( cmd, cmdline,
                       NULL, NULL, false, 0,
                       NULL, NULL, NULL, &procinfo ) ) {
        return procinfo.dwProcessId;
    }
    else
        return 0;
}

/*
 * Return 0: process terminated successfully
 *        >0: error code from child process
 *       -1: no such process/invalid handle
 *       -2: timeout
 *       -3: can't get child exit code
 */
int waitProcess(unsigned long pid, time_t timeout)
{
    HANDLE phandle = OpenProcess( 0, false, pid );

    if (phandle) {
        switch ( WaitForSingleObject( phandle, timeout ) ) {
            case WAIT_TIMEOUT:
                return -2;
                break;
            case WAIT_OBJECT_0:
                DWORD exitcode;
                if ( GetExitCodeProcess(phandle, &exitcode) )
                    return exitcode;
                else
                    return -3;
                break;
            default:
                return -1;
        }
    }
    return -1;
}

#endif   // #ifdef _WIN32_WCE
