/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2010 Funambol, Inc.
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
package com.funambol.syncml.protocol;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

import com.funambol.util.Log;

/**
 * This class is a pool of protocol objects that can be used during a
 * synchronization. Parsing and formatting requires the creation of many small
 * objects that represent incoming and outgoing messages.
 * The pool creates objects and retain them until the pool is released. There
 * are two kinds of release:
 *
 * <ul>
 *   <li> temporary: items are freed and ready to be used again </li>
 *   <li> permanent: items are freed and are no longer available for subsequent
 *   allocations </li>
 * </ul>
 *
 * Once the pool is temporarly released, objects will get re-used, so it is
 * important that the user releases only when old objects are no longer
 * required.
 *
 * The usage of this pool is aimed at reducing the pressure on the garbage
 * collector.
 */
public class ObjectsPool {

    private static final String TAG_LOG = "ObjectsPool";

    private static Hashtable emptyPool = new Hashtable();
    private static Hashtable usedPool = new Hashtable();

    private static final boolean verbose = false;

    static Status createStatus() {
        return (Status)createObject(Status.class);
    }

    static Meta createMeta() {
        return (Meta)createObject(Meta.class);
    }

    static MetInf createMetInf() {
        return (MetInf)createObject(MetInf.class);
    }

    static Item createItem() {
        return (Item)createObject(Item.class);
    }

    static SourceParent createSourceParent() {
        return (SourceParent)createObject(SourceParent.class);
    }

    static Target createTarget() {
        return (Target)createObject(Target.class);
    }

    static Source createSource() {
        return (Source)createObject(Source.class);
    }

    static SourceRef createSourceRef() {
        return (SourceRef)createObject(SourceRef.class);
    }

    static TargetRef createTargetRef() {
        return (TargetRef)createObject(TargetRef.class);
    }

    static SyncMLCommand createSyncMLCommand(String name) {
        synchronized (emptyPool) {
            Vector syncMLCommandEmptyList = (Vector)emptyPool.get(SyncMLCommand.class);
            if (syncMLCommandEmptyList == null || syncMLCommandEmptyList.size() == 0) {
                // Create a new status
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Creating new SyncMLCommand " + name);
                    }
                }
                SyncMLCommand command;
                command = new SyncMLCommand(name, null);
                Vector syncMLCommandUsedList = (Vector)usedPool.get(SyncMLCommand.class);
                if (syncMLCommandUsedList == null) {
                    syncMLCommandUsedList = new Vector();
                    usedPool.put(SyncMLCommand.class, syncMLCommandUsedList);
                }
                syncMLCommandUsedList.addElement(command);
                return command;
            } else {
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Reusing existing SyncMLCommand");
                    }
                }
                SyncMLCommand command;
                command = (SyncMLCommand)syncMLCommandEmptyList.elementAt(syncMLCommandEmptyList.size()-1);
                syncMLCommandEmptyList.removeElementAt(syncMLCommandEmptyList.size() - 1);
                command.init();
                if (name != null) {
                    command.setName(name);
                }
                return command;
            }
        }
    }

    static Data createData(String s, Anchor a, DevInf d, byte b[]) {
        synchronized (emptyPool) {
            Vector dataEmptyList = (Vector)emptyPool.get(Data.class);
            if (dataEmptyList == null || dataEmptyList.size() == 0) {
                // Create a new status
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Creating new Data ");
                    }
                }
                Data data;
                if (s != null) {
                    data = new Data(s);
                } else if (a != null) {
                    data = new Data(a);
                } else if (d != null) {
                    data = new Data(d);
                } else {
                    data = new Data(b);
                }
                Vector dataUsedList = (Vector)usedPool.get(Data.class);
                if (dataUsedList == null) {
                    dataUsedList = new Vector();
                    usedPool.put(Data.class, dataUsedList);
                }
                dataUsedList.addElement(data);
                return data;
            } else {
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Reusing existing Data");
                    }
                }
                Data data = (Data)dataEmptyList.elementAt(dataEmptyList.size() - 1);
                dataEmptyList.removeElementAt(dataEmptyList.size() - 1);
                data.init();
                if (s != null) {
                    data.setData(s);
                } else if (a != null) {
                    data.setAnchor(a);
                } else if (d != null) {
                    data.setDevInf(d);
                } else {
                    data.setBinData(b);
                }
                return data;
            }
        }
    }


    static Object createObject(Class klass) {
        synchronized (emptyPool) {
            Vector emptyList = (Vector)emptyPool.get(klass);
            if (emptyList == null || emptyList.size() == 0) {
                // Create a new status
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Creating new " + klass.getName());
                    }
                }
                try {
                    Object object = klass.newInstance();
                    Vector usedList = (Vector)usedPool.get(klass);
                    if (usedList == null) {
                        usedList = new Vector();
                        usedPool.put(klass, usedList);
                    }
                    usedList.addElement(object);
                    return object;
                } catch (Exception e) {
                    throw new IllegalArgumentException("Cannot instantiate " + klass);
                }
            } else {
                if (verbose) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "Reusing existing " + klass.getName());
                    }
                }
                ReusableObject obj = (ReusableObject)emptyList.elementAt(emptyList.size() - 1);
                emptyList.removeElementAt(emptyList.size() - 1);
                obj.init();
                return obj;
            }
        }
    }

    /**
     * Permanently release all the objects in the pool. After invoking this
     * method all the memory is actually freed and no objects are available for
     * reuse
     */
    public static void releaseAll() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Releasing all pool");
        }
        emptyPool = new Hashtable();
        usedPool = new Hashtable();
    }

    /**
     * Temporary release objects. The memory is not actually released and
     * instances are retained for reuse.
     */
    public static void release() {
        if (verbose) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Releasing all objects in the pool");
            }
        }
        //Move all the used objects into the empty ones
        synchronized(emptyPool) {
            Enumeration keys = usedPool.keys();
            while(keys.hasMoreElements()) {
                Class klass = (Class)keys.nextElement();
                Vector u = (Vector)usedPool.get(klass);
                Vector e = (Vector)emptyPool.get(klass);
                for(int i=0;i<u.size();++i) {
                    Object o = u.elementAt(i);
                    if (e == null) {
                        e = new Vector();
                        emptyPool.put(klass, e);
                    }
                    e.addElement(o);
                }
                u.removeAllElements();
            }
            // Dump stats
            dumpStats();
        }
    }

    private static void dumpStats() {
        if (verbose) {
            Enumeration keys = usedPool.keys();
            while(keys.hasMoreElements()) {
                Class klass = (Class)keys.nextElement();
                Vector u = (Vector)usedPool.get(klass);
                Vector e = (Vector)emptyPool.get(klass);
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Class " + klass.getName() + " status is: " + u.size() + "/" + e.size());
                }
            }
        }
    }
}
