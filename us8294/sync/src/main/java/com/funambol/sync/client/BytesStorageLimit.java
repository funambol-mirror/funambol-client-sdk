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
package com.funambol.sync.client;

/**
 * This class represents the criteria used to limit the local storage space on a
 * device on the basis of keeping free a given amount of bytes of storage usable
 * by applications.
 * 
 * @version $Id$
 */
public class BytesStorageLimit extends StorageLimit {
    
    private int freeBytes;
    
    public BytesStorageLimit(int bytes) {
        this.freeBytes = bytes;
    }
    
    /**
     * @see com.funambol.sync.client.StorageLimit#currentSituation(String, long, long, int)
     */
    protected static String currentSituation(String path, long currentlyAvailableBlocks, long totalUsableBlocks, int blockSize) {
        return "There are currently " + currentlyAvailableBlocks * blockSize + " bytes" +
               " free on " + path +
               " out of a total of " + totalUsableBlocks * blockSize + " bytes" +
               " that can be used by applications";
    }

    /**
     * @see com.funambol.sync.client.StorageLimit#criterion(long, int)
     */
    protected String criterion(long totalUsableBlocks, int blockSize) {
        return "At least " + freeBytes + " bytes must always be kept available";
    }

    /**
     * @see com.funambol.sync.client.StorageLimit#isOK(long, long, long, int)
     */
    protected boolean isOK(long size, long currentlyAvailableBlocks,
            long totalUsableBlocks, int blockSize) {
 
        long availableBytes = currentlyAvailableBlocks * blockSize;
        return (availableBytes - size >= freeBytes);
    }

    /**
     * @see com.funambol.sync.client.StorageLimit#toString()
     */
    public String toString() {
        return freeBytes + " bytes free";
    }
}