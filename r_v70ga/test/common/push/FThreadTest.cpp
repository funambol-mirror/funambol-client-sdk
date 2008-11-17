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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "push/FThread.h"

#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"

USE_NAMESPACE

class MyThread1 : public FThread {
public:
    MyThread1() : tot(0) {}

    void run() {
        for(int i=0;i<10;++i) {
            tot += i;
        }
    }

    int getResult() {
        return tot;
    }

private:
    int tot;
};

class MyThread2 : public FThread {
public:
    MyThread2() {}

    void run() {
        // Wait 5 seconds
        sleep(5000);
    }
};

class MyThread3 : public FThread {
public:
    MyThread3() {}

    void run() {
        MyThread1 myThread;
        myThread.start();
        myThread.wait();
    }
};

/**
 * Test case for the class StringBuffer.
 */
class FThreadTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FThreadTest);
    CPPUNIT_TEST(FThreadStartWait1);
    CPPUNIT_TEST(FThreadStartWait2);
    //CPPUNIT_TEST(FThreadStartWait3);
    CPPUNIT_TEST(FThreadMultipleThreads);
    CPPUNIT_TEST(FThreadCascade);
    CPPUNIT_TEST_SUITE_END();

private:

    void FThreadStartWait1() {
        MyThread1 myThread;
        myThread.start();
        myThread.wait();
        // At this point myThread must be terminated
        CPPUNIT_ASSERT(myThread.finished());
        CPPUNIT_ASSERT(!myThread.running());
        CPPUNIT_ASSERT(myThread.getResult() == 45);
    }

    void FThreadStartWait2() {
        MyThread2 myThread;
        myThread.start();
        myThread.wait();
        // At this point myThread must be terminated
        CPPUNIT_ASSERT(myThread.finished());
        CPPUNIT_ASSERT(!myThread.running());
    }

    void FThreadStartWait3() {
        MyThread2 myThread;
        myThread.start();
        myThread.wait(1000);
        // At this point myThread should not be terminated
        CPPUNIT_ASSERT(!myThread.finished());
        CPPUNIT_ASSERT(myThread.running());
        myThread.wait();
        // At this point myThread should be terminated
        CPPUNIT_ASSERT(!myThread.finished());
        CPPUNIT_ASSERT(myThread.running());
    }

    void FThreadMultipleThreads() {
        MyThread1 myThreads[5];

        // Fire 5 threads
        for(int i=0;i<5;++i) {
            myThreads[i].start();
        }
        // Wait for all of them
        for(int i=0;i<5;++i) {
            myThreads[i].wait();
        }
        // Check they completed execution
        for(int i=0;i<5;++i) {
            CPPUNIT_ASSERT(myThreads[i].finished());
            CPPUNIT_ASSERT(!myThreads[i].running());
            CPPUNIT_ASSERT(myThreads[i].getResult() == 45);
        }
    }

    void FThreadCascade() {
        MyThread3 myThread;
        myThread.start();
        myThread.wait();
        // At this point myThread must be terminated
        CPPUNIT_ASSERT(myThread.finished());
        CPPUNIT_ASSERT(!myThread.running());
    }
};
    
CPPUNIT_TEST_SUITE_REGISTRATION( FThreadTest );

