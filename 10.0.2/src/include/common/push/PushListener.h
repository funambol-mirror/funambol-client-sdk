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


#ifndef INCL_PUSH_LISTENER
#define INCL_PUSH_LISTENER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/**
 * Listener class for push notifications and events.
 * Default implementation is empty.
 * Clients should derivate a classs from this one, register it 
 * for notifications (calling CTPService::registerPushListener(*this))
 * and then override the method onNotificationReceived().
 */
class PushListener {

public:

    //Contructor
    PushListener() {}
    
    virtual ~PushListener() {}
    
    /**
     * This method is called when a push notification has arrived.
     * Clients should override this method and implement 
     * desired actions for the push message.
     * @param serverURIList  ArrayList of StringBuffer: they are the serverURI names
     *                       requested by the Server through the push message.
     */
    virtual void onNotificationReceived(const ArrayList& /* serverURIList */) {}
    
    /**
     * Method called when a CTP error occurs.
     * Clients should override this method and implement 
     * desired actions for the each error.
     * @param errorCode       the CTP error code (one of CtpError)
     * @param additionalInfo  [optional] further information about the error
     */
    virtual void onCTPError(const int errorCode, const int additionalInfo = 0) {}
};


END_NAMESPACE

/** @endcond */
#endif

