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


#ifndef INCL_SEARCH
#define INCL_SEARCH
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/Data.h"
#include "syncml/core/Target.h"

#define SEARCH_COMMAND_NAME "Search"

class Search : public AbstractCommand{

     // ------------------------------------------------------------ Private data
    private:
        char*     COMMAND_NAME;
        BOOL        noResults;
        Target*     target   ;
        ArrayList*  sources; // Source[]. It is an ArrayList of SourceArray object. Every one contains a Source object
        char*     lang;
        Data*       data;

    // ---------------------------------------------------------- Public data
    public:

        Search();
        ~Search();

        /**
         * Creates a new Search object.
         *
         * @param cmdID command identifier - NOT NULL
         * @param noResp is &lt;NoResponse/&gt; required?
         * @param noResults is &lt;NoResults/&gt; required?
         * @param cred  authentication credentials
         * @param target target
         * @param sources sources - NOT NULL
         * @param lang preferred language
         * @param meta meta data - NOT NULL
         * @param data contains the search grammar - NOT NULL
         *
         *
         */
        Search(CmdID*      cmdID    ,
               BOOL        noResp   ,
               BOOL        noResults,
               Cred*       cred     ,
               Target*     target   ,
               ArrayList*  sources  ,
               char*     lang     ,
               Meta*       meta     ,
               Data*       data     );

        /**
         * Returns noResults
         *
         * @return noResults
         *
         */
        BOOL isNoResults();

        /**
         * Sets noResults
         *
         * @param noResults the noResults value
         */
        void setNoResults(BOOL noResults);

        /**
         * Gets the Boolean value of noResults property
         *
         * @return noResults if boolean value is true, otherwise null
         */
        BOOL getNoResults();

        /**
         * Returns target property
         * @return target the Target property
         */
        Target* getTarget();

        /**
         * Sets target property
         *
         * @param target the target property
         */
        void setTarget(Target* target);

        /**
         * Returns command sources
         * @return command sources
         */
        ArrayList* getSources();

        /**
         * Sets command sources
         *
         * @param sources the command sources - NOT NULL
         *
         */
        void setSources(ArrayList* sources);

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
         * @param lang the preferred language
         */
        void setLang(const char* lang);

        /**
         * Returns data
         *
         * @return data
         *
         */
        Data* getData();

        /**
         * Sets data
         *
         * @param data the command's data - NOT NULL
         *
         */
        void setData(Data* data);

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
