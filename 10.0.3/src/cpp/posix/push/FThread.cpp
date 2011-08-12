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

#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "push/FThread.h"
#include "base/fscapi.h"
#include "base/globalsdef.h"

#ifdef ANDROID
#include <errno.h>		
#endif

USE_NAMESPACE

FThread::FThread() : terminate(false),
                     isRunning(false)
{
}

FThread::~FThread() {
}

void FThread::start( FThread::Priority priority ) {
    isRunning = true;
    pthread_create( &pthread, NULL, pthreadEntryFunction, (void*)this);
}

void FThread::wait() {
    pthread_join( pthread, NULL);
}

bool FThread::wait(unsigned long timeout) {
#if defined(POSIX) && !defined(MAC)
    struct timespec t;
    if (clock_gettime(CLOCK_REALTIME, &t) != 0)
    {
        /* Handle error */
        t.tv_sec  = time(NULL);
        t.tv_nsec = 0;
    }

    time_t seconds = timeout / 1000;
    long   nanoseconds = (timeout % 1000) * 1000000;

    t.tv_sec  += seconds;
    t.tv_nsec += nanoseconds;

    // there can not be more than 1000000000 nanoseconds (1 sec) overflow
	if ((unsigned long)t.tv_nsec >= 1000000000) {
		t.tv_nsec -= 1000000000;
		++t.tv_sec;
	}
#endif

#ifdef ANDROID
    int res = pthread_join(pthread, NULL);
    if (res != 0)
    {
    	while (nanosleep(&t, &t) == -1 && EINTR == errno);
		res = pthread_join(pthread, NULL);
    }

#elif defined(MAC)
    int res = pthread_join( pthread, NULL);
#elif defined(POSIX)
    int res = pthread_timedjoin_np(pthread, NULL, &t);
#endif

    return res == 0;
}

bool FThread::finished() const {
    return !isRunning;
}

bool FThread::running() const {
    return isRunning;
}

void FThread::softTerminate() {
    // pthread allows a thread to be killed (pthread_cancel)
    // but this is somehow risky. What should we do?
    terminate = true;
}

void FThread::sleep(long msec) {

    struct timespec t, rt;
    int ret = 0;

    time_t seconds = msec / 1000;
    long   nanoseconds = (msec % 1000) * 1000000;

    t.tv_sec  = seconds;
    t.tv_nsec = nanoseconds;

    do {
        ret = nanosleep(&t, &rt);
        t = rt;
    } while (ret != 0);
}

void FThread::setRunning(bool value) {
    isRunning = value;
}


void *FThread::pthreadEntryFunction(void* fthreadObj) {
    FThread* threadObj = (FThread*)fthreadObj;
    threadObj->run();
    threadObj->setRunning(false);
    pthread_exit(NULL);
    return NULL;
}

