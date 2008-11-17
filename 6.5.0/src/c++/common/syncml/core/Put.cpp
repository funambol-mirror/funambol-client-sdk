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

#include "syncml/core/Put.h"

Put::Put() {
    lang = NULL;
    COMMAND_NAME = new char[strlen(PUT_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, PUT_COMMAND_NAME);
}

Put::~Put() {
    if (lang) {
        delete [] lang; lang = NULL;
    }
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}
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
Put::Put(CmdID* cmdID,
         BOOL noResp,
         char* lang,
         Cred* cred,
         Meta* meta,
         ArrayList* items ) : ItemizedCommand(cmdID, meta, items) {

    this->lang = NULL;

    setCred(cred);
    setNoResp(noResp);
    setLang(lang);

    COMMAND_NAME = new char[strlen(PUT_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, PUT_COMMAND_NAME);
}

// ----------------------------------------------------------- Public methods

/**
* Returns the preferred language
*
* @return the preferred language
*
*/
const char* Put::getLang() {
    return lang;
}

/**
* Sets the preferred language
*
* @param lang new preferred language
*/
void Put::setLang(const char*lang) {
    if (this->lang) {
        delete [] this->lang; this->lang = NULL;
    }
    this->lang = stringdup(lang);
}

/**
* Returns the command name
*
* @return the command name
*/
const char* Put::getName() {
    return COMMAND_NAME;
}

ArrayElement* Put::clone() {
    Put* ret = new Put(getCmdID(), getNoResp(), lang, getCred(), getMeta(), getItems());
    return ret;
}
