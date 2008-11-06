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


#ifndef INCL_GET
#define INCL_GET
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"

#define GET_COMMAND_NAME "Get"

class Get : public ItemizedCommand {

    private:
        char*  lang;
        char*  COMMAND_NAME;

    public:

        Get();
        ~Get();

        /**
         * Creates a new Get object with the given command identifier,
         * noResponse, language, credential, meta and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp true if no response is required
         * @param lang the preferred language for results data
         * @param cred the authentication credential
         * @param meta the meta information
         * @param items the array of item - NOT NULL
         *
         */
        Get(CmdID* cmdID,
            BOOL noResp,
            char*  lang,
            Cred* cred,
            Meta* meta,
            ArrayList* items);


        /**
         * Returns the preferred language
         *
         * @return the preferred language
         *
         */
        const char* getLang();

        /**
         * Sets the preferred language
         *
         * @param lang new preferred language
         */
        void setLang(const char* lang);

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
