/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include "base/memTracker.h"


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
    tracking = TRUE;
}
void MemTracker::disableMemTracker() {
    tracking = FALSE;
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



