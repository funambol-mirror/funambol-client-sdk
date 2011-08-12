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

package com.funambol.util.bus;

import com.funambol.util.Log;
import java.lang.ref.WeakReference;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * Represents a simple bus communication framework which allows handlers to 
 * register for incoming messages of a specific type.
 * 
 * Handlers can be registered using a Class type.
 * If you register a handler for the type BusMessage it will be notified of all
 * the incoming messages.
 * If you register a handler for the type TypifiedBusMessage which is a subclass 
 * of BusMessage, it will be notified of the incoming messages of type
 * BusMessage or TypifiedBusMessage, not any other message types.
 *
 * This class implementation is thread safe.
 */
public class Bus {
    
    private static Bus instance = null;

    private static final String TAG_LOG = "Bus";

    private Hashtable handlers = new Hashtable();
    private MessageQueue messageQueue = new MessageQueue();

    private final Object handlersLock = new Object();
    private final Object messageQueueLock = new Object();
    
    private Bus() {
        // Init/start the message queue runner
        Thread messageQueueThread = new Thread(new MessageQueueRunner());
        messageQueueThread.setDaemon(true);
        messageQueueThread.start();
        // Allow the daemon to start. This is especially important for unit test
        // and has no real impact on client's implementations
        do {
            try {
                Thread.yield();
                Thread.sleep(100);
            } catch (Exception e) {
            }
        } while(!messageQueueThread.isAlive());
    }

    /**
     * @return the current bus instance
     */
    public static synchronized Bus getInstance() {
        if(instance == null) {
            instance = new Bus();
        }
        return instance;
    }

    /**
     * Register a new BusMessageHandler object for the given message type.
     * Handler objects are stored using weak references to avoid memory leaks.
     * 
     * @param type The message type
     * @param handler The new BusMessageHandler object to register
     * @return true if the handler has been registered
     */
    public boolean registerMessageHandler(Class type, BusMessageHandler handler) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "register message handler for type " + type);
        }
        synchronized(handlersLock) {
            Vector typeHandlers = (Vector)handlers.get(type);
            if(typeHandlers == null) {
                typeHandlers = new Vector();
            }
            if(findMessageHandler(type, handler) != null) {
                // Already registered
                return false;
            }
            WeakReference wrHandler = new WeakReference(handler);
            typeHandlers.addElement(wrHandler);
            handlers.put(type, typeHandlers);
            return true;
        }
    }

    /**
     * Unregister an existing BusMessageHandler object for the given message type.
     *
     * @param type The message type
     * @param handler The BusMessageHandler object to unregister
     * @return true if the handler has been unregistered
     */
    public boolean unregisterMessageHandler(Class type, BusMessageHandler handler) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "unregister message handler for type " + type);
        }
        synchronized(handlersLock) {
            Vector typeHandlers = (Vector)handlers.get(type);
            if(typeHandlers != null) {
                WeakReference wrHandler = findWRMessageHandler(type, handler);
                if(wrHandler != null) {
                    typeHandlers.removeElement(wrHandler);
                    handlers.put(type, typeHandlers);
                    return true;
                }
            }
            return false;
        }
    }

    public static void dispose() {
        instance = null;
    }

    /**
     * Send the given message to the registered handlers. A call to this method
     * is asynchronous and will return immediately.
     * 
     * @param message The message to send
     */
    public void sendMessage(BusMessage message) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "sending message " + message);
        }
        synchronized(messageQueueLock) {
            messageQueue.add(message);
            messageQueueLock.notifyAll();
        }
    }

    // Send the given message to all the related handlers
    private void dispatchMessage(BusMessage message) {
        synchronized(handlersLock) {
            Enumeration types = handlers.keys();
            while(types.hasMoreElements()) {
                Class type = (Class)types.nextElement();
                if(type.isInstance(message)) {
                    Vector typeHandlers = (Vector)handlers.get(type);
                    dispatchMessageToHandlers(message, typeHandlers);
                }
            }
        }
    }

    private void dispatchMessageToHandlers(BusMessage message, Vector handlers) {
        Enumeration elements = handlers.elements();
        while(elements.hasMoreElements()) {
            WeakReference wrHandler = (WeakReference)elements.nextElement();
            BusMessageHandler handler = (BusMessageHandler)wrHandler.get();
            // Check if the handler reference is still there
            if(handler != null) {
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "dispatching message to handler " + handler + "," + message);
                }
                handler.receiveMessage(message);
            }
        }
    }

    // Find the specified handler within the message type handlers
    private BusMessageHandler findMessageHandler(Class type, BusMessageHandler handler) {
        synchronized(handlersLock) {
            WeakReference wrHandler = findWRMessageHandler(type, handler);
            if(wrHandler != null) {
                return (BusMessageHandler)wrHandler.get();
            } else {
                return null;
            }
        }
    }
    private WeakReference findWRMessageHandler(Class type, BusMessageHandler handler) {
        synchronized(handlersLock) {
            Vector typeHandlers = (Vector)handlers.get(type);
            if(typeHandlers != null) {
                Enumeration elements = typeHandlers.elements();
                while(elements.hasMoreElements()) {
                    WeakReference wrHandler = (WeakReference)elements.nextElement();
                    if(handler == (BusMessageHandler)wrHandler.get()) {
                        return wrHandler;
                    }
                }
            }
            return null;
        }
    }

    // Dispatches the messages from the messageQueue
    private class MessageQueueRunner implements Runnable {
        public void run() {
            try {
                while (true) {
                    synchronized(messageQueueLock) {
                        messageQueueLock.wait();
                    }
                    BusMessage message;
                    while((message = getNextMessage()) != null) {
                        dispatchMessage(message);
                    }
                }
            } catch (Throwable e) {
                throw new RuntimeException(e);
            }
        }

        public BusMessage getNextMessage() {
            synchronized(messageQueueLock) {
                return messageQueue.get();
            }
        }
    }

    /**
     * Represents a FIFO queue of BusMessage objects. It simply allows to add
     * elements and remove them from the list.
     */
    private class MessageQueue {

        private Vector internalQueue = new Vector();
        
        public BusMessage get() {
            if(internalQueue.size() > 0) {
                BusMessage result = (BusMessage)internalQueue.elementAt(0);
                internalQueue.removeElementAt(0);
                return result;
            } else {
                return null;
            }
        }

        public void add(BusMessage m) {
            internalQueue.addElement(m);
        }
    }
}

