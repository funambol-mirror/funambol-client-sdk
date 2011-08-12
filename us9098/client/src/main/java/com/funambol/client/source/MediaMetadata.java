/**
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

package com.funambol.client.source;

import com.funambol.storage.Table;
import com.funambol.storage.TableFactory;

public class MediaMetadata {

    public static final String METADATA_ID                    = "id";
    public static final String METADATA_NAME                  = "name";
    public static final String METADATA_THUMBNAIL_PATH        = "thumbnail_path";
    public static final String METADATA_PREVIEW_PATH          = "preview_path";
    public static final String METADATA_ITEM_PATH             = "item_path";
    public static final String METADATA_LAST_MOD              = "last_mod";
    public static final String METADATA_SYNCHRONIZED          = "synchronized";
    public static final String METADATA_DELETED               = "deleted";
    public static final String METADATA_DIRTY                 = "dirty";
    public static final String METADATA_SIZE                  = "size";
    public static final String METADATA_GUID                  = "guid";
    public static final String METADATA_MIME                  = "mime";
    public static final String METADATA_REMOTE_URI            = "remote_uri";
    public static final String METADATA_UPLOAD_CONTENT_STATUS = "upload_content_status";
    public static final String METADATA_DURATION              = "duration";

    /**
     * This is the meta data schema for the media table.
     */
    public static final String META_DATA_COL_NAMES[] = {
                                                   METADATA_ID,
                                                   METADATA_NAME,
                                                   METADATA_THUMBNAIL_PATH,
                                                   METADATA_PREVIEW_PATH,
                                                   METADATA_ITEM_PATH,
                                                   METADATA_LAST_MOD,
                                                   METADATA_SYNCHRONIZED,
                                                   METADATA_DELETED,
                                                   METADATA_DIRTY,
                                                   METADATA_SIZE,
                                                   METADATA_GUID,
                                                   METADATA_MIME,
                                                   METADATA_REMOTE_URI,
                                                   METADATA_UPLOAD_CONTENT_STATUS,
                                                   METADATA_DURATION
                                                  };

    public static final int META_DATA_COL_TYPES[] = {
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_STRING,
                                                   Table.TYPE_LONG,
                                                   Table.TYPE_LONG
                                                  };

    private AppSyncSource appSource;
    private Table metadataTable;

    public MediaMetadata(AppSyncSource appSource) {
        this.appSource = appSource;
        // Construct the metadata table for this source
        // the name is name + _ + metadata (e.g. Pictures_metadata)
        // This table has the autoincrement on the key which is an id
        metadataTable = TableFactory.getInstance().getStringTable(appSource.getName() + "_metadata",
                                                                  META_DATA_COL_NAMES,
                                                                  META_DATA_COL_TYPES,
                                                                  0, true);
    }

    public Table getMetadataTable() {
        return metadataTable;
    }
}
