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
 * The interactive user interfaces in modified sourceName and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

package com.funambol.syncml.spds;

import com.funambol.storage.AbstractRecordStore;
import com.funambol.storage.ObjectStore;
import com.funambol.util.Log;
import java.util.Hashtable;
import javax.microedition.rms.RecordStoreException;

/**
 * A class that saves and retrieves the mapping information from the store
 */
public class MappingManager {
    
    private static final String MAPPING_STORE = "SyncMLMappingStore";

    private ObjectStore ms = new ObjectStore();
    
    public MappingManager() {
    }

    /**
     * Returns the ItemMap related to the given name
     * 
     * @param sourceName the name of the source to be retrieved
     * @return ItemMap of the given source
     */
    public Hashtable getMappings(String sourceName) {
        ItemMap im = new ItemMap();

        try {
            //Create or open the Mapping storage
            openMappingStore();
            for (int i=1; i<=ms.size(); i++) {
                im = (ItemMap) ms.retrieve(i, new ItemMap());
                if (im.getSourceName().equals(sourceName)) {
                    Log.debug("[getMappings]Item Map Found");
                    break;
                }
            }
            ms.close();
            return im.getMappings();
        } catch (Exception ex) {
            Log.error("[getMappings]Exception catched reading the mapping stores" + ex);
        }
        
        //returns empty hashtable when no ItemMap is found
        Log.debug("[getMappings]ItemMap not found");
        return im.getMappings();
    }

    /**
     * Save the given ItemMap for the given source
     * @param sourceName the ItemMap name
     * @param mappings the mapping hshtable
     */
    public void saveMappings(String sourceName, Hashtable mappings) {
        //Creates the map to be persisted
        ItemMap newMap = new ItemMap(sourceName, mappings);
        
        try {
            boolean isMapRecordAvailable = false;
            
            //Create or open the Mapping storage
            openMappingStore();
            for (int i=1; i<=ms.size(); i++) {
                ItemMap im = (ItemMap) ms.retrieve(i, new ItemMap());
                if (im.getSourceName().equals(newMap.getSourceName())) {
                    Log.debug("[saveMappings]Item map: " + im.getSourceName());
                    Log.debug("[saveMappings]Required source: " + sourceName);
                    ms.store(i, newMap);
                    Log.debug("[saveMappings]Item map updated for source: " + im.getSourceName());
                    isMapRecordAvailable = true;
                    break;
                }
            }
            
            //if the map doesn't already exist creates it on the store
            if (!isMapRecordAvailable) {
                ms.store(newMap);
                Log.debug("[saveMappings]New Item map stored for source: " + newMap.getSourceName());
            }
            ms.close();
            newMap=null;
        } catch (Exception ex) {
            Log.error("[saveMappings]Exception catched writing the mapping stores" + ex);
        }
    }

    /**
     * Reset the mappings for the given source
     * @param sourceName is the name of the source to be reset
     */
    public void resetMappings(String sourceName) {
        saveMappings(sourceName, new Hashtable());
    }

    private void openMappingStore() {
        if (ms==null) {
            ms = new ObjectStore();
        }

        if (isMappingStorageFound()) {
            try {
                Log.debug("[openMappingStore]Opening MApping storage");
                ms.open(MAPPING_STORE);
            } catch (RecordStoreException ex) {
                Log.error("[openMappingStore]Cannot open Mapping Storage" + ex);
            }
        } else {
            try {
                Log.debug("[openMappingStore]Creating Mapping storage");
                ms.create(MAPPING_STORE);
            } catch (RecordStoreException ex) {
                Log.error("[openMappingStore]Cannot create Mapping Storage" + ex);
            }
        }
    } 
    
    private boolean isMappingStorageFound() {
        Log.debug("[isMappingStorageFound]Lookup for mapping RMS");
        String[] tmp = AbstractRecordStore.listRecordStores();
        
        if (tmp==null) {
            Log.debug(MAPPING_STORE + " lookup gave an empty String[]");
            return false;
        }
        
        for (int i=0; i<tmp.length; i++) {
            if (tmp[i].equals(MAPPING_STORE)) {
                Log.debug("[isMappingStorageFound]Mapping RMS Found");
                return true;
            }
        }
        
        Log.debug("[isMappingStorageFound]Mapping RMS Not Found");
        return false;
    }
}
