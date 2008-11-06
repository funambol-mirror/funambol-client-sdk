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

#include "base/util/utils.h"
#include "syncml/core/AlertCode.h"
#include "syncml/core/SyncAlert.h"

/*
 * This class represent a sync alert notified by the server.
 */

/*
 * Default Constructor
 */
SyncAlert::SyncAlert()
{
    syncType=0;
    contentType=0;
    serverURI=0;
}

/*
 * Destructor
 */
SyncAlert::~SyncAlert()
{
    if(serverURI)
        delete [] serverURI;
}

/*
 * Accessor methods
 */
int SyncAlert::getSyncType() { return syncType; }
int SyncAlert::getContentType () { return contentType; }
const char *SyncAlert::getServerURI () { return serverURI; }

/**
  @brief Set values for the object

  @param sync_type      Sync type (values: 6-10)
  @param content_type   MIME type
  @param len            Server URI lenght
  @param buf            Server URI characters

  @return
 */
int SyncAlert::set(int sync_type, int content_type, const char *uri)
{
    if (sync_type < 6 || sync_type > 10)
        return -1;
    syncType = sync_type+200;
    contentType = content_type;
    serverURI=stringdup(uri);
    return 0;
}

