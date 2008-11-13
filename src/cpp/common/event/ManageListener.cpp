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

#include "event/ManageListener.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

//------------------------------------------------------------- Local declarations

class DestroyManageListener{
public:
    DestroyManageListener() {           };
    ~DestroyManageListener(){ ManageListener::releaseAllListeners(); }
};

DestroyManageListener destroyManageListener;

/** 
 * ArrayElement container for the Listeners. It does not clone the inner
 * Listener pointer, nor deletes it. Cloning the element does not clone
 * the Listener, which is managed by ManageListener.
 * The only method which deletes the Listener is set(), to replace it
 * with the new one.
 */
class ListenerElement: public ArrayElement {

  public:

    ListenerElement(Listener *_l) : l(_l) {} ;
    ListenerElement(ListenerElement &other) : l(other.l) {} ;

    ~ListenerElement() {}

    void set(Listener *_l) { delete l; l = _l; };
    Listener* get() { return l; };

    ArrayElement* clone() {return new ListenerElement(*this);};

  private:

    Listener *l;
};


/* Static Variables */

ManageListener * ManageListener::instance = 0;

/* Release all the listeners for the given list */
void releaseListeners(ArrayList &list) {
    ListenerElement *l=NULL;

    for(l = (ListenerElement*)list.front(); l; l = (ListenerElement*)list.next()) {
        l->set(NULL);
    }
    list.clear();
}

//-------------------------------- Private Methods ------------------------------

/* Release all the listeners on all the lists */
ManageListener::~ManageListener() {
    releaseListeners(synclisteners);
    releaseListeners(transportlisteners);
    releaseListeners(syncstatuslisteners);
    releaseListeners(syncitemlisteners);
    releaseListeners(syncsourcelisteners);
}

/*
 * Search for the given listener in list.
 * 
 * @return the pointer to the element with the same name, 
 *         or NULL otherwise.
 */
Listener *ManageListener::lookupListener(const char* name, ArrayList &list) {
    ListenerElement *l=NULL;

    for(l = (ListenerElement*)list.front(); l; l = (ListenerElement*)list.next()) {
        if (l->get()->getName() == name) {
            return l->get();
        }
    }
    return NULL;
}

/* Set a new listener, replacing an existen one or adding it to the list. */
bool ManageListener::setListener(Listener* listener, ArrayList &list) {
    ListenerElement *l=NULL;

    for(l = (ListenerElement*)list.front(); l; l = (ListenerElement*)list.next()) {
        if (l->get()->getName() == listener->getName()) {
            l->set(listener);
            return false;       // Element already in the list, just change it
        }
    }

    // Not found, add it to the list
    ListenerElement newElement(listener);
    list.add(newElement);
    return true;
}


/* Unset a listener, referenced by name. If the listener with that name is
 * not found, it does nothing. */
void ManageListener::unsetListener(const char* name, ArrayList &list) {
    for(int i=0; i<list.size(); i++) {
        ListenerElement *l = dynamic_cast<ListenerElement*>(list[i]);
        if (l->get()->getName() == name) {
            l->set(NULL);
            list.removeElementAt(i);
        }
    }
}


//--------------------------------- Public Methods ------------------------------

/*
 * Get, or create, ManageListener instance
 */
ManageListener& ManageListener::getInstance() {
    if(instance == NULL) {
        instance = new ManageListener();
    }
    return *instance;
}

/*
 * Release all the listeners and the singleton instance.
 */
void ManageListener::releaseAllListeners() {
    if (instance) {
        delete instance;
        instance = NULL;
    }
}

//
// Get listeners (return internal pointer):
//
SyncListener* ManageListener::getSyncListener(const char *name) {
    return dynamic_cast<SyncListener*>(lookupListener(name, synclisteners));
}
TransportListener* ManageListener::getTransportListener(const char *name) {
    return dynamic_cast<TransportListener*>(lookupListener(name, transportlisteners));
}
SyncSourceListener* ManageListener::getSyncSourceListener(const char *name) {
    return dynamic_cast<SyncSourceListener*>(lookupListener(name, syncsourcelisteners));
}
SyncItemListener* ManageListener::getSyncItemListener(const char *name) {
    return dynamic_cast<SyncItemListener*>(lookupListener(name, syncitemlisteners));
}
SyncStatusListener* ManageListener::getSyncStatusListener(const char *name) {
    return dynamic_cast<SyncStatusListener*>(lookupListener(name, syncstatuslisteners));
}

// Get listeners by position.
SyncListener* ManageListener::getSyncListener(int pos) {
    ListenerElement* le = static_cast<ListenerElement*>(synclisteners[pos]);
    return static_cast<SyncListener*>(le->get());
}
TransportListener* ManageListener::getTransportListener(int pos) {
    ListenerElement* le = static_cast<ListenerElement*>(transportlisteners[pos]);
    return static_cast<TransportListener*>(le->get());
}
SyncSourceListener* ManageListener::getSyncSourceListener(int pos) {
    ListenerElement* le = static_cast<ListenerElement*>(syncsourcelisteners[pos]);
    return static_cast<SyncSourceListener*>(le->get());
}
SyncItemListener* ManageListener::getSyncItemListener(int pos) {
    ListenerElement* le = static_cast<ListenerElement*>(syncitemlisteners[pos]);
    return static_cast<SyncItemListener*>(le->get());
}
SyncStatusListener* ManageListener::getSyncStatusListener(int pos) {
    ListenerElement* le = static_cast<ListenerElement*>(syncstatuslisteners[pos]);
    return static_cast<SyncStatusListener*>(le->get());
}


//
// Set listeners:
//
void ManageListener::setSyncListener(SyncListener* listener) {
    setListener(listener, synclisteners);
}

void ManageListener::setTransportListener(TransportListener* listener) {
    setListener(listener, transportlisteners);
}

void ManageListener::setSyncSourceListener(SyncSourceListener* listener) {
    setListener(listener, syncsourcelisteners);
}

void ManageListener::setSyncItemListener(SyncItemListener* listener) {
    setListener(listener, syncitemlisteners);
}

void ManageListener::setSyncStatusListener(SyncStatusListener* listener) {
    setListener(listener, syncstatuslisteners);
}

//
// Unset listeners:
//
void ManageListener::unsetSyncListener(const char *name) {
    unsetListener(name, synclisteners);
}

void ManageListener::unsetTransportListener(const char *name) {
    unsetListener(name, transportlisteners);
}

void ManageListener::unsetSyncSourceListener(const char *name) {
    unsetListener(name, syncsourcelisteners);
}

void ManageListener::unsetSyncItemListener(const char *name) {
    unsetListener(name, syncitemlisteners);
}

void ManageListener::unsetSyncStatusListener(const char *name) {
    unsetListener(name, syncstatuslisteners);
}


END_NAMESPACE

