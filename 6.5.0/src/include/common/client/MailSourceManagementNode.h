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

#ifndef INCL_MAIL_SOURCE_MANAGEMENT_NODE
#define INCL_MAIL_SOURCE_MANAGEMENT_NODE
/** @cond DEV */

#include "spdm/constants.h"
#include "spdm/DeviceManagementNode.h"
#include "spds/MailSyncSourceConfig.h"


class MailSourceManagementNode : public DeviceManagementNode {

    public:
        // ------------------------------------------ Constructors & destructors

        MailSourceManagementNode( const char*     context,
                                  const char*     name   );

        MailSourceManagementNode( const char*     context,
                                  const char*     name   ,
                                  MailSyncSourceConfig& config );

        ~MailSourceManagementNode();

        // ------------------------------------------------------------- Methods

        /**
         * Returns the mail configuration object from the cached value (if
         * refresh is FALSE) or reading it from the DMT store (if refresh is
         * TRUE);
         *
         * @param refresh should the node be read from the DMT ?
         */
        MailSyncSourceConfig& getMailSourceConfig(BOOL refresh);

        /**
         * Sets the given mail source configuration object to the internal
         * cache and into the DMT.
         *
         * @param c the configuration object to store
         */
        void setMailSourceConfig(MailSyncSourceConfig& c);

        ArrayElement* clone();

        // -------------------------------------------------------- Data members
    private:
        MailSyncSourceConfig config;
};

/** @endcond */
#endif
