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

package com.funambol.syncclient.spds.engine;

import java.lang.reflect.Method;

import java.util.Hashtable;
import java.util.Enumeration;

import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.common.SimpleClassLoader;

/**
 * This is a factory for SyncSource objects. Its istantiates the right instance
 * given the source to be used. The it calls the setXXX() method for each
 * property in the source definition <i>properties</i> hashtable.
 *
 * @version $Id: SyncSourceFactory.java,v 1.4 2007-12-22 19:14:00 nichele Exp $
 */
public class SyncSourceFactory {

    private Hashtable  syncSourcesDefinition  = null         ;
    private Hashtable  syncSources            = null         ;

    private String     libDir                 = null         ;
    private Logger     logger                 = new Logger() ;

    /**
     * Creates a new SyncSourceFactory reading the sync source implementation
     * classes from the given <i>libDirectory</i> and using the given
     * <i>syncSourcesDefinition</i>.
     *
     * @param libDir directory where classes are looked for
     * @param syncSourcesDefinition sources definitions
     *
     */
    public SyncSourceFactory (String libDir, Hashtable syncSourcesDefinition) {

        this.syncSourcesDefinition = syncSourcesDefinition;
        this.syncSources = new Hashtable();

        this.libDir = libDir;

    }

    /**
     * @return syncSource
     */
    public SyncSource getSyncSource(String dataStoreName) {

        SyncSource s = (SyncSource)syncSources.get(dataStoreName);

        if (s != null) {
            return s;
        }

        String syncSourceClass = null;
        SyncSource syncSource = null;

        SyncSourceDefinition def = (SyncSourceDefinition) syncSourcesDefinition.get(dataStoreName);

        syncSourceClass = def.getSourceClass();

        try {
            syncSource = (SyncSource) (Class.forName(syncSourceClass)).newInstance();
        } catch (Exception e) {
            e.printStackTrace();

            String msg = "Error loading class " +
                         syncSourceClass        +
                         ": "                   +
                         e.getMessage()         ;

            if (logger.isLoggable(Logger.INFO)) {
                logger.info(msg);
            }

            return null;
        }


        Hashtable properties = def.getProperties();

        Enumeration e = properties.keys();

        String key = null;
        while (e.hasMoreElements()) {
            key = (String)e.nextElement();
            setProperty(syncSource, key, (String)properties.get(key));
        }

        syncSources.put((Object) dataStoreName, (Object) syncSource);

        return syncSource;
    }

    // ---------------------------------------------------------- Private methds

    private void setProperty(SyncSource source, String key, String value) {
        String methodName = "";

        char firstLetter = key.toUpperCase().charAt(0);
        if (key.length() > 1) {
            methodName = key.substring(1);
        }

        methodName = "set" + firstLetter + methodName;

        Class sourceClass = source.getClass();

        try {
            Method m = sourceClass.getMethod(methodName, new Class[] { String.class });
            m.invoke(source, new String[] {value});
        } catch (Exception e) {
            String msg = "Property "                 +
                          key                        +
                          " not set to "             +
                          value                      +
                          ". Method "                +
                          methodName                 +
                          "(String s) not found in " +
                          sourceClass.getName()      ;

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(msg);
            }

        }
    }
}
