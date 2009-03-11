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

#include "base/Log.h"
#include "event/SyncItemListener.h"

BEGIN_NAMESPACE

void logEvent(const char* msg, SyncItemEvent& event) {
    LOG.debug("%s: (%s, %s, %s)", msg, event.getItemKey(), event.getSourceName(), event.getSourceURI());
}

// listen for the Item added by Server Event
void SyncItemListener::itemAddedByServer(SyncItemEvent& event) {
    logEvent("item added by server", event);
}

// listen for the Item  deleted by Server Event
void SyncItemListener::itemDeletedByServer(SyncItemEvent& event) {
    logEvent("item deleted by server", event);
}

// listen for the Item updated by Server Event
void SyncItemListener::itemUpdatedByServer(SyncItemEvent& event) {
    logEvent("item updated by server", event);
}

// listen for the Item added by Client Event
void SyncItemListener::itemAddedByClient(SyncItemEvent& event) {
    logEvent("item added by client", event);
}

// listen for the Item deleted by Client Event
void SyncItemListener::itemDeletedByClient(SyncItemEvent& event) {
    logEvent("item deleted by client", event);
}

// listen for the Item updated by Server Event
void SyncItemListener::itemUpdatedByClient(SyncItemEvent& event) {
    logEvent("item updated by server", event);
}

END_NAMESPACE