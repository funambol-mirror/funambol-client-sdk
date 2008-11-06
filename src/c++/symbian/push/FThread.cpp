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

#include <e32std.h>


#include "base/fscapi.h"
#include "push/FThread.h"
#include "base/SymbianLog.h"
#include "base/globalsdef.h"


USE_NAMESPACE

// Initialize statically the thread ID.
uint32_t FThread::id = 0;

FThread::FThread() : terminate(false),
                     isRunning(false)
{
}

FThread::~FThread() {
}

typedef void (FThread::*ThreadFunction)(void);

void FThread::start( FThread::Priority priority ) {

    RBuf threadId;
    threadId.Create(30);
    threadId.Format(_L("FThread-%d"), id++);
    
    TRAPD(err, sthread.Create(threadId, (TThreadFunction)symbianRunWrapper,
                              KDefaultStackSize, (RAllocator*)&User::Heap(), this));
    if (err == KErrNone) {
        //LOG.debug("Created thread (id = %d)", id-1);
        isRunning = true;
        if (priority == InheritPriority) {
            // TODO: how do we get the current thread priority?
            priority = NormalPriority;
        }
        sthread.SetPriority((TThreadPriority)priority);
        sthread.Resume();
    }
    threadId.Close();
}

void FThread::wait() {
    TRequestStatus stat;
    if (sthread.Handle() == NULL) {
        return;
    }
    sthread.Logon(stat);
    if (sthread.ExitType() == EExitPending || stat != KRequestPending) {
        User::WaitForRequest(stat);
    }
}

TRequestStatus timerStat;

bool FThread::wait(unsigned long timeout) {
    TRequestStatus stat;
    sthread.Logon(stat);

    RThread tthread;
    TBuf<30> tthreadId;
    tthreadId.Format(_L("FThread-timer-%d"), id);
    // Start the timer thread
    if (timeout) {
        this->timeout = timeout;
        TRAPD(err, tthread.Create(tthreadId, (TThreadFunction)symbianTimeoutWrapper,
                                  KDefaultStackSize, (RAllocator*)NULL, this));
        if (err == KErrNone) {
            tthread.Resume();
        }
        User::WaitForRequest(stat, timerStat);
    }
     
    if (this->timeout || timeout == 0) {
        if (timeout) {
            timer.Cancel();
        }
        return true;
    } else {
        return false;
    }
}

bool FThread::finished() const {
    return !isRunning;
}

bool FThread::running() const {
    return isRunning;
}

void FThread::softTerminate() {
    terminate = true;
    // Kill the thread
    sthread.Kill(0);
    // At this point the thread is no longer running
    isRunning = false;
}

void FThread::sleep(long msec) {
    TTimeIntervalMicroSeconds32 interval(msec * 1000);
    User::After(interval);
}

TInt FThread::startTimeout() {
    TRequestStatus status;
    timer.CreateLocal();
    timer.After(status, (TTimeIntervalMicroSeconds32)timeout);
    User::WaitForRequest(status);
    // If the thread is still running we must kill it
    if (isRunning) {
        // This will unlock the calling thread
#if 0
        User::RequestComplete(timerStat);
#endif
    }
    timeout = 0;
    return 0;
}

BEGIN_NAMESPACE

TInt symbianRunWrapper(TAny* thread) {
    
    // Install a new trap handler for the thread.
    CTrapCleanup* cleanupstack = CTrapCleanup::New();

    // Install a new active scheduler
    TInt err;
#if 0
    CActiveScheduler* activeScheduler = NULL;
    TRAP(err,
        // Add support for active objects (in case the thread uses any)
        activeScheduler = new (ELeave) CActiveScheduler;
        //CleanupStack::PushL(activeScheduler);
        CActiveScheduler::Install(activeScheduler);
    );

    if (err != KErrNone) {
        LOG.error("Cannot create FThread active object");
        return err;
    }
#endif

    // Mandatory!
    // To trap any internal call to "CleanupStack::PushL()"

    FThread* t = (FThread*)thread;
    TRAP(err, 
         {
           t->run();
         }
    )
    t->isRunning = false;

    // Can this be moved into the destructor?
    if (t->sthread.Handle() != NULL) {
        t->sthread.Close();
    }
   
    //delete activeScheduler;
    delete cleanupstack;

    // Terminate the current thread
    User::Exit(err);
    return err;
}

TInt symbianTimeoutWrapper(TAny* thread) 
{
    // Install a new trap handler for the thread.
    CTrapCleanup* cleanupstack = CTrapCleanup::New();

    FThread* t = (FThread*)thread;
    TRAPD(err, 
        {
            t->startTimeout();
            if (t->sthread.Handle() != NULL) {
                t->sthread.Close();
            }
        }
    )
    t->isRunning = false;

    delete cleanupstack;

    // Terminate the current thread
    User::Exit(0);

    return 0;
}

END_NAMESPACE

