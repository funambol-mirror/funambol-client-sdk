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
#include "spds/SyncManagerConfig.h"
#include "spds/DefaultConfigFactory.h"
#include "base/globalsdef.h"
#include "mail/MailSyncSourceConfig.h"

USE_NAMESPACE


SyncManagerConfig::SyncManagerConfig() {
    sourceConfigs = NULL;
    sourceConfigsCount = 0;
    abortSyncProcess = false;
}

SyncManagerConfig::~SyncManagerConfig() {
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }
}

SyncSourceConfig* SyncManagerConfig::getSyncSourceConfig(const char* name,
                                                         bool /* refresh */) const {
    if ((name == NULL) || (strlen(name) == 0)) {
        return NULL;
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (strcmp(sourceConfigs[i].getName(), name) == 0) {
            return &sourceConfigs[i];
        }
    }

    return NULL;
}

SyncSourceConfig* SyncManagerConfig::getSyncSourceConfig(unsigned int i,
                                                         bool /* refresh */) const {
    if (i >= sourceConfigsCount) {
        return NULL;
    }

    return &sourceConfigs[i];
}


bool SyncManagerConfig::setSyncSourceConfig(SyncSourceConfig& sc) {
    unsigned int i=0;
    for (i=0; i<sourceConfigsCount; ++i) {
        if (strcmp(sc.getName(), sourceConfigs[i].getName()) == 0) {
            break;
        }
    }

    if (i >= sourceConfigsCount) {
        //
        // Not found! -> add the SyncSourceConfig.
        //
        return addSyncSourceConfig(sc);
    }

    if ( strcmp( sc.getName(), "mail") == 0 ){
        ((MailSyncSourceConfig&)(sourceConfigs[i])).assign(((MailSyncSourceConfig&)(sourceConfigs[i])));
    }else{
        sourceConfigs[i].assign(sc);
    }
    //dirty |= DIRTY_SYNC_SOURCE;

    return true;
}


/*
 * Add the passed SyncSourceConfig (the object is copied).
 * If SyncSourceConfig name is already present (match name) the
 * config is replaced with the given one.
 * Otherwise it is added in the sourceConfig array.
 */
bool SyncManagerConfig::addSyncSourceConfig(SyncSourceConfig& sc) {

    unsigned int i = 0;
    SyncSourceConfig* s = NULL;

    // copy array in a tmp buffer
    if (sourceConfigsCount > 0) {
        s = new SyncSourceConfig[sourceConfigsCount];
        for (i=0; i<sourceConfigsCount; i++) {
            if (strcmp(sourceConfigs[i].getName(), "mail") == 0) {
                ((MailSyncSourceConfig&)s[i]).assign(((MailSyncSourceConfig&)(sourceConfigs[i])));
            } else {
                s[i].assign(sourceConfigs[i]);
            }
        }       
    }

    // delete old one, create new (+1 element)
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }

    sourceConfigs = new SyncSourceConfig[sourceConfigsCount + 1];
    // copy back.
    for (i = 0; i < sourceConfigsCount; i++) {
        if (strcmp(s[i].getName(), "mail") == 0) {
            ((MailSyncSourceConfig &)sourceConfigs[i]).assign(((MailSyncSourceConfig&)(s[i])));
        } else {
            sourceConfigs[i].assign(s[i]);
        }
    }
    
    sourceConfigsCount++;
    if (strcmp(sc.getName(), "mail") == 0) {
        ((MailSyncSourceConfig&)(sourceConfigs[i])).assign(((MailSyncSourceConfig&)(sc)));
    } else {
        sourceConfigs[i].assign(sc);
    }

    if (s) {
        delete [] s;
    }

    return true;
}

void SyncManagerConfig::setClientDefaults() {
    AccessConfig* ac = DefaultConfigFactory::getAccessConfig();
    setAccessConfig(*ac);
    delete ac;

    DeviceConfig* dc = DefaultConfigFactory::getDeviceConfig();
    setClientConfig(*dc);
    delete dc;
	
	DeviceConfig* sc = new DeviceConfig();
	setServerConfig(*sc);
	delete sc;
}

void SyncManagerConfig::setSourceDefaults(const char* name) {
    SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(name);
    setSyncSourceConfig(*sc);
    delete sc;
}
