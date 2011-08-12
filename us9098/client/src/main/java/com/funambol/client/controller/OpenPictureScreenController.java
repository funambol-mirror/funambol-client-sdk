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

import com.funambol.client.ui.OpenItemScreen;
import com.funambol.util.StringUtil;

/** 
 * This is a generic interface for screens where individual items are 
 * displayed in more details. There will be subinterfaces for different
 * item types.
 */

public class OpenPictureScreenController extends OpenItemScreenController {

    public void initScreen(OpenItemScreen screen) {
        super.initScreen(screen);
        
        String title = localization.getLanguage("open_i_of_n_pictures");
        title = StringUtil.replaceAll(title, "__I__", String.valueOf(getPosition() + 1));
        title = StringUtil.replaceAll(title, "__N__", String.valueOf(getTotal()));
        
        screen.setTitle(title);
        String halluxPath = getPreviewPath();
        // If the path is still remote, we just ignore its value
        if (halluxPath.startsWith("file://") && halluxPath.length() > 7) {
            halluxPath = halluxPath.substring(7);
        } else if (!halluxPath.startsWith("/")) {
            halluxPath = null;
        }
        screen.setHalluxnailPath(halluxPath);
    }
}
