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

package com.funambol.client.test.media;

/**
 * This component lists all the media-related commands available in the automatic test
 * scripting language.
 */
public interface MediaUserCommands {

    /**
     * This command fills the local storage with fake data leaving only <1M free.
     *
     * @example FillLocalStorage();
     *
     */
    public static final String FILL_LOCAL_STORAGE = "FillLocalStorage";
    
    /**
     * This command fills remove fake data created by FillLocalStorage from local 
     * storage.
     *
     * @example RestoreLocalStorage();
     *
     */
    public static final String RESTORE_LOCAL_STORAGE = "RestoreLocalStorage";

    /**
     * This command fills the user quota on server leaving specified free space
     * for pictures (in byte). If user has already less free space available, the
     * command doesn't execute any modification. 
     *
     * @example LeaveNoFreeServerQuotaForPicture("img05.png");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_MEDIA = "LeaveNoFreeServerQuotaForMedia";
    
    /**
     * This command forces the upload of a given item to fail with a network
     * error. The item can be specified by its name (first parameter) or by its
     * progressive id in the sync (third parameter). The command can also specify after how
     * many byte the failuere shall occur.
     *
     * @example InterrupItemUpload("img01.jpg",1023);
     * @example InterrupItemUpload(*,1023,2);
     */
    public static final String INTERRUPT_ITEM_UPLOAD = "InterruptItemUpload";

    /**
     * This command forces the download of a given item to fail with a network
     * error. The item can be specified by its name (first parameter) or by its
     * progressive id in the sync (third parameter).
     * The command can specify after how many byte the failuere shall
     * occur.
     *
     * @example InterrupItemDowload("img01.jpg",1023);
     * @example InterrupItemDowload(*,1023,3);
     */
    public static final String INTERRUPT_ITEM_DOWNLOAD = "InterruptItemDownload";

    /**
     * This command creates a file in the mediahub folder
     * First parameter is the file name
     * Second parameter is the file size
     *
     * @example CreateFile(testfile.pdf,15000);
     */
    public static final String CREATE_FILE = "CreateFile";

    /**
     * This command deletes all local media of the given type.
     *
     * @example DeleteAllMedia("Pictures");
     *
     */
    public static final String DELETE_ALL_MEDIA = "DeleteAllMedia";

    /**
     * This command deletes all remote media of the given type.
     *
     * @example DeleteAllMediaOnServer("Pictures");
     *
     */
    public static final String DELETE_ALL_MEDIA_ON_SERVER = "DeleteAllMediaOnServer";

    /**
     * This command deletes a local media, given its type and filename.
     *
     * @example DeleteMedia("Pictures","picture.jpg");
     *
     */
    public static final String DELETE_MEDIA = "DeleteMedia";

    /**
     * This command deletes a picture from the server, given its type and filename.
     *
     * @example DeleteMediaOnServer("Pictures","picture.jpg");
     *
     */
    public static final String DELETE_MEDIA_ON_SERVER = "DeleteMediaOnServer";

    /**
     * This command asserts on the number of media locally available (for a
     * given type)
     *
     * @example CheckMediaCount(Pictures,5);
     *
     */
    public static final String CHECK_MEDIA_COUNT = "CheckMediaCount";

    /**
     * This command asserts on the number of media remotely available (for a
     * given type)
     *
     * @example CheckMediaCountOnServer(Pictures,5);
     *
     */
    public static final String CHECK_MEDIA_COUNT_ON_SERVER = "CheckMediaCountOnServer";

    /**
     * This command adds a new media locally, given its type and local filename.
     *
     * @example AddMedia("Pictures","picture.jpg");
     *
     */
    public static final String ADD_MEDIA = "AddMedia";
    /**
     * This command adds a new media on server, given its type and local filename.
     *
     * @example AddMediaOnServer("Pictures","picture.jpg");
     *
     */
    public static final String ADD_MEDIA_ON_SERVER = "AddMediaOnServer";

    /**
     * This command checks the integrity of a media content on both client and
     * server. This can be used to verify that a media content has been
     * uploaded/download correctly
     * First parameter is the media type, second is the file name to check.
     * If a third parameter is present it is the file name on the server.
     *
     * @example CheckMediaContentIntegrity(Pictures,testpic.jpg);
     * @example CheckMediaContentIntegrity(Pictures,testpic_client.jpg,testpic_server.jpg);
     */
    public static final String CHECK_MEDIA_CONTENT_INTEGRITY = "CheckMediaContentIntegrity";

    /**
     * This command overrides a given media with the content of another one of
     * the same type.
     * Its main purpose it is simulate an update of the given media.
     *
     * @example OverrideMediaContent(Pictures,target.jpg,source.jpg);
     */
    public static final String OVERRIDE_MEDIA_CONTENT = "OverrideMediaContent";

    /**
     * This command overrides a given media on the server with the content of another media of the same type.
     * Its main purpose it is simulate an update of a media on server.
     *
     * @example OverrideMediaContentOnServer(Pictures,target.jpg,source.jpg);
     */
    public static final String OVERRIDE_MEDIA_CONTENT_ON_SERVER = "OverrideMediaContentOnServer";

    /**
     * This command renames a media on the device.
     * First parameter is the media type
     * Second parameter is the old name
     * Third parameter is the new name
     *
     * @example RenameMedia(Pictures,testfile1.jpg,testfile2.jpg);
     */
    public static final String RENAME_MEDIA = "RenameMedia";

    /**
     * This command renames a media on server.
     * First parameter is the media type
     * Second parameter is the old name
     * Third parameter is the new name
     *
     * @example RenameMediaOnServer(Pictures,testfile1.jpg,testfile2.jpg);
     */
    public static final String RENAME_MEDIA_ON_SERVER = "RenameMediaOnServer";

    /**
     * Checks the name of a thumbnail at the given position.
     * First parameter is the media type
     * Second parameter is the thumbnail position (0 is the first thumbnail
     * displayed to the left side of the source thumbnails view)
     * Third parameter is the item filename
     *
     * @example CheckThumbnailName(Pictures,1,pic1.jpg);
     */
    public static final String CHECK_THUMBNAIL_NAME = "CheckThumbnailName";
    
}
    

