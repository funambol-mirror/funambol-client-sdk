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
     * This command adds a new picture locally, given the local filename.
     *
     * @example AddPicture("picture.jpg");
     *
     */
    public static final String ADD_PICTURE = "AddPicture";

    /**
     * This command adds a new video locally, given the local filename.
     *
     * @example AddVideo("video.3gp");
     *
     */
    public static final String ADD_VIDEO = "AddVideo";

    /**
     * This command adds a new file locally, given the local filename.
     *
     * @example AddFile("filename.txt");
     *
     */
    public static final String ADD_FILE = "AddFile";
    
    /**
     * This command adds a new picture on the server, given the local filename.
     *
     * @example AddPictureOnServer("picture.jpg");
     *
     */
    public static final String ADD_PICTURE_ON_SERVER = "AddPictureOnServer";

    /**
     * This command adds a new video on the server, given the local filename.
     *
     * @example AddVideoOnServer("video.3gp");
     *
     */
    public static final String ADD_VIDEO_ON_SERVER = "AddVideoOnServer";

    /**
     * This command adds a file video on the server, given the local filename.
     *
     * @example AddFileOnServer("filename.txt");
     *
     */
    public static final String ADD_FILE_ON_SERVER = "AddFileOnServer";

    /**
     * This command deletes a local picture, given the filename.
     *
     * @example DeletePicture("picture.jpg");
     *
     */
    public static final String DELETE_PICTURE = "DeletePicture";

    /**
     * This command deletes a local video, given the filename.
     *
     * @example DeleteVideo("video.3gp");
     *
     */
    public static final String DELETE_VIDEO = "DeleteVideo";

    /**
     * This command deletes a local file, given the filename.
     *
     * @example DeleteFile("filename.txt");
     *
     */
    public static final String DELETE_FILE = "DeleteFile";

    /**
     * This command deletes a picture from the server, given the filename.
     *
     * @example DeletePictureOnServer("picture.jpg");
     *
     */
    public static final String DELETE_PICTURE_ON_SERVER = "DeletePictureOnServer";

    /**
     * This command deletes a video from the server, given the filename.
     *
     * @example DeleteVideoOnServer("video.3gp");
     *
     */
    public static final String DELETE_VIDEO_ON_SERVER = "DeleteVideoOnServer";

    /**
     * This command deletes a file from the server, given the filename.
     *
     * @example DeleteFileOnServer("filename.txt");
     *
     */
    public static final String DELETE_FILE_ON_SERVER = "DeleteFileOnServer";

    /**
     * This command deletes all the local pictures.
     *
     * @example DeleteAllPictures();
     *
     */
    public static final String DELETE_ALL_PICTURES = "DeleteAllPictures";

    /**
     * This command deletes all the local videos.
     *
     * @example DeleteAllVideos();
     *
     */
    public static final String DELETE_ALL_VIDEOS = "DeleteAllVideos";

    /**
     * This command deletes all the local files.
     *
     * @example DeleteAllFiles();
     *
     */
    public static final String DELETE_ALL_FILES = "DeleteAllFiles";

    /**
     * This command deletes all the pictures from the server.
     *
     * @example DeleteAllPicturesOnServer();
     *
     */
    public static final String DELETE_ALL_PICTURES_ON_SERVER = "DeleteAllPicturesOnServer";

    /**
     * This command deletes all the videos from the server.
     *
     * @example DeleteAllVideosOnServer();
     *
     */
    public static final String DELETE_ALL_VIDEOS_ON_SERVER = "DeleteAllVideosOnServer";

    /**
     * This command deletes all the files from the server.
     *
     * @example DeleteAllFilesOnServer();
     *
     */
    public static final String DELETE_ALL_FILES_ON_SERVER = "DeleteAllFilesOnServer";

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
     * This command asserts on the number of pictures locally available
     *
     * @example CheckPicturesCount(5);
     *
     */
    public static final String CHECK_PICTURES_COUNT = "CheckPicturesCount";

    /**
     * This command asserts on the number of videos locally available
     *
     * @example CheckVideosCount(5);
     *
     */
    public static final String CHECK_VIDEOS_COUNT = "CheckVideosCount";

    /**
     * This command asserts on the number of files locally available
     *
     * @example CheckFilesCount(5);
     *
     */
    public static final String CHECK_FILES_COUNT = "CheckFilesCount";

    /**
     * This command asserts on the number of pictures available on server
     *
     * @example CheckPicturesCountOnServer(5);
     *
     */
    public static final String CHECK_PICTURES_COUNT_ON_SERVER = "CheckPicturesCountOnServer";

    /**
     * This command asserts on the number of videos available on server
     *
     * @example CheckVideosCountOnServer(5);
     *
     */
    public static final String CHECK_VIDEOS_COUNT_ON_SERVER = "CheckVideosCountOnServer";

    /**
     * This command asserts on the number of files available on server
     *
     * @example CheckFilesCountOnServer(5);
     *
     */
    public static final String CHECK_FILES_COUNT_ON_SERVER = "CheckFilesCountOnServer";

    /**
     * This command fills the user quota on server leaving specified free space
     * for pictures (in byte). If user has already less free space available, the
     * command doesn't execute any modification. 
     *
     * @example LeaveNoFreeServerQuotaForPicture("img05.png");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_PICTURE = "LeaveNoFreeServerQuotaForPicture";
    
    /**
     * This command fills the user quota on server leaving specified free space
     * for videos (in byte). If user has already less free space available, the
     * command doesn't execute any modification. 
     *
     * @example LeaveNoFreeServerQuotaForVideo("img05.png");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_VIDEO = "LeaveNoFreeServerQuotaForVideo";

    /**
     * This command fills the user quota on server leaving specified free space
     * for files (in byte). If user has already less free space available, the
     * command doesn't execute any modification.
     *
     * @example LeaveNoFreeServerQuotaForFile("filename.txt");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_FILE = "LeaveNoFreeServerQuotaForFile";

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
     * This command overrides a given picture with the content of another one.
     * Its main purpose it is simulate an update of a picture.
     *
     * @example OverridePictureContent(target.jpg,source.jpg);
     */
    public static final String OVERRIDE_PICTURE_CONTENT = "OverridePictureContent";

    /**
     * This command overrides a given video with the content of another one.
     * Its main purpose it is simulate an update of a video.
     *
     * @example OverrideVideoContent(target.3gp,source.3gp);
     */
    public static final String OVERRIDE_VIDEO_CONTENT = "OverrideVideoContent";

    /**
     * This command overrides a given file with the content of another one.
     * Its main purpose it is simulate an update of a file.
     *
     * @example OverrideFileContent(target.txt,source.txt);
     */
    public static final String OVERRIDE_FILE_CONTENT = "OverrideFileContent";

    /**
     * This command overrides a given picture on the server with the content of another one.
     * Its main purpose it is simulate an update of a picture.
     *
     * @example OverridePictureContentOnServer(target.jpg,source.jpg);
     */
    public static final String OVERRIDE_PICTURE_CONTENT_ON_SERVER = "OverridePictureContentOnServer";

    /**
     * This command overrides a given video on the server with the content of another one.
     * Its main purpose it is simulate an update of a video.
     *
     * @example OverrideVideoContentOnServer(target.3gp,source.3gp);
     */
    public static final String OVERRIDE_VIDEO_CONTENT_ON_SERVER = "OverrideVideoContentOnServer";

    /**
     * This command overrides a given file on the server with the content of another one.
     * Its main purpose it is simulate an update of a file.
     *
     * @example OverrideFileContent(target.txt,source.txt);
     */
    public static final String OVERRIDE_FILE_CONTENT_ON_SERVER = "OverrideFileContentOnServer";
    
    /**
     * This command creates a file in the mediahub folder
     * First parameter is the file name
     * Second parameter is the file size
     *
     * @example CreateFile(testfile.pdf,15000);
     */
    public static final String CREATE_FILE = "CreateFile";

    /**
     * This command rename a file from the mediahub folder
     * First parameter is the old file name
     * Second parameter is the new file name
     *
     * @example RenameFile(testfile1.pdf,testfile2.pdf);
     */
    public static final String RENAME_FILE = "RenameFile";

    /**
     * This command rename a file from the server
     * First parameter is the old file name
     * Second parameter is the new file name
     *
     * @example RenameFileOnServer(testfile1.pdf,testfile2.pdf);
     */
    public static final String RENAME_FILE_ON_SERVER = "RenameFileOnServer";

    /**
     * This command checks the integrity of a file content on both client and
     * server. This can be used to verify that a file content has been
     * uploaded/download correctly
     * First parameter is the file name to check
     *
     * @example CheckFileContentIntegrity(testfile.txt);
     */
    public static final String CHECK_FILE_CONTENT_INTEGRITY = "CheckFileContentIntegrity";
    
}
    

