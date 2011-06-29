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

package com.funambol.client.controller;

import com.funambol.client.localization.Localization;
import com.funambol.client.ui.BasicDisplayManager;

/**
 * This class includes all basic functions of a DialogController
 * implementation that are currently shared between Android and BlackBerry
 * versions of DialogController.
 */
public abstract class BasicDialogController {

    protected Localization localization;

    /**
     * Prompts an alert with 2 choices on the screen.
     *
     * @param message the message to be displayed
     * @param defaultyes the default parameter in the selection
     * @return boolean true is the user accepted, false otherwise
     */
    public boolean askAcceptDenyQuestion(String message, boolean defaultyes) {
        return getBasicDisplayManager().askAcceptDenyQuestion(message, defaultyes, -1);
    }

    /**
     * This creates an N-button generic dialog box and waits for the user to
     * select one option.
     *
     * @param message The message for the dialog box
     * @param labels The labels for the buttons
     * @return the index of the chosen option or -1 if something goes wrong
     */
    public abstract int askGenericQuestion(String message, String[] labels);

    /**
     * Show a message to the user, with a classic "Ok" button, and the user must press
     * the button in order to dismiss the dialog
     *
     * @param message text to display
     */
    public void showMessageAndWaitUserConfirmation(String message) {
       String okMessage = localization.getLanguage("dialog_ok");
       askGenericQuestion(message, new String[] { okMessage });
    }

    protected abstract BasicDisplayManager getBasicDisplayManager();

}
