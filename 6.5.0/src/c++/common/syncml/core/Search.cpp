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


#include "syncml/core/Search.h"

Search::Search() {

    COMMAND_NAME = new char[strlen(SEARCH_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEARCH_COMMAND_NAME);

    noResults = FALSE;
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
               BOOL        noResp   ,
               BOOL        noResults,
               Cred*       cred     ,
               Target*     target   ,
               ArrayList*  sources  ,
               char*    lang     ,
               Meta*       meta     ,
               Data*       data     ) : AbstractCommand(cmdID, noResp)  {


    COMMAND_NAME = new char[strlen(SEARCH_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEARCH_COMMAND_NAME);

    this->noResults = FALSE;
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
BOOL Search::isNoResults() {
    return (noResults != NULL);
}

/**
* Sets noResults
*
* @param noResults the noResults value
*/
void Search::setNoResults(BOOL noResults) {
     if ((noResults == NULL) || (noResults != TRUE && noResults != FALSE)) {
        this->noResults = NULL;
    } else {
        this->noResults = noResults;
    }
}

/**
* Gets the Boolean value of noResults property
*
* @return noResults if boolean value is true, otherwise null
*/
BOOL Search::getNoResults() {
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

