/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

#ifndef INCL_FIRE_EVENTS
#define INCL_FIRE_EVENTS
/** @cond DEV */

#include "event/BaseEvent.h"
#include "event/SyncEvent.h"
#include "event/SyncSourceEvent.h"
#include "event/SyncItemEvent.h"
#include "event/SyncStatusEvent.h"
#include "event/TransportEvent.h"
#include "event/constants.h"

/*
 * A set of global functions to fire an event from inside the API.
 */


/*
 * Fire a SyncEvent.
 *
 * @param msg  : the message of sync event
 * @param type : the type of event to fire (see event/constants.h)
 * @return     : TRUE if no errors,
 *               FALSE if syncListener not instantiated, or 'type' wrong
 */
bool fireSyncEvent(const char* msg, int type);

/*
 * Fire a TransportEvent.
 *
 * @param size : the data size
 * @param type : the type of event to fire (see event/constants.h)
 * @return     : TRUE if no errors,
 *               FALSE if transportListener not instantiated, or 'type' wrong
 */
bool fireTransportEvent(unsigned long size, int type);

/*
 * Fire a SyncSourceEvent.
 *
 * @param sourceURI : the source being synchronized
 * @param sourceName: the source name being synchronized
 * @param mode      : type of the performed sync
 * @param data      : data information returned
 * @param type      : the type of event to fire (see event/constants.h)
 * @return          : TRUE if no errors,
 *                    FALSE if syncsourceListener not instantiated, or 'type' wrong
 */
bool fireSyncSourceEvent(const char* sourceURI, const char* sourceName, SyncMode mode, int data, int type);

/*
 * Fire a SyncItemEvent.
 *
 * @param sourceURI : the source the item belongs to
 * @param name      : the source name
 * @param itemKey   : the item key (GUID from the server)
 * @param type      : the type of event to fire (see event/constants.h)
 * @return          : TRUE if no errors,
 *                    FALSE if syncitemListener not instantiated, or 'type' wrong
 */
bool fireSyncItemEvent(const char* sourceURI, const char* name, const WCHAR* itemKey, int type);

/*
 * Fire a SyncStatusEvent.
 *
 * @param command   : the command the status relates to
 * @param statusCode: the status code
 * @param name      : the source name
 * @param uri       : the source uri
 * @param itemKey   : the key of the item this status relates to if it is in response of a modification command
 * @param type      : the type of event to fire (see event/constants.h)
 * @return          : TRUE if no errors,
 *                    FALSE if syncstatusListener not instantiated, or 'type' wrong
 */
bool fireSyncStatusEvent(const char* command, int statusCode, const char* name, const char* uri, const WCHAR* itemKey, int type);


/** @endcond */
#endif
