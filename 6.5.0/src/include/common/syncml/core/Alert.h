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


#ifndef INCL_ALERT
#define INCL_ALERT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

#include "syncml/core/ItemizedCommand.h"
#include "syncml/core/Cred.h"
#include "syncml/core/CmdID.h"
#include "syncml/core/ItemizedCommand.h"

#define ALERT_COMMAND_NAME "Alert"

class Alert : public ItemizedCommand {

     // ------------------------------------------------------------ Private data
    private:
       int data;
       char*  COMMAND_NAME;
       void initialize();

    // ---------------------------------------------------------- Public data
    public:

        Alert();
        ~Alert();

        /**
         * Creates a new Alert object with the given command identifier,
         * noResponse, authentication credential, alert code and array of item
         *
         * @param cmdID command identifier - NOT NULL
         * @param noResp is true if no response is required
         * @param cred the authentication credential
         * @param data the code of Alert
         * @param items the array of item - NOT NULL
         *
         */
        Alert(CmdID* cmdID,
              BOOL noResp,
              Cred* cred,
              int data,
              ArrayList* items); //Item[]

        /**
         * Gets the alert code
         *
         * @return the alert code
         */
        int getData();

        /**
         * Sets the alert code
         *
         * @param data the alert code
         */
        void setData(int data);

        /**
         * Gets the command name property
         *
         * @return the command name property
         */
        const char* getName();

        ArrayElement* clone();

};

/** @endcond */
#endif
