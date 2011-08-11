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

#include <CommDbConnPref.h>     // connection prefs (IAP)
#include <apselect.h>           // IAP selection
#include <in_sock.h>
#include <es_enum.h>            // TConnectionInfo

#include "base/FConnection.h"
#include "base/SymbianLog.h"
#include "base/util/stringUtils.h"
#include "base/util/symbianUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE


// Init static pointer.
FConnection* FConnection::iInstance = NULL;


/**
 * Method to create the sole instance of FConnection
 */
FConnection* FConnection::getInstance()
{
    if (iInstance == NULL) {
        iInstance = FConnection::NewL();
    }
    return iInstance;
}

FConnection* FConnection::NewL()
{
    FConnection* self = FConnection::NewLC();
    CleanupStack::Pop( self );

    if (self->getLastError() != KErrNone) {
        // Something wrong.
        delete self;
        return NULL;
    }
    return self;
}

FConnection* FConnection::NewLC()
{
    FConnection* self = new ( ELeave ) FConnection();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

FConnection::FConnection() : iLocalIpAddress("127.0.0.1"),
                             iIAP(0),
                             iIAPName(""),
                             iRetryConnection(0),
                             isConnectionOpened(false), 
                             CActive(CActive::EPriorityStandard),
                             iMainThreadID(0) {
}


FConnection::~FConnection() {
    iConnection.Close();
    iSession.Close();
    delete iASWait;
}

void FConnection::dispose()
{
    if(iInstance) {
        delete iInstance;
    }
    iInstance = NULL;
}


void FConnection::ConstructL()
{
    // Adds this object to the Active Scheduler
    CActiveScheduler::Add(this);
    
    iASWait = new (ELeave) CActiveSchedulerWait();
    
    startSession();
}

void FConnection::setMainThreadID(const TInt mainThreadID) {
    iMainThreadID = mainThreadID;
}

void FConnection::startSession() 
{
    iLastError = KErrNone;
    StringBuffer errMsg;

    // Connect SocketServer session
    LOG.debug("FConnection: connecting the SocketServ session");
    iLastError = iSession.Connect();
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to connect SocketServ (code %d)", iLastError);
        goto error;
    }

    // *Note*: THIS IS MANDATORY TO USE THE SAME SESSION FROM DIFFERENT THREADS!
    // After calling this function the RSocketServ object
    // may be used by threads other than than the one that created it.
    iLastError = iSession.ShareAuto();
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to share SocketServ (code %d)", iLastError);
        goto error;
    }

    // Opens the connection
    openConnection();

    return;

error:
    LOG.error("%s", errMsg.c_str());
}


void FConnection::restartSession() 
{
    // Close the current session (all objects are released)
    closeConnection();
    iSession.Close();
    
    LOG.debug("Restarting HTTP session");
    startSession();
}


const int FConnection::openConnection()
{
    iLastError = KErrNone;

    // We don't want to open a RConnection that is already opened,
    // otherwise there can be errors using the connection.
    if (!isConnectionOpened)
    {
        LOG.debug("FConnection: connection is closed, opening it.");
        // Open connection
        iLastError = iConnection.Open(iSession, KAfInet);
        if (iLastError != KErrNone) {
            LOG.error("FConnection error: unable to open connection (code %d)", iLastError);
            return iLastError;
        }
        isConnectionOpened = true;
        return 0;
    }
    return 1;
}

void FConnection::closeConnection()
{
    LOG.debug("Closing the current connection");
    iConnection.Close();
    isConnectionOpened = false;
    LOG.debug("Connection is closed");
}


const int FConnection::startConnection()
{
    return startConnection(iIAPName);
}

const int FConnection::startConnection(const StringBuffer& aIAPName)
{
    // Safe check
    if (!iMainThreadID) {
        LOG.error("cannot start connection: main thread ID not set");
        return 1;
    }
    
    LOG.info("Starting new connection...");
    LOG.debug("Looking for '%s' IAP name", aIAPName.c_str());

    iLastError = KErrNone;
    StringBuffer errMsg;
    TCommDbConnPref prefs;
    TBuf<50> bearerType;

    prefs.SetDirection(ECommDbConnectionDirectionUnknown);


    if (aIAPName == "Default") {
        //
        // Use the default IAP without prompting the user
        //
        prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
        prefs.SetIapId(0);
    }
    else if (aIAPName.empty() || aIAPName == "Ask") {
        //
        // Prompt user for IAP selection
        //
    }
    else {
        //
        // Search for the desired IAP. If not found, will prompt the user.
        //
        TInt iapID = GetIAPIDFromName(aIAPName);
        if (iapID >= 0) {
            LOG.debug("SetDialogPreference");
            prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            prefs.SetIapId(iapID);
        }
        else {
            LOG.debug("IAP '%s' not found!", aIAPName.c_str());
        }
    }


/*
 * **** TODO: check if we need this! ****
 In S60 3rd Edition, enabling/disabling the inactivity timer will require
 the NetworkControl capability, which is only accessed via Symbian
 partner.
 */
//#if !defined(__SERIES60_3X__) || defined(__S60_3X_NET_CTRL__)
//    // Disable inactivity timer, otherwise inactive connection is closed
//    // within few seconds if there are no activity (e.g sockets)
//    iConnection.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
//#endif


    // Opens the connection
    openConnection();
    
    
    
    RThread thisThread;
    TInt thisThreadID = thisThread.Id();
    if (thisThreadID == iMainThreadID) {
        //
        // It's the main thread: we can use the asynchronous iConnection.Start
        // in order to avoid freezing the UI.
        //
        LOG.debug("setup connection via asynchronous call (active object)");

        if(!IsActive()) { SetActive(); }
        TRAPD(err, iConnection.Start(prefs, iStatus);)
        if (err) {
            LOG.error("Error (%d) starting a new connection", err);
            return iLastError;
        } 
        
        // BLOCKING!! 
        // We must wait until the connection is up, to continue.
        iASWait->Start();
        
        // If here, the iASWait was signaled (see RunL()).
        LOG.debug("%s: signaled! (iStatus = %d)", __FUNCTION__, iStatus.Int());
        
        // Safe check: if not connected anymore, exit!
        // It may happen on very slow devices, the connection setup may take up to 8 seconds
        if (isConnectionOpened == false) {
            LOG.debug("%s: connection was closed: exiting", __FUNCTION__);
            return KErrCancel;
        }
    }
    else {
        //
        // It's NOT the main thread: can't use the asynchronous iConnection.Start
        // because the thread would die using the active object (also the iASWait seems
        // not working between different threads).
        // So we use the synchronous version (blocking!)
        //
        LOG.debug("setup connection via synchronous call (blocking)");
        iStatus = iConnection.Start(prefs);
    }
    
    
    iLastError = iStatus.Int();
    
    if (iStatus != KErrNone && iStatus != KErrAlreadyExists) {
        // 
        // connection error
        //
        if (iStatus == KErrCancel) {
            LOG.debug("connection canceled, exiting");
            return iLastError;
        }
        else if ( iStatus == -30180 ||    // "No WLAN APs matching the IAP settings have been found."
                  iStatus == -30207 ||    // "WLAN connection could not be started because one is already active."
                 (iStatus >= -4159 && iStatus <= - 4153) ) {  // GPRS APN errors (see Symbian OS error codes)
            // no retry!
            StringBuffer msg = resolveErrorCode(iStatus.Int());
            LOG.error("FConnection error, no retry: '%s'", msg.c_str());
            return iLastError;
        }
        else {
            StringBuffer msg = resolveErrorCode(iStatus.Int());
            errMsg.sprintf("FConnection error, retry (%s)", msg.c_str());
            goto retry;
        }
    }
    

    // Save the IAP ID & name of the active connection.
    // Query the CommDb database for the IAP ID in use.
    _LIT(KIAPSettingName, "IAP\\Id");
    iConnection.GetIntSetting(KIAPSettingName, iIAP);
    iIAPName = GetIAPNameFromID(iIAP);
    LOG.debug("Current active IAP ID = %d, name = '%s'", iIAP, iIAPName.c_str());
    //
    // TODO: should we persist the iIAPName in the config, here?
    //
    
    //TUint32 bearer = 0;
    //_LIT(KIapBearer, "IAP\\IAPBearer");
    //iConnection.GetIntSetting(KIapBearer, bearer);
    _LIT(KIapBearerType, "IAP\\IAPBearerType");
    iConnection.GetDesSetting(KIapBearerType, bearerType);
    LOG.debug("Connection type = %s", bufToStringBuffer(bearerType).c_str());
  
    iRetryConnection = 0;
    return 0;


retry:

    LOG.error("%s", errMsg.c_str());

    if (iIAPName.empty() || iIAPName == "Ask") {
        LOG.debug("Connection error and no IAP stored: don't retry");
        return iLastError;
    }

    LOG.debug("About to retry...");
    if (iRetryConnection < MAX_RETRY_CONNECTION) {
        iRetryConnection ++;
        LOG.info("Retry connection (%d time)...", iRetryConnection);
        startConnection(iIAPName);
    }
    else {
        LOG.error("FConnection: %d connection failed", iRetryConnection);
    }
    return iLastError;
}

const bool FConnection::isConnected()
{
    //LOG.debug("FConnection::isConnected?");
    iLastError = KErrNone;
    bool connected = false;
    TUint count;

    // Connection must always be opened before enumerating the active
    // connections, otherwise following function will crash.
    openConnection();

    //Enumerate currently active connections
    TRAP(iLastError, iConnection.EnumerateConnections(count));
    if (iLastError != KErrNone) {
        LOG.error("Error checking active connections (code %d)", iLastError);
        return false;
    }

    //LOG.debug("Number of active connections = %d", count);
    if (count) {
        TPckgBuf<TConnectionInfoV2> connectionInfo;
        for (TUint i = 1; i <= count; ++i) {
            iConnection.GetConnectionInfo(i, connectionInfo);
            if (connectionInfo().iIapId == iIAP) {
                connected = true;
                break;
            }
        }
    }
    return connected;
}

const int FConnection::stopConnection()
{
	LOG.error("*** WARNING! stopConnection not supported! Need NetworkControl capability. ***");
	return KErrNotSupported;

    //iLastError = KErrNone;
    //if (isConnected()) {
    //    LOG.debug("Stopping the current connection");
    //    iLastError = iConnection.Stop(/*RConnection::EStopAuthoritative*/);
    //}
    //else {
    //    LOG.debug("No need to stop connection (not connected)");
    //}
    //return iLastError;
}



const StringBuffer& FConnection::getLocalIpAddress()
{
    //
    // TODO: http://wiki.forum.nokia.com/index.php/LocalDeviceIpAddress
    //
    return iLocalIpAddress;
}


TInt FConnection::GetIAPIDFromName(const StringBuffer& aIAPName)
{
    TInt ret = -1;
    RBuf iapName;
    iapName.Assign(stringBufferToNewBuf(aIAPName));

    CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);

    TBool ok = select->MoveToFirst();
    for (TUint32 i=0; ok &&(i<select->Count()); i++)
    {
        StringBuffer tmp = bufToStringBuffer(select->Name());
        //LOG.debug("Found IAP: %s (id = %d)", tmp.c_str(), select->Uid());
        if (select->Name() == iapName) {
            ret = select->Uid();
            //LOG.debug("Found IAP: %s (id = %d)", aIAPName.c_str(), ret);
            break;
        }
        else {
            ok = select->MoveNext();
        }
    }
    //LOG.debug("IAP Found");
    CleanupStack::PopAndDestroy(2);    //commdb and select

    return ret;
}


StringBuffer FConnection::GetIAPNameFromID(const TUint aIAPID)
{
    StringBuffer ret = "";

    LOG.debug("GetIAPNameFromID");
    CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);

    TBool ok = select->MoveToFirst();
    for (TUint32 i=0; ok &&(i<select->Count()); i++)
    {
        StringBuffer tmp = bufToStringBuffer(select->Name());
        //LOG.debug("Name=%s -- %d", tmp.c_str(), select->Uid());
        if (select->Uid() == aIAPID) {
            ret = bufToStringBuffer(select->Name());
            //LOG.debug("Found IAP: %d (name = %s)", aIAPID, ret.c_str());
            break;
        }
        else {
            ok = select->MoveNext();
        }
    }
    CleanupStack::PopAndDestroy(2);    //commdb and select

    return ret;
}


RArray<HBufC*> FConnection::GetAllIAPNames()
{
    RArray<HBufC*> names;
    names.Reset();

    CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);

    TBool ok = select->MoveToFirst();
    for (TUint32 i=0; ok &&(i<select->Count()); i++) {
        HBufC* currentName = select->Name().Alloc();
        names.Append(currentName);
        ok = select->MoveNext();
    }

    CleanupStack::PopAndDestroy(2);    //commdb and select
    return names;
}



const TApBearerType FConnection::getConnectionType(const StringBuffer aIAPName)
{
    StringBuffer name = aIAPName;
    if (name.empty()) {
        name = iIAPName;
    }
    if (name.empty()) {
        return EApBearerTypeAllBearers;
    }
    
    RBuf iapName;
    iapName.Assign(stringBufferToNewBuf(name));
    TApBearerType type = EApBearerTypeAllBearers;

    CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);

    TBool ok = select->MoveToFirst();
    for (TUint32 i=0; ok &&(i<select->Count()); i++)
    {
        if (select->Name() == iapName) {
            type = select->BearerType();
            //LOG.debug("Found IAP: %s (type = 0x%x)", name.c_str(), type);
            break;
        }
        else {
            ok = select->MoveNext();
        }
    }

    CleanupStack::PopAndDestroy(2);    //commdb and select
    return type;
}

const bool FConnection::isWLANConnection(const StringBuffer aIAPName) {
    
    TApBearerType type = getConnectionType(aIAPName);
    if (type & EApBearerTypeWLAN) {
        return true;
    }
    return false;
}


void FConnection::RunL(void) 
{
    //LOG.debug("FConnection::RunL - iStatus = %d", iStatus.Int());
    
    // OK the connection is up -> continue the sync!
    // This call signals the "iASWait->Start()" inside startConnection().
    iASWait->AsyncStop();
}


void FConnection::DoCancel(void) 
{
    // nothing to do 
    LOG.debug("FConnection DoCancel()");
}


TInt FConnection::RunError(TInt aError)
{
    LOG.error("FConnection RunL() error: %d", aError);
    return 0;
}


