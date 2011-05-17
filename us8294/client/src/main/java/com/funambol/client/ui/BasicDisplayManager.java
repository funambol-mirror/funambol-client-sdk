/*
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

package com.funambol.client.ui;

import com.funambol.client.controller.NotificationData;

/**
 * This interface includes all basic functions of a DisplayManager
 * implementation that are currently shared between Android and BlackBerry
 * versions of DisplayManager.
 */
public interface BasicDisplayManager {

    /**
     * int value related to the infinitive time to wait before dismissing a
     * screen or a dialog
     */
    public static final long NO_LIMIT = -1;

    /**
     * Helper function to prompt the user for an accept/deny answer
     * Helper function to prompt the user for a yes/no answer
     * @param question the question to be displayed
     * @param defaultyes the default otpion
     * @param timeToWait time to wait before dismissing the dialog in milliseconds
     * @return True on accept
     */
    public boolean askAcceptDenyQuestion(String question, boolean defaultyes, long timeToWait);

    /**
     * Hide a screen pulling it to the background
     * @param screen The screen to be hidden
     */
    public void hideScreen(Screen screen) throws Exception;


    /**
     * Prompt a message to continue or cancel some pending process
     * @param message the message to be prompted
     * @return boolean true if the user selects to continue, false otherwise
     */
    public boolean promptNext(String message);

    /**
     * Put the application in foreground (Active satus)
     */
    public void toForeground();

    /**
     * Put the application in background (unactive state)
     */
    public void toBackground();

    /**
     * Load the browser to the given url
     * To be implemented.
     * @param url the url to be set on the browser
     */
    public void loadBrowser(String url);


    /**
     * Display a notification
     * @param notificationData the notification data
     */
    public void showNotification(NotificationData notificationData);
}
