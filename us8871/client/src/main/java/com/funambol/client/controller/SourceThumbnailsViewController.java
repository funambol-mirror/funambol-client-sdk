/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.client.controller;

import com.funambol.client.customization.Customization;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.MediaMetadata;
import com.funambol.client.source.MetadataBusMessage;
import com.funambol.client.ui.view.SourceThumbnailsView;
import com.funambol.client.ui.view.ThumbnailView;
import com.funambol.storage.QueryResult;
import com.funambol.storage.Table;
import com.funambol.storage.Tuple;
import com.funambol.util.Log;
import com.funambol.util.bus.BusMessage;
import com.funambol.util.bus.BusMessageHandler;
import com.funambol.util.bus.BusService;
import java.util.Vector;


public class SourceThumbnailsViewController {

    private static final String TAG_LOG = "SourceThumbnailsViewController";

    private Customization customization;
    private AppSyncSource appSource;

    private SourceThumbnailsView sourceThumbsView;
    private Vector datedThumbnails = new Vector();

    public SourceThumbnailsViewController(AppSyncSource appSource, Customization customization) {
        this.appSource = appSource;
        this.customization = customization;
    }

    public void setSourceThumbnailsView(SourceThumbnailsView thumbsView) {
        this.sourceThumbsView = thumbsView;
    }

    public void enable() {
        // TODO FIXME
    }

    public void disable() {
        // TODO FIXME
    }

    public void setSelected(boolean selected) {
        // TODO FIXME
    }

    public void attachToSync() {
        // TODO FIXME
    }

    public void bindToData() {
        if(Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "bindToData");
        }
        if (sourceThumbsView == null) {
            Log.error(TAG_LOG, "SourceThumbnailsView is null");
            return;
        }
        updateSourceTitle(0);
        Table metadata = appSource.getMetadataTable();
        if (metadata == null) {
            Log.error(TAG_LOG, "Source does not provide metadata " + appSource.getName());
            return;
        }

        // We start listening for events on the bus
        TableEventListener tableEventListener = new TableEventListener(appSource, sourceThumbsView);
        BusService.registerMessageHandler(MetadataBusMessage.class, tableEventListener);

        // Load existing thumbnails
        QueryResult thumbnails = null;
        try {
            metadata.open();
            // Pick the items from the most recent one to the oldest
            thumbnails = metadata.query(null, metadata.getColIndexOrThrow(
                    MediaMetadata.METADATA_LAST_MOD), false);

            int count = 0;
            final int maxCount = customization.getMaxThumbnailsCountInMainScreen();

            while(thumbnails.hasMoreElements()) {
                Tuple row = thumbnails.nextElement();
                if(count < maxCount) {
                    String thumbPath = row.getStringField(metadata.getColIndexOrThrow(
                            MediaMetadata.METADATA_THUMB1_PATH));
                    Long lastMod = row.getLongField(metadata.getColIndexOrThrow(
                            MediaMetadata.METADATA_LAST_MOD));
                    if(Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Adding thumbnail with path: " + thumbPath);
                    }
                    ThumbnailView thumbView = sourceThumbsView.createThumbnailView();
                    thumbView.setThumbnail(thumbPath);

                    DatedThumbnailView datedView = new DatedThumbnailView(
                            thumbView, lastMod.longValue());
                    addDatedThumbnail(datedView, true);
                }
                count++;
                updateSourceTitle(count);
            }
        } catch (Exception ex) {
            // We cannot access the thumbnails, how do we handle
            // this error?
            Log.error(TAG_LOG, "Cannot load thumbnails", ex);
        } finally {
            if (thumbnails != null) {
                try {
                    thumbnails.close();
                } catch(Exception e) {
                }
            }
            try {
                metadata.close();
            } catch (Exception e) {
            }
        }
    }

    private void addDatedThumbnail(DatedThumbnailView datedView) {
        addDatedThumbnail(datedView, false);
    }

    private void addDatedThumbnail(DatedThumbnailView datedView, boolean isMostRecent) {
        int index;
        if(isMostRecent) {
            // We alreay know this is the most recent thumbnail
            index = 0;
        } else {
            // Find a proper index for the given thumbnail
            index = datedThumbnails.size();
            for(int i = 0; i < datedThumbnails.size(); i++) {
                DatedThumbnailView view = (DatedThumbnailView)datedThumbnails.elementAt(i);
                if(datedView.getTimestamp() > view.getTimestamp()) {
                    index = i;
                    break;
                }
            }
        }
        datedThumbnails.insertElementAt(datedView, index);
        sourceThumbsView.addThumbnail(datedView.getView(), index);
    }

    private void updateSourceTitle(int count) {
        StringBuffer title = new StringBuffer();
        title.append(appSource.getName().toUpperCase());
        if(count > 0) {
            title.append(" (").append(count).append(")");
        }
        sourceThumbsView.setTitle(title.toString());
    }

    private class TableEventListener implements BusMessageHandler {

        private AppSyncSource source;
        private SourceThumbnailsView sourceView;

        public TableEventListener(AppSyncSource source, SourceThumbnailsView sourceView) {
            this.source = source;
            this.sourceView = sourceView;
        }

        public void receiveMessage(BusMessage message) {

            Log.debug("CARLO", "receiveMessage: " + message);

            if(!(message instanceof MetadataBusMessage)) {
                // Invalid message type, discard it
                return;
            }
            MetadataBusMessage metadataMessage = (MetadataBusMessage)message;

            Log.debug("CARLO", "metadataMessage.getSource(): " + metadataMessage.getSource());
            Log.debug("CARLO", "source: " + source);

            Log.debug("CARLO", "metadataMessage.getAction(): " + metadataMessage.getAction());
            
            if(metadataMessage.getSource() == source) {
                if(metadataMessage.getAction() == MetadataBusMessage.ACTION_INSERTED) {
                    if(Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "New metadata inserted");
                    }
                    Table metadata = source.getMetadataTable();
                    // An item was added
                    Tuple item = (Tuple)message.getMessage();
                    Long lastMod = item.getLongField(metadata.getColIndexOrThrow(
                            MediaMetadata.METADATA_LAST_MOD));

                    ThumbnailView thumbView = sourceView.createThumbnailView();
                    String thumbPath = item.getStringField(metadata
                            .getColIndexOrThrow(MediaMetadata.METADATA_THUMB1_PATH));
                    thumbView.setThumbnail(thumbPath);

                    DatedThumbnailView datedView = new DatedThumbnailView(
                            thumbView, lastMod.longValue());
                    addDatedThumbnail(datedView);
                }
            }
        }
    }

    private class DatedThumbnailView {

        private ThumbnailView view;
        private long timestamp;

        public DatedThumbnailView(ThumbnailView view, long timestamp) {
            this.view = view;
            this.timestamp = timestamp;
        }

        public ThumbnailView getView() {
            return view;
        }

        public long getTimestamp() {
            return timestamp;
        }
    }

}

