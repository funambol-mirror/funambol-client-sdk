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

#ifndef INCL_SYNCREPORT
#define INCL_SYNCREPORT
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "spds/SyncSource.h"
#include "spds/constants.h"
#include "spds/SyncSourceReport.h"
#include "spds/AbstractSyncConfig.h"


// To notify if status comes from Client or server
#define CLIENT      "Client"
#define SERVER      "Server"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/**
 * The SyncReport class is used to summarize all results of a single synchronization.
 * During the synchronization process, all results about different operations
 * are stored in a SyncReport object, so the client will be able to get these
 * informations at the end.
 * Accessing this object a client can easily know for example the outcome
 * of each source synchronized, retrieve the number of items modified
 * on both sides, and the status code of each one.
 *
 */
class SyncReport {

private:

    // The error code of the last error occurred.
    int   lastErrorCode;

    // The error message of the last error occurred.
    char* lastErrorMsg;

    // Array of report for each SyncSource.
    SyncSourceReport* ssReport;
    unsigned int ssReportCount;


    /*
     * Function to initialize members.
     */
    void initialize();

    /*
     * Assign this object with the given SyncReport
     * @param sr the syncReport object
     */
    void assign(const SyncReport& sr);


public:

    SyncReport();
    SyncReport(SyncReport& sr);
    // Constructor passing a given configuration: setSyncSourceReports() is called.
    SyncReport(AbstractSyncConfig& config);
    virtual ~SyncReport();

    /**
     * Returns the last error code for the whole sync;
     * there are other error codes attached to each
     * sync source.
     */
    int          getLastErrorCode()         const;

    /**
     * the error description corresponding to getLastErrorCode(),
     * might be NULL even if an error occurred
     */
    const char*        getLastErrorMsg()          const;

    /** returns number of sync source reports stored in this report */
    unsigned int getSyncSourceReportCount() const;

    void setLastErrorCode(const int code);
    void setLastErrorMsg (const char* msg);

    /** return pointer to internal SyncSourceReport object given the source name */
    SyncSourceReport* getSyncSourceReport(const char* name)   const;

    /** return pointer to internal SyncSourceReport object given its index (>=0, < getSyncSourceReportCount()) */
    SyncSourceReport* getSyncSourceReport(unsigned int index) const;


    /**
     * Create ssReport array from config.
     * The array is allocated new, will be freed in the desctructor.
     * SyncSourceReports are all owned here by SyncReport, each
     * SyncSource object has a link to its correspondent (external)
     * report, but does not own it.
     * SyncSourceReports are linked during SyncClient::sync().
     *
     * @param config: the SyncManager config to get source number/names.
     */
    void setSyncSourceReports(AbstractSyncConfig& config);

    /**
     * Appends a textual representation of the sync report at the end
     * of the string buffer, without clearing it first.
     *
     * @param str      buffer to which text gets appended
     * @param verbose  if true, then detailed information about each item is
     *                 printed, otherwise only a summary
     */
    void toString(StringBuffer &str, bool verbose = false);

    /**
     * Assign operator
     */
    SyncReport& operator = (const SyncReport& sr) {
        assign(sr);
        return *this;
    }
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

