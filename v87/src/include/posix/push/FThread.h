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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#ifndef INCL_FTHREAD
#define INCL_FTHREAD
#include "base/globalsdef.h"

#include <pthread.h>

BEGIN_NAMESPACE

class FThread {

protected:
    FThread();
    bool terminate;

public:
    virtual ~FThread();

    enum Priority { IdlePriority, LowestPriority, LowPriority,
                    NormalPriority, HighPriority, HighestPriority,
                    TimeCriticalPriority, InheritPriority };


    /**
     Starts this thread with the given priority. Threads entry point is
     the run method.
    */
    virtual void start( Priority priority = InheritPriority );

    /**
     Wait for this thread to finish its execution. This method is used to
     synchronize threads execution (similar to pthread_join)
    */
    virtual void wait();

    /**
     Wait for this thread to finish its execution or the timeout expires.
     If the thread terminates its execution before the timeout expires then 
     true is returned, false otherwise.
	 @param timeout Timeout in milliseconds
	 @return true if the thread is stopped, false otherwise
    */
    virtual bool wait(unsigned long timeout);

    /**
     Returns true if the thread execution is terminated
    */
    virtual bool finished() const;

    /**
     Returns true iff the thread is still running
    */
    virtual bool running() const;

    /**
     Ask this thread to terminate its execution. But the thread is not
     forced to terminate. The thread must cooperate to terminate.
    */    
    virtual void softTerminate();

protected:
    virtual void run() = 0;

public:
    /**
     * Suspend the current thread for the given time (milliseconds)
     */
    static void sleep(long msec);

    static void* pthreadEntryFunction(void* fthreadObj);


private:
    bool isRunning;
    pthread_t pthread;

private:
    void setRunning(bool value);
};


END_NAMESPACE

#endif
