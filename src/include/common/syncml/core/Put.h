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


#ifndef INCL_PUT
#define INCL_PUT
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"

#define PUT_COMMAND_NAME "Put"

class Put : public ItemizedCommand {

    private:
        char*  lang;
        char*  COMMAND_NAME;



    // ---------------------------------------------------------- Public data
    public:

        Put();
        ~Put();
        /**
         * Creates a new Put object given its elements.
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp is &lt;NoResponse/&gt; required?
         * @param lang Preferred language
         * @param cred authentication credentials
         * @param meta meta information
         * @param items Item elements - NOT NULL
         *
         */
        Put( CmdID* cmdID,
                    BOOL noResp,
                    char*  lang,
                    Cred* cred,
                    Meta* meta,
                    ArrayList* items ); // items[]



       // ----------------------------------------------------------- Public methods

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
         * Returns the command name
         *
         * @return the command name
         */
         const char* getName();

         ArrayElement* clone();

};

/** @endcond */
#endif
