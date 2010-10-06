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


#ifndef INCL_SEARCH
#define INCL_SEARCH
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/Data.h"
#include "syncml/core/Target.h"

#define SEARCH_COMMAND_NAME "Search"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class Search : public AbstractCommand{

     // ------------------------------------------------------------ Private data
    private:
        char*     COMMAND_NAME;
        bool        noResults;
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
               bool        noResp   ,
               bool        noResults,
               Cred*       cred     ,
               Target*     target   ,
               ArrayList*  sources  ,
               const char* lang     ,
               Meta*       meta     ,
               Data*       data     );

        /**
         * Sets noResults
         *
         * @param noResults the noResults value
         */
        void setNoResults(bool noResults);

        /**
         * Gets the Boolean value of noResults property
         *
         * @return noResults if boolean value is true, otherwise null
         */
        bool getNoResults();

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


END_NAMESPACE

/** @endcond */
#endif
