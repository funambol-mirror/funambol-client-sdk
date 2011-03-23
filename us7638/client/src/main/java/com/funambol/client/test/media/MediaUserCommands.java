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
     * This command fill the user quota on server leaving specified free space
     * for pictures (in byte). If user has already less free space available, the
     * command doesn't execute any modification. 
     *
     * @example LeaveNoFreeServerQuotaForPicture("img05.png");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_PICTURE = "LeaveNoFreeServerQuotaForPicture";
    
    /**
     * This command fill the user quota on server leaving specified free space
     * for videos (in byte). If user has already less free space available, the
     * command doesn't execute any modification. 
     *
     * @example LeaveNoFreeServerQuotaForVideo("img05.png");
     *
     */
    public static final String LEAVE_NO_FREE_SERVER_QUOTA_FOR_VIDEO = "LeaveNoFreeServerQuotaForVideo";

}
    

