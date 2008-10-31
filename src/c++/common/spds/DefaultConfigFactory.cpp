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

#include "base/fscapi.h"
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "spds/DefaultConfigFactory.h"
#include "base/globalsdef.h"

USE_NAMESPACE


DefaultConfigFactory::DefaultConfigFactory() {
}

DefaultConfigFactory::~DefaultConfigFactory() {
}


AccessConfig* DefaultConfigFactory::getAccessConfig() {

    AccessConfig* ac = new AccessConfig();

    ac->setUsername             ("guest");
    ac->setPassword             ("guest");
    ac->setFirstTimeSyncMode    (SYNC_NONE);
    ac->setUseProxy             (false);
    ac->setProxyHost            ("");
    ac->setProxyPort            (8080);
    ac->setProxyUsername        ("");
    ac->setProxyPassword        ("");
    ac->setSyncURL              ("http://localhost:8080/funambol/ds");
    ac->setBeginSync            (0);
    ac->setEndSync              (0);
    ac->setServerAuthRequired   (false);
    ac->setClientAuthType       ("syncml:auth-basic");
    ac->setServerAuthType       ("syncml:auth-basic");
    ac->setServerPWD            ("funambol");
    ac->setServerID             ("funambol");
    ac->setServerNonce          ("");
    ac->setClientNonce          ("");
    ac->setMaxMsgSize           (10000);
    ac->setReadBufferSize       (0);
    ac->setUserAgent            ("");
    ac->setCheckConn            (true);
    ac->setResponseTimeout      (0);
    //ac->setEncryption           (false);

    return ac;
}



DeviceConfig* DefaultConfigFactory::getDeviceConfig() {

    DeviceConfig* dc = new DeviceConfig();

    dc->setMan                  ("");
    dc->setMod                  ("");
    dc->setOem                  ("");
    dc->setFwv                  ("");
    dc->setSwv                  ("");
    dc->setHwv                  ("");
    dc->setDevID                ("funambol-client");
    dc->setDevType              ("workstation");
    dc->setDsV                  ("");
    dc->setUtc                  (true);
    dc->setLoSupport            (false);
    dc->setNocSupport           (false);
    dc->setLogLevel             (LOG_LEVEL_INFO);
    dc->setMaxObjSize           (0);
    dc->setDevInfHash           ("");

    return dc;
}



SyncSourceConfig* DefaultConfigFactory::getSyncSourceConfig(const char* name) {

    SyncSourceConfig* sc = new SyncSourceConfig();

    sc->setName                 (name);
    sc->setSyncModes            ("slow,two-way");
    sc->setSync                 ("two-way");
    sc->setEncoding             ("b64");
    sc->setLast                 (0);
    sc->setSupportedTypes       ("");
    sc->setVersion              ("");
    sc->setEncryption           ("");

    if (!strcmp(name, "contact")){
        sc->setURI              ("scard");
        sc->setType             ("text/x-s4j-sifc");
    }
    else if (!strcmp(name, "calendar")){
        sc->setURI              ("scal");
        sc->setType             ("text/x-s4j-sife");
    }
    else if (!strcmp(name, "task")){
        sc->setURI              ("stask");
        sc->setType             ("text/x-s4j-sift");
    }
    else if (!strcmp(name, "note")){
        sc->setURI              ("snote");
        sc->setType             ("text/x-s4j-sifn");
    }

    // *** TBD ***
    //sc->setCtCap

    return sc;
}

