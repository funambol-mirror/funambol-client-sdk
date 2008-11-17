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


#include "syncml/core/Search.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Search::Search() {

    COMMAND_NAME = new char[strlen(SEARCH_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEARCH_COMMAND_NAME);

    noResults = false;
    target    = NULL;
    sources   = new ArrayList();
    lang      = NULL;
    data      = NULL;
}
Search::~Search() {
    if (COMMAND_NAME)   { delete [] COMMAND_NAME;   COMMAND_NAME = NULL; }
    if (target)         { delete    target;         target       = NULL; }
    if (sources)        { sources->clear(); } //delete sources; sources = NULL; }
    if (lang)           { delete [] lang;           lang         = NULL; }
    if (data)           { delete    data;           data         = NULL; }

}

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
*/
Search::Search(CmdID*      cmdID    ,
               bool        noResp   ,
               bool        noResults,
               Cred*       cred     ,
               Target*     target   ,
               ArrayList*  sources  ,
               char*    lang     ,
               Meta*       meta     ,
               Data*       data     ) : AbstractCommand(cmdID, noResp)  {


    COMMAND_NAME = new char[strlen(SEARCH_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEARCH_COMMAND_NAME);

    this->noResults = false;
    this->target    = NULL;
    this->sources   = new ArrayList();
    this->lang      = NULL;
    this->data      = NULL;

    setCred(cred);
    setMeta(meta);
    setSources(sources);
    setData(data);

    setNoResults(noResults);
    setTarget(target);
    setLang(lang);

}

/**
* Returns noResults
*
* @return noResults
*
*/
bool Search::isNoResults() {
    return (noResults != NULL);
}

/**
* Sets noResults
*
* @param noResults the noResults value
*/
void Search::setNoResults(bool noResults) {
    this->noResults = noResults;
}

/**
* Gets the Boolean value of noResults property
*
* @return noResults if boolean value is true, otherwise null
*/
bool Search::getNoResults() {
    return noResults;
}

/**
* Returns target property
* @return target the Target property
*/
Target* Search::getTarget() {
    return target;
}

/**
* Sets target property
*
* @param target the target property
*/
void Search::setTarget(Target* target) {
    if (this->target) {
        delete this->target ;
    }
    if (target) {
        this->target = target->clone();
    }
}

/**
* Returns command sources
* @return command sources
*/
ArrayList* Search::getSources() {
    return sources;
}

/**
* Sets command sources
*
* @param sources the command sources - NOT NULL
*
*/
void Search::setSources(ArrayList* sources) {
    if (sources == NULL) {
        // TBD
    } else {
        if (this->sources) {
		    this->sources->clear();
        }
	    this->sources = sources->clone();
    }
}

/**
* Returns the preferred language
*
* @return the preferred language
*
*/
const char* Search::getLang() {
    return lang;
}

/**
* Sets the preferred language
*
* @param lang the preferred language
*/
void Search::setLang(const char*lang) {
    if (this->lang) {
        delete [] this->lang; this->lang = NULL;
    }
    this->lang = stringdup(lang);
}

/**
* Returns data
*
* @return data
*
*/
Data* Search::getData() {
    return data;
}

/**
* Sets data
*
* @param data the command's data - NOT NULL
*
*/
void Search::setData(Data* data) {
    if (data == NULL) {
        // TBD
    } else {
        if (this->data) {
            delete this->data ; this->data = NULL;
        }
        this->data = data->clone();
    }
}

/**
* Returns the command name
*
* @return the command name
*/
const char* Search::getName() {
    return COMMAND_NAME;
}

ArrayElement* Search::clone() {
    Search* ret = new Search(getCmdID(), getNoResp(), noResults, getCred(), target, sources,
                             lang, getMeta(), data);
    return ret;
}

