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

import java.io.*;
import java.util.Hashtable;

import java.security.Principal;

import com.funambol.syncclient.common.StringTools;


/**
 * This class groups the configuration information of a SyncSource.
 *
 *
 *
 * @version $Id: SyncSourceDefinition.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncSourceDefinition {

    // --------------------------------------------------------------- Constants

    public static final String CONFIG_NAME                 = "name"          ;
    public static final String CONFIG_CLASS                = "sourceClass"   ;
    public static final String CONFIG_URI                  = "sourceURI"     ;
    public static final String CONFIG_TYPE                 = "type"          ;
    public static final String CONFIG_LAST                 = "last"          ;
    public static final String CONFIG_DEFAULT_SYNC         = "sync"          ;
    public static final String CONFIG_SUPPORTED_SYNC_MODES = "syncModes"     ;

    // ------------------------------------------------------------ Private date

    private Hashtable properties = null;

    public String getName() {
        return (String)properties.get(CONFIG_NAME);
    }

    public String getSourceClass() {
        return (String)properties.get(CONFIG_CLASS);
    }

    public String getSourceUri() {
        return (String)properties.get(CONFIG_URI);
    }

    public String getType() {
        return (String)properties.get(CONFIG_TYPE);
    }

    public String getDefaultSync() {
        return (String)properties.get(CONFIG_DEFAULT_SYNC);
    }

    public String[] getSupportedSyncModes() {
        return StringTools.split((String)properties.get(CONFIG_SUPPORTED_SYNC_MODES));
    }

    public long getLastTimestamp() {
        String last = (String)properties.get(CONFIG_LAST);

        try {
            return (last == null) ? 0 : Long.parseLong(last);
        } catch (NumberFormatException e) {
            return 0;
        }
    }

    public Hashtable getProperties() {
        return properties;
    }

    public SyncSourceDefinition (Hashtable properties) {
        String sourceClass  = (String)properties.get(CONFIG_CLASS               );
        String uri          = (String)properties.get(CONFIG_URI                 );
        String className    = (String)properties.get(CONFIG_CLASS               );
        String name         = (String)properties.get(CONFIG_NAME                );
        String type         = (String)properties.get(CONFIG_TYPE                );
        String defaultSync  = (String)properties.get(CONFIG_DEFAULT_SYNC        );
        String syncModes    = (String)properties.get(CONFIG_SUPPORTED_SYNC_MODES);


        if (sourceClass == null) {
            throw new IllegalArgumentException("Missing 'sourceClass' in properties!");
        }

        if (uri == null) {
            throw new IllegalArgumentException("Missing 'uri' in properties!");
        }

        if (name == null) {
            throw new IllegalArgumentException("Missing 'name' in properties!");
        }

        if (type == null) {
            throw new IllegalArgumentException("Missing 'type' in properties!");
        }

        if (defaultSync == null) {
            properties.put(CONFIG_DEFAULT_SYNC, "two-way");
        }

        if (syncModes == null) {
            properties.put(CONFIG_SUPPORTED_SYNC_MODES, "two-way");
        }

        this.properties = properties;
    }
}
