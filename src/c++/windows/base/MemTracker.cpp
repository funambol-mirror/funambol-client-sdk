/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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


#include "base/memTracker.h"
#include "base/globalsdef.h"

USE_NAMESPACE


MemTracker::MemTracker(bool useMemTracking) {
    tracking = useMemTracking;
}

MemTracker::~MemTracker() {}


// Add alloc informations to the list.
void MemTracker::addTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum) {

	AllocInfo info;
	strncpy(info.file, fname, MAX_LENGHT_FILE-1);
	info.address = addr;
	info.line	 = lnum;
	info.size	 = asize;

    allocList.add(info);
}


// Remove alloc informations from the list by the given address.
void MemTracker::removeTrack(DWORD addr) {

	int size = allocList.size();
    if (!size)
		return;

    if ( addr == ((AllocInfo*)allocList.front())->address ) {
        allocList.removeElementAt(0);
        return;
    }
    else {
        int i;
	    for (i=1; i<size; i++) {
            if ( addr == ((AllocInfo*)allocList.next())->address ) {
                allocList.removeElementAt(i);
			    break;
            }
	    }
    }
}


// To print final results of memory allocations.
void MemTracker::dumpUnfreed() {

    DWORD totalSize = 0;
	AllocInfo *info;
	int i;

    disableMemTracker();

    int size = allocList.size();
	LOG.debug("-------------------- MEMORY LEAKS: ------------------------");
    LOG.debug("-----------------------------------------------------------");
    LOG.debug("%d leaks found!", size);

    info = (AllocInfo*)allocList.front();
    LOG.debug("addr: %lx - size:%3ld, file: %s:%d", info->address, info->size, info->file, info->line);
	totalSize += info->size;
    for(i=1; i<size; i++) {
		info = (AllocInfo*)allocList.next();
		LOG.debug("addr: %lx - size:%3ld, file: %s:%d", info->address, info->size, info->file, info->line);
		totalSize += info->size;
	}

	LOG.debug("Total Unfreed: %d bytes", totalSize);
    LOG.debug("-----------------------------------------------------------\n");

    allocList.clear();
}


//
// Functions to enable/disable tracking of memory leaks.
// Note: need to disable trackers when calling add/removeTracker
//       to avoid loops into new/delete operators!
//
void MemTracker::enableMemTracker() {
    tracking = true;
}
void MemTracker::disableMemTracker() {
    tracking = false;
}

// Are we tracking memory leaks?
bool MemTracker::isMemTracking() {
    return tracking;
}


// -------------------------------------------------

// not used
ArrayElement* AllocInfo::clone() {
    AllocInfo* ret = new AllocInfo();
    ret->address = address;
    ret->size = size;
    ret->line = line;
    strncpy(ret->file, file, MAX_LENGHT_FILE-1);
    return ret;
}



