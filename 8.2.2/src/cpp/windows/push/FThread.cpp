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

#include "push/FThread.h"
#include "base/globalsdef.h"

USE_NAMESPACE

FThread::FThread() : isRunning(false) {
}

FThread::~FThread() {
    CloseHandle(thread);
}

void FThread::start( FThread::Priority priority ) {
    
    isRunning = true;

    thread = CreateThread( 
                NULL,                 // default security attributes
                0,                    // use default stack size  
                threadEntryFunction,  // thread function name
                (void*)this,          // argument to thread function 
                0,                    // use default creation flags 
                &threadID);           // returns the thread identifier

    // Set the thread priority
    if(priority != InheritPriority) {
        SetThreadPriority(thread, getWindowsThreadPriority(priority));
    }
}

void FThread::wait() {
    // Wait until the current thread has finished
    WaitForSingleObject(thread, INFINITE);
}

bool FThread::wait(unsigned long timeout) {
    // return true if the thread finishes before the timeout
    return (WaitForSingleObject(thread, timeout) != WAIT_TIMEOUT);
}

void FThread::softTerminate() {
    terminate = true;
}

void FThread::sleep(unsigned long msec) {
    // Sleep the current thread
    Sleep(msec);
}

bool FThread::finished() const {
    return !isRunning;
}

bool FThread::running() const {
    return isRunning;
}

void FThread::setRunning(bool value) {
    isRunning = value;
}

BEGIN_NAMESPACE

DWORD WINAPI threadEntryFunction( LPVOID lpParam ) {
    FThread* threadObj = (FThread*)lpParam;
    threadObj->run();
    threadObj->setRunning(false);
    return NULL;
}

END_NAMESPACE

int FThread::getWindowsThreadPriority(FThread::Priority priority) {
    if(priority == IdlePriority)              return THREAD_PRIORITY_IDLE; 
    else if(priority == LowestPriority)       return THREAD_PRIORITY_LOWEST;
    else if(priority == LowPriority)          return THREAD_PRIORITY_BELOW_NORMAL;
    else if(priority == NormalPriority)       return THREAD_PRIORITY_NORMAL;
    else if(priority == HighPriority)         return THREAD_PRIORITY_ABOVE_NORMAL;
    else if(priority == HighestPriority)      return THREAD_PRIORITY_HIGHEST;
    else if(priority == TimeCriticalPriority) return THREAD_PRIORITY_TIME_CRITICAL;
    else                                      return THREAD_PRIORITY_NORMAL;
}
