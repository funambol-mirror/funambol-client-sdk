/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

//--README--------------------------------------------------------------
// This file contains the code to set the registry entries used by the
// WindowsMobile test client. It is intended mainly for emulator use.
//
// Remember to adjust the settings to your needs (the server address,
// at least!)
//
// This code is for TEST/DEBUG only, do not use it if you dont't know
// what you are doing.
//---------------------------------------------------------WARNING------

#include "examples/config.h"

#ifdef DEBUG_SETTINGS

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spdm/DMTree.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/ManagementNode.h"

#if 0 //def _WIN32_WCE
static WCHAR *convertSlashes(const WCHAR* str) {
    int i, len;

    len = wcslen(str);
    WCHAR *ret = new WCHAR[len+1];

    for (i=0; i<len; ++i) {
        if (str[i] == L'/') {
            ret[i] = L'\\';
        }
        else {
            ret[i] = str[i];
        }
    }
    ret[len] = 0;
    return ret;
}

/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
static void setProperty(WCHAR* context, WCHAR* property, WCHAR* value) {

    HKEY key;
    DWORD res;

    WCHAR *fullContext = convertSlashes(context);

    RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        goto finally;
    }

    RegSetValueEx(
            key,
            property,
            NULL,
            REG_SZ,  // we currently support only strings
            (UCHAR*)value,
            (wcslen(value)+1)*sizeof(WCHAR*)
            );

finally:

    if (key != 0) {
        RegCloseKey(key);
    }
    delete [] fullContext;
}

#endif  // _WIN32_CE

int settings(const char *rootContext)
{
    char nodeName[DIM_MANAGEMENT_PATH];
    DMTree* dmt = DMTreeFactory::getDMTree(rootContext);
    lastErrorCode = 0;

    // Access config parameters
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SYNCML);

    ManagementNode *node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("deviceId", "");
    node->setPropertyValue("username", "guest");
    node->setPropertyValue("password", "guest");
    node->setPropertyValue("syncUrl", "http://192.168.1.11:8080/funambol/ds");
    node->setPropertyValue("serverName", "http://192.168.1.11:8080");
    node->setPropertyValue("beginTimestamp", "0");
    node->setPropertyValue("endTimestamp", "0");
    node->setPropertyValue("sourceCount", "0");
    node->setPropertyValue("responseTimeout", "0");
    node->setPropertyValue("checkConn", "1");
    node->setPropertyValue("firstTimeSyncMode", "0");
    node->setPropertyValue("proxyHost", "");
    node->setPropertyValue("proxyPort", "");
    node->setPropertyValue("useProxy", "");
    node->setPropertyValue("clientNonce", "cGlwcG8=");
    node->setPropertyValue("serverNonce", "VGB8YHQ6U25lWmAuR3Z3bw==");
    node->setPropertyValue("serverID", "funambol");
    node->setPropertyValue("serverPWD", "funambol");
    node->setPropertyValue("isServerAuthRequired", "F");
    node->setPropertyValue("clientAuthType", "syncml:auth-basic");
    node->setPropertyValue("serverAuthType", "syncml:auth-basic");
    node->setPropertyValue("maxMsgSize", "122880");
    node->setPropertyValue("maxModPerMsg", "20");

    delete node;

    // Contact sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/contacts");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "two-way");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "contact");
    node->setPropertyValue("type", "text/x-s4j-sifc");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri", "scard");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "b64");

    delete node;

    // Appointments sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/appointments");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "none");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "calendar");
    node->setPropertyValue("type", "text/x-s4j-sife");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri",  "scal");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "b64");

    delete node;

    // Tasks sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/tasks");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "none");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "task");
    node->setPropertyValue("type", "text/x-s4j-sift");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri",  "stask");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "b64");

    delete node;

    // Notes sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/notes");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "none");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "note");
    node->setPropertyValue("type", "text/x-s4j-sifn");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri",  "stask");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "b64");

    delete node;

    // Briefcase sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/briefcase");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "two-way");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "briefcase");
    node->setPropertyValue("type", "application/*");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri",  "briefcase");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "b64");

    delete node;

    // Mail sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/mails");

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue("sync", "none");
    node->setPropertyValue("last", "0");
    node->setPropertyValue("name", "mail");
    node->setPropertyValue("type", "application/vnd.omads-email+xml");
    node->setPropertyValue("syncModes", "slow,two-way,refresh");
    node->setPropertyValue("uri", "mail");
    node->setPropertyValue("useSIF", "1");
    node->setPropertyValue("encoding", "text/plain");
    node->setPropertyValue("Inbox", "1" );
    node->setPropertyValue("Outbox", "1" );
    node->setPropertyValue("Trash", "0" );
    node->setPropertyValue("Sent", "1" );
    node->setPropertyValue("Draft", "0" );
    node->setPropertyValue("attachSize", "-1" );
    node->setPropertyValue("bodySyze", "-1" );
    node->setPropertyValue("downloadAge", "-1" );

finally:
    if (node)
        delete node;
    if (dmt)
        delete dmt;

    return lastErrorCode;
}

#endif // DEBUG

