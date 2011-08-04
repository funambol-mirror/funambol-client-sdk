/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.concurrent;

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.TestCase;


public class TaskExecutorTest extends TestCase {

    private TaskExecutor executor;

    public TaskExecutorTest(String name) {
        super(name);
    }
    
    public void setUp() {
        executor = new TaskExecutor();
        executor.setMaxThreads(1);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }
    
    public void tearDown() {
    }

    public void testScheduleSimpleTask() {

        SimpleTask task = new SimpleTask();

        assertTrue(!task.isCompleted());
        executor.scheduleTask(task);

        // Allow the task to complete
        sleep(100);
        
        assertTrue(task.isCompleted());
    }

    public void testSingleThreadNoResume() {

        SimpleResumableTask highPTask = new SimpleResumableTask();

        assertTrue(!highPTask.isRunning());
        executor.scheduleTaskWithPriority(highPTask, TaskExecutor.PRIORITY_HIGH);

        // Allow the high priority task to start
        sleep(50);

        assertTrue(highPTask.isRunning());

        SimpleResumableTask lowPTask = new SimpleResumableTask();
        executor.scheduleTaskWithPriority(lowPTask, TaskExecutor.PRIORITY_LOW);

        // Allow the low priority task to be processed
        sleep(100);

        assertTrue(highPTask.isRunning());
        assertTrue(!lowPTask.isRunning());

        highPTask.stop();

        // Allow the high priority task to stop and the low priority task to start
        sleep(100);

        assertTrue(!highPTask.isRunning());
        assertTrue(lowPTask.isRunning());

        lowPTask.stop();

        // Allow all tasks to complete
        sleep(200);

        assertTrue(!lowPTask.isRunning());
        assertTrue(!highPTask.isRunning());

        assertTrue(!lowPTask.hasBeenSuspended());
        assertTrue(!highPTask.hasBeenSuspended());

        assertTrue(!lowPTask.hasBeenResumed());
        assertTrue(!highPTask.hasBeenResumed());
    }

    public void testSingleThreadNoResume2() {

        SimpleResumableTask highPTask1 = new SimpleResumableTask();

        assertTrue(!highPTask1.isRunning());
        executor.scheduleTaskWithPriority(highPTask1, TaskExecutor.PRIORITY_HIGH);

        // Allow the task to start
        sleep(50);

        assertTrue(highPTask1.isRunning());

        SimpleResumableTask highPTask2 = new SimpleResumableTask();
        executor.scheduleTaskWithPriority(highPTask2, TaskExecutor.PRIORITY_HIGH);

        // Allow the highPTask2 to be processed
        sleep(100);

        assertTrue(highPTask1.isRunning());
        assertTrue(!highPTask2.isRunning());

        highPTask1.stop();

        // Allow the highPTask1 to stop and the highPTask2 to start
        sleep(100);

        assertTrue(!highPTask1.isRunning());
        assertTrue(highPTask2.isRunning());

        highPTask2.stop();

        // Allow all tasks to complete
        sleep(200);

        assertTrue(!highPTask2.isRunning());
        assertTrue(!highPTask1.isRunning());

        assertTrue(!highPTask2.hasBeenSuspended());
        assertTrue(!highPTask1.hasBeenSuspended());

        assertTrue(!highPTask2.hasBeenResumed());
        assertTrue(!highPTask1.hasBeenResumed());
    }

    public void testSingleThreadResume() {

        SimpleResumableTask lowPTask = new SimpleResumableTask();

        assertTrue(!lowPTask.isRunning());
        executor.scheduleTaskWithPriority(lowPTask, TaskExecutor.PRIORITY_LOW);

        // Allow the low priority task to start
        sleep(50);

        assertTrue(lowPTask.isRunning());

        SimpleTask highPTask = new SimpleTask();
        executor.scheduleTaskWithPriority(highPTask, TaskExecutor.PRIORITY_HIGH);

        // Allow the high priority task to complete
        sleep(200);

        assertTrue(highPTask.isCompleted());

        assertTrue(lowPTask.hasBeenSuspended());
        assertTrue(!lowPTask.isRunning());

        // Allow the low priority task to be resumed
        sleep(100);

        assertTrue(lowPTask.hasBeenResumed());
    }

    public void testMultipleThreadsNoResume() {

        executor.setMaxThreads(3);

        SimpleResumableTask task1 = new SimpleResumableTask();
        SimpleResumableTask task2 = new SimpleResumableTask();
        SimpleResumableTask task3 = new SimpleResumableTask();

        assertTrue(!task1.isRunning());
        assertTrue(!task2.isRunning());
        assertTrue(!task3.isRunning());
        
        executor.scheduleTaskWithPriority(task1, TaskExecutor.PRIORITY_LOW);
        executor.scheduleTaskWithPriority(task2, TaskExecutor.PRIORITY_MEDIUM);
        executor.scheduleTaskWithPriority(task3, TaskExecutor.PRIORITY_HIGH);

        // Allow all the tasks to start
        sleep(100);

        assertTrue(task1.isRunning());
        assertTrue(task2.isRunning());
        assertTrue(task3.isRunning());
        
        task1.stop();
        task2.stop();
        task3.stop();

        // Allow all the tasks to complete
        sleep(100);

        assertTrue(!task1.isRunning());
        assertTrue(!task2.isRunning());
        assertTrue(!task3.isRunning());

        assertTrue(!task1.hasBeenSuspended());
        assertTrue(!task2.hasBeenSuspended());
        assertTrue(!task3.hasBeenSuspended());

        assertTrue(!task1.hasBeenResumed());
        assertTrue(!task2.hasBeenResumed());
        assertTrue(!task3.hasBeenResumed());
    }

    public void testMultipleThreadsResume() {

        executor.setMaxThreads(2);

        SimpleResumableTask task1 = new SimpleResumableTask();
        SimpleResumableTask task2 = new SimpleResumableTask();
        SimpleResumableTask task3 = new SimpleResumableTask();

        assertTrue(!task1.isRunning());
        assertTrue(!task2.isRunning());
        assertTrue(!task3.isRunning());

        executor.scheduleTaskWithPriority(task1, TaskExecutor.PRIORITY_LOW);
        executor.scheduleTaskWithPriority(task2, TaskExecutor.PRIORITY_MEDIUM);

        // Allow all the tasks to start
        sleep(100);

        assertTrue(task1.isRunning());
        assertTrue(task2.isRunning());
        assertTrue(!task3.isRunning());
        
        executor.scheduleTaskWithPriority(task3, TaskExecutor.PRIORITY_HIGH);

        // Allow task3 to start
        sleep(100);

        assertTrue(task3.isRunning());

        assertTrue(task1.hasBeenSuspended());
        assertTrue(!task1.isRunning());

        task2.stop();
        task3.stop();

        // Allow all the tasks to complete
        sleep(100);

        assertTrue(!task1.isRunning());
        assertTrue(!task2.isRunning());
        assertTrue(!task3.isRunning());

        assertTrue(task1.hasBeenSuspended());
        assertTrue(!task2.hasBeenSuspended());
        assertTrue(!task3.hasBeenSuspended());

        assertTrue(task1.hasBeenResumed());
        assertTrue(!task2.hasBeenResumed());
        assertTrue(!task3.hasBeenResumed());
    }

    private class SimpleTask implements Task {

        private boolean completed = false;

        public void run() {
            completed = false;
            sleep(20);
            completed = true;
        }

        public boolean isCompleted() {
            return completed;
        }
    }

    private class SimpleResumableTask implements ResumableTask {

        private boolean running = false;
        private boolean suspended = false;
        private boolean resumed = false;

        private boolean stop = false;

        public void run() {
            running = true;
            stop = false;
            while(!stop) {
                sleep(20);
            }
            running = false;
        }

        public void stop() {
            stop = true;
        }

        public boolean suspend() {
            stop();
            suspended = true;
            return true;
        }

        public void resume() {
            resumed = true;
        }

        public boolean isRunning() {
            return running;
        }

        public boolean hasBeenSuspended() {
            return suspended;
        }

        public boolean hasBeenResumed() {
            return resumed;
        }
    }

    private void sleep(long time) {
        try {
            Thread.sleep(time);
        } catch (Exception e) {}
    }
}

