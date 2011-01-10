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

static HANDLE  phWebConnection = NULL;

bool EstablishConnection() {
    bool ret = false;
    CONNMGR_CONNECTIONINFO sConInfo = {0};   

    sConInfo.cbSize   = sizeof(sConInfo);
    sConInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
    sConInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
    sConInfo.dwFlags = CONNMGR_FLAG_PROXY_HTTP;
    sConInfo.bExclusive = false;
    sConInfo.bDisabled = false;
    sConInfo.guidDestNet = IID_DestNetInternet;
    DWORD pdwStatus = 0;
    DWORD timeout = 20000; //20 sec

    // Creates a connection request.
    HRESULT hr = ConnMgrEstablishConnectionSync(&sConInfo, &phWebConnection, timeout, &pdwStatus);
    
    if (hr == S_OK) {
        LOG.info("EstablishConnection: connected successfully");
        ret = true;
    } else {
        LOG.info("EstablishConnection: error establish connection");
        ret = false;
    }
    switch (pdwStatus) {
        case CONNMGR_STATUS_UNKNOWN:
            LOG.error("The status is unknown.");
            break;
        case CONNMGR_STATUS_CONNECTED:
            LOG.info("The connection is up.");
            break;
        case CONNMGR_STATUS_DISCONNECTED: 	
            LOG.error("The connection has been disconnected.");
            break;
        case CONNMGR_STATUS_WAITINGFORPATH:
            LOG.error("A path to the destination exists but is not presently available (for example, the device is out of radio range or is not plugged into its cradle).");
            break;
        case CONNMGR_STATUS_WAITINGFORRESOURCE: 	
            LOG.error("Another client is using resources that this connection requires.");
            break;
        case CONNMGR_STATUS_WAITINGFORPHONE: 	
            LOG.error("An in-progress voice call is using resources that this connection requires.");
            break;
        case CONNMGR_STATUS_WAITINGFORNETWORK: 	
            LOG.error("The device is waiting for a task with a higher priority to connect to the network before connecting to the same network. This status value is returned only to clients that specify a priority of CONNMGR_PRIORITY_LOWBKGND when requesting a connection.");
            break;
        case CONNMGR_STATUS_NOPATHTODESTINATION: 	
            LOG.error("No path to the destination could be found.");
            break;
        case CONNMGR_STATUS_CONNECTIONFAILED: 	
            LOG.error("The connection failed and cannot be reestablished.");
            break;
        case CONNMGR_STATUS_CONNECTIONCANCELED: 	
            LOG.error("The user aborted the connection.");
            break;
        case CONNMGR_STATUS_CONNECTIONDISABLED: 	
            LOG.error("The connection can be made, although the connection is disabled. This value is returned only to clients that set the bDisabled value in the CONNMGR_CONNECTIONINFO structure.");
            break;
        case CONNMGR_STATUS_WAITINGCONNECTION: 	
            LOG.error("The device is attempting to connect.");
            break;
        case CONNMGR_STATUS_WAITINGCONNECTIONABORT: 	
            LOG.error("The device is aborting the connection attempt.");
            break;
        case CONNMGR_STATUS_WAITINGDISCONNECTION: 	
            LOG.error("The connection is being brought down.");
            break;
    }
    return ret;
}

bool EstablishConnectionOld() {

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


    sConInfo.cbSize   = sizeof(sConInfo);
    sConInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
    sConInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
    sConInfo.dwFlags = CONNMGR_FLAG_PROXY_HTTP;
    sConInfo.bExclusive = false;
    sConInfo.bDisabled = false;
    sConInfo.guidDestNet = IID_DestNetInternet;

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
                    LOG.debug("Internet connection successfully completed!");
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

void DropConnection()
{
    if (phWebConnection)
    {
        if (S_OK == ConnMgrReleaseConnection(phWebConnection,0))
        {
            LOG.debug("Connection dropped");
        }
        else
        {
            LOG.debug("Failed to drop connection");
        }
        phWebConnection = NULL;
    }
}
