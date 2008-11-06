/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

#ifndef INCL_SYNCSOURCEREPORT
#define INCL_SYNCSOURCEREPORT
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "spds/constants.h"
#include "event/constants.h"
#include "spds/ItemReport.h"


/** Possible states of syncsource (state member) */
typedef enum SourceState{
    SOURCE_ACTIVE       = 0,        /**< source is part of the current sync and in a sane state */
    SOURCE_INACTIVE     = 1,        /**< source was excluded from the current sync before starting it */
    SOURCE_ERROR        = 2,        /**< source encountered and error sometime during the sync */
} SourceState;


/**
 * SyncSourceReport class rapresents the report of each
 * SyncSource synchronized. It is used to store the sync results
 * for a specific syncsource, its state (active/inactive/error) and
 * the status of each item synchronized (both on client and server side).
 */
class SyncSourceReport {

private:

    // The error code of the last error occurred for this source.
    int         lastErrorCode;

    // The error message of the last error occurred for this source.
    char*       lastErrorMsg;

    // The source name.
    char*       sourceName;

    // The source state, possible values:
    // 0 = SOURCE_ACTIVE   -> used in synchronization
    // 1 = SOURCE_INACTIVE -> ignored in synchronization
    // 2 = SOURCE_ERROR    -> error occurred, will be skipped in sync
    SourceState state;

    // List of ItemReports for client modifications
    ArrayList*  clientAddItems;
    ArrayList*  clientModItems;
    ArrayList*  clientDelItems;

    // List of ItemReports for server modifications
    ArrayList*  serverAddItems;
    ArrayList*  serverModItems;
    ArrayList*  serverDelItems;


    // Return true if status is [200 <-> 299] (successful)
    bool isSuccessful(const int status);

    // Function to initialize members.
    void initialize();

    /*
     * Assign this object with the given SyncReport
     * @param sr the syncReport object
     */
    void assign(const SyncSourceReport& ssr);


public:

    SyncSourceReport(const char* name = NULL);
    SyncSourceReport(SyncSourceReport& ssr);
    virtual ~SyncSourceReport();

    /** source specific error code, see also SyncReport::getLastErrorCode() */
    const int   getLastErrorCode() const;

    /** the current state of the source, see SourceState */
    const SourceState   getState() const;

    /** source specific error message, see also SyncReport::getLastErrorMsg() */
    const char*  getLastErrorMsg() const;

    /** the unique name of the source that this report is about */
    const char*    getSourceName() const;

    void setLastErrorCode(const int code);
    void setState        (const SourceState s);
    void setLastErrorMsg (const char* msg);
    void setSourceName   (const char* name);

    /**
     * Check the state of this source.
     * Returns true if source is active (state = SOURCE_ACTIVE).
     */
    bool checkState();


    /**
     * Get internal pointer to a specific ItemReport.
     *
     * @param target      to select if client/server side          (values = CLIENT - SERVER)
     * @param command     to select the desired list of ItemReport (values = Add - Replace - Delete)
     * @param index       the index of desired item inside the list
     *
     */
    ItemReport* getItemReport(const char* target, const char* command, int index);


    /**
     * Return the total number of ItemReport for a specific list.
     *
     * @param target     to select if client/server side          (values = CLIENT - SERVER)
     * @param command    to select the desired list of ItemReport (values = Add - Replace - Delete)
     *
     */
    int getItemReportCount            (const char* target, const char* command);
    // Only for successful reports.
    int getItemReportSuccessfulCount  (const char* target, const char* command);
    // Only for failed reports.
    int getItemReportFailedCount      (const char* target, const char* command);
    // Only for code = 418 (ALREADY_EXIST)
    int getItemReportAlreadyExistCount(const char* target, const char* command);


    /**
     * Used to add an ItemReport to a specific list.
     * This function is called inside API to store the status of each item
     * added/modified/deleted on client and on server.
     *
     * @param target          to select if client/server side          (values = CLIENT - SERVER)
     * @param command         to select the desired list of ItemReport (values = Add - Replace - Delete)
     * @param ID              the LUID of item                         (used to create the ItemReport element)
     * @param status          the status code of the operation         (used to create the ItemReport element)
     * @param statusMessage   the status message associated to the operation         (used to create the ItemReport element)
     *
     */
    void addItem(const char* target, const char* command, const WCHAR* ID, const int status, const WCHAR* statusMessage);



    /**
     * Utility to switch on the right list, based on target and command.
     *
     * @param target     to select if client/server side          (values = CLIENT - SERVER)
     * @param command    to select the desired list of ItemReport (values = Add - Replace - Delete)
     * @return           a pointer to the desired ArrayList
     *
     */
    ArrayList* getList(const char* target, const char* command) const;

    /**
     * all valid strings for "target", NULL terminated
     */
    static const char* const targets[];

    /**
     * all valid strings for "command", NULL terminated
     */
    static const char* const commands[];

    /**
     * Assign operator
     */
    SyncSourceReport& operator = (const SyncSourceReport& ssr) {
        assign(ssr);
        return *this;
    }
};

/** @} */
/** @endcond */
#endif

