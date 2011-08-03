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


#include "spds/SyncReport.h"
#include "base/globalsdef.h"

USE_NAMESPACE

//--------------------------------------------------- Constructor & Destructor

SyncReport::SyncReport() {
    initialize();
}

SyncReport::SyncReport(SyncReport& sr) {
    initialize();
    assign(sr);
}

SyncReport::SyncReport(AbstractSyncConfig& config) {
    initialize();
    setSyncSourceReports(config);
}

SyncReport::~SyncReport() {}




// Create ssReport array from config.
void SyncReport::setSyncSourceReports(AbstractSyncConfig& config) {

    if (ssReport.size() > 0) {
        ssReport.clear();
    }

    // create one report for each configured sync source, even
    // if it is not active: it is set to inactive below and needs
    // to be activated if it is actually part of the sync
    int ssReportCount = config.getAbstractSyncSourceConfigsCount();
    for (int i=0; i<ssReportCount; i++) {
        AbstractSyncSourceConfig* sc = config.getAbstractSyncSourceConfig(i);
        SyncSourceReport ssr(sc->getName());
        ssReport.add(ssr);
    }
}


int SyncReport::addSyncSourceReport(SyncSourceReport& sourceReport) {

    StringBuffer sname = sourceReport.getSourceName();
    if (sname.empty()) {
        LOG.error("SyncSourceReport must contain at least the source name");
        return -1;
    }

    // Remove the source report if already existing
    for (int i=0; i<ssReport.size(); i++) {
        SyncSourceReport* ssr = (SyncSourceReport*)ssReport.get(i);
        if (!ssr) continue;
        if (sname == ssr->getSourceName()) {
            ssReport.removeElementAt(i);
            i--;
        }
    }

    ssReport.add(sourceReport);
    return ssReport.size();
}


int SyncReport::getLastErrorCode() const {
    return lastErrorCode;
}
void SyncReport::setLastErrorCode(const int code) {
    lastErrorCode = code;
}

const char* SyncReport::getLastErrorMsg() const {
    return lastErrorMsg.c_str();
}
void SyncReport::setLastErrorMsg(const char* msg) {
    if (msg) {
        lastErrorMsg = msg;
    } else {
        lastErrorMsg = "";
    }
}

const char* SyncReport::getLastErrorType() const {
    return lastErrorType.c_str();
}
void SyncReport::setLastErrorType(const char* type) {
    if (type) {
        lastErrorType = type;
    } else {
        lastErrorType = "";
    }
}


SyncSourceReport* SyncReport::getSyncSourceReport(const char* name) const {
    if ((name == NULL) || (strlen(name) == 0)) {
        return NULL;
    }
    for (int i=0; i<ssReport.size(); i++) {
        SyncSourceReport* ssr = (SyncSourceReport*)ssReport.get(i);
        if (!ssr) continue;
        if (!strcmp(ssr->getSourceName(), name)) {
            return ssr;
        }
    }
    // if here, source not found
    return NULL;
}

SyncSourceReport* SyncReport::getSyncSourceReport(int index) const {
    if (index < 0 || index >= ssReport.size()) {
        return NULL;
    }
    return (SyncSourceReport*)ssReport.get(index);
}


int SyncReport::getSyncSourceReportCount() const {
    return ssReport.size();
}


//------------------------------------------------------------- Private Methods

void SyncReport::initialize() {
    lastErrorCode  = 0;
    lastErrorMsg   = "";
    lastErrorType  = "";
    ssReport.clear();
}

void SyncReport::toString(StringBuffer &str, bool verbose) {
    StringBuffer tmp;

    str += "===========================================================\n";
    str += "================   SYNCHRONIZATION REPORT   ===============\n";
    str += "===========================================================\n\n";

    if (getLastErrorCode() == 0) {
        str += tmp.sprintf("SYNC COMPLETED SUCCESSFULLY\n");
        str += tmp.sprintf("---------------------------\n\n");
    }
    else {
        str += tmp.sprintf("SYNC COMPLETED WITH ERRORS\n");
        str += tmp.sprintf("--------------------------\n\n");
    }

    str += tmp.sprintf("Last error code = %s%d\n", getLastErrorType(), getLastErrorCode());
    str += tmp.sprintf("Last error msg  = %s\n\n", getLastErrorMsg());

    str += "----------|--------CLIENT---------|--------SERVER---------|\n";
    str += "  Source  |  NEW  |  MOD  |  DEL  |  NEW  |  MOD  |  DEL  |\n";
    str += "----------|-----------------------------------------------|\n";
    for (int i=0; i<ssReport.size(); i++) {
        SyncSourceReport* ssr = getSyncSourceReport(i);
        if (!ssr) continue;

        if (ssr->getState() == SOURCE_INACTIVE) {
            continue;
        }

        str += tmp.sprintf("%10s|", ssr->getSourceName());
        str += tmp.sprintf("%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_ADD),
                                       ssr->getItemReportCount(CLIENT, COMMAND_ADD));
        str += tmp.sprintf("%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_REPLACE),
                                       ssr->getItemReportCount(CLIENT, COMMAND_REPLACE));
        str += tmp.sprintf("%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_DELETE),
                                       ssr->getItemReportCount(CLIENT, COMMAND_DELETE));

        str += tmp.sprintf("%3d/%3d|", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_ADD),
                                       ssr->getItemReportCount(SERVER, COMMAND_ADD));
        str += tmp.sprintf("%3d/%3d|", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_REPLACE),
                                       ssr->getItemReportCount(SERVER, COMMAND_REPLACE));
        str += tmp.sprintf("%3d/%3d|\n", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_DELETE),
                                         ssr->getItemReportCount(SERVER, COMMAND_DELETE));
        str += "----------|-----------------------------------------------|\n";
    }
    str += "\n";

    for (int i=0; i<ssReport.size(); i++) {
        SyncSourceReport* ssr = getSyncSourceReport(i);
        if (!ssr) continue;

        if (ssr->getState() == SOURCE_INACTIVE) {
            continue;
        }

        str += tmp.sprintf("%s:\n----------\n", ssr->getSourceName());
        str += tmp.sprintf("   Source state    = %s\n", ssr->getStateString());
        str += tmp.sprintf("   Last error code = %s%d\n", ssr->getLastErrorType(), ssr->getLastErrorCode());
        str += tmp.sprintf("   Last error msg  = %s\n", ssr->getLastErrorMsg());

        if (ssr->getItemReportCount(SERVER, HTTP_UPLOAD) > 0) {
            str += tmp.sprintf("   HTTP uploaded   = %d/%d\n", ssr->getItemReportSuccessfulCount(SERVER, HTTP_UPLOAD),
                                                               ssr->getItemReportCount(SERVER, HTTP_UPLOAD));
        }
        if (ssr->getItemReportCount(CLIENT, HTTP_DOWNLOAD) > 0) {
            str += tmp.sprintf("   HTTP downloaded = %d/%d\n", ssr->getItemReportSuccessfulCount(CLIENT, HTTP_DOWNLOAD),
                                                               ssr->getItemReportCount(CLIENT, HTTP_DOWNLOAD));
        }

#ifndef SYMBIAN
        if (verbose) {
            for (const char* const* target = SyncSourceReport::targets;
                 *target;
                 target++) {
                for (const char* const* commands = SyncSourceReport::commands;
                     *commands;
                     commands++) {
                    ArrayList* items = ssr->getList(*target, *commands);
                    if (!items)  continue;
                    for (ItemReport *item = (ItemReport*)items->front();
                         item;
                         item = (ItemReport*)items->next()) {
                        str += tmp.sprintf("   %s %s: id '%" WCHAR_PRINTF "' status %d %" WCHAR_PRINTF "\n",
                                           *target, *commands,
                                           item->getId(),
                                           item->getStatus(),
                                           item->getStatusMessage() ? item->getStatusMessage() : TEXT(""));
                    }
                }
            }
        }
#endif
    }
    str += "\n";
}

void SyncReport::assign(const SyncReport& sr) {

    setLastErrorCode(sr.getLastErrorCode());
    setLastErrorMsg (sr.getLastErrorMsg());

    ssReport.clear();
    for (int i=0; i<sr.getSyncSourceReportCount(); i++) {
        SyncSourceReport* ssr = sr.getSyncSourceReport(i);
        if (ssr) ssReport.add(*ssr);
    }
}
