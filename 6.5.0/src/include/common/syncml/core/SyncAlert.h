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


#ifndef INCL_SYNC_ALERT
#define INCL_SYNC_ALERT
/** @cond DEV */

#include "base/fscapi.h"

/*
 * This class represent a sync alert notified by the server.
 */

class SyncAlert {

    public:

        /*
         * Default Constructor
         */
        SyncAlert();

        /*
         * Destructor
         */
        ~SyncAlert();

        /*
         * Accessor methods
         */
        int getSyncType( void );
        int getContentType ( void );
        const char *getServerURI ( void );

    private:
        int syncType;
        int contentType;
        char *serverURI;

        /**
          @brief Set values for the object

          @param sync_type      Sync type (values: 6-10)
          @param content_type   MIME type
          @param len            Server URI lenght
          @param buf            Server URI characters

          @return
         */
        int set(int sync_type, int content_type, const char *uri);

        friend class SyncNotification;
};

/** @endcond */
#endif

