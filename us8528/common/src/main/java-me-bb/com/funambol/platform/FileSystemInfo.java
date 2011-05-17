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

package com.funambol.platform;

import com.funambol.util.StringUtil;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

import java.io.IOException;
import java.util.Enumeration;

/**
 * Provides informations about the File System of this specific platform
 */
public class FileSystemInfo {

    private static final String TAG_LOG = "FileSystemInfo";

    private long availableBlocks;
    private long usableBlocks;
    
    public FileSystemInfo(String path) throws IOException {
        
        FileConnection location = (FileConnection)Connector.open(path, Connector.READ);
        
        long currentlyAvailableBytes = location.availableSize();
        long totalUsableBytes = location.totalSize();
        
        availableBlocks = currentlyAvailableBytes / getBlockSize();
        usableBlocks = totalUsableBytes / getBlockSize();
        
        location.close();
    }

    public int getBlockSize() {
        return 1024;
    }

    public long getAvailableBlocks() {
        return availableBlocks;
    }

    public long getTotalUsableBlocks() {
        return usableBlocks;
    }

    /**
     * Returns the sd card root folder
     */
    public static String getSDCardRoot() {
        return "file:///SDCard";
    }

    /**
     * Returns whether the sd card is available
     */
    public static boolean isSDCardAvailable() {
        String root = null;
        Enumeration e = FileSystemRegistry.listRoots();
        while (e.hasMoreElements()) {
            root = (String) e.nextElement();
            if(StringUtil.equalsIgnoreCase(root, "sdcard/")) {
                //device has a microSD inserted
                return true;
            }
        }
        //device has NOT a microSD inserted
        return false;
    }
}

