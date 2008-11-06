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

#include "base/fscapi.h"
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "spds/DefaultConfigFactory.h"


DefaultConfigFactory::DefaultConfigFactory() {
}

DefaultConfigFactory::~DefaultConfigFactory() {
}


AccessConfig* DefaultConfigFactory::getAccessConfig() {

    AccessConfig* ac = new AccessConfig();

    ac->setUsername             ("guest");
    ac->setPassword             ("guest");
    ac->setFirstTimeSyncMode    (SYNC_NONE);
    ac->setUseProxy             (FALSE);
    ac->setProxyHost            ("");
    ac->setProxyPort            (8080);
    ac->setProxyUsername        ("");
    ac->setProxyPassword        ("");
    ac->setSyncURL              ("http://localhost:8080/funambol/ds");
    ac->setBeginSync            (0);
    ac->setEndSync              (0);
    ac->setServerAuthRequired   (FALSE);
    ac->setClientAuthType       ("syncml:auth-basic");
    ac->setServerAuthType       ("syncml:auth-basic");
    ac->setServerPWD            ("funambol");
    ac->setServerID             ("funambol");
    ac->setServerNonce          ("");
    ac->setClientNonce          ("");
    ac->setMaxMsgSize           (10000);
    ac->setReadBufferSize       (0);
    ac->setUserAgent            ("");
    ac->setCheckConn            (TRUE);
    ac->setResponseTimeout      (0);
    //ac->setEncryption           (FALSE);

    return ac;
}



DeviceConfig* DefaultConfigFactory::getDeviceConfig() {

    DeviceConfig* dc = new DeviceConfig();

    dc->setVerDTD               ("1.1");
    dc->setMan                  ("");
    dc->setMod                  ("");
    dc->setOem                  ("");
    dc->setFwv                  ("");
    dc->setSwv                  ("");
    dc->setHwv                  ("");
    dc->setDevID                ("funambol-client");
    dc->setDevType              ("workstation");
    dc->setDsV                  ("");
    dc->setUtc                  (TRUE);
    dc->setLoSupport            (FALSE);
    dc->setNocSupport           (FALSE);
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

