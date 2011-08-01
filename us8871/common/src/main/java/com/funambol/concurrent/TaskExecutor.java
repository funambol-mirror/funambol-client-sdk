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

import java.util.Vector;

public class TaskExecutor {

    private final int DEFAULT_MAX_THREADS_COUNT = 3;

    public static final int PRIORITY_LOW    = -1;
    public static final int PRIORITY_MEDIUM = 0;
    public static final int PRIORITY_HIGH   = 1;

    private final Vector tasksThreadPool = new Vector();
    private final TaskQueue taskQueue = new TaskQueue();
    
    private static TaskExecutor instance = null;

    private int maxThreads = DEFAULT_MAX_THREADS_COUNT;

    private TaskExecutor() {
    }

    /**
     * @return the current TaskExecutor instance
     */
    public static synchronized TaskExecutor getInstance() {
        if(instance == null) {
            instance = new TaskExecutor();
        }
        return instance;
    }

    /**
     * Set the max number of threads used to schedule tasks.
     * @param count
     */
    public void setMaxThreads(int count) {
        maxThreads = count;
    }
    /**
     * Schedule the given task with low priority.
     * @param task
     */
    public void scheduleTask(Task task) {
        scheduleTaskWithPriority(task, PRIORITY_LOW);
    }

    /**
     * Schedule the given task with the given priority.
     * @param task
     * @param priority
     */
    public void scheduleTaskWithPriority(Task task, int priority) {
        synchronized(tasksThreadPool) {
            PriorityTask ptask = new PriorityTask(task, priority);
            boolean started = startNewThreadedTask(ptask);
            if(!started) {
                enqueueTask(ptask);
                // If there is no room to run the given task, then we start looking
                // for a lower priority task to suspend.
                for(int i=0; i<tasksThreadPool.size(); i++) {
                    ThreadedTask threadedTask = (ThreadedTask)tasksThreadPool.elementAt(i);
                    if((threadedTask.getPriorityTask().getPriority() < ptask.getPriority())) {
                        if(threadedTask.isResumable() && threadedTask.suspend()) {
                            // Enqueue suspended task
                            enqueueTask(threadedTask.getPriorityTask());
                            break;
                        }
                    }
                }
            }
        }
    }

    private void enqueueTask(PriorityTask task) {
        synchronized(taskQueue) {
            taskQueue.put(task);
        }
    }

    private boolean startNewThreadedTask(PriorityTask task) {
        synchronized(tasksThreadPool) {
            if(tasksThreadPool.size() < maxThreads) {
                ThreadedTask threadedTask = new ThreadedTask(task);
                tasksThreadPool.addElement(threadedTask);
                threadedTask.start();
                return true;
            } else {
                return false;
            }
        }
    }
    
    private void taskCompleted(ThreadedTask threadedTask) {
        synchronized(tasksThreadPool) {
            // Make room and run a new task
            tasksThreadPool.remove(threadedTask);
            PriorityTask next = taskQueue.get();
            if(next != null) {
                startNewThreadedTask(next);
            }
        }
    }

    private class ThreadedTask {

        private PriorityTask ptask;
        private TaskRunnable taskRunnable;
        private Thread taskThread;

        public ThreadedTask(PriorityTask task) {
            this.ptask = task;
            this.taskRunnable = new TaskRunnable(this);
            this.taskThread = new Thread(taskRunnable);
        }

        public PriorityTask getPriorityTask() {
            return ptask;
        }

        /**
         * Start the task from a new thread
         */
        public void start() {
            taskThread.start();
        }

        /**
         * @return whether the current task is resumable
         */
        public boolean isResumable() {
            return (ptask.getTask() instanceof ResumableTask);
        }

        /**
         * Suspend the thread task
         * @return true if the task has been correctly suspended
         */
        public boolean suspend() {
            if(isResumable()) {
                boolean suspended = ((ResumableTask)ptask.getTask()).suspend();
                ptask.setSuspended(suspended);
                return suspended;
            } else {
                return false;
            }
        }
    }

    private class TaskRunnable implements Runnable {

        private ThreadedTask threadedTask;

        public TaskRunnable(ThreadedTask threadedTask) {
            this.threadedTask = threadedTask;
        }

        public void run() {
            PriorityTask ptask = threadedTask.getPriorityTask();
            // Resume task if it has been previously suspended
            if(ptask.isSuspended() && threadedTask.isResumable()) {
                ptask.setSuspended(false);
                ResumableTask resumableTask = ((ResumableTask)ptask.getTask());
                resumableTask.resume();
            } else {
                ptask.getTask().run();
            }
            getInstance().taskCompleted(threadedTask);
        }
    }

    private class TaskQueue {
        
        private Vector internalQueue = new Vector();

        public PriorityTask get() {
            if(internalQueue.size() > 0) {
                PriorityTask pt = (PriorityTask)internalQueue.elementAt(0);
                internalQueue.removeElementAt(0);
                return pt;
            } else {
                return null;
            }
        }

        public void put(PriorityTask task) {
            int index = 0;
            for(; index<internalQueue.size(); index++) {
                PriorityTask pt = (PriorityTask)internalQueue.elementAt(index);
                if(pt.getPriority() < task.getPriority()) {
                    break;
                }
            }
            internalQueue.insertElementAt(task, index);
        }
    }

    private class PriorityTask {

        private Task task;
        private int priority;
        private boolean suspended;

        public PriorityTask(Task task, int priority) {
            this.task = task;
            this.priority = priority;
            this.suspended = false;
        }

        public Task getTask() {
            return task;
        }

        public int getPriority() {
            return priority;
        }

        public boolean isSuspended() {
            return suspended;
        }

        public void setSuspended(boolean suspended) {
            this.suspended = suspended;
        }
    }

}

