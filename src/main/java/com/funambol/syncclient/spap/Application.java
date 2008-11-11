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

import java.util.Vector;

import com.funambol.syncclient.sps.common.DataStoreMetadata;
import com.funambol.syncclient.common.StringTools;

/**
 * This class models a SyncClient Conduit application
 *
 *
 * @version $Id: Application.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class Application {

    /**
     * Creates a new instance of Application. The application is identified by
     * the given <i>application uri</i>.
     *
     * @param uri the application uri
     *
     */
    public Application(String uri) {
        this.uri = uri;
        this.dataStoresMetadata = new Vector();
    }

    /**
     * The application URI
     */
    private String uri;

    public String getUri() {
        return this.uri;
    }

    public void setUri(String uri) {
        this.uri = uri;
    }

    /**
     * The displayName property
     */
    private String displayName;

    public String getDisplayName() {
        return this.displayName;
    }

    public void setDisplayName(String displayName) {
        this.displayName = displayName;
    }

    /**
     * The creatorId property
     */
    private String creatorId;

    public String getCreatorId() {
        return this.creatorId;
    }

    public void setCreatorId(String creatorId) {
        this.creatorId = creatorId;
    }

    /**
     * The dataStoreType property
     */
    private String dataStoreType;

    public String getDataStoreType() {
        return this.dataStoreType;
    }

    public void setDataStoreType(String dataStoreType) {
        this.dataStoreType = dataStoreType;
    }

    /**
     * The description property
     */
    private String description;

    public String getDescription() {
        return this.description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    /**
     * The supportUrl property
     */
    private String supportUrl;

    public String getSupportUrl() {
        return this.supportUrl;
    }

    public void setSupportUrl(String supportUrl) {
        this.supportUrl = supportUrl;
    }

    /**
     * The supportEmail property
     */
    private String supportEmail;

    public String getSupportEmail() {
        return this.supportEmail;
    }

    public void setSupportEmail(String supportEmail) {
        this.supportEmail = supportEmail;
    }

    /**
     * The sync property
     */
    private boolean sync;

    public boolean isSync() {
        return this.sync;
    }

    public void setSync(boolean sync) {
        this.sync = sync;
    }
    /**
     * The storeManagerPkg property
     */
    private String storeManagerPkg;

    public String getStoreManagerPkg() {
        return this.storeManagerPkg;
    }

    public void setStoreManagerPkg(String storeManagerPkg) {
        this.storeManagerPkg = storeManagerPkg;
    }

    /**
     * The dataStoresMetadata property
     */
    private Vector dataStoresMetadata;

    public Vector getDataStoresMetadata() {
        return this.dataStoresMetadata;
    }

    public void setDataStoresMetadata(Vector storeManagerPkg) {
        this.dataStoresMetadata = dataStoresMetadata;
    }

    /**
     * The contentId property
     */
    private String contentId;

    public String getContentId() {
        return this.contentId;
    }

    public void setContentId(String contentId) {
        this.contentId = contentId;
    }

    /**
     * The author
     */
    private String author;

    public String getAuthor() {
        return this.author;
    }

    public void setAuthor(String author) {
        this.author = author;
    }

    /**
     * The version
     */
    private String version;

    public String getVersion() {
        return this.version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    /**
     * The asset the application was installed from
     */
    private String assetId;

    public String getAssetId() {
        return this.assetId;
    }

    public void setAssetId(String assetId) {
        this.assetId = assetId;
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Returns the application uri after changing special characters to '_'
     *
     * @return the fixed application uri
     */
    public String getFixedURI() {
        return StringTools.replaceSpecial(uri);
    }

    /**
     * Adds a new data store metadata to the metadata vector
     *
     * @param md the DataStoreMetadata object - NOT NULL
     *
     */
    public void addDataStoreMetadata(DataStoreMetadata md) {
        dataStoresMetadata.addElement(md);
    }

    /**
     * Returns the display name
     *
     * @return this application's display name
     */
    public String toString() {
        return getDisplayName();
    }

    // --------------------------------------------------------- Private methods
}
