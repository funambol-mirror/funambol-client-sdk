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
package com.funambol.syncclient.spap;

import java.security.Principal;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Map;
import java.util.Vector;

import sync4j.framework.tools.Base64;

import com.funambol.syncclient.common.XMLHashMapParser;
import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.spds.SyncException;
import com.funambol.syncclient.spds.engine.SyncItem;
import com.funambol.syncclient.spds.engine.SyncItemImpl;
import com.funambol.syncclient.spds.engine.SyncItemProperty;
import com.funambol.syncclient.spds.engine.SyncSource;

/**
 * This class implements a <i>SyncSource</i> that handle the <code>Asset</code>
 * store on a device.
 * <p>Retrieves the list of the Assets and their state using the <code>AssetDAO</code>
 * that manages the memorization of the Assets using the <i>Device Management</i>
 * as database.
 *
 * <p>This SyncSource maps a Asset into properties of the corresponding SyncItem.
 * <p>A asset has transformed in XML using the
 * <code>com.funambol.syncclient.framework.provisioning.XMLHashMapParser</code>
 * class and the xml as gotten is store in the property SyncItem.PROPERTY_BINARY_CONTENT.
 *
 * @version $Id: AssetSyncSource.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class AssetSyncSource implements SyncSource {


    // -------------------------------------------------------------- Properties

    private String sourceURI;

    /**
     * Sets property sourceURI
     * @param sourceURI
     *     description: uri of the SyncSource
     *     displayName: sourceURI
     */
    public void setSourceURI(String sourceURI) {
        this.sourceURI = sourceURI;
    }

    /**
     * Returns property sourceURI
     */
    public String getSourceURI() {
        return sourceURI;
    }


    private String type;

    /**
     * Sets property type
     * @param type
     *     description: type of the SyncSource
     *     displayName: type
     */
    public void setType(String type) {
        this.type = type;
    }

    /**
     * Returns property type
     */
    public String getType() {
        return type;
    }


    private String name;

    /**
     * Sets property name
     * @param name
     *     description: name of the SyncSource
     *     displayName: name
     */
    public void setName(String name) {
        this.name = name;
    }

    /**
     * Returns property name
     */
    public String getName() {
        return name;
    }


    // ------------------------------------------------------------ Private data

    /** AssetDAO used for get asset's information */
    private AssetDAO assetDAO = null;

    private Logger logger = new Logger();

    // ------------------------------------------------------------- Constructor

    /**
     * Constructs a AssetSyncSource
     */
    public AssetSyncSource() {
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling AssetSyncSource()");
        }
        assetDAO = new AssetDAO();
    }


    /**
     * Returns a list of the assets except those with state 'D'
     * @param principal the principal for which the data has to be considered.
     * @return an array of all SyncItems stored in this source.
     *         If there are no items an empty array is returned.
     */
    public SyncItem[] getAllSyncItems(Principal principal) {
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling getAllSyncItems in AssetSyncSource");
        }

        Vector assetsList = assetDAO.getAllAsset();
        int numAssets = assetsList.size();

        SyncItem[] syncItems = new SyncItem[numAssets];

        SyncItem syncItem = null;
        String key = null;
        String type = null;
        String state = null;
        String xml = null;
        Map assetValues = null;
        Asset asset = null;
        AssetVersion newVersion  = null;
        AssetVersion currentVersion = null;
        Timestamp timeStamp = null;

        for (int i=0; i<numAssets; i++) {
            asset = (Asset)assetsList.elementAt(i);
            key = asset.getId();
            state = asset.getState();
            timeStamp = asset.getLastUpdate();

            if (state.equals(Asset.STATE_DELETE)) {
                // skip asset with state='D'
                continue;
            }

            syncItem = new SyncItemImpl(this, key, 'N');

            assetValues = asset.toMap();
            newVersion =  asset.getNewVersion();
            if (newVersion != null) {
                assetValues.putAll(newVersion.toMap());
            } else {
                currentVersion = asset.getCurrentVersion();
                assetValues.putAll(currentVersion.toMap());
            }

            syncItem.setProperty(
                new SyncItemProperty(
                    SyncItem.PROPERTY_BINARY_CONTENT    ,
                    Base64.encode((XMLHashMapParser.toXML(assetValues)).getBytes())
                )
            );

            syncItem.setProperty(
                    new SyncItemProperty(SyncItem.PROPERTY_TIMESTAMP,timeStamp)
                    );

            syncItems[i] = syncItem;
        }

        return syncItems;
    }

    /**
     *
     * @param principal
     * @param since
     * @return deleted syncItems
     */
    public SyncItem[] getDeletedSyncItems(Principal principal, Date since) {
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling getDeletedSyncItems in AssetSyncSource");
        }

        return new SyncItem[0];
    }

    /**
     *
     * @param principal
     * @param since
     * @return new syncItems
     */
    public SyncItem[] getNewSyncItems(Principal principal, Date since) {
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling getNewSyncItems in AssetSyncSource");
        }
        return new SyncItem[0];
    }

    /**
     *
     * @param principal
     * @param since
     * @return update syncItems
     */
    public SyncItem[] getUpdatedSyncItems(Principal principal, Date since) {
        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling getUpdatedSyncItems in AssetSyncSource");
        }

        SyncItem[] syncItems = new SyncItem[0];
        return syncItems;
    }


    /**
     * Replaces an existing Asset or adds a new Asset if it does not exist.
     * The contained version is save as <i>newVersion</i>.
     *
     * @param principal
     * @param syncItem
     * @return syncItem
     * @throws SyncException
     */
    public SyncItem setSyncItem(Principal principal, SyncItem syncItem)
            throws SyncException{

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling setSyncItem in AssetSyncSource");
        }

        String key = syncItem.getKey().getKeyAsString();

        Date dateSync = (Date)syncItem.getPropertyValue(SyncItem.PROPERTY_TIMESTAMP);


        Timestamp timeSync = new Timestamp(dateSync.getTime());

        String xml = new String(
            Base64.decode(
                (byte[])syncItem.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT)
            )
        );

        // transforms the SyncItem.PROPERTY_BINARY_CONTENT in Map
        Map values = XMLHashMapParser.toMap(xml);


        // Cheks if a asset with the same key already exists and if has a currentVersion
        Asset asset = null;
        AssetVersion currentVersion = null;
        try {
            asset = assetDAO.getAsset(key);
            currentVersion = asset.getCurrentVersion();
        }
        catch (AssetManagementException ex) {
        }

        // creates a new asset with a newVersion
        asset = new Asset(values, true);
        asset.setId(key);

        // if a asset with the same key already exists set the currentVersion
        // of the new asset and set the state to 'U'
        if (currentVersion != null) {
            asset.setCurrentVersion(currentVersion);
            asset.setState(Asset.STATE_UPDATE);
        } else {
            asset.setState(Asset.STATE_NEW);
        }

        asset.setLastUpdate(timeSync);

        try {
            assetDAO.setAsset(asset, timeSync);
        }
        catch (AssetManagementException ex) {
            throw new SyncException("Error setting item: " + key, ex);
        }

        return syncItem;

    }

    /**
     * Removes a Asset given its key.
     *
     * @param principal the entity that wants to do the operation
     * @param syncItem the item to remove
     * @throws SyncException in case of error
     */
    public void removeSyncItem(Principal principal, SyncItem syncItem)
            throws SyncException {

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Calling removeSyncItem in AssetSyncSource");
        }

        String key = syncItem.getKey().getKeyAsString();

        Date dateSync = (Date)syncItem.getPropertyValue(SyncItem.PROPERTY_TIMESTAMP);

        try {
            assetDAO.setAssetState(key, "D");

            /**
             * @todo
             */
            // for a bug of the SyncPlatform the dateSync is null
            //assetDAO.setAssetLastUpdate(key, "" + dateSync.getTime());
        }
        catch (AssetManagementException ex) {
            throw new SyncException("Error remove item '" + key + "'", ex);
        }
    }


    public void beginSync(int syncMode) {

    }

    public void beginSync() {

    }

    public void commitSync() {

    }
}
