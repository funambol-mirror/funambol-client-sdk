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


#ifndef INCL_SYNC_ML
#define INCL_SYNC_ML
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/SyncHdr.h"
#include "syncml/core/SyncBody.h"


class SyncML {

     // ------------------------------------------------------------ Private data
    private:
       SyncHdr*  header;
       SyncBody* body;

    // ---------------------------------------------------------- Public data
    public:


        SyncML();
        ~SyncML();

        /**
         * Creates a new SyncML object from header and body.
         *
         * @param header the SyncML header - NOT NULL
         * @param body the SyncML body - NOT NULL
         *
         */
        SyncML(SyncHdr*  header,
               SyncBody* body);

        /**
         * Returns the SyncML header
         *
         * @return the SyncML header
         *
         */
        SyncHdr* getSyncHdr();

        /**
         * Sets the SyncML header
         *
         * @param header the SyncML header - NOT NULL
         *
         */
        void setSyncHdr(SyncHdr* header);

        /**
         * Returns the SyncML body
         *
         * @return the SyncML body
         *
         */
        SyncBody* getSyncBody();

        /**
         * Sets the SyncML body
         *
         * @param body the SyncML body - NOT NULL
         *
         */
        void setSyncBody(SyncBody* body);

        /**
         * Is this message the last one of the package?
         *
         * @return lastMessage
         */
        BOOL isLastMessage();

        /**
         * Sets lastMessage
         *
         * @param lastMessage the new lastMessage value
         *
         */
        void setLastMessage();

};

/** @endcond */
#endif
