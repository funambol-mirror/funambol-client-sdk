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

import java.util.Vector;

import com.funambol.client.test.CommandRunner;


public class MediaCommandRunner extends CommandRunner implements MediaUserCommands {

    private static final String TAG_LOG = "MediaCommandRunner";

    public MediaCommandRunner(MediaRobot robot) {
        super(robot);
    }

    public boolean runCommand(String command, Vector pars) throws Throwable {
        if (ADD_MEDIA.equals(command)) {
            addMedia(command, pars);
        } else if (ADD_MEDIA_ON_SERVER.equals(command)) {
            addMediaOnServer(command, pars);
        } else if (DELETE_MEDIA.equals(command)) {
            deleteMedia(command, pars);
        } else if (DELETE_MEDIA_ON_SERVER.equals(command)) {
            deleteMediaOnServer(command, pars);
        } else if (DELETE_ALL_MEDIA.equals(command)) {
            deleteAllMedia(command, pars);
        } else if (DELETE_ALL_MEDIA_ON_SERVER.equals(command)) {
            deleteAllMediaOnServer(command, pars);
        } else if (FILL_LOCAL_STORAGE.equals(command)) {
            fillLocalStorage();
        } else if (RESTORE_LOCAL_STORAGE.equals(command)) {
            restoreLocalStorage();
        } else if (CHECK_MEDIA_COUNT.equals(command)) {
            checkMediaCount(command, pars);
        } else if (CHECK_MEDIA_COUNT_ON_SERVER.equals(command)) {
            checkMediaCountOnServer(command, pars);
        } else if (LEAVE_NO_FREE_SERVER_QUOTA_FOR_MEDIA.equals(command)) {
            leaveNoFreeServerQuota(command, pars);
        } else if (INTERRUPT_ITEM_UPLOAD.equals(command)) {
            interruptItem("sending", command, pars);
        } else if (INTERRUPT_ITEM_DOWNLOAD.equals(command)) {
            interruptItem("receiving", command, pars);
        } else if (OVERRIDE_MEDIA_CONTENT.equals(command)) {
            overrideMediaContent(command, pars);
        } else if (OVERRIDE_MEDIA_CONTENT_ON_SERVER.equals(command)) {
            overrideMediaContentOnServer(command, pars);
        } else if (CREATE_FILE.equals(command)) {
            createFile(command, pars);
        } else if (RENAME_MEDIA.equals(command)) {
            renameFile(command, pars);
        } else if (RENAME_MEDIA_ON_SERVER.equals(command)) {
            renameFileOnServer(command, pars);
        } else if (CHECK_MEDIA_CONTENT_INTEGRITY.equals(command)) {
            checkFileContentIntegrity(command, pars);
        } else if (CHECK_THUMBNAIL_NAME.equals(command)) {
            checkThumbnailName(command, pars);
        } else if (CHECK_DISPLAYED_THUMBNAILS_COUNT.equals(command)) {
            checkDisplayedThumbnailsCount(command, pars);
        } else if (CHECK_THUMBNAILS_COUNT.equals(command)) {
            checkThumbnailsCount(command, pars);
        } else {
            return false;
        }
        return true;
    }

    private MediaRobot getMediaRobot() {
        return (MediaRobot)robot;
    }

    private void addMedia(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String filename = getParameter(args, i++);
        checkArgument(filename, "Missing filename in " + command);
        getMediaRobot().addMedia(type, filename);
    }

    private void addMediaOnServer(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String filename = getParameter(args, i++);
        checkArgument(filename, "Missing filename in " + command);
        getMediaRobot().addMediaOnServer(type, filename);
    }

    private void deleteMedia(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String filename = getParameter(args, i++);
        checkArgument(filename, "Missing filename in " + command);
        getMediaRobot().deleteMedia(type, filename);
    }
    
    private void deleteMediaOnServer(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String filename = getParameter(args, i++);
        checkArgument(filename, "Missing filename in " + command);
        getMediaRobot().deleteMediaOnServer(type, filename);
    }

    private void deleteAllMedia(String command, Vector args) throws Throwable {
        String type = getParameter(args, 0);
        getMediaRobot().deleteAllMedia(type);
    }

    private void deleteAllMediaOnServer(String command, Vector args) throws Throwable {
        String type = getParameter(args, 0);
        getMediaRobot().deleteAllMediaOnServer(type);
    }

    private void checkMediaCount(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String number = getParameter(args, i++);
        checkArgument(number, "Missing expected count in " + command);
        getMediaRobot().checkMediaCount(type, Integer.parseInt(number));
    }

    private void checkMediaCountOnServer(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String number = getParameter(args, i++);
        checkArgument(number, "Missing expected count in " + command);
        getMediaRobot().checkMediaCountOnServer(type, Integer.parseInt(number));
    }

    private void interruptItem(String phase, String command, Vector args) throws Throwable {
        String name = getParameter(args, 0);
        String pos = getParameter(args, 1);
        String itemIdx = getParameter(args, 2);
        int p;
        if (pos != null) {
            p = Integer.parseInt(pos);
        } else {
            p = -1;
        }

        int idx;
        if (itemIdx != null) {
            idx = Integer.parseInt(itemIdx);
        } else {
            idx = -1;
        }
        getMediaRobot().interruptItem(phase, name, p, idx);
    }

    private void leaveNoFreeServerQuota(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String filename = getParameter(args, i++);
        checkArgument(filename, "Missing filename in " + command);
        getMediaRobot().leaveNoFreeServerQuota(type, filename);
    }

    private void overrideMediaContent(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String targetFileName = getParameter(args, i++);
        checkArgument(targetFileName, "Missing target filename in " + command);
        String sourceFileName = getParameter(args, i++);
        checkArgument(targetFileName, "Missing source filename in " + command);
        getMediaRobot().overrideMediaContent(type, targetFileName, sourceFileName);
    }

    private void overrideMediaContentOnServer(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String targetFileName = getParameter(args, i++);
        checkArgument(targetFileName, "Missing target filename in " + command);
        String sourceFileName = getParameter(args, i++);
        checkArgument(targetFileName, "Missing source filename in " + command);
        getMediaRobot().overrideMediaContentOnServer(type, targetFileName, sourceFileName);
    }
    
    private void fillLocalStorage() {
        getMediaRobot().fillLocalStorage();
    }
    
    private void restoreLocalStorage() {
        getMediaRobot().restoreLocalStorage();
    }

    private void createFile(String command, Vector args) throws Throwable {
        String fileName = getParameter(args, 0);
        checkArgument(fileName, "Missing filename in " + command);
        String fileSizeString = getParameter(args, 1);
        checkArgument(fileName, "Missing file size in " + command);
        long fileSize = Long.parseLong(fileSizeString);
        getMediaRobot().createFile(fileName, fileSize);
    }

    private void renameFile(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String oldFileName = getParameter(args, i++);
        checkArgument(oldFileName, "Missing oldFileName in " + command);
        String newFileName = getParameter(args, i++);
        checkArgument(newFileName, "Missing newFileName in " + command);

        getMediaRobot().renameFile(type, oldFileName, newFileName);
    }

    private void renameFileOnServer(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String oldFileName = getParameter(args, i++);
        checkArgument(oldFileName, "Missing oldFileName in " + command);
        String newFileName = getParameter(args, i++);
        checkArgument(newFileName, "Missing newFileName in " + command);

        getMediaRobot().renameFileOnServer(type, oldFileName, newFileName);
    }

    private void checkFileContentIntegrity(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String fileNameClient = getParameter(args, i++);
        String fileNameServer = getParameter(args, i++);
        checkArgument(fileNameClient, "Missing fileNameClient in " + command);
        if(fileNameServer == null) {
            fileNameServer = fileNameClient;
        }
        getMediaRobot().checkFileContentIntegrity(type, fileNameClient, fileNameServer);
    }

    private void checkThumbnailName(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String position = getParameter(args, i++);
        String fileName = getParameter(args, i++);

        checkArgument(type, "Missing type in " + command);
        checkArgument(position, "Missing position in " + command);
        checkArgument(fileName, "Missing fileName in " + command);

        int intPosition = Integer.parseInt(position);

        getMediaRobot().checkThumbnailName(type, intPosition, fileName);
    }

    private void checkDisplayedThumbnailsCount(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String count = getParameter(args, i++);

        checkArgument(type, "Missing type in " + command);
        checkArgument(count, "Missing count in " + command);

        int intCount = Integer.parseInt(count);

        getMediaRobot().checkDisplayedThumbnailsCount(type, intCount);
    }

    private void checkThumbnailsCount(String command, Vector args) throws Throwable {
        int i = 0;
        String type = getParameter(args, i++);
        String count = getParameter(args, i++);

        checkArgument(type, "Missing type in " + command);
        checkArgument(count, "Missing count in " + command);

        int intCount = Integer.parseInt(count);

        getMediaRobot().checkThumbnailsCount(type, intCount);
    }
    
}

