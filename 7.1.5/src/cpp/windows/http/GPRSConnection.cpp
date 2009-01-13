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



#include "winsock2.h"


#include <objbase.h>
#include <initguid.h>
#include <connmgr.h>
#include <wininet.h>

#include "http/GPRSConnection.h"
#include "base/Log.h"
#include "base/globalsdef.h"

USE_NAMESPACE



bool EstablishConnection() {

    bool ret = false;
    CONNMGR_CONNECTIONINFO sConInfo = {0};

    //
    // Create mutex for GPRS Connection.
    //
    HANDLE hMutex = CreateMutex(NULL, true, TEXT("FunGPRSConnection"));
    switch (GetLastError()) {
        case ERROR_SUCCESS:
            LOG.debug("GPRS mutex created.");
            break;
        case ERROR_ALREADY_EXISTS:
            LOG.debug("Already testing GPRS connection, exiting.");
            ret = true;
            goto finally;
        default:
            LOG.error("Failed to create GPRS mutex");
            ret = false;
            goto finally;
    }


    sConInfo.cbSize         = sizeof(sConInfo);
        sConInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
        sConInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
        sConInfo.dwFlags=CONNMGR_FLAG_PROXY_HTTP;
        sConInfo.bExclusive = false;
        sConInfo.bDisabled = false;
        sConInfo.guidDestNet = IID_DestNetInternet;
    HANDLE  phWebConnection = NULL;

    // Creates a connection request.
    HRESULT hr = ConnMgrEstablishConnection(&sConInfo, &phWebConnection);

    if (FAILED(hr)) {
        LOG.error("It is impossibile to create an internet connection");
        ret = false;
        goto finally;
                }
                else {
        LOG.debug("Checking internet connection...");                
        DWORD pdwStatus = 0;
        int maxRetry = 10;
        for (int k = 0; k <= maxRetry; k++) {

            // Returns status about the current connection.
                    ConnMgrConnectionStatus(phWebConnection,&pdwStatus);

            switch (pdwStatus) {
                case CONNMGR_STATUS_UNKNOWN:
                case CONNMGR_STATUS_WAITINGCONNECTION:
                    LOG.debug("Attempting to connect...");
                    break;
                case CONNMGR_STATUS_CONNECTED:
                    LOG.debug("Internet connection succesfully completed!");
                    ret = true;
                    goto finally;
                case CONNMGR_STATUS_CONNECTIONCANCELED:
                    LOG.debug("Internet connection canceled.");
                    ret = false;
                    goto finally;
                case CONNMGR_STATUS_WAITINGCONNECTIONABORT:
                    LOG.debug("Internet connection aborted.");
                    ret = false;
                    goto finally;
                case CONNMGR_STATUS_PHONEOFF:
                    LOG.debug("Phone is off, connection aborted.");
                    ret = false;
                    goto finally;
                default:
                    LOG.debug("Unknown connection status (0x%02x)", pdwStatus);
                    break;
            }

            // If connecting, give some time to create the connection.
            Sleep(2000);
        }
    }

finally:
    CloseHandle( hMutex );
    LOG.debug("GPRS mutex released.");
    return ret;
}
